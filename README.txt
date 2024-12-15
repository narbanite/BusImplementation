UNIVERSITY BUS SIMULATION
~~~~~~~~~~~~~~~~~~~~~~~~~


Author
------
Nikoletta Arvaniti (csd4844@csd.uoc.gr)


Overview
--------
    This program simulates the transportation of students by a bus that has maximum capacity of 12 students and a limit 
of 3 students per department(MATH, PHYSICS, CHEM, CSD). The number of the students is determined by user input and each
student is assigned a random department and study time.
    Students are represented as threads, as well as the bus(separate thread), and they interact with each other using semaphores
and mutexes to achieve synchronization. Queues are used to manage students waiting at Stop A, Stop B and inside the bus, while an
array keeps track of students inside the university. This is because we need to keep the FIFO logic throughout the program but 
since each student has unique study time, the queue would not be able to work for the students inside uni.
    The bus thread handles the boarding, travels between the stops and uni, as well as pick-ups and drop-offs. Students board the 
bus only when their department's limit allows it, and they leave the bus to study or return back home keeping the FIFO logic. This
continues until all students have completed their trip and returned back home.