
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>

/**
 * 测试LED工作
 * 
 * @param NULL
 *
 * @return NULL
 */
int main(int argc, const char *argv[])
{
    unsigned char val = 1;
    int fd;

    fd = open("/dev/beep", O_RDWR | O_NDELAY);
    if(fd == -1)
    {
        printf("/dev/beep open error");
        return -1;
    }

    if(argc > 1){   
        val = atoi(argv[1]);
    }

    write(fd, &val, 1);

    close(fd); 

    exit(0);  
}
