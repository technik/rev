from math import *
from matplotlib import pyplot as plt

nAir = 1.00029

def rayleight(wavelength, theta):
	norm = 0.5*pi*(nAir**2-1)**2 # Normalization factor
	return norm * (1+cos(theta)**2) / (wavelength**4)

thetaSamples = [2*pi*t/200.0 for t in range(201)]

redWaveLengh = 700e-9
greenWaveLength = 540e-9
blueWaveLength = 470e-9

def plotRayleigh(wavelength, fmt):
	rad = [rayleight(wavelength, t) for t in thetaSamples]

	x = [cos(thetaSamples[i])*rad[i] for i in range(201)]
	y = [sin(thetaSamples[i])*rad[i] for i in range(201)]

	plt.plot(x,y, fmt)

def airDensity(h):
	return pow(e, -h/8000)

#plotRayleigh(redWaveLengh, 'r')
#plotRayleigh(greenWaveLength, 'g')
#plotRayleigh(blueWaveLength, 'b')

def fresnelRs(n_ratio, theta):
	a = n_ratio*cos(theta)
	b = sqrt(1-(n_ratio*sin(theta))**2)
	num = a-b
	den = a+b
	return (num/den)**2

def fresnelRp(n_ratio, theta):
	a = n_ratio*sqrt(1-(n_ratio*sin(theta))**2)
	b = cos(theta)
	num = a-b
	den = a+b
	return (num/den)**2

def schlickFresnel(n_ratio, theta):
	F0 = ((n_ratio-1)/(n_ratio+1))**2
	return F0 + (1-F0)*pow(1-cos(theta), 5)


def plot_fresnel(n_ratio):
	assert(n_ratio < 1.0)
	angles = [t/4 for t in thetaSamples]
	rs = [fresnelRs(n_ratio, t) for t in angles]
	rp = [fresnelRp(n_ratio, t) for t in angles]
	rEff = [0.5*(fresnelRs(n_ratio, t)+fresnelRp(n_ratio, t)) for t in angles]
	plt.plot(angles, rs, 'r')
	plt.plot(angles, rp, 'g')
	plt.plot(angles, rEff, 'b')
	plt.plot(angles, [schlickFresnel(n_ratio, t) for t in angles], 'purple')
	
#plt.figure(1)
hs = range(0,100000, 100)
#plt.plot(hs, [airDensity(h) for h in hs])


plt.figure(2)
plot_fresnel(1/1.1)
plot_fresnel(1/1.5)
plot_fresnel(1/2.0)
plot_fresnel(1/5.0)

plt.show()