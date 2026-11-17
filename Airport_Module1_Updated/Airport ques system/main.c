
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "queue.h"
#include "algorithms.h"
#include "greedy.h"
#include "graph.h"
#include "huffman.h"

#define MAX_LANES 5
#define ACTIVE_LANES 3
#define MAX_ARRAY_SIZE 100
#define MAX_LANE_CAPACITY 10
#define MAX_COMPLETED_PASSENGERS 1000
#define MAX_OFFICERS ACTIVE_LANES

/* ================= DATA STRUCTURES ================= */

typedef struct {
    int officerId;
    char name[30];
    int laneId;
    int isOnDuty;
} SecurityOfficer;

typedef struct {
    int laneId;
    int capacity;
    int currentLoad;
    int isOpen;
    int officerId;
    Passenger screened[MAX_LANE_CAPACITY];
} SecurityLane;

Queue securityQueue;
SecurityLane lanes[MAX_LANES];
SecurityOfficer officers[MAX_OFFICERS];
int simulationTime = 0;
int completedPassengerIds[MAX_COMPLETED_PASSENGERS];
int completedPassengerCount = 0;
int totalPassengersScreened = 0;
int totalWaitingTime = 0;
int maximumWaitingTime = 0;
int totalScreeningTime = 0;
int laneBusyMinutes[ACTIVE_LANES] = {0};
int officerBusyMinutes[MAX_OFFICERS] = {0};
int totalPassengersAllocated = 0;

/* ================= INPUT HELPERS ================= */

void clearInputBuffer(void) {
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

int readArray(int arr[], int *size) {

    printf("Enter number of elements (1-%d): ",
           MAX_ARRAY_SIZE);

    if (scanf("%d", size) != 1 ||
        *size < 1 || *size > MAX_ARRAY_SIZE) {

        printf("Invalid array size!\n");
        clearInputBuffer();
        return 0;
    }

    printf("Enter %d integers:\n", *size);

    for (int i = 0; i < *size; i++) {

        if (scanf("%d", &arr[i]) != 1) {
            printf("Invalid input!\n");
            clearInputBuffer();
            return 0;
        }
    }

    return 1;
}

void displayArray(const int arr[], int size) {

    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }

    printf("\n");
}

/* ================= LANE INITIALIZATION ================= */

void initializeLanes(void) {

    for (int i = 0; i < ACTIVE_LANES; i++) {

        lanes[i].laneId = i + 1;
        lanes[i].capacity = MAX_LANE_CAPACITY;
        lanes[i].currentLoad = 0;
        lanes[i].isOpen = 1;
        lanes[i].officerId = i + 1;
        officers[i].officerId = i + 1;
        snprintf(officers[i].name, sizeof(officers[i].name), "Officer-%d", i + 1);
        officers[i].laneId = i + 1;
        officers[i].isOnDuty = 1;
    }
}

/* ================= PASSENGER ID CHECK ================= */

int passengerIdExists(int id) {

    for (int i = 0; i < securityQueue.count; i++) {

        int index =
            (securityQueue.front + i) % MAX_PASSENGERS;

        if (securityQueue.passengers[index].id == id) {
            return 1;
        }
    }

    for (int i = 0; i < ACTIVE_LANES; i++) {
        for (int j = 0; j < lanes[i].currentLoad; j++) {
            if (lanes[i].screened[j].id == id) return 1;
        }
    }

    for (int i = 0; i < completedPassengerCount; i++) {
        if (completedPassengerIds[i] == id) return 1;
    }
    return 0;
}

/* ================= PASSENGER REGISTRATION ================= */

