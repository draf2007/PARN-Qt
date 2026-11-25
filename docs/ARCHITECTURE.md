#### Архитектура проекта «PARN Setup»

#### 1. Назначение и стек

* **Задача:** десктопная программа для связи с платами регулирования напряжения «ПАРН» (ATmega128) по последовательному порту (USB-UART через QSerialPort). Переписывание программы на Delphi (parn6, рабочие формы с суффиксом _mks).
* **Функционал:** чтение измерений, калибровка (запись коэффициентов в EEPROM), настройка уставок, работа с часами платы, просмотр счётчиков переключений ступеней, журнал обмена (hex + текст).
* **Стек:** Qt 6 (Widgets, SerialPort, позже Test), C++17, CMake (qt_add_executable(parn ...), WIN32_EXECUTABLE ON), целевая ОС — Windows.

#### 2. Различия вариантов плат (PARN1 и PARN3)

Подтверждено по прошивкам parn1_58.PAS / parn3_58.PAS. Плата сама версию не сообщает — вариант выбирает пользователь (enum BoardVariant { Parn1, Parn3 }). 

* **Измерение фаз:** в обеих платах реально измеряется только фаза A (код фазы B в read_u_abc всегда возвращает 0, измерение фаз B/C закомментировано в обеих прошивках).
* **Число ступеней:** PARN1 — до ±16; PARN3 — до ±12.
* **Определение текущей ступени:** PARN1 — концевики/реле, прерывание Int5 считает импульсы; PARN3 — микросхема MCP23S17 по SPI, опрашивается тем же прерыванием.
* **Шаг ступени:** PARN1 — зависит от tip_podkl (0.00625 или 0.009375); PARN3 — 0.20 / step_max.
* **Смысл set_step / get_step:** PARN1 — текущая ступень (-16..16); PARN3 — номер ступени, которая считается нулевой (1..13).

#### 3. Структура каталогов и слои зависимостей

Зависимости строго в одну сторону: MainWindow → CalibrationSession → BoardClient → SerialLink и Protocol. 

text

src/
  main.cpp                 — [ГОТОВ] заглушка: пустое окно «PARN Setup» 800x600
  link/seriallink.h/.cpp   — [ГОТОВ] низкоуровневый транспорт COM-порта
  protocol/protocol.h/.cpp — [В ПЛАНАХ] чистые функции сборки/разбора кадров (namespace Protocol)
  core/BoardClient         — [В ПЛАНАХ] «переводчик» между SerialLink и Protocol, бизнес-API платы
  core/CalibrationSession  — [В ПЛАНАХ] шаги калибровки, накопление точек, расчёт коэффициентов (по Unit_calibr_mks.pas)
  ui/MainWindow            — [В ПЛАНАХ] выбор порта, варианта платы, таблицы значений, журнал обмена

Используйте код с осторожностью.

Скопировать код в буфер обмена.

#### 4. Состояние и замечания по готовому классу SerialLink

* **Конечный автомат (QTimer, без блокировок):** Closed → Idle → Sending → WaitingFirstByte → Receiving → Idle.
* **Параметры порта:** 9600, 8N1, без управления потоком.
* **Логика передачи:** 1 байт адреса уходит с MarkParity (эмуляция 9-го бита) → пауза ADDRESS_PAUSE_MS = 100 мс → переключение на SpaceParity → отправка тела команды.
* **Логика приёма:** ожидание первого байта FIRST_BYTE_TIMEOUT_MS = 4000 мс, остаток кадра FRAME_TIMEOUT_MS = 1000 мс; при накоплении 122 байт эмитится frameReceived(QByteArray).
* **Публичный API:** 

  * Методы: open(portName), close(), isOpen(), isBusy(), errorString(), sendFrame(address, body).
  * Сигналы: frameReceived(QByteArray), responseTimeout(), portError(QString), bytesSent(QByteArray), bytesReceived(QByteArray).
* **Известные замечания к SerialLink (ожидают правки на этапе доработки):** 

  1. SerialLink::onPortError сейчас закрывает порт при любой ошибке. Нужно закрывать только при ResourceError, DeviceNotFoundError, PermissionError, а остальные ошибки только логировать через portError без смены m_state.
  2. RESPONSE_SIZE продублирована в SerialLink — после создания Protocol::RESPONSE_SIZE определить единое место хранения константы.
  3. m_pendingBody не очищается в onTimeout и close() (не критично).