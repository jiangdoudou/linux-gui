/*
* file uart_test.c
* for rk3288 uart test   
* author by jiangdou
* time 2015-04-07   jiangdou_passwd.h
*/

//jiangdou   #################
/*test by */
//#include <jiangdou_passwd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

#include "jiangdou_passwd.h"


/*打开串口*/
int open_port(int comport)
{
	int fd;
	char *dev[]={"/dev/ttyS0","/dev/ttyS1","/dev/ttyS2"};
	long vdisable;
	if (comport==1)//串口 1
	{
		fd = open( "/dev/ttyS3", O_RDWR|O_NOCTTY|O_NDELAY);
	if (-1 == fd){
		perror("Can't Open Serial Port...\n");
		return(-1);
		}
	}
	else if(comport==2)//串口 2
	{
		fd = open( "/dev/ttyS1", O_RDWR|O_NOCTTY|O_NDELAY);
		if (-1 == fd){
			perror("Can't Open Serial Port");
			return(-1);
		}
	}
	else if (comport==3)//串口 3
	{
		fd = open( "/dev/ttyS2", O_RDWR|O_NOCTTY|O_NDELAY);
		if (-1 == fd){
		perror("Can't Open Serial Port");
		return(-1);
		}
	}

/*恢复串口为阻塞状态*/

	if(fcntl(fd, F_SETFL, 0)<0)
		printf("fcntl failed!\n");
	else
		//printf("fcntl=%d\n",fcntl(fd, F_SETFL,0));

	/*测试是否为终端设备*/

	if(isatty(STDIN_FILENO)==0)
		printf("standard input is not a terminal device\n");
	else
		printf("isatty success!\n");
	//printf("fd-open=%d\n",fd);
	return fd;
	//return 1;
}
///////////////////////
//set_opt
//////////////////////
int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop)
{
	struct termios newtio,oldtio;
	/*保存测试现有串口参数设置,在这里如果串口号等出错,会有相关的出错信息*/
	if ( tcgetattr( fd,&oldtio) != 0) {
		perror("SetupSerial 1");
		return -1;
	}
	bzero( &newtio, sizeof( newtio ) );
	/*步骤一,设置字符大小*/
	newtio.c_cflag |= CLOCAL | CREAD;
	newtio.c_cflag &= ~CSIZE;
	/*设置停止位*/
	switch( nBits )
	{
	case 7:
		newtio.c_cflag |= CS7;
		break;
	case 8:
		newtio.c_cflag |= CS8;
		break;
	}
/*设置奇偶校验位*/
	switch( nEvent )
	{
	case 'O': //奇数
		newtio.c_cflag |= PARENB;
		newtio.c_cflag |= PARODD;
		newtio.c_iflag |= (INPCK | ISTRIP);
		break;
	case 'E': //偶数
		newtio.c_iflag |= (INPCK | ISTRIP);
		newtio.c_cflag |= PARENB;
		newtio.c_cflag &= ~PARODD;
		break;
	case 'N': //无奇偶校验位
		newtio.c_cflag &= ~PARENB;
	break;
	}
	/*设置波特率*/
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
	case 460800:
		cfsetispeed(&newtio, B460800);
		cfsetospeed(&newtio, B460800);
		break;
	default:
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
		break;
	}
	/*设置停止位*/
	if( nStop == 1 )
		newtio.c_cflag &= ~CSTOPB;
	else if ( nStop == 2 )
		newtio.c_cflag |= CSTOPB;
	/*设置等待时间和最小接收字符*/
	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VMIN] = 0;
	/*处理未接收字符*/
	tcflush(fd,TCIFLUSH);
	/*激活新配置*/
	if((tcsetattr(fd,TCSANOW,&newtio))!=0)
	{
		perror("com set error");
	return -1;
	}
	//printf("set done!\n");
return 0;
}

//////////////
/*
char bret[10],recebuf[20];
		bret = keyID_sendisturb();//发送start干扰码
		if(bret){
			send_keyID();//发送keyID
			recebuf = receive_key();//接收key
			if(analyze_key(recebuf)){ //analyze ok
				goto go_on;
				
			}else{
				keyID_sendisturb();//发送end干扰码
				show_error_UI();//显示ERROR!
				sleep(2);//延时2S
				rk3288_shut_down();//关机
			}
		   
		}
		
	//char bret[10],recebuf[20];
	//char *pp;
	keyID_parse();//进入加密程序
		

//go_on:  //加密OK  正常启动！！！！！！！！！！！

*/
int uart_fd;


