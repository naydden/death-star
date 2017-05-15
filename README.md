# Death Star MPI Project

Project that computes (in parallel - MPI) if a satellite has a probability to crash with an asteroid (simulating space debris). Main asteroid belt asteroids will be scaled down to orbits around the Earth simulating space debris.

For the propagation of the orbits, **SPG4** will be used.

Note: The program ends when the **first** crash is detected!

## Deadline
29/05/2017

## Todos
### Procedure
Parallel discretization can be done in time or by asteroids per processor.
It will be done by asteroids since  the crashes can be detected in a more efficient way. If it was in a timely manner, many unnecessary computations would be done, since the crash could be the first month of simulation and many processors would already be calculating several months after that.
    1. Retrieve data of Main Asteroid Belt asteroids.
    2. Adapt the data to an orbit around the Earth. Orbit is to be decided.
    3. Satellite to study will be an input by the user where they will provide the Keplerian Elements.
    4. Each processore will propagate the satellite and some of the asteroids.
        - Asteroids will be divided as equally as possible  between the different processors.
    5. In the temporal loop, MPI Barrier will be used periodically in order to check the status of all the processors. If there are several crashes, then the first (in the time scale) is the good one.
    
NOTE: a crash will be found when the asteroid distance to the satellite is less than a distance of security, specified by the user. This is done so as to account for the innaccuracies of the propagator.

## Optional Todos
    1. Plot objects movement from Earth's point of view.
