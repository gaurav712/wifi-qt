/*
 * This uses a daemon called "priviledged_actions_service" to spawn specified commands
 *
 * It is here at: https://github.com/gaurav712/priviledged_actions_service
 */

#include "wlan_control.h"

std::string wpa_supplicant_cmd = "wpa_supplicant -i interface -D nl80211 -c config -B";

void toggle_wlan(int mode) {

    std::string toggle_cmd = "toggle ";

    int client_socket;
    struct sockaddr_in server_address;

    if((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "socket() error!" << std::endl;
        return;
    }

    /* Pad with zeroes */
    memset(server_address.sin_zero, '\0', sizeof(server_address.sin_zero));

    /* Set-up the address to send commands */
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(HOST);
    server_address.sin_port = htons(PORT);

    if(connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "connect() failed!" << std::endl;
        return;
    }

    /* Append mode to the toggle command */
    toggle_cmd = toggle_cmd + std::to_string(mode);

    send(client_socket, toggle_cmd.c_str(), toggle_cmd.length(), 0);

    shutdown(client_socket, SHUT_RDWR);
}
