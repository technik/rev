// MPU 6050 interface
#include "mpu6050.h"
#include <Wire.h>

void Mpu6050::configSensor() const
{
	Wire.begin();
	Wire.beginTransmission(MPU_addr);
	Wire.write(0x6b);
	Wire.write(0);
	Wire.endTransmission(true);
}

coid Mpu6050::readSensor()
{
	Wire.beginTransmission(MPU_addr);
	Wire.write(0x3b);
	Wire.endTransmission(false);
	Wire.requestFrom(MPU_addr,sizeof(ImuData),true);
	receive(m_accel);
	receive(m_temp);
	receive(m_gyro);
}

void Mpu6050::receive(unsigned& dst)
{
	dst = (unsigned(Wire.read())<<8)|Wire.read();
}

void Mpu6050::receive(Vec3u& v)
{
	receive(v.x());
	receive(v.y());
	receive(v.z());
}
