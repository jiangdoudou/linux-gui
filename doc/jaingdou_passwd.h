//http://m.blog.csdn.net/blog/A1w0n/37727971   BGRA 

int keyID_parse(void);
int load_565rle_image(char *fn);




//   A          红色       R        绿色        G       蓝色          B
//0xxx000000   红色（0x00ff0000）、绿色（0x0000ff00）、蓝色（0x000000ff   ->BGRA


// fbfd = open("/dev/graphics/fb0", O_RDWR);

// show_bmp("./logo.bmp");
