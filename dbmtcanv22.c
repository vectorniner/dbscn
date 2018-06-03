//Garrett Poppe
//DBMTCAN Density Based Multi Threaded Clustering Algorithm with Noise
//Dominates on any multicore machine  ***coming soon***

/*Program notes:
need to run against dataset.
included a CHAMELEON set "newdata.txt"
./a.out newdata.txt ouptut 788 100 3
not sure about epsmin and points


Version 21
removed recursive functions and added loops for values larger than 2 digits
works.
./a.out infilec.txt outfile 14 1000 1

Version 19
complete working version
need to improve efficiency and remove duplicates
./a.out infileb.txt outfile 14 2 1


Version 16.0
working on merge function.

version 15
no bugs, merge function not finished, format functions complete.

run with: 
"./a.out infile.txt outfile 11 epsmin mnpnts"

compile with: 
"gcc dbmtcanv21.c -lm -lpthread"


*/


#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <string.h>
#include <error.h>
#include <ctype.h>
#include <semaphore.h>
#define PATH_MAX 25
#define sSize 40
#include <time.h>

//Variables for thread
struct job{
//        FILE *fileptr;
	char *filename;
        double *jobstorage;
	double *initmem;
        double EpsMin;
	int MinPts;
        int elements;
};

sem_t holdtrue; 
pthread_mutex_t lock;

int datagrabber(FILE *fptr, double *storage);
void* scann(void *);
void mrgCluster(char fileNames[][100],int numOfiles);
void filenames(char cluster[],double *values);
void appendFile(char fileA[100], char fileB[100]);
int printTime(void);
 
int main(int argc, char *argv[]){
        FILE *inPtr;    //pointer for file that is read from
        FILE *outPtr;   //pointer for file that is written to
        int check,i,G;
	int tharg,tharg2;
	int mnpts = atoi(argv[5]);
	tharg= atoi(argv[3]);
	int timer=0;
	double *memory, *initMem;
	double EPSmin= (double)(atoi(argv[4]));
	tharg2= 2*tharg;
	char str1[20];
	strcpy(str1,argv[2]);
	char str2[20];
	pthread_t tid[tharg];
	timer=printTime();
		
	if(sem_init(&holdtrue,1,1)<0)
	{
		perror("semaphore initialization");
		exit(0);
	}

	int iret1, iret2;
	if (pthread_mutex_init(&lock, NULL) != 0)
	{
        	printf("\n mutex init failed\n");
        	return 1;
	}


	char names[tharg][100];
	for(i = 0; i < tharg; ++i )
    	{	
//		char *string;
//                asprintf(&string,"%d",i);
//                printf("%s\n", string);	//debugging
//		strcat(string,str1);
		strcpy(names[i],str1);
//		printf("%s\n",names[i]);	//debugging
//		free (string);
	}
/*
	for (i=0;i<tharg;i++)
        {
        	printf("names[%d]= %s\n",i,names[i]);
        }
*/
	
	memory = calloc(tharg2, sizeof(double)+1);
	initMem = &memory[0];
        
	if(argc!=6)
	{ //4 arguments check
		puts("Usage: ./a.out infile outfile size epsmin mnpnts");
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
                        perror("File could not be opened for writing:");
                        exit(1);
                }
	}
	
	check = datagrabber(inPtr,memory);
	//check memory values
	for(i=0;i<tharg2;i++)
	{
		printf("Mem= %.02lf\n",*(memory+i));
	}
	
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

	printf("Points in dataset = %d \n",check);
	printf("Points specified = %d \n",tharg);
