# University Bus Simulation

---

## Author  
**Nikoletta Arvaniti**  
Email: csd4844@csd.uoc.gr  

---

## Overview  
This program simulates the transportation of students by a bus with the following constraints:  
- **Maximum Capacity**: 12 students.  
- **Department Limit**: Maximum of 3 students per department (MATH, PHYSICS, CHEM, CSD).  
- **Student Count**: Determined by user input, with each student assigned a random department and study time.  

### Implementation Details  
- **Threads**:  
  - Each student is represented as a thread.  
  - The bus is also implemented as a separate thread.  

- **Synchronization**:  
  - **Semaphores** and **mutexes** are used to ensure proper synchronization between threads.  

- **Data Structures**:  
  - **Queues**: Manage students waiting at Stop A, Stop B, and inside the bus.  
  - **Array**: Tracks students inside the university, maintaining the FIFO logic required for unique study times.  

### Key Operations  
1. **Boarding and Travel**:  
   - The bus thread handles boarding, traveling between stops, and pick-ups/drop-offs.  
   - Students board the bus only if their department's limit allows.  

2. **FIFO Logic**:  
   - Students board and leave the bus in a FIFO manner.  

3. **Completion**:  
   - The process continues until all students have completed their trips and returned home.  

---