void addPassenger(void) {

    Passenger p;

    if (securityQueue.count >= MAX_PASSENGERS) {
        printf("Waiting queue is full!\n");
        return;
    }

    printf("\n===== PASSENGER REGISTRATION =====\n");

    printf("Enter Passenger ID: ");

    if (scanf("%d", &p.id) != 1 || p.id <= 0) {
        printf("Invalid passenger ID!\n");
        clearInputBuffer();
        return;
    }

    if (passengerIdExists(p.id)) {
        printf("Passenger ID already exists!\n");
        return;
    }

    printf("Enter Passenger Name: ");

    if (scanf(" %49[^\n]", p.name) != 1) {
        printf("Invalid name!\n");
        clearInputBuffer();
        return;
    }

    printf("Enter Age: ");

    if (scanf("%d", &p.age) != 1 ||
        p.age < 0 || p.age > 120) {

        printf("Invalid age!\n");
        clearInputBuffer();
        return;
    }

    printf("Enter Risk Level (1-3): ");

    if (scanf("%d", &p.riskLevel) != 1 ||
        p.riskLevel < 1 || p.riskLevel > 3) {

        printf("Invalid risk level! Enter 1, 2 or 3.\n");
        clearInputBuffer();
        return;
    }

    printf("Enter estimated screening duration (1-30 minutes): ");
    if (scanf("%d", &p.screeningDuration) != 1 ||
        p.screeningDuration < 1 || p.screeningDuration > 30) {
        printf("Invalid screening duration!\n");
        clearInputBuffer();
        return;
    }

    p.arrivalTime = simulationTime;
    p.screeningStartTime = -1;
    p.screeningEndTime = -1;
    p.waitingTime = 0;

    if (enqueue(&securityQueue, p)) {
        printf("Passenger registered successfully at simulation time %d minutes!\n", simulationTime);
    } else {
        printf("Passenger registration failed!\n");
    }
}

/* ================= RISK-BASED PRIORITY QUEUE ================= */

int dequeueHighestRisk(Queue *q, Passenger *p) {

    if (q->count == 0) {
        return 0;
    }

    Passenger temp[MAX_PASSENGERS];
    int selectedIndex = 0;

    for (int i = 1; i < q->count; i++) {

        int index =
            (q->front + i) % MAX_PASSENGERS;

        int selected =
            (q->front + selectedIndex) % MAX_PASSENGERS;

        if (q->passengers[index].riskLevel >
            q->passengers[selected].riskLevel) {

            selectedIndex = i;
        }
    }

    int selectedPosition =
        (q->front + selectedIndex) % MAX_PASSENGERS;

    *p = q->passengers[selectedPosition];

    int newCount = 0;

    for (int i = 0; i < q->count; i++) {

        if (i == selectedIndex) {
            continue;
        }

        int index =
            (q->front + i) % MAX_PASSENGERS;

        temp[newCount++] = q->passengers[index];
    }

    for (int i = 0; i < newCount; i++) {
        q->passengers[i] = temp[i];
    }

    q->front = 0;
    q->count = newCount;
    q->rear = newCount - 1;

    return 1;
}

/* ================= DISPLAY SECURITY LANES ================= */

void displayLanes(void) {

    printf("\n========== SECURITY LANES ==========\n");

    for (int i = 0; i < ACTIVE_LANES; i++) {

        SecurityLane *lane = &lanes[i];

        printf("\nLane %d | Status: %s | Load: %d/%d | Officer: %s\n",
               lane->laneId, lane->isOpen ? "OPEN" : "CLOSED",
               lane->currentLoad, lane->capacity,
               officers[lane->officerId - 1].name);

        if (lane->currentLoad == 0) {
            printf("No passengers currently screening.\n");
        }

        for (int j = 0; j < lane->currentLoad; j++) {

            Passenger p = lane->screened[j];

            printf("ID: %d | Name: %s | Age: %d | Risk: %d | Start: %d | End: %d\n",
                   p.id, p.name, p.age, p.riskLevel,
                   p.screeningStartTime, p.screeningEndTime);
        }
    }
}

/* ================= FIND AVAILABLE LANE ================= */

int findAvailableLane(void) {
    int selectedLane = -1;
    int earliestFreeTime = 0x7fffffff;

    for (int i = 0; i < ACTIVE_LANES; i++) {
        SecurityLane *lane = &lanes[i];
        int officerIndex = lane->officerId - 1;
        if (!lane->isOpen || lane->currentLoad >= lane->capacity ||
            officerIndex < 0 || officerIndex >= MAX_OFFICERS ||
            !officers[officerIndex].isOnDuty) continue;

        int laneFreeTime = simulationTime;
        if (lane->currentLoad > 0) {
            laneFreeTime = lane->screened[lane->currentLoad - 1].screeningEndTime;
        }
        if (selectedLane == -1 || laneFreeTime < earliestFreeTime ||
            (laneFreeTime == earliestFreeTime &&
             lane->currentLoad < lanes[selectedLane].currentLoad)) {
            selectedLane = i;
            earliestFreeTime = laneFreeTime;
        }
    }
    return selectedLane;
}

