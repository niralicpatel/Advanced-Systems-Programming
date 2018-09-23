#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>

int action_value(char ch);
int count;
int m;
int nthreads;
int buffer_size;
pthread_mutex_t *mtx_ptr;
pthread_cond_t *cond_ptr;
pthread_mutex_t token;
pthread_mutex_t lock1;
int *input,*output;
struct tuples (*pointer);
int number[10];
struct tuples
    	{
    	char userID[5];
    	int action;
    	char topic[15];
    	};
struct tuples *ptr ;
struct tuples tuple[100];
struct tuples reducer[60][60];
//struct tuples **buffer1;
struct tuples buffer1[60][60];
void *mapperthread(void *arg)
	{
		printf("Mapper thread created\n");

        char *fp1,*fp2;
        char *buffer,value,topic[15],c;
        int action,counter,flag;
        int i=0,j,len,count=0,m=0,n;
		
     
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
        printf("\nThe number of tuples are: %d\n",count);
        //struct tuples tuple[count];
        //ptr=&tuple;
		char *array[count];
		char *array1[count];
		     
		i=0;
        fp1 = strtok(&buffer[1],",");       
		strcpy(tuple[i].userID,fp1);
		//printf("%d %s\n",i,tuple[i].userID);
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
                 //   printf("%d %s\n",i,tuple[i].userID);
                    break;
                }

            fp1 = strtok(NULL,"(");

            fp1 = strtok(NULL,",");	
		
            strcpy(tuple[i+1].userID,fp1);	
	    	//printf("%d %s\n",i+1,tuple[i+1].userID); 
	    	i++;       
         }
	 	//printf("ended\n");
	 	m=0;
	 	for(i=0;i<count;i++)
		{
			array[m]=tuple[i].userID;
			//printf("%s\n",tuple[i].userID);
			m++;
		}

	
	printf("ended\n");
	m=0;
	printf("All different userIDs\n");
	for(i=0;i<count;i++)
		{					
			for(j=0;j<i;j++)
			{

				if((strcmp(array[i],array[j]))==0)
					{
						counter++;						
						break;
					}
				
			}	
			 if (i==j)
			  {
				array1[m]=array[i];
				printf("%s\n",array[i]);
				m++;				
			  }
		}
		
	printf("Buffers for each user ID\n");	
	//struct tuples buffer1[m][count];
	//number[i]=counter;
	
	n=0;
	counter=0;
	for(i=0;i<m;i++)
		{
			pthread_mutex_lock(&token);
			counter=0;
			n=0;
			printf("buffer number is %d\n",i);			
			for(j=0;j<count;j++)
				{
				   if((strcmp(tuple[j].userID,array1[i]))==0)
					{
						strcpy(buffer1[i][n].userID,tuple[j].userID);
						buffer1[i][n].action=tuple[j].action;
						strcpy(buffer1[i][n].topic,tuple[j].topic);
						counter++;
						//fprintf("The buffer %d data:\n",i);
						//fprintf("(%s,%s,%d)\n",buffer1[i][n].userID,buffer1[i][n].topic,buffer1[i][n].action);
						n++;

					}	
				
				}
				number[i]=counter;
				printf("%d\n",counter);	
				pthread_mutex_unlock(&token);
		}	
	
	for(i=0;i<m;i++)
	{
		printf("In for buffer %d\n",i);
		for(j=0;j<number[i];j++)
		{

			strcpy(pointer[j].userID,buffer1[i][j].userID);
			pointer[j].action=buffer1[i][j].action;
			strcpy(pointer[j].topic,buffer1[i][j].topic);
			//printf("(%s,%s,%d)\n",pointer[j].userID,pointer[j].topic,pointer[j].action);
			//printf("ended\n");
			//printf("([%d][%d] %s,%s,%d)\n",i,j,buffer1[i][j].userID,buffer1[i][j].topic,buffer1[i][j].action);
		}
		//printf("Buffer %d ended\n",i);
	}
	//printf("mapper ends");
	pthread_exit(NULL);

	}

void *reducerthread(void *arg)
	{
	int i;
	pthread_mutex_t* mtx=&mtx_ptr[i];
	pthread_cond_t* cond=&cond_ptr[i];
	i=(int)arg;
	printf("\n %d Thread number \n",i);
	static int s;
	s++;
	char input_topic[6];
    int input_action,total;
	int h=0,j,n,k;
	//pthread_mutex_lock(&lock);
	//for(i=0;i<m;i++)
	//pthread_mutex_lock(&lock1);
	k=0;
	int flag[number[i]];
	pthread_mutex_lock(&mtx);
	for(j=0;j<number[i];j++)
	{	

		flag[j] = 0;
		
		if(j==0)

		{
			strcpy(reducer[i][j].userID,buffer1[i][j].userID);
			strcpy(reducer[i][j].topic,buffer1[i][j].topic);
			reducer[i][j].action=buffer1[i][j].action;
			//printf("%d",j);
			//printf(" %s %d %s",reducer[i][j].userID,reducer[i][j].action,reducer[i][j].topic);
			h=1;
		}
		else
		{
			for(n=0;n<j;n++)
			{
				if((strcmp(buffer1[i][j].topic,reducer[i][n].topic)) == 0)
				{
					reducer[i][n].action+=buffer1[i][j].action;
					flag[j] = 1;
				
					break;
				}
				strcpy(reducer[i][j].userID,buffer1[i][j].userID);
				strcpy(reducer[i][j].topic,buffer1[i][j].topic);
				reducer[i][j].action=buffer1[i][j].action;
				h++;

			}

		}
		
	}

	//for(i=0;i<m;i++)
	{
		for(j=0;j<number[i];j++)
		{
			if (flag[j]==0)
				//fprintf("Output of %d thread",i);
				//fprintf("(%s,%s,%d)\n",reducer[i][j].userID,reducer[i][j].topic,reducer[i][j].action);
				printf("(%s,%s,%d)\n",reducer[i][j].userID,reducer[i][j].topic,reducer[i][j].action);
		}
	}
	
	pthread_mutex_unlock(&mtx);		
	//printf("%d thread created\n",s);
	pthread_exit(NULL);	
	}


int main(int argc,char *argv[])
	{	
		 
		long i;
		
		//fp = fopen("output.txt","w");
		int buffer_size=atoi(argv[1]);
		int nthreads=atoi(argv[2]);
		pthread_t tid[nthreads];
		pthread_mutex_t mtx[nthreads];
		pthread_cond_t cond[nthreads];
		mtx_ptr = mtx;
		cond_ptr = cond;
		struct tuples c_buffer[buffer_size];
		pointer = &c_buffer;
		//struct tuples **buffer1;
		//buffer1=(struct tuples **) malloc (sizeof (struct tuples*) *nthreads);

		//*buffer1 = (struct tuples *) malloc (sizeof(struct tuples) *nthreads *buffer);
		//printf("%d %d \n",buffer_size,nthreads);
		pthread_create(&tid[0],NULL,mapperthread,NULL);	
		pthread_join(tid[0],NULL);
		sleep(0.5);
		for(i=0;i<nthreads;i++)
			{
			pthread_create(&tid[i],NULL,reducerthread,(void *)i);

			}
		//pthread_create(&tid[0],NULL,reducerthread,0);
		pthread_exit(NULL);
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