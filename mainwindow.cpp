#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "wlan_control.h"

#include <QDebug>

#define BLOCK   1
#define UNBLOCK 0
#define SSID_INDEX  4

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

    /* Make the window non-resizable */
    this->setFixedSize(QApplication::desktop()->width()/3.5, QApplication::desktop()->height()/1.75);

    /* Initialize the WlanInfo and WPASupplicantControl instances */
    wlanInfo = new WlanInfo;
    wpaSupplicantControl = new WPASupplicantControl(wlanInfo->wlan_interface_name);

    /* Scan for networks when it starts up */
    ui->refreshButton->setVisible(false);
    wpaSupplicantControl->scan_for_networks();

    /* Add the networks found to the listWidget */
    updateNetworkList = new UpdateNetworkList(wpaSupplicantControl, ui->listWidget, ui->refreshButton);
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
    /* Disable the refresh button */
    ui->refreshButton->setVisible(false);

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

UpdateNetworkList::UpdateNetworkList(WPASupplicantControl *wpaSupplicantControl,
                                     QListWidget *listWidget,
                                     QPushButton *refreshButton)
    :wpaSupplicantControl(wpaSupplicantControl)
    , listWidget(listWidget)
    , refreshButton(refreshButton)
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
        listWidget->addItem((wpaSupplicantControl->networks[index]).split('\t')[SSID_INDEX]);
    }

    /* make the refresh button visible again */
    run_on_main_thread( [this]{
        refreshButton->setVisible(true);
    });
}

/*
 * run the specified callback() on the main thread
 *
 * Credits: https://riptutorial.com/qt/example/21783/using-qtimer-to-run-code-on-main-thread
 */
void UpdateNetworkList::run_on_main_thread(std::function<void()> callback)
{
    QTimer* timer = new QTimer();
    timer->moveToThread(qApp->thread());
    timer->setSingleShot(true);
    QObject::connect(timer, &QTimer::timeout, [=]()
    {
        // execute the callback
        callback();
        timer->deleteLater();
    });
    QMetaObject::invokeMethod(timer, "start", Qt::QueuedConnection, Q_ARG(int, 0));
}
