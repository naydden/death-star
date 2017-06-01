#!/usr/bin/python

import csv
import sys
from numpy  import array
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

f = open(sys.argv[1], 'rt')
# satellite
x_s = [];
y_s = [];
z_s = [];
t = [];
try:
	reader = csv.reader(f)
	for row in reader:
		t.append(row[0]);
		x_s.append(row[1]);
		y_s.append(row[2]);
		z_s.append(row[3]);
finally:
	f.close()

f = open(sys.argv[2], 'rt')
# debris
x_d = [];
y_d = [];
z_d = [];
t = [];
try:
	reader = csv.reader(f)
	for row in reader:
		t.append(row[0]);
		x_d.append(row[1]);
		y_d.append(row[2]);
		z_d.append(row[3]);
finally:
	f.close()


fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
x_s = map(lambda x: float(x),x_s)
y_s = map(lambda x: float(x),y_s)
z_s = map(lambda x: float(x),z_s)
z_s = array(z_s)

x_d = map(lambda x: float(x),x_d)
y_d = map(lambda x: float(x),y_d)
z_d = map(lambda x: float(x),z_d)
z_d = array(z_d)

ax.plot(array(x_s), array(y_s), zs=z_s, label='Satellite')
ax.plot(array(x_d), array(y_d), zs=z_d, label='Debris')
plt.xlabel('X [km]', fontsize=8)
plt.ylabel('Y [km]', fontsize=8)
ax.legend()

plt.show()
Axes3D.plot()



