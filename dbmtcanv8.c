//Garrett Poppe
//DBMTCAN Density Based Multi Threaded Clustering Algorithm with Noise
//Dominates on any multicore machine

/*Program notes:
runs when using gdb except the number of files created is one less than normal.
need to add abs value for distances.
change file naming scheme and use arg 2.
*/


#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <string.h>
#include <error.h>
#include <ctype.h>
#define PATH_MAX 25
#define sSize 40


//Variables for thread
struct job{
//        FILE *fileptr;
	char *filename;
        double *jobstorage;
        double EpsMin;
	int MinPts;
        int elements;
};


pthread_mutex_t lock;
int datagrabber(FILE *fptr, double *storage);
void* scann(void *);

 
int main(int argc, char *argv[]){
        FILE *inPtr;    //pointer for file that is read from
        FILE *outPtr;   //pointer for file that is written to
        int check,i,G;
	int tharg,tharg2,mnpts;
	tharg= atoi(argv[3]);
	double *memory;
	double EPSmin=0;
	tharg2= 2*tharg;
	char str1[20]="outfile";
	char str2[20];
	pthread_t tid[tharg];

	int iret1, iret2;
	
	char names[tharg][100];
	for(i = 0; i < tharg; ++i )
    	{	
		char *string;
                asprintf(&string,"%d",i);
                printf("%s\n", string);
		strcat(string,str1);
		strcpy(names[i],string);
		printf("%s\n",names[i]);
		free (string);
	}

	for (i=0;i<tharg;i++)
        {
        	printf("names[%d]= %s\n",i,names[i]);
        }

	
	memory = calloc(tharg2, sizeof(double)+1);
	printf("arg3= %d\n",tharg);
        if(argc!=4)
	{ //4 arguments check
		puts("Usage: infile outfile size");
        }
	else
	{ //check if files exist
                inPtr = fopen(argv[1],"r");
                if(!inPtr)
		{ //if no file
                        perror("File could not be opened for reading:");
                        exit(1);
        	}

                outPtr = fopen(argv[2],"w+");
                if(!outPtr)
		{ //if error appending
                        perror("File could not opened for writing:");
                        exit(1);
                }
	}
	check = datagrabber(inPtr,memory);
	if (check == tharg)
		printf("check size match\n");
	else if(check > tharg)
		printf("check > tharg\n");
	else if(check < tharg)
	{
		G= tharg - check;	
		printf("check < tharg G= %d\n",G);
	}
	else
		printf("The sky is falling\n");

	printf("check = %d \n",check);
	printf("tharg = %d \n",tharg);

	printf("Enter EPSmin: ");
	scanf("%lf",&EPSmin);
	//printf("memory[0]= %lf\n",*memory); //error check
	
	printf("Enter MinPoints: ");
        scanf("%d",&mnpts);
	
	struct job *jobptr = malloc(sizeof *jobptr);
	for(i=0;i< tharg;i++)
	{

		if (jobptr != NULL)
		{
			puts("starting thread");
			printf("i= %d threadname= %s\n",i,names[i]);
			jobptr->filename = names[i];
	        	jobptr->jobstorage = (memory+(2*i));
	        	jobptr->EpsMin = EPSmin;
	        	jobptr->elements = 2*(tharg-i);
			jobptr->MinPts = mnpts;
			iret1 = pthread_create( &tid[i], NULL, scann, jobptr);
        		if(iret1)
        		{
                		fprintf(stderr, "Error - pthread_create() return code: %d\n",iret1);
                		exit(EXIT_FAILURE);
        		}
		}
	}
	int h;
	for(h=0;h<tharg;h++)
	{
		pthread_join(tid[h], NULL);
	}
	puts("scan complete");	

        fclose(outPtr); //closes write file
	fclose(inPtr);
	free (memory);
	free (jobptr);
	return 0;
}


int datagrabber(FILE *fptr, double *storage)
{
	int i=0,j=0;
	double temp=0;
	char c;
        while( (c=fgetc(fptr)) != EOF){
                if(c=='\n')
		{
                        i++;
			j++;
		}
		else if(c==',')
                        j++;
                else
		{	//need to create recursion here xx.xx
			temp = (double)c - '0';
			c=fgetc(fptr);
			if(c=='\n')
	                {
        	                *(storage+j)= temp;
				i++;
                	        j++;
			}
                	else if(c==',')
			{
                        	*(storage+j)= temp;
				j++;
                	}
			else
			{
				temp = (temp*10) + (double)c - '0';
				*(storage+j)= temp;
			}
			//printf("storage = %lf\n",*(storage+j)); //error check
		}
        }
	return i;
}

