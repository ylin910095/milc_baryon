/******* ks_ratinv.c - multi-mass CG for rational function approximations
 forSU3/fermions 
 Also includes a routine to evaluate the rational function after the inversion
 is done
****/
/* MIMD version 7 */

/* Multi-mass CG inverter for staggered fermions */

/* Based on B. Jegerlehner, hep-lat/9612014.
   See also A. Frommer, S. G\"usken, T. Lippert, B. N\"ockel,"
   K. Schilling, Int. J. Mod. Phys. C6 (1995) 627. 

   This version is based on d_congrad5_fn.c and d_congrad5_eo.c 

   approximation form:  a0 + a1/(x-b1) + ... aN/(x-bN)  t
   N = "order"
   a[i] = residues
   b[i] = roots
   psim[i-1] = 1/(M^dagger M + b[i] )

   For "fat link actions", ie when FN is defined, this version
   assumes connection to nearest neighbor points is stored in fatlink.
   For actions with a Naik term, it assumes the connection to third
   nearest neighbors is in longlink.

*/


#include "ks_imp_includes.h"	/* definitions files and prototypes */

int ks_ratinv(	/* Return value is number of iterations taken */
    field_offset src,	/* source vector (type su3_vector) */
    su3_vector **psim,	/* solution vectors */
    Real *roots,	/* the roots */
    int order,		/* order of rational function approx */
    int my_niter,	/* maximal number of CG interations */
    Real rsqmin,	/* desired residue squared */
    int prec,           /* desired intermediate precicion */
    int parity,		/* parity to be worked on */
    Real *final_rsq_ptr,/* final residue squared */
    ferm_links_t *fn      /* Fermion links */
    )
{
    // Just a multimass inversion.  start at roots[1] because first term
    // in rational function expansion is just the constant term
    // It's "order" instead of "order-1" because order is order of expansion, arrays
    // have order+1 elements 
  return ks_multicg( src, psim, roots+1, order, my_niter, rsqmin, 
		      prec, parity, final_rsq_ptr, fn );
}

/* evaluate the rational function approximation after all the
  1 / ( M^dagger M + root) 's have been computed */
int ks_rateval(	
    su3_vector *dest,	/* answer vector */
    field_offset src,	/* source vector (for a_0 term) */
    su3_vector **psim,	/* solution vectors  from multiroot CG */
    Real *residues,	/* the residues */
    int order,		/* order of approximation */
    int parity		/* parity to be worked on */
    )
{
   register int i,j; register site *s;
   FORSOMEPARITY(i,s,parity){
      scalar_mult_su3_vector( (su3_vector *)F_PT(s,src), residues[0], &(dest[i]) );
      for(j=1;j<=order;j++){
        scalar_mult_add_su3_vector( &(dest[i]), &(psim[j-1][i]), residues[j], &(dest[i]) );
      }
   }
   return 0;
}
