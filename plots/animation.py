#!/usr/bin/python
import csv
import sys
import seaborn
from numpy  import array
import matplotlib.pyplot as plt
import matplotlib.animation as animation

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



# First set up the figure, the axis, and the plot element we want to animate

fig = plt.figure(figsize=(20,10))
fig.suptitle('ISS and FENGYUN_1C_DEB crash', fontsize=14, fontweight='bold')
1
ax_xy = fig.add_subplot(1, 2, 1)
ax_xy.set_xlabel('x [km]')
ax_xy.set_ylabel('y [km]',rotation=0)
ax_xy.ticklabel_format(axis='both', style='sci', scilimits=(-500,500))
line_xy, = ax_xy.plot([], [], lw=2)
deb_xy,  = ax_xy.plot([], [], lw=2)
ax_xy.add_line(line_xy)
ax_xy.add_line(deb_xy)
ax_xy.set_xlim(-8000, 8000)
ax_xy.set_ylim(-8000, 8000)

ax_yz = fig.add_subplot(2, 2, 2)
ax_yz.set_xlabel('y [km]')
ax_yz.set_ylabel('z [km]',rotation=0)
line_yz, = ax_yz.plot([], [], lw=2)
deb_yz,  = ax_yz.plot([], [], lw=2)
ax_yz.add_line(line_yz)
ax_yz.add_line(deb_yz)
ax_yz.set_xlim(-8000, 8000)
ax_yz.set_ylim(-8000, 8000)

ax_zx = fig.add_subplot(2, 2, 4)
ax_zx.set_xlabel('z [km]')
ax_zx.set_ylabel('x [km]',rotation=0)
line_zx, = ax_zx.plot([], [], lw=2)
deb_zx,  = ax_zx.plot([], [], lw=2)
ax_zx.add_line(line_zx)
ax_zx.add_line(deb_zx)
ax_zx.set_xlim(-8000, 8000)
ax_zx.set_ylim(-8000, 8000)

fig.autofmt_xdate()

# initialization function: plot the background of each frame
def init():
	line_xy.set_data([], [])
	deb_xy.set_data([], [])
	line_yz.set_data([], [])
	deb_yz.set_data([], [])
	line_zx.set_data([], [])
	deb_zx.set_data([], [])
	return line_xy, deb_xy, line_yz, deb_yz, line_zx, deb_zx,

# animation function.  This is called sequentially
def animate(i):
	x = x_s[:i]
	y = y_s[:i]
	z = z_s[:i]
	line_xy.set_data(x, y)
	line_yz.set_data(y, z)
	line_zx.set_data(z, x)
	x = x_d[:i]
	y = y_d[:i]
	z = z_d[:i]
	deb_xy.set_data(x, y)
	deb_yz.set_data(y, z)
	deb_zx.set_data(z, x)
	return line_xy, deb_xy, line_yz, deb_yz, line_zx, deb_zx,


# call the animator.  blit=True means only re-draw the parts that have changed.
anim = animation.FuncAnimation(fig, animate, init_func=init,
							   frames=1500, interval=5, blit=True)

anim.save('figure.mp4', fps=30, extra_args=['-vcodec', 'libx264'])
# plt.show()