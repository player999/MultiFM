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
class Receiver: public QVariant
{
    public:
        Receiver(ReceiverType type, QString serial);
        ReceiverType getType();
        QString &getSerial();
        QString toString();
    private:
        ReceiverType _type;
        QString _serial;
};

void find_receivers(QList<Receiver> &rcvrs);

#endif // RECEIVER_LOCATOR_H
