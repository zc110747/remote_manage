#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>

#define I2C_DRIVER_NAME "/dev/ap3216"	

int main(int argc, char *argv[])
{
	int fd;
	unsigned short databuf[3];
	unsigned short ir, als, ps;
	int nSize = 0;

	fd = open(I2C_DRIVER_NAME, O_RDWR);
	if(fd < 0) 
	{
		printf("can't open file %s\r\n", I2C_DRIVER_NAME);
		return -1;
	}

	while (1)
	{
		nSize = read(fd, databuf, sizeof(databuf));
		if(nSize >= 0)
		{ 			/* 数据读取成功 */
			ir =  databuf[0]; 	/* ir传感器数据 */
			als = databuf[1]; 	/* als传感器数据 */
			ps =  databuf[2]; 	/* ps传感器数据 */
			printf("read size:%d\r\n", nSize);
			printf("ir = %d, als = %d, ps = %d\r\n", ir, als, ps);
		}
		usleep(200000); /*100ms */
	}
	close(fd);	/* 关闭文件 */	
	return 0;
}

