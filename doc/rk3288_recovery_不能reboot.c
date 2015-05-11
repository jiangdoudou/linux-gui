/bootable/recovery/recovery.cpp:    ui->Print("Rebooting...\n");//jiangdou ,Rebooting...

.....(略)
  
      ui->Print("Rebooting...\n");//jiangdou ,Rebooting...
      //property_set(ANDROID_RB_PROPERTY, "reboot,");
  	android_reboot(ANDROID_RB_RESTART, 0, 0);
      return EXIT_SUCCESS;
  }
.....(略)

android_reboot（）  -->>

/system/core/libcutils/android_reboot.c:int android_reboot(int cmd, int flags, char *arg)
