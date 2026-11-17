
#include <stdio.h>
#include "greedy.h"

/* ================= SORT BY FINISH TIME ================= */

static void sortByFinish(Activity activities[], int n) {

    for (int i = 0; i < n - 1; i++) {

        for (int j = 0; j < n - i - 1; j++) {

            if (activities[j].finish >
                activities[j + 1].finish) {

                Activity temp = activities[j];
                activities[j] = activities[j + 1];
                activities[j + 1] = temp;
            }
        }
    }
}

/* ================= SORT BY START TIME ================= */

static void sortByStart(Activity activities[], int n) {

    for (int i = 0; i < n - 1; i++) {

        for (int j = 0; j < n - i - 1; j++) {

            if (activities[j].start >
                activities[j + 1].start) {

                Activity temp = activities[j];
                activities[j] = activities[j + 1];
                activities[j + 1] = temp;
            }
        }
    }
}

/* ================= ACTIVITY SELECTION ================= */

void activitySelection(Activity activities[], int n) {

    if (n <= 0) {
        printf("No activities available!\n");
        return;
    }

    sortByFinish(activities, n);

    printf("\nSelected Activities:\n");

    int lastFinish = -1;
    int selectedCount = 0;

    for (int i = 0; i < n; i++) {

        if (activities[i].start >= lastFinish) {

            printf("Activity %d | Start: %d | Finish: %d\n",
                   activities[i].id,
                   activities[i].start,
                   activities[i].finish);

            lastFinish = activities[i].finish;
            selectedCount++;
        }
    }

    printf("\nTotal Selected Activities: %d\n",
           selectedCount);
}

/* ================= INTERVAL PARTITIONING ================= */

int intervalPartitioning(Activity activities[], int n) {

    if (n <= 0) {
        printf("No activities available!\n");
        return 0;
    }

    sortByStart(activities, n);

    int laneFinish[MAX_ACTIVITIES];
    int laneCount = 0;

    printf("\n===== INTERVAL PARTITIONING RESULT =====\n");

    for (int i = 0; i < n; i++) {

        int selectedLane = -1;

        for (int j = 0; j < laneCount; j++) {

            if (laneFinish[j] <= activities[i].start) {
                selectedLane = j;
                break;
            }
        }

        if (selectedLane == -1) {

            selectedLane = laneCount;
            laneCount++;
        }

        laneFinish[selectedLane] = activities[i].finish;

        printf("Activity %d | Start: %d | Finish: %d | Lane: %d\n",
               activities[i].id,
               activities[i].start,
               activities[i].finish,
               selectedLane + 1);
    }

    printf("\nMinimum Number of Lanes Required: %d\n",
           laneCount);

    return laneCount;
}
