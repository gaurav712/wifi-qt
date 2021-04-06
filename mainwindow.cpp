#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Place the window in center

    // move the window to (x, y) where
    // x = (width of screen)/2 - (width of window)/2 and
    // y = (height of screen)/2 - (height of window)/2

    this->move((QApplication::desktop()->width())/2 - (this->size().width())/2, (QApplication::desktop()->height())/2 - (this->size().height())/2);
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
