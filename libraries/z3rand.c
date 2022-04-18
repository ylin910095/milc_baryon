/*********** z3rand.c (in su3.a) ****************************************
*									*
*  int z3_ran_no( double_prn *prn_pt )   				*
*  Z(3) distributed random number 				*
*  This requires a random number generator named "myrand()", returning	*
*  a Real uniformly distributed between zero and one. The argument of	*
*  this routine is a pointer to be passed to myrand(). 			*
*/

#include "../include/config.h"
#include <math.h>
#include "../include/su3.h"
#include "../include/random.h"

complex z3_rand_no( double_prn *prn_pt ){

  complex result;
  double rndno = 3.*myrand(prn_pt);
  if( rndno > 2. ) { result.real = 1.; result.imag = 0.; }
  else if( rndno < 1. ) { result.real = -0.5; result.imag = 0.5*sqrt(3.); }
  else { result.real = -0.5; result.imag = -0.5*sqrt(3.); }
  return result;
}
