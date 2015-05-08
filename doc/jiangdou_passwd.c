#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/time.h>
#include<pthread.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>
#include<stdlib.h>
#include<time.h>
#include <private/android_filesystem_config.h>
#include "jiangdou_passwd.h"





#include <dirent.h>

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>

#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include <semaphore.h>
#include <pthread.h>
#include <termios.h>
#include <signal.h>

#include <linux/kd.h>
#include <linux/fb.h>

#include <sys/time.h>
#include <sys/ipc.h>
//#include <sys/msg.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
//#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/un.h>



pthread_t thread[1];
pthread_mutex_t mut;
int fd =0;
int IsReceve = 0;
unsigned char msg[1024];
unsigned char buff[80];

time_t now;
struct tm *tm_now;
char *datetime;















int Set_Port(int fd,int baud_rate,int data_bits,char parity,int stop_bits)
{
	struct termios newtio,oldtio; //
	
	//fprintf(stdout,"The Fucntion Set_Port() Begin!\n");
	
	if( tcgetattr(fd,&oldtio) !=0 )
	{
		perror("Setup Serial:");
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
		perror("Com set error");
		return -1;
	}
	
	//fprintf(stdout,"The Fucntion Set_Port() End!\n");
	
	return 0;
}
int Open_Port(int com_port)
{
	int fd = 0;
	
	//fprintf(stdout,"Function Open_Port Begin!\n");
		
	char *dev[] = { "/dev/ttyS0","/dev/ttyS1","/dev/ttyS2","/dev/ttyS3","/dev/ttyS4","/dev/ttyS5","/dev/ttyS6"};
		
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
		perror("Open serial port:");
		return -1;
	}
	
	if( fcntl(fd,F_SETFL,0)<0 )
	{
		perror("fcntl F_SETFL:");
		return -1;
	}
	
	if( isatty(fd) ==0 )
	{
		perror("isatty is not a terminal device");
		return -1;
	}
	
	return fd;
}




int StrToInt(char *str)
{
	 int value  = 0;
	 int sign   = 1;
	 int result = 0;
	 if(NULL == str)
	 {
		return -1;
	 }
	 if('-' == *str)
	 {
		  sign = -1;
		  str++;
	 }
	 while(*str)
	 {
		  value = value * 10 + *str - '0';
		  str++;
	 }
	 result = sign * value;
	 return result;
}



void read_port(void)
{
	fd_set rd;
	int nread,retval;

	struct timeval timeout;
	FD_ZERO(&rd);
	FD_SET(fd,&rd);
	timeout.tv_sec = 3;
	timeout.tv_usec = 0;
	while(IsReceve == 1);
	retval = select(fd+1,&rd,NULL,NULL,&timeout);
	switch(retval)
	{
	case 0:
		//printf("No data input within 1 seconds.\n");
		break;
	case -1:
		perror("select:");
		break;
	default:
		if( (nread = read(fd,msg,1024))>0 )
		{
			IsReceve =1;
			//printf("%sReceiveMessage: %s\n",msg,datetime);
			//printf("\n%sReceive %d bytes,Message is:\n%s\n",datetime,nread,msg);
			sprintf(buff, "%s", msg);
			memset(msg,0,1024);
		}
		break;		
	}//end of switch

}




void create_thread(void)
{
	int temp;
	memset(thread,0,sizeof(thread));
	if((temp = pthread_create(&thread[0],NULL,(void *)read_port,NULL)) != 0)
		printf("Create recv_thread failed!\n");
	
}

void wait_thread(void)
{

	if(thread[0] != 0)
	{
		pthread_join(thread[0],NULL);//等待线程结束
		//printf("recev_thread end\n");

	}
}


int rk3288_shut_down()//关机
{
	
	show_error_logo();//显示ERROR!
	sleep(6);//延时2S
	//xxx_xx();//关机命令
	write(fd,"shutd",5);//关机命令
//	for(;;){  //死循环
//		sleep(3);//延时2S  
//	}
	return 1;
}



