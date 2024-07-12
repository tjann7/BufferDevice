#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
	int dev = open("/dev/first_device", O_RDONLY);
	if (dev < 0) {
		printf("Opening not successful!\n");
		return dev;
	}
	printf("Opening successful!\n");
	close(dev);
	return 0;
}

