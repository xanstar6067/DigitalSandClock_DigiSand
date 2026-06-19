# SandClock STM32

Digital sand clock reconstructed and ported to **STM32F411CEU6 BlackPill**.

Цифровые песочные часы, реконструированные и перенесённые на **STM32F411CEU6 BlackPill**.

The project is based on the idea of the original Arduino project by AlexGyver:

Original project: https://github.com/AlexGyver/DigiSand

This repository contains my STM32 version, adapted for my own hardware build, wiring, display orientation and firmware structure.

---

# Русская версия

## Описание

**SandClock STM32** — это цифровые песочные часы на базе микроконтроллера **STM32F411CEU6 BlackPill**.

Проект использует две последовательно соединённые светодиодные матрицы **MAX7219 8×8** и акселерометр **MPU6050**. Акселерометр определяет положение устройства, а прошивка симулирует пересыпание цифрового песка между двумя матрицами.

Изначально проект был собран по мотивам Arduino-проекта **DigiSand** от AlexGyver, но позже был перенесён на STM32. Версия на STM32 работает быстрее, стабильнее и имеет запас производительности для дальнейших улучшений.

## Возможности

* STM32F411CEU6 BlackPill вместо Arduino Nano.
* Две последовательно соединённые матрицы MAX7219 8×8.
* Акселерометр MPU6050 для определения ориентации.
* Симуляция цифрового песка.
* Настройка времени таймера внешними кнопками.
* Настройка яркости матриц.
* Сохранение настроек во Flash-памяти микроконтроллера.
* Автоматическое сохранение настроек после изменения.
* Индикация состояния акселерометра встроенным светодиодом.
* Поддержка программной настройки ориентации акселерометра и матриц.
* В планах: добавление пищалки / пьезодинамика.

## Аппаратная часть

Основные компоненты:

| Компонент        | Описание                                |
| ---------------- | --------------------------------------- |
| Микроконтроллер  | STM32F411CEU6 BlackPill                 |
| Дисплей          | 2 × MAX7219 8×8 LED matrix              |
| Датчик положения | MPU6050                                 |
| Кнопки           | 2 внешние кнопки                        |
| Дополнительно    | Пьезодинамик / пищалка в будущей версии |

## Подключение

### Матрицы MAX7219

Две матрицы MAX7219 8x8 соединены последовательно.

В этой версии проекта MAX7219 подключены не к аппаратному SPI, а к обычным GPIO
STM32. Передача данных выполняется программно, по SPI-подобному интерфейсу.

| Сигнал MAX7219 | Пин STM32 | Назначение    |
| -------------- | --------- | ------------- |
| CLK            | PB3       | `MAX7219_CLK` |
| CS             | PB4       | `MAX7219_CS`  |
| DIN            | PB5       | `MAX7219_DIN` |
| VCC            | 5V        | Питание матриц |
| GND            | GND       | Общая земля   |

Матрицы соединяются последовательно:

```text
STM32 PB5 / MAX7219_DIN -> DIN первой матрицы
DOUT первой матрицы       -> DIN второй матрицы
CLK и CS                  -> общие для обеих матриц
```

### MPU6050

Акселерометр MPU6050 подключен к шине `I2C1`.

| Сигнал MPU6050 | Пин STM32 | Назначение |
| -------------- | --------- | ---------- |
| SCL            | PB6       | `I2C1_SCL` |
| SDA            | PB7       | `I2C1_SDA` |
| VCC            | 3.3V      | Питание модуля |
| GND            | GND       | Общая земля |

Для STM32 безопаснее питать модуль MPU6050 от `3.3V`. Некоторые платы MPU6050
имеют собственный стабилизатор и могут принимать `5V`, но логика STM32 остается
3.3-вольтовой.

### Кнопки

Внешние кнопки `EXT_BTN1` и `EXT_BTN2` работают как входы с внутренней подтяжкой STM32 к питанию.

