// OS Scheduling - Ivanna Pena - ip807
// Test several scheduling algorithm
// ./scheduling  inp1.txt 0
// Compile with: gcc–Wall –o scheduling –std=c99 scheduling.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

//defining process to retrieve information from
struct Process{
    int processID;
    int cpuTime;
    int ioTime;
    int arrivalTime;
    int turnaroundTime;
    int CPU;
    int remainingTime;
};

typedef struct Process process;

// 0: First-Come First-Served (nonpreemptive)
// -queue of ready processes
// -newly arrriving processes are added to end of queue
// -when process is blocked, due to I/O, and then becomes ready, it is added to 
//    the end of the queue
// -if two processes happen to be ready at the same time, give preference to one
//    with lower id 

void fcfs(process * processes, char *output_file, int processNum) {

    FILE *output = fopen(output_file, "w");
    //struct queue* ready_queue = init();
    int min = INT_MAX;
    int cycle = 0;
    int ended = 0;
    int idleTime = 0;
    int running = -1;
    int processState[processNum]; // 0 - Ready , 1 - Running, 2 - Blocked , 3 - Ended
    int ready[processNum];
    int blocked[processNum];
    //initializing values to -1
    for (int i = 0; i < processNum; i++){
        processState[i] = -1;
        ready[i] = -1;
        blocked[i] = -1;
    }

    while (ended < processNum){
        //enqueue arrived processes
        for (int i = 0; i < processNum; i++){
            if(processes[i].arrivalTime == cycle){
                ready[i] = cycle;
                processState[i] = 0; //READY
            }
        }
        //if a process is running
        if (running != -1){
            //reduce remaining time
            processes[running].remainingTime--;
            //if after remaining time, it still is not done
            if (processes[running].remainingTime != 0){
                //if remaining time is greater than half the cpu time
                if(processes[running].remainingTime > processes[running].cpuTime * 0.5){
                    //if blocked by IO
                    blocked[running] = cycle + processes[running].CPU - processes[running].cpuTime;
                    processState[running] = 2; // BLOCKED
                    running = -1;
                }
            }else{ //dequeue
                //finished process
                ended++;
                //set turnaround time
                processes[running].turnaroundTime = cycle - processes[running].arrivalTime + 1;
                processState[running] = 3; // ENDED
                running = -1;
            }
        }
        for (int i = 0; i < processNum; i++){
            //if blocked goes out of io
            if (blocked[i] == cycle){
                //unblock
                blocked[i] = -1;
                ready[i] = cycle; //enqueue
                processState[i] = 0; //READY
            }else if (blocked[i] != -1){
                processes[i].remainingTime--;
            }
        }
        //swapping for lower id
        if (running == -1){
            for (int i = 0; i < processNum; i++){
                if (ready[i] > -1){
                    if(ready[i] < min){
                        min = ready[i];
                        running = i;
                    }
                }
            }
            //if a process is ready
            if (running != -1){
                    processState[running] = 1; //RUNNING
                    ready[running] = -1;
            }
        }
        if(ended != processNum){
            if (running == -1){
                //cpu is idle
                idleTime++;
            }
            //Timing snapshot
            fprintf(output, "%d ", cycle);
            for (int i = 0; i < processNum; i++){
                if (processState[i] == 1){ //RUNNING
                    fprintf(output, "%d : running ", i);
                }
                else if (processState[i] == 0){ //READY
                    fprintf(output, "%d : ready ", i);
                }
                else if (processState[i] == 2){ // BLOCKED
                    fprintf(output, "%d : blocked ", i);
                }
            }
            fprintf(output,"\n");
            (cycle)++;
        }
    
    }
    //Statistics
    //last cycle
    fprintf(output, "\nFinishing time : %d\n", cycle - 1);
    fprintf(output, "CPU utilization : %.2f\n", (float)(cycle - idleTime)/cycle);
    for (int i = 0; i < processNum; i++){
        //Turnaround time (i.e. cycle this process finished –cycle it started + 1)          
        fprintf(output, "Turnaround process %d : %d", i, processes[i].turnaroundTime - 1);
        if(i != processNum - 1){
            fprintf(output, "\n");
        }
    }
    fclose(output);
    return;
}

// 1: Round-robin with quantum 2
// -another process scheduled if one of the following occurs
//     - current running process terminates
//     -current running process is blocked on I/O 
//     - current running process ran for 2 cycles
// -you can think of RR as a queue of ready processes. When a process goes from 
//     running to ready, it moves to the back of the queue
// -if two processes become ready at the same time, give preference to the one 
//     with the smaller id 

