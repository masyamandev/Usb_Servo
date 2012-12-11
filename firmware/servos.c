
#include <avr/io.h>
#include <avr/interrupt.h>  /* for sei() */
#include "servos.h"

#define PORT_CONCAT(a, b)             a ## b
#define SERVO_OUTPORT(name)           PORT_CONCAT(PORT, name)
#define SERVO_INPORT(name)            PORT_CONCAT(PIN, name)
#define SERVO_DDRPORT(name)           PORT_CONCAT(DDR, name)


struct ServoPos {
	uint16_t pos; // servo position, > 0; < TICKS_PER_SERVO.
} servos[SERVO_NUM];

uint8_t currentServo = 0;
uint8_t currentServoOutput = 0;
uint16_t currentServoDelay = 0;

inline void setPosition(uint8_t servoId, uint16_t servoPos) {
	// Check overflow
	if (servoPos <= 0) {
		servos[servoId].pos = 1;
	} else if (servoPos >= TICKS_PER_SERVO) {
		servos[servoId].pos = TICKS_PER_SERVO - 1;
	} else {
		servos[servoId].pos = servoPos;
	}
}

inline void initNextServoInterrupt() {
	// set pins and calculate next delay
	SERVO_DDRPORT(SERVO_PORT) |= SERVO_PINS[currentServo];
	if (currentServoOutput) {
		currentServoDelay = TICKS_PER_SERVO - servos[currentServo].pos;
		currentServoOutput = 0;
		SERVO_OUTPORT(SERVO_PORT) &= ~SERVO_PINS[currentServo];
	} else {
		currentServoDelay = servos[currentServo].pos;
		currentServoOutput = 1;
		SERVO_OUTPORT(SERVO_PORT) |= SERVO_PINS[currentServo];
	}
	// switch to next servo
	if (! currentServoOutput) {
		currentServo ++;
		if (currentServo >= SERVO_NUM) {
			currentServo = 0;
		}
	}
	// init next timer interrupt
	TCNT1 = 0; // reset timer counter

//	OCR1A = currentServoDelay;
//	TIMSK = (1 << OCIE1A); // Enable interrupt on OCR1A
}

inline void initServos() {
	int i;
	for (i = 0; i < SERVO_NUM; i++) {
		servos[i].pos = INITIAL_SERVOS_POSITION;
	}
	initNextServoInterrupt();

	// Init timer
	TCCR1B |= (1 << CS11); // Set up timer (prescale / 8)
	TCNT1 = 0; // reset timer counter
//	sei();
}

//ISR(TIMER1_COMPA_vect) {
//	TIMSK = 0; // switch off timer interrupt
//	sei(); // switch on USB interrupt
//	initNextServoInterrupt();
//}

inline uint16_t controlServos() {
	if (TCNT1 >= currentServoDelay) {
		initNextServoInterrupt();
	}
	return currentServoDelay - TCNT1;
}
