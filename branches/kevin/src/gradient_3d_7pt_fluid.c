/*****************************************************************************
 *
 *  gradient_3d_7pt_fluid.c
 *
 *  Gradient operations for 3D seven point stencil.
 *
 *                        (ic, jc+1, kc)
 *         (ic-1, jc, kc) (ic, jc  , kc) (ic+1, jc, kc)
 *                        (ic, jc-1, kc)
 *
 *  ...and so in z-direction
 *
 *  d_x phi = [phi(ic+1,jc,kc) - phi(ic-1,jc,kc)] / 2
 *  d_y phi = [phi(ic,jc+1,kc) - phi(ic,jc-1,kc)] / 2
 *  d_z phi = [phi(ic,jc,kc+1) - phi(ic,jc,kc-1)] / 2
 *
 *  nabla^2 phi = phi(ic+1,jc,kc) + phi(ic-1,jc,kc)
 *              + phi(ic,jc+1,kc) + phi(ic,jc-1,kc)
 *              + phi(ic,jc,kc+1) + phi(ic,jc,kc-1)
 *              - 6 phi(ic,jc,kc)
 *
 *  Corrections for Lees-Edwards planes in X are included.
 *
 *  $Id: gradient_3d_7pt_fluid.c,v 1.2 2010-10-15 12:40:03 kevin Exp $
 *
 *  Edinburgh Soft Matter and Statistical Physics Group and
 *  Edinburgh Parallel Computing Centre
 *
 *  Kevin Stratford (kevin@epcc.ed.ac.uk)
 *  (c) 2010 The University of Edinburgh
 *
 *****************************************************************************/

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "leesedwards_s.h"
#include "gradient_3d_7pt_fluid.h"


HOST static void gradient_3d_7pt_fluid_operator(const int nop, 
					   const double * field,
					   double * t_field,
					   double * grad,
					   double * t_grad,
					   double * delsq,
					   double * t_delsq,
					   char * siteMask,
					   char * t_siteMask,
			     const int nextra);
HOST static void gradient_3d_7pt_fluid_le_correction(const int nop,
						const double * field,
						double * grad,
						double * delsq,
						const int nextra);

HOST static int gradient_dab_le_correct(int nf, const double * field, double * dab);
HOST static int gradient_dab_compute(int nf, const double * field, double * dab);

/*****************************************************************************
 *
 *  gradient_3d_7pt_fluid_d2
 *
 *****************************************************************************/

HOST int gradient_3d_7pt_fluid_d2(const int nop, 
			     const double * field,
			     double * t_field,
			     double * grad,
			     double * t_grad,
			     double * delsq,
			     double * t_delsq, 
			     char * siteMask,
			     char * t_siteMask
			     ) {

  int nhalo;
  int nextra;

  /* PENDING */
  le_nhalo(le_stat, &nhalo);
  nextra = nhalo - 1;
  assert(nextra >= 0);

  assert(field);
  assert(grad);
  assert(delsq);

  gradient_3d_7pt_fluid_operator(nop, field, t_field, grad, t_grad,
				 delsq, t_delsq, siteMask, t_siteMask, nextra);
  gradient_3d_7pt_fluid_le_correction(nop, field, grad, delsq, nextra);

  return 0;
}

/*****************************************************************************
 *
 *  gradient_3d_7pt_fluid_d4
 *
 *  Higher derivatives are obtained by using the same operation
 *  on appropriate field.
 *
 *****************************************************************************/

HOST int gradient_3d_7pt_fluid_d4(const int nop, 
			     const double * field,
			     double * t_field,
			     double * grad,
			     double * t_grad,
			     double * delsq,
			     double * t_delsq, 
			     char * siteMask,
			     char * t_siteMask
			     ) {
  int nhalo;
  int nextra;

  /* PENDING */
  le_nhalo(le_stat, &nhalo);
  nextra = nhalo - 2;
  assert(nextra >= 0);

  assert(field);
  assert(grad);
  assert(delsq);

  gradient_3d_7pt_fluid_operator(nop, field, t_field, grad, t_grad, delsq, t_delsq, siteMask, t_siteMask, nextra);
  gradient_3d_7pt_fluid_le_correction(nop, field, grad, delsq, nextra);

  return 0;
}

