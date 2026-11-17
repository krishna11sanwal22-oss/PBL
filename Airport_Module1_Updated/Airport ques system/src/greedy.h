
#ifndef GREEDY_H
#define GREEDY_H

#define MAX_ACTIVITIES 100

typedef struct {
    int id;
    int start;
    int finish;
} Activity;

void activitySelection(Activity activities[], int n);
int intervalPartitioning(Activity activities[], int n);

#endif
