#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QStyle>
#include <QDesktopWidget>
#include <QScreen>

#include "wlan_info.h"
#include "wpa_supplicant_control.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

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

private:
    Ui::MainWindow *ui;
    WPASupplicantControl *wpa_supplicant_control;
};
#endif // MAINWINDOW_H
