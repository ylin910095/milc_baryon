/*********************** gauge_force_imp.c  -- ****************************/
/* MIMD version 7 */
/* gauge action stuff for improved action
* T.D. and A.H. general gauge action updating code
* D.T. modified  5/97
* D.T. modified 12/97, optimized gauge_force a little
* D.T. modified 3/99, gauge action in include file
* C.D. split from gauge_stuff.c 10/06 */

/**#define GFTIME**/ /* For timing gauge force calculation */
#include "generic_includes.h"	/* definitions files and prototypes */
#include "../include/openmp_defs.h"

//#ifdef LOOPEND
//#undef FORALLSITES
//#define FORALLSITES(i,s) 
//{ register int loopend; loopend=sites_on_node; 
 //for( i=0,  s=lattice ; i<loopend; i++,s++ )
//#define END_LOOP }
//#else
//#define END_LOOP        /* define it to be nothing */
//#endif

#define GOES_FORWARDS(dir) (dir<=TUP)
#define GOES_BACKWARDS(dir) (dir>TUP)
void printpath( int *path, int length );

#ifdef QCDOC
#define special_alloc qcdoc_alloc
#define special_free qfree
#else
#define special_alloc malloc
#define special_free free
#endif

/* update the momenta with the gauge force */
void imp_gauge_force_cpu( Real eps, field_offset mom_off ){
    register int i,dir;
    register site *st;
    su3_matrix tmat1,tmat2;
    register Real eb3;
    register anti_hermitmat* momentum;
    su3_matrix *staple, *tempmat1;
#ifdef ANISOTROPY
    int is_temporal; /* to decide what kind of staple we have:
                      0 - spatial, 1 - temporal */
#endif

    /* lengths of various kinds of loops */
    int *loop_length = get_loop_length();
    /* number of rotations/reflections  for each kind */
    int *loop_num = get_loop_num();
    /* table of directions, 1 for each kind of loop */
    int ***loop_table = get_loop_table();
    /* table of coefficients in action, for various "representations"
	(actually, powers of the trace) */
    Real **loop_coeff = get_loop_coeff();
    int max_length = get_max_length();
    int nloop = get_nloop();
    int nreps = get_nreps();

#ifdef GFTIME
    int nflop = 153004;  /* For Symanzik1 action */
    double dtime;
#endif
    int j,k;
    int *dirs,length;
    int *path_dir,path_length;

    int ln,iloop;
    Real action,act2,new_term;

    int ncount;
    char myname[] = "imp_gauge_force_cpu";
    su3_matrix *links;

#ifdef GFTIME
    dtime=-dclock();
#endif

    dirs = (int *)malloc(max_length*sizeof(int));
    if(dirs == NULL){
      printf("%s(%d): Can't malloc dirs\n",myname,this_node);
      terminate(1);
    }
    path_dir = (int *)malloc(max_length*sizeof(int));
    if(path_dir == NULL){
      printf("%s(%d): Can't malloc path_dir\n",myname,this_node);
      terminate(1);
    }
    staple = (su3_matrix *)special_alloc(sites_on_node*sizeof(su3_matrix));
    if(staple == NULL){
      printf("%s(%d): Can't malloc temporary\n",myname,this_node);
      terminate(1);
    }

    tempmat1 = (su3_matrix *)special_alloc(sites_on_node*sizeof(su3_matrix));
    if(tempmat1 == NULL){
      printf("%s(%d): Can't malloc temporary\n",myname,this_node);
      terminate(1);
    }

#ifndef ANISOTROPY
    eb3 = eps*beta/3.0;
#else
    eb3 = eps/3.0;
#endif
    links = create_G_from_site();

    /* Loop over directions, update mom[dir] */
    for(dir=XUP; dir<=TUP; dir++){

	FORALLSITES_OMP(i,st,private(j,k))for(j=0;j<3;j++)for(k=0;k<3;k++){
			staple[i].e[j][k]=cmplx(0.0,0.0);
	} END_LOOP_OMP

	ncount=0;
	for(iloop=0;iloop<nloop;iloop++){
	    length=loop_length[iloop];
	    for(ln=0;ln<loop_num[iloop];ln++){
/**printf("UPD:  "); printpath( loop_table[iloop][ln], length );**/
		/* set up dirs.  we are looking at loop starting in "XUP"
		   direction, rotate so it starts in "dir" direction. */
#ifdef ANISOTROPY
		/* initialize staple flag as spatial */
		is_temporal = 0;
#endif
		for(k=0;k<length;k++){
                    if( GOES_FORWARDS(loop_table[iloop][ln][k]) ){
                	dirs[k]=(dir+loop_table[iloop][ln][k] )% 4;
		    }
            	    else {
                        dirs[k]=OPP_DIR(
			    (dir+OPP_DIR(loop_table[iloop][ln][k]))%4 );
		    }
#ifdef ANISOTROPY
		    /* flip the flag if a temporal link is encountered */
		    if( is_temporal==0 && ( dirs[k]==TUP || dirs[k]==TDOWN ) )
			is_temporal=1;
#endif
		}

		path_length= length-1;  /* generalized "staple" */

		/* check for links in direction of momentum to be
		   updated, each such link gives a contribution. Note
		   the direction of the path - opposite the link. */
		for(k=0;k<length;k++)if( dirs[k]==dir||dirs[k]==OPP_DIR(dir)) {
		    if( GOES_FORWARDS(dirs[k]) ) for(j=0;j<path_length;j++) {
			path_dir[j] = dirs[(k+j+1)%length];
		    }
		    if( GOES_BACKWARDS(dirs[k]) ) for(j=0;j<path_length;j++) {
			path_dir[path_length-1-j] =
			    OPP_DIR(dirs[(k+j+1)%length]);
		    }
/**if(dir==XUP)printf("X_UPDATE PATH: "); printpath( path_dir, path_length );**/
		    path_product_fields(links, path_dir, path_length, tempmat1);

		    /* We took the path in the other direction from our
			old convention in order to get it to end up
			"at our site", so now take adjoint */
		    /* then compute "single_action" contribution to
			staple */
		    FORALLSITES_OMP(i,st, private(tmat1,new_term,act2,action) ){
			su3_adjoint( &(tempmat1[i]), &tmat1 );
			/* first we compute the fundamental term */
			new_term = loop_coeff[iloop][0];
#ifdef ANISOTROPY
			/* multiply the coefficient by the coupling -
                           this weighs spatial and temporal paths
                           differently */
			new_term *= beta[is_temporal];
#endif

			/* now we add in the higher representations */
			if(nreps > 1){
node0_printf("WARNING: THIS CODE IS NOT TESTED\n"); exit(0);
			    act2=1.0;
			    action = 3.0 - realtrace_su3(&(st->link[dir]),
				&tmat1 ); 

			    for(j=1;j<nreps;j++){
				act2 *= action;
				new_term +=
				    loop_coeff[iloop][j]*act2*(Real)(j+1);
			    }
			}  /* end if nreps > 1 */

			scalar_mult_add_su3_matrix( &(staple[i]), &tmat1,
				new_term, &(staple[i]) );

		    } END_LOOP_OMP

		    ncount++;

		} /* k (location in path) */
	    } /* ln */
	} /* iloop */

	/* Now multiply the staple sum by the link, then update momentum */
	FORALLSITES_OMP(i,st, private(tmat1,tmat2,momentum) ){
	    mult_su3_na( &(st->link[dir]), &(staple[i]), &tmat1 );
	    momentum = (anti_hermitmat *)F_PT(st,mom_off);
	    uncompress_anti_hermitian( &momentum[dir], &tmat2 );
	    scalar_mult_sub_su3_matrix( &tmat2, &tmat1,
		eb3, &(staple[i]) );
	    make_anti_hermitian( &(staple[i]), &momentum[dir] );
	} END_LOOP_OMP
    } /* dir loop */
#ifdef GFTIME
dtime+=dclock();
node0_printf("GFTIME:   time = %e (Symanzik1) mflops = %e\n",dtime,
	     nflop*(double)volume/(1e6*dtime*numnodes()) );
#endif
 free(path_dir);
 free(dirs);
 destroy_G(links);
 special_free(staple); 
 special_free(tempmat1); 
} /* imp_gauge_force_cpu.c */

/* Wrapper for backward compatibility */
void imp_gauge_force( Real eps, field_offset mom_off ){
  imp_gauge_force_cpu(eps, mom_off);
}

