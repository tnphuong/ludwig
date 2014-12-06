/*****************************************************************************
 *
 *  coords.h
 *
 *  $Id: coords.h,v 1.4 2010-10-15 12:40:02 kevin Exp $
 *
 *  Edinburgh Soft Matter and Statistical Physics Group and
 *  Edinburgh Parallel Computing Centre
 *
 *  Kevin Stratford (kevin@epcc.ed.ac.uk)
 *  (c) 2010 The University of Edinburgh
 *
 *****************************************************************************/

#ifndef COORDS_H
#define COORDS_H

#include "pe.h"

typedef struct coords_s coords_t;

#define NSYMM 6      /* Elements for general symmetric tensor */

enum cartesian_directions {X, Y, Z};
enum cartesian_neighbours {FORWARD, BACKWARD};
enum upper_triangle {XX, XY, XZ, YY, YZ, ZZ};

/* New interface */

int coords_create(pe_t * pe, coords_t ** pcoord);
int coords_free(coords_t ** pcoord);
int coords_retain(coords_t * cs);

int coords_decomposition_set(coords_t * cs, const int irequest[3]);
int coords_periodicity_set(coords_t * cs, const int iper[3]);
int coords_ntotal_set(coords_t * cs, const int ntotal[3]);
int coords_nhalo_set(coords_t * cs, int nhalo);
int coords_reorder_set(coords_t * cs, int reorder);
int coords_commit(coords_t * cs);
int coords_info(coords_t * cs);

/* Old interface */

int    is_periodic(const int);
double L(const int);
double Lmin(const int);
int    cart_rank(void);
int    cart_size(const int);
int    cart_coords(const int);
int    cart_neighb(const int direction, const int dimension);

MPI_Comm cart_comm(void);

void   coords_nlocal(int n[3]);
void   coords_nlocal_offset(int n[3]);
int    coords_nhalo(void);
int    coords_ntotal(int ntotal[3]);
int    coords_nsites(void);
int    coords_index(const int ic, const int jc, const int kc);
void   coords_minimum_distance(const double r1[3], const double r2[3],
			       double r12[3]);
void   coords_index_to_ijk(const int index, int coords[3]);
int    coords_strides(int * xs, int * ys, int * zs);
int    coords_periodic_comm(MPI_Comm * comm);
int    coords_cart_shift(MPI_Comm comm, int dim, int direction, int * rank);

#endif