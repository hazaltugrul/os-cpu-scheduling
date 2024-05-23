#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define constants for maximum processes and priority levels
#define MAX_PROCESSES 100
#define HIGHEST_PRIORITY 0
#define HIGH_PRIORITY 1
#define MEDIUM_PRIORITY 2
#define LOW_PRIORITY 3

// Define constants for RAM size and CPU capacity
#define CPU2_RAM_SIZE 1536
#define CPU1_RAM_SIZE 512
#define TOTAL_CPU 100

// Define the Process structure to store process information
typedef struct {
    char name[10];
    int arrival_time;
    int priority;
    int burst_time;
    int ram_required;
    int cpu_usage;
    int remaining_time;
} Process;

// Define the ProcessQueue structure to manage processes in a queue
typedef struct {
    Process processes[MAX_PROCESSES];
    int count;
} ProcessQueue;

// Initialize a process queue by setting its count to zero
void initializeQueue(ProcessQueue *queue) {
    queue->count = 0;
}

// Add a process to the end of the queue
void enqueue(ProcessQueue *queue, Process process) {
    queue->processes[queue->count++] = process;
}

// Read processes from a file and allocate them to the appropriate CPU queue
void readProcessesFromFile(const char *filename, ProcessQueue *cpu1_queue, ProcessQueue *cpu2_queues) {
    FILE *input_file = fopen(filename, "r");
    if (input_file == NULL) {
        printf("Error: Could not open input file.\n");
        return;
    }

    char line[100];
    while (fgets(line, sizeof(line), input_file)) {
        Process process;
        sscanf(line, "%[^,],%d,%d,%d,%d,%d", process.name, &process.arrival_time, &process.priority,
               &process.burst_time, &process.ram_required, &process.cpu_usage);
        process.remaining_time = process.burst_time;

        // Check if process exceeds total CPU capacity
        if (process.cpu_usage > TOTAL_CPU) {
            printf("Required CPU usage exceeds the CPU's capacity. Skipping %s\n", process.name);
            continue;
        }

        // Allocate process to the correct queue based on its priority
        if (process.priority == HIGHEST_PRIORITY) {
            if (process.ram_required > CPU1_RAM_SIZE) {
                printf("Process %s cannot be allocated due to insufficient RAM.\n", process.name);
                continue;
            }
            enqueue(cpu1_queue, process);
        } else if (process.priority >= HIGH_PRIORITY && process.priority <= LOW_PRIORITY) {
            if (process.cpu_usage > CPU2_RAM_SIZE) {
                printf("Process %s cannot be allocated due to insufficient RAM.\n", process.name);
                continue;
            }
            enqueue(&cpu2_queues[process.priority - 1], process);
        } else {
            printf("Invalid priority for process %s. Skipping.\n", process.name);
        }
    }

    fclose(input_file);
}

// Print the queue to the output file with specified CPU number, priority, and scheduler type
void printQueue(FILE *output_file, ProcessQueue *queue, int cpu_number, int priority, const char *scheduler) {
    fprintf(output_file, "CPU-%d queue(priority-%d) (%s): ", cpu_number, priority, scheduler);
    for (int i = 0; i < queue->count; i++) {
        fprintf(output_file, "%s", queue->processes[i].name);
        if (i < queue->count - 1) {
            fprintf(output_file, "->");
        }
    }
    fprintf(output_file, "\n");
}

// Execute processes in First-Come, First-Served (FCFS) order for CPU-1
void executeFCFS(FILE *output_file, ProcessQueue *cpu1_queue) {
    for (int i = 0; i < cpu1_queue->count; i++) {
        fprintf(output_file, "Process %s is queued to be assigned to CPU-1.\n", cpu1_queue->processes[i].name);
        fprintf(output_file, "Process %s is assigned to CPU-1.\n", cpu1_queue->processes[i].name);
        fprintf(output_file, "Process %s is completed and terminated.\n\n", cpu1_queue->processes[i].name);
    }
}

// Execute processes in Shortest Job First (SJF) order for CPU-2
void executeSJF(FILE *output_file, ProcessQueue *queue) {
    // Sort processes based on burst time
    for (int i = 0; i < queue->count; i++) {
        for (int j = i + 1; j < queue->count; j++) {
            if (queue->processes[j].burst_time < queue->processes[i].burst_time) {
                Process temp = queue->processes[i];
                queue->processes[i] = queue->processes[j];
                queue->processes[j] = temp;
            }
        }
    }

    // Execute processes in sorted order
    for (int i = 0; i < queue->count; i++) {
        fprintf(output_file, "Process %s is placed in the SJF queue to be assigned to CPU-2\n", queue->processes[i].name);
        fprintf(output_file, "Process %s is assigned to CPU-2.\n", queue->processes[i].name);
        fprintf(output_file, "Process %s is completed and terminated.\n\n", queue->processes[i].name);
    }
}

// Execute processes in Round Robin (RR) order for CPU-2 with specified time quantum
void executeRoundRobin(FILE *output_file, ProcessQueue *cpu2_queue, int quantum) {
    int i = 0;
    while (i < cpu2_queue->count) {
        Process *process = &cpu2_queue->processes[i];
        const char *queue_name = (quantum == 8) ? "que2" : "que3";
        fprintf(output_file, "Process %s is placed in the %s queue to be assigned to CPU-2\n", process->name, queue_name);
        fprintf(output_file, "Process %s is assigned to CPU-2.\n", process->name);

        // Execute process for the time quantum or until completion
        if (process->remaining_time > quantum) {
            fprintf(output_file, "Process %s run until the defined quantum time (%d) and is queued again because the process is not completed.\n\n", process->name, quantum);
            process->remaining_time -= quantum;
            enqueue(cpu2_queue, *process);  // Requeue the process if not completed
        } else {
            process->remaining_time = 0;
            fprintf(output_file, "Process %s is assigned to CPU-2, its operation is completed and terminated.\n\n", process->name);
        }
        i++;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s input.txt\n", argv[0]);
        return 1;
    }

    // Initialize queues for CPU-1 and CPU-2
    ProcessQueue cpu1_queue, cpu2_queues[3];
    initializeQueue(&cpu1_queue);
    for (int i = 0; i < 3; i++) {
        initializeQueue(&cpu2_queues[i]);
    }

    // Read processes from the input file
    readProcessesFromFile(argv[1], &cpu1_queue, cpu2_queues);

    // Open the output file
    FILE *output_file = fopen("output.txt", "w");
    if (output_file == NULL) {
        printf("Error: Could not create output file.\n");
        return 1;
    }

    // Execute processes using different scheduling algorithms
    executeFCFS(output_file, &cpu1_queue);
    executeSJF(output_file, &cpu2_queues[0]);
    executeRoundRobin(output_file, &cpu2_queues[1], 8);
    executeRoundRobin(output_file, &cpu2_queues[2], 16);

    // Close the output file
    fclose(output_file);

    // Print the queues to the standard output
    printQueue(stdout, &cpu1_queue, 1, 0, "FCFS");
    printQueue(stdout, &cpu2_queues[0], 2, 1, "SJF");
    printQueue(stdout, &cpu2_queues[1], 2, 2, "RR8");
    printQueue(stdout, &cpu2_queues[2], 2, 3, "RR16");

    return 0;
}