void rr(process * processes, char *output_file, int processNum) {
    FILE *output = fopen(output_file, "w");
    int min = INT_MAX;
    int cycle = 0;
    int ended = 0;
    int idleTime = 0;
    int running = -1;
    int processState[processNum]; // 0 - Ready , 1 - Running, 2 - Blocked , 3 - Ended
    int ready[processNum];
    int blocked[processNum];

    for (int i = 0; i < processNum; i++){
        processState[i] = -1;
        ready[i] = -1;
        blocked[i] = -1;
    }

    int quantum = 0;
    while (ended < processNum){
        for (int i = 0; i < processNum; i++){
            if(processes[i].arrivalTime == cycle){
                ready[i] = cycle;
                processState[i] = 0; //READY
            }
        }
        //if a process is running
        if (running != -1){
            processes[running].remainingTime--;
            quantum++;
            //if after remaining time, it still is not done
            if (processes[running].remainingTime != 0){                
                //if remaining time is greater than half the cpu time
                if(processes[running].remainingTime > processes[running].cpuTime * 0.5){
                    //if blocked by IO
                    blocked[running] = cycle + processes[running].CPU - processes[running].cpuTime;
                    processState[running] = 2; // BLOCKED
                    running = -1;
                    quantum = 0;
                }else if (quantum == 2) {
                    ready[running] = cycle;
                    processState[running] = 0; //READY
                    running = -1;
                    quantum = 0;
                }
            }else{ 
                //finished process
                ended++;
                //set turnaround time
                processes[running].turnaroundTime = cycle - processes[running].arrivalTime + 1;
                processState[running] = 3; // ENDED
                running = -1;
                //reset quantum
                quantum = 0;
            }
        }
        for (int i = 0; i < processNum; i++){
            //if blocked goes out of io
            if (blocked[i] == cycle){
                blocked[i] = -1;
                ready[i] = cycle;
                processState[i] = 0; //READY
            }
            else if (blocked[i] != -1){
                processes[i].remainingTime--;
            }
        }
        if (running == -1){
            min = INT_MAX;
            for (int i = 0; i < processNum; i++){
                if (ready[i] > -1){
                    if(ready[i] < min){
                        min = ready[i];
                        running = i; 
                    }
                }
            }
            //process is ready
            if (running != -1){
                    processState[running] = 1; //RUNNING
                    ready[running] = -1;
                    quantum = 0;
            }
        }
        if(ended != processNum){
            if (running == -1){
                //cpu is idle
                idleTime++;
            }
            //timing snapshot
            fprintf(output, "%d ", cycle);
            for (int i = 0; i < processNum; i++){
                if (processState[i] == 1){ //RUNNING
                    fprintf(output, "%d : running ", i);
                }
                else if (processState[i] == 0){ //READY
                    fprintf(output, "%d : ready ", i);
                }
                else if (processState[i] == 2){ //BLOCKED
                    fprintf(output, "%d : blocked ", i);
                }
            }
            fprintf(output,"\n");
            (cycle)++;
        }    }
    //Statistics
    //last cycle
    fprintf(output, "\nFinishing time : %d\n", cycle - 1);
    fprintf(output, "CPU utilization : %.2f\n", (float)(cycle - idleTime)/cycle);
    for (int i = 0; i < processNum; i++){
        //Turnaround time (i.e. cycle this process finished –cycle it started + 1)
        fprintf(output, "Turnaround process %d : %d", i, processes[i].turnaroundTime - 1);
        if(i != processNum - 1){
            fprintf(output, "\n");
        }
    }
    fclose(output);
    return;
}


// 2: Shortest remaining job first (preemptive)
// -at each cycle you calculate the remaining CPU time for all ready/running 
//     processes and run the one with the shortest remaining time 
// - if several processes have the same remaining CPU time, give preference to 
//     the process with the lower ID 

