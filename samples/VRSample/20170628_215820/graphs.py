import csv
import matplotlib.pyplot as plot
import numpy as np

x = []
y = []
z = []
dx = []
dy = []
dz = []
ddx = []
ddy = []
ddz = []

camLogFile = open('../camLog.csv', 'rt')
camLog = csv.reader(camLogFile, delimiter=',')

for row in camLog:
	x.append(float(row[0]))
	y.append(float(row[1]))
	z.append(float(row[2]))
	dx.append(float(row[3]))
	dy.append(float(row[4]))
	dz.append(float(row[5]))
	ddx.append(float(row[6]))
	ddy.append(float(row[7]))
	ddz.append(float(row[8]))
nSamples = len(x);

dt = 0.1
t = np.arange(0.0, dt*nSamples, dt)
"""
plot.figure(1)
plot.plot(t,x,'r')
plot.plot(t,y,'g')
plot.plot(t,z,'b')

plot.figure(2)
plot.plot(x, y)
plot.figure(3)
plot.plot(t,dx,'r')
plot.plot(t,dy,'g')
plot.plot(t,dz,'b')
plot.figure(4)
plot.plot(dx, dy)
"""
plot.figure(5)
plot.plot(t,ddx,'r')
plot.plot(t,ddy,'g')
plot.plot(t,ddz,'b')
plot.figure(6)
plot.plot(ddx, ddy)


x = [0]
y = [0]
z = [0]
dx = [0]
dy = [0]
dz = [0]
ddx = []
ddy = []
ddz = []

rawData = csv.reader(open('20170628_215820.csv', 'rt'), delimiter=',')

for row in rawData:
	ddx.append(float(row[0]))
	ddy.append(float(row[1]))
	ddz.append(float(row[2]))
nSamples = len(ddx);
"""
ddx = np.array(ddx).astype(np.float)
ddy = np.array(ddy).astype(np.float)
ddz = np.array(ddz).astype(np.float)
	
xDrift = np.average(ddx);
yDrift = np.average(ddy);
zDrift = np.average(ddz);

print(xDrift)
print(yDrift)
print(zDrift)

dt = 0.1

for i in range(nSamples):
	x.append(x[i] + dx[i]*dt)
	y.append(y[i] + dy[i]*dt)
	z.append(z[i] + dz[i]*dt)
	dx.append(dx[i] + (ddx[i]-xDrift) * dt)
	dy.append(dy[i] + (ddy[i]-yDrift) * dt)
	dz.append(dz[i] + (ddz[i]-zDrift) * dt)

t = np.arange(0.0, dt*nSamples, dt)

plot.figure(3)
plot.plot(t,x[0:nSamples])
plot.plot(t,y[0:nSamples])
plot.plot(t,z[0:nSamples])
#plot.figure(4)
#plot.plot(t,dx[0:nSamples])
#plot.plot(t,dy[0:nSamples])
#plot.plot(t,dz[0:nSamples])
"""
plot.figure(1)
plot.plot(t,ddx,'r')
plot.plot(t,ddy,'g')
plot.plot(t,ddz,'b')
plot.figure(2)
plot.plot(ddx, ddy)
plot.show()