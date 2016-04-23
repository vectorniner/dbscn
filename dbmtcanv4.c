//Garrett Poppe
//DBMTCAN Density Based Multi Threaded Clustering Algorithm with Noise
//Dominates on any multicore machine

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

struct job{
        FILE *fileptr;
        double *jobstorage;
        double EpsMin;
        int elements;
};



int datagrabber(FILE *fptr, double *storage);
void* scann(void *);

 
int main(int argc, char *argv[]){
        FILE *inPtr;    //pointer for file that is read from
        FILE *outPtr;   //pointer for file that is written to
        int check,i,G;
	int tharg,tharg2;
	tharg= atoi(argv[3]);
	double *memory;
	double EPSmin=0;
	tharg2= 2*tharg;
	
	pthread_t tid[1];
	int iret1, iret2;
/*	
    for (int i = 0; i < 2; i++) {
        pthread_create(&tid[i], NULL, routine, NULL);
    }
    for (int i = 0; i < 2; i++)
       pthread_join(tid[i], NULL);
*/
	memory = calloc(tharg2, sizeof(double));
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
	//scann(outPtr,memory,EPSmin,(check*2));		
/*	
	struct job{
        FILE *fileptr;
        double *jobstorage;
        double EpsMin;
        int elements;
	};
*/
	
	struct job *jobptr = malloc(sizeof *jobptr);

	if (jobptr != NULL)
	{
		puts("starting thread");
		jobptr->fileptr = inPtr;
	        jobptr->jobstorage = memory;
	        jobptr->EpsMin = EPSmin;
	        jobptr->elements = tharg2;
		iret1 = pthread_create( &tid[0], NULL, scann, jobptr);
        	if(iret1)
        	{
                	fprintf(stderr, "Error - pthread_create() return code: %d\n",iret1);
                	exit(EXIT_FAILURE);
        	}
	}

	pthread_join(tid[0], NULL);
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
		{	//need to create recursion here.
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

	FILE *fiptr=jobptr2->fileptr;
	double *storage2=jobptr2->jobstorage;
	double epsmin=jobptr2->EpsMin;
	int sizes=jobptr2->elements;
	
	printf("storage2= %.02lf\n",*storage2);
	double x,y,x2,y2,distance,z, tempy, tempx;
        int i;
	x=*storage2;
        printf("%.02f\n",x);
        y=*(storage2+1);
        printf("%.02f\n",y);
	
        for(i=2;i <= (sizes-2);i++)
        {
                x2=*(storage2+i);
                printf("%.02f\n",x2);
		y2=*(storage2+(i+1));
                printf("%.02f\n",y2);
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
                }
        }
	
	free(storage2);
	free(jobs);	
	fclose(fiptr); //closes write file
}	
