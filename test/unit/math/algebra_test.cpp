//----------------------------------------------------------------------------------------------------------------------
// Math unit testing
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#include <math/algebra/matrix.h>

using namespace rev::math;

//----------------------------------------------------------------------------------------------------------------------
// Test operators on Vec3
void testVec3() {
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

	// TODO: Access rows and columns as referencing types
	// TODO: Test operations with matrices of different base number types
}

int main() {
	testMatrix();
	return 0;
}