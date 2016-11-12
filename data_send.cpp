#include "data.h"


int fd;
int on_off=0;
int lock=0;
int humidity = 0;
int temperature = 0;
int light = 0;
int valid_length=0;
int result = 0;

int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio,oldtio;
    if  ( tcgetattr( fd,&oldtio)  !=  0) 
    { 
        perror("SetupSerial 1");
        return -1;
    }
    bzero( &newtio, sizeof( newtio ) );
    newtio.c_cflag  |=  CLOCAL | CREAD; 
    newtio.c_cflag &= ~CSIZE; 

    switch( nBits )
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
    }

    switch( nEvent )
    {
    case 'O':                     
        newtio.c_cflag |= PARENB;
        newtio.c_cflag |= PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'E':                     
        newtio.c_iflag |= (INPCK | ISTRIP);
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        break;
    case 'N':                   
        newtio.c_cflag &= ~PARENB;
        break;
    }

switch( nSpeed )
    {
    case 2400:
        cfsetispeed(&newtio, B2400);
        cfsetospeed(&newtio, B2400);
        break;
    case 4800:
        cfsetispeed(&newtio, B4800);
        cfsetospeed(&newtio, B4800);
        break;
    case 9600:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    case 115200:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
        break;
    default:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    }
    if( nStop == 1 )
    {
        newtio.c_cflag &=  ~CSTOPB;
    }
    else if ( nStop == 2 )
    {
        newtio.c_cflag |=  CSTOPB;
    }
    newtio.c_cc[VTIME]  = 1;
    newtio.c_cc[VMIN] = 1;
    tcflush(fd,TCIFLUSH);
    if((tcsetattr(fd,TCSANOW,&newtio))!=0)
    {
        perror("com set error");
        return -1;
    }
    printf("set done!\n");
    return 0;
}

int open_port(int fd,int comport)
{
  
    if (comport==1)
    {    fd = open( "/dev/ttyUSB0", O_RDWR|O_NOCTTY|O_NDELAY);
        if (-1 == fd)
        {
            perror("Can't Open Serial Port");
            return(-1);
        }
        else 
        {
            printf("open ttyUSB0 .....\n");
        }
    }
    else if(comport==2)
    {    fd = open( "/dev/ttyUSB1", O_RDWR|O_NOCTTY|O_NDELAY);
        if (-1 == fd)
        {
            perror("Can't Open Serial Port");
            return(-1);
        }
        else 
        {
            printf("open ttyUSB1 .....\n");
        }    
    }
    else if (comport==3)
    {
        fd = open( "/dev/ttyUSB2", O_RDWR|O_NOCTTY|O_NDELAY);
        if (-1 == fd)
        {
            perror("Can't Open Serial Port");
            return(-1);
        }
        else 
        {
            printf("open ttyUSB2 .....\n");
        }
    }
    if(fcntl(fd, F_SETFL, 0)<0)
    {
        printf("fcntl failed!\n");
    }
    else
    {
        printf("fcntl=%d\n",fcntl(fd, F_SETFL,0));
    }
    if(isatty(STDIN_FILENO)==0)
    {
        printf("standard input is not a terminal device\n");
    }
    else
    {
        printf("isatty success!\n");
    }
    printf("fd-open=%d\n",fd);
    
    return fd;
}

void uart_init(void)
{
	
    int i;
    fd = open_port(fd,1);
    
    if(fd<0)
    {
        perror("open_port error");
       // return -1;
    }
    
    i=set_opt(fd,115200,8,'N',1);
   
    if(i<0)
    {
        perror("set_opt error");
        //return -1;
    }
    
    
}

void get_data(void)
{
	send_onedata('D');
	usleep(10000);
	int nread,i; 
	unsigned char buff_read[11];
    unsigned char buff_read1[22];
    
    
    nread=read(fd,buff_read,sizeof(buff_read));
   // printf("nread=%d,%s\n",nread,buff);
    //close(fd);
    if(nread>0)
    {          memcpy(buff_read1 + valid_length, buff_read, nread);
               valid_length = valid_length + nread;
               while (valid_length >= 11)
               {
                   for (i = 0; (buff_read1[i] != 0x10) && (i < valid_length); i++);
                   if (i == valid_length)
                   {
				       valid_length = 0;
				       break;
			       }
			       else
			       {
                       if (i > 0)
                       {
					       valid_length = valid_length - i;
				           memmove(buff_read1, buff_read1 + i, valid_length);
			           }
			           
			           if (valid_length >= 11)
			           {
					       if ((buff_read1[2] != ':') || (buff_read1[10] != '\0'))
					       {
					           valid_length--;
				               memmove(buff_read1, buff_read1 + 1, valid_length);
				               continue;
						   }
						   else
						   {
							   
							   
							   switch (buff_read1[1])
							   {
								   case 'd':
								   {
									   temperature= buff_read1[4] | (buff_read1[5] <<8);
									   humidity = buff_read1[3];
									   light = buff_read1[6] | (buff_read1[7] << 8) | (buff_read1[8] << 16 ) | (buff_read1[9] << 24);
										printf("humidity   : %d\n", humidity);
										printf("temperature   : %.1f\n", temperature*0.1);
									    printf("light   : %.1f\n", light*0.0083);	
									   break;
								   }
								  
								   default:
								   {
								       valid_length--;
				                       memmove(buff_read1, buff_read1 + 1, valid_length);
				                       continue;
								   }
							   }
							   valid_length = valid_length - 8;
							   if (valid_length != 0)
							   {
				                   memmove(buff_read1, buff_read1 + 8, valid_length);
						       }
						   }
				       }
		           }
		       }
		   }
}

void uart_write(unsigned char* buff)
{
       write(fd,buff,sizeof(buff));
      // printf("write size: %d\n",sizeof(buff));
}

void send_onedata(int value)
{
	
	unsigned char buff[1];
	buff[0]=value;
	uart_write(buff);
}

void alarm(int variable, int value)
{
	
	
	   if(variable>value)
		{
			if(on_off == 0 && lock ==1)
			{
				lock=0;
		    }
			on_off=1;
			lock++;
			if(lock>=2)
			{
				lock=2;
			}
			
		}
		if(variable<=value)
		{
			if(on_off==1 && lock==1)
			{
				lock=2;
			}
			on_off=0;
			lock --;
			if(lock<=0)
			{
				lock=0;
			}
			
	    }
		if(on_off==1 && lock == 1)
		{
			send_onedata('O');
		}
		if(on_off == 0 && lock == 1)
		{
			
			send_onedata('S');
		}
	
}
