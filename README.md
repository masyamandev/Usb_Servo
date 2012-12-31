Usb_Servo
=========

USB controller for servo motors based on AtMega8.

ATMega8 doesn't have hardware USB, so it's emulated using V-USB library (http://www.obdev.at/products/vusb/index.html).

Circuit part for USB is standart for V-USB library, so any of available circuits in V-USB examples should work. For refference download latest V-USB archive (http://www.obdev.at/products/vusb/download.html) and look under 'circuits' folder.

Circuit part for servos should have external power source. 

Servo connection is th following:

External power     |  Servo connector           |    Control circuit

                      Signal (white or yellow) ----< Signal pin
Ext. +4.8..6.0V >---- [+]    (red)
Ext. GRND       >---- [-]    (black or brown)  ----< GRND

Signal pins can be set up in servos.h file. You can change SERVO_PORT and SERVO_PINS values. Currently two servos are connected to PORTB to PIN1 and PIN2 (pins 15 and 16 in PDIP package). 


==============================
Using comand line control tool
==============================

To send set servos in some positions, the following command should be executed:
hidtool write [pos1] [pos2] ...

Positions [posx] should be in range 0..65535.
Aternative command is:
echo [pos1] [pos2] ... | hidtool writeinput

To read last written values execute command:
hidtool read
