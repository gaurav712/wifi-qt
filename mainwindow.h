#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QStyle>
#include <QDesktopWidget>
#include <QScreen>

#include "network.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_refreshButton_clicked();

    void on_wlanToggleButton_clicked();

private:
    Ui::MainWindow *ui;
    WlanInfo *wlanInfo;
};
#endif // MAINWINDOW_H
