#ifndef RFKILL_CONTROL_H
#define RFKILL_CONTROL_H

#endif // RFKILL_CONTROL_H

#include <iostream>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/rfkill.h>

void toggle_wlan(int mode);
