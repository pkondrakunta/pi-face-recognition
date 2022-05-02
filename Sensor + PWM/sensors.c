#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "tof.h"
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define I2C_ADDR 0x29
volatile int sensorRate;
void setsensorRate(const int rate);
int sensorStart();
int getSensorValue();

void setSensorRate(const int rate) {
	sensorRate = rate*60;
} 

int sensorStart() {
	int fd,status,value=0;
	fd = open ("/dev/i2c-1",O_RDWR);
	if (fd<0) {
		printf("Error opening! Maybe try sudo?");
		return 0;
	}
	printf("Open Success!\n");
	status=ioctl(fd,I2C_SLAVE,I2C_ADDR);
	if (status<0){
		printf("Error addressing!");
		close(fd);
		fd = -1;
		return 0;
	}
	printf("Addressing success!\n");
	printf("Initialization successful!\n");
	return 1;
}

int getSensorValue(void) {
    int iDistance;
	iDistance = tofReadDistance(); 
	return iDistance;
}

int main (void) {
	int i;
	setSensorRate(20);
	if(!sensorStart()) return -1;
	i=tofInit(1,I2C_ADDR,1);
	for (i=0; i<sensorRate; i++) // read values at a set rate times per second
	{
		if (getSensorValue()<4096 ) // get valid output
			printf("%d\n", getSensorValue());
		usleep(50000); // 50ms
	}
return 0;

}


