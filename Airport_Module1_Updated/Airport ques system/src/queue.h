#ifndef QUEUE_H
#define QUEUE_H

#define MAX_PASSENGERS 100

typedef struct {
    int id;
    char name[50];
    int age;
    int riskLevel;
    int arrivalTime;          /* Simulation time in minutes */
    int screeningDuration;    /* Required screening time in minutes */
    int screeningStartTime;
    int screeningEndTime;
    int waitingTime;
} Passenger;

typedef struct {
    Passenger passengers[MAX_PASSENGERS];
    int front;
    int rear;
    int count;
} Queue;

void initializeQueue(Queue *q);
int enqueue(Queue *q, Passenger p);
int dequeue(Queue *q, Passenger *p);
void displayQueue(Queue *q);

#endif
