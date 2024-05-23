# CPU Scheduling Simulation
This program simulates CPU scheduling algorithms using a set of processes read from an input file. The implemented scheduling algorithms include First-Come, First-Served (FCFS), Shortest Job First (SJF), and Round-Robin (RR) with different time quantum values.

## Usage
`gcc cpu-scheduler.c -o output.c`<br>
`./output.c input.txt`

## Input File Format
process_name, arrival_time, priority, burst_time, ram_required, cpu_usage

- **process_name:** Name of the process (up to 10 characters).
- **arrival_time:** Arrival time of the process.
- **priority:** Priority of the process (0 for highest, 1 for high, 2 for medium, 3 for low).
- **burst_time:** Burst time of the process.
- **ram_required:** Amount of RAM required by the process.
- **cpu_usage:** CPU usage of the process.

## Output
The program will generate an output file named **output.txt** containing the simulation results.

