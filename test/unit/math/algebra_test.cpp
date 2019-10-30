//----------------------------------------------------------------------------------------------------------------------
// Math unit testing
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#include <math/algebra/matrix.h>
#include <math/algebra/vector.h>
#include <math/algebra/quaternion.h>
#include <math/algebra/affineTransform.h>

using namespace rev::math;

//----------------------------------------------------------------------------------------------------------------------
void testShaderLikeSyntax()
{
    // Simple initialization
    Vec3f a; // Default construction
    Vec3f b = { 0.f, 1.f, 2.f };
    auto c = Vec3f::ones();
    auto d = Vec3f::zero();
    //Vec3f e = 2.f;
    //auto f = Vec3f( Vec2f(1.f, 2.f), 0.f);
    // Vector Operators (dot, cross, normalize, min, max)
    auto bdotc = dot(a, b);
    assert(bdotc == 3.f);
    // Test nan on min,max to behave like simd, return second operator
    // Operations with scalars
    // Utility testing (is normalized)
    // Test named component access

    // Advanced syntax
    // Test initialization from vectors with different base numbers
    // Test initialization of simd vectors by packing or replicating components
}

//----------------------------------------------------------------------------------------------------------------------
// Test matrix vector operations

//----------------------------------------------------------------------------------------------------------------------
// Test operators on Vec3
void testVector() {
    //testShaderLikeSyntax();
	auto x = Vec2f(1.f,0.f);
	assert(norm(x) == squaredNorm(x) == 1.f);
	Vec2f b = { 1.f, 2.f };
	auto ones = Vec2f::ones();

	auto f = dot(x, b);
	assert(f == 1.f);
}

//----------------------------------------------------------------------------------------------------------------------
// Test basic matrix operations
void testMatrix() {
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
	a = max(iden, -iden);
	assert(a == iden);

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
	//{
	//	Mat44f m;
	//	m.col(0).z() = 5.f;
	//	assert(m.row(2).x() == 5.f);
	//}

	// TODO: Test operations with matrices of different base number types
}

bool approx(const Vec3f& a, const Vec3f& b)
{
	return (1-dot(a, b)) < 1e-5f;
}

void testQuaternions()
{
	// TODO: Test axis angle for angles in the 4 basic quadrants
	{
		const Quatf id = Quatf({0.f,0.f,1.f}, 0.f);
		assert(id.x() == 0.f);
		assert(id.y() == 0.f);
		assert(id.z() == 0.f);
		assert(id.w() == 1.f);
	}
	{
		const Quatf id = Quatf({0.f,0.f,1.f}, HalfPi);
		assert(id.x() == 0.f);
		assert(id.y() == 0.f);
		assert(id.z() == sqrt(2.f)*0.5f);
		assert(id.w() == sqrt(2.f)*0.5f);
	}
	{
		const Quatf id = Quatf({0.f,0.f,1.f}, Pi);
		assert(id.x() == 0.f);
		assert(id.y() == 0.f);
		assert(id.z() == 1.0f);
		assert(id.w() == 0.f);
	}
	// Test basic rotations
	Vec3f i = { 1.f, 0.f, 0.f};
	Vec3f j = { 0.f, 1.f, 0.f};
	Vec3f k = { 0.f, 0.f, 1.f};
	{
		const Quatf q = Quatf({0.f,0.f,1.f}, 0.f);
		assert(i == q.rotate(i));
		assert(j == q.rotate(j));
		assert(k == q.rotate(k));
		assert(i == (Mat33f)q * i);
		assert(j == (Mat33f)q * j);
		assert(k == (Mat33f)q * k);
	}
	{
		const Quatf q = Quatf({0.f,0.f,1.f}, HalfPi);
		assert(approx(q.rotate(i), j));
		assert(approx(q.rotate(j), -i));
		assert(approx(q.rotate(-i), -j));
		assert(approx(q.rotate(-j), i));
		assert(approx(q.rotate(k), k));
		assert(approx((Mat33f)q * i, j));
		assert(approx((Mat33f)q * j, -i));
		assert(approx((Mat33f)q * k , k));
	}
	{
		const Quatf q = Quatf({0.f,0.f,1.f}, Pi);
		assert(approx(q.rotate(i), -i));
		assert(approx(q.rotate(j), -j));
		assert(approx(q.rotate(-j), j));
		assert(approx(q.rotate(-i), i));
		assert(approx(q.rotate(k), k));
		assert(approx((Mat33f)q * j, -j));
		assert(approx((Mat33f)q * -i, i));
		assert(approx((Mat33f)q * k , k));
	}
}

void testComposedTransforms()
{
	Vec4f i = { 1.f, 0.f, 0.f, 0.f};
	Vec4f j = { 0.f, 1.f, 0.f, 0.f};
	Vec4f k = { 0.f, 0.f, 1.f, 0.f};
	Vec4f x2 = { 2.f, 0.f, 0.f, 1.f};
	Vec4f y2 = { 0.f, 2.f, 0.f, 1.f};
	Vec4f z2 = { 0.f, 0.f, 2.f, 1.f};
	Mat34f mA = Mat34f::identity();
	mA.block<3,3>(0,0) = (Mat33f)Quatf({0.f,0.f,1.f},HalfPi);
	mA.col(3) = {1.f,0.f,0.f};
	Mat34f mB = Mat34f::identity();
	mB.block<3,3>(0,0) = (Mat33f)Quatf({1.f,0.f,0.f},HalfPi);
	mB.col(3) = {1.f,0.f,0.f};
	// A transform
	assert(approx(mA*i, Vec3f(0.f,1.f,0.f)));
	assert(approx(mA*j, Vec3f(-1.f,0.f,0.f)));
	assert(approx(mA*k, Vec3f(0.f,0.f,1.f)));
	assert(approx(mA*x2, Vec3f(1.f,2.f,0.f)));
	assert(approx(mA*y2, Vec3f(-1.f,0.f,0.f)));
	assert(approx(mA*z2, Vec3f(1.f,0.f,2.f)));
	// B transform
	assert(approx(mB*i, Vec3f(1.f,0.f,0.f)));
	assert(approx(mB*j, Vec3f(0.f,0.f,1.f)));
	assert(approx(mB*k, Vec3f(0.f,-1.f,0.f)));
	assert(approx(mB*x2, Vec3f(3.f,0.f,0.f)));
	assert(approx(mB*y2, Vec3f(1.f,0.f,2.f)));
	assert(approx(mB*z2, Vec3f(1.f,-2.f,0.f)));
}

void testAffineTransform()
{
	Vec3f pos0 = Vec3f(1.f,2.f,3.f);
	AffineTransform x = AffineTransform::identity();
	auto rotation = Mat33f({
		-1.f, 0.f, 0.f,
		0.f, 0.f, 1.f,
		0.f, 1.f, 0.f
		});
	x.position() = Vec3f(0.f,0.f,10.f);
	assert(x.transformPosition(pos0) == Vec3f(1.f,2.f,13.f));
	x.rotate(rotation);
	//assert(x.transformPosition(pos0) == Vec3f(-1.f,3.f,12.f));
}

int main() {
	testMatrix();
	testVector();
	testQuaternions();
	testComposedTransforms();
	testAffineTransform();
	return 0;
}