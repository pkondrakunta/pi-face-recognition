#include <stdio.h>
#include <string.h>
#include <unistd.h>

int door_open() {
//int main(int argc, char** argv) {
	int gpio = 5;
	char buf[100];

	FILE* fd = fopen("/sys/class/gpio/export","w");
	fprintf(fd, "%d",gpio);
	fclose(fd);

	sprintf(buf, "/sys/class/gpio/gpio%d/direction",gpio);
	fd = fopen(buf,"w");
	fprintf(fd, "out");
	fclose(fd);

	sprintf(buf, "/sys/class/gpio/gpio%d/value",gpio);
	fd = fopen(buf,"w");

	for(int i=0;i<20000;i++) {
		fd = fopen(buf,"w");
		fprintf(fd,"1");
		fclose(fd);
		fd = fopen(buf, "w");
		fprintf(fd,"0");
		fclose(fd);
		usleep(100);
	}
	return 1;
}

			
