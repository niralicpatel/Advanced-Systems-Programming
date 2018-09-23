#include<stdlib.h>
#include<string.h>
#include<semaphore.h>
#include<stdio.h>
#include <sys/mman.h>

struct tuples
	{
		char userID[5];
		int action;
		char topic[15];
	};
struct tuples **buffer1;
int action_value(char ch);
int buffer_size,nthreads;
int *input,*output,*cnt;
int done[100];
int count;
sem_t s[50],full[50],empty[50],print[50];


int main(int argc,char *argv[])
	{	
		int i;
		buffer_size = atoi(argv[1]);
		nthreads = atoi(argv[2]);
		
		if(buffer_size<=nthreads)
		{
			buffer_size = buffer_size+nthreads;
			
		}
		
		buffer1 = (struct tuples **) mmap (NULL, sizeof(struct tuples *) *nthreads * buffer_size, PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED,-1,0);
		*buffer1 = (struct tuples *) mmap (NULL, sizeof(struct tuples) *nthreads * buffer_size, PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED,-1,0);
		for(i=0;i < nthreads;i++)
			{
        		sem_init (&s[i],1,1);
        		sem_init(&full[i],1,0);
				sem_init(&empty[i],1,0);
				sem_init(&print[i],1,0);
			}
		//buffer1=(struct tuples **) malloc (sizeof (struct tuples *) * nthreads);
		//*buffer1 = (struct tuples *) malloc (sizeof(struct tuples) * nthreads * buffer_size);
		for(i=0;i < nthreads;i++)
			{
        		buffer1[i] = (*buffer1 + buffer_size * i);
			}
		
		input=(int *)malloc(nthreads * sizeof(int));
		output=(int *)malloc(nthreads * sizeof(int));
		cnt=(int *)malloc(nthreads * sizeof(int));
		
		for(i=0;i<nthreads;i++)
			{
				input[i]=0;
				output[i]=0;
				cnt[i]=0;
			}	
				printf("Mapper created \n");
				char *fp1,*fp2;
			    char *buffer,value,topic[15],c;
			    int action,counter,temp=0;
			    int j,len,m=0,n;
				i=0;
				buffer = (char *)malloc(3000);
			    while (fgets(buffer,3000,stdin) != NULL)
			    {
			    	//printf("%s\n",buffer);
			    }
			    while (buffer[i] != '\0')
			    {
			        if (buffer[i] == ')')
			        {
			            count++;
			        }
			        
			        i++;
			    }

			    char *array[count];
				char *array1[count];
				struct tuples tuple[count];
				i=0;
			    fp1 = strtok(&buffer[1],",");       
				strcpy(tuple[i].userID,fp1);
				while (fp1 != NULL)
			    {
			        fp1 = strtok(NULL,",");
			        value = *fp1;
			        action =(action_value(value));
			        tuple[i].action = (action);	
				
			        fp1 = strtok(NULL,")");
			        strcpy(tuple[i].topic,fp1);

			        if (i==(count-1))
			            {
			                break;
			            }

			        fp1 = strtok(NULL,"(");

			        fp1 = strtok(NULL,",");	
				
			        strcpy(tuple[i+1].userID,fp1);	 
			    	i++;       
			    }

			    for(i=0;i<count;i++)
				{
					array[m]=tuple[i].userID;
					m++;
				}
				m=0;
				for(i=0;i<count;i++)
				{					
					for(j=0;j<i;j++)
					{

						if((strcmp(array[i],array[j]))==0)
							{					
								break;
							}
						
					}	
					 if (i==j)
					  {
						array1[m]=array[i];
						m++;				
					  }
				}
		
		n=0;
		temp=buffer_size;
		for(i=0;i < nthreads;i++)
		{
			n=0;
			for (j=0;j < count;j++)
			{
				if((strcmp(array1[i],tuple[j].userID))==0)
				{
					sem_wait(&s[i]);
					//pthread_mutex_lock(&buf_tok[i]);
					/* CHECKING IF BUFFER IS FULL OR NOT*/
					if(input[i]==temp)
						{					
							sem_post(&empty[i]);
							//pthread_cond_signal(&empty[i]);
							printf("\n");
							sem_post(&s[i]);
							sem_wait(&full[i]);
							sem_wait(&s[i]);
							//pthread_cond_wait(&full[i],&buf_tok[i]);
							input[i]=0;
							n=0;
						}
					else if(input[i] < temp)
						{
							strcpy(buffer1[i][n].userID,tuple[j].userID);
							strcpy(buffer1[i][n].topic,tuple[j].topic);
							buffer1[i][n].action=tuple[j].action;
							//printf(" Mapper giving input \t %s %d %s \n",buffer1[i][n].userID,buffer1[i][n].action,buffer1[i][n].topic);
							input[i]=input[i]+1;
							cnt[i]=cnt[i]+1;
							//printf("%dMapper count for %d \n",cnt[i],i);
							n++;	
						}
					sem_post(&s[i]);
					//pthread_mutex_unlock(&buf_tok[i]);
					}
				}
			done[i]=1;
			//printf("%d Setting count variable %d\n",i,cnt[i]);
			sem_post(&empty[i]);
			sem_post(&print[i]);
			//pthread_cond_signal(&empty[i]);
			//printf("done is made 1 for %s\n",buffer1[i][0].userID);
			}

				//exit(0);
		
						
				
	for (i=0;i<nthreads;i++)
		{
					switch(fork())
					{


						case 0:
								{	
										//printf("New process created %d\n",i);
										
										char input_topic[6];
			    						int input_action,total=0;
										int h=0,j,n,k;
										k=0;
										int flag;
										int count1[nthreads];
										for(k=0;k<nthreads;k++)
										{
											count1[k]=0;
										}
										struct tuples reducer[100];
										while(1)
										{
											sem_wait(&s[i]);
											//pthread_mutex_lock(&buf_tok[i]);
											/*CHECKING IF BUFFER IS EMPTY*/
											while(input[i]-output[i]==0 && done[i]==0)
											{
												//printf("No tuples in buffer %d.. Waiting! \n\n ",i);
												//pthread_cond_wait(&empty[i],&buf_tok[i]);
												sem_post(&s[i]);
												sem_wait(&empty[i]);
												sem_wait(&s[i]);
												//printf("Received conditional signal to empty buffer\n\n");
											}
											while(input[i]-output[i]>0)
											{
												//printf("Tuple inside reducer \n ");
												count1[i]=count1[i]+1;
												//printf("%d count in reducer %d\n",count1[i],i);
												for(j=0;j<input[i];j++)
												{	

													output[i]++;
													if(j==0 && cnt[i]>buffer_size)

													{
														strcpy(reducer[h].userID,buffer1[i][j].userID);
														strcpy(reducer[h].topic,buffer1[i][j].topic);
														reducer[h].action=buffer1[i][j].action;
														h++;
													}
													else
													{
														for(n=0;n<h;n++)
														{
															if(((strcmp(buffer1[i][j].topic,reducer[n].topic)) == 0))
															{
																reducer[n].action = reducer[n].action + buffer1[i][j].action;
																flag = 1;
																break;	
															}
														}
															if (flag != 1)
															{
																strcpy(reducer[h].userID,buffer1[i][j].userID);
																strcpy(reducer[h].topic,buffer1[i][j].topic);
																reducer[h].action=buffer1[i][j].action;
																h++;

															}	
														flag=0;
													}
												}
											}
									//printf(" REDUCER count%d\n",count1[i]);
										if(done[i]==1 && input[i]==output[i])
										{
											//printf("Final Output after total for threads %d\n",i);
											for(j=0;j<h;j++)
											{
												printf("(%s,%s,%d)\n",reducer[j].userID,reducer[j].topic,reducer[j].action);
											}
											sem_post(&s[i]);
											//pthread_mutex_unlock (&buf_tok[i]);
											break;
										}
										output[i]=0;
										input[i]=0;
										//pthread_cond_signal(&full[i]);
										sem_post(&full[i]);
										sem_post(&s[i]);
										//pthread_mutex_unlock(&buf_tok[i]);
									}
									exit(0);
							}
						default :
								break;

		}
	}
		fflush(stdout);
		while (wait(NULL) > 0)
  			{
   				 
   				// printf("child completed\n");
  			}
		return 0;
	}

int action_value(char ch)
{   int action;
    switch(ch)
        {
            case 'P': action=50;
                        return action;
                        break;
            case 'S': action=40;
                        return action;
                        break;
            case 'L': action=20;
                        return action;
                        break;
            case 'D': action=-10;
                        return action;
                        break;
            case 'C': action=30;
                        return action;
                        break;
		}
}