#include <unistd.h>
#include <stdlib.h>
#include <math.h>

const int max_int = 0x7FFFFFFF;

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

void main(void)
{
}