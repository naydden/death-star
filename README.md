# Death Star MPI Project

Project that computes (in parallel - MPI) if a satellite will crash with some space debris, using a security distance.

For the propagation of the orbits, **SPG4** will be used.

Note: The program ends when the **first** crash is detected!

## Deadline
29/05/2017

## Idea
Parallel discretization can be done in time or by asteroids per processor.
It will be done by asteroids since  the crashes can be detected in a more efficient way. If it was in a timely manner, many unnecessary computations would be done, since the crash could be the first month of simulation and many processors would already be calculating several months after that.

NOTE: a crash will be found when the asteroid distance to the satellite is less than a distance of security, specified by the user. This is done so as to account for the inaccuracies of the propagator.

### Procedure

    1. Retrieve data of SpaceDebris. Source: [Space-track.org](https://www.space-track.org/). To be found in /databases/space-track-db.csv
    2. Adapt the data so that it can be used with the propagators. Certain Keplerian Elements were missing. This is done with /databases/parserCSV.csv
    3. Satellite to study will be an input by the user in the TLE format.
    4. First approach: sequential code using spg4.
    5. Second approach: each processors will propagate the satellite and some of the asteroids.
        - Asteroids will be divided as equally as possible  between the different processors.
        - Armageddon will propagate the satellite using /lib/Astrodynamics.h.
        - main-par will propagate the satellite using the spg4 algorithm.
    6. In the temporal loop, for both cases MPI_Allreduce will be used periodically in order to check the status of all the processors. If there are several crashes, then the first (in the time scale) is the good one.

## To run

    1. To Build: make

    2. To Run main-par: mpirun -np 4 ./main-par -t 'databases/ISS.txt' -d 1000 -f 1 -e 7200 -i 60 2> /dev/null
    By running main-par with no arguments, a help message is displayed showing the required variables and explaining them. NOTE: 2> /dev/null is required because otherwise the terminal is flooded with messages form the spg4 simulator which complains since some of the Orbital Elements provided to it, are not suitable for it and it generates strange results. Those elements are handled in the main code,but the prints are not, since we did not want to change the library code.

    3. To run Armaggedon: mpirun -np 4 ./Armaggedon

    4. To run the sequential code and compare with main-par: ./main-seq -t 'databases/ISS.txt' -d 10 -f 3 -e 7200 -i 60 2> /dev/null

## Clarification Details
A python code has been written (./plotting.py) in order to execute several times the codes with different initializations parameters in order to generate some plots.

The code used to parse and adapt the database, has some external dependencies which were installed with npm. That dependencies are stored inside /node_modules and are:

    1. csv_parsing library.
    2. lodash library for easier data treatment.

This code requires node installed in the computer in order to be run.

## Optional To-dos
    1. Plot objects movement from Earth's point of view.

## Acknowledgments
All the contents inside the /lib folder are supplied by David de la Torre, teacher at UPC.
All the contents inside the /spg_lib are downloaded from [here](http://www.sat.dundee.ac.uk/psc/sgp4.html) and were supplied by Paul Crawford & Andrew Brooks.

Thanks to all of them for helping the open source community!

## License
GPL