/* ================= PROCESS NEXT PASSENGER ================= */

void processNextPassenger(void) {

    if (securityQueue.count == 0) {
        printf("No passengers waiting!\n");
        return;
    }

    int selectedLane = findAvailableLane();

    if (selectedLane == -1) {
        printf("No available security lane!\n");
        return;
    }

    Passenger p;

    if (!dequeueHighestRisk(&securityQueue, &p)) {
        return;
    }

    SecurityLane *lane = &lanes[selectedLane];
    int startTime = simulationTime;
    if (lane->currentLoad > 0) {
        int previousEnd = lane->screened[lane->currentLoad - 1].screeningEndTime;
        if (previousEnd > startTime) startTime = previousEnd;
    }
    p.screeningStartTime = startTime;
    p.screeningEndTime = startTime + p.screeningDuration;
    p.waitingTime = startTime - p.arrivalTime;
    if (p.waitingTime < 0) p.waitingTime = 0;

    lane->screened[lane->currentLoad] = p;
    lane->currentLoad++;
    totalPassengersAllocated++;

    printf("\n===== SECURITY SCREENING SCHEDULED =====\n");

    printf("Passenger ID : %d\n", p.id);
    printf("Name         : %s\n", p.name);
    printf("Age          : %d\n", p.age);
    printf("Risk Level   : %d\n", p.riskLevel);
    printf("Assigned Lane: %d\n", lane->laneId);
    printf("Assigned Officer: %s\n", officers[lane->officerId - 1].name);
    printf("Arrival Time : %d minutes\n", p.arrivalTime);
    printf("Start Time   : %d minutes\n", p.screeningStartTime);
    printf("Expected End : %d minutes\n", p.screeningEndTime);
    printf("Waiting Time : %d minutes\n", p.waitingTime);
    printf("Status       : Scheduled / in lane\n");
}

void autoAllocateWaitingPassengers(void) {
    if (securityQueue.count == 0) {
        printf("No passengers waiting for allocation.\n");
        return;
    }
    int assigned = 0;
    while (securityQueue.count > 0 && findAvailableLane() != -1) {
        int before = securityQueue.count;
        processNextPassenger();
        if (securityQueue.count == before) break;
        assigned++;
    }
    printf("\nAutomatic allocation completed. Passengers assigned: %d | Still waiting: %d\n",
           assigned, securityQueue.count);
}

/* ================= COMPLETE SCREENING ================= */

void completeScreening(void) {

    int laneId;

    printf("Enter Lane ID (1-%d): ", ACTIVE_LANES);

    if (scanf("%d", &laneId) != 1 ||
        laneId < 1 || laneId > ACTIVE_LANES) {

        printf("Invalid lane ID!\n");
        clearInputBuffer();
        return;
    }

    SecurityLane *lane = &lanes[laneId - 1];

    if (lane->currentLoad == 0) {
        printf("No passenger in this lane!\n");
        return;
    }

    Passenger completed = lane->screened[0];
    if (simulationTime < completed.screeningEndTime) {
        printf("Screening is not finished yet. Remaining time: %d minute(s).\n",
               completed.screeningEndTime - simulationTime);
        return;
    }

    printf("\n===== SCREENING COMPLETED =====\n");

    printf("Passenger ID: %d\n", completed.id);
    printf("Name: %s\n", completed.name);
    printf("Risk Level: %d\n", completed.riskLevel);
    printf("Waiting Time: %d minutes\n", completed.waitingTime);
    printf("Screening Duration: %d minutes\n", completed.screeningDuration);

    if (completedPassengerCount < MAX_COMPLETED_PASSENGERS) {
        completedPassengerIds[completedPassengerCount++] = completed.id;
    }
    totalPassengersScreened++;
    totalWaitingTime += completed.waitingTime;
    totalScreeningTime += completed.screeningDuration;
    if (completed.waitingTime > maximumWaitingTime) maximumWaitingTime = completed.waitingTime;

    for (int i = 0; i < lane->currentLoad - 1; i++) {
        lane->screened[i] = lane->screened[i + 1];
    }

    lane->currentLoad--;

    printf("Screening completed successfully!\n");
}