Внешние резисторы не нужны.

| Кнопка   | Пин STM32 | Назначение                  |
| -------- | --------- | --------------------------- |
| EXT_BTN1 | PB8       | Первая внешняя кнопка       |
| EXT_BTN2 | PB9       | Вторая внешняя кнопка       |
| UKEY     | PA0       | Встроенная сервисная кнопка |

Подключение:

```text
GND ---- кнопка ---- PB8  EXT_BTN1
GND ---- кнопка ---- PB9  EXT_BTN2
```

Не подключайте эти кнопки к `3.3V`.

Прошивка ожидает, что при нажатии кнопка замыкает входной пин на землю.

Встроенная кнопка `UKEY` используется как сервисная / тестовая кнопка.

### Встроенный светодиод

| Сигнал | Пин STM32 | Назначение                  |
| ------ | --------- | --------------------------- |
| ULED   | PC13      | Индикация состояния MPU6050 |

### Служебные интерфейсы

| Интерфейс | Пины STM32  | Назначение                              |
| --------- | ----------- | --------------------------------------- |
| SWD       | PA13 / PA14 | Прошивка и отладка                      |
| USART1    | PA9 / PA10  | Последовательный порт, если понадобится |

## Управление

### EXT_BTN1

* Короткое нажатие: увеличить таймер на 1 секунду.
* Удержание: увеличивать таймер по 10 секунд.

### EXT_BTN2

* Короткое нажатие: уменьшить таймер на 1 секунду.
* Удержание: уменьшать таймер по 10 секунд.

### EXT_BTN1 + EXT_BTN2

* Одновременное нажатие: сбросить песок в стартовую камеру.

### UKEY

* Короткое нажатие: сбросить песок.
* Удержание: переключать яркость матриц.

При изменении времени дисплей на несколько секунд показывает значение в формате `MMSS`, затем возвращается к анимации песка.

## Индикация ULED

Встроенный светодиод `ULED` на `PC13` показывает состояние акселерометра:

| Поведение светодиода | Значение                    |
| -------------------- | --------------------------- |
| Медленное мигание    | MPU6050 работает и читается |
| Быстрое мигание      | MPU6050 не отвечает         |

## Сохранение настроек во Flash

Прошивка хранит пользовательские настройки во внутренней Flash-памяти STM32.

Сохраняются:

* установленное время таймера;
* яркость матриц.

Сохранение происходит автоматически примерно через 2 секунды после последнего изменения времени или яркости.

Последний сектор Flash зарезервирован под настройки, поэтому linker script ограничивает область программы первыми **384 KB** Flash-памяти.

## Настройка осей и ориентации матриц

Если акселерометр или матрицы установлены иначе, ориентацию можно исправить программно.

Для этого используются define в файле:

```text
Core/Inc/sand_clock_config.h
```

Основные настройки:

```c
#define SAND_CLOCK_IMU_X_AXIS
#define SAND_CLOCK_IMU_X_SIGN
#define SAND_CLOCK_IMU_Y_AXIS
#define SAND_CLOCK_IMU_Y_SIGN
#define SAND_CLOCK_IMU_Z_AXIS
#define SAND_CLOCK_IMU_Z_SIGN

#define SAND_CLOCK_DISPLAY_MIRROR_X
#define SAND_CLOCK_DISPLAY_MIRROR_Y
```

Текущая прошивка уже настроена под мою сборку. Верх / низ и лево / право исправлены программно.

## Текущий статус

Проект находится в состоянии рабочего прототипа.

Уже работает:

* вывод на две матрицы MAX7219;
* каскадирование двух матриц;
* чтение MPU6050;
* определение ориентации устройства;
* симуляция цифрового песка;
* управление таймером кнопками;
* изменение яркости;
* сохранение настроек во Flash;
* индикация состояния акселерометра.

## Планируемые улучшения

