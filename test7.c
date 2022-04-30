#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "camera.h"
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
//#include <i2c/smbus.h>

#define I2C_ADDR 0x10
volatile int sensorRate;

//int getSensorValue(void);
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
void tryRead() {
	int fd = open ("/dev/i2c-1",O_RDWR);
	ioctl(fd,I2C_SLAVE,I2C_ADDR);
	char buffer[1];
	int i=0;
	//char filename[] = "/home/pi/Documents/TELE6550/TestPj/VL53L0X-master/Image%d.h264";
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
			sleep(5);
			i++;
			printf("Value of i is %d\n",i);			
		}
		usleep(100000);
	}
//organize this!
// char filename[] = "/home/pi/Documents/TELE6550/TestPj/VL53L0X-master.h264";

  //  printf("Recording video for 5 secs...");
    // example options give an upside-down black and white video
   // startVideo(filename, "-cfx 128:128 -rot 180"); 
   // fflush(stdout);
   // sleep(5);
   // stopVideo();
   // printf("\nVideo stopped - exiting in 2 secs.\n");
   // sleep(2);
	
	
}

//int getSensorValue(void) {
   // int iDistance;
	//iDistance = tofReadDistance(); 
	//return iDistance;
//}

int main (void) {
	//int i;
	//setSensorRate(20);
	sensorStart();
	tryRead();	

return 0;

}