#define HOST_PORT 0
int keyID_parse(void)
{
	//int fd = 0;
	//char buffer[BUFFER_SIEZE] = {0};

	if((fd = Open_Port(HOST_PORT)) == -1)
	{
		perror("Open port");
		return -1;
	}
	
	if( Set_Port(fd,9600,8,'N',1) == -1)
	{
		perror("Set_Port");
		return -1;
	}
	
	//Serial_SendStr(fd,"Hello This is from Ubuntu\n");

	pthread_mutex_init(&mut,NULL);
//###########################################################
//取随机数，发送到MCU  MCU解析dou:1234   a=1234  //keyID = (a * 2) - 3   send keyID to rk3288
	unsigned char x[2];
	unsigned int i;
	char buf[20];
	char *pp = buf;
	unsigned char *ppp = buff;
	srand(time(0));
	i = rand();//取随机数
	x[0] = (char)((i >> 8) & 0xff);//16bit_height
	//printf("x[0] =%x  xsize = %d\n", x[0], sizeof(x[0]));
	x[1] = (char)((i >> 24) & 0xff);//16bit_height
	//printf("x[1] =%x  xsize = %d\n", x[1], sizeof(x[1]));
	sprintf(buf, "dou:%d", ((x[1] << 8) | x[0]) & 0xffff);//sprintf(s, "", ,)//(((x[1] << 8) | x[0]) & 0xffff)
	//printf("buf = %s \n",buf);//buf =string
	
	////local_ID = 32577
	int local_ID = ((x[1] << 8) | x[0]) & 0xffff;//local_ID = 32577
	//send str: ="dou:32577"
	write(fd,buf,strlen(buf));//key_id = (a * 2) - 3;//for KEY_ID at MCU!!!!  
	
	char *b = strstr(pp, ":");
	sprintf(buf, "%s",(b + 1));//buf =string
	//printf("b = %d \n",StrToInt(pp));
	
	int whi = 0;
	char *bb;
	int key_id;
	int reve_id;
//###########################################################
	while(1)
	{
		whi++;
		time(&now);
		tm_now = localtime(&now);
		datetime=asctime(tm_now);
		
		create_thread();
        	wait_thread();//等待线程结束
		
		//printf("jiangdou while\n");
		write(fd,"while...",8);
		if( IsReceve ==1)//表示有recv数据
        	{
			//printf("Message is:%s\n",buff);//recv "dou:65151"
			write(fd,"ifif...",7);
			
			bb = strstr(ppp, "dou:");//recv str = "dou:65151"
			if(bb != NULL){
				
				bb = strstr(ppp, ":");
				sprintf(buf, "%s",(bb + 1));
				key_id = StrToInt(pp);//key_id = 65151;
				 
				write(fd,buf,strlen(buf));
				
				reve_id = (key_id + 3)/2;// reve_id = 32577;
				
				//if(reve_id == local_ID){
				if(key_id == 2244){
					
					goto go_on;// passwd success!!
					
				}else{
					//close(fd);
					rk3288_shut_down();//关机
					   // passwd fali!!!
				}
			}
			
			
			IsReceve = 0;
			
			
		}
		
			
		if(whi > 5){
			 // fali!!!  time out 5S!!
			whi = 0;
			//close(fd);
			rk3288_shut_down();//关机	
		}
     

	}
go_on:	
	close(fd);
	
	return 0;
 
}

//################################################################################
//################################################################################



typedef struct
{
    char cfType[2];         /* 文件类型, 必须为 "BM" (0x4D42) */
    char cfSize[4];         /* 文件的大小(字节) */
    char cfReserved[4];     /* 保留, 必须为 0 */
    char cfoffBits[4];      /* 位图阵列相对于文件头的偏移量(字节) */
} BITMAPFILEHEADER;       /* 文件头结构 */

typedef struct
{
    char ciSize[4];         /* size of BITMAPINFOHEADER */
    int ciWidth;        /* 位图宽度(像素) */
    int ciHeight;       /* 位图高度(像素) */
    char ciPlanes[2];       /* 目标设备的位平面数, 必须置为1 */
    int16_t ciBitCount;     /* 每个像素的位数, 1,4,8或24 */
    char ciCompress[4];     /* 位图阵列的压缩方法,0=不压缩 */
    char ciSizeImage[4];    /* 图像大小(字节) */
    char ciXPelsPerMeter[4];/* 目标设备水平每米像素个数 */
    char ciYPelsPerMeter[4];/* 目标设备垂直每米像素个数 */
    char ciClrUsed[4];      /* 位图实际使用的颜色表的颜色数 */
    char ciClrImportant[4]; /* 重要颜色索引的个数 */
} BITMAPINFOHEADER;       /* 位图信息头结构 */

typedef struct 
{
    unsigned char b;
    unsigned char g;
    unsigned char r;
}RGB;

typedef struct
{
    RGB* data;
    unsigned int width;
    unsigned int height;
    int16_t bit_count;
}BMP_IMAGE;

