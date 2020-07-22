
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>

#define KEY_DEV_NAME "/dev/key"

int main(int argc, const char *argv[])
{
    unsigned char val = 1;
    int fd;

    fd = open(KEY_DEV_NAME, O_RDWR | O_NDELAY);
    if(fd != -1){
        printf("%s open error", KEY_DEV_NAME);
        return -1;
    }
    else{
        do{
            ssize_t len;

            len = read(fd, &val, 1);
            if(len >= 0){
                printf("key input:%d\r\n", val);
                sleep(1);
            }        
            else{
                printf("read failed\n");
                break;
            }
        } while(1);
    }

    close(fd);
    return 0;
}
