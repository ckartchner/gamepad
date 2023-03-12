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
int all_button_count = button_count;

Bounce* buttons = new Bounce[button_count];

class Knob
{
public:
  long current_position;
  byte pin1;
  byte pin2;
  byte cw_button;
  byte ccw_button;
  Encoder* encoder;
  Knob(byte pin1, byte pin2)
  {
    pin1 = pin1;
    pin2 = pin2;

    cw_button = all_button_count;
    all_button_count++;
    ccw_button = all_button_count;
    all_button_count++;

    current_position = 7;
    encoder = new Encoder(pin1, pin2);
  }
};

Knob allKnobs[] = {
  Knob(0, 1),
  Knob(2, 3)
};

Joystick_ Joystick(
    JOYSTICK_DEFAULT_REPORT_ID,
    JOYSTICK_TYPE_GAMEPAD,
    all_button_count, 0, // Button Count, Hat Switch Count
    false, false, false, // No X, Y, or Z Axis
    false, false, false, // No Rx, Ry, or Rz
    false, false,        // No rudder or throttle
    false, false, false  // No accelerator, brake, or steering
);

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
  buttonInit();
  Joystick.begin();
}

int ledState = 1;
long newPos;

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
  for (Knob& curr_knob : allKnobs) // & refers to original object and not a copy
  {
    newPos = curr_knob.encoder->read();
    if (newPos != curr_knob.current_position)
    {
      Serial.print("Current buttons: ");
      Serial.print(curr_knob.cw_button);
      Serial.print(" ");
      Serial.print(curr_knob.ccw_button);
      Serial.print(" curr pos: ");
      Serial.print(curr_knob.current_position);
      Serial.print(" new pos: ");
      Serial.println(newPos);

      if (newPos > curr_knob.current_position)
      {
        Joystick.setButton(curr_knob.cw_button, 1);
        delay(50);
        Joystick.setButton(curr_knob.cw_button, 0);
      }
      else
      {
        Joystick.setButton(curr_knob.ccw_button, 1);
        delay(50);
        Joystick.setButton(curr_knob.ccw_button, 0);
      }
      curr_knob.current_position = newPos;
    }
  }
}