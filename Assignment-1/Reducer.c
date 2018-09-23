#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct tuples
    {
    char userID[5];
    int action;
    char topic[16];
    };
int main()
    {
        int n=0,total_score=0,flag=0;
        char *input_userID[5];
        char *input_topic[5];
        int input_action;
        int m=0,counter=0;
        char buffer[200];
        int i,j=0,count=0,a=0;
        int count1=0;
        struct tuples tuple[50];
        FILE *input;
        char *str;
        i=0;
        label:
        while(fgets(buffer,200,stdin))
        {
            str = strtok(&buffer[1],",");
            strcpy(input_userID,str);

            str = strtok(NULL,",");
            strcpy(input_topic,str);

            str = strtok(NULL,")");
            input_action=atoi(str);

            if(i==0)
            {
                strcpy(tuple[i].userID,input_userID);
                strcpy(tuple[i].topic,input_topic);
                tuple[i].action=input_action;
                i++;
            }
            else
            {
                if(strcmp(input_userID,tuple[i-1].userID)==0)
                {

                    for(j=0;j<i;j++)
                    {
                        if(strcmp(input_topic,tuple[j].topic)==0)
                        {
                            total_score=(tuple[j].action+input_action);
                            tuple[j].action=total_score;
                            goto label;
                        }
                    }
                    strcpy(tuple[i].userID,input_userID);
                    strcpy(tuple[i].topic,input_topic);
                    tuple[i].action=input_action;
                   // printf("The new entry at position %d is: (%s,%s,%d)\n",i,tuple[i].userID,tuple[i].topic,tuple[i].action);
                    i++;
                    //printf("Value of the i is: %d\n\n",i);
                }

                else
                {
                    for(m=0;m<i;m++)
                    {
                    printf("(%s,%s,%d) \n",tuple[m].userID,tuple[m].topic,tuple[m].action);
                    }
                    i=0;
                    strcpy(tuple[i].userID,input_userID);
                    strcpy(tuple[i].topic,input_topic);
                    tuple[i].action=input_action;

                    i = 1;

                }

            }

            }
            for(a=0;a<i;a++)
                {
                printf("(%s,%s,%d)\n",tuple[a].userID,tuple[a].topic,tuple[a].action);
                }

        return 0;
    }
