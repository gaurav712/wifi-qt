#ifndef WPASUPPLICANTCONTROL_H
#define WPASUPPLICANTCONTROL_H

#include <QThread>

class WPASupplicantControl: public QObject
{
    Q_OBJECT
private:
    int wpa_control_socket;
    QString wpa_send_ctrl_iface;    // to issue commands to wpa_supplicant
    QString wpa_recv_ctrl_iface;    // to receive wpa_supplicant's response

public slots:
    void process_networks_list(const QStringList &networks_list);

public:
    QStringList networks;

    WPASupplicantControl(std::string wlan_interface_name);
    ~WPASupplicantControl();
    void send_cmd(QString cmd);
    void scan_for_networks();
    QString get_response(bool logging = true);
};

/* The network scan thread class */
class InitiateSearchThread: public QThread
{
    Q_OBJECT
public:
    WPASupplicantControl *wpaSupplicantControl;
    InitiateSearchThread(WPASupplicantControl *wpaSupplicantControl);
private:
    void run();
    QStringList format_networks_list(QString scan_results);
signals:
    void resultReady(const QStringList &networks_list);
};

#endif // WPASUPPLICANTCONTROL_H