/*****************************************************************************
 *
 *  gradient_3d_7pt_fluid_dab
 *
 *  This is the full gradient tensor, which actually requires more
 *  than the 7-point stencil advertised.
 *
 *  d_x d_x phi = phi(ic+1,jc,kc) - 2phi(ic,jc,kc) + phi(ic-1,jc,kc)
 *  d_x d_y phi = 0.25*[ phi(ic+1,jc+1,kc) - phi(ic+1,jc-1,kc)
 *                     - phi(ic-1,jc+1,kc) + phi(ic-1,jc-1,kc) ]
 *  d_x d_z phi = 0.25*[ phi(ic+1,jc,kc+1) - phi(ic+1,jc,kc-1)
 *                     - phi(ic-1,jc,kc+1) + phi(ic-1,jc,kc-1) ]
 *  and so on.
 *
 *  The tensor is symmetric. The 1-d compressed storage is
 *      dab[NSYMM*index + XX] etc.
 *
 *****************************************************************************/

HOST int gradient_3d_7pt_fluid_dab(const int nf, 
				   const double * field,
				   double * dab){

  assert(nf == 1); /* Scalars only */

  gradient_dab_compute(nf, field, dab);
  gradient_dab_le_correct(nf, field, dab);

  return 0;
}


TARGET_CONST int tc_Nall[3];

/*****************************************************************************
 *
 *  gradient_3d_7pt_fluid_operator
 *
 *****************************************************************************/

static TARGET void gradient_3d_7pt_fluid_operator_site(const int nop,
						       const double * t_field,
						       double * t_grad,
						       double * t_del2, 
						       char * t_siteMask,
						       const int index){



  if(t_siteMask[index]){

    int coords[3];
    //coords_index_to_ijk(index, coords);

    GET_3DCOORDS_FROM_INDEX(index,coords,tc_Nall);

    //get index +1 and -1 in X dirn
    int indexm1 = INDEX_FROM_3DCOORDS(coords[0]-1,coords[1],coords[2],tc_Nall);
    int indexp1 = INDEX_FROM_3DCOORDS(coords[0]+1,coords[1],coords[2],tc_Nall);

      
    int n;
    int ys=tc_Nall[Z];
    
    for (n = 0; n < nop; n++) {
      t_grad[3*(nop*index + n) + X]
	= 0.5*(t_field[nop*indexp1 + n] - t_field[nop*indexm1 + n]);
      t_grad[3*(nop*index + n) + Y]
	= 0.5*(t_field[nop*(index + ys) + n] - t_field[nop*(index - ys) + n]);
      t_grad[3*(nop*index + n) + Z]
	= 0.5*(t_field[nop*(index + 1) + n] - t_field[nop*(index - 1) + n]);
      t_del2[nop*index + n]
	= t_field[nop*indexp1      + n] + t_field[nop*indexm1      + n]
	+ t_field[nop*(index + ys) + n] + t_field[nop*(index - ys) + n]
	+ t_field[nop*(index + 1)  + n] + t_field[nop*(index - 1)  + n]
	- 6.0*t_field[nop*index + n];
      
    }
    
  }
  
  return;
}


static TARGET_ENTRY void gradient_3d_7pt_fluid_operator_lattice(const int nop,
					   const double * t_field,
					   double * t_grad,
						double * t_del2, 
						   char * t_siteMask){



  int nSites=tc_Nall[X]*tc_Nall[Y]*tc_Nall[Z];

  int index;
  TARGET_TLP(index,nSites){
    gradient_3d_7pt_fluid_operator_site(nop,t_field,t_grad,t_del2,t_siteMask,index);
  }


}


static void gradient_3d_7pt_fluid_operator(const int nop,
					   const double * field,
					   double * t_field,
					   double * grad,
					   double * t_grad,
					   double * del2,
					   double * t_del2,
					   char * siteMask,
					   char * t_siteMask,
					   const int nextra) {
  int nlocal[3];
  int nhalo;
  int ic, jc, kc;
  int index;

  /* PENDING */

  le_nhalo(le_stat, &nhalo);
  le_nlocal(le_stat, nlocal);


  int Nall[3];
  Nall[X]=nlocal[X]+2*nhalo;  Nall[Y]=nlocal[Y]+2*nhalo;  Nall[Z]=nlocal[Z]+2*nhalo;


  int nSites=Nall[X]*Nall[Y]*Nall[Z];

  int nFields=nop;




  //start constant setup
  copyConstantInt1DArrayToTarget( (int*) tc_Nall,Nall, 3*sizeof(int)); 
  //end constant setup


  copyToTarget(t_field,field,nSites*nFields*sizeof(double)); 


  //set up sitemask for gradient operation
  memset(siteMask,0,nSites*sizeof(char));

  for (ic = 1 - nextra; ic <= nlocal[X] + nextra; ic++) {
    for (jc = 1 - nextra; jc <= nlocal[Y] + nextra; jc++) {
      for (kc = 1 - nextra; kc <= nlocal[Z] + nextra; kc++) {
	
  	index = le_site_index(le_stat, ic, jc, kc);
  	siteMask[index]=1;
	
      }
    }
  }

  copyToTarget(t_siteMask,siteMask,nSites*sizeof(char)); 


  gradient_3d_7pt_fluid_operator_lattice TARGET_LAUNCH(nSites) 
    (nop,t_field,t_grad,t_del2,t_siteMask);
   

  //for GPU version, we leave the results on the target for the next kernel.
  //for C version, we bring back the results to the host (for now).
  //ultimitely GPU and C versions will follow the same pattern
  #ifndef CUDA
  copyFromTarget(grad,t_grad,3*nSites*nFields*sizeof(double)); 
  copyFromTarget(del2,t_del2,nSites*nFields*sizeof(double)); 
  #endif

  return;
}

