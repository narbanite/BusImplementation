/**
 * @file queues.c
 * @author Nikoletta Arvaniti (csd4844@csd.uoc.gr)
 * @date 31-10-2024
 * 
 * 
 */

// C Program to demonstrate how to Implement a queue
#include "queues.h"
#include <stdbool.h>
#include <stdio.h>
#define MAX_SIZE 100

// Function to initialize the queue
void initializeQueue(Queue* q)
{
    q->front = -1;
    q->rear = 0;
}

// Function to check if the queue is empty
bool isEmpty(Queue* q) { return (q->front == q->rear - 1); }

// Function to check if the queue is full
bool isFull(Queue* q) { return (q->rear == MAX_SIZE); }

// Function to add an element to the queue (Enqueue
// operation)
void enqueue(Queue* q, int value)
{
    if (isFull(q)) {
        printf("Queue is full\n");
        return;
    }
    q->items[q->rear] = value;
    q->rear++;
}

// Function to remove an element from the queue (Dequeue
// operation)
void dequeue(Queue* q)
{
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return;
    }
    q->front++;
}

// Function to get the element at the front of the queue
// (Peek operation)
int peek(Queue* q)
{
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return -1; // return some default value or handle
                   // error differently
    }
    return q->items[q->front + 1];
}

// Function to print the current queue
void printQueue(Queue* q, const char* label)
{
    if (isEmpty(q)) {
        printf("%s: [ ]\n", label);
        return;
    }

    printf("%s: [", label);
    for (int i = q->front + 1; i < q->rear; i++) {
        printf("%d ", q->items[i]);
        if(i < q->rear - 1){
            printf(", ");
        }
    }
    printf("]\n");
}

// Function to find the size of the queue
int size(Queue* q) {
    if (q->rear >= q->front) {
        return q->rear - q->front;
    } else {
        return MAX_SIZE - q->front + q->rear;
    }
}