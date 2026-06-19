#ifndef INC_SAND_CLOCK_CONFIG_H_
#define INC_SAND_CLOCK_CONFIG_H_

#define SAND_CLOCK_PARTICLE_COUNT       55U
#define SAND_CLOCK_DEFAULT_SECONDS      60
#define SAND_CLOCK_DEFAULT_BRIGHTNESS   5
#define SAND_CLOCK_MAX_SECONDS          5999

#define SAND_CLOCK_TIME_POPUP_MS        3000U
#define SAND_CLOCK_BUTTON_DEBOUNCE_MS   25U
#define SAND_CLOCK_BUTTON_HOLD_MS       700U
#define SAND_CLOCK_BUTTON_STEP_MS       250U

#define SAND_CLOCK_LED_ACTIVE_LOW       1

/*
 * BlackPill PA0 user key is normally a wake-up key. Most boards drive it high
 * when pressed, so it is configured as active-high with pulldown.
 * Change this to 1 if your exact board shorts PA0 to GND when pressed.
 */
#define SAND_CLOCK_UKEY_ACTIVE_LOW      0

/*
 * MPU axis mapping. These values reproduce the AlexGyver sketch:
 *   screen X <- raw Y inverted
 *   screen Y <- raw Z
 *   depth    <- raw X
 * Flip signs or axes here if the sensor is mounted differently.
 */
#define SAND_CLOCK_IMU_X_AXIS           1U
#define SAND_CLOCK_IMU_X_SIGN           (-1)
#define SAND_CLOCK_IMU_Y_AXIS           2U
#define SAND_CLOCK_IMU_Y_SIGN           (1)
#define SAND_CLOCK_IMU_Z_AXIS           0U
#define SAND_CLOCK_IMU_Z_SIGN           (1)

/*
 * Display orientation helpers. Keep them at 0 first; they are intentionally
 * simple switches for the common cases after the hardware is alive.
 */
#define SAND_CLOCK_DISPLAY_MIRROR_X     0
#define SAND_CLOCK_DISPLAY_MIRROR_Y     0

#endif /* INC_SAND_CLOCK_CONFIG_H_ */
