#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define Nseat 250
#define Ntel 8
#define Nseatlow 1
#define Nseathigh 5
#define Tseatlow 5
#define Tseathigh 10
#define Pcardsuccess 9
#define Cseat 20
#define BILLION 1000000000L

int balance;
int cost;
int waiting_time;

//mutexes
pthread_mutex_t tel_mutex;
pthread_mutex_t bank_mutex;
pthread_mutex_t seats_mutex;
pthread_mutex_t screen_mutex;
pthread_mutex_t time_mutex;

//time
struct timespec start, stop;

double total_waiting_time;
double total_service_time;

//return seats table
int Return[5];

unsigned int seedp;
int tid;

//theater plan
int Seats[Nseat];