#include "rng.h"
#include "rng_internal.h"

template <class S, class T>
union StoT {
    S a;
    T b;
};

uint32_t 
RNG::get_uint32()
{
        return (get_uint64() & 0xFFFFFFFF);
}

uint64_t 
RNG::get_uint64()
{
        return (((uint64_t) get_uint32() << 32) | get_uint32());
}

void
RNG::seed_with(uint32_t s)
{
        if(!s) {
                s = time(NULL);
        }
        LCG g(s);
        this->seed(&g);
}

/* Produces a pseudo-random int on the interval [0,2^32) */
int
RNG::get_int()
{
        return (get_uint32() & 0x7FFFFFFF);
}

/* Produces a pseudo-random integer on the interval [0,n)
 * uses rejection sampling.
 * Given a well behaved generator, in the worst case this will require
 * two invocations of get_uint32().
 * Not guaranteed to halt given a sufficiently pathological generator.
 */
int
RNG::get_int(int n)
{
        int v = n;
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        
        int result;
        do {
                result = get_uint32() & v;
        } while(result > n);

        return (result);
}

float
RNG::get_float()
{
	StoT<int,float> res;
    res.a = 0x3F800000 | (get_uint32() & 0x3FFFFF);
    return (res.b - 1.0);
}

double
RNG::get_double()
{
    StoT<int,double> res;
    res.a = 0x3ff0000000000000ULL | (get_uint64() & (0xFFFFFFFFFFFFFULL));
    return (res.b - 1.0);
}

double
RNG::get_normal(double mu, double sigma)
{
        double x, y, r;

        do {
                x = 2.0 * get_double();
                y = 2.0 * get_double();
                r = x * x + y * y;
        }while(r == 0.0 || r >= 1.0);

        r = sqrt((-2.0 * log(r)) / r);

        return (x * r * sigma + mu);
}

int
RNG::get_geometric() 
{
        // The probability of looping is 1 in 2^32, it's reasonable to
        // truncate the output from a 64-bit generator to avoid unnecessary
        // calls by a 32-bit generator.
        uint32_t population;
        int result = 0;
        do {
                population = get_uint32();        
                result += (population ? __builtin_ctz(population) : 32);
        } while(!population);

        return result;
}

bool
RNG::get_bool()
{
	return ((bool) get_uint32());
}

uint64_t
RNG::get_nonzero_bits(short n)
{
	uint64_t result = 0;
	while(!result) 
	{
		result = get_bits(n);
	}
	return (result);
}

uint64_t
RNG::get_bits(short n)
{
        if(n <= 32) {
                return (get_uint32() & (1LL << n) - 1);
        } else {
                return (get_uint64() & (1LL << n) - 1);
        }
}

/* Linear Congruential Generator for seeding other generators provided a 32-bit
 * seed. A rather poor choice of generator, this isn't exposed outside of this
 * class. Uses the same parameterization as glibc.
 */
LCG::LCG()
{
        _state = 214013;
}

LCG::LCG(uint32_t seed)
{
        _state = seed;
        get_uint32();
}

uint32_t
LCG::get_uint32() {
        _state = 1103515245 * _state + 12345;
        return _state;
}

uint64_t
LCG::get_uint64() {
        return ((uint64_t) get_uint32() << 32 | get_uint32());
}

RNG*
LCG::split()
{
        RNG* gen = new LCG();
        gen->seed(this);
        return gen;
}

void
LCG::seed(RNG* gen)
{
        // This offers no robustness guarantees.
        _state = get_uint32() ^ 0xFFFFFFFF * 1664525 + 1013904223;
}
