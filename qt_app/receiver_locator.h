#ifndef RECEIVER_LOCATOR_H
#define RECEIVER_LOCATOR_H
#include <QVariant>
#include <QString>
#include <QList>
enum class ReceiverType
{
    NONE,
    HACKRF,
    RTLSDR
};
class Receiver
{
    public:
        Receiver(ReceiverType type, QString serial);
        Receiver(QByteArray idata);
        Receiver(Receiver &r);
        Receiver();
        ReceiverType getType() const;
        QString getSerial() const;
        QString toString() const;
        operator QByteArray() const;
        Receiver& operator=(const Receiver &r);
    private:
        ReceiverType _type;
        QString _serial;
};

void find_receivers(QList<Receiver> &rcvrs);

#endif // RECEIVER_LOCATOR_H
