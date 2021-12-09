#include<stdio.h>
#include<string.h>
#include <unistd.h>   
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#define d 256

void search(char pat[], char txt[], int q)
{
	int M = strlen(pat);
	int N = strlen(txt);
	int i, j;
	int p = 0; 
	int t = 0; 
	int h = 1;
	int c=0;

	for (i = 0; i < M-1; i++)
		h = (h*d)%q;


	for (i = 0; i < M; i++)
	{
		p = (d*p + pat[i])%q;
		t = (d*t + txt[i])%q;
	}


	for (i = 0; i <= N - M; i++)
	{


		if ( p == t )
		{
			for (j = 0; j < M; j++)
			{
				if (txt[i+j] != pat[j])
					break;
			}

			if (j == M){
				c=c+1;
				printf("Pattern found at index %d \n", i);
			}
		}


		if ( i < N-M )
		{
			t = (d*(t - txt[i]*h) + txt[i+M])%q;

			if (t < 0)
			t = (t + q);
		}
	}
	printf("Total count: %lu\n",c);
}


int main()
{
  FILE *fp;
  char c;
  char *temp;
  char *bigString;
  struct stat st;
  fp = fopen("Test.txt", "r");
  stat("Test.txt", &st);
  
  long int count;
  
  bigString = (char*) malloc(st.st_size);
  char *trav = bigString;
  
  if (fp == NULL) {
    printf("error\n");
  } 
  
  else {
  	
    while ((c = fgetc(fp)) != EOF) {
       *trav = c;
        trav++;
    }
    
  }
	int q = 101;
    char pat[] = "text";
	clock_t start, end;
    double cpu_time_used;
	start = clock();
	search(pat, bigString, q);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Time: %f",cpu_time_used);
	return 0;
}

