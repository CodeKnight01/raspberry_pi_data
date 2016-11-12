open ttyUSB0, decode the data from stm32 board. 
data: 
extern int temperature
extern int humidity
extern int light

give these three global variable to IoT in data_send.cpp get_data() function.

give raspberry pi a int data to control the light.
example:
int stats;
alarm(stats,50)
then, if stats == 40 , the light will be off. if stats == 60, the light will be on.