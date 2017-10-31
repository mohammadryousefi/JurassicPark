#ifndef random437_h
#define random437_h

#include <unistd.h>
#include <stdlib.h>
#include <math.h>

const int max_int = 0x7FFFFFFF;

class Random {
  public: 
    Random(bool pseudo = true);
    double random_real();
    int random_integer(int, int);
    int poisson(double);
  private:
    int reseed();
    int seed;
    int multiplier;
    int add_on;
};

int Random::reseed() {  
    seed = seed * multiplier + add_on;
    return seed;
}

Random::Random(bool pseudo) {
    if (pseudo)
     	seed = 1;
    else {
	srandom(getpid());
	seed = random();
	}
    multiplier = 2743;
    add_on = 5923;
}

double Random::random_real() {
    double max = max_int+1.0;
    double temp = reseed();
    if (temp < 0) temp = temp + max;
    return temp/max;
}

int Random::random_integer(int low, int high) {
    if (low > high) 
       return random_integer(high,low);
    else
       return ((int)((high-low+1)*random_real())) + low;
}

int Random::poisson(double mean) {
    double limit = exp(-mean);
    double product = random_real();
    int    count = 0;
    while (product > limit) {
	count++;
	product *= random_real();
	}
    return count;
}

#endif /* random437_h */
