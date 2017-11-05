#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <math.h>
#include <pthread.h>
#include "random437.h"
#define NO_ARG 101
#define MAXWAITPEOPLE 800
#define MORNING 7199
#define NOON 17999
#define AFTERNOON 25200

pthread_mutex_t wait_mut; // For loading purposes.
pthread_mutex_t timer_mut;
pthread_mutex_t barrier_mut;
pthread_cond_t barrier_cond;
pthread_cond_t timer_cond;

long simTime, lastDeparture, longestLineTime;
int numCars, maxPerCar;
int threadCount, meanArrival, currentlyWaiting, totalArrivals, totalAccepted, totalRejected, averageWait, longestLineLength;

void uMessage(const char * pName)
{
        fprintf(stderr, "Usage: %s -M n -N n\n", pName);
        exit(NO_ARG); 
}

//do thread stuff here
void* workerThread()
{
  /* Data used for internal bookkeeping of thread. */

  pthread_mutex_lock(&barrier_mut);
  pthread_mutex_unlock(&barrier_mut);
  printf("Worker Started\n");
  int timeOfReturn = 0;
  int outOnRide = 0;
  while (1)
  {
    printf("Worker working\n");
    pthread_mutex_lock(&timer_mut);
    if (simTime >= 36000) // SIMULATION FINISHED
    {
      pthread_mutex_unlock(&timer_mut);
      pthread_mutex_lock(&barrier_mut);
      numCars--;
      if (numCars < 1) pthread_cond_signal(&timer_cond);
      pthread_mutex_unlock(&barrier_mut);
      return 0;
    }
    if (simTime == timeOfReturn) // Return from ride
    {
      outOnRide = 0;
    }

    /*****************************************************************/
    /*  L:oading passengers.                                         */
    /*  No other car can load passengers until 7 seconds has passed. */
    /*****************************************************************/
    if (simTime >= lastDeparture + 7 && !outOnRide)
    {
      pthread_mutex_lock(&wait_mut);
      lastDeparture = simTime;
      timeOfReturn = lastDeparture + 60;
      outOnRide = 1;
      int waitingBeforeRide = currentlyWaiting;
      currentlyWaiting -= maxPerCar;
      if (currentlyWaiting < 0) currentlyWaiting = 0;
      totalAccepted += waitingBeforeRide - currentlyWaiting;
      pthread_mutex_unlock(&wait_mut);
    }
    pthread_mutex_unlock(&timer_mut);

    /* Begin Barrier */ 
    pthread_mutex_lock(&barrier_mut);
    threadCount++;
    if (threadCount == numCars)
    {
      printf("BARRIER HIT\n");
      threadCount = 0;
      pthread_cond_signal(&timer_cond);
      printf("%d sleeping.\n", threadCount);
      pthread_cond_wait(&barrier_cond, &barrier_mut);
    }
    else
    {
      printf("%d sleeping, ", threadCount);
      pthread_cond_wait(&barrier_cond, &barrier_mut);
    }
    pthread_mutex_unlock(&barrier_mut);
  }
}

//used by the reporter thread and to increment our time and call poisson
void* reporterThread()
{
  printf("Timer Started\n");
  while(1)
  {
    pthread_mutex_lock(&timer_mut);
    simTime++;
    if (simTime >= 36000) // SIMULATION FINISHED
    {
      pthread_mutex_unlock(&timer_mut);
      return 0;
    }
    if(simTime % 60 == 0)
    {
      if (simTime > AFTERNOON) meanArrival = 25;
      else if (simTime > NOON) meanArrival = 35;
      else if (simTime > MORNING) meanArrival = 45;
      int rejected = 0;
      int arrival = poisson(meanArrival);
      pthread_mutex_lock(&wait_mut);
      totalArrivals += arrival;
      currentlyWaiting += arrival;
      if(currentlyWaiting > MAXWAITPEOPLE)
      {
        rejected = currentlyWaiting - MAXWAITPEOPLE;
        totalRejected += rejected;
        currentlyWaiting = MAXWAITPEOPLE;
      }
      printf("%03ld arrive %d reject %d wait-line %d at %02ld:%02ld:%02ld\n", simTime / 60, arrival, rejected, currentlyWaiting, 9 + (simTime / 3600), (simTime % 3600) / 60, simTime % 60);
      if (currentlyWaiting > longestLineLength)
      {
        longestLineLength = currentlyWaiting;
        longestLineTime = simTime;
      }
      pthread_mutex_unlock(&wait_mut);
    }
    pthread_mutex_unlock(&timer_mut);
    printf("TIMER DONE!\n");
    pthread_mutex_lock(&barrier_mut);
    printf("Signaling Workers\n");
    pthread_cond_broadcast(&barrier_cond);
    pthread_cond_wait(&timer_cond, &barrier_mut);
  }
}

void initLocks(void)
{
  pthread_mutex_init(&wait_mut, NULL);
  pthread_mutex_init(&timer_mut, NULL);
  pthread_mutex_init(&barrier_mut, NULL);
  pthread_cond_init(&barrier_cond, NULL);
  pthread_cond_init(&timer_cond, NULL);
}

void destroyLocks(void)
{
  pthread_mutex_destroy(&wait_mut);
  pthread_mutex_destroy(&timer_mut);
  pthread_mutex_destroy(&barrier_mut);
  pthread_cond_destroy(&barrier_cond);
  pthread_cond_destroy(&timer_cond);
}

void initParameters(int argc, char* argv[])
{
  int opt;
  while ((opt = getopt(argc, argv, "N:M:")) != -1)
  {
    switch(opt)
    {
      case 'N':
        numCars = atoi(optarg);
        break;
      case 'M':
        maxPerCar = atoi(optarg);
        break;
      default:
        uMessage(argv[0]);
    }
  }
  if (numCars < 1 || maxPerCar < 1) uMessage(argv[0]);
}

void init(int argc, char* argv[])
{
  initParameters(argc, argv);
  initLocks();  
  simTime = 0;
  meanArrival = 25;
  currentlyWaiting = poisson(meanArrival);
  lastDeparture = LONG_MIN;
  longestLineLength = currentlyWaiting;
  longestLineTime = simTime;
  totalArrivals = currentlyWaiting;
  totalAccepted = 0;
  totalRejected = 0;
  averageWait = 0;
  threadCount = 0;
  printf("%03ld arrive %d reject %d wait-line %d at %02ld:%02ld:%02ld\n", simTime / 60, currentlyWaiting, totalRejected, currentlyWaiting, 9 + (simTime / 3600), (simTime % 3600) / 60, simTime % 60);
  }
int main(int argc, char* argv[])
{
  int i;
  void* voidptr=NULL;
  
  init(argc, argv);
  
  pthread_t tid[numCars+1];//need a thread for every car, and one more to report 
  srand(getpid());

  pthread_mutex_lock(&barrier_mut);
  for (i = 0; i < numCars; i++)
    if (pthread_create(&tid[i], NULL, workerThread, NULL))
    { 
      perror("Error in thread creating\n"); return(1);
    }
  pthread_cond_wait(&timer_cond, &barrier_mut);
  printf("Creating Timer\n"); 
  //get the reporter thread on the reporterThread method
  if (pthread_create(&tid[numCars], NULL, reporterThread, NULL))
  { 
    perror("Error in thread creating\n"); return(1);
  }

  //join all threads, including the reporter
  for (i = 0; i < numCars + 1; i++)
    if (pthread_join(tid[i], (void*)&voidptr))
    { 
      perror("Error in thread joining\n"); return(1);
    }
  
  destroyLocks();
  return 0;
}
