#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "wlan_control.h"

#include <QDebug>

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

    this->setFixedSize(QApplication::desktop()->width()/3.5, QApplication::desktop()->height()/1.75);

    wlanInfo = new WlanInfo;
    wpaSupplicantControl = new WPASupplicantControl(wlanInfo->wlan_interface_name);
    wpaSupplicantControl->scan_for_networks();
    updateNetworkList = new UpdateNetworkList(wpaSupplicantControl, ui->listWidget);
    updateNetworkList->start();
}

MainWindow::~MainWindow()
{
    delete updateNetworkList;
    delete wpaSupplicantControl;
    delete wlanInfo;
    delete ui;
}

void MainWindow::on_refreshButton_clicked()
{
    wpaSupplicantControl->scan_for_networks();
    updateNetworkList->start();
}

void MainWindow::append_rows() {
    for(int index = 0; index < wpaSupplicantControl->networks.size(); index++) {
        ui->listWidget->addItem(wpaSupplicantControl->networks[index]);
    }
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
//    QStringList networks_list;
    qInfo() << item->text();
}

UpdateNetworkList::UpdateNetworkList(WPASupplicantControl *wpaSupplicantControl, QListWidget *listWidget)
    :wpaSupplicantControl(wpaSupplicantControl), listWidget(listWidget)
{}

void UpdateNetworkList::run() {

    /* Empty the list widget if it already has elements */
    for(int index = (listWidget->count() - 1); index >= 0; index--) {
        delete (listWidget->item(index));
    }

    /* Wait until the list is filled up */
    while((wpaSupplicantControl->networks.isEmpty())) {
        this->msleep(100);
    }

    for(int index = 0; index < wpaSupplicantControl->networks.size(); index++) {
        listWidget->addItem(wpaSupplicantControl->networks[index]);
    }
}
