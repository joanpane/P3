import sys
import numpy as np
from numpy import loadtxt
from scipy.io.wavfile import read
import matplotlib.pyplot as plt
plt.rcParams["figure.figsize"] = [7.50, 3.50]
plt.rcParams["figure.autolayout"] = True

input_data = read("prueba.wav")
audio = input_data[1]
est = loadtxt("prueba.f0ws")
pot = loadtxt("prueba.pot")
r1norm = loadtxt("prueba.r1norm")
rmaxnorm = loadtxt("prueba.rmaxnorm")

fig, (ax0, ax1, ax2, ax3, ax4) = plt.subplots(5)
ax1.plot(est[0:1024000])
ax1.title.set_text("Estimación pitch Wavesurfer")
ax0.plot(audio[0:1024000])
ax0.title.set_text("Wave")
ax2.plot(pot[0:1024000])
ax2.title.set_text("Power (dB)")
ax3.plot(r1norm[0:1024000])
ax3.title.set_text("r[1]/r[0]")
ax4.plot(rmaxnorm[0:1024000])
ax4.title.set_text("r[lag]/r[0]")
plt.show()