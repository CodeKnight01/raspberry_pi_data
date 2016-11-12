#include "data.h"

extern int humidity;
extern int temperature;
extern int light;


int main()
{
	uart_init();
	
	while(1)
	{
		get_data();
		
		alarm(temperature,240);
		usleep(500000);
	}
}
