#!/usr/bin/python
import shlex
import subprocess
import matplotlib.pyplot as plt

simulation_time = 7200;
delta_time = 60;

# #  start main-seq
# x = []
# y = []
# for i in range(1,simulation_time+1):

# 	command_line = "./main-seq -t 'databases/ISS.txt' -d 10 -f 3 -e "+str(i)+" -i 1 2> /dev/null";
# 	args = shlex.split(command_line);

# 	p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=None)
# 	result = p.communicate()[0]
# 	result = float(filter(str.isdigit, result))/1000000;
# 	x.append(i)
# 	y.append(result)


# #  start main-par
# np = 4;

# x_par = []
# y_par = []
# for i in range(1,simulation_time+1):

# 	command_line = "mpirun -np "+str(np)+" ./main-par -t 'databases/ISS.txt' -d 100 -f 1 -e "+str(i)+" -i 0.1 2> /dev/null";
# 	args = shlex.split(command_line);

# 	p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=None)
# 	result = p.communicate()[0].split("\n")
# 	result = float(filter(str.isdigit, result[1]))/1000000;
# 	x_par.append(i)
# 	y_par.append(result)


np = 8;

x_pr = []
y_t = []
for i in range(1,np+1):

	if i == 1:
		command_line = "./main-seq -t 'databases/ISS.txt' -d 10 -f 3 -e "+str(simulation_time)+" -i "+str(delta_time)+" 2> /dev/null";
	else:
		command_line = "mpirun -np "+str(i)+" ./main-par -t 'databases/ISS.txt' -d 100 -f 1 -e "+str(simulation_time)+" -i "+str(delta_time)+" 2> /dev/null";
	print command_line
	args = shlex.split(command_line);
	p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=None)
	result = p.communicate()[0].split("\n")
	result = float(filter(str.isdigit, result[len(result)-2]))/1000000;
	x_pr.append(i)
	y_t.append(result)
	print i



fig = plt.figure()
# plt.plot(x, y,'g--^', label='Sequential')
# plt.plot(x_par, y_par,'b-o', label='Parallel')
plt.plot(x_pr,y_t)
plt.legend( loc='upper left', numpoints = 1 )
plt.xlabel('Number of processors', fontsize=18)
plt.ylabel('Elapsed Time [s]', fontsize=16)
plt.show()
fig.savefig('speed_vs_process.jpg')