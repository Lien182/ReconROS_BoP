import pandas
import ctypes
import numpy as np
from numpy import genfromtxt
import struct

import matplotlib.pyplot as plt

from matplotlib.ticker import PercentFormatter


dtypes = [('col0', int)]+[('col1', np.float64)]+[('col2', int)]+[('col3', float)]+[('col4', int)]+[('col5', float)]+[('col6', bool)]
description= [["Platform 0 \n HW-SW Distributed", "Platform 1 \n HW-SW Distributed", "Platform 2 \n HW-SW Distributed"], ["Platform 0 \n SW Distributed", "Platform 1 \n SW Distributed", "Platform 2 \n SW Distributed"], ["Platform 0 \nCentral SW", "Platform 1 \nCentral SW", "Platform 2 \nCentral SW"]]
filenames = ["cycletimes_hw_sw_distributed.csv","cycletimes_sw_distributed.csv" , "cycletimes_sw.csv" ]

datasets = []
measurement_sizes = np.zeros(3)
timestamps = np.zeros((2000,3,3))
cycletimes = np.zeros((2000,3,3))
valid_measurement_sizes = np.zeros((3,3))
deadline_missed_threshold = 20
invalid_threshold = 100
misses  = np.zeros((3,3))
invalid = np.zeros((3,3))

for data_files in range(0,3):
    data = np.genfromtxt(filenames[data_files], delimiter=';')
    colw = data.shape[1]
    datasets.append(np.array([tuple(r) for r in data], dtype = dtypes))
    measurement_sizes[data_files] = datasets[data_files].size


for datafiles in range(0,3):
    for i, r in enumerate(datasets[datafiles]):
        timestamps[i,0,datafiles] = r[0]
        timestamps[i,1,datafiles] = r[2]
        timestamps[i,2,datafiles] = r[4]
        if r[1] < deadline_missed_threshold:
            cycletimes[i,0,datafiles] = r[1]
            valid_measurement_sizes[0,datafiles]+=1        
        elif r[1] < invalid_threshold:
            misses[0,datafiles]+=1
        else:
            invalid[0,datafiles]+=1

        if r[3] < deadline_missed_threshold:
            cycletimes[i,1,datafiles] = r[3]
            valid_measurement_sizes[1,datafiles]+=1   
        elif r[3] < invalid_threshold:
            misses[1,datafiles]+=1
        else:
            invalid[1,datafiles]+=1

        if r[5] < deadline_missed_threshold:
            cycletimes[i,2,datafiles] = r[5]
            valid_measurement_sizes[2,datafiles]+=1   
        elif r[5] < invalid_threshold:
            misses[2,datafiles]+=1
        else:
            invalid[2,datafiles]+=1


   

#misses in percent

print("Misses")
print(np.transpose(misses))
print("Invalid")
print(np.transpose(invalid))
print("Valid")
print(np.transpose(valid_measurement_sizes))
print("Sizes")
print(np.transpose(measurement_sizes))

for j in range(0,3):
    for i in range(0,3):
        print(str(misses[i,j]) + "/" + str(measurement_sizes[j]))
        misses[i,j] /=  measurement_sizes[j]
        misses[i,j] *= 100
        

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

        axs[j,i+1].hist(cycletimes[:,i,j], bins=np.arange(start=0.5, stop=20, step=0.5), weights=np.zeros_like(cycletimes[:,i,j]) + 100. / valid_measurement_sizes[i,j])

        # axs[1,i].plot(timestamps[:,i])
        axs[j,i+1].set_xlim(0, 20)
        axs[j,i+1].set_xlabel(("Cycletime\n ({:2.2f}% DM)").format(misses[i,j]))
        axs[j,i+1].set_ylim(0, 25)
        axs[j,i+1].set_ylabel('Frequency')
        axs[j,i+1].yaxis.set_major_formatter(PercentFormatter(xmax=100))
        axs[j,i+1].grid(True)
        #axs[j+1,i+1].set_title(description[j][i]+ ("\n {:2.2f}% DM").format(misses[i,j]))
        if (j != 0):
            #axs[j,i+1].set_title(("{:2.2f}% DM").format(misses[i,j]))
            print()
        else:
            #axs[j,i+1].set_title(("Platform {}\n{:2.2f}% DM").format(i,misses[i,j]))
            axs[j,i+1].set_title(("Platform {}\n").format(i))


fig.tight_layout()
plt.show()
print(misses)