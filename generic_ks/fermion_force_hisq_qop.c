/******* fermion_force_hisq_qop.c ****************/
/* MIMD version 7 */

/* This is the MILC wrapper for the SciDAC Level 3 QOP fermion force routine */
/* 5/25/09  C. DeTar and A Bazavov */

/* External entry points in this file

   eo_fermion_force_oneterm (unsupported)
   eo_fermion_force_twoterms (unsupported)
   eo_fermion_force_multi
   ks_multiff_opt_chr

 */

/* Compile with fermion_force_hisq_qop_[FD].c */

/*
 * $Log: fermion_force_hisq_qop.c,v $
 * Revision 1.1  2011/11/29 20:42:29  detar
 * Add
 *
 */

#include "generic_ks_includes.h"
#include "../include/generic_qop.h"
#include "../include/generic_ks_qop.h"
#include "../include/fermion_links.h"

/* Set default if undeclared */
#ifndef KS_MULTIFF
#define KS_MULTIFF FNMAT
#endif

//static char* cvsHeader = "$Header: /lqcdproj/detar/cvsroot/milc_qcd/generic_ks/fermion_force_hisq_qop.c,v 1.1 2011/11/29 20:42:29 detar Exp $";

/**********************************************************************/
/* Standard MILC interface for the single-species HISQ fermion force
   routine */
/**********************************************************************/
void eo_fermion_force_oneterm( Real eps, Real weight, su3_vector *x_off,
			       int prec, fermion_links_t *fl)
{

  node0_printf("QOP wrapper for eo_fermion_force_oneterm is not supported\n");
  terminate(1);
//  if(prec == 1)
//    eo_fermion_force_oneterm_F( eps, weight, x_off, fl );
//  else
//    eo_fermion_force_oneterm_D( eps, weight, x_off, fl );

}

void eo_fermion_force_oneterm_site( Real eps, Real weight, field_offset x_off_site,
				    int prec, fermion_links_t *fl)
{

  node0_printf("QOP wrapper for eo_fermion_force_oneterm is not supported\n");
  terminate(1);

//  su3_vector *x_off;
//  x_off = create_v_field_from_site_member(x_off_site);
//
//  eo_fermion_force_oneterm(eps, weight, x_off, prec, fl);
//
//  destroy_v_field(x_off);

}

/**********************************************************************/
/* Standard MILC interface for the two-species HISQ fermion force
   routine */
/**********************************************************************/
void eo_fermion_force_twoterms( Real eps, Real weight1, Real weight2, 
				su3_vector *x1_off, su3_vector *x2_off,
				int prec, fermion_links_t *fl)
{

  node0_printf("QOP wrapper for eo_fermion_force_twoterms is not supported\n");
  terminate(1);
//  if(prec == 1)
//    eo_fermion_force_twoterms_F( eps, weight1, weight2, x1_off, x2_off, fl );
//  else
//    eo_fermion_force_twoterms_D( eps, weight1, weight2, x1_off, x2_off, fl );

}


void eo_fermion_force_twoterms_site( Real eps, Real weight1, Real weight2, 
				     field_offset x1_off_site, field_offset x2_off_site,
				     int prec, fermion_links_t *fl)
{

  su3_vector *x1_off = create_v_field_from_site_member(x1_off_site);
  su3_vector *x2_off = create_v_field_from_site_member(x2_off_site);

  eo_fermion_force_twoterms(eps, weight1, weight2, x1_off, x2_off, prec, fl);

  destroy_v_field(x2_off);
  destroy_v_field(x1_off);

}


/**********************************************************************/
/*   Parallel transport nterms source vectors                        */
/**********************************************************************/

static void 
fermion_force_multi( Real eps, Real *residues, 
		     su3_vector **xxx, int nterms, int prec,
		     fermion_links_t *fl ) 
{

  if(prec == 1)
    fermion_force_multi_hisq_F( eps, residues, xxx, n_orders_naik, fl );
  else
    fermion_force_multi_hisq_D( eps, residues, xxx, n_orders_naik, fl );

}

// /**********************************************************************/
// /*   Parallel transport vectors in blocks of veclength.               */
// /**********************************************************************/
// /* Requires the xxx1 and xxx2 terms in the site structure */
// 
// void 
// fermion_force_block( Real eps, Real *residues, 
// 		     su3_vector **xxx, int nterms, int veclength, 
// 		     int prec, fermion_links_t *fl) 
// {
//   if(prec == 1)
//     fermion_force_block_F( eps, residues, xxx, nterms, veclength, fl);
//   else
//     fermion_force_block_D( eps, residues, xxx, nterms, veclength, fl);
// 
// }

/**********************************************************************/
/*   Standard MILC interface for fermion force with multiple sources  */
/**********************************************************************/

static void set_qop_hisq_force_opts( int fnmat_src_min, int veclength ) {
  /* Note: the want_deps and want_aux options are set in hisq_links_qop.c */

  /* Set values */
  QOP_opt_t qop_hf_opt[3] = {

    {.tag = "fnmat_src_min",.value=fnmat_src_min},
    {.tag = "veclength",.value=veclength},


#ifdef HISQ_FORCE_FILTER
    {.tag = "force_filter",.value=HISQ_FORCE_FILTER}
#else
    {.tag = "force_filter",.value=0},
#endif

  };

  /* Set links options, overriding defaults */

  if(QOP_hisq_force_set_opts(qop_hf_opt, 3) != QOP_SUCCESS)
    node0_printf("eo_fermion_force_multi: error setting QOP options\n");
  
}

void eo_fermion_force_multi( Real eps, Real *residues, su3_vector **xxx, 
			     int nterms, int prec, fermion_links_t *fl ) {

  int veclength, fnmat_src_min;

#ifdef VECLENGTH
  veclength = VECLENGTH;
#else
  veclength = 4;
#endif

  switch(KS_MULTIFF){
  case ASVEC:
    fnmat_src_min = nterms + 1;
    break;
  default: /* FNMAT */
    fnmat_src_min = 4;
  }

  set_qop_hisq_force_opts(fnmat_src_min, veclength);

  fermion_force_multi( eps, residues, xxx, nterms, prec, fl );
}

/**********************************************************************/
/*   Accessor for string describing the option                        */
/**********************************************************************/
const char 
*ks_multiff_opt_chr( void )
{
  switch(KS_MULTIFF){
  case ASVEC:
    return "ASVEC";
    break;
  default:
    return "FNMAT";
  }
  return NULL;
}