BMP_IMAGE* ReadBmp2Buf(char* bmp_file) {
    FILE *fp;
    int rc;
    BITMAPFILEHEADER FileHead;
    BITMAPINFOHEADER InfoHead;
    int width,height,bit_count;
    fp = fopen( bmp_file, "rb" );
    if (fp == NULL) {
        return -1;
    }
    rc = fread( &FileHead, 1, sizeof(BITMAPFILEHEADER), fp );
    if ( rc != sizeof( BITMAPFILEHEADER ) ) {
        fclose( fp );
        return -1;
    }

    if (memcmp(FileHead.cfType, "BM", 2) != 0) {
        fclose( fp );
        return -1;
    }

    rc = fread( (char *)&InfoHead, 1, sizeof(BITMAPINFOHEADER), fp );
    if ( rc != sizeof(BITMAPINFOHEADER) ) {
        fclose( fp );
       return -1;
    }
    width = InfoHead.ciWidth;
    height = InfoHead.ciHeight;
    bit_count = InfoHead.ciBitCount;

    printf("%d %d %d\n", width, height, bit_count);
    if( bit_count != 24) {
        return -1;
    }
    int lineByte = (width * bit_count + 31 ) / 32 *4;
    int size = lineByte * height;

    RGB* data = (RGB*) malloc(sizeof(RGB) * size);
    fread(data, sizeof(RGB), size , fp);

    BMP_IMAGE *pBmp_image = (BMP_IMAGE*) malloc(sizeof(BMP_IMAGE) );
    pBmp_image->data = data;
    pBmp_image->height = height;
    pBmp_image->width = width;
    pBmp_image->bit_count = bit_count;

    return pBmp_image;
}

void Release_Bmp_Image(BMP_IMAGE **pBmp_image) {
    if(*pBmp_image != NULL ) {
        free((*pBmp_image)->data);
        free(*pBmp_image);
        *pBmp_image = NULL;
    }
}

void show_error_logo(void)
{
	//int main(int argc, char **argv ) {
    BMP_IMAGE* pBmp_image;
    pBmp_image = ReadBmp2Buf("./logo.bmp");

   // BMP_IMAGE* wallpapper;
   // wallpapper = ReadBmp2Buf("./logo.bmp");

    int fp = 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;

    long screen_size = 0;
    char *fbp = NULL;
    int x= 0, y= 0;
    long location = 0;

    float vx = 0.0f,vy = 0.0f;
    fp = open("/dev/graphics/fb0", O_RDWR );

    if(fp < 0 ) {
        printf("Error: Can not open framebuffer!\n");
        return -1;
    }
#if 1 //add by jiangdou
	//set  BGRA   PIXEL_FORMAT == GGL_PIXEL_FORMAT_BGRA_8888
	vinfo.red.offset     = 16;
    vinfo.red.length     = 8;
    vinfo.green.offset   = 8;
    vinfo.green.length   = 8;
    vinfo.blue.offset    = 0;
    vinfo.blue.length    = 8;
    vinfo.transp.offset  = 0;
    vinfo.transp.length  = 0;
	
	finfo.line_length=finfo.line_length/2;

#endif 
	
	
	
	
    if( ioctl(fp, FBIOGET_FSCREENINFO, &finfo) ) {
        printf("Error: reading fixed information\n");
        return -1;
    }

    if( ioctl(fp, FBIOGET_VSCREENINFO, &vinfo) ) {
        printf("Error: reading varible information\n");
        return -1;
    }
    screen_size = finfo.smem_len;
    //    screen_size = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
    fbp = (char*) mmap(0, screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fp, 0);

    if( fbp == -1 ){
        printf("Error : fail to map framebuffer\n");
        close(fp);
    }
    for(y = 0 ; y < vinfo.yres; ++y) {  ////背景黑色
        for(x = 0 ; x < vinfo.xres; ++x ) {
            location = x *(vinfo.bits_per_pixel / 8 )  + y * finfo.line_length ;//location为显示区域起始位置
            *(fbp + location) = 0;       //->B      //背景黑色
            *(fbp + location + 1) = 0;   //->G
            *(fbp + location + 2) = 0;   //->R
            *(fbp + location + 3) = 0;   //->A
        }
    
    }
	int startX = vinfo.xres / 2;
    int startY = vinfo.yres / 2;
	 for(x = 0; x < pBmp_image->height; x++) //显示正常
        for( y = 0; y< pBmp_image->width; y++ ) {
            location =(y) *(vinfo.bits_per_pixel / 8 )  + (x) * finfo.line_length;
			//location =(y+startX) *(vinfo.bits_per_pixel / 8 )  + (startY +x) * finfo.line_length;
            if( pBmp_image->data[y + (pBmp_image->height - 1 - x)* pBmp_image->width].r == 255 &&
                    pBmp_image->data[y + (pBmp_image->height - 1 - x)* pBmp_image->width].g == 255 &&
                    pBmp_image->data[y + (pBmp_image->height - 1 - x)* pBmp_image->width].b == 255) {
                continue;
            }

            *(fbp + location) = pBmp_image->data[y + (pBmp_image->height - 1 - x)* pBmp_image->width].r;
            *(fbp + location + 1) = pBmp_image->data[y + (pBmp_image->height - 1 - x)* pBmp_image->width].g;
            *(fbp + location + 2) = pBmp_image->data[y + (pBmp_image->height - 1 - x)* pBmp_image->width].b;
            *(fbp + location + 3) = 0;
        }
	Release_Bmp_Image(&pBmp_image);
	
    //Release_Bmp_Image(&wallpapper);
    munmap(fbp, screen_size);
	sleep(3);
    close(fp);	
	
	return 1;
	//return -1;
} 
