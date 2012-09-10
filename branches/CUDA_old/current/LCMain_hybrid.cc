#ifndef LCMAIN_CC
#define LCMAIN_CC

#include <math.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

#include "LCMain_hybrid.hh"
#include "String.hh"
#include "LCParallel.hh"

void message(const char *);
void update0_ks(double ****, double ****); 
void update_ks(double ****, double ****);
void streamfile_ks(const int);
void writeDiscFile_ks(const int);
void exchangeTau(void);
void writeRestart(const int);
void readRestart(const int);
void do_poiseuille_distributions(double ****);

int main(int argc, char** argv) 
{

  int Nstart;
  int n,graphstp,improv;
  double ****tmp;

  int i,j,k;

#ifdef PARALLEL
    double t0, t1;
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&nbPE);
  MPI_Comm_rank(MPI_COMM_WORLD,&myPE);
#else
  nbPE=1;
  myPE=0; 
#endif

  ifstream inputFile("liquidCrystal.inp");

  if (!inputFile) {
    cout << "Can't open the input file liquidCrystal.inp" << endl;
    return 1;
  }

  String endOfLine;

  inputFile >> Lx >> endOfLine;
  inputFile >> Ly >> endOfLine;
  inputFile >> Lz >> endOfLine;
  inputFile >> Nstart >> endOfLine;
  inputFile >> Nmax >> endOfLine;
  inputFile >> GRAPHICS >> endOfLine;
  inputFile >> stepskip >> endOfLine;
  inputFile >> itimprov >> endOfLine;
  inputFile >> RANDOM >> endOfLine;
  inputFile >> TWIST >> endOfLine;
  inputFile >> O2STRUCT >> endOfLine;
  inputFile >> O5STRUCT >> endOfLine;
  inputFile >> O8STRUCT >> endOfLine;
  inputFile >> O8MSTRUCT >> endOfLine;
  inputFile >> HEXPLANAR >> endOfLine;
  inputFile >> HEX3DA >> endOfLine;
  inputFile >> HEX3DB >> endOfLine;
  inputFile >> DTSTRUCT >> endOfLine;
  inputFile >> BLUEHAAR >> endOfLine;
  inputFile >> L1init >> endOfLine;
  inputFile >> L2init >> endOfLine;
  inputFile >> numuc >> endOfLine;
  inputFile >> numhftwist >> endOfLine;
  inputFile >> Gamma >> endOfLine;
  inputFile >> gam >> endOfLine;
  inputFile >> REDSHIFT >> endOfLine;
  inputFile >> POISEUILLE >> endOfLine;
  inputFile >> bodyforce >> endOfLine;
  inputFile >> BACKFLOW >> endOfLine;
  inputFile >> phivr >> endOfLine;
  inputFile >> NOISE >> endOfLine;
  inputFile >> noise_strength >> endOfLine;
  inputFile >> ACTIVE >> endOfLine;
  inputFile >> zeta >> endOfLine;
  inputFile >> xi >> endOfLine;
  inputFile >> tau1 >> endOfLine;
  inputFile >> tau2 >> endOfLine;
  inputFile >> delVz >> endOfLine;
  inputFile >> epsa >> endOfLine;
  inputFile >> epsav >> endOfLine;
  inputFile >> vwtp >> endOfLine;
  inputFile >> vwbt >> endOfLine;
  inputFile >> bcstren >> endOfLine;
  inputFile >> angztop >> endOfLine;
  inputFile >> angzbot >> endOfLine;
  inputFile >> angxytop >> endOfLine;
  inputFile >> angxybot >> endOfLine;
  inputFile >> Abulk >> endOfLine;
  inputFile >> threshold >> endOfLine;
  inputFile >> OVDPrintInt >> endOfLine;
  inputFile >> FePrintInt >> endOfLine;
  inputFile >> SigPrintFac >> endOfLine;
  inputFile >> numCase >> endOfLine;
  inputFile >> pe_cartesian_size_[0] >> endOfLine;
  inputFile >> pe_cartesian_size_[1] >> endOfLine;
  inputFile >> pe_cartesian_size_[2] >> endOfLine;
  inputFile >> io_ngroups_ >> endOfLine;

  String logFileName("liquidCrystal.");
  logFileName.concat((int) numCase);
  logFileName.concat(".log");
 
   ifstream testlogFile(logFileName.get());

   if(testlogFile){
      testlogFile.close();
      ofstream logFile(logFileName.get(),ios::app);
      if (myPE==0) logFile << "# new run" << endl;
      logFile.close();
   }

  ofstream logFile(logFileName.get(),ios::app);

if (myPE==0){

  logFile << Lx << "\t\t# Lx" << endl;
  logFile << Ly << "\t\t# Ly" << endl;
  logFile << Lz << "\t\t# Lz" << endl;
  logFile << Nstart << "\t\t# Nstart" << endl;
  logFile << Nmax << "\t\t# Nmax" << endl;
  logFile << GRAPHICS << "\t\t# GRAPHICS" << endl;
  logFile << stepskip << "\t\t# stepskip" << endl;
  logFile << itimprov << "\t\t# itimprov" << endl;
  logFile << RANDOM << "\t\t# RANDOM" << endl;
  logFile << TWIST << "\t\t# TWIST" << endl;
  logFile << O2STRUCT << "\t\t# O2STRUCT" << endl;
  logFile << O5STRUCT << "\t\t# O5STRUCT" << endl;
  logFile << O8STRUCT << "\t\t# O8STRUCT"<< endl;
  logFile << O8MSTRUCT << "\t\t# O8MSTRUCT"<< endl;
  logFile << HEXPLANAR << "\t\t# HEXPLANAR"<< endl;
  logFile << HEX3DA << "\t\t# HEX3DA"<< endl;
  logFile << HEX3DB << "\t\t# HEX3DB"<< endl;
  logFile << DTSTRUCT << "\t\t# DTSTRUCT"<< endl;
  logFile << BLUEHAAR << "\t\t# BLUEHAAR"<< endl;
  logFile << L1init << "\t\t# L1"<< endl;
  logFile << L2init << "\t\t# L2"<< endl;
  logFile << numuc << "\t\t# numuc"<< endl;
  logFile << numhftwist << "\t\t# numhftwist"<< endl;
  logFile << Gamma << "\t\t# Gamma"<< endl;
  logFile << gam << "\t\t# gam"<< endl;
  logFile << REDSHIFT << "\t\t# REDSHIFT"<< endl;
  logFile << POISEUILLE << "\t\t# POISEUILLE"<< endl;
  logFile << bodyforce << "\t\t# bodyforce" << endl;
  logFile << BACKFLOW << "\t\t# BACKFLOW"<< endl;
  logFile << phivr << "\t\t# phivr"<< endl;
  logFile << NOISE << "\t\t# NOISE"<< endl;
  logFile << noise_strength << "\t\t# noise_strength"<< endl;
  logFile << ACTIVE << "\t\t# ACTIVE"<< endl;
  logFile << zeta << "\t\t# zeta"<< endl;
  logFile << xi << "\t\t# xi"<< endl;
  logFile << tau1 << "\t\t# tau1"<< endl;
  logFile << tau2 << "\t\t# tau2"<< endl;
  logFile << delVz << "\t\t# delVz"<< endl;
  logFile << epsa << "\t\t# epsa"<< endl;
  logFile << epsav << "\t\t# epsav"<< endl;
  logFile << vwtp << "\t\t# vwtp"<< endl;
  logFile << vwbt << "\t\t# vwbt"<< endl;
  logFile << bcstren << "\t\t# bcstren"<< endl;
  logFile << angztop << "\t\t# angztop"<< endl;
  logFile << angzbot << "\t\t# angzbot"<< endl;
  logFile << angxytop << "\t\t# angxytop"<< endl;
  logFile << angxybot << "\t\t# angxybot"<< endl;
  logFile << Abulk << "\t\t# Abulk"<< endl;
  logFile << threshold << "\t\t# threshold"<< endl;
  logFile << OVDPrintInt << "\t\t# OVDPrintInt"<< endl;
  logFile << FePrintInt << "\t\t# FePrintInt"<< endl;
  logFile << SigPrintFac << "\t\t# SigPrintFac"<< endl;
  logFile << numCase << "\t\t# numCase"<< endl;
  logFile << pe_cartesian_size_[0] << "\t\t# x proc decomposition" << endl;
  logFile << pe_cartesian_size_[1] << "\t\t# y proc decomposition" << endl;
  logFile << pe_cartesian_size_[2] << "\t\t# z proc decomposition" << endl;
  logFile << io_ngroups_ << "\t\t# number io groups (files)" << endl;

   logFile.close();
}

  initialize();

#ifdef PARALLEL
    MPI_Barrier(MPI_COMM_WORLD);
    t0 = MPI_Wtime();
#endif

    seed=1;
    srand48(seed);

#ifdef PARALLEL
    MPI_Comm_rank(MPI_COMM_WORLD, &seed);
    srand48(seed);
