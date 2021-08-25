import pandas
import ctypes
import numpy as np
from numpy import genfromtxt
import struct

import matplotlib.pyplot as plt


dtypes = [('col0', int)]+[('col1', np.float64)]+[('col2', int)]+[('col3', float)]+[('col4', int)]+[('col5', float)]+[('col6', bool)]

description= [["Platform 0 \n HW-SW Distributed", "Platform 1 \n HW-SW Distributed", "Platform 2 \n HW-SW Distributed"], ["Platform 0 \n SW Distributed", "Platform 1 \n SW Distributed", "Platform 2 \n SW Distributed"], ["Platform 0 \nCentral SW", "Platform 1 \nCentral SW", "Platform 2 \nCentral SW"]]

data = np.genfromtxt('cycletimes_hw_sw_distributed.csv', delimiter=';')
colw = data.shape[1]
cycletimes_hw_sw_distributed = np.array([tuple(r) for r in data], dtype = dtypes)

data = np.genfromtxt('cycletimes_sw.csv', delimiter=';')
colw = data.shape[1]
cycletimes_sw = np.array([tuple(r) for r in data], dtype = dtypes)

data = np.genfromtxt('cycletimes_sw_distributed.csv', delimiter=';')
colw = data.shape[1]
cycletimes_sw_distributed = np.array([tuple(r) for r in data], dtype = dtypes)


timestamps = np.zeros((cycletimes_hw_sw_distributed.size,3,3))
cycletimes = np.zeros((cycletimes_hw_sw_distributed.size,3,3))

misses = np.zeros((3,3))

for i, r in enumerate(cycletimes_hw_sw_distributed):
    timestamps[i,0,0] = r[0]
    timestamps[i,1,0] = r[2]
    timestamps[i,2,0] = r[4]
    if r[1] < 1000:
        cycletimes[i,0,0] = r[1]
        misses[0,0]+=1
    if r[3] < 1000:
        cycletimes[i,1,0] = r[3]
        misses[1,0]+=1
    if r[5] < 1000:
        cycletimes[i,2,0] = r[5]
        misses[2,0]+=1


for i, r in enumerate(cycletimes_sw_distributed):
    timestamps[i,0,1] = r[0]
    timestamps[i,1,1] = r[2]
    timestamps[i,2,1] = r[4]
    if r[1] < 1000:
        cycletimes[i,0,1] = r[1]
        misses[0,1]+=1
    if r[3] < 1000:
        cycletimes[i,1,1] = r[3]
        misses[1,1]+=1
    if r[5] < 1000:
        cycletimes[i,2,1] = r[5]
        misses[2,1]+=1

for i, r in enumerate(cycletimes_sw):
    timestamps[i,0,2] = r[0]
    timestamps[i,1,2] = r[2]
    timestamps[i,2,2] = r[4]
    if r[1] < 1000:
        cycletimes[i,0,2] = r[1]
        misses[0,2]+=1
    if r[3] < 1000:
        cycletimes[i,1,2] = r[3]
        misses[1,2]+=1
    if r[5] < 1000:
        cycletimes[i,2,2] = r[5]
        misses[2,2]+=1


#misses in percent

misses =(100 - misses / 2000 * 100)

fig, axs = plt.subplots(3, 4)

for i in range(0,3):
    #axs[0,i].axis('off')
    axs[i,0].axis('off')

axs[0,1].set_title("Platform 0")
axs[0,2].set_title("Platform 1")
axs[0,3].set_title("Platform 2")
axs[0,0].set_title("HW-SW \n Distributed")
axs[1,0].set_title("SW \n Distributed")
axs[2,0].set_title("SW \n Centralized")

for j in range(0,3):
    for i in range(0,3):

        axs[j,i+1].hist(cycletimes[:,i,j], bins=np.arange(start=0, stop=20, step=0.5))

        # axs[1,i].plot(timestamps[:,i])
        axs[j,i+1].set_xlim(0, 20)
        axs[j,i+1].set_xlabel('Cycletime')
        axs[j,i+1].set_ylim(0, 300)
        axs[j,i+1].set_ylabel('Frequency')
        axs[j,i+1].grid(True)
        #axs[j+1,i+1].set_title(description[j][i]+ ("\n {:2.2f}% DM").format(misses[i,j]))
        if (j != 0):
            axs[j,i+1].set_title(("{:2.2f}% DM").format(misses[i,j]))
        else:
            axs[j,i+1].set_title(("Platform {}\n{:2.2f}% DM").format(i,misses[i,j]))


fig.tight_layout()
plt.show()
print(misses)