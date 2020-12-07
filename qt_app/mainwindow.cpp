#include "mainwindow.h"
#include "receiver_locator.h"
#include "ui_main_window.h"
#include <QDebug>
#include <QDoubleValidator>

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
    _val_fs.setRange(_devopts->minBandwidth(), _devopts->maxBandwidth());
    ui->bwEdit->setValidator(&_val_fs);
    ui->bwEdit->setEnabled(true);
    ui->frequencyButton->setEnabled(true);
    _val_freq.setRange(_devopts->minFrequency(), _devopts->maxFrequency());
    ui->frequencyEdit->setValidator(&_val_freq);
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
        ui->lnaGainCombo->addItem("Automatic", QVariant(-1.0));
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


void MainWindow::on_frequencyButton_toggled(bool checked)
{
    if(checked)
    {
        if(ui->frequencyEdit->hasAcceptableInput())
        {
            _freq_valid = true;
            _freq = ui->frequencyEdit->text().toDouble();
        }
        else
        {
            ui->frequencyButton->setChecked(false);
        }
    }
    else
    {
        _freq_valid = false;
    }
}

void MainWindow::on_bwButton_toggled(bool checked)
{
    if(checked)
    {
        if(ui->bwEdit->hasAcceptableInput())
        {
            _fs_valid = true;
            _fs = ui->bwEdit->text().toDouble();
        }
        else
        {
            ui->bwButton->setChecked(false);
        }
    }
    else
    {
        _fs_valid = false;
    }
}

void MainWindow::on_lnaGainButton_toggled(bool checked)
{
    _lna_valid = checked;
    if(_lna_valid)
    {
        _lna_gain = ui->lnaGainCombo->currentData().toDouble();
    }
}

void MainWindow::on_vgaGainButton_toggled(bool checked)
{
    _vga_valid = checked;
    if(_vga_valid)
    {
        _vga_gain = ui->vgaGainCombo->currentData().toDouble();
    }
}
