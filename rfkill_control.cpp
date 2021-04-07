#include "rfkill_control.h"

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
