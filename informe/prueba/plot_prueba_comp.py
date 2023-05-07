import sys
import numpy as np
from numpy import loadtxt
from scipy.io.wavfile import read
import matplotlib.pyplot as plt
plt.rcParams["figure.figsize"] = [7.50, 3.50]
plt.rcParams["figure.autolayout"] = True

ref = loadtxt("informe/prueba/prueba.f0ref")
wave = loadtxt("informe/prueba/prueba.f0ws")
sist = loadtxt("informe/prueba/prueba.f0")
wave = np.transpose(wave);
wave =wave[0]

fig, (ax0) = plt.subplots(1)

ax0.plot(ref[0:1024000], label="pitch de referencia")
ax0.plot(wave[0:1024000], label="pitch estimado por wavesurfer")
ax0.plot(sist[0:1024000], label="pitch estimado por get_pitch")
ax0.title.set_text("Comparacion de estimaciones")
ax0.legend()
plt.show()