#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>   
#include <sys/stat.h>

#define send_data_tag 2001
#define return_data_tag 2002
#define d 256

int lenS = 0;
int lenP = 0;

void calculate_length(char *p, char *s) {
  while (*p != '\0') {
    p++;
    lenP++;
  }

  while (*s != '\0') {
    s++;
    lenS++;
  }

}

int search(char pat[], char txt[],int si,int q)
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
				printf("Pattern found at index %d \n",si+i);
			}
				
		}


		if ( i < N-M )
		{
			t = (d*(t - txt[i]*h) + txt[i+M])%q;
			
			if (t < 0)
			  t = (t + q);
		}
	}
  
	return c;
}

int main(int argc, char **argv) {
	
  FILE *fp;
  char c;
  char *temp;
  int q=101;
  int pfd[2],retval,pid;
  char pattern[] = "text";
  char *bigString;
  int start_index[20];
  struct stat st;
  
  fp = fopen("Test.txt", "r");
  stat("Test.txt", &st);
  
  long int sum, partial_sum;
  MPI_Status status;
  int my_id, root_process, i, num_rows, num_procs,an_id, num_rows_to_receive, avg_rows_per_process, 
      sender, num_rows_received, si, end_row, num_rows_to_send,si_str;
      
  bigString = (char*) malloc(st.st_size);
  char *trav = bigString;
  char *string1 = (char*) malloc(sizeof(char) * st.st_size);
  
  if (fp == NULL) {
    printf("error\n");
  } 
  
  else {
  	
    while ((c = fgetc(fp)) != EOF) {
       *trav = c;
        trav++;
    }
    
  }
  
  int avg_length = 0;
  calculate_length(pattern, bigString);
  
  MPI_Init(&argc, &argv);
  root_process = 0;
  
  MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  
  double start = MPI_Wtime();

  if(my_id == root_process) {

    avg_rows_per_process = lenS / num_procs;
    char *trav1 = bigString;
    end_row = avg_rows_per_process;
    
    while (trav1[end_row] != ' ') {
      end_row = end_row + 1;
    }
    
    start_index[0] = end_row;
    
    for(an_id = 1; an_id < num_procs; an_id++) {
    	
       si = start_index[an_id-1]+1;
       end_row   =  si + avg_rows_per_process;
       
	   if((lenS - end_row) < 0)
            end_row = lenS;
            
       else {
           while (trav1[end_row] != ' ') {
               end_row = end_row + 1;
           }
      }
      
      start_index[an_id] = end_row;
      num_rows_to_send = end_row - si + 1;
      
      MPI_Send( &num_rows_to_send, 1 , MPI_INT,an_id, send_data_tag, MPI_COMM_WORLD);
      MPI_Send( &si, 1 , MPI_INT, an_id, send_data_tag, MPI_COMM_WORLD);
      
    }

    char *temp_string = (char*) malloc(start_index[1]-1);        
    sum = 0;
    
    for(i = 0; i < (start_index[0]); i++) {
    	
      temp_string[i] = trav1[i];   
    } 
    
    sum = search(pattern, temp_string,0,q);
    temp_string[i+1]='\0';
    printf("count %lu calculated by root process\n", sum);
    


    for(an_id = 1; an_id < num_procs; an_id++) {
    	
      MPI_Recv( &partial_sum, 1, MPI_LONG, MPI_ANY_SOURCE,
      return_data_tag, MPI_COMM_WORLD, &status);
      sender = status.MPI_SOURCE;
      printf("Partial count %lu returned from process %i\n", partial_sum, sender);
      sum += partial_sum;
      
    }

     printf("The total count is: %lu\n", sum);
     
     double end = MPI_Wtime();
     printf("\nTime: %f sec",end-start);
  }

  else {

    MPI_Recv( &num_rows_to_receive, 1, MPI_INT, root_process, send_data_tag, MPI_COMM_WORLD, &status);
    MPI_Recv( &si_str, 1, MPI_INT, root_process, send_data_tag, MPI_COMM_WORLD, &status);
     
    char *temp = bigString;
    num_rows_received = num_rows_to_receive;

    char *temp_string = (char*) malloc(num_rows_received);
    partial_sum = 0;
    
    for(i = 0; i < num_rows_to_receive; i++) {
      temp_string[i] = temp[si_str + i];   
    } 
    
    temp_string[num_rows_to_receive] = '\0';
    partial_sum = search(pattern, temp_string,si_str,q);
    
    MPI_Send( &partial_sum, 1, MPI_LONG, root_process, return_data_tag, MPI_COMM_WORLD);
    
  }

  MPI_Finalize();
}