void* scann(void *jobs)
{
	puts("thread going");
	struct job *jobptr2 = jobs;	
	
	pthread_mutex_lock(&lock);
//	FILE *fiptr=jobptr2->fileptr;
	char *fname=jobptr2->filename;
	double *storage2=jobptr2->jobstorage;
	double epsmin=jobptr2->EpsMin;
	int mnpoints=jobptr2->MinPts;
	int sizes=jobptr2->elements;
	pthread_mutex_unlock(&lock);
	
	printf("File name = %s\n",fname);	


	FILE *fiptr = fopen(fname,"w");
        if(!fiptr)
        { //if error appending
        	perror("File could not open for writing:");
                exit(1);
        }


//	printf("storage2= %.02lf\n",*storage2);
	double x,y,x2,y2,distance,z, tempy, tempx;
        int i;
	pthread_mutex_lock(&lock);	

	x=*storage2;
//        printf("%.02f\n",x);
        y=*(storage2+1);
//        printf("%.02f\n",y);
	pthread_mutex_unlock(&lock);
	
        for(i=2;i <= (sizes-2);i++)
        {
                pthread_mutex_lock(&lock);
		x2=*(storage2+i);
//                printf("%.02f\n",x2);
		y2=*(storage2+(i+1));
//                printf("%.02f\n",y2);
		pthread_mutex_unlock(&lock);
		
		tempy= (y2-y);
		tempx= (x2-x);
                z= tempy*tempy+tempx*tempx;
                printf("%.02f\n",z);
                distance = sqrt(z);
                printf("%.02f\n",distance);
                printf("%.02f\n",epsmin);
                printf("i= %d size= %d\n",i,sizes);
		if(distance < epsmin)
                {
                        fprintf(fiptr,"x(%lf),y(%lf) -> x'(%lf),y'(%lf) = %lf\n",x,y,x2,y2,distance);
                        printf("x(%.02lf),y(%.02lf) -> x'(%.02lf),y'(%.02lf) = %.03lf\n",x,y,x2,y2,distance);
                	mnpoints++;
		}
        }
	
/*	
//function to add number of points to the end of the filename.
	int ret;
	char *stringb;
        asprintf(&stringb, "%d", mnpoints);
        printf("%s\n", stringb);
        char oldname[PATH_MAX];
	strcpy(oldname,fname);
	strcat(fname, stringb);
	printf("%s\n", fname);
	ret = rename(oldname, fname);
	if(ret == 0) 
	{
		printf("File renamed successfully");
	}
   	else 
   	{
     		printf("Error: unable to rename the file");
   	}
*/
//char newname[PATH_MAX];
//snprintf(newname, PATH_MAX, "file_%d.txt", min);
//FILE * f = fopen(fname, "w");
//end of filname minpoints function
//	free(storage2);
//        free(jobs);
//        free(fname);
//        fclose(fiptr); //closes write file

}

/*
void mrgCluster(char *fileNames[][],int numOfiles)
{
		int i,j,b=0,clstpts;
		char c;
		double k,l,tempk,templ;
		char cluster[100]= '\0';
		char clusterb[100]= '\0';
		for(i=0;i<numOfiles;i++)
		{
			cluster=*fileNames[i];
			//need to add number of points in file cluster
			//will be the number of times to run next loop.
				
			while( (c= cluster[j]) != '\0')
			{
        	        	if(c=='_')
                		{
                        		b=1;
                        		j++;
                		}
                		else if(isalpha(c))
                        		break;
                		else if(isdigit(c))
				{
					if(b==0)
					{
						tempk= (double)c - '0';
						//need recursion here xx.xx
						while (c= cluster[++j]) != '\0')
						{
							if(c=='_')
			                        	{
                        			        	b=1;
                                				break;
                        				}
                        				else if(isalpha(c))
							{
                                				b==3;
								while (c= cluster[++j]) != '\0')
                                                		{
                                                        		if(isalpha(c))
                                                                		break;
                                                        		else if(isdigit(c))
                                                        		}
										clstpts=((double)c - '0');
                                                        		}
                                                        		else
                                                                		break;
                                                		}

								break;
							}
							else if(isdigit(c))
							{
								tempk= tempk*10;
								tempk= tempk + ((double)c - '0');
							}
							else
								break;
						}
					}
					else if(b==1)
					{
						templ= (double)c - '0';
						//need recursion here xx.xx
					 	while (c= cluster[++j]) != '\0')
                                        	{
                                                	if(isalpha(c))
							{
								b==3;
                                                                while (c= cluster[++j]) != '\0')
                                                                {
                                                                        if(isalpha(c))
                                                                                break;
                                                                        else if(isdigit(c))
                                                                        }
                                                                                clstpts=((double)c - '0');
                                                                        }
                                                                        else
                                                                                break;
                                                                }

                                                        	break;
							}
                                                	else if(isdigit(c))
                                                	{
                                                        	templ= templ*10;
                                                        	templ= templ + ((double)c - '0');
                                                	}
                                                	else
                                                        	break;
                                        	}
					}
					
			}
			//need to go to next file and compare name to see
			//if point is in file name.
		}
	}
}
*/