#endif


    q0init=numhftwist*numuc*sqrt(2.0)*Pi/Ly;

    if (O2STRUCT || TWIST || DTSTRUCT || BLUEHAAR) {
      q0init=numhftwist*numuc*Pi/Ly;
      L1init=2.0*L1init;
      L2init=2.0*L2init;
    }

    if (HEXPLANAR || HEX3DA || HEX3DB) {
      q0init=numhftwist*numuc*Pi/Lx;
      L1init=2.0*L1init;
      L2init=2.0*L2init;
    }

  q0=q0init;

  reinit();


   String fileName("fe.");
   fileName.concat((int) numCase);
   fileName.concat(".dat");

   ifstream testfile(fileName.get());

   if(testfile){
      testfile.close();
      ofstream file(fileName.get(),ios::app);
      if (myPE==0) file << "# new run" << endl;
      file.close();
   }

   ofstream file(fileName.get(),ios::app);
   file.close();

   lastFreeenergy=-1000000;

  if (BACKFLOW == 1){
      phivr=1.0;
  }

   if (REDSHIFT==1){
      rr=1.0;
   }
   else{
     if(RANDOM) rr=1.0;
     if(O2STRUCT) rr=0.91;
     if(O5STRUCT) rr=0.97;
     if(O8STRUCT) rr=0.82;
     if(O8MSTRUCT) rr=0.83;
   }

   // KS. Note the value of redshift 'rr' also comes from the restart
   // i.e., the read must be here.
   if (Nstart > 0) {
     message("Reading restart files...");
     readRestart(Nstart);
   }

  q0=q0init/rr;
  L1=L1init*rr*rr;
  L2=L2init*rr*rr;
 
  graphstp=0;
  kappa = sqrt(L1*27.0*q0*q0/(Abulk*gam));
  caz = (1.0+4./3.*kappa*kappa);
  tauc = 1.0/8.0*(1-4.0*kappa*kappa+pow(caz,1.5));
  
  if (q0 > 0.00001) 
    aa=(2.0*cos(2.0*Pi/Ly)-2.0+4.0*Pi/Ly*sin(2.0*Pi/Ly))/(Pi/Ly*Pi/Ly);
  else
    aa=4.0;


  for (n=1+Nstart; n<=Nmax; n++) {

	computeStressFreeEnergy(n);

// include code which allows parameter steering during the run 
#include "Controlparameter.cc"

	q0=q0init/rr;
	L1=L1init*rr*rr;
	L2=L2init*rr*rr;

    for (i=ix1; i<ix2; i++) {
      for (j=jy1; j<jy2; j++) {
	  for (k=kz1; k<kz2; k++) {
	  	Qxxold[i][j][k]=Qxx[i][j][k];
	  	Qxyold[i][j][k]=Qxy[i][j][k];
	  	Qxzold[i][j][k]=Qxz[i][j][k];
	  	Qyyold[i][j][k]=Qyy[i][j][k];
	  	Qyzold[i][j][k]=Qyz[i][j][k];
	  }
      }
    }

    parametercalc(n);

    for (i=ix1; i<ix2; i++) {
      for (j=jy1; j<jy2; j++) {
	  for (k=kz1; k<kz2; k++) {

	    if (NOISE==1){
	     DEHxx[i][j][k]+=sqrt(noise_strength)/sqrt(dt)*gasdev(1);
	     DEHxy[i][j][k]+=sqrt(noise_strength)/sqrt(dt)*gasdev(1);
	     DEHxz[i][j][k]+=sqrt(noise_strength)/sqrt(dt)*gasdev(1);
	     DEHyy[i][j][k]+=sqrt(noise_strength)/sqrt(dt)*gasdev(1);
	     DEHyz[i][j][k]+=sqrt(noise_strength)/sqrt(dt)*gasdev(1);
	    }

	  Qxxnew[i][j][k]=Qxxold[i][j][k]+dt*(DEHxx[i][j][k]);
	  Qxynew[i][j][k]=Qxyold[i][j][k]+dt*(DEHxy[i][j][k]);
	  Qxznew[i][j][k]=Qxzold[i][j][k]+dt*(DEHxz[i][j][k]);
	  Qyynew[i][j][k]=Qyyold[i][j][k]+dt*(DEHyy[i][j][k]);
	  Qyznew[i][j][k]=Qyzold[i][j][k]+dt*(DEHyz[i][j][k]);

        DEHxxold[i][j][k]=DEHxx[i][j][k];
        DEHxyold[i][j][k]=DEHxy[i][j][k];
        DEHxzold[i][j][k]=DEHxz[i][j][k];
        DEHyyold[i][j][k]=DEHyy[i][j][k];
        DEHyzold[i][j][k]=DEHyz[i][j][k];

      }
    }
  }

    for (i=ix1; i<ix2; i++) {
      for (j=jy1; j<jy2; j++) {
	 for (k=kz1; k<kz2; k++) {

	  Qxx[i][j][k]=Qxxnew[i][j][k];
	  Qxy[i][j][k]=Qxynew[i][j][k];
	  Qxz[i][j][k]=Qxznew[i][j][k];
	  Qyy[i][j][k]=Qyynew[i][j][k];
	  Qyz[i][j][k]=Qyznew[i][j][k];

      }
    }
  }

    /*corrector*/

    for (improv=0; improv<itimprov; improv++) {

      parametercalc(n);

      for (i=ix1; i<ix2; i++) {
	 for (j=jy1; j<jy2; j++) {
	  for (k=kz1; k<kz2; k++) {

	    if (NOISE==1){
	       DEHxx[i][j][k]+=sqrt(noise_strength)/sqrt(dt)*gasdev(1);
	       DEHxy[i][j][k]+=sqrt(noise_strength)/sqrt(dt)*gasdev(1);
	       DEHxz[i][j][k]+=sqrt(noise_strength)/sqrt(dt)*gasdev(1);
	       DEHyy[i][j][k]+=sqrt(noise_strength)/sqrt(dt)*gasdev(1);
	       DEHyz[i][j][k]+=sqrt(noise_strength)/sqrt(dt)*gasdev(1);
	    }


	    Qxxnew[i][j][k]=Qxxold[i][j][k]+0.5*dt*(DEHxx[i][j][k]+
			  DEHxxold[i][j][k]);
	    Qxynew[i][j][k]=Qxyold[i][j][k]+0.5*dt*(DEHxy[i][j][k]+
			  DEHxyold[i][j][k]);
	    Qxznew[i][j][k]=Qxzold[i][j][k]+0.5*dt*(DEHxz[i][j][k]+
			  DEHxzold[i][j][k]);
	    Qyynew[i][j][k]=Qyyold[i][j][k]+0.5*dt*(DEHyy[i][j][k]+
			  DEHyyold[i][j][k]);
	    Qyznew[i][j][k]=Qyzold[i][j][k]+0.5*dt*(DEHyz[i][j][k]+
			  DEHyzold[i][j][k]);

	  }
	 }
      }


      for (i=ix1; i<ix2; i++) {
	 for (j=jy1; j<jy2; j++) {
	  for (k=kz1; k<kz2; k++) {

	    Qxx[i][j][k]=Qxxnew[i][j][k];
	    Qxy[i][j][k]=Qxynew[i][j][k];
	    Qxz[i][j][k]=Qxznew[i][j][k];
	    Qyy[i][j][k]=Qyynew[i][j][k];
	    Qyz[i][j][k]=Qyznew[i][j][k];

	  }
	 }
      }   

    }

    graphstp++;

    parametercalc(n);    

    equilibriumdist();

  if (n % OVDPrintInt == 0) {

    streamfile_ks(n);
    graphstp=0;
  }

  collisionop();

    /*predictor*/

  update0_ks(fpr, f);
  
    tmp=f;
    f=fpr;
    fpr=tmp;

    /*corrector*/
    for (improv=0; improv<itimprov; improv++) {

      parametercalc(n);

      equilibriumdist();

      update_ks(f, fpr);
    }


    if (n % stepskip == 0 || n ==1) {
	writeDiscFile_ks(n);
    }

  }

  /* Write  restart */

  writeRestart(Nmax);

#ifdef PARALLEL
  t1 = MPI_Wtime();
  total_time_ = t1 - t0;

  if (myPE == 0) {
      cout << "Decomposition: " << pe_cartesian_size_[0] << "," <<
	  pe_cartesian_size_[1] << "," << pe_cartesian_size_[2] << endl;
  }

  cout << endl;
  cout << "[" << myPE << "]" << " Total is " << total_time_ << endl;
  cout << "[" << myPE << "]" << " Exch  is " << total_exch_ << endl;
  cout << "[" << myPE << "]" << " Comm is  " << total_comm_ << endl;
  cout << "[" << myPE << "]" << " I/O is   " << total_io_ << endl;


  MPI_Finalize();
#endif
  
}

/****************************************************************************
 *
 *  Update the distributions.
 *
 *  The tendency terms from the collision are added to the
 *  distributions, and a halo exchange preceeds the
 *  propagation.
 *
 *  Note that fold must be unchanged on exit, hence the subtraction
 *  at the end. 
 *
 *  Pouiseuille boundaries are now in do_pouisuille_distributions().
 *
 ****************************************************************************/


void update0_ks(double ****fnew, double ****fold) {

  int i,j,k,l,imod,jmod,kmod;



  // Add collision tendency terms

  for (i=ix1; i<ix2; i++) {
    for (j=jy1; j<jy2; j++) {
      for (k=kz1; k<kz2; k++) {
	for (l=0; l<15; l++) {
	  fold[i][j][k][l] += dt*Fc[i][j][k][l];
	}
      }
    }
  }

  communicateOldDistributions(fold);

  // 'Pull' propagation

  for (i=ix1; i<ix2; i++) {
    for (j=jy1; j<jy2; j++) {
      for (k=kz1; k<kz2; k++) {
	for (l=0; l<15; l++) {

	  imod = i - e[l][0]; 
	  jmod = j - e[l][1];
	  kmod = k - e[l][2];
	  fnew[i][j][k][l] = fold[imod][jmod][kmod][l];
	}
      }
    }
  }

  if (POISEUILLE == 1) do_poiseuille_distributions(fnew);

  // Reset fold

  for (i=ix1; i<ix2; i++) {
    for (j=jy1; j<jy2; j++) {
      for (k=kz1; k<kz2; k++) {
	for (l=0; l<15; l++) {
	  fold[i][j][k][l] -= dt*Fc[i][j][k][l];
	}
      }
    }
  }

  return;
}

