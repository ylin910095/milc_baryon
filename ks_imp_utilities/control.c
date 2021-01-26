/************************* control.c *******************************/
/* MIMD version 7 */
/* Main procedure for SU3 with dynamical staggered fermions        */
/* general quark action, general gauge action */

/* This code performs and/or checks the KS inversion and performs
   and/or compares with a standard file, the fermion force calculation */

#define CONTROL
#include "ks_imp_includes.h"	/* definitions files and prototypes */
#ifdef HAVE_QIO
#include <qio.h>
#endif
#include "lattice_qdp.h"
#include "params.h"

EXTERN  gauge_header start_lat_hdr;     /* Input gauge field header */

int main( int argc, char **argv ){
  int prompt;
  char *filexml;
  
  initialize_machine(&argc,&argv);

  /* Remap standard I/O if needed */
  if(remap_stdio_from_args(argc, argv) == 1)terminate(1);
  
  g_sync();
  /* set up */
  prompt = setup();

  /* loop over input sets */
  while( readin(prompt) == 0){
    
    if(prompt == 2)continue;
    
    node0_printf("BEGIN\n");

    imp_ferm_links_t *fn = get_fm_links(fn_links)[0];
    /* Initially, the FN links have standard KS phases and
       antiperiodic BC in time.  The next operation allows us to shift
       the KS phases to phases based on a different coordinate origin
       r0, if so desired.  It also modifies the time boundary
       condition in the FN links to conform with the requested
       periodicity */
    Real bdry_phase[4] = {0.,0.,0.,param.time_bc};
    set_boundary_twist_fn(fn, bdry_phase, param.coord_origin);
    boundary_twist_fn(fn, ON);

#ifdef CHECK_INVERT
    check_ks_invert( param.srcfile[0], srcflag, param.ansfile,
		     param.ansflag, param.nmass, param.ksp,
		     param.qic);
#else
#ifdef FERMION_FORCE
#ifndef HAVE_QIO
#error Checking the fermion force requires QIO compilation
#endif
    
    check_fermion_force( param.srcfile, srcflag, param.ansfile[0], 
			 param.ansflag[0], param.nmass, param.ksp);
    node0_printf("Done checking fermion force\n");
#endif
#endif
    
    /* save lattice if requested */
    if( saveflag != FORGET ){
      /* The lattice is saved without the KS phases and without
	 the built-in antiperiodic BC phase */
      rephase( OFF );
      node0_printf("Saving the lattice\n");
      save_lattice( saveflag, savefile, stringLFN );
      rephase( ON );
    }
    
#ifdef FN
    /* save longlinks if requested */
    if (savelongflag != FORGET ){
#ifdef HAVE_QIO
      su3_matrix *lng = get_lnglinks(fn);
      /* The long links are saved without the KS phases, BUT with
	 the time boundary condition requested in param.coord_origin */
      rephase_field_offset( lng, OFF, NULL, param.coord_origin);
      filexml = create_QCDML();
      node0_printf("Saving the long links with LFN\n '%s'\n", stringLFNlong);
      save_color_matrix_scidac_from_field( savelongfile, filexml, 
	   "Long links", QIO_SINGLEFILE, lng, 4, MILC_PRECISION,
	   stringLFNlong);
      free_QCDML(filexml);
#else
      printf("ERROR: Can't save the longlinks.  Recompile with QIO\n");
#endif
    }
    
    /* save fatlinks if requested */
    if (savefatflag != FORGET ){
#ifdef HAVE_QIO
      filexml = create_QCDML();
      su3_matrix *fat = get_fatlinks(fn);
      /* The fat links are saved without the KS phases, BUT with
	 the time boundary condition requested in param.coord_origin */
      rephase_field_offset( fat, OFF, NULL, param.coord_origin);
      node0_printf("Saving the fat links with LFN\n '%s'\n", stringLFNfat);
      save_color_matrix_scidac_from_field( savefatfile, filexml, 
	   "Fat links", QIO_SINGLEFILE, fat, 4, MILC_PRECISION,
	   stringLFNfat);
      free_QCDML(filexml);
#else
      printf("ERROR: Can't save the fatlinks.  Recompile with QIO\n");
#endif
    }
#endif
    boundary_twist_fn(fn, OFF);
  }
  node0_printf("RUNNING COMPLETED\n");

#ifndef CHECK_INVERT

#ifdef HISQ_SVD_COUNTER
  printf("hisq_svd_counter = %d\n", hisq_svd_counter);
#endif
  
#ifdef HISQ_FORCE_FILTER_COUNTER
  printf("hisq_force_filter_counter = %d\n", hisq_force_filter_counter);
#endif

#endif

  return 0;
}

