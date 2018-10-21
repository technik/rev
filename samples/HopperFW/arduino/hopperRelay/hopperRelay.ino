#include <arduino_etl.h>
#include <Servo.h>

OutputPin<Pin13> led;
Servo esc;

struct MsgProcessor
{
  enum class State
  {
    header0,
    header1,
    led,
    pwm
  } curState = State::header0;

  uint8_t led;
  uint8_t pwm = 0;
  
  void consume(uint8_t data)
  {
    switch(curState)
    {
      case State::header0:
        if(data == 'H')
          curState = State::header1;
        break;
      case State::header1:
        if(data == 'R')
          curState = State::led;
        else
          curState = State::header0;
        break;
      case State::led:
        led = data;
        curState = State::pwm;
        break;
      case State::pwm:
        pwm = data;
        curState = State::header0;
        break;
    }
  }
};

MsgProcessor stateMachine;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  esc.attach(10);
  esc.write(0);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available())
  {
    stateMachine.consume(Serial.read());
    if(stateMachine.led == 'A')
      led.setHigh();
    else
      led.setLow();

    esc.write(map(stateMachine.pwm, 0, 255, 0, 180));
  }
}