/* ================= LANE OPEN/CLOSE ================= */

void toggleLane(void) {

    int laneId;

    printf("Enter Lane ID (1-%d): ", ACTIVE_LANES);

    if (scanf("%d", &laneId) != 1 ||
        laneId < 1 || laneId > ACTIVE_LANES) {

        printf("Invalid lane ID!\n");
        clearInputBuffer();
        return;
    }

    SecurityLane *lane = &lanes[laneId - 1];

    if (lane->isOpen) {

        if (lane->currentLoad > 0) {
            printf("Cannot close lane while passengers are screening!\n");
            return;
        }

        lane->isOpen = 0;
        printf("Lane CLOSED successfully.\n");

    } else {

        lane->isOpen = 1;
        printf("Lane OPENED successfully.\n");
    }
}

/* ================= SYSTEM STATISTICS ================= */

void displayStatistics(void) {
    int openLanes = 0, totalLoad = 0, totalCapacity = 0;
    for (int i = 0; i < ACTIVE_LANES; i++) {
        if (lanes[i].isOpen) {
            openLanes++;
            totalLoad += lanes[i].currentLoad;
            totalCapacity += lanes[i].capacity;
        }
    }
    printf("\n========== SYSTEM STATISTICS ==========\n");
    printf("Simulation Time     : %d minutes\n", simulationTime);
    printf("Passengers Waiting  : %d\n", securityQueue.count);
    printf("Open Security Lanes : %d\n", openLanes);
    printf("Passengers Screening: %d\n", totalLoad);
    printf("Passengers Screened : %d\n", totalPassengersScreened);
    printf("Available Capacity  : %d\n", totalCapacity - totalLoad);
    printf("Maximum Waiting Time: %d minutes\n", maximumWaitingTime);
    if (totalPassengersScreened > 0) {
        printf("Average Waiting Time: %.2f minutes\n",
               (double)totalWaitingTime / totalPassengersScreened);
        printf("Average Screening Time: %.2f minutes\n",
               (double)totalScreeningTime / totalPassengersScreened);
    } else {
        printf("Average Waiting Time: N/A\nAverage Screening Time: N/A\n");
    }
}

/* ================= MODULE 2: PERFORMANCE ANALYTICS ================= */

void displayPerformanceReport(void) {
    int totalLaneBusy = 0;
    int totalOfficerBusy = 0;
    double elapsedHours = (double)simulationTime / 60.0;

    printf("\n========== PERFORMANCE ANALYTICS REPORT ==========\n");
    printf("Simulation duration       : %d minutes\n", simulationTime);
    printf("Passengers registered for screening: %d\n", totalPassengersAllocated);
    printf("Passengers completed      : %d\n", totalPassengersScreened);
    printf("Passengers currently active: ");
    {
        int active = 0;
        for (int i = 0; i < ACTIVE_LANES; i++) active += lanes[i].currentLoad;
        printf("%d\n", active);
    }
    if (elapsedHours > 0.0)
        printf("Passenger throughput      : %.2f passengers/hour\n", totalPassengersScreened / elapsedHours);
    else
        printf("Passenger throughput      : N/A (advance simulation time)\n");

    printf("\n--- Lane Utilisation ---\n");
    for (int i = 0; i < ACTIVE_LANES; i++) {
        double utilisation = simulationTime > 0 ?
            (100.0 * laneBusyMinutes[i] / simulationTime) : 0.0;
        if (utilisation > 100.0) utilisation = 100.0;
        totalLaneBusy += laneBusyMinutes[i];
        printf("Lane %d: busy %d min | utilisation %.2f%% | %s\n",
               lanes[i].laneId, laneBusyMinutes[i], utilisation,
               lanes[i].isOpen ? "OPEN" : "CLOSED");
    }
    printf("Overall lane utilisation   : %.2f%%\n",
           simulationTime > 0 ? 100.0 * totalLaneBusy / (simulationTime * ACTIVE_LANES) : 0.0);

    printf("\n--- Officer Utilisation ---\n");
    for (int i = 0; i < MAX_OFFICERS; i++) {
        double utilisation = simulationTime > 0 ?
            (100.0 * officerBusyMinutes[i] / simulationTime) : 0.0;
        if (utilisation > 100.0) utilisation = 100.0;
        totalOfficerBusy += officerBusyMinutes[i];
        printf("%s: busy %d min | utilisation %.2f%% | duty %s\n",
               officers[i].name, officerBusyMinutes[i], utilisation,
               officers[i].isOnDuty ? "ON" : "OFF");
    }
    printf("Overall officer utilisation: %.2f%%\n",
           simulationTime > 0 ? 100.0 * totalOfficerBusy / (simulationTime * MAX_OFFICERS) : 0.0);

    printf("\n--- Waiting and Screening Metrics ---\n");
    printf("Maximum waiting time       : %d minutes\n", maximumWaitingTime);
    if (totalPassengersScreened > 0) {
        printf("Average waiting time       : %.2f minutes\n", (double)totalWaitingTime / totalPassengersScreened);
        printf("Average screening time     : %.2f minutes\n", (double)totalScreeningTime / totalPassengersScreened);
    } else {
        printf("Average waiting time       : N/A\nAverage screening time     : N/A\n");
    }
    printf("==================================================\n");
}

