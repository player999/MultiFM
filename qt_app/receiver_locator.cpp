#include "receiver_locator.h"

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
    rcvrs.append(Receiver(ReceiverType::RTLSDR, "0"));
}

static void find_hackrfs(QList<Receiver> &rcvrs)
{
    rcvrs.append(Receiver(ReceiverType::HACKRF, "1"));
}

void find_receivers(QList<Receiver> &rcvrs)
{
    find_rtlsdrs(rcvrs);
    find_hackrfs(rcvrs);
}
