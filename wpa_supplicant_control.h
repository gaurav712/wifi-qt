#ifndef WPASUPPLICANTCONTROL_H
#define WPASUPPLICANTCONTROL_H

#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <QString>
#include <QDebug>
#include <QThread>

class WPASupplicantControl
{
private:
    int wpa_control_socket;
    QString wpa_send_ctrl_iface;    // to issue commands to wpa_supplicant
    QString wpa_recv_ctrl_iface;    // to receive wpa_supplicant's response

public:
    WPASupplicantControl(std::string wlan_interface_name);
    ~WPASupplicantControl();
    void send_cmd(QString cmd);
    void scan_for_networks();
    QString get_response();
};

class InitiateSearchThread: public QThread
{
    Q_OBJECT
private:
    void run();
signals:
    void resultReady(const QStringList &networksList);
};

#endif // WPASUPPLICANTCONTROL_H
