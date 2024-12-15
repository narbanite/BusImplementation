/**
 * @file main.c
 * @author Nikoletta Arvaniti (csd4844@csd.uoc.gr)
 * @date 31-10-2024
 *  
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include "queues.h" /*queue library, got it from geeksforgeeks, link: https://www.geeksforgeeks.org/queue-in-c/*/

#define N 12
#define NUM_DEPARTMENTS 4
#define T 10
#define WAITING_TIME 6
#define MAX_UNI_CAPACITY 200

int students_num; /*global var for the number of students we will have everytime the program runs*/

typedef struct {
    int id;
    int department; /*0->math, 1->physics, 2->chemistry, 3->CSD*/
    int study_time;
} Student;

typedef struct {
    sem_t department_semaphores[NUM_DEPARTMENTS]; /*semaphore indicating how many students from each department can board on the bus*/
    int onBoard[NUM_DEPARTMENTS]; /*students on board per department*/
} Bus;

Bus bus;

Queue stopA_queue, stopB_queue, bus_queue;

Student *uni[MAX_UNI_CAPACITY]; /*array to keep track of the students studying*/
Student *students; /*array to keep the info of all the students*/
int uni_count = 0;
int students_returned_home = 0;
sem_t *student_ready; /*semaphore to indicate when a student is ready*/

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void initialize_students(Student *students){
    for(int i = 0; i < students_num; i++){
        students[i].id = i + 1; /*AM*/
        students[i].department = rand() % 4; /*random department between 0-3*/
        students[i].study_time = (rand() % 11) + 5; /*random time between 5-15 sec*/
    }
}

void initialize_bus(Bus *bus){
    for(int i = 0; i < NUM_DEPARTMENTS; i++){
        sem_init(&bus->department_semaphores[i], 0, N / NUM_DEPARTMENTS);
        bus->onBoard[i] = 0;
    }
}


void initQueues(){
    initializeQueue(&stopA_queue);
    initializeQueue(&stopB_queue);
    initializeQueue(&bus_queue);
}


void printStatus() {
    
    printQueue(&stopA_queue, "Stop A");
    printQueue(&bus_queue, "Bus");

    printf("University: [");
    for(int i = 0; i < uni_count; i++){
        if(i > 0 ) printf(", ");
        printf("%d", uni[i]->id);
    }
    printf("]\n");

    printQueue(&stopB_queue, "Stop B");
}


Student* find_student_id(int student_id){
    /*SEARCHING INSIDE THE STUDENTS ARRAY TO FIND THE STUDENT WITH THE ID GIVEN AS PARAMETER*/
    for(int i = 0; i < students_num; i++){
        if(students[i].id == student_id){
            return &students[i];
        }
    }
    return NULL;
}


void *student_thread(void *arg) {
    /*Logic for student behavior (waiting at stop, boarding bus, studying, etc.)*/
    Student *student = (Student *)arg;
    
    pthread_mutex_lock(&mutex);
    printf("Student %d (Department: %d) arrives at stop A.\n", student->id, student->department);
    enqueue(&stopA_queue, student->id);
    printStatus();
    pthread_mutex_unlock(&mutex);

    /*WAIT UNTIL THE BUS ALLOWS BOARDING, bus thread does this*/
    sem_wait(&student_ready[student->id - 1]);

    /*at this point the student has arrived at uni and can start the study session*/

    /*simulate studying*/
    sleep(student->study_time);

    pthread_mutex_lock(&mutex);
    printf("Student %d finishes studying and arrives at Stop B to return.\n", student->id);
    for(int i = 0; i < uni_count; i++){
        if(uni[i] && uni[i]->id == student->id){
            uni[i] = uni[--uni_count];
            uni[uni_count] = NULL;
            break;
        }
    }
    enqueue(&stopB_queue, student->id);
    printStatus();
    pthread_mutex_unlock(&mutex);
    

    /*NOW THE BUS THREAD HANDLES THE RETURN OF THE STUDENT*/

    return NULL;
}

