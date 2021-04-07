#include <iostream>
#include <fstream>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <linux/wireless.h>
#include <sys/ioctl.h>

class WlanInfo {
private:
    std::string wlan_interface_name;

    bool interface_is_wireless(void);

public:

    // The Constructor
    WlanInfo(void);

    bool wlan_is_on(void);
    bool wlan_is_connected(void);
};
