
/*
 * Number of servos
 */
#define SERVO_NUM 6

/*
 * Timer ticks of full servo output period. In the other words it's number of ticks per 10-20ms.
 * F_CPU / 8 / (1 / 0.020)
 */
#define TICKS_PER_PERIOD 30000

/*
 * Available ticks per one servo.
 */
#define TICKS_PER_SERVO (TICKS_PER_PERIOD / SERVO_NUM)

/*
 * Min and max delays (in timer ticks).
 */
#define MIN_TICKS 1050
#define MAX_TICKS 3250

/*
 * Default servos position.
 * Central position can be calculated with this formula:
 * F_CPU / 8 / (1 / ((0.008 + 0.023) / 2))
 */
#define INITIAL_SERVOS_POSITION 1 << 15

/*
 * Output port.
 */
#define SERVO_PORT B

/*
 * Output pins.
 */
const static uint8_t SERVO_PINS[SERVO_NUM] = {1 << PINB1, 1 << PINB2, 0, 0, 0, 0};

/*
 * Set position for specified servo.
 * 0 < servoPos < TICKS_PER_SERVO.
 */
inline void setPosition(uint8_t servoId, uint16_t servoPos);

/*
 * Put servos in default position and init timer.
 */
inline void initServos();

/*
 * PWM calculation loop. Should be invoked as fast as possible.
 * Returns number of ticks before next time-critical pin changes.
 */
inline uint16_t controlServos();
