#include "stdlib.h"
#include "unistd.h"
#include "stdio.h"
#include "string.h"
#include "iostream"
#include "fstream"
#include "time.h"
#include "cmath"

// global variables for simout stats

int cpu_cpu = 0;
int cc_count = 0;
int cpu_io = 0;
int ci_count = 0;
int io_cpu = 0;
int ic_count = 0;
int io_io = 0;
int ii_count = 0;

// process class for main CPU and IO processes
class Process
{
    char* id;
    const char* type;
    int arrival;
    int bursts;

    public:

    // constructor
    Process(char* i, const char* t, double l, int b) {
        id = i;
        type = t;
        arrival = floor(next_exp(l));
        while (arrival > b) {
            arrival = floor(next_exp(l));
        }
        bursts = (int) ceil(drand48() * 32);
    }

    // getters
    char* getId() const {return id;}
    const char* getType() const {return type;}
    int getArrival() const {return arrival;}
    int getBursts() const {return bursts;}

    // next_exp
    double next_exp(double l) {
        double rand = drand48();
        return -(log(rand)) / l;
    }

    // run CPU and IO bursts for process
    void runBursts(double l, int b) {
        for (int i = 0; i < getBursts(); i++) {
            // proper cpu time < bound
            int cpu_time = ceil(next_exp(l));
            while (cpu_time > b){
                cpu_time = ceil(next_exp(l));
            }

            // proper io time < bound
            int io_time = 0;
            if (i != getBursts() - 1){
                io_time = ceil(next_exp(l));       
                while (io_time > b){
                    io_time = ceil(next_exp(l));
                }
            }

            // IO_time * 8 or CPU_time * 4,
            // checking for last exception
            // and updating stats
            if (strcmp(getType(), "CPU")){
                io_time *= 8;
                io_cpu += cpu_time;
                ic_count++;
                io_io += io_time;
                if (i != getBursts() - 1){
                    ii_count++;
                }
            } else {
                cpu_time *= 4;
                cpu_cpu += cpu_time;
                cc_count++;
                cpu_io += io_time;
                if (i != getBursts() - 1){
                    ci_count++;
                }
            }
            // output burst data
            if (i != getBursts() - 1) {
                printf("==> CPU burst %dms ==> I/O burst %dms\n", cpu_time, io_time);
            } else {
                printf("==> CPU burst %dms\n", cpu_time);
            }
        }   
    }
};

int main(int argc, char** args)
{
    // error checking
    if (argc != 6) {
        fprintf(stderr, "ERROR: Invalid args\n");
        return EXIT_FAILURE;
    }

    // get and print model params
    int n = atoi(args[1]);
    int n_cpu = atoi(args[2]);
    int seed = atoi(args[3]);
    double lambda = atof(args[4]);
    int bound = atoi(args[5]);
    if (n_cpu != 1){
        printf("<<< PROJECT PART I\n<<< -- process set (n=%d) with %d CPU-bound processes\n<<< -- seed=%d; lambda=%.6f; bound=%d\n", n, n_cpu, seed, lambda, bound);
    } else {
        printf("<<< PROJECT PART I\n<<< -- process set (n=%d) with %d CPU-bound process\n<<< -- seed=%d; lambda=%.6f; bound=%d\n", n, n_cpu, seed, lambda, bound);
    }

    srand48(seed);

    // main program, creates ids, processes, and runs bursts
    // based on params given
    char t1 = 'A';
    int t2 = 0;
    int cpu_count = 0;
    for (int i = 0; i < n; i++) {
        // makes id e.g. "A0"
        char* temp = (char*) calloc(3, sizeof(char));
        if (t2 == 10) {
            t2 = 0;
            t1++;
        }
        sprintf(temp, "%c%d", t1, t2);
        t2++;

        // create process and output information
        if (cpu_count < n_cpu) {
            const char* typ = "CPU";
            Process p = Process(temp, typ, lambda, bound);
            if (p.getBursts() > 1){
                printf("%s-bound process %s: arrival time %dms; %d CPU bursts:\n", p.getType(), p.getId(), p.getArrival(), p.getBursts());
            } else {
                printf("%s-bound process %s: arrival time %dms; %d CPU burst:\n", p.getType(), p.getId(), p.getArrival(), p.getBursts());   
            }
            p.runBursts(lambda, bound);
        } else {
            const char* typ = "I/O";
            Process p = Process(temp, typ, lambda, bound);
            printf("%s-bound process %s: arrival time %dms; %d CPU bursts:\n", p.getType(), p.getId(), p.getArrival(), p.getBursts());
            p.runBursts(lambda, bound);
        }

        cpu_count++;
        free(temp);
    }

    // Output simulator statistics to output simout.txt file
    FILE* out = fopen("simout.txt", "w");
    if (out == NULL){
        fprintf(stderr, "ERROR: file open\n");
        return EXIT_FAILURE;
    }
    fprintf(out, "-- number of processes: %d\n-- number of CPU-bound processes: %d\n-- number of I/O-bound processes: %d\n", n, n_cpu, n - n_cpu);
    float cc = cc_count?(ceil((cpu_cpu * 1000.0 / cc_count)) / 1000.0):0.f;
    float ic = ic_count?(ceil((io_cpu * 1000.0 / ic_count)) / 1000.0):0.f;
    float oc = (cc_count + ic_count)?(ceil(((cpu_cpu + io_cpu) * 1000.0 / (cc_count + ic_count))) / 1000.0):0.f;
    float ci = ci_count?(ceil((cpu_io * 1000.0 / ci_count)) / 1000.0):0.f;
    float ii = ii_count?(ceil((io_io  * 1000.0 / ii_count)) / 1000.0):0.f;
    float oi = (ci_count + ii_count)?(ceil(((cpu_io + io_io) * 1000.0 / (ci_count + ii_count))) / 1000.0):0.f;
    fprintf(out, "-- CPU-bound average CPU burst time: %.3f ms\n-- I/O-bound average CPU burst time: %.3f ms\n-- overall average CPU burst time: %.3f ms\n", cc, ic, oc);
    fprintf(out, "-- CPU-bound average I/O burst time: %.3f ms\n-- I/O-bound average I/O burst time: %.3f ms\n-- overall average I/O burst time: %.3f ms\n", ci, ii, oi);
    fclose(out);

    return EXIT_SUCCESS;
}