/bootable/recovery/recovery.cpp:    ui->Print("Rebooting...\n");//jiangdou ,Rebooting...

.....(略)
  
      ui->Print("Rebooting...\n");//jiangdou ,Rebooting...
      //property_set(ANDROID_RB_PROPERTY, "reboot,");
  	android_reboot(ANDROID_RB_RESTART, 0, 0);
      return EXIT_SUCCESS;
  }
.....(略)
/*************************************我是分割线************************************************************/
android_reboot（）  -->>

/system/core/libcutils/android_reboot.c:int android_reboot(int cmd, int flags, char *arg)
//android_reboot(ANDROID_RB_RESTART, 0, 0);
int android_reboot(int cmd, int flags, char *arg)
{
    int ret;
	
    sync();
    remount_ro();
	
    switch (cmd) {
        case ANDROID_RB_RESTART:
			recovery_reboot();//add by jiangdou!!!!!!!QQ:344283973
            ret = reboot(RB_AUTOBOOT);
            break;

        case ANDROID_RB_POWEROFF:
            ret = reboot(RB_POWER_OFF);
            break;

        case ANDROID_RB_RESTART2:
            ret = __reboot(LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2,
                           LINUX_REBOOT_CMD_RESTART2, arg);
            break;

        default:
            ret = -1;
    }
	//recovery_reboot();//add by jiangdou!!!!!!!QQ:344283973
    return ret;
}


//system/core/include/cutils/android_reboot.h

/*************************************我是分割线************************************************************/

///add  Anroid.mk   at  /system/core/libcutils/

include $(CLEAR_VARS)
LOCAL_MODULE := libcutils
LOCAL_SRC_FILES := $(commonSources) \
        android_reboot.c \
        ashmem-dev.c \
        debugger.c \
        klog.c \
        partition_utils.c \
        properties.c \
        qtaguid.c \
        trace.c \
        uevent.c \
		    jiangdou_reboot.c  //add by jiangdou
		    
/*************************************我是分割线************************************************************/

add file jiangdou_reboot.c

/*
* file jiangdou_reboot.c
* time at 2015-05-10
*
* author by jiangdou  QQ:344283973
*
*
*/

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>

#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <termios.h>//
#include <signal.h>

#include <cutils/android_reboot.h>


#define HOST_PORT 0

int Set_Port_dou(int fd,int baud_rate,int data_bits,char parity,int stop_bits)
{
	struct termios newtio,oldtio; //
	
	//fprintf(stdout,"The Fucntion Set_Port() Begin!\n");
	
	if( tcgetattr(fd,&oldtio) !=0 )
	{
		//perror("Setup Serial:");
		return -1;
	}
	
	bzero(&newtio,sizeof(newtio));
	
	newtio.c_cflag |= CLOCAL | CREAD;
	newtio.c_cflag &= ~CSIZE;
	
	//Set BAUDRATE
	
	switch(baud_rate)
	{
		case 2400:
			cfsetispeed(&newtio,B2400);
			cfsetospeed(&newtio,B2400);
			break;
		case 4800:
			cfsetispeed(&newtio,B4800);
			cfsetospeed(&newtio,B4800);
			break;
		case 9600:
			cfsetispeed(&newtio,B9600);
			cfsetospeed(&newtio,B9600);
			break;
		case 19200:
			cfsetispeed(&newtio,B19200);
			cfsetospeed(&newtio,B19200);
			break;	
		case 38400:
			cfsetispeed(&newtio,B38400);
			cfsetospeed(&newtio,B38400);
			break;
		case 115200:
			cfsetispeed(&newtio,B115200);
			cfsetospeed(&newtio,B115200);
			break;																
		default:
			cfsetispeed(&newtio,B9600);
			cfsetospeed(&newtio,B9600);
			break;
		
	}
	
	//Set databits upon 7 or 8
	switch(data_bits)
	{
		case 7:
			newtio.c_cflag |= CS7;
			break;
		case 8:
		default:
			newtio.c_cflag |= CS8;
	}
	
	switch(parity)
	{
		default:
		case 'N':
		case 'n':
		{
			newtio.c_cflag &= ~PARENB;
			newtio.c_iflag &= ~INPCK;
		}
		break;
		
		case 'o':
		case 'O':
		{
			newtio.c_cflag |= (PARODD|PARENB);
			newtio.c_iflag |= INPCK;
		}
		break;
		
		case 'e':
		case 'E':
		{
			newtio.c_cflag |= PARENB;
			newtio.c_cflag &= ~PARODD;
			newtio.c_iflag |= INPCK;
		}
		break;
		
		
		case 's':
		case 'S':
		{
			newtio.c_cflag &= ~PARENB;
			newtio.c_cflag &= ~CSTOPB;
			
		}
		break;
	}
	
	//Set STOPBITS 1 or 2
	switch(stop_bits)
	{
		default:
		case 1:
		{
			newtio.c_cflag &= ~CSTOPB;
		}
		break;
		
		case 2:
		{
			newtio.c_cflag |= CSTOPB;
		}
		break;
		
	}
	
	newtio.c_cc[VTIME]  = 1;
	newtio.c_cc[VMIN]	= 255;	//Read Comport Buffer when the bytes in Buffer is more than VMIN bytes!
	
	tcflush(fd,TCIFLUSH);
	
	if(( tcsetattr(fd,TCSANOW,&newtio))!=0 )
	{
		//perror("Com set error");
		return -1;
	}
	
	//fprintf(stdout,"The Fucntion Set_Port() End!\n");
	
	return 0;
}
int Open_Port_dou(int com_port)
{
	int fd = 0;
	
	//fprintf(stdout,"Function Open_Port Begin!\n");
		
	//char *dev[] = { "/dev/ttyS0","/dev/ttyS1","/dev/ttyS2","/dev/ttyS3","/dev/ttyS4","/dev/ttyS5","/dev/ttyS6"};
		
	if( (com_port < 0) || (com_port >6) )
	{
		perror("The port is out range:");
		return -1;
	}
	
	//Open the port	
	//fd = open(dev[com_port],O_RDWR|O_NOCTTY|O_NDELAY);
	fd = open("/dev/ttyS3",O_RDWR|O_NOCTTY|O_NDELAY);	
	if( fd<0 )
	{
		//perror("Open serial port:");
		return -1;
	}
	
	if( fcntl(fd,F_SETFL,0)<0 )
	{
		//perror("fcntl F_SETFL:");
		return -1;
	}
	
	if( isatty(fd) ==0 )
	{
		//perror("isatty is not a terminal device");
		return -1;
	}
	
	return fd;
}


int recovery_reboot(void)
//int main(void)
{	
	int fd =0;
	int j = 0;
	char buf[] = "dou:rebo#";
	if((fd = Open_Port_dou(HOST_PORT)) == -1)
	{
		perror("Open port");
		return -1;
	}
	
	if( Set_Port_dou(fd,9600,8,'N',1) == -1)
	{
		perror("Set_Port");
		return -1;
	}
	
	//发送reboot CMD!!!! by jiangdou!!!!!!
	for(;;){
		write(fd,buf,strlen(buf));//key_id = "dou:rebo#"
		sleep(1);
	}
	
	
	return 0;
	
}
/*************************************我是分割线************************************************************/
