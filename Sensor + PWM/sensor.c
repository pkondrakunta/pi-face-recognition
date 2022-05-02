#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "camera.h"
#include "pwm_out.h"
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>


#define I2C_ADDR 0x10
char buffer[1];
char pwm=0;
//Delete the face_match variable after inserting face recognition function
char face_match=1;

int getPWMStatus(void);
int sensorStart(void);
int getSensorValue(void);
int sensorRead(char* buffer);

//Check if the I2C is configured well 
int sensorStart(void) {
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
	return sensorRead(buffer);
}


int getSensorValue(void) {
    return buffer[0];
}

int getPWMStatus() {
	return pwm;
}

int sensorRead(char* buffer) {
	int fd = open ("/dev/i2c-1",O_RDWR);
	ioctl(fd,I2C_SLAVE,I2C_ADDR);
	int i=0;
	buffer[0] = 0b00000010;

	for (;;) {
    		write(fd, buffer, 1);

    		read(fd, buffer, 1);
    		printf("0x%02X\n", buffer[0]);
		if (buffer[0] ==1) {
			char filename[] = "/home/pi/Documents/TELE6550/TestPj/VL53L0X-master/Image%d.jpg",i;
			startPic(filename, "-vf");
			fflush(stdout);
			sleep(5);
			stopPic();
			printf("Image successful!\n");

			//Replace face_match with the face recognition function
			if(face_match){
				pwm = 1;
				printf("Door opened!\n");
				int j = door_open();
				sleep(2);
				if(j) {
					printf("Door closed\n");
				}
				pwm = 0;
			}
			i++;			
		}
		usleep(100000);
	}

	return 1;
}





