#add code  at file android/system/core/init/init.c

      #include "jiangdou_passwd.h"
      //......(略)  
        
        //keyID_parse();//进入加密程序
          //	queue_builtin_action(jiangdou_logo_keyID, "jiangdou");	
          static int jiangdou_logo_keyID(int nargs, char **args)
          {
              keyID_parse();
              return 0;
          }
          
          static int console_init_action(int nargs, char **args)
        
        
  //......(略)
      #if BOOTCHART
          queue_builtin_action(bootchart_init_action, "bootchart_init");
      #endif
      
      #if 1  //add by jiangdou for showbmp_error!!
      
      	//keyID_parse();//进入加密程序
      	queue_builtin_action(jiangdou_logo_keyID, "jiangdou");	
      
      #endif
      
          for(;;) {
              int nr, i, timeout = -1;
      
              execute_one_command();
              restart_processes();
  //......(略)

#add file jiangdou_passwd.c jiangdou_passwd.h

//modify system/core/init/Android.mk

      //#add
      jiangdou_passwd.c
