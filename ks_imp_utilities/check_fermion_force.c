/************************* control.c *******************************/
/* MIMD version 7 */
/* Main procedure for SU3 with dynamical staggered fermions        */
/* general quark action, general gauge action */

/* This code performs and/or checks the fermion force calculation */

#include "ks_imp_includes.h"	/* definitions files and prototypes */
#ifdef HAVE_QIO
#include <qio.h>
#endif

void check_fermion_force( char *srcfile, int srcflag, field_offset src, 
			  char *ansfile, int ansflag, Real mass)
{
  Real diff, maxdiff, norm, maxnorm, reldiff;
  int i, dir;
  site *s;
  Real eps = 1.;
  int nflavors = 4;
  su3_matrix tmat, diffmat, tnorm;
  char *filexml;
  char recxml[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><title>Test fermion force field</title>";
#if (PRECISION == 1)
  Real tol = 1e-3;
#else
  Real tol = 1e-5;
#endif
  

  /* Make a random source in xxx if we don't reload it */
  if(srcflag == RELOAD_SERIAL){
    restore_ks_vector_scidac_to_site (srcfile, src, QIO_SERIAL, 1);
    fflush(stdout);
  }
  else {
    /* generate g_rand random; phi = Mdagger g_rand */
    node0_printf("Generating a random source\n");
    grsource_imp( src, mass, EVENANDODD );
  }
      
  node0_printf("Computing the fermion force\n"); fflush(stdout);
  
  /* Just to be safe, clear the answer */
  FORALLSITES(i,s){
    FORALLUPDIR(dir){
      s->mom[dir].m00im = 0.0;
      s->mom[dir].m11im = 0.0;
      s->mom[dir].m22im = 0.0;
      s->mom[dir].m01.real = 0.0;
      s->mom[dir].m01.imag = 0.0;
      s->mom[dir].m02.real = 0.0;
      s->mom[dir].m02.imag = 0.0;
      s->mom[dir].m12.real = 0.0;
      s->mom[dir].m12.imag = 0.0;
    }
  }

  /** eo_fermion_force_oneterm( eps, ((Real)nflavors)/4., src );**/
  eo_fermion_force_twoterms( eps/2, 0.5*((Real)nflavors)/4., 
       1.5*((Real)nflavors)/4., src, src );

  /* If the answer file is given, read it for comparison */
  if(ansflag == RELOAD_SERIAL){
    restore_color_matrix_scidac_to_site(ansfile, F_OFFSET(ansmom[0]), 4);
    node0_printf("Checking the answer\n"); fflush(stdout);
  }

  /* Unpack the answer and compare if possible */
  maxdiff = 0;
  norm = 0;
  FORALLSITES(i,s){
    FORALLUPDIR(dir){
      uncompress_anti_hermitian( &(s->mom[dir]), &tmat );
      /* If we have loaded an answer file, do the comparison */
      if(ansflag == RELOAD_SERIAL){
	sub_su3_matrix( &(s->ansmom[dir]), &tmat, &diffmat);
	diff = sqrt(realtrace_su3( &diffmat, &diffmat ));
	norm = sqrt(realtrace_su3( &tmat, &tmat));
	if(diff > tol * norm){
	  printf("Intolerable difference %e node %d site %d\n",
		 diff,this_node,i);
	  dumpmat(&(s->ansmom[dir]));
	  dumpmat(&tmat);
	}
	if(maxdiff < diff)maxdiff = diff;
	if(maxnorm < norm)maxnorm = norm;
      }
      /* In any case, copy the new result to the answer matrix */
      s->ansmom[dir] = tmat;
    }
  }

  if(ansflag == RELOAD_SERIAL){
    g_floatmax(&maxdiff);
    g_floatmax(&maxnorm);
    if(maxnorm > 0){
      reldiff = maxdiff/maxnorm;
      node0_printf("Relative difference %e\n",reldiff);
    }
    else
      node0_printf("Absolute difference %e but norm is 0???\n",maxdiff);
  }      

  /* Save source and answer if requested */
#ifdef HAVE_QIO
  if(srcflag == SAVE_SERIAL)
    save_ks_vector_scidac_from_site(srcfile, "source color vector field", 
			  QIO_SINGLEFILE, QIO_SERIAL, src, 1);
  else if(srcflag == SAVE_PARTITION_SCIDAC)
    save_ks_vector_scidac_from_site(srcfile, "source color vector field",
                                    QIO_PARTFILE, QIO_SERIAL, src, 1);
  
  if(ansflag == SAVE_SERIAL){
    filexml = create_QCDML();
    save_color_matrix_scidac_from_site(ansfile, filexml, 
       recxml, QIO_SINGLEFILE,  F_OFFSET(ansmom[0]), 4);
    free_QCDML(filexml);
  }
  else if(ansflag == SAVE_PARTITION_SCIDAC){
    node0_printf("Saving the momentum matrix\n");
    filexml = create_QCDML();
    save_color_matrix_scidac_from_site(ansfile, filexml, 
       recxml, QIO_PARTFILE,  F_OFFSET(ansmom[0]), 4);
    free_QCDML(filexml);
  }
#endif

}      
