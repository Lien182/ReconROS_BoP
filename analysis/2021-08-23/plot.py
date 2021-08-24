import pandas
import ctypes
import numpy as np
from numpy import genfromtxt
import struct

import matplotlib.pyplot as plt





data = np.genfromtxt('data.csv', delimiter=';')
print(data)
print('-'*50)

colw = data.shape[1]

dtypes = [('col0', int)]+[('col1', np.float64)]+[('col2', int)]+[('col3', float)]+[('col4', int)]+[('col5', float)]+[('col6', bool)]
print(dtypes)
print('-'*50)

converted_data = np.array([tuple(r) for r in data], dtype = dtypes)

timestamps = np.zeros((converted_data.size,3))
cycletimes = np.zeros((converted_data.size,3))
for i, r in enumerate(converted_data):
    timestamps[i,0] = r[0]
    timestamps[i,1] = r[2]
    timestamps[i,2] = r[4]
    if r[1] < 1000:
        cycletimes[i,0] = r[1]
    if r[3] < 1000:
        cycletimes[i,1] = r[3]
    if r[5] < 1000:
        cycletimes[i,2] = r[5]


# print(df)

#converted_data[1] = converted_data[1].astype('float64')

fig, axs = plt.subplots(2, 3)

for i in range(0,3):
    axs[0,i].plot(cycletimes[:,i])
    axs[0,i].set_xlim(0, cycletimes[:,i].size)
    axs[0,i].set_xlabel('Sample')
    axs[0,i].set_ylabel('Cycletime')
    axs[0,i].grid(True)

    axs[1,i].hist(cycletimes[:,i], bins=np.arange(20, step=0.5))

    # axs[1,i].plot(timestamps[:,i])
    axs[1,i].set_xlim(0, 20)
    axs[1,i].set_xlabel('Cycletime')
    axs[1,i].set_ylabel('Frequency')
    axs[1,i].grid(True)



fig.tight_layout()
plt.show()