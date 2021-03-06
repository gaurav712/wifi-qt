#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <QDebug>

#include "wpa_supplicant_control.h"

#define WPA_SEND_CTRL_IFACE_PREFIX "/run/wpa_supplicant/"   // append if_name
#define WPA_RECV_CTRL_IFACE_PREFIX "/tmp/wpa_ctrl_"    // append pid
#define MAX_BUFFER_LEN  1024

#define RECV_TIMEOUT    10000    // in micro-seconds

/* The Constructor */
WPASupplicantControl::WPASupplicantControl(std::string wlan_interface_name)
    :wpa_send_ctrl_iface(WPA_SEND_CTRL_IFACE_PREFIX + QString::fromStdString(wlan_interface_name)),
      wpa_recv_ctrl_iface(
          WPA_RECV_CTRL_IFACE_PREFIX +
          QString::fromStdString(std::to_string(getpid())) +
          "-1"
          )
{
    struct sockaddr_un send_address, recv_address;

    memset(&send_address, 0, sizeof (send_address));
    memset(&recv_address, 0, sizeof (recv_address));

    send_address.sun_family = AF_UNIX;
    recv_address.sun_family = AF_UNIX;

    strncpy(send_address.sun_path, wpa_send_ctrl_iface.toStdString().c_str(), wpa_send_ctrl_iface.length());
    strncpy(recv_address.sun_path, wpa_recv_ctrl_iface.toStdString().c_str(), wpa_recv_ctrl_iface.length());

    if ((wpa_control_socket = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
        qCritical() << "socket() failed!";
        exit(EXIT_FAILURE);
    }

    /* Attatch to sending and receiving control interfaces */
    if (::connect(wpa_control_socket, (const struct sockaddr *)&send_address, sizeof(struct sockaddr_un)) == -1) {
        qCritical() << "Error connecting to wpa_supplicant send control iface!";
        close(wpa_control_socket);
        exit(EXIT_FAILURE);
    }

    /* Detach if it's already bound */
    unlink(recv_address.sun_path);

    if (::bind(wpa_control_socket, (const struct sockaddr *)&recv_address, sizeof(struct sockaddr_un)) == -1) {
        qCritical() << "Error binding to wpa_supplicant recv control iface!";
        close(wpa_control_socket);
        exit(EXIT_FAILURE);
    }

    /* issue the "ATTACH" command to wpa_supplicant to be able to receive events */
    this->send_cmd("ATTACH");
    if (!(this->get_response()).startsWith("OK\n")) {
        qCritical() << "Error initializing event listener!";
    }
}

/* Destructor */
WPASupplicantControl::~WPASupplicantControl() {
    unlink(wpa_recv_ctrl_iface.toStdString().c_str());  // remove the file for receiving socket
    close(wpa_control_socket);
}

/* Send Commands to wpa_supplicant */
void WPASupplicantControl::send_cmd(QString cmd) {
    if(send(wpa_control_socket, cmd.toStdString().c_str(), cmd.length(), 0) < 0) {
        qCritical() << "Error sending command:" << cmd;
    }
}

/* Get responses from commands issued to wpa_supplicant */
QString WPASupplicantControl::get_response(bool logging) {

    fd_set readfd;
    int result;
    struct timeval timeout;
    char buffer[MAX_BUFFER_LEN + 1];    // for the '\0'

    timeout.tv_sec = 0;
    timeout.tv_usec = RECV_TIMEOUT;

    FD_ZERO(&readfd);
    FD_SET(wpa_control_socket, &readfd);

    if((result = select(wpa_control_socket + 1, &readfd, NULL, NULL, &timeout)) < 0) {
        if(logging)
            qCritical() << "Socket descriptor has no data to read!";
        return QString::fromStdString("");
    } else if(result == 0) {
        if(logging)
            qCritical() << "Timeout waiting for the socket descriptor!";
        return QString::fromStdString("");
    }

    if(FD_ISSET(wpa_control_socket, &readfd)) {
        if((recv(wpa_control_socket, buffer, MAX_BUFFER_LEN, 0)) < 0) {
            if(logging)
                qCritical() << "Failed to read from socket descriptor!";
            return QString::fromStdString("");
        } else {
            return QString::fromStdString(buffer);
        }
    }

    return QString::fromStdString("");
}

/* Spawn a new thread and scan for available networks */
void WPASupplicantControl::scan_for_networks() {

    /* Empty the networks list if it has elements */
    for(int index = (networks.size() - 1); index >= 0; index--) {
        networks.removeAt(index);
    }

    InitiateSearchThread *initiateSearchThread = new InitiateSearchThread(this);
    initiateSearchThread->start();

    QObject::connect(initiateSearchThread, &InitiateSearchThread::resultReady,
                     this, &WPASupplicantControl::process_networks_list);
    QObject::connect(initiateSearchThread, &InitiateSearchThread::finished,
                     initiateSearchThread, &QObject::deleteLater);
}

void WPASupplicantControl::process_networks_list(const QStringList &networks_list) {
    for(int index = 0; index < networks_list.size(); index++)
        networks.append(networks_list[index]);
}

/* Constructor for the search thread */
InitiateSearchThread::InitiateSearchThread(WPASupplicantControl *wpaSupplicantControl)
    :wpaSupplicantControl(wpaSupplicantControl)
{}

/* run() for the searching thread */
void InitiateSearchThread::run() {

    /* Issue the "SCAN" command */
    wpaSupplicantControl->send_cmd("SCAN");

    if(!wpaSupplicantControl->get_response().startsWith("OK\n")) {
        qCritical() << "Failed to issue scan command!";
        return;
    }

    /* Now wait for the "SCAN_RESULTS" event */

    do {
        if(wpaSupplicantControl->get_response(false).contains("CTRL-EVENT-SCAN-RESULTS", Qt::CaseSensitive)) {
            break;
        }
        this->msleep(100);
    } while (true);

    /* Now get the scan results */
    wpaSupplicantControl->send_cmd("SCAN_RESULTS");
    emit resultReady(this->format_networks_list(wpaSupplicantControl->get_response()));
}

QStringList InitiateSearchThread::format_networks_list(QString scan_results) {

    QStringList networks_list, temp;

    temp = scan_results.split('\n');

    /* TO remove the top header line and the blank one at last */
    for(int index = 1; index < (temp.size() - 1); index++)
        networks_list.append(temp[index]);

    return networks_list;
}
