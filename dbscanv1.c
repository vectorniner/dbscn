//Garrett Poppe
//DBSCAN Single node program
//Needs to connect to task manager.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int datagrabber(FILE *fptr, double *storage);
 
int main(int argc, char *argv[]){
        FILE *inPtr;    //pointer for file that is read from
        FILE *outPtr;   //pointer for file that is written to
        int check,i;
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
                        perror("File could not opened for appending:");
                        exit(1);
                }
	}
	check=datagrabber(inPtr,&memory[0]);
	if (check != tharg)
		printf("check size mismatch\n");

	printf("Enter EPSmin: ");
	scanf("%f",&EPSmin);
		
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
