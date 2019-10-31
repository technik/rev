//----------------------------------------------------------------------------------------------------------------------
// Math unit testing
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#include <math/algebra/vector.h>
#include <math/algebra/matrix.h>
#include <math/algebra/affineTransform.h>
#include <math/geometry/aabb.h>
#include <math/numericTraits.h>

using namespace rev::math;

bool approx(const Vec3f& a, const Vec3f& b)
{
	return (1 - dot(a, b)) < 1e-5f;
}

void testAABBTransform()
{
	AABB aabb(-Vec3f::ones(), Vec3f::ones());
	{
		auto identity = Mat44f::identity();
		auto result = identity * aabb;
		assert(result.center() == Vec3f::zero());
		assert(result.size() == 2.f*Vec3f::ones());
	}
	{
		auto translation = AffineTransform::identity();
		translation.position() = 3.f * Vec3f::ones();
		auto result = translation.matrix() * aabb;
		assert(result.center() == 3.f * Vec3f::ones());
		assert(result.min() == 2.f * Vec3f::ones());
		assert(result.max() == 4.f * Vec3f::ones());
	}
	{
		const float sqrt05 = sqrt(0.5f);
		Mat44f rotation = {
			sqrt05, -sqrt05, 0.f, 0.f,
			sqrt05, sqrt05, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f
		};
		auto result = rotation * aabb;
		const float sqrt2 = sqrt(2.f);
		assert(result.min() == Vec3f(-sqrt2, -sqrt2, -1.f));
		assert(result.max() == Vec3f(sqrt2, sqrt2, 1.f));
	}

	Vec3f longBoxHalfSize(1.f, 2.f, 5.f);
	auto longBox = AABB(-longBoxHalfSize, longBoxHalfSize);
	{
		// Rotate the box
		auto rotationZ = AffineTransform::identity();
		rotationZ.setRotation(Quatf(Vec3f(0.f, 0.f, 1.f), -Constants<float>::halfPi));
		auto result = rotationZ.matrix() * longBox;
		auto rotatedSize = 2.f*Vec3f(2.f, 1.f, 5.f);
		assert(approx(result.size(), rotatedSize));
		assert(result.center() == Vec3f::zero());

		// Rotate then move
		auto translation = AffineTransform::identity();
		auto offset = Vec3f(1.f, 0.f, 0.f);
		translation.position() = offset;
		result = translation.matrix() * result;
		assert(result.center() == offset);
		assert(approx(result.size(), rotatedSize));

		// Move then rotate
		result = rotationZ.matrix() * (translation.matrix() * longBox);
		assert(approx(result.size(), rotatedSize));
		assert(approx(result.center(), Vec3f(0.f, -1.f, 0.f)));
	}
}

int main()
{
	testAABBTransform();
	return 0;
}