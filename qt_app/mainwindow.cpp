#include "mainwindow.h"
#include "receiver_locator.h"
#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    find_receivers(rceivrs);
    for(Receiver &r: rceivrs)
    {
        ui->deviceCombo->addItem(r.toString(), r);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

