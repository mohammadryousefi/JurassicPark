#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include "random437.h"
#define NO_ARG 101
#define MAXWAITPEOPLE 800
#define MORNING 7199
#define NOON 17999
#define AFTERNOON 25200

const int max_int = 0x7FFFFFFF;

long time = 0; //used to simulate a timer
int waiting = 0; //number of people waiting in line
int meanArrival = 25; //mean number of people arriving, will change when needed
int carNum;//number of cars
int maxPerCar;//number of max people per car

pthread_mutex_t waitingAreaLock;
//for end of day display
int totalArrived = 0;
int totalRide = 0;
int totalLeft = 0;//people that left due to the line being too long
int avgWait = 0;//average waiting time per person(in minutes)
int lll = 0; //Length of Longest Line
int toll = 0; //Time of Occurence of Longest Line

void uMessage(const char * pName)
{
        fprintf(stderr, "Usage: %s -M n -N n\n", pName);
        exit(NO_ARG); 
}

//do thread stuff here
void* startLine()
{
  int before;
  pthread_lock(&waitingAreaLock);
  time += 7;
  before = waiting;
  waiting-= maxPerCar;
  if (waiting < 0) waiting = 0;
  totalRide += before - waiting;
  pthread_unlock(&waitingAreaLock);
}

//used by the reporter thread and to increment our time and call poisson
void* reporterThread()
{
  //after every second
  int arrival;
  pthread_lock(&waitingAreaLock);
  if (time > AFTERNOON) meanArrival = 25;
  else if (time > NOON) meanArrival = 35;
  else if (time > MORNING) meanArrival = 45;
  arrival = poisson(meanArrival);
  totalArrived += arrival;
  waiting += arrival;
  if(waiting > MAXWAITPEOPLE)
  {
    totalLeft += waiting - MAXWAITPEOPLE;
    waiting = MAXWAITPEOPLE;
  }
  if (waiting > lll)
  {
    lll = waiting;
    toll = time;
  }
  pthread_ulock(&waitingAreaLock);
}

int main(int argc, char* argv[])
{
  int i;
  int opt;
  
  while ((opt = getopt(argc, argv, "N:M:")) != -1)
  {
    switch(opt)
    {
      case 'N':
        carNum = atoi(optarg);
        break;
      case 'M':
        maxPerCar = atoi(optarg);
        break;
      default:
        uMessage(argv[0]);
    }
  }
  
  void* voidptr=NULL;
  pthread_t tid[carNum+1];//need a thread for every car, and one more to report 
  srand(getpid());
  
  for (i = 0; i < carNum; i++)
    if (pthread_create(&tid[i],NULL,startLine, NULL))
    { 
      perror("Error in thread creating\n"); return(1);
    }

  //get the reporter thread on the reporterThread method
  if (pthread_create(&tid[carNum],NULL,reporterThread, NULL))
  { 
    perror("Error in thread creating\n"); return(1);
  }
  
  for (i = 0; i < carNum; i++)
    if (pthread_join(tid[i], (void*)&voidptr))
    { 
      perror("Error in thread joining\n"); return(1);
    }

  //join the report thread
  if (pthread_join(tid[carNum], (void*)&voidptr))
  { 
    perror("Error in thread joining\n"); return(1);
  }

  return 0;
}