/****************************************************************************
 *
 *  update_ks
 *
 *  Again, quick 3D version of above. Again, fold must be unchanged
 *  on exit.
 *
 ****************************************************************************/

void update_ks(double **** fnew, double **** fold) {

  int i,j,k,l,imod,jmod,kmod;
  double rtau1 = 1.0/tau1;
  double rfac1 = 1.0/oneplusdtover2tau1;

  // Add collision tendency terms

  for (i=ix1; i<ix2; i++) {
    for (j=jy1; j<jy2; j++) {
      for (k=kz1; k<kz2; k++) {
	for (l=0; l<15; l++) {
	  fold[i][j][k][l] += 0.5*dt*Fc[i][j][k][l];
	}
      }
    }
  }

  communicateOldDistributions(fold);

  for (i=ix1; i<ix2; i++) {
    for (j=jy1; j<jy2; j++) {
      for (k=kz1; k<kz2; k++) {
	for (l=0; l<15; l++) {

	  imod = i - e[l][0]; 
	  jmod = j - e[l][1]; 
	  kmod = k - e[l][2];

	  fnew[i][j][k][l] = rfac1*(fold[imod][jmod][kmod][l] +
				    0.5*dt*rtau1*feq[i][j][k][l]);
	}
      }
    }
  }

  if (POISEUILLE == 1) do_poiseuille_distributions(fnew);

  for (i=ix1; i<ix2; i++) {
    for (j=jy1; j<jy2; j++) {
      for (k=kz1; k<kz2; k++) {
	for (l=0; l<15; l++) {
	  fold[i][j][k][l] -= 0.5*dt*Fc[i][j][k][l];
	}
      }
    }
  }

  return;
}

/*****************************************************************************
 *
 *  do_poiseuille_distributions
 *
 *  Poiseuille boundary conditions for distributions.
 *
 *****************************************************************************/

void do_poiseuille_distributions(double **** f) {

  extern int pe_cartesian_coordinates_[3];

  int ix, iy, iz;
  double rb;

  /* Boundary at z = 1 */
  if (pe_cartesian_coordinates_[2] == 0) {

    iz = kz1;
  
    for (ix = ix1; ix < ix2; ix++) {
	 for (iy = jy1; iy < jy2; iy++) {

	   f[ix][iy][iz][5] = f[ix][iy][iz][6];

	   rb = f[ix][iy][iz][0] + f[ix][iy][iz][1] + f[ix][iy][iz][2] +
	     f[ix][iy][iz][3] + f[ix][iy][iz][4] +
	     2.0*(f[ix][iy][iz][6] + f[ix][iy][iz][11] + f[ix][iy][iz][12] +
		  f[ix][iy][iz][13] + f[ix][iy][iz][14]);

	   f[ix][iy][iz][7] =
	     0.25*(-f[ix][iy][iz][1] - f[ix][iy][iz][2] +
		   f[ix][iy][iz][3] + f[ix][iy][iz][4]) +
	     0.25*(-f[ix][iy][iz][11] + f[ix][iy][iz][12] +
		   f[ix][iy][iz][14] + rb*vwbt) + 0.75*f[ix][iy][iz][13];

	   f[ix][iy][iz][8] =
	     0.25*(f[ix][iy][iz][1] - f[ix][iy][iz][2] -
		   f[ix][iy][iz][3] + f[ix][iy][iz][4]) +
	     0.25*(f[ix][iy][iz][11] - f[ix][iy][iz][12] +
		   f[ix][iy][iz][13] + rb*vwbt) + 0.75*f[ix][iy][iz][14];

	   f[ix][iy][iz][9] =
	     0.25*(f[ix][iy][iz][1] + f[ix][iy][iz][2] -
		   f[ix][iy][iz][3] - f[ix][iy][iz][4]) +
	     0.25*(-f[ix][iy][iz][13] + f[ix][iy][iz][14] +
		   f[ix][iy][iz][12] - rb*vwbt) + 0.75*f[ix][iy][iz][11];

	   f[ix][iy][iz][10] =
	     0.25*(-f[ix][iy][iz][1] + f[ix][iy][iz][2] +
		   f[ix][iy][iz][3] - f[ix][iy][iz][4]) +
	     0.25*(f[ix][iy][iz][11] + f[ix][iy][iz][13] -
		   f[ix][iy][iz][14] - rb*vwbt) + 0.75*f[ix][iy][iz][12];

	 }
      }

   }

   /* Boundary at z = Lz */
  if (pe_cartesian_coordinates_[2] == pe_cartesian_size_[2]-1) {

      iz=kz2-1;

      for (ix = ix1; ix < ix2; ix++) {
	 for (iy = jy1; iy < jy2; iy++) {

	   f[ix][iy][iz][6] = f[ix][iy][iz][5];

	   rb = f[ix][iy][iz][0] + f[ix][iy][iz][1] + f[ix][iy][iz][2] +
	     f[ix][iy][iz][3] + f[ix][iy][iz][4] +
	     2.0*(f[ix][iy][iz][5] + f[ix][iy][iz][7] + f[ix][iy][iz][8] +
		  f[ix][iy][iz][9] + f[ix][iy][iz][10]);

	   f[ix][iy][iz][11] =
	     0.25*(-f[ix][iy][iz][1] - f[ix][iy][iz][2] +
		   f[ix][iy][iz][3] + f[ix][iy][iz][4]) +
	     0.25*(-f[ix][iy][iz][7] + f[ix][iy][iz][8] +
		   f[ix][iy][iz][10] + rb*vwtp) + 0.75*f[ix][iy][iz][9];

	   f[ix][iy][iz][12] =
	     0.25*(f[ix][iy][iz][1] - f[ix][iy][iz][2] -
		   f[ix][iy][iz][3] + f[ix][iy][iz][4]) +
	     0.25*(f[ix][iy][iz][7] - f[ix][iy][iz][8] +
		   f[ix][iy][iz][9] + rb*vwtp) + 0.75*f[ix][iy][iz][10];

	   f[ix][iy][iz][13] =
	     0.25*(f[ix][iy][iz][1] + f[ix][iy][iz][2] -
		   f[ix][iy][iz][3] - f[ix][iy][iz][4]) +
	     0.25*(-f[ix][iy][iz][9] + f[ix][iy][iz][10] +
		   f[ix][iy][iz][8] - rb*vwtp) + 0.75*f[ix][iy][iz][7];

	   f[ix][iy][iz][14] =
	     0.25*(-f[ix][iy][iz][1] + f[ix][iy][iz][2] +
		   f[ix][iy][iz][3] - f[ix][iy][iz][4]) +
	     0.25*( f[ix][iy][iz][7] + f[ix][iy][iz][9] -
		  f[ix][iy][iz][10] - rb*vwtp) + 0.75*f[ix][iy][iz][8];
	 }
      }
 
  }

  return;
}

void collisionop(void)
{
  int i,j,k,l;

  for (i=ix1; i<ix2; i++) {
    for (j=jy1; j<jy2; j++) {
      for (k=kz1; k<kz2; k++) {
	for (l=0; l<15; l++) {
	  Fc[i][j][k][l]= (feq[i][j][k][l]-f[i][j][k][l])/tau1; 
	}
      }
    }
  }

// Apply bodyforce 
// The momentum input per unit volume is bodyforce*rho*c/2
// (average between pre- and post-collision); 
// the factor 1/10 is model specific (D3Q15).
// Pressure gradient Dp/Dl=bodyforce*rho*c/2.

if (POISEUILLE == 1){
   for (i=ix1; i<ix2; i++) {
      for (j=jy1; j<jy2; j++) {
	 for (k=kz1; k<kz2; k++) {
	    for (l=0; l<15; l++) {
	    Fc[i][j][k][l] += e[l][1]*0.1*bodyforce*density[i][j][k];
	    }
	 }
      }
   }
}
  return;
}


