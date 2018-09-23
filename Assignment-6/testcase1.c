#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<pthread.h>
#include<unistd.h>
#include<sys/wait.h>
#include <linux/ioctl.h>
#include <sys/ioctl.h>

#define DEVICE "/dev/a5"
#define CDRV_IOC_MAGIC 'Z'
#define E2_IOCMODE1 _IOWR(CDRV_IOC_MAGIC, 1, int)
#define E2_IOCMODE2 _IOWR(CDRV_IOC_MAGIC, 2, int)

int main() 
{
	int i,fd, status;
	pid_t ret;
	//char ch, write_buf[100], read_buf[100];
	fd = open(DEVICE, O_RDWR);
	if(fd == -1)
	{
		printf("File %s either does not exist or is locked by some other process\n", DEVICE);
		exit(-1);
	}
	
	ret= fork();
	if(ret==0)
	{
		execl("./sample",NULL);
		printf("here I reached");
	}	
	else
	{
		wait(&status);
		ioctl(fd,E2_IOCMODE1,0);
	}
	close(fd);	
}
