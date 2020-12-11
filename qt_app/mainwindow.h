/******************************************************************************
Copyright 2020 Taras Zakharchenko taras.zakharchenko@gmail.com

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1.  Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
3.  Neither the name of the copyright holder nor the names of its contributors
    may be used to endorse or promote products derived from this software
    without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/
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
    bool wantStop() const;
    void setControlsEnabled(bool en);
    ~MainWindow();

public slots:
    void selectedNewDevice(int idx);

private slots:
    void on_frequencyButton_toggled(bool checked);
    void on_bwButton_toggled(bool checked);
    void on_lnaGainButton_toggled(bool checked);
    void on_vgaGainButton_toggled(bool checked);

    void on_stopButton_clicked();

    void on_scanButton_clicked();

    void on_startButton_clicked();
private:
    std::list<DSP::ConfigEntry> getReceiverConfiguration();
    Receiver _rcvr;
    DeviceOptions *_devopts = NULL;
    Ui::MainWindow *ui;
    QDoubleValidator _val_fs;
    QDoubleValidator _val_freq;
    QStringListModel *_station_list_model;
    QStringList _station_list;
    std::thread *_rec_thread = NULL;

    bool _please_stop;
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
