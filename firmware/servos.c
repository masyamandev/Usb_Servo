
#include <avr/io.h>
#include <avr/interrupt.h>  /* for sei() */
#include "servos.h"

#define PORT_CONCAT(a, b)       a ## b
#define SERVO_OUTPORT           PORT_CONCAT(PORT, B)
#define SERVO_INPORT            PORT_CONCAT(PIN, B)
#define SERVO_DDRPORT           PORT_CONCAT(DDR, B)


struct ServoPos {
	uint16_t pos; // servo position, > 0; < TICKS_PER_SERVO.
} servos[SERVO_NUM];

uint8_t currentServo = 0;
uint8_t currentServoOutput = 0;

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
	uint16_t nextDelay;
	// set pins and calculate next delay
	SERVO_DDRPORT |= SERVO_PINS[currentServo];
	if (currentServoOutput) {
		nextDelay = TICKS_PER_SERVO - servos[currentServo].pos;
		currentServoOutput = 0;
		SERVO_OUTPORT &= ~SERVO_PINS[currentServo];
	} else {
		nextDelay = servos[currentServo].pos;
		currentServoOutput = 1;
		SERVO_OUTPORT |= SERVO_PINS[currentServo];
	}
	// switch to next servo
	if (! currentServoOutput) {
		currentServo ++;
		if (currentServo >= SERVO_NUM) {
			currentServo = 0;
		}
	}
	// init next timer interrupt
	OCR1A = nextDelay;
	TCCR1B |= (1 << CS11); // Set up timer (prescale / 8)
	TIMSK = (1 << OCIE1A); // Enable interrupt on OCR1A
	TCNT1 = 0; // reset timer counter
}

ISR(TIMER1_COMPA_vect) {
	TIMSK = 0; // switch off timer interrupt
	sei(); // switch on USB interrupt
	initNextServoInterrupt();
}
