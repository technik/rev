//----------------------------------------------------------------------------------------------------------------------
// Math unit testing
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#include <math/algebra/matrix.h>
#include <math/algebra/vector.h>

using namespace rev::math;

//----------------------------------------------------------------------------------------------------------------------
// Test operators on Vec3
void testVector() {
	auto x = Vec2f(1.f,0.f);
	assert(x.norm() == x.squaredNorm() == 1.f);
	Vec2f b = { 1.f, 2.f };
	auto ones = Vec2f::ones();

	auto f = x.dot(b);
	assert(f == 1.f);
}

//----------------------------------------------------------------------------------------------------------------------
// Test basic matrix operations
void testMatrix() {
	static_assert(Mat44f::cols == 4, "Error: Matrix has wrong number of columns");
	static_assert(Mat44f::rows == 4, "Error: Matrix has wrong number of rows");
	auto ones = Mat44f::ones();
	auto iden = Mat44f::identity();
	auto zero = Mat44f::zero();
	auto a = iden;
	assert(a == iden);
	a(1,2) = 4;
	assert(4.f == a(1,2));
	auto two = ones + ones;
	a = two * iden;
	assert(a == two);
	a = two - zero;
	assert(a == two);
	a = ones * 2.f;
	assert(two == a);
	auto four = two.cwiseProduct(two);
	assert(four == (4.f * ones));
	a = four * iden;
	assert(four == a);
	a = iden.cwiseMax(-iden);
	assert(a == iden);

	Mat22f b = {
		0.f, 1.f,
		2.f, 3.f
	};
	assert(b(1,0) == 2.f);

	{
		Vec3f t;
		t(0,0) = 0.f;
		t(1,0) = 1.f;
		t(2,0) = 2.f;
		assert(t.x() == 0.f);
		assert(t.y() == 1.f);
		assert(t.z() == 2.f);
	}
	auto ten = 10.f * Vec3f::ones();
	assert(ten.x() == 10.f);
	assert(ten.y() == 10.f);
	assert(ten.z() == 10.f);

	// Access rows and columns as referencing types
	{
		Mat44f m;
		m.col(0).z() = 5.f;
		assert(m.row(2).x() == 5.f);
	}

	// TODO: Test operations with matrices of different base number types
}

void testQuaternions()
{
	// Test axis angle for angles in the 4 basic quadrants
	{
		Quaternion q({0,0,1}, )
	}

	// Test other constructors
	// Test casting to Matrix
	// Test compositions
	// Test vector rotations
}

int main() {
	testMatrix();
	testVector();
	return 0;
}