void sjf(process * processes, char *output_file, int processNum) {
    FILE *output = fopen(output_file, "w");
    //to help determine the state of each process

    int min = INT_MAX;
    int cycle = 0;
    int ended = 0;
    int idleTime = 0;
    int running = -1;
    int processState[processNum]; // 0 - Ready , 1 - Running, 2 - Blocked , 3 - Ended
    int ready[processNum];
    int blocked[processNum];
    for (int i = 0; i < processNum; i++){
        processState[i] = -1;
        ready[i] = -1;
        blocked[i] = -1;
    }

    int shortestRemaining = -1;
    while (ended < processNum){
        for (int i = 0; i < processNum; i++){
            if(processes[i].arrivalTime == cycle){
                ready[i] = processes[i].remainingTime; 
                processState[i] = 0; //READY
            }
        }
        //if a process is running
        if (running != -1){
            //reduce remaining time
            processes[running].remainingTime--;
            //if after remaining time, still not done
            if (processes[running].remainingTime != 0){
                //if remaining time is greater than half the cpu time
                if(processes[running].remainingTime > processes[running].cpuTime * 0.5){
                    blocked[running] = cycle + processes[running].CPU - processes[running].cpuTime;
                    processState[running] = 2; // BLOCKED
                    running = -1;
                }
            }else{
                //finished process
                ended++;
                //set turnaround time
                processes[running].turnaroundTime = cycle - processes[running].arrivalTime + 1;
                processState[running] = 3; // ENDED
                running = -1;
            }
        }

        for (int i = 0; i < processNum; i++){
            //blocked goes out of io
            if (blocked[i] == cycle){
                blocked[i] = -1;
                ready[i] = processes[i].remainingTime;
                processState[i] = 0; //READY
            }
            else if (blocked[i] != -1){
                processes[i].remainingTime--;
            }
        }
        if (running == -1){
            for (int i = 0; i < processNum; i++){//move process from ready to running
                if (ready[i] > -1){
                    if(ready[i] < min){
                        min = ready[i];
                        running = i;
                    }
                }
            }
            //run min process found
            if (running != -1){
                    processState[running] = 1; //RUNNING
                    ready[running] = -1;
            }
        }
        else{
            shortestRemaining = -1;
            for (int i = 0; i < processNum; i++){
                if (ready[i] > -1){
                    if(ready[i] < min){
                        min = ready[i];
                        shortestRemaining = i;
                    }
                }
            }
            //if process has lower cpu than currently running process
            if (shortestRemaining < running){
                if(min == processes[running].remainingTime){
                    processState[running] = 0; //READY
                    ready[running] = processes[running].remainingTime;
                    processState[shortestRemaining] = 1; //RUNNING
                    ready[shortestRemaining] = -1;
                    running = shortestRemaining;
                }
            }else if (min < processes[running].remainingTime){
                processState[running] = 0; //READY
                ready[running] = processes[running].remainingTime;
                processState[shortestRemaining] = 1; //RUNNING
                ready[shortestRemaining] = -1;
                running = shortestRemaining;
            }
        }
        if(ended != processNum){
            if (running == -1){
                //cpu is idle
                idleTime++;
            }
            //timing snapshot
            fprintf(output, "%d ", cycle);
            for (int i = 0; i < processNum; i++){
                if (processState[i] == 1){ //RUNNING
                    fprintf(output, "%d : running ", i);
                }
                else if (processState[i] == 0){ //READY
                    fprintf(output, "%d : ready ", i);
                }
                else if (processState[i] == 2){ //BLOCKED
                    fprintf(output, "%d : blocked ", i);
                }
            }
            fprintf(output,"\n");
            (cycle)++;
        }    }
    //Statistics
    //last cycle
    fprintf(output, "\nFinishing time: %d\n", cycle - 1);
    fprintf(output, "CPU utilization: %.2f\n", (float)(cycle - idleTime)/cycle);
    for (int i = 0; i < processNum; i++){
        //Turnaround time (i.e. cycle this process finished – cycle it started + 1)
        fprintf(output, "Turnaround process %d: %d", i, processes[i].turnaroundTime - 1);
        if(i != processNum - 1){
            fprintf(output, "\n");
        }
    }
    fclose(output);
    return;
}

int main(int argc, char *argv[]){
    // Input File
    // First line is # of processes
    // A B C D 
    // A - process ID 
    // B - CPU time  
    // C - I/O time  
    // D - Arrival Time  
    int processNum = 0;
    FILE *input_file;
    input_file = fopen(argv[1], "r");
    if (input_file == 0){
        printf("FILE NOT FOUND\n");
        return 1;
    }
    else{
        fscanf(input_file, "%d", &processNum);
    }
    // initializing array of processes
    process * processes = malloc(processNum * sizeof(process));
    for (int i = 0; i < processNum; i++){
        fscanf(input_file, "%d %d %d %d", &processes[i].processID, &processes[i].cpuTime, &processes[i].ioTime, &processes[i].arrivalTime);
        if(processes[i].cpuTime % 2 == 0){
            processes[i].CPU = processes[i].cpuTime + processes[i].ioTime;
            processes[i].remainingTime = processes[i].cpuTime + processes[i].ioTime;
        }else{         // rounding up to next value if process cpu time is odd
            processes[i].cpuTime += 1;
            processes[i].CPU = processes[i].cpuTime + processes[i].ioTime;
            processes[i].remainingTime = processes[i].cpuTime + processes[i].ioTime;
        }

    }
    //Output
    // You output a file with name: inputfilename-s.txt
    //     –inputfilename is the name of the input file without the extension
    //     –s is the scheduling algorithm:0, 1, or 2
    //     –Example: if input file is inp1.txt, your output  file for FCFS shall be:  inp1-0.txt
    //removes the .txt extension from filename
    argv[1][strlen(argv[1])-4] = '\0';
    if (strcmp(argv[2], "0") == 0){
        strcat(argv[1], "-0.txt");
        fcfs(processes, argv[1], processNum);
    } else if (strcmp(argv[2], "1") == 0){
        strcat(argv[1], "-1.txt");
        rr(processes, argv[1], processNum);
    } else if (strcmp(argv[2], "2") == 0){
        strcat(argv[1], "-2.txt");
        sjf(processes, argv[1], processNum);
    }

    fclose(input_file);
    free(processes);
    return 0;
}