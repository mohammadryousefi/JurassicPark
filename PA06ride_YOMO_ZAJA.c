#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>

#define NO_ARG 101
#define MAXWAITPEOPLE 800

const int max_int = 0x7FFFFFFF;

int timeCounter = 0; //used to simulate a timer
int waiting = 0; //number of people waiting in line
int meanArrival = 0; //mean number of people arriving, will change when needed
int carNum;//number of cars
int maxPerCar;//number opf max people per car

//for end of day display
int totalArrived = 0;
int totalRide = 0;
int totalLeft = 0;//people that left due to the line being too long
int avgWait = 0;//average waiting time per person(in minutes)
int longestLine = 0;//length of line at worst case(when the line was longest)
int longestTime = 0;//time that line was at its worst

int pseudo = 1;
void Random();
double random_real();
int random_integer(int low, int high);
int poisson(double mean);

int reseed();
int seed;
int multiplier;
int add_on;


int reseed()
{
  seed = seed * multiplier + add_on;
  return seed;
}

void Random()
{
  if (pseudo)
    seed = 1;
  else
  {
    srandom(getpid());
    seed = random();
  }
  multiplier = 2743;
  add_on = 5923;
}

double random_real()
{
  double max = max_int+1.0;
  double temp = reseed();
  if (temp < 0) temp = temp + max;
  return temp/max;
}

int random_integer(int low, int high)
{
  if (low > high) 
    return random_integer(high,low);
  else
    return ((int)((high-low+1)*random_real())) + low;
}

int poisson(double mean)
{
  double limit = exp(-mean);
  double product = random_real();
  int    count = 0;
  while (product > limit)
  {
    count++;
    product *= random_real();
  }
  return count;
}

void uMessage(const char * pName)
{
        fprintf(stderr, "Usage: %s -M n -N n\n", pName);
        exit(NO_ARG); 
}

//do thread stuff here
void* startLine()
{
  
}

//used by the reporter thread and to increment our time and call poisson
void* reporterThread()
{
  //after every second
  waiting += poisson(meanArrival);
  
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
