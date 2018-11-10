#include <arduino_etl.h>
#include <Servo.h>
#include <Wire.h>
#include <stdint.h>
#include "hal/mpu6050.h"

OutputPin<Pin13> led;
Servo esc;

/*void wireRead(ImuData& out)
{
  auto dst = reinterpret_cast<uint8_t*>(&out);
  for(unsigned i = 0; i < sizeof(ImuData); ++i)
  {
    *dst = Wire.read();
    ++dst;
  }
}*/

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

const int MPU_addr=0x68;  // I2C address of the MPU-6050

void setupSensor()
{
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6b);
  Wire.write(0);
  Wire.endTransmission(true);
}

void print(const Vec3u& v)
{
  Serial.print("(");
  Serial.print(v.x());
  Serial.print(",");
  Serial.print(v.y());
  Serial.print(",");
  Serial.print(v.z());
  Serial.print(")");
}

void readSensor()
{
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3b);
  Wire.endTransmission(false);
//  Wire.requestFrom(MPU_addr,sizeof(ImuData),true);
  //ImuData state;
  //wireRead(state);
  //Serial.print("\nAcc : "); print(state.accel);
  //Serial.print("; Gyr = "); print(state.gyro);
  //Serial.print("; Tmp = "); Serial.println(state.temp);
}

MsgProcessor stateMachine;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  esc.attach(10);
  esc.write(0);
  setupSensor();
}

unsigned long lastMillis = 0;

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
  // Timed tasks
  auto t = millis();
  if(t - lastMillis > 500)
  {
    lastMillis = t;
    readSensor();
  }
}
