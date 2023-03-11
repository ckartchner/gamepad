// Simple plan
// 1 rotary encoder
// 3 buttons -- add more later
// arduino pro micro

// Joystick library: https://github.com/MHeironimus/ArduinoJoystickLibrary
// Encoder library: https://github.com/PaulStoffregen/Encoder

// Avoid using the interrupt pins for the buttons so they can be fully leveraged by the encoders
// pin 3 is interrupt 0 (INT0),
// pin 2 is interrupt 1 (INT1),
// pin 0 is interrupt 2 (INT2),
// pin 1 is interrupt 3 (INT3),
// pin 7 is interrupt 4 (INT6).
// https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/

#include <Joystick.h>
#include <Encoder.h>
#include <Bounce2.h>

int rx_led = 17;

const int button_pins[] = {10, 16, 14, 15, 18};
const int button_count = sizeof(button_pins) / sizeof(int);

Bounce * buttons = new Bounce[button_count];

Joystick_ Joystick(
    JOYSTICK_DEFAULT_REPORT_ID,
    JOYSTICK_TYPE_GAMEPAD,
    button_count, 0,     // Button Count, Hat Switch Count
    false, false, false, // No X, Y, or Z Axis
    false, false, false, // No Rx, Ry, or Rz
    false, false,        // No rudder or throttle
    false, false, false  // No accelerator, brake, or steering
);

void rotaryInit() {
}

void buttonInit()
{
  for (int i = 0; i < button_count; i++) {
    buttons[i].attach(button_pins[i], INPUT_PULLUP);
    buttons[i].interval(25);
  }
}

void setup()
{
  // Serial.begin(9600);
  // Serial.println("starting setup");
  buttonInit();
  rotaryInit();
  Joystick.begin();
  // Serial.println("setup over");
}

int ledState = 1;

void loop()
{
  // read pin values for all buttons -> send signal if changed
  for (int i=0; i < button_count; i++) {
    buttons[i].update();
    // Toggle
    if (buttons[i].fell()) {
      ledState = !ledState;
      digitalWrite(rx_led, ledState);
    }
    // on/off
    if (buttons[i].changed()) {
      Joystick.setButton(i, !buttons[i].read());
    }
  }
}