/*****************************************************************************
 *
 *  gradient_3d_7pt_le_correction
 *
 *  Additional gradient calculations near LE planes to account for
 *  sliding displacement.
 *
 *****************************************************************************/

HOST static void gradient_3d_7pt_fluid_le_correction(const int nop,
						const double * field,
						double * grad,
						double * del2,
						const int nextra) {
  int nlocal[3];
  int nh;                                 /* counter over halo extent */
  int n, np;
  int nplane;                             /* Number LE planes */
  int ic, jc, kc;
  int ic0, ic1, ic2;                      /* x indices involved */
  int index, indexm1, indexp1;            /* 1d addresses involved */
  int zs, ys, xs;                         /* strides for 1d address */

  /* PENDING */
  le_t * le;
  le = le_stat;

  le_nplane_local(le, &nplane);
  le_nlocal(le, nlocal);
  le_strides(le, &xs, &ys, &zs);

  for (np = 0; np < nplane; np++) {

    ic = le_plane_location(np);

    /* Looking across in +ve x-direction */
    for (nh = 1; nh <= nextra; nh++) {
      ic0 = le_index_real_to_buffer(le, ic, nh-1);
      ic1 = le_index_real_to_buffer(le, ic, nh  );
      ic2 = le_index_real_to_buffer(le, ic, nh+1);

      for (jc = 1 - nextra; jc <= nlocal[Y] + nextra; jc++) {
	for (kc = 1 - nextra; kc <= nlocal[Z] + nextra; kc++) {

	  indexm1 = le_site_index(le, ic0, jc, kc);
	  index   = le_site_index(le, ic1, jc, kc);
	  indexp1 = le_site_index(le, ic2, jc, kc);

	  for (n = 0; n < nop; n++) {
	    grad[3*(nop*index + n) + X]
	      = 0.5*(field[nop*indexp1 + n] - field[nop*indexm1 + n]);
	    grad[3*(nop*index + n) + Y]
	      = 0.5*(field[nop*(index + ys) + n]
		     - field[nop*(index - ys) + n]);
	    grad[3*(nop*index + n) + Z]
	      = 0.5*(field[nop*(index + 1) + n] - field[nop*(index - 1) + n]);
	    del2[nop*index + n]
	      = field[nop*indexp1      + n] + field[nop*indexm1      + n]
	      + field[nop*(index + ys) + n] + field[nop*(index - ys) + n]
	      + field[nop*(index + 1)  + n] + field[nop*(index - 1)  + n]
	      - 6.0*field[nop*index + n];
	  }
	}
      }
    }

    /* Looking across the plane in the -ve x-direction. */
    ic += 1;

    for (nh = 1; nh <= nextra; nh++) {
      ic2 = le_index_real_to_buffer(le, ic, -nh+1);
      ic1 = le_index_real_to_buffer(le, ic, -nh  );
      ic0 = le_index_real_to_buffer(le, ic, -nh-1);

      for (jc = 1 - nextra; jc <= nlocal[Y] + nextra; jc++) {
	for (kc = 1 - nextra; kc <= nlocal[Z] + nextra; kc++) {

	  indexm1 = le_site_index(le, ic0, jc, kc);
	  index   = le_site_index(le, ic1, jc, kc);
	  indexp1 = le_site_index(le, ic2, jc, kc);

	  for (n = 0; n < nop; n++) {
	    grad[3*(nop*index + n) + X]
	      = 0.5*(field[nop*indexp1 + n] - field[nop*indexm1 + n]);
	    grad[3*(nop*index + n) + Y]
	      = 0.5*(field[nop*(index + ys) + n]
		     - field[nop*(index - ys) + n]);
	    grad[3*(nop*index + n) + Z]
	      = 0.5*(field[nop*(index + 1) + n] - field[nop*(index - 1) + n]);
	    del2[nop*index + n]
	      = field[nop*indexp1      + n] + field[nop*indexm1      + n]
	      + field[nop*(index + ys) + n] + field[nop*(index - ys) + n]
	      + field[nop*(index + 1)  + n] + field[nop*(index - 1)  + n]
	      - 6.0*field[nop*index + n];
	  }
	}
      }
    }
    /* Next plane */
  }

  return;
}

