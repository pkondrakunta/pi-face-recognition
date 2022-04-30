#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define I2C_ADDR 0x10                //STm32 Slave address

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
/*
Write to STm32 receiver buffer 0x02
Once the receiver buffer gets this value, it is a trigger
to start transmitting and thereafter it resets its receiver 
buffer to 0
*/


void tryRead() {
	int fd = open ("/dev/i2c-1",O_RDWR);
	ioctl(fd,I2C_SLAVE,I2C_ADDR);
	char buffer[1];
	buffer[0] = 0b00000010;
	for (;;) {
    		write(fd, buffer, 1);
    		read(fd, buffer, 1);
    		printf("0x%02X\n", buffer[0]);
		    usleep(100000);
	}
}


int main (void) {
	
	sensorStart();
	tryRead();	

return 0;

}


