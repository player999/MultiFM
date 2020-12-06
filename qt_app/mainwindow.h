#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "receiver_locator.h"
#include "device_options.h"

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

private:
    Receiver _rcvr;
    DeviceOptions *_devopts = NULL;
    Ui::MainWindow *ui;
    void updateDeviceCombo();
    void disableDevRelatedInterface();
};
#endif // MAINWINDOW_H
