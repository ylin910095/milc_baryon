#ifndef _PARAMS_H
#define _PARAMS_H

#include "../include/macros.h"  /* For MAXFILENAME */
#include "../include/generic_quark_types.h"  /* For quark_source */
#include "lattice.h"   /* For MAX_NKAP */

/* structure for passing simulation parameters to each node */
typedef struct {
  int nx,ny,nz,nt;  /* lattice dimensions */
  uint32_t iseed;
	int nkap;   /******no. of kappa values******/
	int start_kap,start_spin,start_color;	/****** starting values of
                                                 kappa, spin, color in their
                                                 loops ******/
	int end_kap,end_spin,end_color;	/****** end values of
                                                 kappa, spin, color in their
                                                 loops ******/
	int flag;	/* flag=0 if psi(0)=0, 1 if not */
	int startflag;  /* what to do for beginning lattice */
	int startflag_w[MAX_NKAP];  /* what to do for beginning wilson vector */
	int saveflag_w[MAX_NKAP];  /* what to do for saving wilson vector */
	int saveflag_m;  /* what to do for saving meson props */
	quark_source wqs; /* source parameters */
	int source_parity;       /* Parity for hopping source */
	Real beta,cappa[MAX_NKAP],kappa_c; /* gauge coupling, 
                                   quark hopping parameter, 
                                  approx kappa_critial */
        int wall_separation; /* distance between wall gaussians at sink; should 
                                be even and should divide nx, ny, and nz */
	int nchannels;  /* number of meson channels; = NCHANNELS +2 for extra
                           sinks  */
	int niter; 	/* maximum number of MR or cg iterations */
	int nrestart; 	/* maximum number of MR or cg restarts */
	int nhop;   /* number of hopping steps */
	Real rsqmin,rsqprop;  /* for deciding on convergence */
	char startfile[MAXFILENAME];
	char startfile_w[MAX_NKAP][MAXFILENAME]; /******/
	char savefile_w[MAX_NKAP][MAXFILENAME]; /******/
	char savefile_m[MAX_NKAP][MAXFILENAME]; /******/

	int fixflag;    /* whether to gauge fix **/
	char savefile[MAXFILENAME];  /** what to do with the gauge file ***/
	char stringLFN[MAXFILENAME];  /** ILDG LFN if applicable ***/
	int saveflag ;

	int stopflag ; 


}  params;

#endif /* _PARAMS_H */
