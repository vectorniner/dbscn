//Garrett Poppe
//converts xx.xx to xxxx for dbmtcanv

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void main(void)
{
	FILE *inptr, *outptr;
	char letters[100];
        double number=0,a=0,b=0;
        int decimal_found = 0,c=0,d=0;

        inptr=fopen("data.txt","r");
	outptr=fopen("newdata.txt","w");

	while(!feof(inptr))
	{
		fscanf(inptr,"%lf%lf",&a,&b);
		c = (int)(100*a);
		d = (int)(100*b);
		fprintf(outptr,"%d,%d\n",c,d);
	}
}
	
	