void benchmarkSortingAlgorithms(void) {
    enum { BENCH_SIZE = 10000 };
    int mergeData[BENCH_SIZE];
    int quickData[BENCH_SIZE];
    clock_t start, end;
    double mergeSeconds, quickSeconds;

    for (int i = 0; i < BENCH_SIZE; i++) {
        int value = (BENCH_SIZE - i) * 37 % 100003;
        mergeData[i] = value;
        quickData[i] = value;
    }

    start = clock();
    mergeSort(mergeData, 0, BENCH_SIZE - 1);
    end = clock();
    mergeSeconds = (double)(end - start) / CLOCKS_PER_SEC;

    start = clock();
    quickSort(quickData, 0, BENCH_SIZE - 1);
    end = clock();
    quickSeconds = (double)(end - start) / CLOCKS_PER_SEC;

    printf("\n========== ALGORITHM BENCHMARK ==========");
    printf("\nDataset size : %d integers (same input data)", BENCH_SIZE);
    printf("\nMerge Sort   : %.6f seconds", mergeSeconds);
    printf("\nQuick Sort   : %.6f seconds", quickSeconds);
    printf("\nNote: timings depend on your computer and compiler.\n");
}

void advanceSimulationTime(void) {
    int minutes;
    printf("Current simulation time: %d minutes\n", simulationTime);
    printf("Advance by how many minutes? ");
    if (scanf("%d", &minutes) != 1 || minutes <= 0 || minutes > 1440) {
        printf("Enter a valid value between 1 and 1440.\n");
        clearInputBuffer();
        return;
    }
    {
        int oldTime = simulationTime;
        int newTime = simulationTime + minutes;
        for (int i = 0; i < ACTIVE_LANES; i++) {
            SecurityLane *lane = &lanes[i];
            for (int j = 0; j < lane->currentLoad; j++) {
                Passenger *p = &lane->screened[j];
                int start = p->screeningStartTime > oldTime ? p->screeningStartTime : oldTime;
                int end = p->screeningEndTime < newTime ? p->screeningEndTime : newTime;
                if (end > start) {
                    int busy = end - start;
                    laneBusyMinutes[i] += busy;
                    if (lane->officerId >= 1 && lane->officerId <= MAX_OFFICERS)
                        officerBusyMinutes[lane->officerId - 1] += busy;
                }
            }
        }
        simulationTime = newTime;
    }
    printf("Simulation time advanced to %d minutes.\n", simulationTime);
}

void displayOfficers(void) {
    printf("\n========== SECURITY OFFICERS ==========\n");
    for (int i = 0; i < MAX_OFFICERS; i++) {
        int load = lanes[officers[i].laneId - 1].currentLoad;
        printf("Officer: %s | ID: %d | Lane: %d | Duty: %s | Status: %s\n",
               officers[i].name, officers[i].officerId, officers[i].laneId,
               officers[i].isOnDuty ? "ON" : "OFF",
               load == 0 ? "AVAILABLE" : "BUSY / QUEUED");
    }
}

