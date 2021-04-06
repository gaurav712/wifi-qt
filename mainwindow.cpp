#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>

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

    this->move((QApplication::desktop()->width())/2 - (this->size().width())/2, (QApplication::desktop()->height())/2 - (this->size().height())/2);

    wlanInfo = new WlanInfo;

    if(wlanInfo->wlan_is_on()) {
        std::cout << "wlan is up" << std::endl;
    } else {
        std::cout << "wlan is down" << std::endl;
    }

    if(wlanInfo->wlan_is_connected()) {
        std::cout << "wlan is connected" << std::endl;
    } else {
        std::cout << "wlan is disconnected" << std::endl;
    }

    delete wlanInfo;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_refreshButton_clicked()
{
    QListWidget *networksList = ui->listWidget;
    networksList->addItem("Hey There!");
    std::cout<<"Hello"<<std::endl;
}

void MainWindow::on_wlanToggleButton_clicked()
{
    std::string wlanInterfaceName;
    // Check if wifi is enabled
//    wlanInterfaceName = get_wireless_network_interface_name();
    std::cout << wlanInterfaceName << std::endl;
}
