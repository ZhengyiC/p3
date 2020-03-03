#include "rand.h"

struct xorshift32_state {
  unsigned int a;
} state = {1};

int xv6_rand (void)
{
  /* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
  unsigned int x = state.a;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  return state.a = x % ((unsigned int)XV6_RAND_MAX+1);
}


void xv6_srand (unsigned int seed)
{
  state.a = seed;
}