int open_uart()
{
	//int fd = 0;
	int ret;
	if((uart_fd = open_port(1))<0)
	{//打开串口
		perror("open_port error");
		return -1;
	}
	if((ret=set_opt(uart_fd, 115200, 8, 'N', 1))<0){//设置串口
		perror("set_opt error");
	return -1;
	}
	
	return 1;
}

int show_error_UI()//显示ERROR!
{
	
	return -1;
}

int log_error(char *buff)//
{	

	int nwrite;
	//char buff[]="3qvideo.com\n\r";
	nwrite = write(uart_fd,buff,strlen(buff));//写串口
	return -1;
}

int keyID_sendisturb()//发送end干扰码
{	

	int nwrite;
	char buff[]="3qvideo.com\n\r";
	nwrite = write(uart_fd,buff,strlen(buff));//写串口
	return -1;
}
	
int rk3288_shut_down()//关机
{
	keyID_sendisturb();//发送end干扰码
	show_error_UI();//显示ERROR!
	sleep(3);//延时2S
	
	for(;;){  //死循环
		sleep(3);//延时2S  
	}
	
}

char *Passwd_reve()
{
	char buf[80], *tmp;
	int nread;
	 nread = read(uart_fd,buf,strlen(buf));
	 
	 //tmp = buf;
	 tmp = 0x11;
	return tmp;//==OK
	
}

int Passwd_parse()//密码处理
{
	int nwrite;
	char buff[]="DOU";//buff= pawd
	char *pp = 0x11;
	nwrite = write(uart_fd,buff,strlen(buff));//写串口
	if(nwrite){
		rk3288_shut_down();
	}else{
		while(pp == Passwd_reve()){
			
			pp = 0x10;
			return 1; //okok!!!!!!!!!!!!!!!!!!!
			
		}
		
	}
	
	return -1;
}	
	
////bootable/recovery/minui/graphics.c:    fd = open("/dev/graphics/fb0", O_RDWR);
//ui->Print("rebooting...\n");
	
	
	
//add 
int keyID_parse(void)
{
	int ret;
	//sleep(10);
	
	
	ret = open_uart();
	
	
	int nwrite,i;
	char buff0[]="\n\r";
	char buff1[]="uart_test by jiang_dou QQ:344283973\n\r";
	char buff[80];
	nwrite=write(uart_fd,buff0,strlen(buff0));//写串口
	for(i=0; i<5; i++){
		sprintf(buff, "receive data_%d  %s", i, buff1);
		printf("nwrite=%d,send data_%d %s\n",nwrite, i, buff1);
		nwrite=write(uart_fd, buff, strlen(buff));//写串口
		//buff = buff1;
		sleep(1);
	}
	close(uart_fd);
	
	
	show_error_logo();
	sleep(5);
	//for(;;){
		
	//	sleep(1);
	//}
	return 1;
	
	
	
	
	
	//show_logo();
	
	
	return 1;
	
	if(ret < 0){
		rk3288_shut_down();//关机
	}
	ret = Passwd_parse();//密码
	if(ret < 0){
		return -1;
	}
	
	return 0;
	

}	
//mount -o remount,rw rootfs /   重新挂载
//######################################################################################
//*************************显示图片×××××××××××××××××××××××××××××××××××××××××××××××××××××
//system/core/init.c   static int console_init_action(int nargs, char **args)
//if( load_565rle_image(INIT_IMAGE_FILE) ) {
////system/core/init/init.h:#define INIT_IMAGE_FILE	"/initlogo.rle"

//system/core/logo.c  int load_565rle_image(char *fn)
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

int show_error_logo()
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
	
    close(fp);	
	
	return 1;
	//return -1;
} 
 
 
 
//######################################################################################	
	
/*

#if 0
int main(void)
{
	int fd;
	int nwrite,i;
	char buff0[]="\n\r";
	char buff1[]="uart_test by jiang_dou QQ:344283973\n\r";
	char buff[80];
	if((fd=open_port(fd,1))<0)
	{//打开串口
		perror("open_port error");
		return;
	}
	if((i=set_opt(fd,115200,8,'N',1))<0){//设置串口
		perror("set_opt error");
	return;
	}
	//printf("fd=%d\n",fd);
	printf("uart-test, baud is 115200,  is RX\n\r");
	printf("uart test to starting. press 'enter 'key to continue.......\n");
	getchar();
	
	
	nwrite=write(fd,buff0,strlen(buff0));//写串口
	for(i=0; i<2000; i++){
		sprintf(buff, "receive data_%d  %s", i, buff1);
		printf("nwrite=%d,send data_%d %s\n",nwrite, i, buff1);
		nwrite=write(fd, buff, strlen(buff));//写串口
		//buff = buff1;
		sleep(1);
	}
	close(fd);
	return;
}

#endif
*/
