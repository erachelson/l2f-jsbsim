import sys, os
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

filename=sys.argv[1]
f=open(str(filename),'r')

data={}
x=[]
y=[]
z=[]
t=[]
firstline=True
for line in f:
    words = line.split(', ')
    if firstline:
        for item in words:
            position = words.index(item)
            data[position,item.strip()]=[]
        firstline=False
    else:
        for k in data:
            data[k].append(float(words[k[0]]))

for key in data.keys():
    if key[1]=='Time':
        t=data[key]
    if key[1]=='h-sl-meters':
        z=data[key]
    if key[1]=='distance-from-start-lat-mt':
        x=data[key]
    if key[1]=='distance-from-start-lon-mt':
        y=data[key]            

#plt.plot(x,y)
#plt.show()
plt.plot(x,z)
#fig = plt.figure()
#ax = fig.add_subplot(111, projection='3d')
#ax.plot(x, y, z)
#plt.xlabel('Time of Simulation', fontsize=10)
plt.xlabel('Distance along X direction[m]', fontsize=10)
plt.ylabel('Distance along Z direction[m]', fontsize=10)
plt.show()