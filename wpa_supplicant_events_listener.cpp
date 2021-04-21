/*
 * Uses wpa_cli as the mediator to listen to wpa_supplicant's events
 *
 * Its main purpose is to catch these events:
 *      CTRL-EVENT-CONNECTED
 *      CTRL-EVENT-EAP-SUCCESS
 *      CTRL-EVENT-EAP-FAILURE
 *      CTRL-EVENT-SCAN-RESULTS
 */

#include "wpa_supplicant_events_listener.h"

WPASupplicantEventsListener::WPASupplicantEventsListener(std::string wlan_interface_name)
{
    /* start wpa_cli to listen to the updates */
    arguments_list.append(QString::fromStdString(wlan_interface_name));
    wpa_cli_process.start("wpa_cli", arguments_list);
}

WPASupplicantEventsListener::~WPASupplicantEventsListener() {
    wpa_cli_process.terminate();
}

void WPASupplicantEventsListener::terminate() {
    wpa_cli_process.terminate();
}

QString WPASupplicantEventsListener::read_output() {
    return wpa_cli_process.readLine();
}
