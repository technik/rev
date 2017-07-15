# Generate data for a circular path trajectory, no noise
import csv
from math import *
import numpy as np
from matplotlib import pyplot as plt

# Sampling info
duration = 35.0
period = 10.0
dt = 0.1
radius = 1.0
nSteps = int(1+duration/dt)
w = 2*pi/period
print(w)

# generate simulation data
csvFile = open("circle.csv", 'w', newline='\n');
csvWriter = csv.writer(csvFile, delimiter=",");

data = [] # Accumulate data here so we can reconstruct the trajectory with a filter later
gt = []
T = []

for i in range(nSteps):
	t = i*dt
	T.append(t)
	angle = t*2*pi / period
	ddx = -cos(angle)*radius*w*w
	ddy = -sin(angle)*radius*w*w
	gt.append([cos(angle)*radius, sin(angle)*radius])
	row = [ddx, ddy, 0.0, 0.0, 0.0, 0.0]
	csvWriter.writerow(row)
	data.append(row)
	
# ground truth
gtx = np.array([r[0] for r in gt])
gty = np.array([r[1] for r in gt])

# --------------------- Reconstruction ---------------------
x = np.array([radius, 0, 0, 0, radius*w, 0, -radius*w*w, 0, 0]) # State estimate: pos, vel, accel
# Model transition function
F = np.array([	[1, 0, 0,dt, 0, 0, 0, 0, 0], # p' = p + dt*v
				[0, 1, 0, 0,dt, 0, 0, 0, 0],
				[0, 0, 1, 0, 0,dt, 0, 0, 0],
				[0, 0, 0, 1, 0, 0,dt, 0, 0], # v' = v + dt*a
				[0, 0, 0, 0, 1, 0, 0,dt, 0],
				[0, 0, 0, 0, 0, 1, 0, 0,dt],
				[0, 0, 0, 0, 0, 0, 1, 0, 0], # a' = a
				[0, 0, 0, 0, 0, 0, 0, 1, 0],
				[0, 0, 0, 0, 0, 0, 0, 0, 1] ])
p0p = 0.0001 # initial uncertainty on position
p0va = 1000.0 # initial uncertainty on vel and accel
P = np.identity(9) * p0va
P[0,0] = p0p
P[1,1] = p0p
P[2,2] = p0p

Q = np.identity(9) * dt * radius * w; # This can actually be improved. Maybe derived from actual data

# Observation model
H = np.array([	[0, 0, 0, 0, 0, 0, 1, 0, 0],
				[0, 0, 0, 0, 0, 0, 0, 1, 0],
				[0, 0, 0, 0, 0, 0, 0, 0, 1] ])

R = np.identity(3)*0.0001 # measurement noise
I = np.identity(9)

# Aux variables to store simulation signals for later analysis
px = [x[0]]
py = [x[1]]
ppx = [p0va]
ddx = [x[6]]
ddy = [x[7]]
# Actual reconstruction loop
# Update occurs before prediction because first measurement occurs at time t=0
for i in range(nSteps):
	# Update
	z = data[i][0:3] # read accelerations
	y = z - np.dot(H,x)
	S = np.dot(np.dot(H,P),H.transpose()) + R
	invS = np.linalg.inv(S)
	HinvS = np.dot(H.transpose(),invS)
	K = np.dot(P,HinvS)
	deltaX = np.dot(K,y)
	x = x + deltaX
	P = np.dot(I-np.dot(K,H),P)
	# Predict
	x = np.dot(F,x)
	P = np.dot(np.dot(F,P),F.transpose()) + Q
	# extract data
	ppx.append(P[6,6])
	px.append(x[0])
	py.append(x[1])
	ddx.append(x[6])
	ddy.append(x[7])

"""
plt.plot(px, py)
plt.plot(gtx, gty)
plt.show()
"""

# accel error
errorX = np.abs(np.array(ddx[1:]) - np.array([r[0] for r in data]));
print("Max error in x accel axis: ", np.max(errorX));
errorY = np.abs(np.array(ddy[1:]) - np.array([r[1] for r in data]));
print("Max error in y accel axis: ", np.max(errorY));
# pos error (drifts with time)
minDriftT = 50;
errorX = np.abs((np.array(ddx[minDriftT+2:]) - gtx[minDriftT+1:]) / (T[minDriftT+1:]));
print("Max time corrected error in x axis: ", np.max(errorX));
errorY = np.abs((np.array(ddy[minDriftT+2:]) - gty[minDriftT+1:]) / (T[minDriftT+1:]));
print("Max time corrected error in y axis: ", np.max(errorY));