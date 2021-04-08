#include "configenv.h"

using namespace std;

int createDir(const char *sPathName)  
{
    char DirName[256];
    strcpy(DirName, sPathName);
    int i,len = strlen(DirName);

    for(i = 1; i < len; i++) {
        if(DirName[i]=='/') {
            DirName[i] = 0;
            if(access(DirName, 0) != 0) {
                printf("%s: %s not exsit \n",__func__,DirName);
                if(mkdir(DirName, 0755) == -1)
                {
                    printf("mkdir error\n");
                    return -1;   
                }
                printf("%s: %s create dir successed \n",__func__,DirName);
            }
            DirName[i] = '/';
        }
    }
    return 0;  
}

int writeFile(const char *file_name, const char *contents, bool append)
{
    int fd,len;

    fd = open(file_name, O_CREAT | O_TRUNC | O_RDWR, 0600);
    if(fd < 0) {
      printf("open file failed\n");
      return -1;
    }

    if(append) {
        lseek(fd, 0, SEEK_END);
        len = strlen(contents);
        if(write(fd, contents, len) == -1) {
            printf("append file failed\n");
            close(fd);
            return -1;
        }
    } else {
        len = strlen(contents);
        if(write(fd, contents, len) == -1) {
           printf("write file failed\n");
            close(fd);
            return -1;
        }
    }
    
    close(fd);
    return 0;
}

int setCameraEnv()
{
    int ret;
    string setting_str = "IFEDualClockThreshold=600000000\nmaxHalRequests=8";

    string setting_file = "/etc/camera/camxoverridesettings.txt";
    string display_file = "/data/misc/display/gbm_dbg_cfg.txt";

    ret = createDir("/etc/camera/") | createDir("/data/misc/display/");
    //ret = createDir("/data/detection/") | createDir("/data/detection/img/");
    ret |= writeFile(setting_file.data(),setting_str.data(),false);
    ret |= writeFile(display_file.data(),"0",false);

    return ret;
}