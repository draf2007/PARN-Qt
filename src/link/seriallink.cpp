#include "seriallink.h"

SerialLink::SerialLink(QObject *parent)
    : QObject{parent}
{}

bool SerialLink::isOpen() const {

    return m_state != State::Closed;
}

bool SerialLink::open(const QString &portName){

    if (m_port.isOpen()) {
        m_port.close();
    }

    m_port.setPortName(portName);
    m_port.setBaudRate(QSerialPort::Baud9600);
    m_port.setDataBits(QSerialPort::Data8);
    m_port.setParity(QSerialPort::SpaceParity);
    m_port.setStopBits(QSerialPort::OneStop);
    m_port.setFlowControl(QSerialPort::NoFlowControl);

    if (m_port.open(QIODevice::ReadWrite)) {
        m_buffer.clear();
        m_errorString.clear();
        m_state = State::Idle;
    } else {
        switch (m_port.error()) {
        case QSerialPort::DeviceNotFoundError:
            m_errorString = tr("Устройство не найденно!");
            return false;
            break;
        case QSerialPort::PermissionError:
            m_errorString = tr("Порт занят!");
            return false;
            break;
        default:
            m_errorString = m_port.errorString();
            return false;
            break;
        }
    }
    return true;
}

void SerialLink::close(){
    m_timer.stop();
    if (m_port.isOpen()) {
        m_port.close();
    }
    m_buffer.clear();
    m_state = State::Closed;
}

bool SerialLink::isBusy() const {

    switch (m_state) {
    case State::Sending:
        return true;
        break;
    case State::WaitingFirstByte:
        return true;
        break;
    case State::Receiving:
        return true;
        break;
    default:
        return false;
        break;
    }
}

QString SerialLink::errorString() const {
    return m_errorString;
}
























































