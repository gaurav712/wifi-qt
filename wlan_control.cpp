/*
 * This uses a daemon called "wlan_toggle_service" to toggle wifi
 *
 * It is here at: https://github.com/gaurav712/wlan_toggle_service
 */

#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <QDebug>

#define HOST    "127.0.0.1"
#define PORT    14465

#include "wlan_control.h"

void toggle_wlan(int mode) {

    int client_socket;
    char toggle_cmd[2];
    struct sockaddr_in server_address;

    toggle_cmd[0] = mode + '0';
    toggle_cmd[1] = '\0';

    if((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        qCritical() << "socket() error!";
        return;
    }

    /* Pad with zeroes */
    memset(server_address.sin_zero, '\0', sizeof(server_address.sin_zero));

    /* Set-up the address to send commands */
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(HOST);
    server_address.sin_port = htons(PORT);

    if(connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        qCritical() << "connect() failed!";
        return;
    }

    /* Issue the command */
    send(client_socket, toggle_cmd, 2, 0);

    shutdown(client_socket, SHUT_RDWR);
}
