#include "scg.h"
// Synchronized Congruential Generator
// A putatively safely splittable PRNG based
// on multiplicative linear congruential generators and
// L'Eucyer (1998)

uint64_t skip_table[64] = {
    0x6a5d39eae116586d,  0xb9547aa6eb471e69, 
    0x785b29c2a7dac711,  0x5f7e5273d1bf6f21, 
    0xa3851e40e57ba241,  0xe318c87d9b4c5481, 
    0xe8f99b107e7ce901,  0x6435886f890ad201, 
    0x5c49c2182659a401,  0x53b10043bdc34801, 
    0x18439dce3fc69001,  0xd09a507f908d2001, 
    0x12d972cb651a4001,  0x86421ec7da348001, 
    0x5220b253f4690001,  0xdafb7b8e8d20001, 
    0xd8fcbbb5d1a40001,  0x678ea87ba3480001, 
    0x9e72153746900001,  0x42373b6e8d200001, 
    0xd9babadd1a400001,  0x8a685ba34800001, 
    0x66114b7469000001,  0x1f3396e8d2000001, 
    0x8aab2dd1a4000001,  0x46665ba348000001, 
    0x510cb74690000001,  0xb3196e8d20000001, 
    0xaa32dd1a40000001,  0x6465ba3480000001, 
    0x8cb746900000001,   0x1196e8d200000001, 
    0x232dd1a400000001,  0x465ba34800000001, 
    0x8cb7469000000001,  0x196e8d2000000001, 
    0x32dd1a4000000001,  0x65ba348000000001, 
    0xcb74690000000001,  0x96e8d20000000001, 
    0x2dd1a40000000001,  0x5ba3480000000001, 
    0xb746900000000001,  0x6e8d200000000001, 
    0xdd1a400000000001,  0xba34800000000001, 
    0x7469000000000001,  0xe8d2000000000001, 
    0xd1a4000000000001,  0xa348000000000001, 
    0x4690000000000001,  0x8d20000000000001, 
    0x1a40000000000001,  0x3480000000000001, 
    0x6900000000000001,  0xd200000000000001, 
    0xa400000000000001,  0x4800000000000001, 
    0x9000000000000001,  0x2000000000000001, 
    0x4000000000000001,  0x8000000000000001, 
    0x1,                 0x1
};

SCG::SCG() {
	_x = 123456789;
	_y = 36243600;
    _offset = 0;
}

SCG::SCG(const SCG* rng) {
    _x = rng->_x;
    _y = rng->_y;
    _offset = rng->_offset;
}

RNG*
SCG::split() {
    skip(5 * _offset + 1);
	SCG* k = new SCG(this);
    k->step();
	return k;
}

void
SCG::seed(uint32_t s)
{
    _x = s;
    _y = 36243600;
    _offset = 0;
}

void
SCG::seed(RNG *rng)
{
    _x = rng->get_uint64();
    _y = rng->get_uint64();
    _offset = 0;
}

uint32_t
SCG::get_uint32()
{
    _x *= 0x6a5d39eae116586d;
    _y *= 0x3b91f78bdac4c89d;
    return ((_x + _y) >> 32);
}

void
SCG::step() {
    _x *= 0x6a5d39eae116586d;
    _offset += 1;
}

void
SCG::skip(uint64_t distance) {
    for(uint64_t ii = 0; ii < 64; ++ii) {
        if(distance & (0x1ULL << ii)) {
            _x *= skip_table[ii];
        }
    }

    _offset += distance;
}
