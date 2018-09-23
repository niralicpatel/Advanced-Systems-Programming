#include <sys/types.h>
#include "tlpi_hdr.h"
#include<unistd.h>
#include<stdio.h>

int main(int argc, char *argv[])
{
    int pfd[2];                                     
    (pipe(pfd) == -1) ;                           
        

    switch (fork()) 
	{
 
    case 0:             
        	(close(pfd[0]));                    

        	if (pfd[1] != STDOUT_FILENO) 
		{             
	            	(dup2(pfd[1], STDOUT_FILENO));
	             	(close(pfd[1]));
       		 }

        	execvp(argv[1], argv);          

    default:            
        	break;
   	 }

    switch (fork()) 
	{
   
    case 0:             
       	  	close(pfd[1]);

        	if (pfd[0] != STDIN_FILENO)
		 {               
            		(dup2(pfd[0], STDIN_FILENO));
              
            		(close(pfd[0]));
              
      		  }

        	execvp(argv[2],argv);

    default: 
        break;
    }

   close(pfd[0]);
   close(pfd[1]);
    exit(EXIT_SUCCESS);
}
