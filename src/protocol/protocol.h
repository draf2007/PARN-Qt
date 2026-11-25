#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <QByteArray>
#include <QString>
#include <QStringList>

namespace Protocol {

constexpr int RESPONSE_SIZE = 122; // Размер ответа, длинна ответа фиксированная, и обусловлена самим протоколом
constexpr char FIELD_SEPARATOR = '\r'; //Сепаратор разделения ответов, или если ответ короче 122 символов, то до 122 символов дополняется этим сепаратором

/*Ошибки ответа*/
enum class ParseError{ None, // Нет ответа
                        WrongLength, //Ошибка длинны
                        WrongAddress, //Ошибка адреса
                        BadChecksum, //Ошибка CRC
                        Empty //Пришли одни 0
};

struct ParsedResponse
{
    bool ok = false; // true — ответ разобран, fields заполнены; false — смотри error
    ParseError error = ParseError::None; // Причина неудачи, содержит поля ParseError
    QStringList fields; // поля ответа текстом, без разделителей '\r'
};

QByteArray buildRequest(quint8 address, const QString &command, const QStringList &values = {}); // Формирование запроса в плату
ParsedResponse parseResponse(const QByteArray &frame, quint8 expectedAddress); // Разбор ответа
quint16 checksum(const QByteArray &data); //Проверка CRC
QString decodeCp1251(const QByteArray &data); // Функция декодирует ответ в cp1251

}//namespace Protocol;

#endif // PROTOCOL_H
