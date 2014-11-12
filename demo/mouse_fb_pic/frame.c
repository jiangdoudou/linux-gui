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
#include <sys/msg.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/shm.h>
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
        return NULL;
    }
    rc = fread( &FileHead, 1, sizeof(BITMAPFILEHEADER), fp );
    if ( rc != sizeof( BITMAPFILEHEADER ) ) {
        fclose( fp );
        return NULL;
    }

    if (memcmp(FileHead.cfType, "BM", 2) != 0) {
        fclose( fp );
        return NULL;
    }

    rc = fread( (char *)&InfoHead, 1, sizeof(BITMAPINFOHEADER), fp );
    if ( rc != sizeof(BITMAPINFOHEADER) ) {
        fclose( fp );
        return NULL;
    }
    width = InfoHead.ciWidth;
    height = InfoHead.ciHeight;
    bit_count = InfoHead.ciBitCount;

    printf("%d %d %d\n", width, height, bit_count);
    if( bit_count != 24) {
        return NULL;
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
int main(int argc, char **argv ) {
    BMP_IMAGE* pBmp_image;
    pBmp_image = ReadBmp2Buf("./mouse.bmp");

    BMP_IMAGE* wallpapper;
    wallpapper = ReadBmp2Buf("./wallpapper.bmp");

    int fp = 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;

    long screen_size = 0;
    char *fbp = NULL;
    int x= 0, y= 0;
    long location = 0;

    float vx = 0.0f,vy = 0.0f;
    fp = open("/dev/fb0", O_RDWR );

    if(fp < 0 ) {
        printf("Error: Can not open framebuffer!\n");
        exit(-1);
    }

    if( ioctl(fp, FBIOGET_FSCREENINFO, &finfo) ) {
        printf("Error: reading fixed information\n");
        exit(-1);
    }

    if( ioctl(fp, FBIOGET_VSCREENINFO, &vinfo) ) {
        printf("Error: reading varible information\n");
        exit(-1);
    }
    screen_size = finfo.smem_len;
    //    screen_size = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
    fbp = (char*) mmap(0, screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fp, 0);

    if( fbp == -1 ){
        printf("Error : fail to map framebuffer\n");
        close(fp);
    }
    for(y = 0 ; y < vinfo.yres; ++y) {
        for(x = 0 ; x < vinfo.xres; ++x ) {
            location = x *(vinfo.bits_per_pixel / 8 )  + y * finfo.line_length ;
            *(fbp + location) = 255;
            *(fbp + location + 1) = 255;
            *(fbp + location + 2) = 255;
            *(fbp + location + 3) = 0;
        }
    
    }

  for(x = 0; x < wallpapper->height; x++) 
        for( y = 0; y< wallpapper->width; y++ ) {
            location =(y) *(vinfo.bits_per_pixel / 8 )  + (x) * finfo.line_length;
            *(fbp + location) = wallpapper->data[y + (wallpapper->height - 1 - x)* wallpapper->width].b;
            *(fbp + location + 1) = wallpapper->data[y + (wallpapper->height - 1 - x)* wallpapper->width].g;
            *(fbp + location + 2) = wallpapper->data[y + (wallpapper->height - 1 - x)* wallpapper->width].r;
            *(fbp + location + 3) = 0;
        }


    int startX = vinfo.xres / 2;
    int startY = vinfo.yres / 2;

    int fd;
    char buffer_mouse[6];
    fd = open("/dev/input/mice", O_RDONLY);
    if( fd < 0 ) {
        printf("Error : fail to open mice\n");
        exit(-1);
    }

    RGB buffer[pBmp_image->width * pBmp_image->height];

    //init mouse 
    for(x = 0; x < pBmp_image->height; x++ ) 
        for( y= 0; y < pBmp_image->width; y++) {
            location =(y+startX) *(vinfo.bits_per_pixel / 8 )  + (startY +x) * finfo.line_length ;
            buffer[y +  x* pBmp_image->width].r = *(fbp + location);
            buffer[y +  x* pBmp_image->width].g = *(fbp + location + 1 );
            buffer[y +  x* pBmp_image->width].b = *(fbp + location + 2 );
        }

    for(x = 0; x < pBmp_image->height; x++) 
        for( y = 0; y< pBmp_image->width; y++ ) {
            location =(y+startX) *(vinfo.bits_per_pixel / 8 )  + (startY +x) * finfo.line_length;
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
    int pre_X = startX,pre_Y = startY;
    int next_X = startX,next_Y = startY;

    fd_set rset;
    int retval = -1;
    FD_ZERO(&rset);
    while(1) {
        FD_SET(0,&rset);
        FD_SET(fd,&rset);
        retval = select(fd + 1, &rset, NULL,NULL ,NULL);
        switch(retval) {
            case -1 :
                printf("select error!\n");
                exit(-1);
            default:
                if( FD_ISSET(fd, &rset) ) {
                    if( read(fd, buffer_mouse, sizeof(buffer_mouse) ) <= 0 ) {
                        printf("Error : fail to read\n");
                        continue;
                    }
                    //TODO: when the cursor is the bottom of the screen, some detail will be considered
                    if(pre_X + buffer_mouse[1] >= vinfo.xres ) buffer_mouse[1] = 0;
                    if(pre_Y - buffer_mouse[2] + pBmp_image->height  >= vinfo.yres ) buffer_mouse[2] = 0;
                    if(pre_X + buffer_mouse[1] < 0 ) buffer_mouse[1] = 0;
                    if(pre_Y - buffer_mouse[2] < 0 ) buffer_mouse[2] = 0;
                   
                    next_X = pre_X + buffer_mouse[1];
                    next_Y = pre_Y - buffer_mouse[2];

                    for(x = 0; x < pBmp_image->height; x++ ) 
                        for( y= 0; y < pBmp_image->width; y++) {
                            //copy buffer to pre place
                            location =(y+ pre_X) *(vinfo.bits_per_pixel / 8 )  + (pre_Y +x) * finfo.line_length;

                            *(fbp + location) =   buffer[y +  x* pBmp_image->width].r;  
                            *(fbp + location + 1) =   buffer[y +  x* pBmp_image->width].g; 
                            *(fbp + location + 2) =   buffer[y +  x* pBmp_image->width].b;
                            *(fbp + location + 3) = 0;
                        }

                    for(x = 0; x < pBmp_image->height; x++ ) 
                        for( y= 0; y < pBmp_image->width; y++) {
                            location =(y+next_X) *(vinfo.bits_per_pixel / 8 )  + (next_Y +x) * finfo.line_length ;
                            //copy new place to buffer   
                            buffer[y +  x* pBmp_image->width].r = *(fbp + location);
                            buffer[y +  x* pBmp_image->width].g = *(fbp + location + 1 );
                            buffer[y +  x* pBmp_image->width].b = *(fbp + location + 2 );
                        }


                    for(x = 0; x < pBmp_image->height; x++ ) 
                        for( y= 0; y < pBmp_image->width; y++) {
                            location =(y+next_X) *(vinfo.bits_per_pixel / 8 )  + (next_Y +x) * finfo.line_length ;
                            if( pBmp_image->data[y + (pBmp_image->height - 1 - x)* pBmp_image->width].r == 255 &&
                                    pBmp_image->data[y + (pBmp_image->height - 1 - x)* pBmp_image->width].g == 255 &&
                                    pBmp_image->data[y + (pBmp_image->height - 1 - x)* pBmp_image->width].b == 255) {
                                continue;
                            }

                            //copy the cursor to new place 
                            *(fbp + location) = pBmp_image->data[y + (pBmp_image->height - 1 - x)* pBmp_image->width].r;
                            *(fbp + location + 1) = pBmp_image->data[y + (pBmp_image->height - 1 - x)* pBmp_image->width].g;
                            *(fbp + location + 2) = pBmp_image->data[y + (pBmp_image->height - 1 - x)* pBmp_image->width].b;
                            *(fbp + location + 3) = 0;
                        }
                    pre_X = next_X;
                    pre_Y = next_Y;
                }
        }
    }
    close(fd);
    Release_Bmp_Image(&pBmp_image);
    Release_Bmp_Image(&wallpapper);
    munmap(fbp, screen_size);
    close(fp);
    return 1;
}
