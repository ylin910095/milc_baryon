#ifndef _GENERIC_QUARK_TYPES_H
#define _GENERIC_QUARK_TYPES_H

#include "../include/macros.h"
#include "../include/precision.h"
#include "../include/int32type.h"
#include "../include/complex.h"
#include "../include/su3.h"

/* Structures defining a generic quark source for both KS and Dirac fermions */

#define ALL_T_SLICES -1
#define MAXDESCRP 128
#define MAXSRCLABEL 32
#define MAXWEIGHTS 5

#ifdef HAVE_QIO
#include <qio.h>
#endif

/* For quark source and sink routines - both Wilson and KS */
/* The Weyl representation types are included for w_source_h */
enum source_type { 
  UNKNOWN = 0, 
  ASLASH_KS_FILE,
  COMPLEX_FIELD_FILE, 
  COMPLEX_FIELD_FM_FILE, 
  COMPLEX_FIELD_STORE,
  CORNER_WALL, 
  CORNER_WALL_0, 
  CORNER_WALL_X, 
  CORNER_WALL_Y, 
  CORNER_WALL_XY, 
  CORNER_WALL_Z, 
  CORNER_WALL_ZX, 
  CORNER_WALL_YZ, 
  CORNER_WALL_XYZ, 
  CUTOFF_GAUSSIAN, 
  CUTOFF_GAUSSIAN_WEYL, 
  COVARIANT_GAUSSIAN,
  DERIV1,
  DERIV2_D,
  DERIV2_B,
  DERIV3_A,
  EXT_SRC_KS,
  EXT_SRC_DIRAC,
  DIRAC_FIELD_FILE, 
  DIRAC_FIELD_FM_FILE, 
  DIRAC_FIELD_STORE,
  DIRAC_INVERSE,
  DIRAC_PROPAGATOR_FILE,
  EVEN_WALL, 
  EVENANDODD_WALL, 
  EVENMINUSODD_WALL, 
  FAT_COVARIANT_GAUSSIAN,
  FAT_COVARIANT_LAPLACIAN,
  FUNNYWALL1,
  FUNNYWALL2,
  GAMMA,
  GAUSSIAN, 
  HOPPING, 
  IDENTITY,
  KS_GAMMA,
  KS_GAMMA_INV,
  KS_INVERSE,
  MODULATION_FILE,
  MOMENTUM,
  PAR_XPORT_SRC_KS,
  POINT, 
  POINT_WEYL, 
  PROJECT_T_SLICE,
  RANDOM_COMPLEX_WALL,
  RANDOM_COLOR_WALL,
  RANDOM_SPARSE_Z3,
  RANDOM_ANTIPODE_Z3,
  RANDOM_TETRAHEDRAL_Z3,
  ROTATE_3D,
  SAVE_VECTOR_SRC,
  SPIN_TASTE,
  SPIN_TASTE_EXTEND,
  WAVEFUNCTION_FILE,
  VECTOR_FIELD_FILE, 
  VECTOR_FIELD_FM_FILE, 
  VECTOR_FIELD_STORE,
  VECTOR_PROPAGATOR_FILE
} ;

enum subset_type {
  FULL,
  HYPERCUBE
};

/* Header structure for a KS source in FNAL format */
typedef struct {
  int32type magic_number;
  int32type gmtime;
  int32type size_of_element;
  int32type elements_per_site;
  int32type dims[4];
  int32type site_order;
} ks_source_header;


/* File structure for a KS source in FNAL format */
typedef struct {
  ks_source_header *header;
  FILE *fp;
  char filename[MAXFILENAME];
  int byterevflag;
} ks_fm_source_file;

/* Propagator types */

#define CLOVER_TYPE 0
#define KS_TYPE 1
#define KS0_TYPE 2
#define KS4_TYPE 3
#define IFLA_TYPE 4

/* Field types */

#define WILSON_FIELD 0
#define KS_FIELD 1

/* Structures required for specific inverters */

/* Structure defining parameters of Dirac matrix for clover inversion */
/* To be passed through to inverter. */
typedef struct {
  Real Kappa;        /* hopping */
  Real Clov_c;       /* Perturbative clover coeff */
  Real U0;           /* Tadpole correction to Clov_c */
} dirac_clover_param;

/* Same for Wilson case */
typedef struct {
  Real Kappa;        /* hopping */
} dirac_wilson_param;

/* Size of a bookkeeping table holding unique charges */
#define MAX_CHARGE 16

/* Same for plain KS case */
typedef struct {
  Real mass;
  Real charge;
  Real offset;    /* For RHMC, the pole position */
  Real residue;   /* For RHMC, the pole residue */
  int naik_term_epsilon_index;
  int charge_index;
  Real naik_term_epsilon;
} ks_param;

/* This is the IFLA case */
typedef struct {
  Real kapifla;
  Real kappa_s;
  Real kappa_t;
  Real r_s;
  Real r_t;
  Real zeta;
  Real c_E;
  Real c_B;
  Real c_1;
  Real c_2;
  Real c_3;
  Real c_4;
  Real c_5;
  Real c_EE;
  Real u0;
} newaction_ifla_param;

/* Structure defining quark inversion parameters for most inverters */

enum inv_type {
  MGTYPE,
  CGTYPE
};

enum mg_rebuild_type {
  FULLREBUILD,               /* do a full rebuild, expensive but best solve */
  THINREBUILD,               /* do a thin rebuild, skips overhead of rebuild but
                         leads to less effective preconditioner */
  CGREBUILD                  /* override and perform CG instead */
};

