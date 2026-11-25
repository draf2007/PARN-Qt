#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <QByteArray>
#include <QString>
#include <QStringList>

namespace Protocol {

constexpr int RESPONSE_SIZE = 122;
constexpr char FIELD_SEPARATOR = '\r';

enum class ParseError{ None, WrongLenght, WrongAddress, BadChecksum, Empty };

struct ParsedResponse
{
    bool ok = false;
    ParseError error = ParseError::None;
    QStringList fields;
};

QByteArray buildRequest(quint8 address, const QString &command, const QStringList &values = {});
ParsedResponse parseResponse(const QByteArray &frame, quint8 address);
quint16 checksum(const QByteArray &data);
QString decodeCp1251(const QByteArray &data);

}

#endif // PROTOCOL_H
