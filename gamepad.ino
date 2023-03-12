// Simple plan
// 1 rotary encoder
// 3 buttons -- add more later
// arduino pro micro

// Joystick library: https://github.com/MHeironimus/ArduinoJoystickLibrary
// Encoder library: https://github.com/PaulStoffregen/Encoder

// Avoid using the interrupt pins for the buttons so they can be fully leveraged by the encoders
// pin 0 is interrupt 2 (INT2),
// pin 1 is interrupt 3 (INT3),
// pin 2 is interrupt 1 (INT1),
// pin 3 is interrupt 0 (INT0),
// pin 7 is interrupt 4 (INT6).
// https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/

#include <Joystick.h>
#include <Encoder.h>
#include <Bounce2.h>

int rx_led = 17;

const int button_pins[] = {10, 16, 14, 15, 18};
const int button_count = sizeof(button_pins) / sizeof(int);
const int knob_count = 1;
const int all_button_count = button_count + (knob_count * 2);

Bounce* buttons = new Bounce[button_count];

Joystick_ Joystick(
    JOYSTICK_DEFAULT_REPORT_ID,
    JOYSTICK_TYPE_GAMEPAD,
    all_button_count, 0, // Button Count, Hat Switch Count
    false, false, false, // No X, Y, or Z Axis
    false, false, false, // No Rx, Ry, or Rz
    false, false,        // No rudder or throttle
    false, false, false  // No accelerator, brake, or steering
);

class Knob
{
public:
  long current_position;
  byte pin1;
  byte pin2;
  Encoder* encoder;
  Knob(byte pin1, byte pin2)
  {
    pin1 = pin1;
    pin2 = pin2;
    current_position = 0;
    encoder = new Encoder(pin1, pin2);
  }
};

Knob knobA = Knob(0, 1);
// Knob knobB(2, 3);
// Knob knobs[2] = {knobA, knobB};

void buttonInit()
{
  for (int i = 0; i < button_count; i++)
  {
    buttons[i].attach(button_pins[i], INPUT_PULLUP);
    buttons[i].interval(25);
  }
}

void setup()
{
  Serial.begin(9600);
  // Serial.println("starting setup");
  buttonInit();
  Joystick.begin();
  // Serial.println("setup over");
}

int ledState = 1;
// long positionA = -999;
// long positionRight = -999;
long newPos, newLeft, newRight;

void loop()
{
  // read pin values for all buttons -> send signal if changed
  for (int i = 0; i < button_count; i++)
  {
    buttons[i].update();
    // Toggle
    if (buttons[i].fell())
    {
      ledState = !ledState;
      digitalWrite(rx_led, ledState);
    }
    // on/off
    if (buttons[i].changed())
    {
      Joystick.setButton(i, !buttons[i].read());
    }
  }
  // read rotary encoders
  // TODO: for knob in knobs
  newPos = knobA.encoder->read();
  if (newPos != knobA.current_position)
  {
    Serial.print("New position: ");
    Serial.println(newPos);
    if (newPos > knobA.current_position)
    {
      // TODO: set button number as an object property
      Joystick.setButton(5, 1);
      delay(50);
      Joystick.setButton(5, 0);
    }
    else
    {
      Joystick.setButton(6, 1);
      delay(50);
      Joystick.setButton(6, 0);
    }
    knobA.current_position = newPos;
    // Serial.print("Left = ");
    // Serial.println();
    // positionLeft = newLeft;
  }
}