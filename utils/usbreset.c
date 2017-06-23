#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <linux/usbdevice_fs.h>

int main(int argc, char *argv[]) {
    int fd;

    if (argc != 2) {
        fprintf(stderr, "Usage: usbreset device-filename\n");
        return 1;
    }

    if ((fd = open(argv[1], O_WRONLY)) == -1) {
        perror(argv[1]);
        return 1;
    }

    printf("Resetting USB device %s\n", argv[1]);
    if (ioctl(fd, USBDEVFS_RESET, 0) == -1) {
        perror(argv[0]);
        goto error;
    }

    printf("Reset successful\n");
    close(fd);
    return 0;

error:
    close(fd);
    return 1;
}