* Добавить пищалку / пьезодинамик.
* Добавить звуковой сигнал окончания таймера.
* Добавить звук при перевороте устройства.
* Уложить проводку внутри корпуса.
* Заменить отладочную BlackPill с пинами на новую плату без гребёнок.
* Улучшить сборку корпуса.

## Отличия от Arduino-версии

Эта версия перенесена на STM32F411CEU6 BlackPill.

Преимущества STM32-версии:

* более высокая производительность;
* плавная симуляция песка;
* больше памяти;
* больше возможностей для дальнейшего расширения;
* удобная работа с периферией STM32;
* сохранение настроек во внутреннюю Flash-память.

## Благодарности

Оригинальный проект:

* AlexGyver/DigiSand
  https://github.com/AlexGyver/DigiSand

Этот репозиторий содержит мою STM32-адаптацию проекта.

---

# English version

## Description

**SandClock STM32** is a digital sand clock based on the **STM32F411CEU6 BlackPill** microcontroller board.

The project uses two chained **MAX7219 8×8 LED matrix modules** and an **MPU6050 accelerometer**. The accelerometer detects the device orientation, while the firmware simulates digital sand falling between the two LED matrices.

The project was inspired by the original Arduino-based **DigiSand** project by AlexGyver and later ported to STM32. The STM32 version is faster, more stable and has more performance headroom for future improvements.

## Features

* STM32F411CEU6 BlackPill instead of Arduino Nano.
* Two chained MAX7219 8×8 LED matrix modules.
* MPU6050 accelerometer for orientation detection.
* Digital sand simulation.
* Timer adjustment using external buttons.
* Matrix brightness adjustment.
* Settings stored in internal MCU Flash memory.
* Automatic settings save after changes.
* Accelerometer status indication using the onboard LED.
* Software configuration for IMU and display orientation.
* Planned: piezo buzzer / small speaker support.

## Hardware

Main components:

| Component          | Description                                      |
| ------------------ | ------------------------------------------------ |
| Microcontroller    | STM32F411CEU6 BlackPill                          |
| Display            | 2 × MAX7219 8×8 LED matrix                       |
| Orientation sensor | MPU6050                                          |
| Buttons            | 2 external buttons                               |
| Optional           | Piezo buzzer / small speaker in a future version |

## Connections

### MAX7219 display connection

Two MAX7219 8x8 matrix modules are connected in a chain.

In this project the MAX7219 modules are not connected to the hardware SPI
peripheral. They use ordinary STM32 GPIO pins, and the SPI-like protocol is
implemented in firmware.

| MAX7219 signal | STM32 pin | Function      |
| -------------- | --------- | ------------- |
| CLK            | PB3       | `MAX7219_CLK` |
| CS             | PB4       | `MAX7219_CS`  |
| DIN            | PB5       | `MAX7219_DIN` |
| VCC            | 5V        | Matrix power  |
| GND            | GND       | Common ground |

The two matrix modules are connected in a chain:

```text
STM32 PB5 / MAX7219_DIN -> DIN of the first matrix
DOUT of the first matrix -> DIN of the second matrix
CLK and CS               -> shared by both matrices
```

### MPU6050

The MPU6050 accelerometer is connected to the `I2C1` bus.

| MPU6050 signal | STM32 pin | Function     |
| -------------- | --------- | ------------ |
| SCL            | PB6       | `I2C1_SCL`   |
| SDA            | PB7       | `I2C1_SDA`   |
| VCC            | 3.3V      | Module power |
| GND            | GND       | Common ground |

Using `3.3V` power is the safer option with STM32. Some MPU6050 breakout boards
include a regulator and may accept `5V`, but STM32 logic is still 3.3 V.

### Buttons

External buttons `EXT_BTN1` and `EXT_BTN2` are configured as active-low inputs with internal STM32 pull-up resistors.

External resistors are not required.

