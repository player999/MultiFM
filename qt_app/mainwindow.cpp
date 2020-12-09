#include "mainwindow.h"
#include "receiver_locator.h"
#include "ui_main_window.h"
#include <QDebug>
#include <QDoubleValidator>
#include <cutedsp.hpp>

using namespace DSP;
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    updateDeviceCombo();
    _station_list_model = new QStringListModel(this);
    ui->stationList->setModel(_station_list_model);
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

std::list<ConfigEntry> MainWindow::getReceiverConfiguration()
{
    std::list<ConfigEntry> conf;

    if(_rcvr.getType() == ReceiverType::HACKRF)
    {
        if(_vga_valid && _lna_valid && _fs_valid && _freq_valid)
        {
            conf.push_back(ConfigEntry("source_type", "hackrf"));
            conf.push_back(ConfigEntry("lna_gain", (int64_t)_lna_gain));
            conf.push_back(ConfigEntry("vga_gain", (int64_t)_vga_gain));
            conf.push_back(ConfigEntry("sampling_rate", (double)_fs));
            conf.push_back(ConfigEntry("frequency", (double)_freq));
        }
    }
    else if(_rcvr.getType() == ReceiverType::RTLSDR)
    {
        if(_vga_valid && _lna_valid && _fs_valid && _freq_valid)
        {
            conf.push_back(ConfigEntry("frequency", (double)_freq));
            conf.push_back(ConfigEntry("gain", _lna_gain));
            conf.push_back(ConfigEntry("sampling_rate", (double)_fs));
        }
    }

    return conf;
}

static Error station_searching(queue<RfChunk> &q, double fs, double cf, vector<double> &found_freqs)
{
    Error err;
    const uint32_t analyze_length = 1024 * 1024;
    vector<double> deinter_i;
    vector<double> deinter_q;

    deinter_i.reserve(analyze_length * 2);
    deinter_q.reserve(analyze_length * 2);

    do
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        while((q.size() > 0) && (deinter_i.size() < analyze_length))
        {
            RfChunk chunk = q.front();
            deinter_i.insert(deinter_i.end(), chunk.I.begin(), chunk.I.end());
            deinter_q.insert(deinter_q.end(), chunk.Q.begin(), chunk.Q.end());
            q.pop();
        }
    } while(deinter_i.size() < analyze_length);

    FmStationsFinder<double> finder((int64_t)fs, (int64_t) cf, analyze_length, 100e3, 1e3, 3, 5.0);
    err = finder.findStations(deinter_i.data(), deinter_q.data(), analyze_length, found_freqs);
    return err;
}

void MainWindow::on_stopButton_clicked()
{

}

void MainWindow::on_scanButton_clicked()
{
    static std::thread t;
    std::list<ConfigEntry> configs = getReceiverConfiguration();
    if(configs.empty())
    {
        return;
    }
    ui->stopButton->setEnabled(false);
    ui->startButton->setEnabled(false);
    ui->scanButton->setEnabled(false);
    t = std::thread([configs,this](){
        RfSource *src = createSource(configs);
        queue<RfChunk> data_queue;
        vector<double> found_freqs;
        src->registerQueue(&data_queue);
        src->start();
        Error err = station_searching(data_queue, (double) _fs, (double)_freq, found_freqs);
        src->stop();
        if(Error::SUCCESS == err)
        {
            _station_list.clear();
            for(double f: found_freqs)
            {
                _station_list << QString::number(f, 'f', 0);
            }
            _station_list_model->setStringList(_station_list);
        }
        delete src;
        ui->stopButton->setEnabled(true);
        ui->scanButton->setEnabled(true);
        ui->startButton->setEnabled(true);
    });
}

void MainWindow::on_startButton_clicked()
{

}