/*****************************************************************************
 *
 *  gradient_dab_compute
 *
 *****************************************************************************/

HOST static int gradient_dab_compute(int nf, const double * field, double * dab) {

  int nlocal[3];
  int nhalo;
  int nextra;
  int n;
  int ic, jc, kc;
  int zs, ys, xs;
  int icm1, icp1;
  int index, indexm1, indexp1;

  assert(nf == 1);
  assert(field);
  assert(dab);

  /* PENDING */
  le_t * le = le_stat;
  le_nhalo(le, &nhalo);
  nextra = nhalo - 1;
  assert(nextra >= 0);

  le_nlocal(le, nlocal);
  le_strides(le, &xs, &ys, &zs);

  for (ic = 1 - nextra; ic <= nlocal[X] + nextra; ic++) {
    icm1 = le_index_real_to_buffer(le, ic, -1);
    icp1 = le_index_real_to_buffer(le, ic, +1);
    for (jc = 1 - nextra; jc <= nlocal[Y] + nextra; jc++) {
      for (kc = 1 - nextra; kc <= nlocal[Z] + nextra; kc++) {

	index = le_site_index(le, ic, jc, kc);
	indexm1 = le_site_index(le, icm1, jc, kc);
	indexp1 = le_site_index(le, icp1, jc, kc);

	for (n = 0; n < nf; n++) {
	  dab[NSYMM*(nf*index + n) + XX]
	    = field[nf*indexp1 + n] + field[nf*indexm1 + n]
	    - 2.0*field[nf*index + n];
	  dab[NSYMM*(nf*index + n) + XY] = 0.25*
	    (field[nf*(indexp1 + ys) + n] - field[nf*(indexp1 - ys) + n]
	     - field[nf*(indexm1 + ys) + n] + field[nf*(indexm1 - ys) + n]);
	  dab[NSYMM*(nf*index + n) + XZ] = 0.25*
	    (field[nf*(indexp1 + 1) + n] - field[nf*(indexp1 - 1) + n]
	     - field[nf*(indexm1 + 1) + n] + field[nf*(indexm1 - 1) + n]);

	  dab[NSYMM*(nf*index + n) + YY]
	    = field[nf*(index + ys) + n] + field[nf*(index - ys) + n]
	    - 2.0*field[nf*index + n];
	  dab[NSYMM*(nf*index + n) + YZ] = 0.25*
	    (field[nf*(index + ys + 1) + n] - field[nf*(index + ys - 1) + n]
	   - field[nf*(index - ys + 1) + n] + field[nf*(index - ys - 1) + n]
	     );

	  dab[NSYMM*(nf*index + n) + ZZ]
	    = field[nf*(index + 1)  + n] + field[nf*(index - 1)  + n]
	    - 2.0*field[nf*index + n];
	}
      }
    }
  }

  return 0;
}

/*****************************************************************************
 *
 *  gradient_dab_le_correct
 *
 *****************************************************************************/

