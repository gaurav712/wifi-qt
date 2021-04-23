#include "wpa_supplicant_control.h"

#define WPA_SEND_CTRL_IFACE_PREFIX "/run/wpa_supplicant/"   // append if_name
#define WPA_RECV_CTRL_IFACE_PREFIX "/tmp/wpa_ctrl_"    // append pid
#define MAX_BUFFER_LEN  1024

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

    strncpy(send_address.sun_path, wpa_send_ctrl_iface.toStdString().c_str(), sizeof(send_address.sun_path));
    strncpy(recv_address.sun_path, wpa_recv_ctrl_iface.toStdString().c_str(), sizeof(recv_address.sun_path));

    if ((wpa_control_socket = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
        qCritical() << "socket() failed!";
        exit(EXIT_FAILURE);
    }

    /* Attatch to sending and receiving control interfaces */
    if (connect(wpa_control_socket, (const struct sockaddr *)&send_address, sizeof(struct sockaddr_un)) == -1) {
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
}

/* Destructor */
WPASupplicantControl::~WPASupplicantControl() {
    unlink(wpa_recv_ctrl_iface.toStdString().c_str());  // remove the file for receiving socket
    close(wpa_control_socket);
}

/* Send Commands to wpa_supplicant */
void WPASupplicantControl::send_cmd(QString cmd) {
    send(wpa_control_socket, cmd.toStdString().c_str(), cmd.length(), 0);
}

/* Get responses from commands issued to wpa_supplicant */
QString WPASupplicantControl::get_response() {
    char buffer[MAX_BUFFER_LEN + 1];    // for the '\0'

    recv(wpa_control_socket, buffer, MAX_BUFFER_LEN, 0);

    return QString::fromStdString(buffer);
}

/* Spawn a new thread and scan for available networks */
void WPASupplicantControl::scan_for_networks() {
    InitiateSearchThread *initiateSearchThread = new InitiateSearchThread();
    initiateSearchThread->start();

    QObject::connect(initiateSearchThread, &InitiateSearchThread::finished, initiateSearchThread, &QObject::deleteLater);
}

/* run() for the searching thread */
void InitiateSearchThread::run() {
    for(int i = 0; i < 3; i++) {
        qInfo() << "Hey there! I am the new thread.";
        sleep(1);
    }
}
