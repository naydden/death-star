#!/usr/bin/python
import shlex
import subprocess
import matplotlib.pyplot as plt

simulation_time = 7200;
delta_time = 60;

np = 16;

x_pr = []
y_t = []
for i in range(1,np+1):
	command_line = "mpirun -np "+str(i)+" ./Armaggedon";
	print command_line
	args = shlex.split(command_line);
	p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=None)
	result = p.communicate()[0].split("\n")
	result = float(filter(str.isdigit, result[len(result)-2]))/1000000;
	x_pr.append(i)
	y_t.append(result)
	print result
	print i



fig = plt.figure()
plt.plot(x_pr,y_t)
plt.legend( loc='upper right', numpoints = 1 )
plt.xlabel('Number of processors', fontsize=18)
plt.ylabel('Elapsed Time [s]', fontsize=16)
plt.xlim(1, np)
plt.show()
fig.savefig('plots/speed_vs_process_arm.jpg')