void equilibriumdist(void)
{
  double A0,A1,A2,B1,B2,C0,C1,C2,D1,D2;
  double G1xx,G2xx,G2xy,G2xz,G2yz,G1yy,G2yy,G1zz,G2zz;
  double dbdtauxb,dbdtauyb,dbdtauzb;
  double rho,Qxxl,Qxyl,Qyyl,Qxzl,Qyzl,usq,udote,omdote;
  double nnxxl,nnyyl;
  double Hxx,Hyy,Hxy,Hxz,Hyz,Qsqxx,Qsqxy,Qsqyy,Qsqzz,Qsqxz,Qsqyz,TrQ2;
  double sigxx,sigyy,sigxy,sigxz,sigyz;
  double Gammap;
  int i,j,k,l,iup,idwn,jup,jdwn,kup,kdwn;


  /* Communication of tau required here */
  exchangeTau();

  for (i=ix1; i<ix2; i++) {
    iup=i+1;
    idwn=i-1;
    for (j=jy1; j<jy2; j++) {
      jup=j+1;
      jdwn=j-1;
      for (k=kz1; k<kz2; k++) {
	kup=k+1;
	kdwn=k-1;

	rho=density[i][j][k];
	Qxxl=Qxx[i][j][k];
	Qxyl=Qxy[i][j][k];
	Qyyl=Qyy[i][j][k];
	Qxzl=Qxz[i][j][k];
	Qyzl=Qyz[i][j][k];

	nnxxl= Qxxl+1.0/3.0;
	nnyyl= Qyyl+1.0/3.0;

	Qsqxx=Qxxl+Qyyl;
	Qsqzz=Qsqxx*Qsqxx+Qxzl*Qxzl+Qyzl*Qyzl;
	Qsqxy=Qxyl*Qsqxx+Qxzl*Qyzl;
	Qsqxz=Qxxl*Qxzl-Qxzl*Qsqxx+Qxyl*Qyzl;
	Qsqyz=Qxyl*Qxzl-Qyzl*Qsqxx+Qyyl*Qyzl;
	Qsqxx=Qxxl*Qxxl+Qxyl*Qxyl+Qxzl*Qxzl;
	Qsqyy=Qyyl*Qyyl+Qxyl*Qxyl+Qyzl*Qyzl;
	TrQ2=Qsqxx+Qsqyy+Qsqzz;

	Hxx= molfieldxx[i][j][k];
	Hxy= molfieldxy[i][j][k];
	Hxz= molfieldxz[i][j][k];
	Hyy= molfieldyy[i][j][k];
	Hyz= molfieldyz[i][j][k];

	Gammap=Gamma;

#ifdef BE     
	sigxx=2.0/3.0*xi*((1.0+3.0*Qxxl)*
		  (Hxx*(1.0-2.0*Qxxl-Qyyl)-Hyy*(Qxxl+2.0*Qyyl)-2.0*Hyz*Qyzl)+
		  (Hxy*Qxyl+Hxz*Qxzl)*(1.0-6.0*Qxxl));
	sigxy=xi*(Hxy*(2.0/3.0+Qxxl-4.0*Qxyl*Qxyl+Qyyl)-
		  Hxx*Qxyl*(-1.0+4.0*Qxxl+2.0*Qyyl)-
		  Hyy*Qxyl*(-1.0+4.0*Qyyl+2.0*Qxxl)+
		  Hxz*(-4.0*Qxyl*Qxzl+Qyzl)+Hyz*(Qxzl-4.0*Qxyl*Qyzl));
	sigyy=2.0/3.0*xi*((1.0+3.0*Qyyl)*
		  (Hyy*(1.0-Qxxl-2.0*Qyyl)-Hxx*(2.0*Qxxl+Qyyl)-2.0*Hxz*Qxzl)+
		  (Hxy*Qxyl+Hyz*Qyzl)*(1.0-6.0*Qyyl));
	sigxz=xi*(Hxz*(2.0/3.0-4.0*Qxzl*Qxzl-Qyyl)-
		  Hxx*Qxzl*(4.0*Qxxl+2.0*Qyyl)-
		  Hyy*Qxzl*(1.0+4.0*Qyyl+2.0*Qxxl)+
		  Hxy*(Qyzl-4.0*Qxyl*Qxzl)+Hyz*(Qxyl-4.0*Qxzl*Qyzl));
	sigyz=xi*(Hyz*(2.0/3.0-4.0*Qyzl*Qyzl-Qxxl)-
		  Hyy*Qyzl*(4.0*Qyyl+2.0*Qxxl)-
		  Hxx*Qyzl*(1.0+4.0*Qxxl+2.0*Qyyl)+
		  Hxy*(Qxzl-4.0*Qxyl*Qyzl)+Hxz*(Qxyl-4.0*Qxzl*Qyzl));
#else
	sigxx=2.0/3.0*xi*Hxx;
	sigxy=2.0/3.0*xi*Hxy;
	sigyy=2.0/3.0*xi*Hyy;
	sigxz=2.0/3.0*xi*Hxz;
	sigyz=2.0/3.0*xi*Hyz;
#endif
        // active contribution to the stress tensor
	 if(ACTIVE==1){
	    sigxx+=zeta*Qxx[i][j][k];
	    sigxy+=zeta*Qxy[i][j][k];
	    sigxz+=zeta*Qxz[i][j][k];
	    sigyy+=zeta*Qxx[i][j][k];
	    sigyz+=zeta*Qxx[i][j][k];
	 }

	dbdtauxb= (tauxy[i][jup][k]-tauxy[i][jdwn][k])/2.0+
	  (tauxz[i][j][kup]-tauxz[i][j][kdwn])/2.0;
	dbdtauyb= -(tauxy[iup][j][k]-tauxy[idwn][j][k])/2.0+
	  (tauyz[i][j][kup]-tauyz[i][j][kdwn])/2.0;
	dbdtauzb= -(tauyz[i][jup][k]-tauyz[i][jdwn][k])/2.0-
	  (tauxz[iup][j][k]-tauxz[idwn][j][k])/2.0;

/* BC: If we have a wall we have to use one-sided derivatives */
#if BC
   if(POISEUILLE==1){
      if (pe_cartesian_coordinates_[2] == 0 && k==kz1){
	  
	 dbdtauxb= (tauxy[i][jup][k]-tauxy[i][jdwn][k])*0.5+
	    (-3.0*tauxz[i][j][k]+4.0*tauxz[i][j][k+1]-tauxz[i][j][k+2])*0.5;
	 dbdtauyb= -(tauxy[iup][j][k]-tauxy[idwn][j][k])*0.5+
	    (-3.0*tauyz[i][j][k]+4.0*tauyz[i][j][k+1]-tauyz[i][j][k+2])*0.5;
	}

      if (pe_cartesian_coordinates_[2] == pe_cartesian_size_[2]-1 && k==kz2-1){
	  
	 dbdtauxb= (tauxy[i][jup][k]-tauxy[i][jdwn][k])*0.5+
	    (3.0*tauxz[i][j][k]-4.0*tauxz[i][j][k-1]+tauxz[i][j][k-2])*0.5;
	 dbdtauyb= -(tauxy[iup][j][k]-tauxy[idwn][j][k])*0.5+
	    (3.0*tauyz[i][j][k]-4.0*tauyz[i][j][k-1]+tauyz[i][j][k-2])*0.5;

	}
   }
#endif

	A2= (rho*temperature)/10.0;
	A1= A2;
	A0= rho-14.0*A2;
	B2= rho/24.0;
	B1= 8.0*B2;
	C2= -rho/24.0;
	C1= 2.0*C2;
	C0= -2.0*rho/3.0;
	D2= rho/16.0;
	D1= 8.0*D2;
	G2xx= phivr*(sigxx)/16.0;
	G2yy= phivr*(sigyy)/16.0;
	G2zz= phivr*(-(sigxx+sigyy))/16.0;
	G2xy= phivr*(sigxy)/16.0;
	G2xz= phivr*(sigxz)/16.0;
	G2yz= phivr*(sigyz)/16.0;
	G1xx= 8.0*G2xx;
	G1yy= 8.0*G2yy;
	G1zz= 8.0*G2zz;
       
	usq=u[i][j][k][0]*u[i][j][k][0]+u[i][j][k][1]*u[i][j][k][1]+
	  u[i][j][k][2]*u[i][j][k][2];
	feq[i][j][k][0]=A0+C0*usq;

	for (l=1; l<=6; l++) {
	  udote=u[i][j][k][0]*e[l][0]+u[i][j][k][1]*e[l][1]+
	    u[i][j][k][2]*e[l][2];

// body force contribution from antisymmetric part of stress tensor and spurious gradient terms
	  omdote=dbdtauxb*e[l][0]+dbdtauyb*e[l][1]+dbdtauzb*e[l][2];
	  omdote+=-phivr*(e[l][0]*Fh[i][j][k][0]+e[l][1]*Fh[i][j][k][1]+e[l][2]*Fh[i][j][k][2]);

	  feq[i][j][k][l]=A1+B1*udote+C1*usq+D1*udote*udote+
	    G1xx*e[l][0]*e[l][0]+G1yy*e[l][1]*e[l][1]+G1zz*e[l][2]*e[l][2]+
	    tau1*omdote/3.0; 
	}

	for (l=7; l<15; l++) {
	  udote=u[i][j][k][0]*e[l][0]+u[i][j][k][1]*e[l][1]+
	    u[i][j][k][2]*e[l][2];
 	  omdote=dbdtauxb*e[l][0]+dbdtauyb*e[l][1]+dbdtauzb*e[l][2];
	  omdote+=-phivr*(e[l][0]*Fh[i][j][k][0]+e[l][1]*Fh[i][j][k][1]+e[l][2]*Fh[i][j][k][2]);
	  feq[i][j][k][l]=A2+B2*udote+C2*usq+D2*udote*udote+
	    G2xx*e[l][0]*e[l][0]+2.0*G2xy*e[l][0]*e[l][1]+
	    2.0*G2xz*e[l][0]*e[l][2]+2.0*G2yz*e[l][1]*e[l][2]+
	    G2yy*e[l][1]*e[l][1]+G2zz*e[l][2]*e[l][2]+tau1*omdote/24.0;
	}	

      }
    }
  }
}


