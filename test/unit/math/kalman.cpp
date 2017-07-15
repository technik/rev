//----------------------------------------------------------------------------------------------
// Test: Reconstruct a circular trajectory using the kalman filter
//----------------------------------------------------------------------------------------------
#include <filters.h>
#include <constants.h>
#include <algebra/vector.h>
#include <vector>

using namespace Eigen;
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

	// ----- Generate trajectory data -----
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

	// ----- Model definition -----
	// Model transition function
	MatrixXf F;
	F << 1, 0, 0,dt, 0, 0, 0, 0, 0, // p' = p + dt*v
		 0, 1, 0, 0,dt, 0, 0, 0, 0,
		 0, 0, 1, 0, 0,dt, 0, 0, 0,
		 0, 0, 0, 1, 0, 0,dt, 0, 0, // v' = v + dt*a
		 0, 0, 0, 0, 1, 0, 0,dt, 0,
		 0, 0, 0, 0, 0, 1, 0, 0,dt,
		 0, 0, 0, 0, 0, 0, 1, 0, 0, // a' = a
		 0, 0, 0, 0, 0, 0, 0, 1, 0,
		 0, 0, 0, 0, 0, 0, 0, 0, 1;
	MatrixXf Q = MatrixXf::Identity(9,9) * dt * radius * w; // This can actually be improved.Maybe derived from actual data
	// Observation model
	MatrixXf H = MatrixXf::Zero(3, 9);
	H.block<3, 3>(0, 6) = Matrix3f::Identity();
	MatrixXf R = MatrixXf::Identity(3, 3)*0.0001; // measurement noise

	// ----- Initial state estimation -----
	VectorXf x0;
	x0 << radius, 0, 0, 0, radius*w, 0, -radius*w*w, 0, 0; // State estimate : pos, vel, accel
	float p0p = 0.0001; // initial uncertainty on position
	float p0va = 1000.0; // initial uncertainty on vel and accel
	MatrixXf P = MatrixXf::Identity(9, 9) * p0va;
	P.block<3, 3>(0, 0) = Matrix3f::Identity() * p0p;

	//
	return 0;
}