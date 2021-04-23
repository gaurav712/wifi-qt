#include <iostream>

#ifndef WLANINFO_H
#define WLANINFO_H

class WlanInfo {
private:
    bool interface_is_wireless(void);

public:
    std::string wlan_interface_name;

    // The Constructor
    WlanInfo(void);

    bool wlan_is_on(void);
    bool wlan_is_connected(void);
};

#endif // WLANINFO_H
