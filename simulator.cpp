#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "cmath"
#include "time.h"


class Process
{
    char* id;
    char* type;
    int arrival;
    int bursts;

    public:

    Process(char* i, char* t, double l) {
        double rand = drand48();
        id = i;
        type = t;
        arrival = -(log(rand)) / l;
        bursts = (int) ceil(rand * 32);
    }
    char* getId() const {return id;}
    char* getType() const {return type;}
    int getArrival() const {return arrival;}
    int getBursts() const {return bursts;}
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
    for (int i = 0; i < n; i++) {
        char* temp;
        if (t2 == 10) {
            t2 = 0;
            t1++;
        }
        sprintf(temp, "%c%d", t1, t2);
        t2++;
        Process p = Process(temp, "CPU", lambda);
        printf("%s-bound process %s: arrival time %dms; %d CPU bursts:\n", p.getType(), p.getId(), p.getArrival(), p.getBursts());
    }

    return EXIT_SUCCESS;
}