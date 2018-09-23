#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int action_value(char array);
struct tuples
    {
    char userID[4];
    int action;
    char topic[15];
    };
int main()
{
        FILE *fp;
        char *fp1,*fp2,*ptr;
        char *buffer,value,topic[15],c;
        int action;
        int i=0,j,len,count=0;

        fp=fopen("/home/nirali/Downloads/input.txt","r");
        if (fp == NULL)
        {
        	printf("Could not open file");
        	return 1;
        }
        buffer = (char *)malloc(3000);
        while (fgets(buffer,3000,fp) != NULL)
        {
           // printf("%s\n", buffer);
        }
        fseek(fp,0,SEEK_SET);
        while ((c=getc(fp)) != EOF)
        {
            if (c == ')')
            {
                count++;
            }
        }
        //printf("\nThe number of tuples are: %d\n",count);
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
                    printf("(%.4s,%-15s,%d)\n",tuple[i].userID,tuple[i].topic,tuple[i].action);
                    break;
                }

            fp1 = strtok(NULL,"(");

            fp1 = strtok(NULL,",");
            strcpy(tuple[i+1].userID,fp1);

            printf("(%.4s,%-15s,%d)\n",tuple[i].userID,tuple[i].topic,tuple[i].action);
            i++;
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
