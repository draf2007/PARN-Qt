#include "protocol.h"

quint16 Protocol::checksum(const QByteArray &data){
    quint16 sum = 0;
    for (char c : data) {

        sum +=static_cast<quint8>(c);
    }
    return sum;
}

QByteArray Protocol::buildRequest(quint8 address, const QString &command, const QStringList &values)
{

    return body;
}