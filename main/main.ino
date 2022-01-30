/**
 * @file main.ino
 * @author Krystian Śliwiński (k.sliwinski@protonmail.com)
 * @brief SecureDoors, Alarm for your doors with call notification.
 * @version 0.1
 * @date 2022-01-30
 * 
 * @copyright Copyright (c) 2022 Krystian Śliwiński, under MIT license.
 * 
 * Parts used in this project:
 *      - Arduino Nano (or UNO, It's basically the same)
 *      - Reed Switch (Normally closed)
 *      - Siren (Or any other sound emmiter working by applying voltage)
 *      - SIM800L EVB 2.0 GSM module.
 *      - 1CH Relay (Low level trigger)
 *      - Pushbutton
 *      - Battery, or any other power source (Prefferably 12V)
 *      - Step-down converter (On output 5V and min. 2.2A)
 * 
 */

// gsm module
#include <SoftwareSerial.h>
SoftwareSerial gsm_conn(11, 12); // 11 -> TX; 12 -> RX

// pins
#define REED_SWITCH 2
#define DISARM_SWITCH 3
#define SIREN 4

// config
#define TIME_FOR_DISARMING 15000 // in ms; [default: 15s (15000ms)]
#define ALARM_DURATION 180000 // in ms, add time for disarming; [default: 3min - TIME_FOR_DISARMING (180000ms)]
#define CALL_DURATION 20000 // in ms [default: 20s (20000ms)]
#define CALL_COUNT 5 // default: 5

bool is_alarm_triggerable = true;

bool is_call_made = false;
byte made_call_count;
unsigned long when_call_made;

bool is_alarm_disarmed = false;

bool is_alarm_toggled = false;
unsigned long when_alarm_toggled;

/// Turns on a SIREN and makes call when door is opened and alarm is not disarmed and has not been disabled
void toggle_alarm() {
    if(digitalRead(REED_SWITCH) && !is_alarm_disarmed && is_alarm_triggerable) {
        if(!is_alarm_toggled) {
            when_alarm_toggled = millis();
            is_alarm_toggled = true;
        } else if(millis() - when_alarm_toggled > TIME_FOR_DISARMING) {
            if(!is_call_made && made_call_count < CALL_COUNT) {
                if(made_call_count != 0) delay(1000);
                gsm_conn.println("AT");
                delay(100);
                gsm_conn.println("ATD+ +48XXXXXXXXX;"); //replace X's with your phone number
                when_call_made = millis();
                is_call_made = true;
                made_call_count++;
            }
            digitalWrite(SIREN, LOW);
        }
    }
}

/// Hangs up a call and turns off a SIREN when alarm disarmed or when alarm was toggled for longer than specified.
/// When alarm was running for longer than specified, it is disabled and cannot be triggered, untill disarmed.
void turn_off_alarm() {
    if(is_alarm_disarmed || (is_alarm_toggled && millis() - when_alarm_toggled > ALARM_DURATION)) {
        if(is_call_made) gsm_conn.println("ATH");
        is_alarm_triggerable = is_alarm_disarmed; // when action toggled because of disarming an alarm, it can still be triggered
        is_call_made = is_alarm_toggled = false;
        made_call_count = 0;
        digitalWrite(SIREN, HIGH);
    }
}

void arm_or_disarm_alarm() {
    if(!digitalRead(DISARM_SWITCH)) {
        is_alarm_disarmed = true; // disarming an alarm
        while(!digitalRead(DISARM_SWITCH)) {
            // while pressed, wait and do nothing
        }
    } else if(!digitalRead(REED_SWITCH)) {
        is_alarm_disarmed = false; // arming an alarm
    }
}

/// Hangs up a call when it is longer than specified
void hang_up_call() {
    if(is_call_made && millis() - when_call_made > CALL_DURATION) {
        is_call_made = false;
        gsm_conn.println("ATH");
    }
}


void setup() {
	pinMode(REED_SWITCH, INPUT_PULLUP);
    pinMode(DISARM_SWITCH, INPUT_PULLUP);
    pinMode(SIREN, OUTPUT);
    digitalWrite(SIREN, HIGH);
    gsm_conn.begin(9600);
}

void loop() {
    arm_or_disarm_alarm();
    toggle_alarm();
    turn_off_alarm();
    hang_up_call();
}