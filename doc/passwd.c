#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
int main()
{
	unsigned int i, reve_id;
	unsigned int a;
	unsigned char x[2];
	unsigned int key_id;
	
	char buf[20];
	srand((int)time(0));
	for(;;){
		i = rand();//取随机数
		//i = i >> 8;
		//printf("i =%x  asize = %d\n", i, sizeof(i));
		x[0] = (char)((i >> 8) & 0xff);//16bit_height
		printf("x[0] =%x  xsize = %d\n", x[0], sizeof(x[0]));
		x[1] = (char)((i >> 24) & 0xff);//16bit_height
		printf("x[1] =%x  xsize = %d\n", x[1], sizeof(x[1]));
		sprintf(buf, "jiangdou:%d%d", x[0], x[1]);//sprintf(s, "", ,)
		
		//itoa(b,buf,10);
		printf("xbuf = %s \n",buf);//buf =string
		
		a = ((x[1] << 8) | x[0]) & 0xffff;//for KEY_ID
		
		printf("a =%x  asize = %d\n", a, sizeof(a));
		//send_id(a);
		 key_id = (a * 2) - 3;//for KEY_ID at MCU!!!!
		printf("key_id = %x \n",key_id);
		//unsigned int reve_id = get();
		reve_id = (key_id + 3)/2;
		printf("reve_id = %x \n",reve_id);
		//t = get()
		sleep(1);
		if(a == reve_id){
			printf("passwd ok!!!!\n");
		}
	}
}
