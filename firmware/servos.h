
/*
 * Number of servos
 */
#define SERVO_NUM 2

/*
 * Timer ticks of full servo output period. In the other words it's number of ticks per 10-20ms.
 * F_CPU / 8 / (1 / 0.020)
 */
#define TICKS_PER_PERIOD 28000

/*
 * Available ticks per one servo.
 */
#define TICKS_PER_SERVO (TICKS_PER_PERIOD / SERVO_NUM)

/*
 * Default servos position.
 * Central position can be calculated with this formula:
 * F_CPU / 8 / (1 / ((0.008 + 0.023) / 2))
 */
#define INITIAL_SERVOS_POSITION 2360

/*
 * Output port.
 */
#define SERVO_PORT B

/*
 * Output pins.
 */
const static uint8_t SERVO_PINS[SERVO_NUM] = {1 << PINB1, 1 << PINB2};

/*
 * Set position for specified servo.
 * 0 < servoPos < TICKS_PER_SERVO.
 */
inline void setPosition(uint8_t servoId, uint16_t servoPos);

/*
 * Init interrupt for controlling servos.
 */
// inline void initNextServoInterrupt();

/*
 * Put servos in default position and init timer.
 */
inline void initServos();
