#include <stdio.h>
#include "queue.h"

void initializeQueue(Queue *q) {
    if (!q) return;
    q->front = 0;
    q->rear = -1;
    q->count = 0;
}

int enqueue(Queue *q, Passenger p) {
    if (!q || q->count >= MAX_PASSENGERS) {
        printf("Security queue is full!\n");
        return 0;
    }
    q->rear = (q->rear + 1) % MAX_PASSENGERS;
    q->passengers[q->rear] = p;
    q->count++;
    return 1;
}

int dequeue(Queue *q, Passenger *p) {
    if (!q || !p || q->count <= 0) {
        printf("No passengers waiting!\n");
        return 0;
    }
    *p = q->passengers[q->front];
    q->front = (q->front + 1) % MAX_PASSENGERS;
    q->count--;
    if (q->count == 0) {
        q->front = 0;
        q->rear = -1;
    }
    return 1;
}

void displayQueue(Queue *q) {
    if (!q || q->count == 0) {
        printf("\nWaiting queue is empty!\n");
        return;
    }
    printf("\n===== WAITING SECURITY QUEUE =====\n");
    for (int i = 0; i < q->count; i++) {
        int index = (q->front + i) % MAX_PASSENGERS;
        Passenger p = q->passengers[index];
        printf("ID: %d | Name: %s | Risk: %d | Arrival: %d min | Screening: %d min\n",
               p.id, p.name, p.riskLevel, p.arrivalTime, p.screeningDuration);
    }
    printf("Passengers waiting: %d\n", q->count);
}
