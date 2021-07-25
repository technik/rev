import json
from matplotlib import pyplot as plt
import numpy as np
from math import *

jsonData = open('brdfFresnel.json').read()
sbData = json.loads(jsonData)
numSamples = len(sbData)
ndvSamples = [i/(numSamples-1.0) for i in range(len(sbData))]

x = [i/numSamples for i in range(numSamples)]

def plotFit(scaleWeight, biasWeight, fitFn, yLabel):
	data = [[sbData[r][v][0]*scaleWeight + biasWeight*sbData[r][v][1] for v in range(len(sbData[r]))] for r in range(len(sbData))]
	plt.figure()
	plt.xlabel("ndv")
	plt.ylabel(yLabel)

	plt.plot(x, [data[r][0] for r in range(numSamples)])
	plt.plot(x, [data[r][int(numSamples/4)] for r in range(numSamples)])
	plt.plot(x, [data[r][int(numSamples/2)] for r in range(numSamples)])
	plt.plot(x, [data[r][int(3*numSamples/4)] for r in range(numSamples)])
	plt.plot(x, [data[r][63] for r in range(numSamples)])

	for ndv in [0,0.25,0.5,0.75,1]:
		bias = [fitFn(r,ndv) for r in x]
		plt.plot(x, bias, '--')

	plt.figure()
	plt.xlabel("roughness")
	plt.ylabel(yLabel)

	plt.plot(x, [data[0][r] for r in range(numSamples)])
	plt.plot(x, [data[int(numSamples/4)][r] for r in range(numSamples)])
	plt.plot(x, [data[int(numSamples/2)][r] for r in range(numSamples)])
	plt.plot(x, [data[int(3*numSamples/4)][r] for r in range(numSamples)])
	plt.plot(x, [data[63][r] for r in range(numSamples)])

	for r in [0,0.25,0.5,0.75,1]:
		bias = [fitFn(r,ndv) for ndv in x]
		plt.plot(x, bias, '--')

# Clear coat fit
def ccFit(r, ndv):
	k = 1-r*r
	eR0 = pow(1-ndv,5)*0.7 + 0.3
	rMap = (r-0.05)/0.95
	#return k + eR0*(1-k)
	return min(1, (1-rMap)**3 * 0.92 + 0.08) * pow(1 - ndv, 5) * 0.96 + 0.04

#plotFit(0.04, 1, ccFit, "Clear coat brdf")

# General BRDF fit
def biasFit(r, ndv):
	rMap = (r-0.05)/0.95
	return min(1, (1-rMap)**3 * 0.96 + 0.04) * pow(1 - ndv, 5)

def af1Fit(r, ndv):
	ar1 = ndv*log(0.01+ndv/(ndv+1)) + 1;
	return (1-r*r) + r*r * ar1;	

def scaleFit(r, ndv):
	return af1Fit(r, ndv) - biasFit(r,ndv)

# Generalized BRDF scale
plotFit(1, 0, scaleFit, "brdf scale, a1-s0")
plotFit(0, 1, biasFit, "brdf bias, a f0=0")
plotFit(1, 1, af1Fit, "brdf eAvg, a f0=1")
plotFit(1, 1, kullaFit, "brdf eAvg Kulla, a f0=1")

plt.show()