typedef struct {
  int prec;           /* precision of the inversion 1 = single; 2 = double */
  int min;            /* minimum number of iterations (being phased out) */
  int max;            /* maximum number of iterations per restart */
  int nrestart;       /* maximum restarts */
  int parity;         /* EVEN, ODD, or EVENANDODD (for some inverters) */
  int start_flag;     /* 0: use a zero initial guess; 1: use dest */
  int nsrc;           /* Number of source vectors */
  int deflate;        /* True if we want to deflate. False if not. */
  Real resid;         /* desired residual - NOT SQUARED!
			 normalized as sqrt(r*r)/sqrt(src_e*src_e) */
  Real relresid;      /* desired relative residual - NOT SQUARED! */
  Real mixed_rsq;     /* For QOP single-mass mixed-precision solver */
  Real final_rsq;     /* Final true (absolute) residual. Norm'ed to (r*r)/(src*src) */
  Real final_relrsq;  /* Final relative residual. Same normalization. */
  Real size_r;        /* resulting cumulative residual. Same normalization. */
  Real size_relr;     /* resulting cumulative relative residual. Same normalization. */
  int converged;      /* returned 0 if not converged; 1 if converged */
  int  final_iters;
  int  final_restart;
  enum inv_type inv_type;  /* requested inverter type */
  char mgparamfile[MAXFILENAME];        /* Name of file with the staggered multigrid parameters */
  enum mg_rebuild_type mg_rebuild_type;    /* how to refresh MG solve if mass/gauge links change */
                      /* Add further parameters as needed...  */
} quark_invert_control;

void report_status(quark_invert_control *qic);

/* Forward declaration of qss_op_struct as it will be used in 
   quark_source definition. The actual definition of qss_op_struct
   is at the end of the file */
struct qss_op_struct;
typedef struct qss_op_struct quark_source_sink_op;

/* Structure defining a staggered or Wilson (or clover) quark source */

typedef struct {
  int field_type;     /* type of field for this source (KS or Dirac) */
  int type;           /* source type */
  int subset;         /* hypercube corners or full time slice */
  Real scale_fact;    /* scale factor */
  char descrp[MAXDESCRP];  /* alpha description for most */
  char label[MAXSRCLABEL]; /* Abbreviation of description */
  int ksource;        /* Counter for a list of wilson_vectors generated */
  int nsource;        /* Number of source wilson_vectors to be generated */
  int color;          /* Counter for source colors generated */
  int ncolor;         /* number of source su3_vectors to be generated*/
  int spin_snk;       /* Counter for KS propagators from a Dirac source */
  int skip;           /* Number of sites to skip between support in sparse sources */

  Real a;             /* Lattice spacing for converting wave function file */
  int x0,y0,z0,t0;    /* source coordinates for most */ 
  char source_file[MAXFILENAME]; /* file name for some sources */
  int sourceflag;      /* mode of reading or writing for some sources */
  char save_file[MAXFILENAME]; /* file name for saving the source */
  int savetype;        /* Type of file */
  int saveflag;           /* mode of writing */
  int source_file_initialized;
  int save_file_initialized;
  int mom[3];         /* insertion momentum for some sources */
  Real r0;            /* source size for gaussian, width for gauge invt  */
#ifdef HAVE_QIO
  QIO_Reader *infile;
  QIO_Writer *outfile;
#endif
  /* To be discontinued ... */
  quark_source_sink_op *op;   /* op need to create this 
				      source from parent */
  complex *c_src;      /* Pointer for complex source field storage */
  su3_vector *v_src;    /* su3_vector source for color walls */
  wilson_vector *wv_src; /* su3_vector source for color walls */
  ks_fm_source_file *kssf;
  int orig_type;      /* original source type */
  int parity;         /* even or odd sites for w_source_h */
  int src_pointer ;   /* smearing function (for the moment, only
		         clover_finite_p_vary/create_wilson_source.c) */
  int wall_cutoff;    /* half size of box for w_source_h */

} quark_source;


/* Structure defining a quark source operator */
struct qss_op_struct {
  int type;           /* operator type */
  char descrp[MAXDESCRP]; /* alpha description for most */
  char label[MAXSRCLABEL]; /* Abbreviation of description */
  Real a;             /* Lattice spacing for converting wave function file */
  Real d1;            /* Fermilab 3D rotation parameter */
  int dir1, dir2;     /* Directions for derivatives and hopping */
  int disp;           /* Stride for derivatives */
  Real weights[MAXWEIGHTS];  /* Weights for derivatives */
  int dhop;           /* 0 for hop, 1 for 1st deriv of hop, 2 for 2nd */
  int fb;             /* For hop: +1 = forward only, -1 = backward only, 0 = both */
  int iters;          /* iterations for covariant gaussian source */
  Real r0;            /* source size for gaussian, width for gauge invt  */
  int stride;         /* Subset flag for gaussian source */
  int r_offset[4];    /* Coordinate offset for phases for some operators */
  int spin_taste;     /* For staggered fermions for some operators */
  int gamma;          /* For Dirac fermions for some operators */
  int mom[3];         /* insertion momentum for some operators */
  char source_file[MAXFILENAME]; /* file name for some sources */
  dirac_clover_param dcp; /* For Dirac solver */
  char kappa_label[32]; /* For Dirac solver */
  ks_param ksp;        /* For KS solver */
  char mass_label[32]; /* For KS solver */
  Real eps_naik;      /* Naik epsilon for KS hopping operator and KS inverse */
  quark_invert_control qic; /* For Dirac and KS solver */
  Real bp[4];         /* Boundary phase for Dirac and KS solvers */
  int t0;             /* For time slice projection */
  quark_source qs_save; /* for SAVE_QUARK_SRC */
  struct qss_op_struct *op;   /* Next operation in the chain */
};

#endif /* _GENERIC_QUARK_TYPES_H */


