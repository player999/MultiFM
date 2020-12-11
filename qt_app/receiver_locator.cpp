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
#include "receiver_locator.h"
#include <rtl-sdr.h>
#include <libhackrf/hackrf.h>
#include <type_traits>
#include <QDataStream>

Receiver::Receiver(ReceiverType type, QString serial):
    _type(type), _serial(serial)
{

}

Receiver::Receiver(QByteArray input_bytes)
{
    QDataStream ist(input_bytes);
    std::underlying_type<ReceiverType>::type t;
    ist >> t;
    _type = static_cast<ReceiverType>(t);
    ist >> _serial;
}

Receiver::Receiver(Receiver &r)
{
    _type = r.getType();
    _serial = r.getSerial();
}

Receiver::Receiver()
{
    _type = ReceiverType::NONE;
    _serial = "";
}

Receiver& Receiver::operator=(const Receiver &r)
{
    _type = r._type;
    _serial = r._serial;
    return *this;
}

ReceiverType Receiver::getType() const
{
    return _type;
}

QString Receiver::getSerial() const
{
    return _serial;
}

QString Receiver::toString() const
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
    else if(_type == ReceiverType::NONE)
    {
        text = "";
    }
    else
    {
        text = "Unknown";
    }

    text += _serial;
    return text;
}

Receiver::operator QByteArray() const
{
    QByteArray ba;
    QDataStream ost(&ba, QIODevice::WriteOnly);
    std::underlying_type<ReceiverType>::type t = static_cast<std::underlying_type<ReceiverType>::type>(_type);
    ost<<t;
    ost<<_serial;
    return ba;
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
    rcvrs.append(Receiver(ReceiverType::NONE, "Select receiver"));
    find_rtlsdrs(rcvrs);
    find_hackrfs(rcvrs);
}
