#include "seriallink.h"

SerialLink::SerialLink(QObject *parent)
    : QObject{parent}
{}

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
            m_errorString = tr("Порт не найден. Проверте кабель USB.");
            break;
        case QSerialPort::PermissionError:
            m_errorString = tr("Порт занят другой программой.");
            break;
        default:
            m_errorString = m_port.errorString();
            break;
        }
    }

    /*если порт открылся,
     * очищаем буфер приёма/передачи
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
























































