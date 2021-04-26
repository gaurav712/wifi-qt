#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "wlan_control.h"

#include <QDebug>

#define BLOCK   1
#define UNBLOCK 0
#define SSID_INDEX          4
#define PROTECTION_INDEX    3

/* Constructor for the Main Window */
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

/* Toggle wlan */
void MainWindow::on_wlanToggleButton_clicked()
{
    if(wlanInfo->wlan_is_on()) {
        toggle_wlan(BLOCK);
    } else {
        toggle_wlan(UNBLOCK);
    }
}

/* Initiate connection to the selected network */
void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    bool pass_entered = false;
    bool is_protected;
    QString psk = "";

    psk = QInputDialog::getText(this,
                                tr("QInputDialog::getText()"),
                                tr("Enter Password"),
                                QLineEdit::Normal,
                                "", &pass_entered);

    if(pass_entered) {
        if(network_is_protected(item->text())) {
            is_protected = true;
        } else {
            is_protected = false;
        }

        /* Initiate the thread to authenticate with the network */
        AuthenticateToNetwork *authenticateToNetwork = new AuthenticateToNetwork(
                    wpaSupplicantControl,
                    item->text(),   // SSID
                    psk,
                    is_protected);

        authenticateToNetwork->start();

        /* To handle authentication results */
        QObject::connect(authenticateToNetwork, &AuthenticateToNetwork::authenticatedToNetwork,
                         this, &MainWindow::on_networkAuthenticated);

        /* Make sure it is destroyed */
        QObject::connect(authenticateToNetwork, &AuthenticateToNetwork::finished,
                         authenticateToNetwork, &QObject::deleteLater);
    }
}

/* Check if network is open or protected */
bool MainWindow::network_is_protected(QString ssid) {

    for(int index = 0; index < wpaSupplicantControl->networks.size(); index++) {
        if(wpaSupplicantControl->networks[index].split('\t')[SSID_INDEX] == ssid) {
            if(wpaSupplicantControl->networks[index].split('\t')[PROTECTION_INDEX] == "[ESS]") {
                return false;
            }
        }
    }

    return true;
}

/*
 * Handle authentication results
 * like: Warning popup on Failure
 */
void MainWindow::on_networkAuthenticated(bool authenticated) {

    /* Just these debug messages for now */
    if(authenticated) {
        qDebug() << "Connected!";
    } else {
        qDebug() << "Connection failed!";
    }
}

/* Constructor for the thread updating networks list */
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

/* Contructor for thread used for connecting to networks */
AuthenticateToNetwork::AuthenticateToNetwork(WPASupplicantControl *wpaSupplicantControl,
                                             QString ssid,
                                             QString psk,
                                             bool is_protected)
    :wpaSupplicantControl(wpaSupplicantControl)
    , ssid(ssid)
    , psk(psk)
    , is_protected(is_protected)
{}

void AuthenticateToNetwork::run() {

    bool authenticated = false;

    this->sleep(2);
    qDebug() << "Inside the thread" << ssid << psk;

    emit authenticatedToNetwork(authenticated);
}