void *bus_thread(void *arg) {
    /* Logic for bus behavior (waiting at stops, allowing students to board, etc.)*/
    Student *student;
    while(1){
        pthread_mutex_lock(&mutex);
        printf("Bus is waiting at Stop A...\n");
        pthread_mutex_unlock(&mutex);

        /*waiting time to stop A*/
        sleep(WAITING_TIME); 

        /*students boarding at stop A*/
        pthread_mutex_lock(&mutex);

        int queueA_size = size(&stopA_queue);
        int processed_studentsA = 0;

        while(!isEmpty(&stopA_queue) && processed_studentsA < queueA_size){
            int student_id = peek(&stopA_queue);
            student = find_student_id(student_id); 
            if(bus.onBoard[student->department] < N / NUM_DEPARTMENTS){
                bus.onBoard[student->department]++;
                dequeue(&stopA_queue);
                enqueue(&bus_queue, student_id); 
                printf("Student %d from department %d boards the bus at Stop A.\n", student->id, student->department);

                sem_wait(&bus.department_semaphores[student->department]);

            }else{
                /*if the department limit is reached, re-enqueue the student at the end so that we can check all the students*/
                printf("Student %d from department %d cannot board the bus because the department limit is reached.\n", student->id, student->department);
                dequeue(&stopA_queue);
                enqueue(&stopA_queue, student_id);
            }

            processed_studentsA++;
        }
        printStatus();
        pthread_mutex_unlock(&mutex);
        

        pthread_mutex_lock(&mutex);
        printf("Bus is traveling to Stop B...\n");
        pthread_mutex_unlock(&mutex);

        /*simulate travel time*/
        sleep(T);

        pthread_mutex_lock(&mutex);
        printf("Bus arrived at Stop B!!\n");
        pthread_mutex_unlock(&mutex);

        /*students get out of the bus and go to uni*/
        pthread_mutex_lock(&mutex);
        while(!isEmpty(&bus_queue)){
            int student_id = peek(&bus_queue);
            dequeue(&bus_queue);
            student = find_student_id(student_id); 
            bus.onBoard[student->department]--;

            /*add the student to the university array*/
            if(uni_count < MAX_UNI_CAPACITY){
                uni[uni_count++] = student;
                printf("Student %d from department %d enters the university to study.\n", student->id, student->department);
            }
            sem_post(&student_ready[student->id - 1]);
            sem_post(&bus.department_semaphores[student->department]);
        }
        printStatus();
        pthread_mutex_unlock(&mutex);

        pthread_mutex_lock(&mutex);
        printf("Bus is waiting at Stop B...\n");
        pthread_mutex_unlock(&mutex);
        
        /*simulate waiting time*/
        sleep(WAITING_TIME);
        

        /*students board on bus to go back home*/
        pthread_mutex_lock(&mutex);

        int queueB_size = size(&stopB_queue);
        int processed_studentsB = 0;

        while(!isEmpty(&stopB_queue) && processed_studentsB < queueB_size){
            int student_id = peek(&stopB_queue);
            student = find_student_id(student_id);
            if(bus.onBoard[student->department] < N / NUM_DEPARTMENTS){
                bus.onBoard[student->department]++;
                dequeue(&stopB_queue);
                enqueue(&bus_queue, student_id);
                printf("Student %d from department %d boards the bus at Stop B to return home.\n", student->id, student->department);

                sem_wait(&bus.department_semaphores[student->department]);
            }else{
                /*if the department limit is reached, re-enqueue the student at the end so that we can check all the students*/
                printf("Student %d from department %d cannot board the bus because the department limit is reached.\n", student->id, student->department);
                dequeue(&stopB_queue);
                enqueue(&stopB_queue, student_id);
            }

            processed_studentsB++;
        }
        printStatus();
        pthread_mutex_unlock(&mutex);
        

        pthread_mutex_lock(&mutex);
        printf("Bus is traveling back to Stop A...\n");
        pthread_mutex_unlock(&mutex);

        /*simulate travel time*/
        sleep(T);

        pthread_mutex_lock(&mutex);
        printf("Bus arrived at Stop A!!\n");
        pthread_mutex_unlock(&mutex);

        /*students exit bus and go home*/
        pthread_mutex_lock(&mutex);
        while(!isEmpty(&bus_queue)){
            int student_id = peek(&bus_queue);
            dequeue(&bus_queue);
            Student *student = find_student_id(student_id);
            bus.onBoard[student->department]--;
            sem_post(&bus.department_semaphores[student->department]);
            printf("Student %d from department %d exits the bus at Stop A.\n", student->id, student->department);

            students_returned_home++;
        }
        printStatus();
        pthread_mutex_unlock(&mutex);
        

        if(students_returned_home >= students_num){
            printf("All students have returned home. Bus thread terminating.\n");
            break;
        }

    }
    return NULL;
}




int main(int argc, char* argv[]){

    printf("Enter the number of students: ");
    scanf("%d", &students_num);

    if (students_num <= 0 || students_num > 200) {
        printf("Invalid number of students! Please enter a number between 1 and 200.\n");
        return 1;
    }

    /*malloc size for students array and their semaphores*/
    students = malloc(students_num * sizeof(Student));
    student_ready = malloc(students_num * sizeof(sem_t));

    /*seed random number generator*/
    srand(time(NULL));

    /*INITIALIZATIONS*/
    initialize_students(students);
    initialize_bus(&bus);
    initQueues();

    for(int i = 0; i < students_num; i++){
    sem_init(&student_ready[i], 0, 0);
    }

    
    /*create bus thread*/
    pthread_t bus_thread_id;
    pthread_create(&bus_thread_id, NULL, bus_thread, NULL);

    /*create student threads*/
    pthread_t student_threads[students_num];
    for (int i = 0; i < students_num; i++) {
        pthread_create(&student_threads[i], NULL, student_thread, (void*)&students[i]);
    }

    /*wait for all student threads to finish*/
    for (int i = 0; i < students_num; i++) {
        pthread_join(student_threads[i], NULL);
    }

    /*wait for the bus thread to finish*/
    pthread_join(bus_thread_id, NULL);

    /*clean up*/
    pthread_mutex_destroy(&mutex);
    for (int i = 0; i < NUM_DEPARTMENTS; i++) {
        sem_destroy(&bus.department_semaphores[i]);
    }

    printf("SUCCESS!! All students have returned home.\n");
    return 0;
}