# Death Star MPI Project

Project that computes (in parallel - MPI) the nearest approach of a PHO (Potentially hazardous object) to the Earth.


## Deadline
29/05/2017

## Todos
### Sequential
    1. Retrieve data of PHO  and the Earth from a given database.
        1.1 Decide Timespan.
    2. Obtain Mean Anomaly from True Anomaly and propagate  objects and the Earth. (the other parameters are considered to be constant.)
    3.  Convert the **6 Keplerian elements** into cartesian coordinates.
    4.  Compute distances between the objects and the Earth in a timely manner. 

### Parallel
    1. Retrieve data of PHO  and the Earth from a given database.
        1.1 Decide Timespan.
    2. Obtain Mean Anomaly from True Anomaly and propagate objects and  the Earth. (the other parameters are considered to be constant.)
    3.  Convert the **6 Keplerian elements** into cartesian coordinates.
    4.  Compute distances between the objects and the Earth in a timely manner. 

## Optional Todos
    1. Plot objects movement from Earth's point of view.
    2. Plot object movement from the top of the Solar System.
