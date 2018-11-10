// Vector
#pragma once

struct Vec3u
{
	unsigned& x() { return m[0]; }
	unsigned& y() { return m[1]; }
	unsigned& z() { return m[2]; }
	unsigned x() const { return m[0]; }
	unsigned y() const { return m[1]; }
	unsigned z() const { return m[2]; }
	
private:
	unsigned m[3];
};