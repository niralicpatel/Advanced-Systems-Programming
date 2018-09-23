ASP Assignment 5
Nirali Patel
UFID -36318593

The folder has char_driver.c, userapp.c, Readme.txt, Makefile

Steps followed to run the module:

1. Become root user : sudo su 

2. Compile driver module : # make

3. Load module : # insmod char_driver.ko <arg>
	Example: # insmod char_driver.ko 4
	(It will take default number of devices as 3(if not specified in place of arg))

4. Compile userapp : # make userapp

5. Run userapp : # ./userapp <number>
	Example: # ./userapp 0			
		where number specifies the id number of the device to be tested   
		   
6. Unload module : # rmmod char_driver.ko









