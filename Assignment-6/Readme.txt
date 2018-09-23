Possible Deadlocks
Testcase1 (testcase1.c)
In the given code, a process is forked to produce its child process and is transformed into another process using execl. The parent and child process are in Mode 1 and both of them try to access the device driver.The child process executes and it tries to access the lock but the parent process has the lock and it doesn't give away the lock. Simultaneously, the parent process waits for the child
process to terminate. Hence, it is one of the possible deadlock situations in the given code, as the processes are waiting for the one another to proceed which is not possible.

Testcase2 (testcase2.c)
In next case, situation is somewhat same to testcase1 except that parent and child processes both will be in Mode 2. And, the 'count2' variable in device driver program becomes 2. The parent process waits for the child process to complete. The child process will try to change mode using ioctl but it will not be able to do that, because the count2 is 2 and the ioctl waits until count2 becomes 1. But the parent process terminates only when the child process ends. Hence, a deadlock will occur in this case.

Testcase3 (testcase3.c)
When two threads are spawned. It can never be assumed which thread will run when(sequence of execution) or will they run together or not. Therefore, one of the threads will open in Mode 1 and it might have the lock for the driver and it will wait foreverr because of the while loop whereas the other thread will get blocked because of first thread while trying to open the device in Mode 1 because it will not be able to grab the lock from the other thread.

Testcase4 (testcase4.c)
The first thread opens in Mode 2 and the second thread opens in Mode 1. The two threads will try to perform ioctl to switch their present modes. The second thread switches to Mode 2 before the first thread goes to Mode 1. Such sequence can be achieved by putting a sleep statement(sleep) in the driver program. So, at some point both the threads will be in same mode in Mode 2, and the first thread when tries to change to Mode 1 by ioctl will get blocked because count2 will be 2, and the second thread waits for the first thread to end. 

Commands used-
1. sudo su
2. make
3. insmod assignment6.c
4. ./testcase1
		(ctrl c) 
5. ./testcase2
		(ctrl c)
6  ./testcase3
	(and need to restart VM instance before running testcase3 for checking deadlock)
7. ./testcase4
		(ctrl c)

Possible Race Conditions 
Critical region1 (e2_open(),e2_release())
In the open() function, updating the shared variable values of 'count1' and 'count2' might become the possible critical region. Improper synchronization without using 'sem1' and 'sem2' would have made the devices to read incorrect value of count1 and count2 which is result of Race condition. In the code, the critical region is enclosed within semaphores sem1 and sem2 which avoids the potential Race Condition. The same reasoning goes for the e2_release() function also.

Critical region2 (e2_read(),e2_write())
In these functions, accessing the devc->ramdisk shared variable might be the critical region. This is synchronised using sem1 and sem2. If it was not synchronised the devices might have read and
written wrong values to the driver.

Critical region3 (e2_ioctl())
The count1 and count2 variables are the shared variables. They are important in changing the modes of the devices. They are synchronised correctly using sem1 and sem2. Or else the devices may end up changing to a wrong mode.
