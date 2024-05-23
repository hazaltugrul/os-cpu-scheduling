#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 100
#define HIGHEST_PRIORITY 0
#define HIGH_PRIORITY 1
#define MEDIUM_PRIORITY 2
#define LOW_PRIORITY 3

#define TOTAL_RAM_SIZE 2048
#define CPU1_RAM_SIZE 512

typedef struct {
    char name[10];
    int arrival_time;
    int priority;
    int burst_time;
    int ram_required;
    int cpu_usage;
    int remaining_time;
} Process;

typedef struct {
    Process processes[MAX_PROCESSES];
    int count;
} ProcessQueue;

void printSJF(FILE *output_file, ProcessQueue *queue) {
    for (int i = 0; i < queue->count; i++) {
        fprintf(output_file, "Process %s is placed in the SJF queue to be assigned to CPU-2\n", queue->processes[i].name);
        fprintf(output_file, "Process %s is assigned to CPU-2.\n", queue->processes[i].name);
        fprintf(output_file, "Process %s is completed and terminated.\n\n", queue->processes[i].name);
    }
}

void executeSJF(FILE *output_file, ProcessQueue *destination, ProcessQueue *queue) {
    for (int i = 0; i < queue->count; i++) {
        temp[i] = queue->processes[i];
    }
    for (int i = 0; i < queue->count - 1; i++) {
        for (int j = i + 1; j < queue->count; j++) {
            if (temp[j].arrival_time <= temp[i].arrival_time && temp[j].burst_time < temp[i].burst_time) {
                Process swap = temp[i];
                temp[i] = temp[j];
                temp[j] = swap;
            }
        }
    }
    for (int i = 0; i < queue->count; i++) {
        enqueue(destination, temp[i]);
        elapsed_time += temp[i].burst_time;
    }

    printSJF(output_file, destination);
}

void ExecuteFCFS(ProcessQueue *cpu1_queue, FILE *output_file) {
    for (int i = 0; i < cpu1_queue->count; i++) {
        if (cpu1_queue->processes[i].ram_required <= CPU1_RAM_SIZE) {
            fprintf(output_file, "Process %s is queued to be assigned to CPU-1.\n", cpu1_queue->processes[i].name);
            fprintf(output_file, "Process %s is assigned to CPU-1.\n", cpu1_queue->processes[i].name);
            fprintf(output_file, "Process %s is completed and terminated.\n\n", cpu1_queue->processes[i].name);
 }
}

void removeFromQueue(ProcessQueue *queue, char *name) {
    int found = 0;
    for (int i = 0; i < queue->count; i++) {
        if (strcmp(queue->processes[i].name, name) == 0) {
            found = 1;
            for (int j = i; j < queue->count - 1; j++) {
                queue->processes[j] = queue->processes[j + 1];
            }
            queue->count--;
            break; 
        }
    }
    if (!found) {
        printf("Error: Process '%s' not found in the queue.\n",name);
}
}

void initializeQueue(ProcessQueue *queue) {
    queue->count = 0;
}

Process dequeue(ProcessQueue *queue) {
    Process process = queue->processes[0];
    for (int i = 0; i < queue->count - 1; i++) {
        queue->processes[i] = queue->processes[i + 1];
    }
    queue->count--;
    return process;
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s input.txt\n", argv[0]);
        return 1;
    }

    FILE *input_file = fopen(argv[1], "r");
    if (input_file == NULL) {
        printf("Error: Could not open input file.\n");
        return 1;
    }

    ProcessQueue cpu1_queue, cpu2_queues[3], sorted_sjf;
    initializeQueue(&cpu1_queue);
    initializeQueue(&sorted_sjf);
    for (int i = 0; i < 3; i++) {
        initializeQueue(&cpu2_queues[i]);
    }

    char line[100];
    while (fgets(line, sizeof(line), input_file)) {
        Process process;
        sscanf(line, "%[^,],%d,%d,%d,%d,%d", process.name, &process.arrival_time, &process.priority,
               &process.burst_time, &process.ram_required, &process.cpu_usage);

        if (process.priority == HIGHEST_PRIORITY) {
            if (process.ram_required > CPU1_RAM_SIZE) {
                printf("Process %s cannot be allocated due to insufficient RAM.\n", process.name);
                continue;
            }
            enqueue(&cpu1_queue, process);
        } else if (process.priority >= HIGH_PRIORITY && process.priority <= LOW_PRIORITY) {
            enqueue(&cpu2_queues[process.priority - 1], process);
        } else {
            printf("Invalid priority for process %s. Skipping.\n", process.name);
        }
    }

    fclose(input_file);
	
	ExecuteFCFS(&cpu1_queue, output_file); 
	
	printf("CPU-1 que1(priority-0) (FCFS)â€º ");
    for (int i = 0; i < cpu1_queue.count; i++) {
        printf("%s", cpu1_queue.processes[i].name);
        if (i < cpu1_queue.count - 1) {
            printf("->");
        }
        if (i == cpu1_queue.count - 1) {
            printf("\n");
	}
	
    return 0;
}

