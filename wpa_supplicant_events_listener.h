#ifndef WPASUPPLICANTEVENTSLISTENER_H
#define WPASUPPLICANTEVENTSLISTENER_H

#include <QProcess>

class WPASupplicantEventsListener
{
private:
    QStringList arguments_list = {
      "-i", // for the interface
    };
    QProcess wpa_cli_process;

public:
    WPASupplicantEventsListener(std::string wlan_interface_name);
    ~WPASupplicantEventsListener();
    void terminate();
    QString read_output();
};

#endif // WPASUPPLICANTEVENTSLISTENER_H
