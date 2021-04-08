#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/rfkill.h>

void toggle_wlan(int mode) {

    int fd;
    struct rfkill_event event;

    if ((fd = open("/dev/rfkill", O_RDWR)) < 1) {
        std::cerr << "Can't open RFKILL control device" << std::endl;
        std::exit(1);
    }

    memset(&event, 0, sizeof(event));
    event.op = RFKILL_OP_CHANGE_ALL;
    event.type = RFKILL_TYPE_WLAN;
    event.soft = mode;

    if ((write(fd, &event, sizeof(event))) < 0) {
        std::cerr << "Failed to change RFKILL state" << std::endl;
    }

    close(fd);
}


int main(int argc, char *argv[]) {
    printf("running with mode %d\n", atoi(argv[1]));
    toggle_wlan(atoi(argv[1]));
    return 0;
}