| Button   | STM32 pin | Function                 |
| -------- | --------- | ------------------------ |
| EXT_BTN1 | PB8       | First external button    |
| EXT_BTN2 | PB9       | Second external button   |
| UKEY     | PA0       | Onboard service button   |

Connection:

```text
GND ---- button ---- PB8  EXT_BTN1
GND ---- button ---- PB9  EXT_BTN2
```

Do not connect these buttons to `3.3V`.

The firmware expects a pressed button to short the input pin to ground.

The onboard `UKEY` button is used as a service / test button.

### Onboard LED

| Signal | STM32 pin | Function                  |
| ------ | --------- | ------------------------- |
| ULED   | PC13      | MPU6050 status indication |

### Service interfaces

| Interface | STM32 pins  | Function                  |
| --------- | ----------- | ------------------------- |
| SWD       | PA13 / PA14 | Programming and debugging |
| USART1    | PA9 / PA10  | Serial port, if needed    |

## Controls

### EXT_BTN1

* Short press: increase timer by 1 second.
* Hold: increase timer repeatedly by 10 seconds.

### EXT_BTN2

* Short press: decrease timer by 1 second.
* Hold: decrease timer repeatedly by 10 seconds.

### EXT_BTN1 + EXT_BTN2

* Press both buttons at the same time: reset sand to the starting chamber.

### UKEY

* Short press: reset sand.
* Hold: cycle display brightness.

When the timer value is changed, the display shows the current value in `MMSS` format for a few seconds, then returns to the sand animation.

## ULED indication

The onboard `ULED` on `PC13` shows the MPU6050 status:

| LED behavior | Meaning                        |
| ------------ | ------------------------------ |
| Slow blink   | MPU6050 is online and readable |
| Fast blink   | MPU6050 is not responding      |

## Flash settings storage

The firmware stores user settings in the internal Flash memory of the STM32.

Stored settings:

* timer value;
* matrix brightness.

Settings are saved automatically about 2 seconds after the last timer or brightness change.

The last Flash sector is reserved for settings storage. Because of this, the linker script limits the application area to the first **384 KB** of Flash memory.

## Axis and display orientation configuration

If the accelerometer or LED matrices are installed differently, the orientation can be corrected in software.

The configuration is located in:

```text
Core/Inc/sand_clock_config.h
```

Main configuration defines:

```c
#define SAND_CLOCK_IMU_X_AXIS
#define SAND_CLOCK_IMU_X_SIGN
#define SAND_CLOCK_IMU_Y_AXIS
#define SAND_CLOCK_IMU_Y_SIGN
#define SAND_CLOCK_IMU_Z_AXIS
#define SAND_CLOCK_IMU_Z_SIGN

#define SAND_CLOCK_DISPLAY_MIRROR_X
#define SAND_CLOCK_DISPLAY_MIRROR_Y
```

The current firmware is already configured for my physical build. Matrix direction and accelerometer orientation are corrected in software.

## Current status

The project is a working prototype.

Already working:

* output to two MAX7219 matrices;
* chained MAX7219 modules;
* MPU6050 reading;
* device orientation detection;
* digital sand simulation;
* timer control using buttons;
* brightness adjustment;
* settings storage in Flash;
* accelerometer status indication.

## Planned improvements

* Add piezo buzzer / small speaker.
* Add timer finish sound.
* Add sound when the device is flipped.
* Improve internal wiring.
* Replace the debug BlackPill board with a clean board without pin headers.
* Improve the final assembly.

## Differences from the Arduino version

This version was ported to STM32F411CEU6 BlackPill.

Advantages of the STM32 version:

* higher performance;
* smoother sand simulation;
* more memory;
* more room for future expansion;
* convenient STM32 peripheral support;
* settings stored in internal Flash memory.

## Credits

Original project:

* AlexGyver/DigiSand
  https://github.com/AlexGyver/DigiSand

This repository contains my STM32 adaptation of the project.