HOST static int gradient_dab_le_correct(int nf, const double * field,
				   double * dab) {

  int nlocal[3];
  int nhalo;
  int nextra;
  int nh;                                 /* counter over halo extent */
  int n, np;
  int nplane;                             /* Number LE planes */
  int ic, jc, kc;
  int ic0, ic1, ic2;                      /* x indices involved */
  int index, indexm1, indexp1;            /* 1d addresses involved */
  int zs, ys, xs;                         /* strides for 1d address */

  /* PENDING */
  le_t * le = le_stat;

  le_nplane_local(le, &nplane);
  le_nhalo(le, &nhalo);
  le_nlocal(le, nlocal);
  le_strides(le, &xs, &ys, &zs);

  nextra = nhalo - 1;
  assert(nextra >= 0);

  for (np = 0; np < nplane; np++) {

    ic = le_plane_location(np);

    /* Looking across in +ve x-direction */
    for (nh = 1; nh <= nextra; nh++) {
      ic0 = le_index_real_to_buffer(le, ic, nh-1);
      ic1 = le_index_real_to_buffer(le, ic, nh  );
      ic2 = le_index_real_to_buffer(le, ic, nh+1);

      for (jc = 1 - nextra; jc <= nlocal[Y] + nextra; jc++) {
	for (kc = 1 - nextra; kc <= nlocal[Z] + nextra; kc++) {

	  indexm1 = le_site_index(le, ic0, jc, kc);
	  index   = le_site_index(le, ic1, jc, kc);
	  indexp1 = le_site_index(le, ic2, jc, kc);

	  for (n = 0; n < nf; n++) {
	    dab[NSYMM*(nf*index + n) + XX]
	      = field[nf*indexp1 + n] + field[nf*indexm1 + n]
	      - 2.0*field[nf*index + n];
	    dab[NSYMM*(nf*index + n) + XY] = 0.25*
	      (field[nf*(indexp1 + ys) + n] - field[nf*(indexp1 - ys) + n]
	       - field[nf*(indexm1 + ys) + n] + field[nf*(indexm1 - ys) + n]);
	    dab[NSYMM*(nf*index + n) + XZ] = 0.25*
	      (field[nf*(indexp1 + 1) + n] - field[nf*(indexp1 - 1) + n]
	       - field[nf*(indexm1 + 1) + n] + field[nf*(indexm1 - 1) + n]);

	    dab[NSYMM*(nf*index + n) + YY]
	      = field[nf*(index + ys) + n] + field[nf*(index - ys) + n]
	      - 2.0*field[nf*index + n];
	    dab[NSYMM*(nf*index + n) + YZ] = 0.25*
	      (field[nf*(index + ys + 1) + n] - field[nf*(index + ys - 1) + n]
	     - field[nf*(index - ys + 1) + n] + field[nf*(index - ys - 1) + n]
	       );

	    dab[NSYMM*(nf*index + n) + ZZ]
	      = field[nf*(index + 1)  + n] + field[nf*(index - 1)  + n]
	      - 2.0*field[nf*index + n];
	  }
	}
      }
    }

    /* Looking across the plane in the -ve x-direction. */
    ic += 1;

    for (nh = 1; nh <= nextra; nh++) {
      ic2 = le_index_real_to_buffer(le, ic, -nh+1);
      ic1 = le_index_real_to_buffer(le, ic, -nh  );
      ic0 = le_index_real_to_buffer(le, ic, -nh-1);

      for (jc = 1 - nextra; jc <= nlocal[Y] + nextra; jc++) {
	for (kc = 1 - nextra; kc <= nlocal[Z] + nextra; kc++) {

	  indexm1 = le_site_index(le, ic0, jc, kc);
	  index   = le_site_index(le, ic1, jc, kc);
	  indexp1 = le_site_index(le, ic2, jc, kc);

	  for (n = 0; n < nf; n++) {
	    dab[NSYMM*(nf*index + n) + XX]
	      = field[nf*indexp1 + n] + field[nf*indexm1 + n]
	      - 2.0*field[nf*index + n];
	    dab[NSYMM*(nf*index + n) + XY] = 0.25*
	      (field[nf*(indexp1 + ys) + n] - field[nf*(indexp1 - ys) + n]
	       - field[nf*(indexm1 + ys) + n] + field[nf*(indexm1 - ys) + n]);
	    dab[NSYMM*(nf*index + n) + XZ] = 0.25*
	      (field[nf*(indexp1 + 1) + n] - field[nf*(indexp1 - 1) + n]
	       - field[nf*(indexm1 + 1) + n] + field[nf*(indexm1 - 1) + n]);

	    dab[NSYMM*(nf*index + n) + YY]
	      = field[nf*(index + ys) + n] + field[nf*(index - ys) + n]
	      - 2.0*field[nf*index + n];
	    dab[NSYMM*(nf*index + n) + YZ] = 0.25*
	      (field[nf*(index + ys + 1) + n] - field[nf*(index + ys - 1) + n]
	     - field[nf*(index - ys + 1) + n] + field[nf*(index - ys - 1) + n]
	       );

	    dab[NSYMM*(nf*index + n) + ZZ]
	      = field[nf*(index + 1)  + n] + field[nf*(index - 1)  + n]
	      - 2.0*field[nf*index + n];
	  }
	}
      }
    }
    /* Next plane */
  }

  return 0;
}
