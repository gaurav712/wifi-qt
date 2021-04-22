#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "wlan_control.h"

#define BLOCK   1
#define UNBLOCK 0

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*
     * Place the window in center
     *
     * move the window to (x, y) where
     * x = (width of screen)/2 - (width of window)/2 and
     * y = (height of screen)/2 - (height of window)/2
     */

    this->move((QApplication::desktop()->width())/2 - (this->size().width())/2,
               (QApplication::desktop()->height())/2 - (this->size().height())/2);

    wlanInfo = new WlanInfo;
    wpa_supplicant_control = new WPASupplicantControl(wlanInfo->wlan_interface_name);
    wpa_supplicant_events_listener = new WPASupplicantEventsListener(wlanInfo->wlan_interface_name);

    wpa_supplicant_control->send_cmd("PING");
    qInfo() << wpa_supplicant_control->get_response();
}

MainWindow::~MainWindow()
{
    delete wpa_supplicant_control;
    delete wpa_supplicant_events_listener;
    delete wlanInfo;
    delete ui;
}


void MainWindow::on_refreshButton_clicked()

{
    QListWidget *networksList = ui->listWidget;
    networksList->addItem("Hey There!");
}

void MainWindow::on_wlanToggleButton_clicked()
{

    if(wlanInfo->wlan_is_on()) {
        toggle_wlan(BLOCK);
    } else {
        toggle_wlan(UNBLOCK);
    }
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    /* Connect to the selected network */
}
