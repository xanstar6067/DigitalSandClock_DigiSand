# SandClock

Digital sand clock on STM32F411CEU6 BlackPill, two chained MAX7219 8x8 LED
matrices, and an MPU6050 accelerometer.

## Описание

Цифровые песочные часы на STM32F411CEU6 BlackPill, двух последовательно
соединенных матрицах MAX7219 8x8 и акселерометре MPU6050.

Прошивка хранит настройки во flash памяти контроллера:

- установленное время таймера;
- яркость матриц.

Сохранение происходит автоматически примерно через 2 секунды после последнего
изменения времени или яркости. Последний сектор flash зарезервирован под
настройки, поэтому linker script ограничивает область программы первыми 384 KB.

## Подключение Кнопок

Внешние кнопки `EXT_BTN1` и `EXT_BTN2` работают как входы с внутренней подтяжкой
STM32 к питанию. Внешние резисторы не нужны.

Подключение:

```text
GND ---- кнопка ---- PB8  EXT_BTN1
GND ---- кнопка ---- PB9  EXT_BTN2
```

Не подключайте эти кнопки к `3.3V`. Прошивка ожидает, что при нажатии кнопка
замкнет входной пин на землю.

Встроенная кнопка `UKEY` используется как сервисная/тестовая кнопка.

## Управление

`EXT_BTN1`:
- короткое нажатие: увеличить таймер на 1 секунду;
- удержание: увеличивать таймер по 10 секунд.

`EXT_BTN2`:
- короткое нажатие: уменьшить таймер на 1 секунду;
- удержание: уменьшать таймер по 10 секунд.

`EXT_BTN1 + EXT_BTN2` одновременно:
- сбросить песок в стартовую камеру.

`UKEY`:
- короткое нажатие: сбросить песок;
- удержание: переключать яркость матриц.

При изменении времени дисплей на несколько секунд показывает значение в формате
`MMSS`, затем возвращается к анимации песка.

## Индикация ULED

Встроенный светодиод `ULED` на PC13 показывает состояние акселерометра:

- медленное мигание: MPU6050 работает и читается;
- быстрое мигание: MPU6050 не отвечает.

## Настройка Осей И Матриц

Если акселерометр или матрицы установлены иначе, правьте только эти define в
`Core/Inc/sand_clock_config.h`:

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

Текущая прошивка уже настроена под эту сборку: верх/низ и лево/право исправлены
программно.

## English

External buttons are connected as active-low inputs with internal STM32 pull-up
resistors. External resistors are not required.

Connect them like this:

```text
GND ---- button ---- PB8  EXT_BTN1
GND ---- button ---- PB9  EXT_BTN2
```

Do not connect these buttons to 3.3 V. The firmware expects a button press to
short the input pin to GND.

Controls:

- `EXT_BTN1` short press: increase timer by 1 second.
- `EXT_BTN1` hold: increase timer repeatedly by 10 seconds.
- `EXT_BTN2` short press: decrease timer by 1 second.
- `EXT_BTN2` hold: decrease timer repeatedly by 10 seconds.
- `EXT_BTN1 + EXT_BTN2`: reset sand to the starting chamber.
- `UKEY` short press: reset sand.
- `UKEY` hold: cycle display brightness.

Settings are stored in MCU flash automatically about 2 seconds after the last
time or brightness change.

The onboard `ULED` on PC13 shows accelerometer status:

- slow blink: MPU6050 is online;
- fast blink: MPU6050 is not responding.