void toggleOfficerDuty(void) {
    int officerId;
    printf("Enter Officer ID (1-%d): ", MAX_OFFICERS);
    if (scanf("%d", &officerId) != 1 || officerId < 1 || officerId > MAX_OFFICERS) {
        printf("Invalid officer ID!\n");
        clearInputBuffer();
        return;
    }
    SecurityOfficer *officer = &officers[officerId - 1];
    SecurityLane *lane = &lanes[officer->laneId - 1];
    if (officer->isOnDuty && lane->currentLoad > 0) {
        printf("Cannot take officer off duty while assigned lane has passengers.\n");
        return;
    }
    officer->isOnDuty = !officer->isOnDuty;
    printf("%s is now %s duty.\n", officer->name,
           officer->isOnDuty ? "ON" : "OFF");
}

/* ================= MERGE SORT ================= */

void runMergeSort(void) {

    int arr[MAX_ARRAY_SIZE];
    int size;

    if (!readArray(arr, &size)) return;

    printf("Before Merge Sort: ");
    displayArray(arr, size);

    mergeSort(arr, 0, size - 1);

    printf("After Merge Sort: ");
    displayArray(arr, size);
}

/* ================= QUICK SORT ================= */

void runQuickSort(void) {

    int arr[MAX_ARRAY_SIZE];
    int size;

    if (!readArray(arr, &size)) return;

    printf("Before Quick Sort: ");
    displayArray(arr, size);

    quickSort(arr, 0, size - 1);

    printf("After Quick Sort: ");
    displayArray(arr, size);
}

/* ================= ITERATIVE BINARY SEARCH ================= */

void runIterativeBinarySearch(void) {

    int arr[MAX_ARRAY_SIZE];
    int size, target;

    if (!readArray(arr, &size)) return;

    mergeSort(arr, 0, size - 1);

    printf("Sorted Array: ");
    displayArray(arr, size);

    printf("Enter value to search: ");

    if (scanf("%d", &target) != 1) {
        printf("Invalid input!\n");
        clearInputBuffer();
        return;
    }

    int index =
        binarySearchIterative(arr, size, target);

    if (index != -1) {
        printf("Value found at index %d.\n", index);
    } else {
        printf("Value not found.\n");
    }
}

/* ================= RECURSIVE BINARY SEARCH ================= */

void runRecursiveBinarySearch(void) {

    int arr[MAX_ARRAY_SIZE];
    int size, target;

    if (!readArray(arr, &size)) return;

    mergeSort(arr, 0, size - 1);

    printf("Sorted Array: ");
    displayArray(arr, size);

    printf("Enter value to search: ");

    if (scanf("%d", &target) != 1) {
        printf("Invalid input!\n");
        clearInputBuffer();
        return;
    }

    int index =
        binarySearchRecursive(arr, 0, size - 1, target);

    if (index != -1) {
        printf("Value found at index %d.\n", index);
    } else {
        printf("Value not found.\n");
    }
}

/* ================= ACTIVITY SELECTION ================= */

void runActivitySelection(void) {

    Activity activities[MAX_ACTIVITIES];
    int n;

    printf("\n===== ACTIVITY SELECTION =====\n");

    printf("Enter number of activities (1-%d): ",
           MAX_ACTIVITIES);

    if (scanf("%d", &n) != 1 ||
        n < 1 || n > MAX_ACTIVITIES) {

        printf("Invalid number of activities!\n");
        clearInputBuffer();
        return;
    }

    for (int i = 0; i < n; i++) {

        activities[i].id = i + 1;

        printf("\nActivity %d\n", i + 1);

        printf("Enter start time: ");

        if (scanf("%d", &activities[i].start) != 1 ||
            activities[i].start < 0) {

            printf("Invalid start time!\n");
            clearInputBuffer();
            return;
        }

        printf("Enter finish time: ");

        if (scanf("%d", &activities[i].finish) != 1 ||
            activities[i].finish <= activities[i].start) {

            printf("Invalid finish time!\n");
            clearInputBuffer();
            return;
        }
    }

    activitySelection(activities, n);
}

/* ================= INTERVAL PARTITIONING ================= */

