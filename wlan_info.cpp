#include "wlan_info.h"

WlanInfo::WlanInfo(void) {
    struct ifaddrs *ifad;
    std::string interface_name;

    if(getifaddrs(&ifad) == -1) {
        std::cerr << "getifaddrs() failed!" << std::endl;
        std::exit(1);
    }

    while (ifad->ifa_next) {

        // Update wlan_interface_name with the wifi interface

        wlan_interface_name = ifad->ifa_name;

        if(interface_is_wireless()) {
            return;
        }

        ifad = ifad->ifa_next;
    }
}

/*
 * This function is mostly C-Styled as all the default linux APIs are in C.
 */
bool WlanInfo::interface_is_wireless(void) {

    int sock = -1;
    struct iwreq iw;
    char protocol[IFNAMSIZ];

    memset(&iw, 0, sizeof (iw));
    strncpy(iw.ifr_name, wlan_interface_name.c_str(), IFNAMSIZ);

    if((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        std::cerr << "socket() failed!" << std::endl;
        std::exit(1);
    }

    if(ioctl(sock, SIOCGIWNAME, &iw) != -1) {
        strncpy(protocol, iw.u.name, IFNAMSIZ);
        close(sock);
        return true;
    }

    return false;
}

bool WlanInfo::wlan_is_on(void) {

    struct ifreq ifr;

    int sock = socket(PF_INET6, SOCK_DGRAM, 0);
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, wlan_interface_name.c_str(), IFNAMSIZ);

    if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0) {
        std::cerr << "ioctl(.., SIOCGIFFLAGS, ..) failed!" << std::endl;
        std::exit(1);
    }

    close(sock);
    return !!(ifr.ifr_flags & IFF_UP);
}

bool WlanInfo::wlan_is_connected(void) {

    struct ifreq ifr;

    int sock = socket(PF_INET6, SOCK_DGRAM, 0);
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, wlan_interface_name.c_str(), IFNAMSIZ);

    if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0) {
        std::cerr << "ioctl(.., SIOCGIFFLAGS, ..) failed!" << std::endl;
        std::exit(1);
    }

    close(sock);
    return !!(ifr.ifr_flags & IFF_RUNNING);
}
