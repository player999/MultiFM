#include "receiver_locator.h"
#include <rtl-sdr.h>
#include <libhackrf/hackrf.h>

Receiver::Receiver(ReceiverType type, QString serial):
    _type(type), _serial(serial)
{

}

ReceiverType Receiver::getType()
{
    return _type;
}

QString &Receiver::getSerial()
{
    return _serial;
}

QString Receiver::toString()
{
    QString text;
    if(_type == ReceiverType::RTLSDR)
    {
        text = "RTLSDR ";
    }
    else if(_type == ReceiverType::HACKRF)
    {
        text = "HackRF ";
    }
    else
    {
        text = "Unknown";
    }

    text += _serial;
    return text;
}

static void find_rtlsdrs(QList<Receiver> &rcvrs)
{
    uint32_t number_of_devices = rtlsdr_get_device_count();
    char manufacturer_name[1024];
    char product_name[1024];
    char serial_number[1024];
    for(uint32_t ii = 0; ii < number_of_devices; ii++)
    {
        int ret = rtlsdr_get_device_usb_strings(ii,
            manufacturer_name, product_name, serial_number);
        if(0 == ret)
        {
            rcvrs.append(Receiver(ReceiverType::RTLSDR, serial_number));
        }

    }
}

static void find_hackrfs(QList<Receiver> &rcvrs)
{
    int ret;
    hackrf_device_list_t* devlist;
    ret = hackrf_init();
    if(ret) return;
    devlist = hackrf_device_list();
    if(devlist)
    {
        for(int ii = 0; ii < devlist->devicecount; ii++)
        {
            rcvrs.append(Receiver(ReceiverType::HACKRF, devlist->serial_numbers[ii]));
        }
        (void)hackrf_device_list_free(devlist);
    }
    (void)hackrf_exit();
}

void find_receivers(QList<Receiver> &rcvrs)
{
    find_rtlsdrs(rcvrs);
    find_hackrfs(rcvrs);
}
