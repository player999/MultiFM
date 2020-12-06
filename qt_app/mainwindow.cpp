#include "mainwindow.h"
#include "receiver_locator.h"
#include "ui_main_window.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    updateDeviceCombo();
    connect(ui->deviceCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedNewDevice(int)));
}

void MainWindow::updateDeviceCombo()
{
    QList<Receiver> rec;
    find_receivers(rec);
    for(Receiver &r: rec)
    {
        ui->deviceCombo->addItem(r.toString(), QVariant(static_cast<QByteArray>(r)));
    }
}

void MainWindow::disableDevRelatedInterface()
{
    ui->bwButton->setEnabled(false);
    ui->bwEdit->setEnabled(false);
    ui->frequencyButton->setEnabled(false);
    ui->frequencyEdit->setEnabled(false);
    ui->lnaGainButton->setEnabled(false);
    ui->lnaGainCombo->setEnabled(false);
    ui->vgaGainButton->setEnabled(false);
    ui->vgaGainCombo->setEnabled(false);
    ui->scanButton->setEnabled(false);
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(false);
    ui->vgaGainCombo->clear();
    ui->lnaGainCombo->clear();
}

void MainWindow::selectedNewDevice(int idx)
{
    Q_UNUSED(idx);
    _rcvr = Receiver(ui->deviceCombo->currentData().toByteArray());
    qDebug() << "Selected device: " << _rcvr.toString();
    if(_devopts) delete _devopts;
    _devopts = createDeviceOptions(_rcvr);

    disableDevRelatedInterface();

    ui->bwButton->setEnabled(true);
    ui->bwEdit->setEnabled(true);
    ui->frequencyButton->setEnabled(true);
    ui->frequencyEdit->setEnabled(true);
    ui->lnaGainButton->setEnabled(true);
    ui->lnaGainCombo->setEnabled(true);
    ui->scanButton->setEnabled(true);
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(true);

    if(_rcvr.getType() == ReceiverType::HACKRF)
    {
        ui->vgaGainButton->setEnabled(true);
        ui->vgaGainCombo->setEnabled(true);

        for(double g: _devopts->gains(0))
        {
            ui->lnaGainCombo->addItem(QString::number(g, 'f', 1), QVariant(g));
        }

        for(double g: _devopts->gains(1))
        {
            ui->vgaGainCombo->addItem(QString::number(g, 'f', 1), QVariant(g));
        }
    }
    else if(_rcvr.getType() == ReceiverType::RTLSDR)
    {
        for(double g: _devopts->gains(0))
        {
            ui->lnaGainCombo->addItem(QString::number(g, 'f', 1), QVariant(g));
        }
    }
    return;
}

MainWindow::~MainWindow()
{
    if(_devopts) delete _devopts;
    delete ui;
}

