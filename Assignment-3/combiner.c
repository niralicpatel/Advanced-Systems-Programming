#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>
#include<semaphore.h>

int action_value(char ch);
int buffer_size,nthreads;
int *input,*output,*cnt;
sem_t s[50],full[50],empty[50];
struct tuples
	{
		char userID[5];
		int action;
		char topic[15];
	};
int count;
	/* POINTER TO MAIN BUFFER*/
struct tuples **buffer1;
void *mapperthread(void *arg);
void *reducerthread(void *arg);
//pthread_mutex_t buf_tok[100];	
//pthread_cond_t full[100],empty[100];
int done[100];

		/* MAIN THREAD */
int main(int argc,char *argv[])
	{	
		int i;
		buffer_size = atoi(argv[1]);
		nthreads = atoi(argv[2]);
		if(buffer_size<nthreads)
		{
			buffer_size=nthreads+1;
		}
		pthread_t tid[nthreads],tid1;
		
		for(i=0;i < nthreads;i++)
			{
        		sem_init (&s[i],0,1);
        		sem_init(&full[i],0,0);
				sem_init(&empty[i],0,0);
			}

		/* CREATING MAIN BUFFER */	
		buffer1=(struct tuples **) malloc (sizeof (struct tuples *) * nthreads);
		*buffer1 = (struct tuples *) malloc (sizeof(struct tuples) * nthreads * buffer_size);
	
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
				//pthread_mutex_init(&buf_tok[i],NULL);
				//pthread_cond_init(&full[i],NULL);
				//pthread_cond_init(&empty[i],NULL);
			}	
		
		pthread_create(&tid1,NULL,mapperthread,NULL);	
		
		for(i=0;i<nthreads;i++)
			{
				pthread_create(&tid[i],NULL,reducerthread,(void *)i);
			}

		pthread_join(tid1,NULL);

		for(i=0;i<nthreads;i++)
			{
				pthread_join(tid[i],NULL);
			}

		
		/*for(i=0;i<nthreads;i++)
			{
	 			pthread_mutex_destroy(&buf_tok[i]);
	 			pthread_cond_destroy(&full[i]);
	 			pthread_cond_destroy(&empty[i]);
	 		}*/
		pthread_exit(NULL);
		return 0;
	}
	/* MAPPER THREAD */

void *mapperthread(void *arg)
{
	char *fp1,*fp2;
    char *buffer,value,topic[15],c;
    int action,counter,temp=0;
    int i=0,j,len,m=0,n;
	
 
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
							n++;	
						}
					sem_post(&s[i]);
					//pthread_mutex_unlock(&buf_tok[i]);
				}
			}
		done[i]=1;
		sem_post(&empty[i]);
		//pthread_cond_signal(&empty[i]);
		//printf("done is made 1 for %s\n",buffer1[i][0].userID);
		}
	pthread_exit(NULL);
}
	/* REDUCER THREAD */
void *reducerthread(void *arg)
{
	int i=(int) arg;
	
	char input_topic[6];
    int input_action,total;
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
			for(j=0;j<input[i];j++)
			{	

				//printf(" in reducer \t (%s,%s,%d)\n",buffer1[i][j].userID,buffer1[i][j].topic,buffer1[i][j].action);
				//flag[j] = 0;
				output[i]++;
				//printf("----%d",j);
				if(j==0 && cnt[i]>buffer_size)

				{
					strcpy(reducer[h].userID,buffer1[i][j].userID);
					strcpy(reducer[h].topic,buffer1[i][j].topic);
					reducer[h].action=buffer1[i][j].action;
					count1[i]=count1[i]+1;
					
					//printf("inside reducer(%s,%s,%d)\n",reducer[h].userID,reducer[h].topic,reducer[h].action);
					h++;
					//printf("(%s,%s,%d)\n",reducer[i][j].userID,reducer[i][j].topic,reducer[i][j].action);
					//h=1;
				}
				else
				{
					for(n=0;n<h;n++)
					{
						if(((strcmp(buffer1[i][j].topic,reducer[n].topic)) == 0))
						{
							//printf("here");
							reducer[n].action = reducer[n].action + buffer1[i][j].action;
							flag = 1;
							//printf("hello everthing is same here\n");
							//printf("updating (%s,%s,%d)\n",reducer[n].userID,reducer[n].topic,reducer[n].action);
							break;
						}
						//if(n<cnt[i]-1){
						//	continue;	
						//}
					}
						if (flag != 1)
						{
							strcpy(reducer[h].userID,buffer1[i][j].userID);
							strcpy(reducer[h].topic,buffer1[i][j].topic);
							reducer[h].action=buffer1[i][j].action;
							count1[i]=count1[i]+1;
						
							//printf("inside reducer (%s,%s,%d)\n",reducer[h].userID,reducer[h].topic,reducer[h].action);
							h++;

						}
						flag = 0;
						
						//printf("everything is not same. its a new entry\n");
						//printf("(%s,%s,%d)\n",reducer[i][n].userID,reducer[i][n].topic,reducer[i][n].action);
						//h++;	
				}
			}
			//printf("tuples in reducer %d\n",h);
		}
		output[i]=0;
		input[i]=0;
		if(done[i]==1)
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
		//pthread_cond_signal(&full[i]);
		sem_post(&full[i]);
		sem_post(&s[i]);
		//pthread_mutex_unlock(&buf_tok[i]);
	}
	
	pthread_exit(NULL);
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