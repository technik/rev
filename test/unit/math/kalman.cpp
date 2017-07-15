//----------------------------------------------------------------------------------------------
// Test: Reconstruct a circular trajectory using the kalman filter
//----------------------------------------------------------------------------------------------
#include <filters.h>
#include <constants.h>
#include <algebra/vector.h>
#include <vector>

using namespace rev::math;
using namespace std;

int main() {
	// Trajectory parameters
	float duration = 35.0f;
	float period = 10.0f;
	float dt = 0.1f;
	float radius = 1.0f;
	size_t nSteps = size_t(1 + duration / dt);
	float w = Constant<float>::TwoPi / period;

	// Generate trajectory data
	vector<Vec3f> groundTruth(nSteps);
	vector<Vec3f> acceleration(nSteps);

	for (size_t i = 0; i < nSteps; ++i) {
		float t = i*dt;
		float angle = t * w;
		// Acceleration
		float ddx = -cos(angle)*radius*w*w;
		float ddy = -sin(angle)*radius*w*w;
		acceleration[i] = { ddx, ddy, 0.f };
		// Exact position
		groundTruth[i] = Vec3f(cos(angle), sin(angle), 0.f) * radius;
	}

	//
	return 0;
}