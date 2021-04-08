#ifndef WLAN_CONTROL_H
#define WLAN_CONTROL_H

#endif // WLAN_CONTROL_H

#include <iostream>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define HOST    "127.0.0.1"
#define PORT    14465

void toggle_wlan(int mode);