#include <arduino_etl.h>
#include <Servo.h>
#include <Wire.h>

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

const int MPU_addr=0x68;  // I2C address of the MPU-6050

void setupSensor()
{
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6b);
  Wire.write(0);
  Wire.endTransmission(true);
}

void readSensor()
{
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3b);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);
  auto AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  auto AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  auto AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  auto Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  auto GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  auto GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  auto GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  Serial.print("AcX = "); Serial.println(AcX);
  Serial.print("AcY = "); Serial.println(AcY);
  Serial.print("AcZ = "); Serial.println(AcZ);
  Serial.print("Tmp = "); Serial.println(Tmp/340.00+36.53);  //equation for temperature in degrees C from datasheet
  Serial.print("GyX = "); Serial.println(GyX);
  Serial.print("GyY = "); Serial.println(GyY);
  Serial.print("GyZ = "); Serial.println(GyZ);
  Serial.println("--------------");
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
  if(t - lastMillis > 1000)
  {
    lastMillis = t;
    readSensor();
  }
}
