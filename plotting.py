#!/usr/bin/python
import shlex
import subprocess
import matplotlib.pyplot as plt

simulation_time = 10;

#  start main-seq
x = []
y = []
for i in range(1,simulation_time+1):

	command_line = "./main-seq -t 'databases/ISS.txt' -d 10 -f 3 -e "+str(i)+" -i 1 2> /dev/null";
	args = shlex.split(command_line);

	p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=None)
	result = p.communicate()[0]
	result = float(filter(str.isdigit, result))/1000000;
	x.append(i)
	y.append(result)


#  start main-par
np = 4;

x_par = []
y_par = []
for i in range(1,simulation_time+1):

	command_line = "mpirun -np "+str(np)+" ./main-par -t 'databases/ISS.txt' -d 100 -f 1 -e "+str(i)+" -i 0.1 2> /dev/null";
	args = shlex.split(command_line);

	p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=None)
	result = p.communicate()[0]
	result = float(filter(str.isdigit, result))/1000000;
	x_par.append(i)
	y_par.append(result)



fig = plt.figure()
plt.plot(x, y)
plt.plot(x_par, y_par)
fig.suptitle('Efficiency Study', fontsize=20)
plt.xlabel('Simulated Time[s]', fontsize=18)
plt.ylabel('Elapsed Time [s]', fontsize=16)
plt.show()
