#include "stdlib.h"
#include "unistd.h"
#include "stdio.h"
#include "string.h"
#include "time.h"
#include "cmath"


class Process
{
    char* id;
    const char* type;
    int arrival;
    int bursts;

    public:

    Process(char* i, const char* t, double l, int b) {
        id = i;
        type = t;
        arrival = floor(next_exp(l));
        while (arrival > b) {
            arrival = floor(next_exp(l));
        }
        double rand = drand48();
        bursts = (int) ceil(rand * 32);
    }
    char* getId() const {return id;}
    const char* getType() const {return type;}
    int getArrival() const {return arrival;}
    int getBursts() const {return bursts;}

    double next_exp(double l) {
        double rand = drand48();
        return -(log(rand)) / l;
    }


    void runBursts(double l) {
        for (int i = 0; i < getBursts(); i++) {
            int cpu_time = ceil(next_exp(l));
            int io_time = ceil(next_exp(l));

            if (strcmp(getType(), "CPU")){
                cpu_time *= 4;
            } else {
                io_time *= 8;
            }
            if (i != getBursts() - 1) {
                printf("===> CPU burst %dms ==> I/O burst %dms\n", cpu_time, io_time);
            } else {
                printf("===> CPU burst %dms\n", cpu_time);
            }
        }
    }


};

int main(int argc, char** args)
{
    if (argc != 6) {
        fprintf(stderr, "invalid args\n");
        return EXIT_FAILURE;
    }

    int n = atoi(args[1]);
    int n_cpu = atoi(args[2]);
    int seed = atoi(args[3]);
    double lambda = atof(args[4]);
    int bound = atoi(args[5]);

    srand48(seed);

    char t1 = 'A';
    int t2 = 0;

    int cpu_count = 0;
    for (int i = 0; i < n; i++) {
        
        char* temp = (char*) calloc(2, sizeof(char));
        if (t2 == 10) {
            t2 = 0;
            t1++;
        }
        sprintf(temp, "%c%d", t1, t2);
        t2++;

        if (cpu_count < n_cpu) {
            const char* typ = "CPU";
            Process p = Process(temp, typ, lambda, bound);
            printf("%s-bound process %s: arrival time %dms; %d CPU bursts:\n", p.getType(), p.getId(), p.getArrival(), p.getBursts());
            p.runBursts(lambda);
        } else {
            const char* typ = "I/O";
            Process p = Process(temp, typ, lambda, bound);
            printf("%s-bound process %s: arrival time %dms; %d CPU bursts:\n", p.getType(), p.getId(), p.getArrival(), p.getBursts());
            p.runBursts(lambda);
        }

        cpu_count++;
        free(temp);
    }

    return EXIT_SUCCESS;
}