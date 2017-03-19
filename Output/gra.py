import sys, os
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

filename=sys.argv[1]
f=open(str(filename),'r')

t=[]
x=[]
y=[]
z=[]
for line in f:
	words = line.split()
	t.append(float(words[0]))
	x.append(float(words[1]))
	y.append(float(words[2]))
	z.append(float(words[3]))
plt.plot(x,y)
plt.xlabel('Distance along X direction[m]', fontsize=10)
plt.ylabel('Distance along Y direction[m]', fontsize=10)
plt.show()
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.plot(x, y, z)
plt.show()	