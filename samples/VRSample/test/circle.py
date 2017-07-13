# Generate data for a circular path trajectory, no noise
import csv
from math import *

# Sampling info
duration = 10.0
period = 10.0
dt = 0.1
radius = 1.0
nSteps = int(1+duration/dt)

# generate simulation data
csvFile = open("circle.csv", 'w', newline='\n');
csvWriter = csv.writer(csvFile, delimiter=",");

for i in range(nSteps):
	t = i*dt
	angle = t*2*pi / period
	ddx = -cos(angle)*radius
	ddy = -sin(angle)*radius
	csvWriter.writerow([ddx, ddy, 0.0, 0.0, 0.0, 0.0])
