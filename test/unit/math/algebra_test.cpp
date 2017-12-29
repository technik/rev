//----------------------------------------------------------------------------------------------------------------------
// Math unit testing
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#include <math/algebra/vector.h>

using namespace rev::math;

//----------------------------------------------------------------------------------------------------------------------
// Test operators on Vec3
void testVec3() {
}

//----------------------------------------------------------------------------------------------------------------------
// Test basic matrix operations
void testMatrix() {
	auto ones = Mat44f::ones();
	auto iden = Mat44f::identity();
	auto zero = Mat44f::zero();
	auto two = ones + ones;
	auto a = two * iden;
	assert(a == two);
	a = two - zero();
	assert(a == two);
	a = ones * 2;
	assert(two == a);
	auto four = two.cwiseProduct(two);
	assert(four == (4.f * ones));
	a = iden.cwiseMax(-iden);
	assert(a == iden);
}

int main() {
	testVec3();
	return 0;
}