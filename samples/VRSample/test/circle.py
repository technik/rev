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
vel = []

for i in range(nSteps):
	t = i*dt
	angle = t*2*pi / period
	ddx = -cos(angle)*radius*w*w
	ddy = -sin(angle)*radius*w*w
	row = [ddx, ddy, 0.0, 0.0, 0.0, 0.0]
	csvWriter.writerow(row)
	data.append(row)
	
x = [r[0] for r in data]
#y = [r[1] for r in data]
	
#plt.plot(x,y)
plt.plot(vel)

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
	
plt.plot(ddx, ddy)

plt.figure(2)
plt.plot([r[0] for r in data])
plt.plot([x for x in ddx])

plt.figure(3)
plt.plot(ppx)
plt.show()