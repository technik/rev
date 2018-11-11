// MPU 6050 interface
#pragma once

#include "../math/vector.h"

class Mpu6050
{
public:

	// Operation
	void configSensor() const;
	void readSensor();

	// Accessors
	const Vec3u& accel() const { return m_accel; }
	const Vec3u& gyro() const { return m_gyro; }
	unsigned temp() const { return m_temp; }

private:
	void receive(unsigned&);
	void receive(Vec3u&);

private:
	const uint8_t MpuI2cAddress = 0x68;

	Vec3u m_accel, m_gyro;
	unsigned m_temp;
};