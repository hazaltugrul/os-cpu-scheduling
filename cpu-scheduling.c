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
    return 0;
}