/*
	printf("Enter EPSmin: ");
	scanf("%lf",&EPSmin);
	//printf("memory[0]= %lf\n",*memory); //error check
	
	printf("Enter MinPoints: ");
        scanf("%d",&mnpts);
*/
	struct job *jobptr = malloc(sizeof *jobptr);

	
	for(i=0;i< tharg;i++)
	{
		
		sem_wait(&holdtrue);	
		if (jobptr != NULL)
		{
			puts("starting thread");
			printf("i= %d threadname= %s\n",i,names[i]);
			jobptr->filename = names[i];
	        	jobptr->jobstorage = (memory+(2*i));
	        	jobptr->initmem = initMem;
			jobptr->EpsMin = EPSmin;
	        	jobptr->elements = 2*(tharg);
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
	mrgCluster(names,tharg);	
        fclose(outPtr); //closes write file
	fclose(inPtr);
	sem_destroy(&holdtrue);
	free (memory);
	free (jobptr);
	pthread_mutex_destroy(&lock);
	timer = printTime() - timer;
	printf("timer= %d\n",timer);
	return 0;
}


int datagrabber(FILE *fptr, double *storage)
{
	int i=0,j=0;
	double temp=0;
	char c='0';
        c=fgetc(fptr);
	while( c != EOF){
                if(c=='\n')
		{
                        i++;
			j++;
			c=fgetc(fptr);
		}
		else if(c==',')
                {
		        j++;
			c=fgetc(fptr);
                }
		else
		{	//need to create recursion here xx.xx
			temp = (double)c - '0';
			c=fgetc(fptr);
			if(c=='\n')
	                {
        	                //*(storage+j)= temp;
				i++;
                	        j++;
				c=fgetc(fptr);
			}
                	else if(c==',')
			{
                        	*(storage+j)= temp;
				j++;
				temp=0;
				c=fgetc(fptr);
                	}
			else if(isdigit(c))
			{
				while(isdigit(c))
				{
					temp = (temp*10) + (double)c - '0';
					c=fgetc(fptr);
				}
/*				if(c=='.')
				{
					c=fgetc(fptr);
					temp = ((double)c - '0')/10;
					int u=10;
					while(isdigit(c))
					{
						temp = temp + ((double)c - '0')/(10*u);
						u*=10;
						c=fgetc(fptr);
					}
				}
*/
//				fseek(fptr, -1, SEEK_CUR);

				*(storage+j)= temp;
				
			}
//			printf("storage = %lf\n",*(storage+j)); //error check
		}
        }
	return i;
}

void* scann(void *jobs)
{
	puts("thread going");
	double x,y,x2,y2,distance,z, tempy, tempx, *temptr;
        int i,pointcount=0;

	pthread_mutex_lock(&lock);

	struct job *jobptr2 = jobs;	
	
//	FILE *fiptr=jobptr2->fileptr;
	char *fname=jobptr2->filename;
	double *storage2=jobptr2->jobstorage;
	double *Initmem=jobptr2->initmem;
	double epsmin=jobptr2->EpsMin;
	int mnpoints=jobptr2->MinPts;
	int sizes=jobptr2->elements;
	
	sem_post(&holdtrue);	
	pthread_mutex_unlock(&lock);	
	
	x=*storage2;
//        printf("%.02f\n",x);
        y=*(storage2+1);
//        printf("%.02f\n",y);

        char *stringx;
        char *stringy;
        char tempchar='_';
        asprintf(&stringx, "%0.0lf", x);
        asprintf(&stringy, "%0.0lf", y);
        char oldname[PATH_MAX];

        strcpy(oldname,stringx);
	strcat(oldname,"_");
        strcat(oldname,stringy);
        strcat(oldname,fname);
//        printf("%s\n", oldname);

	FILE *fiptr = fopen(oldname,"w");
        if(!fiptr)
        { //if error appending
        	perror("File could not open for writing:");
                exit(1);
        }


//	printf("storage2= %.02lf\n",*storage2);		//debugging

	storage2 = Initmem; 
        for(i=0;i < (sizes);(i+=2))
        {
		x2=*(storage2+i);
//                printf("%.02f\n",x2);		//debugging
		y2=*(storage2+(i+1));
//                printf("%.02f\n",y2);		//debugging
		
		tempy= abs(y2-y);
		tempx= abs(x2-x);
                z= tempy*tempy+tempx*tempx;
//		printf("%.02f\n",z);		//debugging
                distance = sqrt(z);
//                printf("%.02f\n",distance);		//debugging
//                printf("%.02f\n",epsmin);		//debugging
//                printf("i= %d size= %d current point=(%.02lf,%.02lf)\n",i,sizes,x,y);
		if(distance < epsmin)
                {
                        fprintf(fiptr,"x(%0.0lf),y(%0.0lf) -> x_(%0.0lf),y_(%0.0lf) = %lf\n",x,y,x2,y2,distance);
//                        printf("x(%.02lf),y(%.02lf) -> x_(%.02lf),y_(%.02lf) = %.03lf\n",x,y,x2,y2,distance);
                	pointcount++;
		}
        }
	
	
//function to add number of points to the end of the filename.
	int ret;
	char *stringb;
	char *stringSep;
	char newname[PATH_MAX];
//        printf("mnpts= %d currentPts= %d\n",mnpoints,pointcount);
	asprintf(&stringb, "%d", pointcount);
//        printf("%s\n", stringb);
	strcpy(newname,oldname);
	asprintf(&stringSep,"_");
	strcat(newname, stringSep);
	strcat(newname, stringb);
//	printf("%s\n", newname);
	ret = rename(oldname, newname);
	if(ret == 0) 
	{
		printf("File renamed successfully\n");
	}
   	else 
   	{
     		printf("\nError: unable to rename the file\n");
   	}
	strcpy(fname, newname);
	free (stringb);
	free (stringx);
	free (stringy);

	fclose(fiptr);
}


void mrgCluster(char fileNames[][100],int numOfiles)
{
	puts("Merge started...\n");
	FILE *master;
	FILE *slave;
	int i,h,j,hnd=0;
	double p1,p2;
	double handoff[3]={0};
	char c;
	double k,l,clstptsA,tempk, templ, clstptsB;
	char clusterc[100];
	char clusterb[100];
//file format d_dfilename_dd
		
	for(h=0;h<numOfiles;h)
	{
		strcpy(clusterb,fileNames[h++]);
		filenames(clusterb,handoff);
		k = handoff[0];
		l = handoff[1];
		clstptsA = handoff[2];	
		//printf("This %s\n",clusterb);
		master = fopen(clusterb,"r");
                if(master)
                { 
			while((c=fgetc(master)) != EOF)
			{
                		//printf("start = %c\n",c);
				if(c=='\n')
                        	{
                	        	p1 = 0;
                        		p2 = 0;
                        	}
                        	else if(c=='x')
                        	{
                        		//printf("step 1 = %c\n",c);
					c=fgetc(master);
                                	if (c == '_')
                                	{
                                		c=fgetc(master);
						c = fgetc(master);
                                        	//printf("step 2 = %c\n",c);
						p1 = (double)c - '0';
                                        	c=fgetc(master);
                                        	if (isdigit(c))
                                        	{
                                			while(isdigit(c))
                                			{
                                        			p1 = (p1*10) + ((double)c - '0');
                                        			c=fgetc(master);
                                			}
                                			fseek(master, -1, SEEK_CUR);

                                        	}
                                                //need recursion to go higher than double digits
                                	}
				}
                        	else if(c=='y')
                        	{
                        		c=fgetc(master);
                                	//printf("step 3 = %c\n",c);
					if (c == '_')
                                	{
                                		c = fgetc(master);
                                        	c = fgetc(master);
						//printf("step 4 = %c\n",c);
						p2 = (double)c - '0';
                                        	c=fgetc(master);
                                        	if (isdigit(c))
                                        	{
                                        		while(isdigit(c))
                                                        {
                                                                p2 = (p2*10) + ((double)c - '0');
                                                                c=fgetc(master);
                                                        }
                                                        fseek(master, -1, SEEK_CUR);
                                        	}
                                                //need recursion to go higher than double digits
					
//						printf("p1=%f p2=%f\n",p1,p2);
						for(j=h;j<=(numOfiles-h);j++)
                				{
                        				strcpy(clusterc,fileNames[j]);
//                        				printf("%s\n",clusterc);
                        				//retrieve dd_ddcluster and # of points in cluster
                        				filenames(clusterc,handoff);
                        				tempk = handoff[0];
                        				templ = handoff[1];
                        				clstptsB = handoff[2];

                        				//printf("tempk= %.2lf\n",tempk);
                        				//printf("templ= %.2lf\n",templ);
                        				//printf("clstpts= %.2lf\n",clstptsB);

                                			if((p1 == tempk) && (p2 == templ))
                                			{
                                				puts("merge request");
								appendFile(clusterc,clusterb);
								unlink(clusterc);
                                			}
							tempk=0;
                                        		templ=0;
                                        		clstptsB=0;
						}
					}

				}
                        	else
                        	{
                        		//printf("step else = %c\n",c);
                        	}
			}
			fclose(master);		
		}
	}
}

void filenames(char cluster[],double *values)
{
	int j=0,b=0;
	double templ=0, tempk=0, clstpts=0;
	char c;
	while( (c= cluster[j]) != '\0')
                        {
                                if(c=='_')
                                {
                                        b++;
                                        j++;
//                                      printf("j=_ %d\n",j);           //debugging
                                }
                                else if(isalpha(c))
                                {
                                        j++;
//                                      printf("j= %d\n",j);            //debugging
                                }
                                else if(isdigit(c))
                                {
                                        if(b==0)
                                        {
                                                tempk= (double)c - '0';
                                                //need recursion here xx.xx
                                                while ((c= cluster[++j]) != '\0')
                                                {
                                                        if(c=='_')
                                                        {
                                                                b++;
                                                                j++;
//                                                              printf("j=__ %d b= %d\n",j,b);          //debugging
                                                                break;
                                                        }
                                                        else if(isalpha(c))
                                                        {
                                                                puts("wrong file format-> dd_ddfilename_dd");
//                                                              printf("b= %d\n",b);            //debugging
                                                                break;
                                                        }
                                                        else if(isdigit(c))
                                                        {
                                                                while(isdigit(c))
                                                        	{
                                                                	tempk = (tempk*10) + ((double)c - '0');
                                                                	c = cluster[++j];
                                                        	}
								j--;                                                        	
                                                        }
                                                        else
                                                                break;
                                                }

                                        }
                                        else if(b==1)
                                        {
						templ= (double)c - '0';
//                                              printf("templ= %.2lf\n",templ);         //debugging
                                                //need recursion here xx.xx
                                                while ((c= cluster[++j]) != '\0')
                                                {
                                                        if(isalpha(c))
                                                        {
                                                                break;
                                                        }
                                                        else if(isdigit(c))
                                                        {
                                                                while(isdigit(c))
                                                                {
                                                                        templ = (templ*10) + ((double)c - '0');
                                                                        c = cluster[++j];
                                                                }
                                                                j--;
                                                        }
                                                        else
                                                                break;
                                                }
                                        }
                                        else if(b==2)
                                        {
                                                clstpts= (double)c - '0';
//                                              printf("templ= %.2lf\n",templ);         //debugging
                                                //need recursion here xx.xx
                                                while ((c= cluster[++j]) != '\0')
                                                {
                                                        if(isalpha(c))
                                                        {
                                                                puts("wrong file format-> dd_ddfilename_dd");
                                                                printf("j= %d b= %d\n",j,b);
                                                                break;
                                                        }
                                                        else if(isdigit(c))
                                                        {
                                                                while(isdigit(c))
                                                                {
                                                                        clstpts = (clstpts*10) + ((double)c - '0');
                                                                        c = cluster[++j];
                                                                }
                                                                j--;
                                                        }
                                                        else
                                                                break;
                                                }

                                        }
                                        else
                                        {
//                                               puts("");
//                                                printf("j= %d b= %d\n",j,b);
						         j++;
                                        }
                                }
//                              printf("# of Points= %0.2lf\n",templ);          //debugging
                        }
			*values = tempk;
			values++;
			*values = templ;
			values++;
			*values = clstpts;
}

void appendFile(char fileA[100], char fileB[100])
{
//	printf("Starting system call\n");	
	char systemCall[255];
	char commands[10]="cat ";
	strcpy(systemCall,commands);
	strcat(systemCall,fileA);
	strcpy(commands," >> ");
	strcat(systemCall,commands);
	strcat(systemCall,fileB);
//	printf("%s\n",systemCall);
	system(systemCall);
}

int printTime(void)
{
  time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  printf ( "Current local time and date: %s", asctime (timeinfo) );

  return rawtime;
}
