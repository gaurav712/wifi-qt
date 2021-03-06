#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QStyle>
#include <QDesktopWidget>
#include <QScreen>
#include <QTimer>
#include <QInputDialog>

#include "wlan_info.h"
#include "wpa_supplicant_control.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class UpdateNetworkList: public QThread
{
    Q_OBJECT
public:
    UpdateNetworkList(WPASupplicantControl *wpaSupplicantControl,
                      QListWidget *listWidget,
                      QPushButton *refreshButton);
private:
    WPASupplicantControl *wpaSupplicantControl;
    QListWidget *listWidget;
    QPushButton *refreshButton;
    void run();
    void run_on_main_thread(std::function<void()> callback);
};

class AuthenticateToNetwork: public QThread
{
    Q_OBJECT
public:
    AuthenticateToNetwork(WPASupplicantControl *wpaSupplicantControl,
                          QString ssid,
                          QString psk,
                          bool is_protected);
private:
    WPASupplicantControl *wpaSupplicantControl;
    QString ssid;
    QString psk;
    bool is_protected;
    void run();

signals:
    void authenticatedToNetwork(bool authenticated);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    WlanInfo *wlanInfo;

private slots:
    void on_refreshButton_clicked();
    void on_wlanToggleButton_clicked();
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_networkAuthenticated(bool authenticated);

private:
    Ui::MainWindow *ui;
    WPASupplicantControl *wpaSupplicantControl;
    UpdateNetworkList *updateNetworkList;

    bool network_is_protected(QString ssid);
};

#endif // MAINWINDOW_H
