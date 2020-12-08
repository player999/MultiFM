#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDoubleValidator>
#include <QStringListModel>
#include "receiver_locator.h"
#include "device_options.h"
#include "rfsource.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void selectedNewDevice(int idx);

private slots:
    void on_frequencyButton_toggled(bool checked);
    void on_bwButton_toggled(bool checked);
    void on_lnaGainButton_toggled(bool checked);
    void on_vgaGainButton_toggled(bool checked);

    void on_stopButton_clicked();

private:
    std::list<DSP::ConfigEntry> getReceiverConfiguration();
    Receiver _rcvr;
    DeviceOptions *_devopts = NULL;
    Ui::MainWindow *ui;
    QDoubleValidator _val_fs;
    QDoubleValidator _val_freq;
    QStringListModel *_station_list_model;
    QStringList _station_list;

    bool _freq_valid;
    int64_t _freq;
    bool _fs_valid;
    int64_t _fs;
    bool _lna_valid;
    double _lna_gain;
    bool _vga_valid;
    double _vga_gain;

    void updateDeviceCombo();
    void disableDevRelatedInterface();
};
#endif // MAINWINDOW_H
