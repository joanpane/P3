import sys
import numpy as np
from numpy import loadtxt
from scipy.io.wavfile import read
import matplotlib.pyplot as plt
plt.rcParams["figure.figsize"] = [7.50, 3.50]
plt.rcParams["figure.autolayout"] = True

x = loadtxt("informe/resultats_x.txt")
r = loadtxt("informe/resultats_r.txt")


fig, (ax0, ax1) = plt.subplots(2)
ax1.plot(r[0:1024000])
ax1.plot(78, r[78],marker="o", markersize=10, markeredgecolor="blue")
ax1.title.set_text("Autocorrelación r[k]")
ax0.plot(x[0:1024000])
ax0.title.set_text("Trama 30 ms x[n]")
plt.show()  