void runIntervalPartitioning(void) {

    Activity activities[MAX_ACTIVITIES];
    int n;

    printf("\n===== INTERVAL PARTITIONING =====\n");

    printf("Enter number of activities (1-%d): ",
           MAX_ACTIVITIES);

    if (scanf("%d", &n) != 1 ||
        n < 1 || n > MAX_ACTIVITIES) {

        printf("Invalid number of activities!\n");
        clearInputBuffer();
        return;
    }

    for (int i = 0; i < n; i++) {

        activities[i].id = i + 1;

        printf("\nActivity %d\n", i + 1);

        printf("Enter start time: ");

        if (scanf("%d", &activities[i].start) != 1 ||
            activities[i].start < 0) {

            printf("Invalid start time!\n");
            clearInputBuffer();
            return;
        }

        printf("Enter finish time: ");

        if (scanf("%d", &activities[i].finish) != 1 ||
            activities[i].finish <= activities[i].start) {

            printf("Invalid finish time!\n");
            clearInputBuffer();
            return;
        }
    }

    intervalPartitioning(activities, n);
}

/* ================= MAIN FUNCTION ================= */

int main(void) {

    int choice;

    initializeQueue(&securityQueue);
    initializeLanes();

    do {

        printf("\n========================================\n");
        printf("     AIRPORT SECURITY QUEUE SIMULATOR\n");
        printf("========================================\n");

        printf("1.  Register Passenger\n");
        printf("2.  Display Waiting Queue\n");
        printf("3.  Assign Highest-Risk Passenger to Lane\n");
        printf("4.  Display Security Lanes\n");
        printf("5.  Complete Screening\n");
        printf("6.  Open/Close Security Lane\n");
        printf("7.  Display System Statistics\n");
        printf("8.  Test Merge Sort\n");
        printf("9.  Test Quick Sort\n");
        printf("10. Test Iterative Binary Search\n");
        printf("11. Test Recursive Binary Search\n");
        printf("12. Test Activity Selection\n");
        printf("13. Test Interval Partitioning\n");
        printf("14. Test Dijkstra Shortest Path\n");
        printf("15. Test Huffman Coding\n");
        printf("16. Test Prim's Algorithm\n");
        printf("17. Test Kruskal's Algorithm\n");
        printf("18. Test Bellman-Ford Algorithm\n");
        printf("19. Test Floyd-Warshall Algorithm\n");
        printf("20. Test 0/1 Knapsack Algorithm\n");
        printf("21. Advance Simulation Time\n");
        printf("22. Display Security Officers\n");
        printf("23. Toggle Officer Duty\n");
        printf("24. Automatically Allocate All Waiting Passengers\n");
        printf("25. Display Performance Analytics Report\n");
        printf("26. Benchmark Merge Sort vs Quick Sort\n");
        printf("0.  Exit\n");

        printf("\nEnter your choice: ");

        if (scanf("%d", &choice) != 1) {

            printf("Invalid menu input!\n");
            clearInputBuffer();
            continue;
        }

        switch (choice) {

            case 1:
                addPassenger();
                break;

            case 2:
                displayQueue(&securityQueue);
                break;

            case 3:
                processNextPassenger();
                break;

            case 4:
                displayLanes();
                break;

            case 5:
                completeScreening();
                break;

            case 6:
                toggleLane();
                break;

            case 7:
                displayStatistics();
                break;

            case 8:
                runMergeSort();
                break;

            case 9:
                runQuickSort();
                break;

            case 10:
                runIterativeBinarySearch();
                break;

            case 11:
                runRecursiveBinarySearch();
                break;

            case 12:
                runActivitySelection();
                break;

            case 13:
                runIntervalPartitioning();
                break;

            case 14:
                runDijkstra();
                break;

            case 15:
                clearInputBuffer();
                runHuffmanCoding();
                break;

            case 16:
                runPrim();
                break;

            case 17:
                runKruskal();
                break;

            case 18:
                runBellmanFord();
                break;

            case 19:
                runFloydWarshall();
                break;

            case 20:
                runKnapsack();
                break;

            case 21:
                advanceSimulationTime();
                break;

            case 22:
                displayOfficers();
                break;

            case 23:
                toggleOfficerDuty();
                break;

            case 24:
                autoAllocateWaitingPassengers();
                break;

            case 25:
                displayPerformanceReport();
                break;

            case 26:
                benchmarkSortingAlgorithms();
                break;

            case 0:
                printf("\nSimulator closed. Goodbye!\n");
                break;

            default:
                printf("Invalid choice! Try again.\n");
        }

    } while (choice != 0);

    return 0;
}
