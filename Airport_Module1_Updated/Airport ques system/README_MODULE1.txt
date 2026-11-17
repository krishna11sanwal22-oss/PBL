AIRPORT SECURITY QUEUE SIMULATOR - MODULE 1 UPDATE

Build (Windows PowerShell / GCC):
gcc -Wall -Wextra -std=c11 -Iinclude -Isrc main.c src/queue.c src/algorithms.c src/greedy.c src/graph.c src/huffman.c -o airport.exe

Run:
.\airport.exe

New Module 1 features:
- Simulation clock measured in minutes (starts at 0).
- Passenger arrival time recorded at registration.
- Screening duration entered per passenger (1-30 minutes).
- Three security officer records, each assigned to a lane.
- Officer duty can be toggled; an officer cannot go off duty while their lane contains passengers.
- Risk priority: risk 3, then 2, then 1; FIFO is preserved for equal risk levels.
- Automatic lane allocation chooses the eligible lane with the earliest next-free time.
- A passenger's screening start/end and waiting time are scheduled.
- Screening cannot be completed before its scheduled end time.
- Waiting, average/max waiting and average screening duration statistics.
- Completed passenger IDs are retained to prevent duplicate registration after screening.
- Menu 21 advances simulation time.
- Menu 22 displays officers.
- Menu 23 toggles officer duty.
- Menu 24 automatically assigns as many waiting passengers as available lane capacity permits.

Simulation note:
The clock does not advance automatically. Use menu option 21 to advance time before completing screening.
