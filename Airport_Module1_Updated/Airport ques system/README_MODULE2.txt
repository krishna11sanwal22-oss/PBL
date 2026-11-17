AIRPORT SECURITY QUEUE SIMULATOR - MODULE 2 UPDATE

Added in this update:
1. Performance analytics report (menu option 25)
2. Lane-wise busy minutes and utilisation percentage
3. Officer-wise busy minutes and utilisation percentage
4. Overall lane and officer utilisation
5. Passenger throughput per simulated hour
6. Merge Sort vs Quick Sort execution-time benchmark (option 26)

How utilisation is measured:
- Busy minutes are accumulated from each passenger's scheduled screening interval
  as simulation time advances.
- Lane utilisation = lane busy minutes / simulation minutes * 100.
- Officer utilisation uses the same calculation for the officer assigned to that lane.
- Overall utilisation divides total busy minutes by (simulation minutes * number of lanes/officers).
- A report at simulation time zero shows N/A for throughput and 0% utilisation.

Windows build (run from the folder containing main.c):
gcc -Wall -Wextra -std=c11 -Isrc main.c src/queue.c src/algorithms.c src/greedy.c src/graph.c src/huffman.c -o airport.exe

Run:
.\airport.exe

New menu options:
25. Display Performance Analytics Report
26. Benchmark Merge Sort vs Quick Sort

Note: Benchmark timings vary by computer, compiler, and build settings. They are
comparative measurements, not fixed performance guarantees.
