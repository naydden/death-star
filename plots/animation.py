#!/usr/bin/python
import csv
import sys
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
fig_xy = plt.figure()
ax_xy = plt.axes(xlim=(-8000, 8000), ylim=(-8000, 8000))
line_xy, = ax_xy.plot([], [], lw=2)
deb_xy,  = ax_xy.plot([], [], lw=2)

fig_yz = plt.figure()
ax_yz = plt.axes(xlim=(-8000, 8000), ylim=(-8000, 8000))
line_yz, = ax_yz.plot([], [], lw=2)
deb_yz,  = ax_yz.plot([], [], lw=2)

fig_zx = plt.figure()
ax_zx = plt.axes(xlim=(-8000, 8000), ylim=(-8000, 8000))
line_zx, = ax_zx.plot([], [], lw=2)
deb_zx,  = ax_zx.plot([], [], lw=2)

# initialization function: plot the background of each frame
def init_xy():
	line_xy.set_data([], [])
	deb_xy.set_data([], [])
	return line_xy, deb_xy,

# animation function.  This is called sequentially
def animate_xy(i):
	x = x_s[:i]
	y = y_s[:i]
	line_xy.set_data(x, y)
	x = x_d[:i]
	y = y_d[:i]
	deb_xy.set_data(x, y)
	return line_xy, deb_xy,
# *********************************************************


# initialization function: plot the background of each frame
def init_yz():
	line_yz.set_data([], [])
	deb_yz.set_data([], [])
	return line_yz, deb_yz,

# animation function.  This is called sequentially
def animate_yz(i):
	x = y_s[:i]
	y = z_s[:i]
	line_yz.set_data(x, y)
	x = y_d[:i]
	y = z_d[:i]
	deb_yz.set_data(x, y)
	return line_yz, deb_yz,
# *********************************************************



# initialization function: plot the background of each frame
def init_zx():
	line_zx.set_data([], [])
	deb_zx.set_data([], [])
	return line_zx, deb_zx,

# animation function.  This is called sequentially
def animate_zx(i):
	x = z_s[:i]
	y = x_s[:i]
	line_zx.set_data(x, y)
	x = z_d[:i]
	y = x_d[:i]
	deb_zx.set_data(x, y)
	return line_zx, deb_zx,
# *********************************************************


# call the animator.  blit=True means only re-draw the parts that have changed.
anim = animation.FuncAnimation(fig_xy, animate_xy, init_func=init_xy,
							   frames=3000, interval=50, blit=True)
anim2 = animation.FuncAnimation(fig_yz, animate_yz, init_func=init_yz,
							   frames=3000, interval=50, blit=True)
anim3 = animation.FuncAnimation(fig_zx, animate_zx, init_func=init_zx,
							   frames=3000, interval=50, blit=True)

# save the animation as an mp4.  This requires ffmpeg or mencoder to be
# installed.  The extra_args ensure that the x264 codec is used, so that
# the video can be embedded in html5.  You may need to adjust this for
# your system: for more information, see
# http://matplotlib.sourceforge.net/api/animation_api.html
# anim.save('xy.mp4', fps=30, extra_args=['-vcodec', 'libx264'])
# anim2.save('yz.mp4', fps=30, extra_args=['-vcodec', 'libx264'])
# anim3.save('zx.mp4', fps=30, extra_args=['-vcodec', 'libx264'])
plt.show()








# plt.figure(1)
# plt.subplot(221)
# plt.plot(x_s,y_s,'g--^', label='Sat')
# plt.plot(x_d,y_d,'b--^', label='Deb')

# plt.subplot(222)
# plt.plot(y_s,z_s,'g--^', label='Sat')
# plt.plot(y_d,z_d,'b--^', label='Deb')

# plt.subplot(223)
# plt.plot(z_s,x_s,'g--^', label='Sat')
# plt.plot(z_d,x_d,'b--^', label='Deb')


# plt.show()


# plt.plot(x, y,'g--^', label='Sequential')
# plt.plot(x_par, y_par,'b-o', label='Parallel')
# plt.plot(x_s,y_s,'g--^', label='Sat')
# plt.plot(x_d,y_d,'b--^', label='Deb')
# plt.legend( loc='upper right', numpoints = 1 )
# plt.xlabel('X [km]', fontsize=16)
# plt.ylabel('Y [km]', fontsize=16)
# plt.show()
# fig.savefig('speed_vs_process.jpg')