void parametercalc(int n){

  int i,j,k,l,iup,idwn,jup,jdwn,kup,kdwn;
  double dQxxdx,dQxxdy,dQxxdz,dQxydx,dQxydy,dQxydz,dQyydx,dQyydy,dQyydz;
  double dQxzdx,dQxzdy,dQxzdz,dQyzdx,dQyzdy,dQyzdz;
  double dQyxdx,dQyxdy,dQyxdz;
  double dQzxdx,dQzxdy,dQzxdz,dQzydx,dQzydy,dQzydz;
  double dQzzdx,dQzzdy,dQzzdz;
  double trt,trd2Q,TrQ2,TrE2;
  double d2Qxxdxdx,d2Qxxdydy,d2Qxxdxdy,d2Qxxdzdz,d2Qxxdxdz,d2Qxxdydz;
  double d2Qyydxdx,d2Qyydydy,d2Qyydxdy,d2Qyydzdz,d2Qyydxdz,d2Qyydydz;
  double d2Qxydxdx,d2Qxydydy,d2Qxydxdy,d2Qxydzdz,d2Qxydxdz,d2Qxydydz;
  double d2Qxzdxdx,d2Qxzdydy,d2Qxzdxdy,d2Qxzdzdz,d2Qxzdxdz,d2Qxzdydz;
  double d2Qyzdxdx,d2Qyzdydy,d2Qyzdxdy,d2Qyzdzdz,d2Qyzdxdz,d2Qyzdydz;
  double divQx,divQy,divQz;
  double txx,tyy,txy,tyx,tzz,txz,tzx,tyz,tzy;
  double avestress;
  double Qsqxx,Qsqxy,Qsqxz,Qsqyy,Qsqyz,Qsqzz,Qxxl,Qxyl,Qxzl,Qyyl,Qyzl,Qzzl;
  double Hxx,Hyy,Hxy,Hxz,Hyz,TrDQI;
  double duxdx,duxdy,duxdz,duydx,duydy,duydz,duzdx,duzdy,duzdz,Gammap;
  double mDQ4xx,mDQ4xy,mDQ4yy,mDQ4xz,mDQ4yz,mDQ4zz,nnxxl,nnyyl;

  for (i=ix1; i<ix2; i++) {
    for (j=jy1; j<jy2; j++) {
      for (k=kz1; k<kz2; k++) {

	density[i][j][k]=0.0;
        freeenergy=0.0;
        freeenergytwist=0.0;
	avestress=0.0;
	u[i][j][k][0]=0.0;
	u[i][j][k][1]=0.0;
	u[i][j][k][2]=0.0;

	for (l=0; l<15; l++) {
	  density[i][j][k] += f[i][j][k][l];
	  u[i][j][k][0] += f[i][j][k][l]*e[l][0];
	  u[i][j][k][1] += f[i][j][k][l]*e[l][1];
	  u[i][j][k][2] += f[i][j][k][l]*e[l][2];
	}

	u[i][j][k][0]=u[i][j][k][0]/density[i][j][k];
	u[i][j][k][1]=u[i][j][k][1]/density[i][j][k];
	u[i][j][k][2]=u[i][j][k][2]/density[i][j][k];

      }
    }
  }

  exchangeMomentumAndQTensor();

  for (i=ix1; i<ix2; i++) {
    iup=i+1;
    idwn=i-1;
    for (j=jy1; j<jy2; j++) {
      jup=j+1;
      jdwn=j-1;
      for (k=kz1; k<kz2; k++) {
	kup=k+1;
	kdwn=k-1;

/* first order derivative in the bulk */

	dQxxdx=(Qxx[iup][j][k]-Qxx[idwn][j][k])*0.5;

	dQxydx=(Qxy[iup][j][k]-Qxy[idwn][j][k])*0.5;
	dQxzdx=(Qxz[iup][j][k]-Qxz[idwn][j][k])*0.5;

	dQyxdx=dQxydx;
	dQyydx=(Qyy[iup][j][k]-Qyy[idwn][j][k])*0.5;
	dQyzdx=(Qyz[iup][j][k]-Qyz[idwn][j][k])*0.5;

	dQzxdx=dQxzdx;
	dQzydx=dQyzdx;
	dQzzdx=-(dQxxdx+dQyydx);

	dQxxdy=(Qxx[i][jup][k]-Qxx[i][jdwn][k])*0.5;
	dQxydy=(Qxy[i][jup][k]-Qxy[i][jdwn][k])*0.5;
	dQxzdy=(Qxz[i][jup][k]-Qxz[i][jdwn][k])*0.5;

	dQyxdy=dQxydy;
	dQyydy=(Qyy[i][jup][k]-Qyy[i][jdwn][k])*0.5;
	dQyzdy=(Qyz[i][jup][k]-Qyz[i][jdwn][k])*0.5;

	dQzxdy=dQxzdy;
	dQzydy=dQyzdy;
	dQzzdy=-(dQxxdy+dQyydy);


	dQxxdz=(Qxx[i][j][kup]-Qxx[i][j][kdwn])*0.5;
	dQxydz=(Qxy[i][j][kup]-Qxy[i][j][kdwn])*0.5;
	dQxzdz=(Qxz[i][j][kup]-Qxz[i][j][kdwn])*0.5;

	dQyxdz=dQxydz;
	dQyydz=(Qyy[i][j][kup]-Qyy[i][j][kdwn])*0.5;
	dQyzdz=(Qyz[i][j][kup]-Qyz[i][j][kdwn])*0.5;

	dQzxdz=dQxzdz;
	dQzydz=dQyzdz;
	dQzzdz=-(dQxxdz+dQyydz);


/* second order derivative in the bulk */

	d2Qxxdxdx=Qxx[iup][j][k]-2.0*Qxx[i][j][k]+Qxx[idwn][j][k];
	d2Qxxdydy=Qxx[i][jup][k]-2.0*Qxx[i][j][k]+Qxx[i][jdwn][k];
	d2Qxxdzdz=Qxx[i][j][kup]-2.0*Qxx[i][j][k]+Qxx[i][j][kdwn];
	d2Qxxdxdy=(Qxx[iup][jup][k]-Qxx[iup][jdwn][k]-
		   Qxx[idwn][jup][k]+Qxx[idwn][jdwn][k])*0.25;
	d2Qxxdxdz=(Qxx[iup][j][kup]-Qxx[iup][j][kdwn]-
		   Qxx[idwn][j][kup]+Qxx[idwn][j][kdwn])*0.25;
	d2Qxxdydz=(Qxx[i][jup][kup]-Qxx[i][jup][kdwn]-
		   Qxx[i][jdwn][kup]+Qxx[i][jdwn][kdwn])*0.25;

	d2Qyydxdx=Qyy[iup][j][k]-2.0*Qyy[i][j][k]+Qyy[idwn][j][k];
	d2Qyydydy=Qyy[i][jup][k]-2.0*Qyy[i][j][k]+Qyy[i][jdwn][k];
	d2Qyydzdz=Qyy[i][j][kup]-2.0*Qyy[i][j][k]+Qyy[i][j][kdwn];
	d2Qyydxdy=(Qyy[iup][jup][k]-Qyy[iup][jdwn][k]-
		   Qyy[idwn][jup][k]+Qyy[idwn][jdwn][k])*0.25;
	d2Qyydxdz=(Qyy[iup][j][kup]-Qyy[iup][j][kdwn]-
		   Qyy[idwn][j][kup]+Qyy[idwn][j][kdwn])*0.25;
	d2Qyydydz=(Qyy[i][jup][kup]-Qyy[i][jup][kdwn]-
		   Qyy[i][jdwn][kup]+Qyy[i][jdwn][kdwn])*0.25;
	
	d2Qxydxdx=Qxy[iup][j][k]-2.0*Qxy[i][j][k]+Qxy[idwn][j][k];
	d2Qxydydy=Qxy[i][jup][k]-2.0*Qxy[i][j][k]+Qxy[i][jdwn][k];
	d2Qxydzdz=Qxy[i][j][kup]-2.0*Qxy[i][j][k]+Qxy[i][j][kdwn];
	d2Qxydxdy=(Qxy[iup][jup][k]-Qxy[iup][jdwn][k]-
		   Qxy[idwn][jup][k]+Qxy[idwn][jdwn][k])*0.25;
	d2Qxydxdz=(Qxy[iup][j][kup]-Qxy[iup][j][kdwn]-
		   Qxy[idwn][j][kup]+Qxy[idwn][j][kdwn])*0.25;
	d2Qxydydz=(Qxy[i][jup][kup]-Qxy[i][jup][kdwn]-
		   Qxy[i][jdwn][kup]+Qxy[i][jdwn][kdwn])*0.25;

	d2Qxzdxdx=Qxz[iup][j][k]-2.0*Qxz[i][j][k]+Qxz[idwn][j][k];
	d2Qxzdydy=Qxz[i][jup][k]-2.0*Qxz[i][j][k]+Qxz[i][jdwn][k];
	d2Qxzdzdz=Qxz[i][j][kup]-2.0*Qxz[i][j][k]+Qxz[i][j][kdwn];
	d2Qxzdxdy=(Qxz[iup][jup][k]-Qxz[iup][jdwn][k]-
		   Qxz[idwn][jup][k]+Qxz[idwn][jdwn][k])*0.25;
	d2Qxzdxdz=(Qxz[iup][j][kup]-Qxz[iup][j][kdwn]-
		   Qxz[idwn][j][kup]+Qxz[idwn][j][kdwn])*0.25;
	d2Qxzdydz=(Qxz[i][jup][kup]-Qxz[i][jup][kdwn]-
		   Qxz[i][jdwn][kup]+Qxz[i][jdwn][kdwn])*0.25;

	d2Qyzdxdx=Qyz[iup][j][k]-2.0*Qyz[i][j][k]+Qyz[idwn][j][k];
	d2Qyzdydy=Qyz[i][jup][k]-2.0*Qyz[i][j][k]+Qyz[i][jdwn][k];
	d2Qyzdzdz=Qyz[i][j][kup]-2.0*Qyz[i][j][k]+Qyz[i][j][kdwn];
	d2Qyzdxdy=(Qyz[iup][jup][k]-Qyz[iup][jdwn][k]-
		   Qyz[idwn][jup][k]+Qyz[idwn][jdwn][k])*0.25;
	d2Qyzdxdz=(Qyz[iup][j][kup]-Qyz[iup][j][kdwn]-
		   Qyz[idwn][j][kup]+Qyz[idwn][j][kdwn])*0.25;
	d2Qyzdydz=(Qyz[i][jup][kup]-Qyz[i][jup][kdwn]-
		   Qyz[i][jdwn][kup]+Qyz[i][jdwn][kdwn])*0.25;


/* BC: If we have a wall we have to use one-sided derivatives */
#if BC
   if(POISEUILLE==1){
      if (pe_cartesian_coordinates_[2] == 0 && k==kz1){

	  dQxxdz= (-3.0*Qxx[i][j][k]+4.0*Qxx[i][j][k+1]-Qxx[i][j][k+2])*0.5;
	  dQxydz= (-3.0*Qxy[i][j][k]+4.0*Qxy[i][j][k+1]-Qxy[i][j][k+2])*0.5; 
	  dQyydz= (-3.0*Qyy[i][j][k]+4.0*Qyy[i][j][k+1]-Qyy[i][j][k+2])*0.5;
	  dQxzdz= (-3.0*Qxz[i][j][k]+4.0*Qxz[i][j][k+1]-Qxz[i][j][k+2])*0.5; 
	  dQyzdz= (-3.0*Qyz[i][j][k]+4.0*Qyz[i][j][k+1]-Qyz[i][j][k+2])*0.5;

	  d2Qxxdzdz= -Qxx[i][j][k+3]+4.0*Qxx[i][j][k+2]-
	    5.0*Qxx[i][j][kup]+2.0*Qxx[i][j][k];
	  d2Qxydzdz= -Qxy[i][j][k+3]+4.0*Qxy[i][j][k+2]-
	    5.0*Qxy[i][j][kup]+2.0*Qxy[i][j][k];
	  d2Qyydzdz= -Qyy[i][j][k+3]+4.0*Qyy[i][j][k+2]-
	    5.0*Qyy[i][j][kup]+2.0*Qyy[i][j][k];
	  d2Qxzdzdz= -Qxz[i][j][k+3]+4.0*Qxz[i][j][k+2]-
	    5.0*Qxz[i][j][kup]+2.0*Qxz[i][j][k];
	  d2Qyzdzdz= -Qyz[i][j][k+3]+4.0*Qyz[i][j][k+2]-
	    5.0*Qyz[i][j][kup]+2.0*Qyz[i][j][k];

	  d2Qxxdxdz=(-3.0*Qxx[iup][j][k]+4.0*Qxx[iup][j][k+1]-Qxx[iup][j][k+2]+
	      3.0*Qxx[idwn][j][k]-4.0*Qxx[idwn][j][k+1]+Qxx[idwn][j][k+2])*0.25;
	  d2Qxydxdz=(-3.0*Qxy[iup][j][k]+4.0*Qxy[iup][j][k+1]-Qxy[iup][j][k+2]+
	      3.0*Qxy[idwn][j][k]-4.0*Qxy[idwn][j][k+1]+Qxy[idwn][j][k+2])*0.25;
	  d2Qyydxdz=(-3.0*Qyy[iup][j][k]+4.0*Qyy[iup][j][k+1]-Qyy[iup][j][k+2]+
	      3.0*Qyy[idwn][j][k]-4.0*Qyy[idwn][j][k+1]+Qyy[idwn][j][k+2])*0.25;
	  d2Qxzdxdz=(-3.0*Qxz[iup][j][k]+4.0*Qxz[iup][j][k+1]-Qxz[iup][j][k+2]+
	      3.0*Qxz[idwn][j][k]-4.0*Qxz[idwn][j][k+1]+Qxz[idwn][j][k+2])*0.25;
	  d2Qyzdxdz=(-3.0*Qyz[iup][j][k]+4.0*Qyz[iup][j][k+1]-Qyz[iup][j][k+2]+
	      3.0*Qyz[idwn][j][k]-4.0*Qyz[idwn][j][k+1]+Qyz[idwn][j][k+2])*0.25;

	  d2Qxxdydz=(-3.0*Qxx[i][jup][k]+4.0*Qxx[i][jup][k+1]-Qxx[i][jup][k+2]+
	      3.0*Qxx[i][jdwn][k]-4.0*Qxx[i][jdwn][k+1]+Qxx[i][jdwn][k+2])*0.25;
	  d2Qxydydz=(-3.0*Qxy[i][jup][k]+4.0*Qxy[i][jup][k+1]-Qxy[i][jup][k+2]+
	      3.0*Qxy[i][jdwn][k]-4.0*Qxy[i][jdwn][k+1]+Qxy[i][jdwn][k+2])*0.25;
	  d2Qyydydz=(-3.0*Qyy[i][jup][k]+4.0*Qyy[i][jup][k+1]-Qyy[i][jup][k+2]+
	      3.0*Qyy[i][jdwn][k]-4.0*Qyy[i][jdwn][k+1]+Qyy[i][jdwn][k+2])*0.25;
	  d2Qxzdydz=(-3.0*Qxz[i][jup][k]+4.0*Qxz[i][jup][k+1]-Qxz[i][jup][k+2]+
	      3.0*Qxz[i][jdwn][k]-4.0*Qxz[i][jdwn][k+1]+Qxz[i][jdwn][k+2])*0.25;
	  d2Qyzdydz=(-3.0*Qyz[i][jup][k]+4.0*Qyz[i][jup][k+1]-Qyz[i][jup][k+2]+
	      3.0*Qyz[i][jdwn][k]-4.0*Qyz[i][jdwn][k+1]+Qyz[i][jdwn][k+2])*0.25;

	}

      if (pe_cartesian_coordinates_[2] == pe_cartesian_size_[2]-1 && k==kz2-1){

	  dQxxdz=(3.0*Qxx[i][j][k]-4.0*Qxx[i][j][k-1]+Qxx[i][j][k-2])*0.5;
	  dQxydz=(3.0*Qxy[i][j][k]-4.0*Qxy[i][j][k-1]+Qxy[i][j][k-2])*0.5; 
	  dQyydz=(3.0*Qyy[i][j][k]-4.0*Qyy[i][j][k-1]+Qyy[i][j][k-2])*0.5;
	  dQxzdz=(3.0*Qxz[i][j][k]-4.0*Qxz[i][j][k-1]+Qxz[i][j][k-2])*0.5; 
	  dQyzdz=(3.0*Qyz[i][j][k]-4.0*Qyz[i][j][k-1]+Qyz[i][j][k-2])*0.5;

	  d2Qxxdzdz= -Qxx[i][j][k-3]+4.0*Qxx[i][j][k-2]-
	    5.0*Qxx[i][j][kdwn]+2.0*Qxx[i][j][k];
	  d2Qxydzdz= -Qxy[i][j][k-3]+4.0*Qxy[i][j][k-2]-
	    5.0*Qxy[i][j][kdwn]+2.0*Qxy[i][j][k];
	  d2Qyydzdz= -Qyy[i][j][k-3]+4.0*Qyy[i][j][k-2]-
	    5.0*Qyy[i][j][kdwn]+2.0*Qyy[i][j][k];
	  d2Qxzdzdz= -Qxz[i][j][k-3]+4.0*Qxz[i][j][k-2]-
	    5.0*Qxz[i][j][kdwn]+2.0*Qxz[i][j][k];
	  d2Qyzdzdz= -Qyz[i][j][k-3]+4.0*Qyz[i][j][k-2]-
	    5.0*Qyz[i][j][kdwn]+2.0*Qyz[i][j][k];

	  d2Qxxdxdz=(3.0*Qxx[iup][j][k]-4.0*Qxx[iup][j][k-1]+Qxx[iup][j][k-2]-
	      3.0*Qxx[idwn][j][k]+4.0*Qxx[idwn][j][k-1]-Qxx[idwn][j][k-2])*0.25;
	  d2Qxydxdz=(3.0*Qxy[iup][j][k]-4.0*Qxy[iup][j][k-1]+Qxy[iup][j][k-2]-
	      3.0*Qxy[idwn][j][k]+4.0*Qxy[idwn][j][k-1]-Qxy[idwn][j][k-2])*0.25;
	  d2Qyydxdz=(3.0*Qyy[iup][j][k]-4.0*Qyy[iup][j][k-1]+Qyy[iup][j][k-2]-
	      3.0*Qyy[idwn][j][k]+4.0*Qyy[idwn][j][k-1]-Qyy[idwn][j][k-2])*0.25;
	  d2Qxzdxdz=(3.0*Qxz[iup][j][k]-4.0*Qxz[iup][j][k-1]+Qxz[iup][j][k-2]-
	      3.0*Qxz[idwn][j][k]+4.0*Qxz[idwn][j][k-1]-Qxz[idwn][j][k-2])*0.25;
	  d2Qyzdxdz=(3.0*Qyz[iup][j][k]-4.0*Qyz[iup][j][k-1]+Qyz[iup][j][k-2]-
	      3.0*Qyz[idwn][j][k]+4.0*Qyz[idwn][j][k-1]-Qyz[idwn][j][k-2])*0.25;

	  d2Qxxdydz=(3.0*Qxx[i][jup][k]-4.0*Qxx[i][jup][k-1]+Qxx[i][jup][k-2]-
	      3.0*Qxx[i][jdwn][k]+4.0*Qxx[i][jdwn][k-1]-Qxx[i][jdwn][k-2])*0.25;
	  d2Qxydydz=(3.0*Qxy[i][jup][k]-4.0*Qxy[i][jup][k-1]+Qxy[i][jup][k-2]-
	      3.0*Qxy[i][jdwn][k]+4.0*Qxy[i][jdwn][k-1]-Qxy[i][jdwn][k-2])*0.25;
	  d2Qyydydz=(3.0*Qyy[i][jup][k]-4.0*Qyy[i][jup][k-1]+Qyy[i][jup][k-2]-
	      3.0*Qyy[i][jdwn][k]+4.0*Qyy[i][jdwn][k-1]-Qyy[i][jdwn][k-2])*0.25;
	  d2Qxzdydz=(3.0*Qxz[i][jup][k]-4.0*Qxz[i][jup][k-1]+Qxz[i][jup][k-2]-
	      3.0*Qxz[i][jdwn][k]+4.0*Qxz[i][jdwn][k-1]-Qxz[i][jdwn][k-2])*0.25;
	  d2Qyzdydz=(3.0*Qyz[i][jup][k]-4.0*Qyz[i][jup][k-1]+Qyz[i][jup][k-2]-
	      3.0*Qyz[i][jdwn][k]+4.0*Qyz[i][jdwn][k-1]-Qyz[i][jdwn][k-2])*0.25;

	}
   }
#endif

	duydz=(u[i][j][kup][1]-u[i][j][kdwn][1])*0.5;
	
/*BC: If we have a wall we have to use one-sided derivatives */
#if BC
   if(POISEUILLE==1){
      if (pe_cartesian_coordinates_[2] == 0 && k==kz1){
	 duydz= (-3.0*u[i][j][k][1]+4.0*u[i][j][k+1][1]-u[i][j][k+2][1])*0.5;
      }

      if (pe_cartesian_coordinates_[2] == pe_cartesian_size_[2]-1 && k==kz2-1){
	 duydz= (3.0*u[i][j][k][1]-4.0*u[i][j][k-1][1]+u[i][j][k-2][1])*0.5;	  
      }
   }
#endif

     txx=txy=txz=tyy=tyx=tyz=tzz=tzy=tzx=0;

#ifdef CHOL

     txx = 4*q0*L1*(dQxydz-dQxzdy);
     txy = 4*q0*L1*(dQyydz-dQyzdy);
     txz = 4*q0*L1*(dQyzdz+dQxxdy+dQyydy);

     tyy = 4*q0*L1*(dQyzdx-dQxydz);
     tyx = 4*q0*L1*(dQxzdx-dQxxdz);
     tyz = 4*q0*L1*(dQzzdx-dQxzdz);

     tzz = 4*q0*L1*(dQxzdy-dQyzdx);
     tzy = 4*q0*L1*(dQxydy-dQyydx);
     tzx = 4*q0*L1*(dQxxdy-dQxydx);


     trt=txx+tyy+tzz;

     trd2Q=d2Qxxdxdx+2.0*d2Qxydxdy+2.0*d2Qxzdxdz+2.0*d2Qyzdydz
      +d2Qyydydy-d2Qxxdzdz-d2Qyydzdz;

#endif

	/* second order derivative contribution to H */

	DEHxx[i][j][k] = L1 * (d2Qxxdxdx+d2Qxxdydy+d2Qxxdzdz)
          +(L2 - L1)/2.0*(2.0*d2Qxxdxdx+2.0*d2Qxydxdy+2.0*d2Qxzdxdz
          - 2.0/3.0*(trd2Q) ) 
          + txx -1.0/3.0 * trt ;

	DEHxy[i][j][k] = L1 * (d2Qxydxdx+d2Qxydydy+d2Qxydzdz)
          + (L2 - L1)/2.0 * (d2Qxydxdx+d2Qyydxdy+d2Qyzdxdz
          +d2Qxxdxdy+d2Qxydydy+d2Qxzdydz)
          + (txy + tyx)*0.5 ;

	DEHxz[i][j][k] = (L2-L1)/2.0 * (d2Qxzdxdx+d2Qyzdxdy-d2Qxxdxdz
          -d2Qyydxdz+d2Qxxdxdz+d2Qxydydz+d2Qxzdzdz)
          + L1 * (d2Qxzdxdx+d2Qxzdydy+d2Qxzdzdz)
          + (txz + tzx)*0.5;

	DEHyy[i][j][k]= L1 * (d2Qyydxdx+d2Qyydydy+d2Qyydzdz)
          + (L2 - L1)/2.0 * (2.0*(d2Qxydxdy+d2Qyydydy+d2Qyzdydz)
          -2.0/3.0*trd2Q)
          + tyy -1.0/3.0 * trt;

	DEHyz[i][j][k] = L1 * (d2Qyzdxdx+d2Qyzdydy+d2Qyzdzdz)
          + (L2 - L1)/2.0 * (d2Qxzdxdy+d2Qyzdydy-d2Qxxdydz
          -d2Qyydydz+d2Qxydxdz+d2Qyydydz+d2Qyzdzdz) 
          + (tyz + tzy)*0.5;

	divQx=dQxxdx+dQxydy+dQxzdz;
	divQy=dQxydx+dQyydy+dQyzdz;
	divQz=dQxzdx+dQyzdy-dQxxdz-dQyydz;

      Qxxl=Qxx[i][j][k];
      Qxyl=Qxy[i][j][k];
      Qyyl=Qyy[i][j][k];
      Qxzl=Qxz[i][j][k];
      Qyzl=Qyz[i][j][k];
      Qzzl=-Qxxl-Qyyl;

      nnxxl= Qxxl+1.0/3.0;
      nnyyl= Qyyl+1.0/3.0;

      Qsqxx=Qxxl+Qyyl;
      Qsqzz=Qsqxx*Qsqxx+Qxzl*Qxzl+Qyzl*Qyzl;
      Qsqxy=Qxyl*Qsqxx+Qxzl*Qyzl;
      Qsqxz=Qxxl*Qxzl-Qxzl*Qsqxx+Qxyl*Qyzl;
      Qsqyz=Qxyl*Qxzl-Qyzl*Qsqxx+Qyyl*Qyzl;
      Qsqxx=Qxxl*Qxxl+Qxyl*Qxyl+Qxzl*Qxzl;
      Qsqyy=Qyyl*Qyyl+Qxyl*Qxyl+Qyzl*Qyzl;
             
#ifdef EON
	/* calculate E-field */
	Ex[i][j][k]= 0.0;
	Ey[i][j][k]= 0.0;
	Ez[i][j][k]= delVz/(-1.0+Lz);

	/* E-field contribution to H; flexoelectric term would contribute here*/
	
	TrE2 = Inv12Pi*epsa*(Ex[i][j][k]*Ex[i][j][k]+Ey[i][j][k]*Ey[i][j][k]+
			     Ez[i][j][k]*Ez[i][j][k]);
	DEHxx[i][j][k] += Inv12Pi*epsa*Ex[i][j][k]*Ex[i][j][k]-TrE2/3.0; 
	DEHxy[i][j][k] += Inv12Pi*epsa*Ex[i][j][k]*Ey[i][j][k];  
	DEHyy[i][j][k] += Inv12Pi*epsa*Ey[i][j][k]*Ey[i][j][k]-TrE2/3.0;
	DEHxz[i][j][k] += Inv12Pi*epsa*Ex[i][j][k]*Ez[i][j][k];
	DEHyz[i][j][k] += Inv12Pi*epsa*Ey[i][j][k]*Ez[i][j][k];
#endif

	TrQ2=Qsqxx+Qsqyy+Qsqzz;

	Hxx= Abulk*(-(1.0-gam/3.0)*Qxxl+gam*(Qsqxx-TrQ2/3.0)-
             gam*Qxxl*TrQ2)-aa*L1*q0*q0*Qxxl+DEHxx[i][j][k];
	Hxy= Abulk*(-(1.0-gam/3.0)*Qxyl+gam*Qsqxy-gam*Qxyl*TrQ2)
	  -aa*L1*q0*q0*Qxyl+DEHxy[i][j][k];
	Hyy= Abulk*(-(1.0-gam/3.0)*Qyyl+gam*(Qsqyy-TrQ2/3.0)-
             gam*Qyyl*TrQ2)-aa*L1*q0*q0*Qyyl+DEHyy[i][j][k];
	Hxz= Abulk*(-(1.0-gam/3.0)*Qxzl+gam*Qsqxz-gam*Qxzl*TrQ2)
	  -aa*L1*q0*q0*Qxzl+DEHxz[i][j][k];
	Hyz= Abulk*(-(1.0-gam/3.0)*Qyzl+gam*Qsqyz-gam*Qyzl*TrQ2)
	  -aa*L1*q0*q0*Qyzl+DEHyz[i][j][k];


/* Qiibot and Qiitop for refer to homeotropic anchoring */
#ifdef FIXEDQ
if(RANDOM==1){
  if (pe_cartesian_coordinates_[2] == 0 && k==kz1) {
       Hxx= -bcstren*(Qxxl-Qxxbot);
       Hxy= -bcstren*(Qxyl-Qxybot);
       Hyy= -bcstren*(Qyyl-Qyybot);
       Hxz= -bcstren*(Qxzl-Qxzbot);
       Hyz= -bcstren*(Qyzl-Qyzbot);
   }

  if (pe_cartesian_coordinates_[2] == pe_cartesian_size_[2]-1 && k==kz2-1) {
       Hxx= -bcstren*(Qxxl-Qxxtop);
       Hxy= -bcstren*(Qxyl-Qxytop);
       Hyy= -bcstren*(Qyyl-Qyytop);
       Hxz= -bcstren*(Qxzl-Qxztop);
       Hyz= -bcstren*(Qyzl-Qyztop);
   }
}
#endif 

	molfieldxx[i][j][k]=Hxx;
	molfieldxy[i][j][k]=Hxy;
	molfieldxz[i][j][k]=Hxz;
	molfieldyy[i][j][k]=Hyy;
	molfieldyz[i][j][k]=Hyz;

// Fh for body force contribution of spurious gradient terms
	  
	Fh[i][j][k][0]=Hxx*dQxxdx+2.0*Hxy*dQxydx
	  +2.0*Hxz*dQxzdx+Hyy*dQyydx+2.0*Hyz*dQyzdx
	  +(-Hyy-Hxx)*(-dQxxdx-dQyydx);
	Fh[i][j][k][1]=Hxx*dQxxdy+2.0*Hxy*dQxydy
	  +2.0*Hxz*dQxzdy+Hyy*dQyydy+2.0*Hyz*dQyzdy
	  +(-Hyy-Hxx)*(-dQxxdy-dQyydy);
	Fh[i][j][k][2]=Hxx*dQxxdz+2.0*Hxy*dQxydz
	  +2.0*Hxz*dQxzdz+Hyy*dQyydz+2.0*Hyz*dQyzdz
	  +(-Hyy-Hxx)*(-dQxxdz-dQyydz);
 
/* At the walls Fh is set to 0 for Poiseuille flow */
#ifdef FIXEDQ
     if(POISEUILLE==1){
	if (pe_cartesian_coordinates_[2] == 0 && k==kz1) {
	 Fh[i][j][k][0]=0.0;
	 Fh[i][j][k][1]=0.0;
	 Fh[i][j][k][2]=0.0;
	}
	if (pe_cartesian_coordinates_[2] == pe_cartesian_size_[2]-1 && k==kz2-1) {
	 Fh[i][j][k][0]=0.0;
	 Fh[i][j][k][1]=0.0;
	 Fh[i][j][k][2]=0.0;
	}
    }
#endif
	/* antisymmetric part of the stress tensor */

	tauxy[i][j][k]= -1.0*phivr*
	  (Qxy[i][j][k]*(Hxx-Hyy)-
	   Hxy*(Qxx[i][j][k]-Qyy[i][j][k])+
	   Hxz*Qyz[i][j][k]-Hyz*Qxz[i][j][k]);
	tauxz[i][j][k]= -1.0*phivr*
	  (Qxz[i][j][k]*(2.0*Hxx+Hyy)-
	   Hxz*(2.0*Qxx[i][j][k]+Qyy[i][j][k])+
	   Hxy*Qyz[i][j][k]-Hyz*Qxy[i][j][k]);
	tauyz[i][j][k]= -1.0*phivr*
	  (Qyz[i][j][k]*(2.0*Hyy+Hxx)-
	   Hyz*(2.0*Qyy[i][j][k]+Qxx[i][j][k])+
	   Hxy*Qxz[i][j][k]-Hxz*Qxy[i][j][k]);

	duxdx=(u[iup][j][k][0]-u[idwn][j][k][0])*0.5;
	duxdy=(u[i][jup][k][0]-u[i][jdwn][k][0])*0.5;
	duxdz=(u[i][j][kup][0]-u[i][j][kdwn][0])*0.5;
	duydx=(u[iup][j][k][1]-u[idwn][j][k][1])*0.5;
	duydy=(u[i][jup][k][1]-u[i][jdwn][k][1])*0.5;
	duydz=(u[i][j][kup][1]-u[i][j][kdwn][1])*0.5;
	duzdx=(u[iup][j][k][2]-u[idwn][j][k][2])*0.5;
	duzdy=(u[i][jup][k][2]-u[i][jdwn][k][2])*0.5;
	duzdz=(u[i][j][kup][2]-u[i][j][kdwn][2])*0.5;

/*BC: If we have a wall we have to use one-sided derivatives */
#if BC
   if(POISEUILLE==1){
      if (pe_cartesian_coordinates_[2] == 0 && k==kz1){

	 duxdz= (-3.0*u[i][j][k][0]+4.0*u[i][j][k+1][0]-u[i][j][k+2][0])*0.5;
	 duydz= (-3.0*u[i][j][k][1]+4.0*u[i][j][k+1][1]-u[i][j][k+2][1])*0.5;
	 duzdz= (-3.0*u[i][j][k][2]+4.0*u[i][j][k+1][2]-u[i][j][k+2][2])*0.5;
	  
      }

      if (pe_cartesian_coordinates_[2] == pe_cartesian_size_[2]-1 && k==kz2-1){

	 duxdz= (3.0*u[i][j][k][0]-4.0*u[i][j][k-1][0]+u[i][j][k-2][0])*0.5;
	 duydz= (3.0*u[i][j][k][1]-4.0*u[i][j][k-1][1]+u[i][j][k-2][1])*0.5;
	 duzdz= (3.0*u[i][j][k][2]-4.0*u[i][j][k-1][2]+u[i][j][k-2][2])*0.5;
	  
      }

   }
#endif

           Gammap=Gamma;

	/* -(Q+1/3) Tr(D.(Q+1/3)) term*/
	TrDQI= -(duxdx+duydy+duzdz)/3.0-
	  (Qxxl*duxdx+Qyyl*duydy-(Qxxl+Qyyl)*duzdz+Qxyl*(duxdy+duydx)+
	   Qxzl*(duxdz+duzdx)+Qyzl*(duydz+duzdy));
	mDQ4xy=Qxyl*TrDQI;
	mDQ4yy=nnyyl*TrDQI;
	mDQ4xz=Qxzl*TrDQI;
	mDQ4yz=Qyzl*TrDQI;
	mDQ4zz= (-Qxxl-Qyyl+1.0/3.0)*TrDQI;
	mDQ4xx=nnxxl*TrDQI;

	  Hxx=Gammap*Hxx+duxdy*Qxyl*(1.0+xi)+duxdx*2.0*nnxxl*xi+
	  duydx*Qxyl*(xi-1.0)+duxdz*Qxzl*(1.0+xi)+duzdx*Qxzl*(xi-1.0)+
	  2.0*xi*mDQ4xx;
	Hxy=Gammap*Hxy+0.5*(nnxxl*(xi-1.0)+nnyyl*(1.0+xi))*duxdy+
	  Qxyl*xi*(duydy+duxdx)+0.5*(nnxxl*(1.0+xi)+nnyyl*(xi-1.0))*duydx+
	  0.5*duxdz*Qyzl*(1.0+xi)+0.5*duzdx*Qyzl*(xi-1.0)+
	  0.5*duydz*Qxzl*(1.0+xi)+0.5*duzdy*Qxzl*(xi-1.0)+2.0*xi*mDQ4xy;
	Hxz=Gammap*Hxz+(-Qxxl-0.5*Qyyl*(1.0+xi)+xi/3.0)*duxdz+
	  Qxzl*xi*(duxdx+duzdz)+duzdx*(Qxxl+0.5*Qyyl*(1.0-xi)+xi/3.0)+
	  0.5*Qyzl*((1.0+xi)*duxdy+(xi-1.0)*duydx)+
	  0.5*Qxyl*((xi-1.0)*duydz+(1.0+xi)*duzdy)+2.0*xi*mDQ4xz;
	Hyy=Gammap*Hyy+duxdy*Qxyl*(xi-1.0)+duydy*2.0*nnyyl*xi+
	  duydx*Qxyl*(xi+1.0)+duydz*Qyzl*(1.0+xi)+duzdy*Qyzl*(xi-1.0)+
	  2.0*xi*mDQ4yy;
	Hyz=Gammap*Hyz+(-Qyyl-0.5*Qxxl*(1.0+xi)+xi/3.0)*duydz+
	  Qyzl*xi*(duydy+duzdz)+duzdy*(Qyyl+0.5*Qxxl*(1.0-xi)+xi/3.0)+
	  0.5*Qxzl*((1.0+xi)*duydx+(xi-1.0)*duxdy)+
	  0.5*Qxyl*((xi-1.0)*duxdz+(1.0+xi)*duzdx)+2.0*xi*mDQ4yz;

	Hxx-=u[i][j][k][0]*dQxxdx+u[i][j][k][1]*dQxxdy+u[i][j][k][2]*dQxxdz;
	Hxy-=u[i][j][k][0]*dQxydx+u[i][j][k][1]*dQxydy+u[i][j][k][2]*dQxydz;
	Hxz-=u[i][j][k][0]*dQxzdx+u[i][j][k][1]*dQxzdy+u[i][j][k][2]*dQxzdz;
	Hyy-=u[i][j][k][0]*dQyydx+u[i][j][k][1]*dQyydy+u[i][j][k][2]*dQyydz;
	Hyz-=u[i][j][k][0]*dQyzdx+u[i][j][k][1]*dQyzdy+u[i][j][k][2]*dQyzdz;

	DEHxx[i][j][k]= Hxx;
	DEHxy[i][j][k]= Hxy;
	DEHxz[i][j][k]= Hxz;
	DEHyy[i][j][k]= Hyy;
	DEHyz[i][j][k]= Hyz;

      }
    }
  }

}
 
#include "Initialization.cc"
#include "fe_stress.cc"
#include "Various.cc"
#include "nr.cc"

void message(const char * s) {

    if (myPE == 0) {
	cout << s << endl;
    }
}

#endif