/**
 * @file alt.ino
 * @author Krystian Śliwiński (k.sliwinski@protonmail.com)
 * @brief SecureDoors, Alarm for your doors with sms notification.
 * @version 0.1
 * @date 2022-02-24
 *
 * @copyright Copyright (c) 2022 Krystian Śliwiński, under MIT license
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
 * This is an alternative version of an alarm device.
 * Upon toggling an alarm there is an SMS sent instead of calls
 * Disarmed state is active for declared time, not until the door is closed
 *
 */

// gsm module
#include <SoftwareSerial.h>
SoftwareSerial gsm_conn(11, 12); // 11 -> TX; 12 -> RX

// pins
#define REED_SWITCH 2
#define DISARM_SWITCH 3
#define SYREN 4

// config
#define TIME_FOR_DISARMING 3000       // in ms
#define DISARMED_STATE_DURATION 10000 // in ms
#define ALARM_DURATION 15000          // in ms, add time for disarming

bool is_alarm_triggerable = true;

bool is_sms_sent = false;

bool is_alarm_disarmed = false;
unsigned long when_alarm_disarmed;

bool is_alarm_toggled = false;
unsigned long when_alarm_toggled;

// Turns on a syren and sends an sms when door is opened and alarm is not disarmed and has not been disabled
void toggle_alarm()
{
    if (digitalRead(REED_SWITCH) && !is_alarm_disarmed && is_alarm_triggerable)
    {
        if (!is_alarm_toggled)
        {
            when_alarm_toggled = millis();
            is_alarm_toggled = true;
        }
        else if (millis() - when_alarm_toggled > TIME_FOR_DISARMING)
        {
            if (!is_sms_sent)
            {
                gsm_conn.print("AT+CMGF=1\r");
                delay(100);
                gsm_conn.print("AT+CMGS=\"+48XXXXXXXXX\"\r"); // replace X's with your phone number
                delay(500);
                gsm_conn.print("OTWORZONO DRZWI, Alarm włączony"); // replace with your own message
                delay(500);
                gsm_conn.print((char)26);
                delay(500);
                gsm_conn.println();
            }
            is_sms_sent = true;
            digitalWrite(SYREN, HIGH);
        }
    }
}

// turns off a syren when alarm disarmed or when alarm was toggled for longer than specified.
// When alarm was running for longer than specified, it is disabled and cannot be triggered, untill disarmed.
void turn_off_alarm()
{
    if (is_alarm_disarmed || (is_alarm_toggled && millis() - when_alarm_toggled > ALARM_DURATION))
    {
        is_alarm_triggerable = is_alarm_disarmed;
        is_sms_sent = is_alarm_toggled = false;
        digitalWrite(SYREN, LOW);
    }
}

void arm_or_disarm_alarm()
{
    // disarming an alarm
    if (!digitalRead(DISARM_SWITCH))
    {
        is_alarm_disarmed = is_alarm_triggerable = true;
        is_alarm_toggled = false;
        when_alarm_disarmed = millis();
        while (!digitalRead(DISARM_SWITCH))
        {
            // while pressed do nothing
        }
    }

    // arming an alarm
    if (millis() - when_alarm_disarmed > DISARMED_STATE_DURATION)
    {
        is_alarm_disarmed = false;
    }
}

void setup()
{
    pinMode(REED_SWITCH, INPUT_PULLUP);
    pinMode(DISARM_SWITCH, INPUT_PULLUP);
    pinMode(SYREN, OUTPUT);
    gsm_conn.begin(9600);
}

void loop()
{
    arm_or_disarm_alarm();
    toggle_alarm();
    turn_off_alarm();
}