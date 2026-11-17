#ifndef SERIALLINK_H
#define SERIALLINK_H

#include <QObject>
#include <QSerialPort>
#include <QTimer>
#include <QByteArray>
#include <QString>

class SerialLink : public QObject
{
    Q_OBJECT
public:
    explicit SerialLink(QObject *parent = nullptr);
    bool open(const QString &portName); // Открыть порт, вернёт true если открылся иначе false
    void close(); //Закрыть порт
    bool isOpen() const; // Порт открыт?
    bool isBusy() const; // Порт занят?
    QString errorString() const; // Ошибка, возвращается текстом
    bool sendFrame(quint8 address, const QByteArray &body); // Отправить фрейм в плату, true передача началась, false если порт закрыт или занят

    static constexpr int RESPONSE_SIZE = 122; // Размер посылки в байтах
    static constexpr int ADDRESS_PAUSE_MS = 100; // Пауза после отправки адреса
    static constexpr int FIRST_BYTE_TIMEOUT_MS = 4000; // Таймаут ожидания первого байта, иначе нет ответа от платы
    static constexpr int FRAME_TIMEOUT_MS = 1000; // Досылка оставшихся байт после получения первого

signals:
    void frameReceived(const QByteArray &frame); // несёт в себе полученные 122 байта из платы
    void responseTimeout(); // таймаут если если за заданное время данные из платы не получены
    void portError(const QString &text); // несёт в себе текст ошибки
    void bytesSent(const QByteArray &data); // что ушло в порт для журнала
    void bytesReceived(const QByteArray &data); // что принято из порта для журнала

private slots:
    void onTimeout(); // слот срабатывает если ответ от платы не пришёл вовремя
    void onReadyRead(); // слот срабатывает когда в порту появились данные для чтения
    void onPortError(QSerialPort::SerialPortError error); // слот срабатывает если произошла ошибка(выдернули USB кабель)

private:
    /*Класс перечисления состояний порта*/
    enum class State{
        Closed, // закрыт
        Idle, // свободен
        Sending, // идёт отправка
        WaitingFirstByte, // ждём первый байт ответа
        Receiving // получение
    };

    QSerialPort m_port; // порт для подключения, содержит структуру со всеми параметрами для подключения
    QTimer m_timer; // Обычный таймер
    QByteArray m_buffer; // Буфер для накопления ответа
    State m_state = State::Closed; // Состояние порта(при запуске программы всегда Close)
    QString m_errorString; // Содержит в себе текст ошибки
    QByteArray m_pendingBody; // тело кадра, которое ждёт окончание таймаута

};

#endif // SERIALLINK_H
