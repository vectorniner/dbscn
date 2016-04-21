//Garrett Poppe
//DBMTCAN Density Based Multi Threaded Clustering Algorithm with Noise
//Dominates on any multicore machine

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

int datagrabber(FILE *fptr, double *storage);
void scann(FILE *fiptr, double *storage2, int epsmin,int sizes);

 
int main(int argc, char *argv[]){
        FILE *inPtr;    //pointer for file that is read from
        FILE *outPtr;   //pointer for file that is written to
        int check,i,G;
	int tharg;
	tharg= atoi(argv[3]);

	double EPSmin=0, memory[tharg];
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
	scanf("%f",&EPSmin);
	printf("memory[1]= %lf\n",memory[1]);
	scann(outPtr,&memory[0],EPSmin,check);		

	
/*
	double x,y,x2,y2,distance,z;
	x=memory[0];
	printf("%.02f\n",x);
	y=memory[1];
	printf("%.02f\n",y);


	for(i=2;i < check;i++)
	{
		x2=memory[i];
                printf("%.02f\n",x2);
                y2=memory[i+1];
                printf("%.02f\n",y2);

		z= y*y2+x*x2;
		printf("%.02f\n",z);
		distance = sqrt(z);
		printf("%.02f\n",distance);
		printf("%.02f\n",EPSmin);
		if(distance< EPSmin)
		{
			fprintf(outPtr,"x(%lf),y(%lf) -> x'(%lf),y'(%lf) = %lf\n",x,y,x2,y2,distance);
			printf("x(%.02lf),y(%.02lf) -> x'(%.02lf),y'(%.02lf) = %.03lf\n",x,y,x2,y2,distance); 		
		}		
	}
*/
        fclose(inPtr);  //closes read file
        fclose(outPtr); //closes write file

        return 0;
}


int datagrabber(FILE *fptr, double *storage)
{
	int i=0,j=0;
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
			*(storage+j)= (double)c - '0';
        }
	return i;
}

/*
void threadtest(void)
{
int pthread_create(pthread_t * pth, pthread_attr_t *att, void * (*function), void * arg);
}
*/
void scann(FILE *fiptr, double *storage2, int epsmin, int sizes)
{
	printf("storage2= %.02lf\n",*storage2);
	double x,y,x2,y2,distance,z;
        int i;
	x=*storage2;
        printf("%.02f\n",x);
        y=*(storage2+1);
        printf("%.02f\n",y);

        for(i=2;i < sizes;i++)
        {
                x2=storage2[i];
                printf("%.02f\n",x2);
                y2=storage2[i+1];
                printf("%.02f\n",y2);

                z= y*y2+x*x2;
                printf("%.02f\n",z);
                distance = sqrt(z);
                printf("%.02f\n",distance);
                printf("%.02f\n",epsmin);
                if(distance< epsmin)
                {
                        fprintf(fiptr,"x(%lf),y(%lf) -> x'(%lf),y'(%lf) = %lf\n",x,y,x2,y2,distance);
                        printf("x(%.02lf),y(%.02lf) -> x'(%.02lf),y'(%.02lf) = %.03lf\n",x,y,x2,y2,distance);
                }
        }
	fclose(fiptr); //closes write file
}	
