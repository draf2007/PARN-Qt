#include "seriallink.h"

SerialLink::SerialLink(QObject *parent)
    : QObject{parent}
{
    m_timer.setSingleShot(true);
    QObject::connect(&m_timer, &QTimer::timeout, this, &SerialLink::onTimeout);
}

bool SerialLink::isOpen() const // метод проверяет открыт ли порт
{
    return m_state != State::Closed;
}

bool SerialLink::open(const QString &portName) // метод настройки и открытия порта
{
    // Проверка открыт ли уже порт, если открыт то закрываем
    if (m_port.isOpen()) {
        close(); // метод закрытия нужен свой, что бы сбросить таймер и закрыть именно тот порт который открыт, иначе есть риск закрыть не тот порт
    }
    // Тут настраиваем порт
    m_port.setPortName(portName);
    m_port.setBaudRate(QSerialPort::Baud9600);
    m_port.setDataBits(QSerialPort::Data8);
    m_port.setParity(QSerialPort::SpaceParity);
    m_port.setStopBits(QSerialPort::OneStop);
    m_port.setFlowControl(QSerialPort::NoFlowControl);

    /*Пытаемся открыть порт*/
    if (!m_port.open(QIODevice::ReadWrite)) { // если не смогли открыть
        switch (m_port.error()) {
        case QSerialPort::DeviceNotFoundError:
            m_errorString = tr("Порт не найден. Проверьте кабель USB.");
            break;
        case QSerialPort::PermissionError:
            m_errorString = tr("Порт занят другой программой.");
            break;
        default:
            m_errorString = m_port.errorString();
            break;
        }
        return false;
    }

    /*если порт открылся,
     * очищаем буфер приёма
     * очищаем буфер ошибок
     * состояние порта переключаем в Idle(свободен)*/
    m_buffer.clear();
    m_errorString.clear();
    m_state = State::Idle;

    return true;
}

void SerialLink::close() // метод закрытия порта
{
    m_timer.stop(); // остановили таймер отвечающий за таймауты
    if (m_port.isOpen()) { // если порт открыт закрываем (защита от двойного закрытия)
        m_port.close();
    }
    m_buffer.clear(); // чистим буфер приема/передачи
    m_state = State::Closed; // выставляем состояние порта в Closed
}

bool SerialLink::isBusy() const // метод определения в каком состоянии порт (занят или нет)
{
    switch (m_state) {
    case State::Sending:
        return true;
    case State::WaitingFirstByte:
        return true;
    case State::Receiving:
        return true;
    case State::Closed:
        return false;
    case State::Idle:
        return false;
    }
    return false;
}

QString SerialLink::errorString() const // метод возвращает ошибку порта
{
    return m_errorString;
}

bool SerialLink::sendFrame(quint8 address, const QByteArray &body) // метод отправки кадра с адресом
{
    /*проверка что порт свободен, если не Idle то возвращаем false(порт занят)*/
    if (m_state != State::Idle) {
        return false;
    }
    m_buffer.clear(); // чистим буфер приёма
    m_pendingBody = body; // сохраняем команду для отправки
    m_state = State::Sending; // выставляем состояние порта в Sending (отправка)
    m_port.setParity(QSerialPort::MarkParity); // меняем чётность согласно протоколу
    QByteArray addr(1, static_cast<char>(address)); // делаем из адреса в quint8, массив символов
    m_port.write(addr); // записываем адрес в порт
    emit bytesSent(addr); // отправляем сигнал, что отправили в порт, в журнал
    m_timer.start(ADDRESS_PAUSE_MS); // запускаем таймер для отправки адреса
    return true;
}

void SerialLink::onTimeout() // метод проверяет m_state и в зависимости от того в каком State сработал таймер выполняет действие
{
    switch (m_state) {
    case State::Sending: // таймер сработал после отправки адреса
        m_port.setParity(QSerialPort::SpaceParity); // переключаем чётность
        m_port.write(m_pendingBody); // пишем тело запроса
        emit bytesSent(m_pendingBody); // отправляем тело запроса в журнал
        m_state = State::WaitingFirstByte; // меняем состояние на ожидаем получение первого байта
        m_timer.start(FIRST_BYTE_TIMEOUT_MS); // запускаем таймер ожидания первого байта
        break;
    /*ниже два case - одно и тоже действие*/
    case State::WaitingFirstByte: // ждём первый байт
    case State::Receiving: // в состоянии ожидания ответа от платы
        m_buffer.clear(); // чистим буфер приёма
        m_state = State::Idle; // меняем состояние порта на свободен
        emit responseTimeout(); // ответ не пршёл вовремя -> по сигналу выдаём сообщение "Нет ответа."
        break;
    /*На эти два состояния таймеры срабатывать не должны поэтому break*/
    case State::Closed:
    case State::Idle:
        break;
    }
}
























































