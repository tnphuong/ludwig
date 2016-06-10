/*****************************************************************************
 *
 *  gradient_2d_tomita_fluid.h
 *
 *  $Id$
 *
 *  Edinburgh Soft Matter and Statistical Physics Group and
 *  Edinburgh Parallel Computing Centre
 *
 *  Kevin Stratford (kevin@epcc.ed.ac.uk)
 *  (c) 2011-2016 The University of Edinburgh
 *
 *****************************************************************************/

#ifndef GRADIENT_2D_TOMITA_FLUID_H
#define GRADIENT_2D_TOMITA_FLUID_H

#include "field_grad.h"

__host__ int grad_2d_tomita_fluid_d2(field_grad_t * fgrad);
__host__ int grad_2d_tomita_fluid_d4(field_grad_t * fgrad);

#endif
