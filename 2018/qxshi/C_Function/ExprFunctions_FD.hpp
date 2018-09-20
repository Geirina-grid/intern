/******************************************************************************
 * Copyright (c) 2015-2016, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL Query Language
 * udf.hpp: a library of user defined functions used in queries.
 *
 * - This library should only define functions that will be used in
 *   GraphSQL Query scripts. Other logics, such as structs and helper
 *   functions that will not be directly called in the GQuery scripts,
 *   must be put into "ExprUtil.hpp" under the same directory where
 *   this file is located.
 *
 * - Supported type of return value and parameters
 *     - int
 *     - float
 *     - double
 *     - bool
 *     - string (don't use std::string)
 *     - accumulators
 *
 * - Function names are case sensitive, unique, and can't be conflict with
 *   built-in math functions and reserve keywords.
 *
 * - Please don't remove necessary codes in this file
 *
 * - A backup of this file can be retrieved at
 *     <graphsql_root_path>/dev_<backup_time>/gdk/gsql/src/QueryUdf/ExprFunctions.hpp
 *   after upgrading the system.
 *
 ******************************************************************************/

#ifndef EXPRFUNCTIONS_HPP_
#define EXPRFUNCTIONS_HPP_

#define MAX_VERTICES 15000

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <algorithm> // Header file needed for ListAccum sorting
#include <gle/engine/cpplib/headers.hpp>
#include <tuple> // for accessing elements in tuple
#include <list>
#include <vector>

/**     XXX Warning!! Put self-defined struct in ExprUtil.hpp **
 *  No user defined struct, helper functions (that will not be directly called
 *  in the GQuery scripts) etc. are allowed in this file. This file only
 *  contains user-defined expression function's signature and body.
 *  Please put user defined structs, helper functions etc. in ExprUtil.hpp
 */
#include "ExprUtil.hpp"

#include "ca_linedetail.hpp"
//// ============= Header files needed for NISCLU =============
#include "graphlu.h"
#include "graphlu_util.h"
#include <ctime>
#include <unistd.h>
#include <sys/time.h>
#include <unordered_set> // use for ILU filter
#include <iostream>
// #include <chrono>
// typedef std::chrono::high_resolution_clock Clock;
//// ==========================================================
//


typedef std::string string; //XXX DON'T REMOVE

/****** BIULT-IN FUNCTIONS **************/
/****** XXX DON'T REMOVE ****************/
inline int str_to_int (string str) {
  return atoi(str.c_str());
}

inline int float_to_int (float val) {
  return (int) val;
}

inline string ToString (double val) {
  char result[200];
  sprintf(result, "%g", val);
  return string(result);
}
/****************************************/



// // [Chen Yuan] Testing
// inline void static_C_testing1(){
	// //list<LU_data> testing = new list<LU_data>();
	// testing.push_back(1,1,0.03);
	// std::cout << "teting " << testing << std::endl;
// }


// inline void static_C_testing2(){
	// std::cout << "testing " << testing << std::endl;
// }


// [Chen Yuan] gain matrix sorting
template <typename T_gainmatrix> //typename vertex_pp_comp>
inline string gain_matrix (ArrayAccum<ListAccum<T_gainmatrix>>& gGainMatrix) {
   struct timeval t1_st, t1_end; long seconds, useconds;
   
   gettimeofday(&t1_st, 0);
   
   
	// extern size_t g_si, g_sd, g_sp;
	printf("\n\n------------------------------------------------------------------------------------------- ");
	printf("\nStart Running gain_matrix function!\n");
    std::cout << "Gain Matrix Number of Rows:" << gGainMatrix.data_.size() << std::endl;
    printf("-------------------------------------------------------------------------------------------- \n\n");	
	// ------------------------------------------------------------------------------------------------
	// 				Initialize variables and arrays
	// ------------------------------------------------------------------------------------------------
	
 
	// Initialize arrays and variables
	//uint__t n, nnz, n_pp, nnz_pp, n_e, nnz_e;
	//int ret, i, j, p, iter;
	//real__t maxDeltaP=0, maxDeltaQ=0, max_change_ex=maxchange;
	//SGraphLU *graphlu, *graphlu_pp;
	uint__t n;
	int i;
  
    string result = "FAILED";
  
	//const double pi_value = 3.141592653589793;
    //const int64_t LUflag = 1; 
	// real__t *x, err;	
	// uint__t fctnnz; // number of nonzeros after the matrix is factorized

	// Get the dimension and the nnz of the matrix B' and B"
	n = gGainMatrix.data_.size(); // number of the row of gain matrix	//nnz=gBp; n_pp=gVertex.data_.size(); nnz_pp=gBpp;
    //n_e=gVertex.data_.size();  nnz_e/ = gYbus;	//nnz_e=gMatrix_all.data_.size(); // get the size of the Y bus matrix
  
	
	//// Convert vectors to GRAPHLU Data Structure (pointers)

	std::cout << " ======================== Initialization of ararys used to store the factorized matrix and LU ========================"<< std::endl;

	struct timeval t2_st, t2_end, t3_st, t3_end;

  //initialization to 0
  // [tc] use memset here
  //memset(Vm, 0, sizeof(real__t)*(n));
  //memset(Va, 0, sizeof(real__t)*(n));
  //memset(deltaP, 0, sizeof(real__t)*(n));
  //memset(deltaQ, 0, sizeof(real__t)*(n));
	
  // =================================== Sort all input HeapAccum =================================
  gettimeofday(&t3_st, 0);
/*  gettimeofday(&t2_st, 0);
  // Input HeapAccums are assumed to be unsorted. Sort them before assigning values to
  // local arrays
  gVertex.sort_heap();
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gVertex HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");   */
  
  struct ListAccum_sort{
            inline bool operator() (const T_gainmatrix& tuple1, const T_gainmatrix& tuple2)
            {
                return (tuple1.index < tuple2.index);
            }
	};
  
  
  gettimeofday(&t2_st, 0);  
  
  for(i=0;i<gGainMatrix.data_.size(); ++i){
	std::sort(gGainMatrix.data_[i].begin(), gGainMatrix.data_[i].end(), ListAccum_sort());
  }
  
  //std::sort(gMatrix_all.data_.begin(), gMatrix_all.data_.end(), ListAccum_sort());
 	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gMatrix_all ListAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	std::cout << "gMatrix_all Size:: " << gGainMatrix.data_.size() << std::endl;
	printf("\n================================================================================================ ");   
    
  // get the time for sorting all HeapAccums 
	gettimeofday(&t3_end, 0);
	seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	useconds = t3_end.tv_usec  - t3_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort all HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
	

EXIT:
	
	
  printf("\n\n----------------------------------------------------------------------------------------");
	printf("\t\t End of Running Gain Matrix function!");
	printf("\t\t ----------------------------------------------------------------------------------------\n\n");	
	
	gettimeofday(&t1_end, 0);
	seconds=t1_end.tv_sec  - t1_st.tv_sec; 
	useconds = t1_end.tv_usec  - t1_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Total Time of ExpressFunction " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");  
 
  return result;
  
  

  
}


// **********************************************************************************************************************************
// Created by: Ting Chan, ting.chan@geirina.net
// Date: 02/22/2017
// This code performs fast-decoupled power flow analysis. It is used as a benchmark to assess the time it takes to run a fast-decoupled
// power flow in C++ with GRAPHLU. This version performs LU factorization on the B' and B" matrix, then solve power flow iterations
// with forward/backward substitution on the LU matrices.
// After performing LU factorization, the resulting B' and B" LU factors are stored in SetAccums. In the query, new edges (fill-ins)
// are created, and existing edges are updated with factorized values. 
// History: 
// 11/15/2016: [Ting Chan] added another SetAccum for Bpp fill-in edges
// 01/09/2017: [Ting Chan] added a control parameter (fillin) to specify whether we want to find the fill-ins.
//             1 - find fill-ins
//             0 - otherwise
// 01/17/2017: [Ting Chan] Replaced GraphLU_Solve with our own implementation of forward/backward substitution. 
// 01/18/2017: [Ting Chan] Added two new HeapAccums to store factorized L values (diagonal), scaling factors, permutation and permutation
//             inverse.
// 01/25/2017: [Ting Chan] Replace input parameter "fillin" by "insert_LU". 
//             1 - Dump LU after factorization and store them in Set/HeapAccums
//             0 - Only factorization is performed 
// 01/26/2017: [Ting Chan] 
//             In an attempt to reduce some complexity, try to store the factorized pointer information onto the vertex as well
// 01/30/2017: [Ting Chan] Fixed forward/backward substitution to use cpi instead of rpi
// 02/01/2017: [Ting Chan] Created to be used with schema version flowSchema_LU7_graphsql44.ddl 
// 02/06/2017: [Ting Chan] Updated logic for storing the pointer information
// 02/06/2017: [Ting Chan] Added the update/insert of edges and attributes for factorized LU using "upsert"
// 02/07/2017: [Ting Chan] Added the update of vertex attributes for factorized LU using "upsert"
// 02/22/2017: [Ting Chan] Added STATIC HeapAccums for storing the factorized LU matrices as inputs. After LU factorization, the LUs 
//             are stored in these HeapAccums.
//             Removed "upsert" and all necessary code for inserting/updating vertex/edge. These are no longer needed as the factorized
//             LU are stored using the STATIC HeapAccums. In sequential runs, solve power flow directly using these HeapAccums. 
// **********************************************************************************************************************************
template <typename T_vertex, typename vertex_comp, 
          typename T_matrix_all, typename matrix_all_comp,
          typename T_p_L_matrix, typename p_L_matrix_comp,
          typename T_p_U_matrix, typename p_U_matrix_comp, 
          typename T_pp_L_matrix, typename pp_L_matrix_comp,
          typename T_pp_U_matrix, typename pp_U_matrix_comp,
          typename T_vertex_p, typename vertex_p_comp,
          typename T_vertex_pp, typename vertex_pp_comp>
inline string GRAPHLU_fdpf_LU_factorize_4_mod (int64_t& gBp, int64_t& gBpp, 
              HeapAccum<T_vertex, vertex_comp>& gVertex, HeapAccum<T_matrix_all, matrix_all_comp>& gMatrix_all,
              HeapAccum<T_p_L_matrix, p_L_matrix_comp>& gMatrix_p_L, HeapAccum<T_p_U_matrix, p_U_matrix_comp>& gMatrix_p_U, HeapAccum<T_vertex_p, vertex_p_comp>& gVertex_p,
              HeapAccum<T_pp_L_matrix, pp_L_matrix_comp>& gMatrix_pp_L, HeapAccum<T_pp_U_matrix, pp_U_matrix_comp>& gMatrix_pp_U, HeapAccum<T_vertex_pp, vertex_pp_comp>& gVertex_pp, 
              MaxAccum<int> maxiter, MaxAccum<double> maxchange) { //MaxAccum<int> insert_LU) {

	// extern size_t g_si, g_sd, g_sp;
	printf("\n\n------------------------------------------------------------------------------------------- ");
	printf("\nStart Running GRAPHLU_fdpf_LU_factorize_4_mod function!\n");
  std::cout << "Bp Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gBp << std::endl;
  std::cout << "Bpp Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gBpp << std::endl;
  std::cout << "Y bus Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gMatrix_all.data_.size() << std::endl;
	printf("-------------------------------------------------------------------------------------------- \n\n");	
	// ------------------------------------------------------------------------------------------------
	// 				Initialize variables and arrays
	// ------------------------------------------------------------------------------------------------
	
 
	// Initialize arrays and variables
	uint__t n, nnz, n_pp, nnz_pp, n_e, nnz_e;
	int ret, i, j, p, iter;
	real__t maxDeltaP=0, maxDeltaQ=0, max_change_ex=maxchange;
	SGraphLU *graphlu, *graphlu_pp;
  
  string result = "FAILED";
  
	const double pi_value = 3.141592653589793;
  const int64_t LUflag = 1; 
	// real__t *x, err;	
	// uint__t fctnnz; // number of nonzeros after the matrix is factorized

	// Get the dimension and the nnz of the matrix B' and B"
	n=gVertex.data_.size();	nnz=gBp; n_pp=gVertex.data_.size(); nnz_pp=gBpp;
  n_e=gVertex.data_.size(); 	nnz_e=gMatrix_all.data_.size(); // get the size of the Y bus matrix
  
	
	//// Convert vectors to GRAPHLU Data Structure (pointers)
	real__t *ax, *ax_pp, *eG, *eB, *deltaP, *deltaQ, *Vm, *Va, *Pn, *Qn; 
	uint__t *ai, *ai_pp, *ei, *piv, *btype; 
	uint__t *ap, *ap_pp, *ep; 
 
  // arrays to store the sine and cosine terms of the angle difference
  // values are computed during Jacobian initialization and each iteration 
  real__t *cosine_array, *sine_array;
  cosine_array = (real__t *)malloc(sizeof(real__t)*(nnz_e));
  sine_array = (real__t *)malloc(sizeof(real__t)*(nnz_e)); 
	
	ax = (real__t *)malloc(sizeof(real__t)*(nnz)); // values in B' 
	ai = (uint__t *)malloc(sizeof(uint__t)*(nnz)); // column indices of B'
	ap = (uint__t *)malloc(sizeof(uint__t)*(n+1)); // initial row pointers
	// b = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b

	ax_pp = (real__t *)malloc(sizeof(real__t)*(nnz_pp)); // values in B"
	ai_pp = (uint__t *)malloc(sizeof(uint__t)*(nnz_pp)); // column indices of B"
	ap_pp = (uint__t *)malloc(sizeof(uint__t)*(n_pp+1)); // initial row pointers

	eG = (real__t *)malloc(sizeof(real__t)*(nnz_e)); // G values in Y 
	eB = (real__t *)malloc(sizeof(real__t)*(nnz_e)); // B values in Y
	ei = (uint__t *)malloc(sizeof(uint__t)*(nnz_e)); // column indices of Y
	ep = (uint__t *)malloc(sizeof(uint__t)*(n_e+1)); // initial row pointers	

	deltaP = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b
	deltaQ = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b

	Vm = (real__t *)malloc(sizeof(real__t)*(n)); 
	Va = (real__t *)malloc(sizeof(real__t)*(n)); 

	Pn = (real__t *)malloc(sizeof(real__t)*(n)); 
	Qn = (real__t *)malloc(sizeof(real__t)*(n));

	btype = (uint__t *)malloc(sizeof(uint__t)*(n));
 
  double sum, diag;
  real__t *delta, *b;
  delta = (real__t *)malloc(sizeof(real__t)*(n));
  b = (real__t *)malloc(sizeof(real__t)*(n));
  
	std::cout << " ======================== Initialization of ararys used to store the factorized matrix and LU ========================"<< std::endl;

	struct timeval t2_st, t2_end, t3_st, t3_end; long seconds, useconds;

  //initialization to 0
  // [tc] use memset here
  memset(Vm, 0, sizeof(real__t)*(n));
  memset(Va, 0, sizeof(real__t)*(n));
  memset(deltaP, 0, sizeof(real__t)*(n));
  memset(deltaQ, 0, sizeof(real__t)*(n));
	
  // =================================== Sort all input HeapAccum =================================
  gettimeofday(&t3_st, 0);
  gettimeofday(&t2_st, 0);
  // Input HeapAccums are assumed to be unsorted. Sort them before assigning values to
  // local arrays
  gVertex.sort_heap();
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gVertex HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
  
  gettimeofday(&t2_st, 0);  
  gMatrix_all.sort_heap();
 	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gMatrix_all HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
    
  // get the time for sorting all HeapAccums 
	gettimeofday(&t3_end, 0);
	seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	useconds = t3_end.tv_usec  - t3_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort all HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
	
	// ========================================= Convert Ap, Ap_pp, ep and the Vertex =========================================
	gettimeofday(&t2_st, 0); 
  int i_p, i_pp;
	int ii = 1; // counter for the array  
  // Assign value to the first position of the pointer array 
  ap[0] = 0;
  ap_pp[0] = 0;
  ep[0] = 0;
	for (i=0;i<gVertex.size();i++){
    ap[ii] = gVertex.data_[i].Bp_p + ap[ii-1];
    //std::cout<< ii << ",ap,"<< ap[ii] << std::endl;
    ap_pp[ii] = gVertex.data_[i].Bpp_p + ap_pp[ii-1];
    ep[ii] = gVertex.data_[i].ep + ep[ii-1];
    ii++;
    Vm[i] = gVertex.data_[i].Vm;
    Va[i] = gVertex.data_[i].Va;
    Pn[i] = gVertex.data_[i].Pn;
    Qn[i] = gVertex.data_[i].Qn;
    btype[i] = gVertex.data_[i].bustype;
	}  
 	// ========================================= Convert Ybus (ei, eG and eB), B' (ei and Bp_x) and B" (ei and Bpp_x) =========================================
	i_p = 0;
  i_pp = 0;
  for (ii=0; ii<gMatrix_all.size(); ++ii) {
    ei[ii] = gMatrix_all.data_[ii].ei;
    eG[ii] = gMatrix_all.data_[ii].eG;
    eB[ii] = gMatrix_all.data_[ii].eB; 
    if(gMatrix_all.data_[ii].Bp_x != 0)
    {
      ai[i_p] = gMatrix_all.data_[ii].ei;
      ax[i_p] = gMatrix_all.data_[ii].Bp_x;
      //std::cout<< i_p << ",ax,"<< ax[i_p] << ",ai,"<< ai[i_p] << std::endl;
      i_p++;
    }
    if(gMatrix_all.data_[ii].Bpp_x != 0)
    {
      ai_pp[i_pp] = gMatrix_all.data_[ii].ei;
      ax_pp[i_pp] = gMatrix_all.data_[ii].Bpp_x;
      i_pp++;
    }
  } 	
  // Done converting all input HeapAccums to arrays
  
	// Get the time to convert the data between containers and arrays
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to convert data to GRAPHLU time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");		
	
	
	// -----------------------------------------------------------------------------------------------------------
	// ----------------------------- [Debug] [Chen Yuan] print out B' matrix -------------------------------------
	// -----------------------------------------------------------------------------------------------------------
	
	
	// Convert vectors to GRAPHLU Data Structure (pointers)
	real__t *B_p, *B_pp; //*deltaP, *deltaQ, *Vm, *Va; *Pn, *Qn; *eG, *eB,
	
	B_p = (real__t *)malloc(sizeof(real__t)*(n*n)); 
	B_pp = (real__t *)malloc(sizeof(real__t)*(n_pp*n_pp));
	
	std::cout << "Start to print out B_p matrix" << std::endl;
	
	int temp_ap;
	for (i=0; i<n; ){
		temp_ap = ap[i];
		//std::cout << i << ", ap: " << ap_GainP[i] << std::endl;
		for(ii=0; ii<n; ) {
			if (ii == ai[temp_ap]) {
				B_p[i*n + ii] = ax[temp_ap];
				temp_ap++;
			} 
			else {
				B_p[i*n + ii] = 0;
			}
			std::cout << B_p[i*n + ii] << ",";
			ii++;
		}
		std::cout << "" << std::endl;
		i++;
	}
	
	std::cout << "Finish B_p matrix print" << std::endl;
	
	std::cout << "Start to print out B_pp matrix" << std::endl;
	
	int temp_ap_pp;
	for (i=0; i<n; ){
		temp_ap_pp = ap_pp[i];
		//std::cout << i << ", ap: " << ap_GainP[i] << std::endl;
		for(ii=0; ii<n; ) {
			if (ii == ai_pp[temp_ap_pp]) {
				B_pp[i*n + ii] = ax_pp[temp_ap_pp];
				temp_ap_pp++;
			} 
			else {
				B_pp[i*n + ii] = 0;
			}
			std::cout << B_pp[i*n + ii] << ",";
			ii++;
		}
		std::cout << "" << std::endl;
		i++;
	}
	
	std::cout << "Finish B_pp matrix print" << std::endl;
	
	// [Debug] [Chen Yuan] print out B" matrix
	
	
	// ----------------------------------------------------------------------------------------------------
	// 								Call GRAPHLU and Factorize B' Matirx
	// ----------------------------------------------------------------------------------------------------
  int ret_init, ret_creat;

  int error_p = 0, error_pp = 0;
	gettimeofday(&t2_st, 0);
	std::cout << "\n ======================== Start factorizing B' Matirx ======================== \n"<< std::endl;
	// Initialize the structure for GRAPHLU
	graphlu = (SGraphLU *)malloc(sizeof(SGraphLU));
	GraphLU_Initialize(graphlu);

	// Create Matrix
	GraphLU_CreateMatrix(graphlu, n, nnz, ax, ai, ap);
	// graphlu->cfgf[0] = 1.;

	// // Set control parameters
	//graphlu->cfgi[1] = 0; // 0 is no MC64 scaling
	// 					 // 1 is with MC64 scaling

	// Analyze (column/row ordering, scaling. For details please refer to GRAPHLU user's guide)
	GraphLU_Analyze(graphlu);
	printf("analysis time: %.8g\n", graphlu->stat[0]);

	// creates the task scheduler for parallel LU factorization. If you want to run 
	// parallel factorization or parallel re-factorization, it should be called after GraphLU Analyze.
	ret = GraphLU_CreateScheduler(graphlu);
	printf("time of creating scheduler: %.8g\n", graphlu->stat[4]);
	printf("suggestion: %s.\n", ret==0?"parallel":"sequential");
	//std::cout << "GraphLU MC64 scaling 4th:" << graphlu->cfgi[1] <<std::endl; 
	// This function creates threads for parallel computation. The second argument (thread)
	// specifies the number of threads, including the main thread. The last argument (check)
	// specifies whether to check the number of threads or not.
  if (!ret){ // parallel factorization 
	  GraphLU_CreateThreads(graphlu, 2, TRUE);
	  printf("total cores: %d, threads created: %d\n", (int)(graphlu->stat[9]), (int)(graphlu->cfgi[5]));
	
	  // This function binds threads to cores (unbind = FALSE) or unbinds threads from cores (unbind = TRUE).
	  GraphLU_BindThreads(graphlu, FALSE);
	
	  // Numerical LU factorization with partial pivoting, parallel
	  error_p = GraphLU_Factorize_MT(graphlu);
	  printf("factorization time: %.8g\n", graphlu->stat[1]);
    if (error_p < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_p <<std::endl;  
  }   
  else{  // Sequential factorization
	  error_p = GraphLU_Factorize(graphlu);
	  printf("Factorization time: %.8g\n", graphlu->stat[1]);
    if (error_p < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_p <<std::endl; 
	}
 
  real__t *lx, *ux; 
  uint__t *li, *ui; 
  size_t *lp, *up; 
 	uint__t fctnnz; // number of nonzeros after the matrix is factorized
	size_t l_nnz, u_nnz; // number of nonzeros in L and U factors
  // get the permutation arrays, please refer to graphlu.h for details
	uint__t *rp, *cp, *rpi, *cpi; //row (column) permutation, row (column) permutation inverse
  real__t *ldiag, *cscale, *rows, *cols;
  int__t *pivot, *pivot_inv;
  
  lx = ux = NULL; 
  li = ui = NULL; 
  lp = up = NULL; 

  int row, col, k;
  int LUrow, LUcol;
    
  GraphLU_DumpLU(graphlu, &lx, &li, &lp, &ux, &ui, &up); 
     
   // Get the number of nonzeros in the factorized matrix
 	fctnnz = graphlu->lu_nnz;
 	l_nnz = graphlu->l_nnz;
 	u_nnz = graphlu->u_nnz;
 
 	std::cout << " ======================== Number of Total nonzeros after factorization is: "<< graphlu->lu_nnz << "========================"<<std::endl;
 	std::cout << " ======================== Number of nonzeros in L: "<< graphlu->l_nnz << "========================"<<std::endl;
 	std::cout << " ======================== Number of nonzeros in U: "<< graphlu->u_nnz << "========================"<<std::endl;
 	
   // get the permutation arrays, rp and cp may be different if the matrix is not symmetric
 	rp = graphlu->row_perm;
  rpi = graphlu->row_perm_inv;
 	cp = graphlu->col_perm;
  cpi = graphlu->col_perm_inv;

  ldiag = graphlu->ldiag; // diagnal elements
  cscale = graphlu->cscale; // sclaing factors
  rows = graphlu->row_scale; 
  cols = graphlu->col_scale_perm;
  pivot = graphlu->pivot; 
  pivot_inv = graphlu->pivot_inv; // pivoting array   
    
 	// print out and check
/* 	for (i=0; i<n; ++i){ // check the permutation arrays
 	 	std::cout << i << ",rp," << rp[i] << ",rpi," << rpi[i] << ",cp," << cp[i] << ",cpi," << cpi[i] << std::endl;
 	} 
	
	for (i=0; i<n; ++i){ // check the permutation arrays
 	 	std::cout << i << ",ldiag," << ldiag[i] << ",cscale," << cscale[i] << ",rows," << cp[i] << ",cols," << cpi[i] << std::endl;
 	}   */
  
  //std::cout << " ======================== Get L Factor ========================"<<std::endl;
 	/*for (i=0; i<l_nnz; ++i){
     std::cout<< i << ",lx,"<< lx[i] << ",li,"<< li[i] << std::endl; 
  }
   
  for (i=0; i<n+1; ++i){
     std::cout<< i << ",lp,"<< lp[i] << std::endl;
  }
   
  //std::cout << "\n ======================== Get U Factor ========================"<<std::endl;
 	for (i=0; i<u_nnz; ++i){
     std::cout<< i << ",ux,"<< ux[i] << ",ui,"<< ui[i] << std::endl;
  }
   
  for (i=0; i<n+1; ++i){
 		std::cout<< i << ",up,"<< up[i] << std::endl;
 	}*/

 	// check arrays
  //for (i=0; i<n; ++i){
 	// 	std::cout<< i << ",\t diag:"<< ldiag[i] << ",\t cscale:"<< cscale[i] << ",\t rows:"<< rows[i] << ",\t cols:"<< cols[i] << std::endl;
  //}
  //for (i=0; i<n; ++i){
 	// 	std::cout<< i << ",rp," << rp[i]  << ",cpi," << cpi[i] << ",rows,"<< rows[i] << ",cols," << cols[i] <<std::endl;
  //}  
 
 	// // multiply U factor by cols (which is )
 	// for (i=0; i<n; ++i){
 	// 	for (j=up[i]; j<up[i+1]; ++j){
 	// 		p=rp[piv[ui[j]]]; // this may be rp!!!!!!!!!!!
 	// 		ux[j] = ux[j]/cols[p]; // include the scaling factor into L factor
 	// 	}
 	// }
 
 	// ================================= Get the pivoting vectors =================================
/* 	for (i=0; i<n; ++i){
 		std::cout<< i << ",\t pivot:"<< pivot[i] << ",\t pivot_inv:"<< pivot_inv[i] << std::endl;
 	}  */

  //if (insert_LU == 1)
  //{  
    //get the factorized LU values and find the row and col before permutation
    for (i = 0; i < n; ++i){
      row = rp[i];
      LUrow = row+1;
      
      gVertex_p += T_vertex_p(i+1, lp[i+1]-lp[i], up[i+1]-up[i], rp[i], cpi[i], rows[i], cols[i], rp[i]+1); 
      // process all non-zeros in L matrix on row i
      for (j = lp[i]; j < lp[i+1]; ++j){
        col = cp[li[j]];             
        LUcol = col+1;
        
        gMatrix_p_L += T_p_L_matrix((i+1)*100000+(li[j]+1), li[j], lx[j]); 
      } 
         
      // process all non-zeros in U matrix on row i
      for (j = up[i]; j < up[i+1]; ++j){
        col = cp[ui[j]];
        LUcol = col+1; 
        
        gMatrix_p_U += T_p_U_matrix((i+1)*100000+(ui[j]+1), ui[j], ux[j]);          
      }   
    }
    gVertex_p.sort_heap();
    gMatrix_p_L.sort_heap();
    gMatrix_p_U.sort_heap();
  //}
  
  gVertex_p.sort_heap();
  gMatrix_p_L.sort_heap();
  gMatrix_p_U.sort_heap();
	// ----------------------------------------------------------------------------------------------------
	// 								Call GRAPHLU and Factorize B" Matirx
	// ----------------------------------------------------------------------------------------------------
	std::cout << "\n ======================== Start factorizing B'' Matirx ======================== \n"<< std::endl;
	// Initialize the structure for GRAPHLU
	graphlu_pp = (SGraphLU *)malloc(sizeof(SGraphLU));
	GraphLU_Initialize(graphlu_pp);
 
	// Create Matrix
	GraphLU_CreateMatrix(graphlu_pp, n_pp, nnz_pp, ax_pp, ai_pp, ap_pp);
	// graphlu_pp->cfgf[0] = 1.;

	// // Set control parameters
	//graphlu_pp->cfgi[1] = 0; // 0 is no MC64 scaling
	// 					 // 1 is with MC64 scaling
 
	// Analyze (column/row ordering, scaling. For details please refer to GRAPHLU user's guide)
	GraphLU_Analyze(graphlu_pp);
	printf("analysis time: %.8g\n", graphlu_pp->stat[0]);
  
	// creates the task scheduler for parallel LU factorization. If you want to run 
	// parallel factorization or parallel re-factorization, it should be called after GraphLU Analyze.
	ret = GraphLU_CreateScheduler(graphlu_pp);
	printf("time of creating scheduler: %.8g\n", graphlu_pp->stat[4]);
	printf("suggestion: %s.\n", ret==0?"parallel":"sequential");
	
	// This function creates threads for parallel computation. The second argument (thread)
	// specifies the number of threads, including the main thread. The last argument (check)
	// specifies whether to check the number of threads or not.
  if (!ret){ // parallel factorization
	  GraphLU_CreateThreads(graphlu_pp, 2, TRUE);
	  printf("total cores: %d, threads created: %d\n", (int)(graphlu_pp->stat[9]), (int)(graphlu_pp->cfgi[5]));
	
	  // This function binds threads to cores (unbind = FALSE) or unbinds threads from cores (unbind = TRUE).
	  GraphLU_BindThreads(graphlu_pp, FALSE);
	
	  // Numerical LU factorization with partial pivoting, parallel
	  error_pp = GraphLU_Factorize_MT(graphlu_pp);
	  printf("factorization time: %.8g\n", graphlu_pp->stat[1]);
    if (error_pp < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_pp <<std::endl;      
  }
  else{  //Sequential factorization 
	  error_pp = GraphLU_Factorize(graphlu_pp);
    printf("Factorization time: %.8g\n", graphlu_pp->stat[1]);
    if (error_pp < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_pp <<std::endl; 
  }

  real__t *lx_pp, *ux_pp; 
  uint__t *li_pp, *ui_pp; 
  size_t *lp_pp, *up_pp; 
 	uint__t fctnnz_pp; // number of nonzeros after the matrix is factorized
	size_t l_nnz_pp, u_nnz_pp; // number of nonzeros in L and U factors
  // get the permutation arrays, please refer to graphlu.h for details
	uint__t *rp_pp, *cp_pp, *rpi_pp, *cpi_pp; //row (column) permutation, row (column) permutation inverse
  real__t *ldiag_pp, *cscale_pp, *rows_pp, *cols_pp;
  int__t *pivot_pp, *pivot_inv_pp; 
  
  lx_pp = ux_pp = NULL; 
  li_pp = ui_pp = NULL; 
  lp_pp = up_pp = NULL; 
  
  
  GraphLU_DumpLU(graphlu_pp, &lx_pp, &li_pp, &lp_pp, &ux_pp, &ui_pp, &up_pp); 
   
   // Get the number of nonzeros in the factorized matrix
 	fctnnz_pp = graphlu_pp->lu_nnz;
 	l_nnz_pp = graphlu_pp->l_nnz;
 	u_nnz_pp = graphlu_pp->u_nnz;
 
 	std::cout << " ======================== Number of Total nonzeros after factorization is: "<< graphlu_pp->lu_nnz << "========================"<<std::endl;
 	std::cout << " ======================== Number of nonzeros in L: "<< graphlu_pp->l_nnz << "========================"<<std::endl;
 	std::cout << " ======================== Number of nonzeros in U: "<< graphlu_pp->u_nnz << "========================"<<std::endl;
 	
  // get the permutation arrays, rp and cp may be different if the matrix is not symmetric
 	rp_pp = graphlu_pp->row_perm;
  rpi_pp = graphlu_pp->row_perm_inv;
 	cp_pp = graphlu_pp->col_perm;
  cpi_pp = graphlu_pp->col_perm_inv;
  
  ldiag_pp = graphlu_pp->ldiag; // diagnal elements
  cscale_pp = graphlu_pp->cscale; // sclaing factors
  rows_pp = graphlu_pp->row_scale; 
  cols_pp = graphlu_pp->col_scale_perm;
  pivot_pp = graphlu_pp->pivot; 
  pivot_inv_pp = graphlu_pp->pivot_inv; // pivoting array   
    
 	// print out and check
 	//for (i=0; i<n; ++i){ // check the permutation arrays
 	// 	std::cout << "rp_pp," << rp_pp[i] << ",rpi_pp," << rpi_pp[i] << ",cp_pp,"<< cp_pp[i] << ",cpi_pp," << cpi_pp[i] << std::endl;
 	//} 
  
  //std::cout << " ======================== Get L Factor ========================"<<std::endl;
 	/*for (i=0; i<l_nnz_pp; ++i){
    std::cout<< i << ",lx_pp,"<< lx_pp[i] << ",li_pp,"<< li_pp[i] << std::endl; 
  }
   
  for (i=0; i<n+1; ++i){
    std::cout<< i << ",lp_pp,"<< lp_pp[i] << std::endl;
  }
   
   //std::cout << "\n ======================== Get U Factor ========================"<<std::endl;
 	for (i=0; i<u_nnz_pp; ++i){
    std::cout<< i << ",ux_pp,"<< ux_pp[i] << ",ui_pp,"<< ui_pp[i] << std::endl;
  }
   
 	for (i=0; i<n+1; ++i){
 		std::cout<< i << ",up_pp,"<< up_pp[i] << std::endl;
 	}*/
 
 	// check arrays
  //for (i=0; i<n; ++i){
 	// 	std::cout<< i << ",\t diag_pp:"<< ldiag_pp[i] << ",\t cscale_pp:"<< cscale_pp[i] << ",\t rows_pp:"<< rows_pp[i] << ",\t cols_pp:"<< cols_pp[i] << std::endl;
  //}
  //for (i=0; i<n; ++i){
 	// 	std::cout<< i << ",rp_pp," << rp_pp[i]  << ",cpi_pp," << cpi_pp[i] << ",rows_pp,"<< rows_pp[i] << ",cols_pp," << cols_pp[i] <<std::endl;
  //}   
 
 	// // multiply U factor by cols (which is )
 	// for (i=0; i<n; ++i){
 	// 	for (j=up[i]; j<up[i+1]; ++j){
 	// 		p=rp[piv[ui[j]]]; // this may be rp!!!!!!!!!!!
 	// 		ux[j] = ux[j]/cols[p]; // include the scaling factor into L factor
 	// 	}
 	// }
 
 	// ================================= Get the pivoting vectors =================================
 	//for (i=0; i<n; ++i){
 	//	std::cout<< i << ",\t pivot_pp:"<< pivot_pp[i] << ",\t pivot_inv_pp:"<< pivot_inv_pp[i] << std::endl;
 	//}
  
  //if (insert_LU == 1)
  //{      	
    //get the factorized LU values and find the row and col before permutation
    for (i = 0; i < n; ++i){
      row = rp_pp[i];
      LUrow = row+1;
      
      gVertex_pp += T_vertex_pp(i+1, lp_pp[i+1]-lp_pp[i], up_pp[i+1]-up_pp[i], rp_pp[i], cpi_pp[i], rows_pp[i], cols_pp[i], rp_pp[i]+1); 
      // process all non-zeros in L matrix on row i
      for (j = lp_pp[i]; j < lp_pp[i+1]; ++j){
        col = cp_pp[li_pp[j]];
        LUcol = col+1;
        
        gMatrix_pp_L += T_pp_L_matrix((i+1)*100000+(li_pp[j]+1), li_pp[j], lx_pp[j]); 
      } 
      
      // process all non-zeros in U matrix on row i
      for (j = up_pp[i]; j < up_pp[i+1]; ++j){
        col = cp_pp[ui_pp[j]];
        LUcol = col+1;
        
        gMatrix_pp_U += T_pp_U_matrix((i+1)*100000+(ui_pp[j]+1), ui_pp[j], ux_pp[j]);
      }    
    }
    gVertex_pp.sort_heap();
    gMatrix_pp_L.sort_heap();
    gMatrix_pp_U.sort_heap();          
  //}
  
  // Get the time for factorizing B' and B"
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	
	printf("\n\n============================================================================================== ");
	std::cout << "Time for GRAPHLU to factorize B' and B'':: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");
  
  if(error_p >= 0 && error_pp >= 0){
    //continue to do the iterations only if there is no error from factorization
  	printf("\n\n============================================================================================== ");
  	std::cout << "\n Start iteratively updating deltaP and deltaQ " << std::endl;
  	printf("\n================================================================================================\n ");
  
  	gettimeofday(&t2_st, 0);
   
  	for (iter=0; iter<maxiter; ++iter){
  		// update deltaP and deltaQ
  		maxDeltaP=0;
  		maxDeltaQ=0;
  
  		// std::cout << " ======================== Update deltaP and deltaQ ========================"<<std::endl;
      gettimeofday(&t3_st, 0);
  		// Calculate deltaP and deltaQ to update Va
  		for (i=0; i<n; ++i){
  			deltaP[i] = 0;
  			deltaQ[i] = 0;
  
  			if (btype[i]!=3){
  				deltaP[i] = Pn[i];
  				if (btype[i]<2)
  					deltaQ[i] = Qn[i];
  			}
  
  			// Calculate network injections
  			if (btype[i]!=3){
  				for (j=ep[i]; j<ep[i+1]; ++j){
  					p=ei[j];
            if (iter == 0){        
              //if this is the first iteraton, calculate the cosine and sine terms        
              cosine_array[j] = cos(Va[i]-Va[p]);
              sine_array[j] = sin(Va[i]-Va[p]);
            }    
				deltaP[i] += - Vm[i]*Vm[p]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);        
  				if (btype[i]<2) // calculate Q for PQ buses
				deltaQ[i] += - Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]); 
  				}
  			}  
        
  			// Get max P and max Q
  			if (abs(deltaP[i]) > maxDeltaP)
  				maxDeltaP = abs(deltaP[i]);
  			if (abs(deltaQ[i]) > maxDeltaQ)
  				maxDeltaQ = abs(deltaQ[i]);
  
  			deltaP[i] /= Vm[i];
  			deltaQ[i] /= Vm[i];
  
  			// check output
 			  //std::cout << "Iter" << iter << ", deltaP" << i << ", " << deltaP[i]<< ", deltaQ" << i << ", " << deltaQ[i]<< std::endl;
  		}
  
  		std::cout << "Iter" << iter << " Updated for Va, maxDeltaP, " << maxDeltaP << ", maxDeltaQ, " << maxDeltaQ << std::endl;
   	  
      gettimeofday(&t3_end, 0);
  	  seconds=t3_end.tv_sec  - t3_st.tv_sec; 
  	  useconds = t3_end.tv_usec  - t3_st.tv_usec;
  	  printf("\n\n==============================================================================================\n ");
  	  std::cout << "Time for updating deltaP and deltaQ (iteration):: " << iter << " :" << (seconds*1000000 + useconds) << " us" << std::endl;
  	  printf("\n================================================================================================\n ");
      
      // Decide if the program converges	
  		if ( maxDeltaP < max_change_ex && maxDeltaQ < max_change_ex){
        result = "OK";
  			break;
  		}
      
      //reset the max mismatche
      maxDeltaP = 0;
      maxDeltaQ = 0;
      
   		// solve for deltaVa
  		// std::cout << " ======================== Solve for V angle ========================"<<std::endl;
      
      // A*x = b
      // multiply deltaP with row scaling (rows) to get b
    	for (i=0; i<n; ++i){
        b[i]=deltaP[rp[i]]*rows[rp[i]];
    	}
     
    	// Full forward substitution 
    	for (i=0; i<n; ++i)
    	{
    		sum=0.;
    		diag=0.;
    		for(p=lp[i]; p< lp[i+1]; ++p){
    			if (i!=li[p]){ // if it is not diagnal element
    				j=li[p];
    				sum += lx[p] * b[j];
    			}
    			else{
    				diag = lx[p];
    			}
    		}

    		b[i] -= sum;
    		b[i] /= diag;
    	} 
     
    	// Full backward substitution 
    	for (i=n-1; i>=0; --i)
    	{
    		sum=0.;
    		for(p=up[i]; p< up[i+1]; ++p){
    			if (i!=ui[p]){ // if it is not diagnal element
    				sum += ux[p] * b[ui[p]]; 
    			}
    		}  
    		b[i] -= sum; 
    	}      
    	// permute the array back to the original order
    	//for (i=0; i<n; ++i)
      //{
    	//	delta[i]=b[cpi[i]];
      //  //std::cout << "delta: " << i << ", " << delta[i] << std::endl;           
  		//}
        
      // Update V angle (Va)
  		for (i=0; i<n; ++i)
      { 
        //Va[i] -= delta[i];  
        Va[i] -= b[cpi[i]]*cols[cpi[i]];      
      }
     
  		// std::cout << " ======================== Update deltaP and deltaQ ========================"<<std::endl;
      gettimeofday(&t3_st, 0);
  		// Calculate deltaP and deltaQ to update Vm
  		for (i=0; i<n; ++i){
  			deltaP[i] = 0;
  			deltaQ[i] = 0;
  
  			if (btype[i]!=3){
  				deltaP[i] = Pn[i];
  				if (btype[i]<2)
  					deltaQ[i] = Qn[i];
  			}
  
  			// Calculate network injections
  			if (btype[i]!=3){
  				for (j=ep[i]; j<ep[i+1]; ++j){
  					p=ei[j];
					// Va has been updated, update the cosine and sine terms 
					cosine_array[j] = cos(Va[i]-Va[p]);
					sine_array[j] = sin(Va[i]-Va[p]);        
					deltaP[i] += - Vm[i]*Vm[p]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);        
					if (btype[i]<2) // calculate Q for PQ buses
					{
						deltaQ[i] += - Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]); 
					}
						
				}
  			}
  
  			// Get max P and max Q
  			if (abs(deltaP[i]) > maxDeltaP)
  				maxDeltaP = abs(deltaP[i]);
  			if (abs(deltaQ[i]) > maxDeltaQ)
  				maxDeltaQ = abs(deltaQ[i]);
  
  			deltaP[i] /= Vm[i];
  			deltaQ[i] /= Vm[i];
  
  			// check output
  			// std::cout << "Iter" << iter << ", deltaP" << i << ", " << deltaP[i]<< ", deltaQ" << i << ", " << deltaQ[i]<< std::endl;
      }
  
  		std::cout << "Iter" << iter << " Updated for Vm, maxDeltaP, " << maxDeltaP << ", maxDeltaQ, " << maxDeltaQ << std::endl;
  
      gettimeofday(&t3_end, 0);
  	  seconds=t3_end.tv_sec  - t3_st.tv_sec; 
  	  useconds = t3_end.tv_usec  - t3_st.tv_usec;
  	  printf("\n\n==============================================================================================\n ");
  	  std::cout << "Time for updating deltaP and deltaQ (iteration):: " << iter << " :" << (seconds*1000000 + useconds) << " us" << std::endl;
  	  printf("\n================================================================================================\n ");
       
  		// Decide if the program converges	
  		if ( maxDeltaP < max_change_ex && maxDeltaQ < max_change_ex){
        result = "OK";
  			break;
  		}

  		// solve for deltaVm
  		// std::cout << " ======================== Solve for V magnitude ========================"<<std::endl;
     
      // A*x = b
      // multiply deltaQ with row scaling (rows_pp) to get b
    	for (i=0; i<n; ++i){
			b[i]=deltaQ[rp_pp[i]]*rows_pp[rp_pp[i]];
    	}
     
    	// Full forward substitution 
    	for (i=0; i<n; ++i)
    	{
    		sum=0.;
    		diag=0.;
    		for(p=lp_pp[i]; p< lp_pp[i+1]; ++p){
    			if (i!=li_pp[p]){ // if it is not diagnal element
    				j=li_pp[p];
    				sum += lx_pp[p] * b[j]; 
    			}
    			else{
    				diag = lx_pp[p];
    			}
    		}

    		b[i] -= sum; 
    		b[i] /= diag;
    	} 
     
    	// Full backward substitution 
    	for (i=n-1; i>=0; --i)
    	{
    		sum=0.;
    		for(p=up_pp[i]; p< up_pp[i+1]; ++p){
    			if (i!=ui_pp[p]){ // if it is not diagnal element
    				sum += ux_pp[p] * b[ui_pp[p]]; 
    			}
    		}
    
    		b[i] -= sum; 
    	}      
    	// permute the array back to the original order
    	//for (i=0; i<n; ++i)
      //{
    	//	delta[i]=b[cpi_pp[i]];
      //  //std::cout << "delta: " << i << ", " << delta[i] << std::endl;           
  		//}
  
  		// Update V magnitude (Vm)
  		for (i=0; i<n; ++i)
      {
        //Vm[i] -= delta[i];
        Vm[i] -= b[cpi_pp[i]]*cols_pp[cpi_pp[i]];
      }       
  	}
   
  	gettimeofday(&t2_end, 0);
  	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
  	useconds = t2_end.tv_usec  - t2_st.tv_usec;
  	
  	printf("\n\n============================================================================================== ");
  	std::cout << "Time for performing power flow iteration: " << (seconds*1000000 + useconds) << " us" << std::endl;
  	printf("\n================================================================================================ ");
  
  	// ----------------------------------------------------------------------------------------------------
  	// 								Store the Result back to the array
  	// ----------------------------------------------------------------------------------------------------
  	// Get solved Vm and Va back into HeapAccum
  	for (ii=0; ii<n; ++ii) {
        gVertex.data_[ii].Va = Va[ii]; // in radian
        gVertex.data_[ii].Vm = Vm[ii];
        //std::cout << "Final, " << ii+1 << ", Vm, " << Vm[ii]<< ", Va" << ii+1 << ", " << Va[ii]/pi_value*180<< std::endl;
  	}
  } else {
    //factorization failed
 	  result = "FAILED";
    printf("\n\n============================================================================================== ");
  	std::cout << "Factorization FAILED" << std::endl;
  	printf("\n================================================================================================ ");  
  }
EXIT:
	// std::cout << "\n free1 " << std::endl;
	GraphLU_Destroy(graphlu);
	GraphLU_Destroy(graphlu_pp);
	// std::cout << "\n free2 " << std::endl;
	free(ax);
	free(ai);
	free(ap);
	free(ax_pp);
	free(ai_pp);
	free(ap_pp);
	free(graphlu);
	free(graphlu_pp);
	// std::cout << "\n free3 " << std::endl;
		
	free(eG);
	free(eB);
	free(ei);
	free(ep);

	free(deltaP);
	free(deltaQ);
	free(Vm);
	free(Va);		
	free(Pn);
	free(Qn);	
  
 	free(lx);
	free(li);
	free(lp);
  free(ux);
	free(ui);
	free(up);  
 	free(lx_pp);
	free(li_pp);
	free(lp_pp);
 	free(ux_pp);
	free(ui_pp);
	free(up_pp);    
	
  printf("\n\n----------------------------------------------------------------------------------------");
	printf("\t\t End of Running GRAPHLU_fdpf_LU_factorize_4_mod C function!");
	printf("\t\t ----------------------------------------------------------------------------------------\n\n");	
 
  return result;
}



// **********************************************************************************************************************************
// Created by: Chen Yuan, chen.yuan@geirina.net
// Date: 06/08/2018
// This code originates from the function "GRAPHLU_fdpf_LU_factorize_4_mod". It performs fast-decoupled power flow analysis. This version performs LU factorization on the B' and B" matrices, then solve power flow iterations with forward/backward substitution on the LU matrices. After performing LU factorization, the resulting B' and B" LU factors are stored in static class via C++.  
// History: 
// 06/08/2018 [Chen Yuan]: Built this fdpf function to work in EMS system. Using static class to store LU matrices in B' and B".
// **********************************************************************************************************************************
template <typename T_vertex, typename vertex_comp, 
          typename T_matrix_all, typename matrix_all_comp>
inline string GRAPHLU_fdpf_LU_factorize_EMS (int64_t& gBp, int64_t& gBpp, 
              HeapAccum<T_vertex, vertex_comp>& gVertex, HeapAccum<T_matrix_all, matrix_all_comp>& gMatrix_all, 
              MaxAccum<int> maxiter, MaxAccum<double> maxchange) { //MaxAccum<int> insert_LU) {

	// extern size_t g_si, g_sd, g_sp;
	printf("\n\n------------------------------------------------------------------------------------------- ");
	printf("\nStart Running GRAPHLU_fdpf_LU_factorize_EMS function!\n");
    std::cout << "Bp Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gBp << std::endl;
    std::cout << "Bpp Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gBpp << std::endl;
    std::cout << "Y bus Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gMatrix_all.data_.size() << std::endl;
	printf("-------------------------------------------------------------------------------------------- \n\n");	
	// ------------------------------------------------------------------------------------------------
	// 				Initialize variables and arrays
	// ------------------------------------------------------------------------------------------------
	
 
	// Initialize arrays and variables
	uint__t n, nnz, n_pp, nnz_pp, n_e, nnz_e;
	int ret, i, j, p, iter;
	real__t maxDeltaP=0, maxDeltaQ=0, max_change_ex=maxchange;
	SGraphLU *graphlu_Bp, *graphlu_Bpp;
  
    string result = "FAILED";
  
	const double pi_value = 3.141592653589793;
    const int64_t LUflag = 1; 
	// real__t *x, err;	
	// uint__t fctnnz; // number of nonzeros after the matrix is factorized

	// Get the dimension and the nnz of the matrix B' and B"
	n=gVertex.data_.size();	nnz=gBp; n_pp=gVertex.data_.size(); nnz_pp=gBpp;
    n_e=gVertex.data_.size(); 	nnz_e=gMatrix_all.data_.size(); // get the size of the Y bus matrix
  
	
	//// Convert vectors to GRAPHLU Data Structure (pointers)
	real__t *ax, *ax_pp, *eG, *eB, *deltaP, *deltaQ, *P_operation, *Q_operation, *Vm, *Va, *dVm_sens, *dVa_sens, *Pn, *Qn, *Zth, *ZL, *STI; 
	uint__t *ai, *ai_pp, *ei, *piv, *btype; 
	uint__t *ap, *ap_pp, *ep; 
	double *line_Q1;
 
	// arrays to store the sine and cosine terms of the angle difference
	// values are computed during Jacobian initialization and each iteration 
	real__t *cosine_array, *sine_array;
	cosine_array = (real__t *)malloc(sizeof(real__t)*(nnz_e));
	sine_array = (real__t *)malloc(sizeof(real__t)*(nnz_e)); 
	
	ax = (real__t *)malloc(sizeof(real__t)*(nnz)); // values in B' 
	ai = (uint__t *)malloc(sizeof(uint__t)*(nnz)); // column indices of B'
	ap = (uint__t *)malloc(sizeof(uint__t)*(n+1)); // initial row pointers
	// b = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b
	line_Q1 = (double *)malloc(sizeof(double)*(nnz)); // values in B' 

	ax_pp = (real__t *)malloc(sizeof(real__t)*(nnz_pp)); // values in B"
	ai_pp = (uint__t *)malloc(sizeof(uint__t)*(nnz_pp)); // column indices of B"
	ap_pp = (uint__t *)malloc(sizeof(uint__t)*(n_pp+1)); // initial row pointers

	eG = (real__t *)malloc(sizeof(real__t)*(nnz_e)); // G values in Y 
	eB = (real__t *)malloc(sizeof(real__t)*(nnz_e)); // B values in Y
	ei = (uint__t *)malloc(sizeof(uint__t)*(nnz_e)); // column indices of Y
	ep = (uint__t *)malloc(sizeof(uint__t)*(n_e+1)); // initial row pointers	

	deltaP = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b
	deltaQ = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b
	P_operation = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b for VSA
	Q_operation = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b for VSA
	
	Vm = (real__t *)malloc(sizeof(real__t)*(n)); 
	Va = (real__t *)malloc(sizeof(real__t)*(n)); 
	dVm_sens = (real__t *)malloc(sizeof(real__t)*(n)); 
	dVa_sens = (real__t *)malloc(sizeof(real__t)*(n));
	Zth = (real__t *)malloc(sizeof(real__t)*(n)); 
	ZL = (real__t *)malloc(sizeof(real__t)*(n)); 
	STI = (real__t *)malloc(sizeof(real__t)*(n));
	
	Pn = (real__t *)malloc(sizeof(real__t)*(n)); 
	Qn = (real__t *)malloc(sizeof(real__t)*(n));

	btype = (uint__t *)malloc(sizeof(uint__t)*(n));
 
	double sum, diag;
	real__t *delta, *b, *lambda_b;
	delta = (real__t *)malloc(sizeof(real__t)*(n));
	b = (real__t *)malloc(sizeof(real__t)*(n));
	lambda_b = (real__t *)malloc(sizeof(real__t)*(n));
  
	std::cout << " ======================== Initialization of ararys used to store the factorized matrix and LU ========================"<< std::endl;

	struct timeval t2_st, t2_end, t3_st, t3_end; long seconds, useconds;

	//initialization to 0
	// [tc] use memset here
	memset(Vm, 0, sizeof(real__t)*(n));
	memset(Va, 0, sizeof(real__t)*(n));
	memset(deltaP, 0, sizeof(real__t)*(n));
	memset(deltaQ, 0, sizeof(real__t)*(n));
	
	memset(dVm_sens, 0, sizeof(real__t)*(n));
	memset(dVa_sens, 0, sizeof(real__t)*(n));
	memset(Zth, 0, sizeof(real__t)*(n));
	memset(ZL, 0, sizeof(real__t)*(n));
	memset(STI, 0, sizeof(real__t)*(n));
	memset(P_operation, 0, sizeof(real__t)*(n));
	memset(Q_operation, 0, sizeof(real__t)*(n));
	
	// =================================== Sort all input HeapAccum =================================
	gettimeofday(&t3_st, 0);
	gettimeofday(&t2_st, 0);
	// Input HeapAccums are assumed to be unsorted. Sort them before assigning values to
	// local arrays
	gVertex.sort_heap();
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gVertex HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
  
	gettimeofday(&t2_st, 0);  
	gMatrix_all.sort_heap();
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gMatrix_all HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
    
  // get the time for sorting all HeapAccums 
	gettimeofday(&t3_end, 0);
	seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	useconds = t3_end.tv_usec  - t3_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort all HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
	
	// ========================================= Convert Ap, Ap_pp, ep and the Vertex =========================================
	gettimeofday(&t2_st, 0); 
  int i_p, i_pp;
	int ii = 1; // counter for the array  
  // Assign value to the first position of the pointer array 
  ap[0] = 0;
  ap_pp[0] = 0;
  ep[0] = 0;
	for (i=0;i<gVertex.size();i++){
    ap[ii] = gVertex.data_[i].Bp_p + ap[ii-1];
    //std::cout<< ii << ",ap,"<< ap[ii] << std::endl;
    ap_pp[ii] = gVertex.data_[i].Bpp_p + ap_pp[ii-1];
    ep[ii] = gVertex.data_[i].ep + ep[ii-1];
    ii++;
    Vm[i] = gVertex.data_[i].Vm;
    Va[i] = gVertex.data_[i].Va;
    Pn[i] = gVertex.data_[i].Pn;
    Qn[i] = gVertex.data_[i].Qn;
	Zth[i] = gVertex.data_[i].Zth;
    STI[i] = gVertex.data_[i].STI;
    btype[i] = gVertex.data_[i].bustype;
	}  
 	// ========================================= Convert Ybus (ei, eG and eB), B' (ei and Bp_x) and B" (ei and Bpp_x) =========================================
	i_p = 0;
  i_pp = 0;
  for (ii=0; ii<gMatrix_all.size(); ++ii) {
    ei[ii] = gMatrix_all.data_[ii].ei;
    eG[ii] = gMatrix_all.data_[ii].eG;
    eB[ii] = gMatrix_all.data_[ii].eB; 
    line_Q1[i_p] = gMatrix_all.data_[ii].line_Q1;
    if(gMatrix_all.data_[ii].Bp_x != 0)
    {
      ai[i_p] = gMatrix_all.data_[ii].ei;
      ax[i_p] = gMatrix_all.data_[ii].Bp_x;
      // std::cout<< i_p << ",ax,"<< ax[i_p] << ",ai,"<< ai[i_p] << std::endl;
      i_p++;
    }
    if(gMatrix_all.data_[ii].Bpp_x != 0)
    {
      ai_pp[i_pp] = gMatrix_all.data_[ii].ei;
      ax_pp[i_pp] = gMatrix_all.data_[ii].Bpp_x;
      i_pp++;
    }
  } 	
  // Done converting all input HeapAccums to arrays
  
	// Get the time to convert the data between containers and arrays
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to convert data to GRAPHLU time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");		
	
	
	// -----------------------------------------------------------------------------------------------------------
	// ----------------------------- [Debug] [Chen Yuan] print out B' matrix -------------------------------------
	// -----------------------------------------------------------------------------------------------------------
	
	
	// // Convert vectors to GRAPHLU Data Structure (pointers)
	// real__t *B_p, *B_pp; //*deltaP, *deltaQ, *Vm, *Va; *Pn, *Qn; *eG, *eB,
	
	// B_p = (real__t *)malloc(sizeof(real__t)*(n*n)); 
	// B_pp = (real__t *)malloc(sizeof(real__t)*(n_pp*n_pp));
	
	// std::cout << "Start to print out B_p matrix" << std::endl;
	
	// int temp_ap;
	// for (i=0; i<n; ){
		// temp_ap = ap[i];
		// //std::cout << i << ", ap: " << ap_GainP[i] << std::endl;
		// for(ii=0; ii<n; ) {
			// if (ii == ai[temp_ap]) {
				// B_p[i*n + ii] = ax[temp_ap];
				// temp_ap++;
			// } 
			// else {
				// B_p[i*n + ii] = 0;
			// }
			// std::cout << B_p[i*n + ii] << ",";
			// ii++;
		// }
		// std::cout << "" << std::endl;
		// i++;
	// }
	
	// std::cout << "Finish B_p matrix print" << std::endl;
	
	// std::cout << "Start to print out B_pp matrix" << std::endl;
	
	// int temp_ap_pp;
	// for (i=0; i<n; ){
		// temp_ap_pp = ap_pp[i];
		// //std::cout << i << ", ap: " << ap_GainP[i] << std::endl;
		// for(ii=0; ii<n; ) {
			// if (ii == ai_pp[temp_ap_pp]) {
				// B_pp[i*n + ii] = ax_pp[temp_ap_pp];
				// temp_ap_pp++;
			// } 
			// else {
				// B_pp[i*n + ii] = 0;
			// }
			// std::cout << B_pp[i*n + ii] << ",";
			// ii++;
		// }
		// std::cout << "" << std::endl;
		// i++;
	// }
	
	// std::cout << "Finish B_pp matrix print" << std::endl;
	
	// [Debug] [Chen Yuan] print out B" matrix
	
	
	// ----------------------------------------------------------------------------------------------------
	// 								Call GRAPHLU and Factorize B' Matirx
	// ----------------------------------------------------------------------------------------------------
	int ret_init, ret_creat;

	int error_p = 0, error_pp = 0;
	gettimeofday(&t2_st, 0);
	std::cout << "\n ======================== Start factorizing B' Matirx ======================== \n"<< std::endl;
	// Initialize the structure for GRAPHLU
	graphlu_Bp = (SGraphLU *)malloc(sizeof(SGraphLU));
	GraphLU_Initialize(graphlu_Bp);

	// Create Matrix
	GraphLU_CreateMatrix(graphlu_Bp, n, nnz, ax, ai, ap);
	// graphlu_Bp->cfgf[0] = 1.;

	// // Set control parameters
	//graphlu_Bp->cfgi[1] = 0; // 0 is no MC64 scaling
	// 					 // 1 is with MC64 scaling

	// Analyze (column/row ordering, scaling. For details please refer to GRAPHLU user's guide)
	GraphLU_Analyze(graphlu_Bp);
	printf("analysis time: %.8g\n", graphlu_Bp->stat[0]);

	// creates the task scheduler for parallel LU factorization. If you want to run 
	// parallel factorization or parallel re-factorization, it should be called after GraphLU Analyze.
	ret = GraphLU_CreateScheduler(graphlu_Bp);
	printf("time of creating scheduler: %.8g\n", graphlu_Bp->stat[4]);
	printf("suggestion: %s.\n", ret==0?"parallel":"sequential");
	//std::cout << "GraphLU MC64 scaling 4th:" << graphlu_Bp->cfgi[1] <<std::endl; 
	// This function creates threads for parallel computation. The second argument (thread)
	// specifies the number of threads, including the main thread. The last argument (check)
	// specifies whether to check the number of threads or not.
  if (!ret){ // parallel factorization 
	  GraphLU_CreateThreads(graphlu_Bp, 2, TRUE);
	  printf("total cores: %d, threads created: %d\n", (int)(graphlu_Bp->stat[9]), (int)(graphlu_Bp->cfgi[5]));
	
	  // This function binds threads to cores (unbind = FALSE) or unbinds threads from cores (unbind = TRUE).
	  GraphLU_BindThreads(graphlu_Bp, FALSE);
	
	  // Numerical LU factorization with partial pivoting, parallel
	  error_p = GraphLU_Factorize_MT(graphlu_Bp);
	  printf("factorization time: %.8g\n", graphlu_Bp->stat[1]);
    if (error_p < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_p <<std::endl;  
  }   
  else{  // Sequential factorization
	  error_p = GraphLU_Factorize(graphlu_Bp);
	  printf("Factorization time: %.8g\n", graphlu_Bp->stat[1]);
    if (error_p < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_p <<std::endl; 
	}
 
  real__t *lx_Bp, *ux_Bp; 
  uint__t *li_Bp, *ui_Bp; 
  size_t *lp_Bp, *up_Bp; 
 	//uint__t fctnnz; // number of nonzeros after the matrix is factorized
	//size_t l_nnz, u_nnz; // number of nonzeros in L and U factors
  // get the permutation arrays, please refer to graphlu.h for details
	uint__t *rp_Bp, *cp_Bp, *rpi_Bp, *cpi_Bp, l_nnz_Bp, u_nnz_Bp; //row (column) permutation, row (column) permutation inverse
  real__t *ldiag_Bp, *cscale_Bp, *rows_Bp, *cols_Bp;
  int__t *pivot_Bp, *pivot_inv_Bp;
  
  lx_Bp = ux_Bp = NULL; 
  li_Bp = ui_Bp = NULL; 
  lp_Bp = up_Bp = NULL; 

  int row, col, k;
  int LUrow, LUcol;
    
  GraphLU_DumpLU(graphlu_Bp, &lx_Bp, &li_Bp, &lp_Bp, &ux_Bp, &ui_Bp, &up_Bp); 
     
   // Get the number of nonzeros in the factorized matrix
 	//fctnnz = graphlu_Bp->lu_nnz;
 	l_nnz_Bp = graphlu_Bp->l_nnz;
 	u_nnz_Bp = graphlu_Bp->u_nnz;
 
 	std::cout << " ======================== Number of Total nonzeros after factorization is: "<< graphlu_Bp->lu_nnz << "========================"<<std::endl;
 	std::cout << " ======================== Number of nonzeros in L: "<< graphlu_Bp->l_nnz << "========================"<<std::endl;
 	std::cout << " ======================== Number of nonzeros in U: "<< graphlu_Bp->u_nnz << "========================"<<std::endl;
 	
   // get the permutation arrays, rp_Bp and cp_Bp may be different if the matrix is not symmetric
	rp_Bp = graphlu_Bp->row_perm;
	rpi_Bp = graphlu_Bp->row_perm_inv;
	cp_Bp = graphlu_Bp->col_perm;
	cpi_Bp = graphlu_Bp->col_perm_inv;

	ldiag_Bp = graphlu_Bp->ldiag; // diagnal elements
	cscale_Bp = graphlu_Bp->cscale; // sclaing factors
	rows_Bp = graphlu_Bp->row_scale; 
	cols_Bp = graphlu_Bp->col_scale_perm;
	pivot_Bp = graphlu_Bp->pivot; 
	pivot_inv_Bp = graphlu_Bp->pivot_inv; // pivoting array  

	
	// [Chen Yuan] static L_Bp and U_Bp matrix  

	// free MatrixBp, MatrixLineQ1, MatrixLp, MatrixUp
	SingletonMatrixInterface::deleteMatrixBp();
	SingletonMatrixInterface::deleteMatrixLineQ1();
	SingletonMatrixInterface::deleteMatrixLp();
	SingletonMatrixInterface::deleteMatrixUp();

	CSRMatrix *Bp = SingletonMatrixInterface::getMatrixBp();
	CSRMatrix *Bp_L = SingletonMatrixInterface::getMatrixLp();
	CSRMatrix *Bp_U = SingletonMatrixInterface::getMatrixUp();
	CSRMatrix *Bp_LineQ1 = SingletonMatrixInterface::getMatrixLineQ1();	
	std::cout << "Start Bp and its LU matrices store" << std::endl;
	
	//Bp->loadMatrix(n, nnz, ap, ai, ax, btype, Va, Vm, Pn);
	//std::cout << "Finish Bp store" << std::endl;   // Bp and Va matrix load is placed to the position after Va calculation.
	
	Bp_L->loadMatrix(n, l_nnz_Bp, lp_Bp, li_Bp, lx_Bp, rp_Bp, cpi_Bp, rows_Bp, cols_Bp);
	std::cout << "Finish Bp_L store" << std::endl;
	
	Bp_U->loadMatrix(n, u_nnz_Bp, up_Bp, ui_Bp, ux_Bp);
	std::cout << "Finish Bp_U store" << std::endl;
	
	Bp_LineQ1->loadMatrix(n, nnz, ap, ai, line_Q1);
	// Bp LU matrices static storage end
	
	std::cout << "Finish Bp and its LU matrices store" << std::endl;
	
	// [Chen Yuan] static L_Ybus and U_Ybus matrix   
	CSRMatrix *Ybus_eG = SingletonMatrixInterface::getMatrixeG();
	CSRMatrix *Ybus_eB = SingletonMatrixInterface::getMatrixeB();
	Ybus_eG->loadMatrix(n, nnz_e, ep, ei, eG);
	Ybus_eB->loadMatrix(n, nnz_e, ep, ei, eB);
	// Ybus LU matrices static storage end 
    
 	// print out and check
/* 	for (i=0; i<n; ++i){ // check the permutation arrays
 	 	std::cout << i << ",rp," << rp[i] << ",rpi," << rpi[i] << ",cp," << cp[i] << ",cpi," << cpi[i] << std::endl;
 	} 
	
	for (i=0; i<n; ++i){ // check the permutation arrays
 	 	std::cout << i << ",ldiag," << ldiag[i] << ",cscale," << cscale[i] << ",rows," << cp[i] << ",cols," << cpi[i] << std::endl;
 	}   */
  
  //std::cout << " ======================== Get L Factor ========================"<<std::endl;
 	/*for (i=0; i<l_nnz; ++i){
     std::cout<< i << ",lx,"<< lx[i] << ",li,"<< li[i] << std::endl; 
  }
   
  for (i=0; i<n+1; ++i){
     std::cout<< i << ",lp,"<< lp[i] << std::endl;
  }
   
  //std::cout << "\n ======================== Get U Factor ========================"<<std::endl;
 	for (i=0; i<u_nnz; ++i){
     std::cout<< i << ",ux,"<< ux[i] << ",ui,"<< ui[i] << std::endl;
  }
   
  for (i=0; i<n+1; ++i){
 		std::cout<< i << ",up,"<< up[i] << std::endl;
 	}*/

 	// check arrays
  //for (i=0; i<n; ++i){
 	// 	std::cout<< i << ",\t diag:"<< ldiag[i] << ",\t cscale:"<< cscale[i] << ",\t rows:"<< rows[i] << ",\t cols:"<< cols[i] << std::endl;
  //}
  //for (i=0; i<n; ++i){
 	// 	std::cout<< i << ",rp," << rp[i]  << ",cpi," << cpi[i] << ",rows,"<< rows[i] << ",cols," << cols[i] <<std::endl;
  //}  
 
 	// // multiply U factor by cols (which is )
 	// for (i=0; i<n; ++i){
 	// 	for (j=up[i]; j<up[i+1]; ++j){
 	// 		p=rp[piv[ui[j]]]; // this may be rp!!!!!!!!!!!
 	// 		ux[j] = ux[j]/cols[p]; // include the scaling factor into L factor
 	// 	}
 	// }
 
 	// ================================= Get the pivoting vectors =================================
/* 	for (i=0; i<n; ++i){
 		std::cout<< i << ",\t pivot:"<< pivot[i] << ",\t pivot_inv:"<< pivot_inv[i] << std::endl;
 	}  */

  //if (insert_LU == 1)
  //{  
    //get the factorized LU values and find the row and col before permutation
    // for (i = 0; i < n; ++i){
      // row = rp[i];
      // LUrow = row+1;
      
      // gVertex_p += T_vertex_p(i+1, lp[i+1]-lp[i], up[i+1]-up[i], rp[i], cpi[i], rows[i], cols[i], rp[i]+1); 
      // // process all non-zeros in L matrix on row i
      // for (j = lp[i]; j < lp[i+1]; ++j){
        // col = cp[li[j]];             
        // LUcol = col+1;
        
        // gMatrix_p_L += T_p_L_matrix((i+1)*100000+(li[j]+1), li[j], lx[j]); 
      // } 
         
      // // process all non-zeros in U matrix on row i
      // for (j = up[i]; j < up[i+1]; ++j){
        // col = cp[ui[j]];
        // LUcol = col+1; 
        
        // gMatrix_p_U += T_p_U_matrix((i+1)*100000+(ui[j]+1), ui[j], ux[j]);          
      // }   
    // }
    // gVertex_p.sort_heap();
    // gMatrix_p_L.sort_heap();
    // gMatrix_p_U.sort_heap();
  // //}
  
  // gVertex_p.sort_heap();
  // gMatrix_p_L.sort_heap();
  // gMatrix_p_U.sort_heap();
	// ----------------------------------------------------------------------------------------------------
	// 								Call GRAPHLU and Factorize B" Matirx
	// ----------------------------------------------------------------------------------------------------
	std::cout << "\n ======================== Start factorizing B'' Matirx ======================== \n"<< std::endl;
	// Initialize the structure for GRAPHLU
	graphlu_Bpp = (SGraphLU *)malloc(sizeof(SGraphLU));
	GraphLU_Initialize(graphlu_Bpp);
 
	// Create Matrix
	GraphLU_CreateMatrix(graphlu_Bpp, n_pp, nnz_pp, ax_pp, ai_pp, ap_pp);
	// graphlu_Bpp->cfgf[0] = 1.;

	// // Set control parameters
	//graphlu_Bpp->cfgi[1] = 0; // 0 is no MC64 scaling
	// 					 // 1 is with MC64 scaling
 
	// Analyze (column/row ordering, scaling. For details please refer to GRAPHLU user's guide)
	GraphLU_Analyze(graphlu_Bpp);
	printf("analysis time: %.8g\n", graphlu_Bpp->stat[0]);
  
	// creates the task scheduler for parallel LU factorization. If you want to run 
	// parallel factorization or parallel re-factorization, it should be called after GraphLU Analyze.
	ret = GraphLU_CreateScheduler(graphlu_Bpp);
	printf("time of creating scheduler: %.8g\n", graphlu_Bpp->stat[4]);
	printf("suggestion: %s.\n", ret==0?"parallel":"sequential");
	
	// This function creates threads for parallel computation. The second argument (thread)
	// specifies the number of threads, including the main thread. The last argument (check)
	// specifies whether to check the number of threads or not.
  if (!ret){ // parallel factorization
	  GraphLU_CreateThreads(graphlu_Bpp, 2, TRUE);
	  printf("total cores: %d, threads created: %d\n", (int)(graphlu_Bpp->stat[9]), (int)(graphlu_Bpp->cfgi[5]));
	
	  // This function binds threads to cores (unbind = FALSE) or unbinds threads from cores (unbind = TRUE).
	  GraphLU_BindThreads(graphlu_Bpp, FALSE);
	
	  // Numerical LU factorization with partial pivoting, parallel
	  error_pp = GraphLU_Factorize_MT(graphlu_Bpp);
	  printf("factorization time: %.8g\n", graphlu_Bpp->stat[1]);
    if (error_pp < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_pp <<std::endl;      
  }
  else{  //Sequential factorization 
	  error_pp = GraphLU_Factorize(graphlu_Bpp);
    printf("Factorization time: %.8g\n", graphlu_Bpp->stat[1]);
    if (error_pp < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_pp <<std::endl; 
  }

  real__t *lx_Bpp, *ux_Bpp; 
  uint__t *li_Bpp, *ui_Bpp; 
  size_t *lp_Bpp, *up_Bpp; 
 	//uint__t fctnnz_pp; // number of nonzeros after the matrix is factorized
	//size_t l_nnz_pp, u_nnz_pp; // number of nonzeros in L and U factors
  // get the permutation arrays, please refer to graphlu.h for details
	uint__t *rp_Bpp, *cp_Bpp, *rpi_Bpp, *cpi_Bpp, l_nnz_Bpp, u_nnz_Bpp; //row (column) permutation, row (column) permutation inverse
  real__t *ldiag_Bpp, *cscale_Bpp, *rows_Bpp, *cols_Bpp;
  int__t *pivot_Bpp, *pivot_inv_Bpp; 
  
  lx_Bpp = ux_Bpp = NULL; 
  li_Bpp = ui_Bpp = NULL; 
  lp_Bpp = up_Bpp = NULL; 
  
  
  GraphLU_DumpLU(graphlu_Bpp, &lx_Bpp, &li_Bpp, &lp_Bpp, &ux_Bpp, &ui_Bpp, &up_Bpp); 
   
   // Get the number of nonzeros in the factorized matrix
 	//fctnnz_pp = graphlu_Bpp->lu_nnz;
 	l_nnz_Bpp = graphlu_Bpp->l_nnz;
 	u_nnz_Bpp = graphlu_Bpp->u_nnz;
 
 	std::cout << " ======================== Number of Total nonzeros after factorization is: "<< graphlu_Bpp->lu_nnz << "========================"<<std::endl;
 	std::cout << " ======================== Number of nonzeros in L: "<< graphlu_Bpp->l_nnz << "========================"<<std::endl;
 	std::cout << " ======================== Number of nonzeros in U: "<< graphlu_Bpp->u_nnz << "========================"<<std::endl;
 	
  // get the permutation arrays, rp and cp may be different if the matrix is not symmetric
	rp_Bpp = graphlu_Bpp->row_perm;
	rpi_Bpp = graphlu_Bpp->row_perm_inv;
	cp_Bpp = graphlu_Bpp->col_perm;
	cpi_Bpp = graphlu_Bpp->col_perm_inv;

	ldiag_Bpp = graphlu_Bpp->ldiag; // diagnal elements
	cscale_Bpp = graphlu_Bpp->cscale; // sclaing factors
	rows_Bpp = graphlu_Bpp->row_scale; 
	cols_Bpp = graphlu_Bpp->col_scale_perm;
	pivot_Bpp = graphlu_Bpp->pivot; 
	pivot_inv_Bpp = graphlu_Bpp->pivot_inv; // pivoting array   \
	
	// [Chen Yuan] static L_Bpp and U_Bpp matrix    

	CSRMatrix *Bpp = SingletonMatrixInterface::getMatrixBpp();
	CSRMatrix *Bpp_L = SingletonMatrixInterface::getMatrixLpp();
	CSRMatrix *Bpp_U = SingletonMatrixInterface::getMatrixUpp();
	//CSRMatrix *Bpp_LineQ1 = SingletonMatrixInterface::getMatrixLineQ1();
	Bpp->loadMatrix(n_pp, nnz_pp, ap_pp, ai_pp, ax_pp);
	Bpp_L->loadMatrix(n, l_nnz_Bpp, lp_Bpp, li_Bpp, lx_Bpp, rp_Bpp, cpi_Bpp, rows_Bpp, cols_Bpp);
	Bpp_U->loadMatrix(n, u_nnz_Bpp, up_Bpp, ui_Bpp, ux_Bpp);
	//Bp_LineQ1->loadMatrix(n, nnz, ap, ai, line_Q1);
	// Bpp LU matrices static storage end
    
 	// print out and check
 	//for (i=0; i<n; ++i){ // check the permutation arrays
 	// 	std::cout << "rp_pp," << rp_pp[i] << ",rpi_pp," << rpi_pp[i] << ",cp_pp,"<< cp_pp[i] << ",cpi_pp," << cpi_pp[i] << std::endl;
 	//} 
  
  //std::cout << " ======================== Get L Factor ========================"<<std::endl;
 	/*for (i=0; i<l_nnz_pp; ++i){
    std::cout<< i << ",lx_pp,"<< lx_pp[i] << ",li_pp,"<< li_pp[i] << std::endl; 
  }
   
  for (i=0; i<n+1; ++i){
    std::cout<< i << ",lp_pp,"<< lp_pp[i] << std::endl;
  }
   
   //std::cout << "\n ======================== Get U Factor ========================"<<std::endl;
 	for (i=0; i<u_nnz_pp; ++i){
    std::cout<< i << ",ux_pp,"<< ux_pp[i] << ",ui_pp,"<< ui_pp[i] << std::endl;
  }
   
 	for (i=0; i<n+1; ++i){
 		std::cout<< i << ",up_pp,"<< up_pp[i] << std::endl;
 	}*/
 
 	// check arrays
  //for (i=0; i<n; ++i){
 	// 	std::cout<< i << ",\t diag_pp:"<< ldiag_pp[i] << ",\t cscale_pp:"<< cscale_pp[i] << ",\t rows_pp:"<< rows_pp[i] << ",\t cols_pp:"<< cols_pp[i] << std::endl;
  //}
  //for (i=0; i<n; ++i){
 	// 	std::cout<< i << ",rp_pp," << rp_pp[i]  << ",cpi_pp," << cpi_pp[i] << ",rows_pp,"<< rows_pp[i] << ",cols_pp," << cols_pp[i] <<std::endl;
  //}   
 
 	// // multiply U factor by cols (which is )
 	// for (i=0; i<n; ++i){
 	// 	for (j=up[i]; j<up[i+1]; ++j){
 	// 		p=rp[piv[ui[j]]]; // this may be rp!!!!!!!!!!!
 	// 		ux[j] = ux[j]/cols[p]; // include the scaling factor into L factor
 	// 	}
 	// }
 
 	// ================================= Get the pivoting vectors =================================
 	//for (i=0; i<n; ++i){
 	//	std::cout<< i << ",\t pivot_pp:"<< pivot_pp[i] << ",\t pivot_inv_pp:"<< pivot_inv_pp[i] << std::endl;
 	//}
  
  //if (insert_LU == 1)
  //{      	
    //get the factorized LU values and find the row and col before permutation
    // for (i = 0; i < n; ++i){
      // row = rp_pp[i];
      // LUrow = row+1;
      
      // gVertex_pp += T_vertex_pp(i+1, lp_pp[i+1]-lp_pp[i], up_pp[i+1]-up_pp[i], rp_pp[i], cpi_pp[i], rows_pp[i], cols_pp[i], rp_pp[i]+1); 
      // // process all non-zeros in L matrix on row i
      // for (j = lp_pp[i]; j < lp_pp[i+1]; ++j){
        // col = cp_pp[li_pp[j]];
        // LUcol = col+1;
        
        // gMatrix_pp_L += T_pp_L_matrix((i+1)*100000+(li_pp[j]+1), li_pp[j], lx_pp[j]); 
      // } 
      
      // // process all non-zeros in U matrix on row i
      // for (j = up_pp[i]; j < up_pp[i+1]; ++j){
        // col = cp_pp[ui_pp[j]];
        // LUcol = col+1;
        
        // gMatrix_pp_U += T_pp_U_matrix((i+1)*100000+(ui_pp[j]+1), ui_pp[j], ux_pp[j]);
      // }    
    // }
    // gVertex_pp.sort_heap();
    // gMatrix_pp_L.sort_heap();
    // gMatrix_pp_U.sort_heap();          
  //}
  
  // Get the time for factorizing B' and B"
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	
	printf("\n\n============================================================================================== ");
	std::cout << "Time for GRAPHLU to factorize B' and B'':: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");
  
    std::cout << "n - number of buses:" << n << std::endl;
if(error_p >= 0 && error_pp >= 0){
    //continue to do the iterations only if there is no error from factorization
  	printf("\n\n============================================================================================== ");
  	std::cout << "\n Start iteratively updating deltaP and deltaQ " << std::endl;
  	printf("\n================================================================================================\n ");
  
  	gettimeofday(&t2_st, 0);
   
  	for (iter=0; iter<maxiter; ++iter){
  		// update deltaP and deltaQ
  		maxDeltaP=0;
  		maxDeltaQ=0;
  
  		// std::cout << " ======================== Update deltaP and deltaQ ========================"<<std::endl;
        gettimeofday(&t3_st, 0);
  		// Calculate deltaP and deltaQ to update Va
  		for (i=0; i<n; ++i){
  			deltaP[i] = 0;
  			deltaQ[i] = 0;
  
  			if (btype[i]!=3){
  				deltaP[i] = Pn[i];
  				if (btype[i]<2)
  					deltaQ[i] = Qn[i];
  			}
  
  			// Calculate network injections
  			if (btype[i]!=3){
  				for (j=ep[i]; j<ep[i+1]; ++j){
  					p=ei[j];
					if (iter == 0){        
					  //if this is the first iteraton, calculate the cosine and sine terms        
					  cosine_array[j] = cos(Va[i]-Va[p]);
					  sine_array[j] = sin(Va[i]-Va[p]);
					}    
					deltaP[i] += - Vm[i]*Vm[p]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);        
					if (btype[i]<2) // calculate Q for PQ buses
					deltaQ[i] += - Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]); 
					
				    // if(iter==0){
						// std::cout << "B: i= " << i << ", j= " << j << ", p= " << p << ", B, " << eB[j] << std::endl; 
					// }					
  				}
  			} 			
		
  			// Get max P and max Q
  			if (abs(deltaP[i]) > maxDeltaP)
  				maxDeltaP = abs(deltaP[i]);
  			if (abs(deltaQ[i]) > maxDeltaQ)
  				maxDeltaQ = abs(deltaQ[i]);
  
  			deltaP[i] /= Vm[i];
  			deltaQ[i] /= Vm[i];
  
  			// check output
 			  //std::cout << "Iter" << iter << ", deltaP" << i << ", " << deltaP[i]<< ", deltaQ" << i << ", " << deltaQ[i]<< std::endl;
  		}
  
  		// std::cout << "Iter" << iter << " Updated for Va, maxDeltaP, " << maxDeltaP << ", maxDeltaQ, " << maxDeltaQ << std::endl;
   	  
		gettimeofday(&t3_end, 0);
		seconds=t3_end.tv_sec  - t3_st.tv_sec; 
		useconds = t3_end.tv_usec  - t3_st.tv_usec;
		printf("\n\n==============================================================================================\n ");
		std::cout << "Time for updating deltaP and deltaQ (iteration):: " << iter << " :" << (seconds*1000000 + useconds) << " us" << std::endl;
		printf("\n================================================================================================\n ");
      
		// Decide if the program converges	
  		if ( maxDeltaP < max_change_ex && maxDeltaQ < max_change_ex){
			result = "OK";
  			break;
  		}
      
		//reset the max mismatche
		maxDeltaP = 0;
		maxDeltaQ = 0;
      	  
   		// solve for deltaVa
  		// std::cout << " ======================== Solve for V angle ========================"<<std::endl;
      
        // A*x = b
        // multiply deltaP with row scaling (rows) to get b
    	for (i=0; i<n; ++i){
			b[i]=deltaP[rp_Bp[i]]*rows_Bp[rp_Bp[i]];
			//std::cout << "rp_Bp:" << rp_Bp[i] << ", rows_Bp:" << rows_Bp[i] << ", b:" << b[i] << std::endl;
    	}
     
    	// Full forward substitution 
    	for (i=0; i<n; ++i)
    	{
    		sum=0.;
    		diag=0.;
    		for(p=lp_Bp[i]; p< lp_Bp[i+1]; ++p){
    			if (i!=li_Bp[p]){ // if it is not diagonal element
    				j=li_Bp[p];
    				sum += lx_Bp[p] * b[j];
    			}
    			else{
    				diag = lx_Bp[p];
    			}
				
    		}
			// print indices of V angle
			// std::cout << "lp_Bp:" << lp_Bp[i] << std::endl;
    		b[i] -= sum;
    		b[i] /= diag;
    	} 
     
    	// Full backward substitution 
    	for (i=n-1; i>=0; --i)
    	{
    		sum=0.;
    		for(p=up_Bp[i]; p< up_Bp[i+1]; ++p){
    			if (i!=ui_Bp[p]){ // if it is not diagnal element
    				sum += ux_Bp[p] * b[ui_Bp[p]]; 
    			}
    		}  
    		b[i] -= sum; 
    	}      
    	// permute the array back to the original order
			//for (i=0; i<n; ++i)
		  //{
			//	delta[i]=b[cpi[i]];
		  //  //std::cout << "delta: " << i << ", " << delta[i] << std::endl;           
			//}
			
		  // Update V angle (Va)
		for (i=0; i<n; ++i){ 
			//Va[i] -= delta[i];  
			Va[i] -= b[cpi_Bp[i]]*cols_Bp[cpi_Bp[i]];  // in radian    
		}
	  
	    Bp->loadMatrix(n, nnz, ap, ai, ax, btype, Va, Vm, Pn);   // load Va and Bp
	    std::cout << "Finish Bp store" << std::endl;
     
  		// std::cout << " ======================== Update deltaP and deltaQ ========================"<<std::endl;
        gettimeofday(&t3_st, 0);
  		// Calculate deltaP and deltaQ to update Vm
  		for (i=0; i<n; ++i){
  			deltaP[i] = 0;
  			deltaQ[i] = 0;
  
  			if (btype[i]!=3){
  				deltaP[i] = Pn[i];
  				if (btype[i]<2)
  					deltaQ[i] = Qn[i];
  			}
  
  			// Calculate network injections
  			if (btype[i]!=3){
  				for (j=ep[i]; j<ep[i+1]; ++j){
  					p=ei[j];
					// Va has been updated, update the cosine and sine terms 
					cosine_array[j] = cos(Va[i]-Va[p]);
					sine_array[j] = sin(Va[i]-Va[p]);        
					deltaP[i] += - Vm[i]*Vm[p]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);        
					if (btype[i]<2) // calculate Q for PQ buses
					{
						deltaQ[i] += - Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]); 
					}
						
				}
  			}
  
  			// Get max P and max Q
  			if (abs(deltaP[i]) > maxDeltaP)
  				maxDeltaP = abs(deltaP[i]);
  			if (abs(deltaQ[i]) > maxDeltaQ)
  				maxDeltaQ = abs(deltaQ[i]);
  
  			deltaP[i] /= Vm[i];
  			deltaQ[i] /= Vm[i];
  
  			// check output
  			// std::cout << "Iter" << iter << ", deltaP" << i << ", " << deltaP[i]<< ", deltaQ" << i << ", " << deltaQ[i]<< std::endl;
        }
  
  		// std::cout << "Iter" << iter << " Updated for Vm, maxDeltaP, " << maxDeltaP << ", maxDeltaQ, " << maxDeltaQ << std::endl;
  
        gettimeofday(&t3_end, 0);
  	    seconds=t3_end.tv_sec  - t3_st.tv_sec; 
  	    useconds = t3_end.tv_usec  - t3_st.tv_usec;
  	    printf("\n\n==============================================================================================\n ");
  	    std::cout << "Time for updating deltaP and deltaQ (iteration):: " << iter << " :" << (seconds*1000000 + useconds) << " us" << std::endl;
  	    printf("\n================================================================================================\n ");
       
  		// Decide if the program converges	
  		if ( maxDeltaP < max_change_ex && maxDeltaQ < max_change_ex){
        result = "OK";
  			break;
  		}

  		// solve for deltaVm
  		// std::cout << " ======================== Solve for V magnitude ========================"<<std::endl;
     
        // A*x = b
        // multiply deltaQ with row scaling (rows_pp) to get b
    	for (i=0; i<n; ++i){
			b[i]=deltaQ[rp_Bpp[i]]*rows_Bpp[rp_Bpp[i]];
			//std::cout << "rp_Bpp:" << rp_Bpp[i] << ", rows_Bpp:" << rows_Bpp[i] << " ,b:" << b[i] << std::endl;
    	}
     
    	// Full forward substitution 
    	for (i=0; i<n; ++i){
    		sum=0.;
    		diag=0.;
    		for(p=lp_Bpp[i]; p< lp_Bpp[i+1]; ++p){
    			if (i!=li_Bpp[p]){ // if it is not diagnal element
    				j=li_Bpp[p];
    				sum += lx_Bpp[p] * b[j]; 
    			}
    			else{
    				diag = lx_Bpp[p];
    			}
    		}
			// prinf indices of V angle
			// std::cout << "lp_Bpp:" << lp_Bpp[i] << std::endl;
    		b[i] -= sum; 
    		b[i] /= diag;
    	} 
     
    	// Full backward substitution 
    	for (i=n-1; i>=0; --i){
    		sum=0.;
    		for(p=up_Bpp[i]; p< up_Bpp[i+1]; ++p){
    			if (i!=ui_Bpp[p]){ // if it is not diagnal element
    				sum += ux_Bpp[p] * b[ui_Bpp[p]]; 
    			}
    		}    
    		b[i] -= sum; 
    	}      
    	// permute the array back to the original order
    	//for (i=0; i<n; ++i)
      //{
    	//	delta[i]=b[cpi_pp[i]];
      //  //std::cout << "delta: " << i << ", " << delta[i] << std::endl;           
  		//}
  
  		// Update V magnitude (Vm)
  		for (i=0; i<n; ++i){
			//Vm[i] -= delta[i];
			Vm[i] -= b[cpi_Bpp[i]]*cols_Bpp[cpi_Bpp[i]];
		}       
  	}
	// This is the end of power flow iteration
   
  	gettimeofday(&t2_end, 0);
  	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
  	useconds = t2_end.tv_usec  - t2_st.tv_usec;
  	
  	printf("\n\n============================================================================================== ");
  	std::cout << "Time for performing power flow iteration: " << (seconds*1000000 + useconds) << " us" << std::endl;
  	printf("\n================================================================================================ ");
  
	// ----------------------------------------------------------------------------------------------------
  	// 								Store the Result back to the array
  	// ----------------------------------------------------------------------------------------------------
  	// Get solved Vm and Va back into HeapAccum
	for (ii=0; ii<n; ++ii){
		gVertex.data_[ii].Va = Va[ii]; // in radian
		gVertex.data_[ii].Vm = Vm[ii];
		//std::cout << "Final, " << ii+1 << ", Vm, " << Vm[ii]<< ", Va" << ii+1 << ", " << Va[ii]/pi_value*180<< std::endl;
	}
	 	
	// ----------------------------------------------------------------------------------------------------
  	// 								Calculate the sensitivity factor STI
  	// ----------------------------------------------------------------------------------------------------
	// Arrange the operation point power injection
	for (i=0; i<n; ++i){
		if (btype[i]!=3){
			P_operation[i] = Pn[i];
			// P_operation[i] = Pn[i]/Vm[i];
			
			if (btype[i]<2)
				Q_operation[i] = Qn[i];
				// Q_operation[i] = Qn[i]/Vm[i];
				
		}
		// P_operation[i] /= Vm[i];
		// Q_operation[i] /= Vm[i];
	}	 
	
	// std::cout << " ======================== Solve for V angle ========================"<<std::endl;	
	// A*x = b
    // multiply deltaP with row scaling (rows) to get b
	for (i=0; i<n; ++i){
		lambda_b[i]=P_operation[rp_Bp[i]]*rows_Bp[rp_Bp[i]];	
		// //print row/column indices of V angle
		// std::cout << "rp_Bp:" << rp_Bp[i] << ", rows_Bp:" << rows_Bp[rp_Bp[i]] << ", lambda_b (P):" << lambda_b[i] << std::endl;
	}
 
	// Full forward substitution 
	for (i=0; i<n; ++i)
	{
		sum=0.;
		diag=0.;
		for(p=lp_Bp[i]; p< lp_Bp[i+1]; ++p){
			if (i!=li_Bp[p]){ // if it is not diagonal element
				j=li_Bp[p];
				sum += lx_Bp[p] * lambda_b[j];
			}
			else{
				diag = lx_Bp[p];
			}		 
			
			// std::cout << "i:" << i << " p:" << p << " lx_Bp:" << lx_Bp[i] << " lambda_b(P):" << lambda_b[li_Bp[p]] << std::endl;			
		}
		// print indices of V angle
		// std::cout << "lp_Bp:" << lp_Bp[i] << std::endl;
		lambda_b[i] -= sum;
		lambda_b[i] /= diag;
	} 
 
	// Full backward substitution 
	for (i=n-1; i>=0; --i)
	{
		sum=0.;
		for(p=up_Bp[i]; p< up_Bp[i+1]; ++p){
			if (i!=ui_Bp[p]){ // if it is not diagnal element
				sum += ux_Bp[p] * lambda_b[ui_Bp[p]]; 			
			}
			
			// std::cout << "i:" << i << " p:" << p << " ux_Bp:" << ux_Bp[i] <<" lambda_b(P):" << lambda_b[ui_Bp[p]] << std::endl;	
		}  
		lambda_b[i] -= sum; 	
	}      
	
	for (i=0; i<n; ++i){ 
        dVa_sens[i] -= lambda_b[cpi_Bp[i]]*cols_Bp[cpi_Bp[i]];  // in radian  
		
		std::cout << "i:" << i << " cpi_Bp:" << cpi_Bp[i] << " lambda:" << lambda_b[cpi_Bp[i]] << " cols_Bp:" << cols_Bp[cpi_Bp[i]] << std::endl;
    }  
	
	// std::cout << " ======================== Solve for V magnitude ========================"<<std::endl;
 
	// A*x = b
	// multiply deltaQ with row scaling (rows_pp) to get b
	for (i=0; i<n; ++i){
		lambda_b[i]=Q_operation[rp_Bpp[i]]*rows_Bpp[rp_Bpp[i]];	
		// std::cout << "rp_Bp:" << rp_Bp[i] << ", rows_Bp:" << rows_Bp[rp_Bp[i]] << ", lambda_b (Q):" << lambda_b[i] << std::endl;
	}
 
	// Full forward substitution 
	for (i=0; i<n; ++i)
	{
		sum=0.;
		diag=0.;
		for(p=lp_Bpp[i]; p< lp_Bpp[i+1]; ++p){
			if (i!=li_Bpp[p]){ // if it is not diagnal element
				j=li_Bpp[p];
				sum += lx_Bpp[p] * lambda_b[j]; 
			}
			else{
				diag = lx_Bpp[p];
			}
		}
	
		lambda_b[i] -= sum; 
		lambda_b[i] /= diag;
	} 
 
	// Full backward substitution 
	for (i=n-1; i>=0; --i)
	{
		sum=0.;
		for(p=up_Bpp[i]; p< up_Bpp[i+1]; ++p){
			if (i!=ui_Bpp[p]){ // if it is not diagnal element
				sum += ux_Bpp[p] * lambda_b[ui_Bpp[p]]; 
			}
		}

		lambda_b[i] -= sum; 
	}      
			// Update V magnitude (Vm)
	for (i=0; i<n; ++i){
		dVm_sens[i] -= lambda_b[cpi_Bpp[i]]*cols_Bpp[cpi_Bpp[i]];
		// std::cout << "i:" << i << " cpi_Bpp:" << cpi_Bpp[i] << " lambda:" << lambda_b[cpi_Bpp[i]] << " cols_Bpp:" << cols_Bpp[cpi_Bpp[i]] << std::endl;
	}       
	
	// Calculate the equivalent impedance and STI
	for (i=0; i<n; ++i){	
		if (abs(dVm_sens[i]-0)>0.0001){
			Zth[i]=sqrt((dVm_sens[i]/Vm[i])*(dVm_sens[i]/Vm[i])+dVa_sens[i]*dVa_sens[i]);
			ZL[i]=sqrt((1-dVm_sens[i]/Vm[i])*(1-dVm_sens[i]/Vm[i])+dVa_sens[i]*dVa_sens[i]);
			STI[i]=Zth[i]/ZL[i];
		}				
		  std::cout << "Vm: " << Vm[i] << " Va: " << Va[i]  << " Pn: " << Pn[i] << " Qn: " << Qn[i] << " Zth: " << Zth[i] << " STI: " << STI[i] << std::endl;
	}	
  
	// Get solved Zth and STI back into HeapAccum
	for (ii=0; ii<n; ++ii){
		gVertex.data_[ii].Zth = Zth[ii]; // in radian
		gVertex.data_[ii].STI = STI[ii];
		//std::cout << "Final, " << ii+1 << ", Vm, " << Vm[ii]<< ", Va" << ii+1 << ", " << Va[ii]/pi_value*180<< std::endl;
	}
}
	else{
    //factorization failed
 	  result = "FAILED";
    printf("\n\n============================================================================================== ");
  	std::cout << "Factorization FAILED" << std::endl;
  	printf("\n================================================================================================ ");  
}

	
EXIT:
	// std::cout << "\n free1 " << std::endl;
	GraphLU_Destroy(graphlu_Bp);
	GraphLU_Destroy(graphlu_Bpp);
	// std::cout << "\n free2 " << std::endl;
	free(ax);
	free(ai);
	free(ap);
	free(ax_pp);
	free(ai_pp);
	free(ap_pp);
	free(graphlu_Bp);
	free(graphlu_Bpp);
	// std::cout << "\n free3 " << std::endl;
		
	free(eG);
	free(eB);
	free(ei);
	free(ep);

	free(deltaP);
	free(deltaQ);
	free(P_operation);
	free(Q_operation);	
	free(Vm);
	free(Va);	
	free(dVm_sens);
	free(dVa_sens);	
	free(Pn);
	free(Qn);
    free(Zth);
	free(ZL);
	free(STI);	
  
 	free(lx_Bp);
	free(li_Bp);
	free(lp_Bp);
    free(ux_Bp);
	free(ui_Bp);
	free(up_Bp);  
 	free(lx_Bpp);
	free(li_Bpp);
	free(lp_Bpp);
 	free(ux_Bpp);
	free(ui_Bpp);
	free(up_Bpp);

	free(cosine_array);
	free(sine_array);
	free(btype);
	free(delta);
	free(b);
	free(lambda_b);
	free(line_Q1);
	
    printf("\n\n----------------------------------------------------------------------------------------");
	printf("\t\t End of Running GRAPHLU_fdpf_LU_factorize_EMS C function!");
	printf("\t\t ----------------------------------------------------------------------------------------\n\n");	
 
  return result;
}





// **********************************************************************************************************************************
// Created by: Ting Chan, ting.chan@geirina.net
// Date: 10/26/2016
// This code performs fast-decoupled power flow analysis. It is used as a benchmark to assess the time it takes to run a fast-decoupled
// power flow in C++ with GRAPHLU. This version (v6) takes HeapAccum as inputs instead of MapAccum. The input HeapAcuum is assumed to be
// unsorted, so sorting them is needed before assigning the values to local arrays. Differences between this version (v6) and v2 is 
// the number of input HeapAccum. In this version, only 4 heapAccums are needed (vertex, Bp, Bpp and e). With fewer HeapAccums, the sorting
// time is reduced. This version does not require calling GRAPHLU_build_Ap2 for the pointer structures (i.e. Bp_p, Bpp_p and ep).
// This version (v7) is based on v6 but there are only two input HeapAccums - one for vertex, another one for Ybus, B' and B". 
// History: 
// 10/26/2016: [Ting Chan] Added return type as a string to indicate whether power flow is converged.
// **********************************************************************************************************************************
template <typename T_vertex_tuple, typename vertex_comp, typename T_edge_all_tuple, typename edge_all_comp>
inline string GRAPHLU_fdpf_v7 (int64_t& gBp, int64_t& gBpp, HeapAccum<T_vertex_tuple, vertex_comp>& gVertex, 
              HeapAccum<T_edge_all_tuple, edge_all_comp>& gEdge_all, MaxAccum<int> maxiter, MaxAccum<double> maxchange) {

	// extern size_t g_si, g_sd, g_sp;
	printf("\n\n------------------------------------------------------------------------------------------- ");
	printf("\nStart Running GRAPHLU_fdpf_v7 C function!\n");
  std::cout << "Bp Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gBp << std::endl;
  std::cout << "Bpp Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gBpp << std::endl;
  std::cout << "Y bus Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gEdge_all.data_.size() << std::endl;
	printf("-------------------------------------------------------------------------------------------- \n\n");	
	// ------------------------------------------------------------------------------------------------
	// 				Initialize variables and arrays
	// ------------------------------------------------------------------------------------------------
	
	// Initialize arrays and variables
	uint__t n, nnz, n_pp, nnz_pp, n_e, nnz_e;
	int ret, i, j, p, iter;
	real__t maxDeltaP=0, maxDeltaQ=0, max_change_ex=maxchange;
	SGraphLU *graphlu, *graphlu_pp;
  
  string result = "FAILED";
  
	const double pi_value = 3.141592653589793;
	// real__t *x, err;	
	// uint__t fctnnz; // number of nonzeros after the matrix is factorized

	// Get the dimension and the nnz of the matrix B' and B"
	n=gVertex.data_.size();	nnz=gBp; n_pp=gVertex.data_.size(); nnz_pp=gBpp;
  n_e=gVertex.data_.size(); 	nnz_e=gEdge_all.data_.size(); // get the size of the Y bus addmittance
  
	
	//// Convert vectors to GRAPHLU Data Structure (pointers)
	real__t *ax, *ax_pp, *eG, *eB, *deltaP, *deltaQ, *b, *Vm, *Va, *Pn, *Qn; 
	uint__t *ai, *ai_pp, *ei, *piv, *btype; 
	uint__t *ap, *ap_pp, *ep; 
 
  // arrays to store the sine and cosine terms of the angle difference
  // values are computed during Jacobian initialization and each iteration 
  real__t *cosine_array, *sine_array;
  cosine_array = (real__t *)malloc(sizeof(real__t)*(nnz_e));
  sine_array = (real__t *)malloc(sizeof(real__t)*(nnz_e)); 
	
	ax = (real__t *)malloc(sizeof(real__t)*(nnz)); // values in B' 
	ai = (uint__t *)malloc(sizeof(uint__t)*(nnz)); // column indices of B'
	ap = (uint__t *)malloc(sizeof(uint__t)*(n+1)); // initial row pointers
	// b = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b

	ax_pp = (real__t *)malloc(sizeof(real__t)*(nnz_pp)); // values in B"
	ai_pp = (uint__t *)malloc(sizeof(uint__t)*(nnz_pp)); // column indices of B"
	ap_pp = (uint__t *)malloc(sizeof(uint__t)*(n_pp+1)); // initial row pointers

	eG = (real__t *)malloc(sizeof(real__t)*(nnz_e)); // G values in Y 
	eB = (real__t *)malloc(sizeof(real__t)*(nnz_e)); // B values in Y
	ei = (uint__t *)malloc(sizeof(uint__t)*(nnz_e)); // column indices of Y
	ep = (uint__t *)malloc(sizeof(uint__t)*(n_e+1)); // initial row pointers	

	deltaP = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b
	deltaQ = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b

	Vm = (real__t *)malloc(sizeof(real__t)*(n)); 
	Va = (real__t *)malloc(sizeof(real__t)*(n)); 

	Pn = (real__t *)malloc(sizeof(real__t)*(n)); 
	Qn = (real__t *)malloc(sizeof(real__t)*(n));

	btype = (uint__t *)malloc(sizeof(uint__t)*(n));
  
	std::cout << " ======================== Initialization of ararys used to store the factorized matrix and LU ========================"<< std::endl;

	struct timeval t2_st, t2_end, t3_st, t3_end; long seconds, useconds;

  //initialization to 0
  // [tc] use memset here
  memset(Vm, 0, sizeof(real__t)*(n));
  memset(Va, 0, sizeof(real__t)*(n));
  memset(deltaP, 0, sizeof(real__t)*(n));
  memset(deltaQ, 0, sizeof(real__t)*(n));
	
  // =================================== Sort all input HeapAccum =================================
  gettimeofday(&t3_st, 0);
  gettimeofday(&t2_st, 0);
  // Input HeapAccums are assumed to be unsorted. Sort them before assigning values to
  // local arrays
  gVertex.sort_heap();
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gVertex HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
  
  gettimeofday(&t2_st, 0);  
  gEdge_all.sort_heap();
 	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gEdge_all HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
     
  // get the time for sorting the HeapAccums 
	gettimeofday(&t3_end, 0);
	seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	useconds = t3_end.tv_usec  - t3_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort all HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================\n "); 
	
	// ========================================= Convert Ap, Ap_pp, ep and the Vertex =========================================
	gettimeofday(&t2_st, 0); 
  int i_p, i_pp;
	int ii = 1; // counter for the array  
  // Assign value to the first position of the pointer array 
  ap[0] = 0;
  ap_pp[0] = 0;
  ep[0] = 0;
	for (i=0;i<gVertex.size();i++){
    ap[ii] = gVertex.data_[i].Bp_p + ap[ii-1];
    ap_pp[ii] = gVertex.data_[i].Bpp_p + ap_pp[ii-1];
    ep[ii] = gVertex.data_[i].ep + ep[ii-1];
    ii++;
    Vm[i] = gVertex.data_[i].Vm;
    Va[i] = gVertex.data_[i].Va;
    Pn[i] = gVertex.data_[i].Pn;
    Qn[i] = gVertex.data_[i].Qn;
    btype[i] = gVertex.data_[i].bustype;
	}  

	// print out ep   
	for (i=0;i<=gVertex.size();i++){
		std::cout<< i << ", ep," << ep[i] << std::endl;
	}
	
	// print out ap, app
	for (i=0;i<=gVertex.size();i++){
		std::cout<< i << ", ap," << ap[i] << ", ap_pp," << ap_pp[i] << std::endl;
	}
	
	
 	// ========================================= Convert Ybus (ei, eG and eB), B' (ei and Bp_x) and B" (ei and Bpp_x) =========================================
	i_p = 0;
  i_pp = 0;
  for (ii=0; ii<gEdge_all.size(); ++ii) {
    ei[ii] = gEdge_all.data_[ii].ei;
    eG[ii] = gEdge_all.data_[ii].eG;
    eB[ii] = gEdge_all.data_[ii].eB; 
    if(gEdge_all.data_[ii].Bp_x != 0)
    {
      ai[i_p] = gEdge_all.data_[ii].ei;
      ax[i_p] = gEdge_all.data_[ii].Bp_x;
      i_p++;
    }
    if(gEdge_all.data_[ii].Bpp_x != 0)
    {
      ai_pp[i_pp] = gEdge_all.data_[ii].ei;
      ax_pp[i_pp] = gEdge_all.data_[ii].Bpp_x;
      i_pp++;
    }
  } 	
  
  // print out eG, eB, ei  
  for (ii=0; ii<gEdge_all.size(); ++ii) {
	  std::cout << ii << ", ei," << ei[ii] << ", eG," << eG[ii] << ", eB," << eB[ii] << std::endl;
  }
  
  
  // print out ai, ax
  for (ii=0; ii<nnz; ++ii) {
	  std::cout << ii << ", ai," << ai[ii] << ", ax," << ax[ii] << std::endl;
  }
  
  // print out ai_pp, ax_pp
  for (ii=0; ii<nnz_pp; ++ii) {
	  std::cout << ii << ", ai_pp," << ai_pp[ii] << ", ax_pp," << ax_pp[ii] << std::endl;
  }
  
  // Done converting all input HeapAccums to arrays
  
	// Get the time to convert the data between containers and arrays
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to convert data to GRAPHLU time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");		
	
	// ----------------------------------------------------------------------------------------------------
	// 								Call GRAPHLU and Factorize B' Matirx
	// ----------------------------------------------------------------------------------------------------
  int error_p = 0, error_pp = 0;
	gettimeofday(&t2_st, 0);
	std::cout << "\n ======================== Start factorizing B' Matirx ======================== \n"<< std::endl;
	// Initialize the structure for GRAPHLU
	graphlu = (SGraphLU *)malloc(sizeof(SGraphLU));
	ret = GraphLU_Initialize(graphlu);
	std::cout<< "Initialize " << ret << std::endl;

	// Create Matrix
	ret = GraphLU_CreateMatrix(graphlu, n, nnz, ax, ai, ap);
	// graphlu->cfgf[0] = 1.;
	std::cout<< "CreateMatrix " << ret << std::endl;

	// // Set control parameters
	// graphlu->cfgi[1] = 0; // 0 is no MC64 scaling
	// 					 // 1 is with MC64 scaling

	// Analyze (column/row ordering, scaling. For details please refer to GRAPHLU user's guide)
	GraphLU_Analyze(graphlu);
	printf("analysis time: %.8g\n", graphlu->stat[0]);
	
  // [Ting][begin] comment out if to try sequential factorization 
	// creates the task scheduler for parallel LU factorization. If you want to run 
	// parallel factorization or parallel re-factorization, it should be called after GraphLU Analyze.
	ret = GraphLU_CreateScheduler(graphlu);
	printf("time of creating scheduler: %.8g\n", graphlu->stat[4]);
	printf("suggestion: %s.\n", ret==0?"parallel":"sequential");
	
	// This function creates threads for parallel computation. The second argument (thread)
	// specifies the number of threads, including the main thread. The last argument (check)
	// specifies whether to check the number of threads or not.
  if (!ret){ // parallel factorization 
	  GraphLU_CreateThreads(graphlu, 2, TRUE);
	  printf("total cores: %d, threads created: %d\n", (int)(graphlu->stat[9]), (int)(graphlu->cfgi[5]));
	
	  // This function binds threads to cores (unbind = FALSE) or unbinds threads from cores (unbind = TRUE).
	  GraphLU_BindThreads(graphlu, FALSE);
	
	  // Numerical LU factorization with partial pivoting, parallel
	  error_p = GraphLU_Factorize_MT(graphlu);
	  printf("factorization time: %.8g\n", graphlu->stat[1]);
    if (error_p < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_p <<std::endl;  
  }   
  // [Ting][end] comment out if to try sequential factorization 
  else{
    // Sequential factorization
	  error_p = GraphLU_Factorize(graphlu);
	  printf("Factorization time: %.8g\n", graphlu->stat[1]);
    if (error_p < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_p <<std::endl; 
	}
 
	//std::cout << " ======================== Number of Total nonzeros after factorization is: "<< graphlu->lu_nnz << "========================"<<std::endl;
	//std::cout << " ======================== Number of nonzeros in L: "<< graphlu->l_nnz << "========================"<<std::endl;
	//std::cout << " ======================== Number of nonzeros in U: "<< graphlu->u_nnz << "========================"<<std::endl;
	//std::cout << " ======================== End of LU Factorization Routines ========================"<<std::endl;
	
	// ----------------------------------------------------------------------------------------------------
	// 								Call GRAPHLU and Factorize B" Matirx
	// ----------------------------------------------------------------------------------------------------
	std::cout << "\n ======================== Start factorizing B'' Matirx ======================== \n"<< std::endl;
	// Initialize the structure for GRAPHLU
	graphlu_pp = (SGraphLU *)malloc(sizeof(SGraphLU));
	GraphLU_Initialize(graphlu_pp);

	// Create Matrix
	GraphLU_CreateMatrix(graphlu_pp, n_pp, nnz_pp, ax_pp, ai_pp, ap_pp);
	// graphlu_pp->cfgf[0] = 1.;

	// // Set control parameters
	// graphlu->cfgi[1] = 0; // 0 is no MC64 scaling
	// 					 // 1 is with MC64 scaling
  
	// Analyze (column/row ordering, scaling. For details please refer to GRAPHLU user's guide)
	GraphLU_Analyze(graphlu_pp);
	printf("analysis time: %.8g\n", graphlu_pp->stat[0]);
  
	// [Ting][begin] comment out if to try sequential factorization 
	// creates the task scheduler for parallel LU factorization. If you want to run 
	// parallel factorization or parallel re-factorization, it should be called after GraphLU Analyze.
	ret = GraphLU_CreateScheduler(graphlu_pp);
	printf("time of creating scheduler: %.8g\n", graphlu_pp->stat[4]);
	printf("suggestion: %s.\n", ret==0?"parallel":"sequential");
	
	// This function creates threads for parallel computation. The second argument (thread)
	// specifies the number of threads, including the main thread. The last argument (check)
	// specifies whether to check the number of threads or not.
  if (!ret){ // parallel factorization
	  GraphLU_CreateThreads(graphlu_pp, 2, TRUE);
	  printf("total cores: %d, threads created: %d\n", (int)(graphlu_pp->stat[9]), (int)(graphlu_pp->cfgi[5]));
	
	  // This function binds threads to cores (unbind = FALSE) or unbinds threads from cores (unbind = TRUE).
	  GraphLU_BindThreads(graphlu_pp, FALSE);
	
	  // Numerical LU factorization with partial pivoting, parallel
	  error_pp = GraphLU_Factorize_MT(graphlu_pp);
	  printf("factorization time: %.8g\n", graphlu_pp->stat[1]);
    if (error_pp < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_pp <<std::endl;      
  }
  // [Ting][end] comment out if to try sequential factorization 
  else{ 
    //Sequential factorization 
	  error_pp = GraphLU_Factorize(graphlu_pp);
    printf("Factorization time: %.8g\n", graphlu_pp->stat[1]);
    if (error_pp < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_pp <<std::endl; 
  }
	// // Get the number of nonzeros in the factorized matrix
	// fctnnz = graphlu_pp->lu_nnz;
	// l_nnz = graphlu_pp->l_nnz;
	// u_nnz = graphlu_pp->u_nnz;

	// std::cout << " ======================== Number of Total nonzeros after factorization is: "<< graphlu_pp->lu_nnz << "========================"<<std::endl;
	// std::cout << " ======================== Number of nonzeros in L: "<< graphlu_pp->l_nnz << "========================"<<std::endl;
	// std::cout << " ======================== Number of nonzeros in U: "<< graphlu_pp->u_nnz << "========================"<<std::endl;
	// std::cout << " ======================== End of LU Factorization Routines ========================"<<std::endl;
	
  // Get the time for factorizing B' and B"
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	
	printf("\n\n============================================================================================== ");
	std::cout << "Time for GRAPHLU to factorize B' and B'':: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");
  
  if(error_p >= 0 && error_pp >= 0){
    //continue to do the iterations only if there is no error from factorization
  	printf("\n\n============================================================================================== ");
  	std::cout << "\n Start iteratively updating deltaP and deltaQ " << std::endl;
  	printf("\n================================================================================================\n ");
  
  	gettimeofday(&t2_st, 0);
   
  	for (iter=0; iter<maxiter; ++iter){
  		// update deltaP and deltaQ
  		maxDeltaP=0;
  		maxDeltaQ=0;
  
  		// std::cout << " ======================== Update deltaP and deltaQ ========================"<<std::endl;
      gettimeofday(&t3_st, 0);
  		// Calculate deltaP and deltaQ to update Va
  		for (i=0; i<n; ++i){
  			deltaP[i] = 0;
  			deltaQ[i] = 0;
  
  			if (btype[i]!=3){
  				deltaP[i] = Pn[i];
  				if (btype[i]<2)
  					deltaQ[i] = Qn[i];
  			}
			//std::cout << i << ", ep[i], " <<ep[i] << ", ep[i+1], " << ep[i+1] << std::endl;
  			// Calculate network injections
  			if (btype[i]!=3){
  				for (j=ep[i]; j<ep[i+1]; ++j)
				{
  					p=ei[j];
					if (iter == 0)
					{
					  //if this is the first iteraton, calculate the cosine and sine terms        
					  cosine_array[j] = cos(Va[i]-Va[p]);
					  sine_array[j] = sin(Va[i]-Va[p]);
					}    
					deltaP[i] += - Vm[i]*Vm[p]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);        
					if (btype[i]<2) // calculate Q for PQ buses
					  deltaQ[i] += - Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]); 
					}
					//std::cout << "Iter" << iter << ", deltaP, " << i << ", " << deltaP[i]<< ", deltaQ, " << i << ", " << deltaQ[i]<< std::endl;
				}  
        
  			// Get max P and max Q
  			if (abs(deltaP[i]) > maxDeltaP)
  				maxDeltaP = abs(deltaP[i]);
  			if (abs(deltaQ[i]) > maxDeltaQ)
  				maxDeltaQ = abs(deltaQ[i]);
  
  			deltaP[i] /= Vm[i];
  			deltaQ[i] /= Vm[i];
  
  			// deltaP[i] = deltaP[i]/Vm[i];
  			// deltaQ[i] = deltaQ[i]/Vm[i];
  
  			// check output
 			  //std::cout << "Iter" << iter << ", deltaP, " << i << ", " << deltaP[i]<< ", deltaQ, " << i << ", " << deltaQ[i]<< std::endl;
			  std::cout << "Iter, " << iter << ", " << i << ", deltaP, " << deltaP[i] << std::endl;
  		}
  
  		std::cout << "Iter" << iter << " Updated for Va, maxDeltaP, " << maxDeltaP << ", maxDeltaQ, " << maxDeltaQ << std::endl;
   	  
      gettimeofday(&t3_end, 0);
  	  seconds=t3_end.tv_sec  - t3_st.tv_sec; 
  	  useconds = t3_end.tv_usec  - t3_st.tv_usec;
  	  printf("\n\n==============================================================================================\n ");
  	  std::cout << "Time for updating deltaP and deltaQ (iteration):: " << iter << " :" << (seconds*1000000 + useconds) << " us" << std::endl;
  	  printf("\n================================================================================================\n ");
      
      // Decide if the program converges	
		if ( maxDeltaP < max_change_ex && maxDeltaQ < max_change_ex){
		result = "OK";
			break;
		}
      
      //reset the max mismatche
      maxDeltaP = 0;
      maxDeltaQ = 0;
      
  		// solve for deltaVa
  		// std::cout << " ======================== Solve for V angle ========================"<<std::endl;
  		GraphLU_Solve(graphlu, deltaP);
  
  		// Update V angle (Va)
  		for (i=0; i<n; ++i)
		{
			Va[i] -= deltaP[i];
			std::cout << "Iter, " << iter << ", " << i << ", Va, " << Va[i]<< ", deltaVa, " << deltaP[i]<< std::endl;
		}          
  
  		// std::cout << " ======================== Update deltaP and deltaQ ========================"<<std::endl;
      gettimeofday(&t3_st, 0);
  		// Calculate deltaP and deltaQ to update Vm
  		for (i=0; i<n; ++i){
  			deltaP[i] = 0;
  			deltaQ[i] = 0;
  
  			if (btype[i]!=3){
  				deltaP[i] = Pn[i];
  				if (btype[i]<2)
  					deltaQ[i] = Qn[i];
  			}
  
  			// Calculate network injections
  			if (btype[i]!=3){
  				for (j=ep[i]; j<ep[i+1]; ++j){
  					p=ei[j];
            // Va has been updated, update the cosine and sine terms 
            cosine_array[j] = cos(Va[i]-Va[p]);
            sine_array[j] = sin(Va[i]-Va[p]);        
            deltaP[i] += - Vm[i]*Vm[p]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);        
  					if (btype[i]<2) // calculate Q for PQ buses
              deltaQ[i] += - Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]); 
  				}
  			}
  
  			// Get max P and max Q
  			if (abs(deltaP[i]) > maxDeltaP)
  				maxDeltaP = abs(deltaP[i]);
  			if (abs(deltaQ[i]) > maxDeltaQ)
  				maxDeltaQ = abs(deltaQ[i]);
  
  			deltaP[i] /= Vm[i];
  			deltaQ[i] /= Vm[i];
  
  			// check output
  			std::cout << "Iter, " << iter << ", " << i << ", deltaQ, " << deltaQ[i] << std::endl;
      }
  
  		std::cout << "Iter" << iter << " Updated for Vm, maxDeltaP, " << maxDeltaP << ", maxDeltaQ, " << maxDeltaQ << std::endl;
  
      gettimeofday(&t3_end, 0);
  	  seconds=t3_end.tv_sec  - t3_st.tv_sec; 
  	  useconds = t3_end.tv_usec  - t3_st.tv_usec;
  	  printf("\n\n==============================================================================================\n ");
  	  std::cout << "Time for updating deltaP and deltaQ (iteration):: " << iter << " :" << (seconds*1000000 + useconds) << " us" << std::endl;
  	  printf("\n================================================================================================\n ");
       
  		// Decide if the program converges	
  		if ( maxDeltaP < max_change_ex && maxDeltaQ < max_change_ex){
        result = "OK";
  			break;
  		}
     
  		// solve for deltaVm
  		// std::cout << " ======================== Solve for V magnitude ========================"<<std::endl;
  		GraphLU_Solve(graphlu_pp, deltaQ);
  
  		// Update V magnitude (Vm)
  		for (i=0; i<n; ++i)
  		{
      	Vm[i] -= deltaQ[i];
        std::cout << "Iter, " << iter << ", " << i << ", Vm, " << Vm[i]<< ", deltaVm, " << deltaQ[i]<< std::endl;
		
      }        
  	}
   
  	gettimeofday(&t2_end, 0);
  	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
  	useconds = t2_end.tv_usec  - t2_st.tv_usec;
  	
  	printf("\n\n============================================================================================== ");
  	std::cout << "Time for performing power flow iteration: " << (seconds*1000000 + useconds) << " us" << std::endl;
  	printf("\n================================================================================================ ");
  
  	// ----------------------------------------------------------------------------------------------------
  	// 								Store the Result back to the array
  	// ----------------------------------------------------------------------------------------------------
  	// Get solved Vm and Va back into HeapAccum
  	for (ii=0; ii<n; ++ii) {
        gVertex.data_[ii].Va = Va[ii]/pi_value*180;
        gVertex.data_[ii].Vm = Vm[ii];
        //std::cout << "Final, " << ii+1 << ", Vm, " << Vm[ii]<< ", Va" << ", " << Va[ii]/pi_value*180<< std::endl;
  	}
  } else {
    //factorization failed
 	  result = "FAILED";
    printf("\n\n============================================================================================== ");
  	std::cout << "Factorization FAILED" << std::endl;
  	printf("\n================================================================================================ ");  
  }
EXIT:
	// std::cout << "\n free1 " << std::endl;
	GraphLU_Destroy(graphlu);
	GraphLU_Destroy(graphlu_pp);
	// std::cout << "\n free2 " << std::endl;
	free(ax);
	free(ai);
	free(ap);
	free(ax_pp);
	free(ai_pp);
	free(ap_pp);
	free(graphlu);
	free(graphlu_pp);
	// std::cout << "\n free3 " << std::endl;
		
	free(eG);
	free(eB);
	free(ei);
	free(ep);

	free(deltaP);
	free(deltaQ);
	free(Vm);
	free(Va);		
	free(Pn);
	free(Qn);	
	
  printf("\n\n----------------------------------------------------------------------------------------");
	printf("\t\t End of Running GRAPHLU_fdpf_v7 C function!");
	printf("\t\t ----------------------------------------------------------------------------------------\n\n");	
 
  return result;
}




// **********************************************************************************************************************************
// Created by: Ting Chan, ting.chan@geirina.net
// Date: 02/22/2017
// This code performs fast-decoupled power flow analysis. It is used as a benchmark to assess the time it takes to run a fast-decoupled
// power flow in C++ with GRAPHLU. This version does NOT perform LU factorization. B' and B" are assumed to be previously factorized and
// the resulting L and U matrices are taken as inputs. Power flow iterations are solved with forward/backward substitution.
// In this function, the HeapAccum for Ybus matrix  and factorized LU matrices are assumed to be built and sorted in the past. 
// Sorting of HeapAccums is only done for the one that contains state variables.
// History: 
// 11/15/2016: [Ting Chan] added another SetAccum for Bpp fill-in edges
// 01/09/2017: [Ting Chan] added a control parameter (fillin) to specify whether we want to find the fill-ins.
//             1 - find fill-ins
//             0 - otherwise
// 01/17/2017: [Ting Chan] Replaced GraphLU_Solve with our own implementation of forward/backward substitution. 
// 01/18/2017: [Ting Chan] Added two new HeapAccums to store factorized L values (diagonal), scaling factors, permutation and permutation
//             inverse.
// 01/20/2017: [Ting Chan] B' and B" have previously been factorized. HeapAccums for the factorized values, index and pointers are 
//             taken as inputs. Peform sorting of the HeapAccum first, then convert them into arrays for power flow iteration and
//             forward/backward substitution.
// 01/23/2017: [Ting Chan] Consolidated HeapAccums to reduce sorting time.
// 01/30/2017: [Ting Chan] Fixed forward/backward substitution to use cpi instead of rpi
// 01/31/2017: [Ting Chan] Added sizes of L', U' L", and U" HeapAccums as inputs
// 02/01/2017: [Ting Chan] Created to be used with schema version flowSchema_LU7_graphsql44.ddl 
// 02/03/2017: [Ting Chan] Added cols and cols_pp for column scaling factor
// 02/06/2017: [Ting Chan] Use Lflag and Uflag to decide whether an non-zero element should be added to lx or ux
//             (I think this step might not be necessary and can probably be implemented in a better way by directly using
//              Lvalue and U value, but do this for now)
// 02/08/2017: [Ting Chan] Use Lvalue and Uvalue to decide whether an non-zero element should be added to lx or ux instead of
//             Lflag and Uflag. (see previous comment)
// 02/08/2017: [Ting Chan] With the previous change, we can no longer use Lpointer and Upointer stored on the vertex directly.
//             This is discovered during testing of the sc_20160818_1120 case as there are zero elements in the output LU
//             structure from GraphLU_DumpLU. This should be investigated. At the meantime, update the logic to get the pointer
//             information.
// 02/17/2017: [Ting Chan] All HeapAccums except the Ybus vertex which contains state variables are static and have been built and sorted
//             so there is no need to perform sorting.
// 02/22/2017: [Ting Chan] L' and U' are represented by two seperate HeapAccums. Updated array creation logic.
// 02/23/2017: [Ting Chan] Removed the LU sizing input parameters. The LU number of non-zeros can be obtained from the HeapAccums directly.  
// **********************************************************************************************************************************
template <typename T_vertex_Ybus, typename vertex_Ybus_comp, 
          typename T_matrix_Ybus, typename matrix_Ybus_comp, 
          typename T_p_L_matrix, typename p_L_matrix_comp,
          typename T_p_U_matrix, typename p_U_matrix_comp,  
          typename T_pp_L_matrix, typename pp_L_matrix_comp,
          typename T_pp_U_matrix, typename pp_U_matrix_comp,
          typename T_vertex_p, typename vertex_p_comp,
          typename T_vertex_pp, typename vertex_pp_comp>
inline string GRAPHLU_fdpf_LU_solve_4_static_mod (HeapAccum<T_vertex_Ybus, vertex_Ybus_comp>& gVertex_Ybus, HeapAccum<T_matrix_Ybus, matrix_Ybus_comp>& gMatrix_Ybus, 
              HeapAccum<T_p_L_matrix, p_L_matrix_comp>& gMatrix_p_L, HeapAccum<T_p_U_matrix, p_U_matrix_comp>& gMatrix_p_U, HeapAccum<T_vertex_p, vertex_p_comp>& gVertex_p,
              HeapAccum<T_pp_L_matrix, pp_L_matrix_comp>& gMatrix_pp_L, HeapAccum<T_pp_U_matrix, pp_U_matrix_comp>& gMatrix_pp_U, HeapAccum<T_vertex_pp, vertex_pp_comp>& gVertex_pp,
              MaxAccum<int> maxiter, MaxAccum<double> maxchange ) {


	// extern size_t g_si, g_sd, g_sp;
	printf("\n\n------------------------------------------------------------------------------------------- ");
	printf("\nStart Running GRAPHLU_fdpf_LU_solve_4_static_mod function!\n");
  std::cout << "Lp Number of rows:" << gVertex_p.data_.size() << ",\tNumber of nonzeros:" << gMatrix_p_L.size() << std::endl;
  std::cout << "Up Number of rows:" << gVertex_p.data_.size() << ",\tNumber of nonzeros:" << gMatrix_p_U.size() << std::endl;
  std::cout << "Lpp Number of rows:" << gVertex_pp.data_.size() << ",\tNumber of nonzeros:" << gMatrix_pp_L.size() << std::endl;
  std::cout << "Upp Number of rows:" << gVertex_pp.data_.size() << ",\tNumber of nonzeros:" << gMatrix_pp_U.size() << std::endl;
  std::cout << "Y bus Number of rows:" << gVertex_Ybus.data_.size() << ",\tNumber of nonzeros:" << gMatrix_Ybus.data_.size() << std::endl;
	printf("-------------------------------------------------------------------------------------------- \n\n");	
	// ------------------------------------------------------------------------------------------------
	// 				Initialize variables and arrays
	// ------------------------------------------------------------------------------------------------
	
  string result = "FAILED";
	// Initialize arrays and variables
	uint__t n, nnz_p_L, nnz_p_U, nnz_pp_L, nnz_pp_U, n_e, nnz_e;
	int ret, i, j, p, iter;
	double maxDeltaP=0, maxDeltaQ=0, max_change_ex=maxchange;
  
	const double pi_value = 3.141592653589793;

	// Get the dimension and the nnz for the LU of B' and B"
	n=gVertex_Ybus.data_.size();	
  nnz_p_L=gMatrix_p_L.size(); nnz_p_U=gMatrix_p_U.size();
  nnz_pp_L=gMatrix_pp_L.size(); nnz_pp_U=gMatrix_pp_U.size();
  n_e=gVertex_Ybus.data_.size(); nnz_e=gMatrix_Ybus.data_.size(); // get the size of the Y bus matrix
  
	
	//// For Y-bus
	double *eG, *eB, *deltaP, *deltaQ, *Vm, *Va, *Pn, *Qn; 
	uint *ei, *piv, *btype; 
	uint *ep; 
  
  // For L' and U'
  double *lx, *ux, *rows, *cols; 
  uint *li, *ui, *rp, *cpi; 
  size_t *lp, *up; 
 	//uint__t fctnnz; // number of nonzeros after the matrix is factorized
	//size_t l_nnz = gLp_edge.data_.size(), u_nnz = gUp_edge.data_.size(); // number of nonzeros in L and U factors 

	lx = (double *)malloc(sizeof(double)*(nnz_p_L));
	li = (uint *)malloc(sizeof(uint)*(nnz_p_L)); // column indices 
	lp = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer
	ux = (double *)malloc(sizeof(double)*(nnz_p_U));
	ui = (uint *)malloc(sizeof(uint)*(nnz_p_U)); // column indices 
	up = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer 
  rows = (double *)malloc(sizeof(double)*(n));
  cols = (double *)malloc(sizeof(double)*(n));
  rp = (uint *)malloc(sizeof(uint)*(n));
  cpi = (uint *)malloc(sizeof(uint)*(n));
  
  // for L" and U"
  double *lx_pp, *ux_pp, *rows_pp, *cols_pp; 
  uint *li_pp, *ui_pp, *rp_pp, *cpi_pp; 
  size_t *lp_pp, *up_pp; 
 	//uint__t fctnnz_pp; // number of nonzeros after the matrix is factorized
	//size_t l_nnz_pp, u_nnz_pp; // number of nonzeros in L and U factors
 
	lx_pp = (double *)malloc(sizeof(double)*(nnz_pp_L));
	li_pp = (uint *)malloc(sizeof(uint)*(nnz_pp_L)); // column indices 
	lp_pp = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer
	ux_pp = (double *)malloc(sizeof(double)*(nnz_pp_U));
	ui_pp = (uint *)malloc(sizeof(uint)*(nnz_pp_U)); // column indices 
	up_pp = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer
  rows_pp = (double *)malloc(sizeof(double)*(n));
  cols_pp = (double *)malloc(sizeof(double)*(n));
  rp_pp = (uint *)malloc(sizeof(uint)*(n));
  cpi_pp = (uint *)malloc(sizeof(uint)*(n));    
 
  // arrays to store the sine and cosine terms of the angle difference
  // values are computed during Jacobian initialization and each iteration 
  double *cosine_array, *sine_array;
  cosine_array = (double *)malloc(sizeof(double)*(nnz_e));
  sine_array = (double *)malloc(sizeof(double)*(nnz_e)); 

	eG = (double *)malloc(sizeof(double)*(nnz_e)); // G values in Y 
	eB = (double *)malloc(sizeof(double)*(nnz_e)); // B values in Y
	ei = (uint *)malloc(sizeof(uint)*(nnz_e)); // column indices of Y
	ep = (uint *)malloc(sizeof(uint)*(n_e+1)); // initial row pointers	

	deltaP = (double *)malloc(sizeof(double)*(n)); // b in the Ax=b
	deltaQ = (double *)malloc(sizeof(double)*(n)); // b in the Ax=b

	Vm = (double *)malloc(sizeof(double)*(n)); 
	Va = (double *)malloc(sizeof(double)*(n)); 

	Pn = (double *)malloc(sizeof(double)*(n)); 
	Qn = (double *)malloc(sizeof(double)*(n));

	btype = (uint *)malloc(sizeof(uint)*(n));
 
  double sum, diag;
  double *delta, *b;
  delta = (double *)malloc(sizeof(double)*(n));
  b = (double *)malloc(sizeof(double)*(n));
  
	std::cout << " ======================== Initialization of ararys used to store the factorized matrix and LU ========================"<< std::endl;

	struct timeval t2_st, t2_end, t3_st, t3_end; long seconds, useconds;

  //initialization to 0
  // [tc] use memset here
  memset(Vm, 0, sizeof(double)*(n));
  memset(Va, 0, sizeof(double)*(n));
  memset(deltaP, 0, sizeof(double)*(n));
  memset(deltaQ, 0, sizeof(double)*(n));
	
  // =================================== Sort all input HeapAccum =================================
  gettimeofday(&t3_st, 0);

  // Input HeapAccums are assumed to be unsorted. Sort them before assigning values to
  // local arrays
  gettimeofday(&t2_st, 0);
  gVertex_Ybus.sort_heap();
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gVertex_Ybus HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");    
  
  /*gettimeofday(&t2_st, 0); 
  gMatrix_Ybus.sort_heap();
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gMatrix_Ybus HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");  

  gettimeofday(&t2_st, 0);  
  gVertex_p.sort_heap(); 
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gVertex_p HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 

  gettimeofday(&t2_st, 0);  
  gMatrix_p.sort_heap();
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gMatrix_p HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
 
  gettimeofday(&t2_st, 0);  
  gVertex_pp.sort_heap(); 
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gVertex_pp HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");  
  
  gettimeofday(&t2_st, 0);    
  gMatrix_pp.sort_heap();
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gMatrix_pp HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");*/ 
     
  // get the time for sorting all HeapAccums 
	gettimeofday(&t3_end, 0);
	seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	useconds = t3_end.tv_usec  - t3_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort all HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
	
	// ========================================= Convert pointers and the Vertex =========================================
	gettimeofday(&t2_st, 0); 
  int i_p, i_pp;
	int ii = 1; // counter for the array  
  // Assign value to the first position of the pointer array 
  lp[0] = 0;
  up[0] = 0;
  lp_pp[0] = 0;
  up_pp[0] = 0;
  ep[0] = 0;
	for (i=0; i<gVertex_Ybus.size(); ++i){
    //process pointers for the matrices
    lp[ii] = gVertex_p.data_[i].Lp + lp[ii-1];
    up[ii] = gVertex_p.data_[i].Up + up[ii-1];
    lp_pp[ii] = gVertex_pp.data_[i].Lp + lp_pp[ii-1];
    up_pp[ii] = gVertex_pp.data_[i].Up + up_pp[ii-1];    
    ep[ii] = gVertex_Ybus.data_[i].ep + ep[ii-1];
    ii++;
    // [tc] removed 02/08/2017. Cannot directly use pointers stored on vertex
    // see modifications history for details
    //lp[i] = gVertex_p.data_[i].Lp;
    //up[i] = gVertex_p.data_[i].Up;
    //lp_pp[i] = gVertex_pp.data_[i].Lp;
    //up_pp[i] = gVertex_pp.data_[i].Up;
    //process state variables
    Vm[i] = gVertex_Ybus.data_[i].Vm;
    Va[i] = gVertex_Ybus.data_[i].Va;
    Pn[i] = gVertex_Ybus.data_[i].Pn;
    Qn[i] = gVertex_Ybus.data_[i].Qn;
    btype[i] = gVertex_Ybus.data_[i].bustype;;
    //process factorized information for B'
    //i_p = gVertex_p.data_[i].exId - 1;
    rows[i]= gVertex_p.data_[i].row_scaling;
    cols[i]= gVertex_p.data_[i].col_scaling;
    rp[i] = gVertex_p.data_[i].rp;
    cpi[i] = gVertex_p.data_[i].cpi;  
    //process factorized information for B"  
    //i_pp = gVertex_pp.data_[i].exId - 1;
    rows_pp[i]= gVertex_pp.data_[i].row_scaling;
    cols_pp[i]= gVertex_pp.data_[i].col_scaling;
    rp_pp[i] = gVertex_pp.data_[i].rp;
    cpi_pp[i] = gVertex_pp.data_[i].cpi;    
	}
  //debug print
  //for (i=0; i<n; ++i){
 	// 	std::cout<< i << ",rp," << rp[i]  << ",cpi," << cpi[i] << ",rows,"<< rows[i] << ",cols," << cols[i] <<std::endl;
  //}
  //for (i=0; i<n; ++i){
 	// 	std::cout<< i << ",rp_pp," << rp_pp[i]  << ",cpi_pp," << cpi_pp[i] << ",rows_pp,"<< rows_pp[i] << ",cols_pp," << cols_pp[i] <<std::endl;
  //}   
  // [tc] removed 02/08/2017. Cannot directly use pointers stored on vertex
  // see modifications history for details       
  // Assign total number of non-zeros to the last slot of the pointer arrays
  //lp[n] = gLp;
  //up[n] = gUp;
  //lp_pp[n] = gLpp;
  //up_pp[n] = gUpp;  
 
 	// ========================================= Convert Ybus (ei, eG and eB) =========================================
  for (ii=0; ii<gMatrix_Ybus.size(); ++ii) {
    ei[ii] = gMatrix_Ybus.data_[ii].ei;
    eG[ii] = gMatrix_Ybus.data_[ii].eG;
    eB[ii] = gMatrix_Ybus.data_[ii].eB; 
  } 
 	// ========================================= Convert L' and U' =========================================
  int i_l = 0, i_u = 0;

  for (ii=0; ii<gMatrix_p_L.data_.size(); ++ii) {
    li[ii] = gMatrix_p_L.data_[ii].cpi;
    lx[ii] = gMatrix_p_L.data_[ii].value;      
  } 
  
  for (ii=0; ii<gMatrix_p_U.data_.size(); ++ii) {  
    ui[ii] = gMatrix_p_U.data_[ii].cpi;
    ux[ii] = gMatrix_p_U.data_[ii].value;   
  } 
  
  //debug print
  /*for (i=0; i<gMatrix_p_L.data_.size(); ++i)
  {
     std::cout<< i << ",lx,"<< lx[i] << ",li,"<< li[i] << std::endl;  
  } 
  for (i=0; i<n+1; ++i){
     std::cout<< i << ",lp,"<< lp[i] << std::endl;
  } 
  for (i=0; i<gMatrix_p_U.data_.size(); ++i)
  {
     std::cout<< i << ",ux,"<< ux[i] << ",ui,"<< ui[i] << std::endl;  
  } 
  for (i=0; i<n+1; ++i){
     std::cout<< i << ",up,"<< up[i] << std::endl;
  }*/
 	// ========================================= Convert L" and U" =========================================
  i_l = 0; 
  i_u = 0;
  
  for (ii=0; ii<gMatrix_pp_L.data_.size(); ++ii) {
    li_pp[ii] = gMatrix_pp_L.data_[ii].cpi;
    lx_pp[ii] = gMatrix_pp_L.data_[ii].value;
  }
  
  for (ii=0; ii<gMatrix_pp_U.data_.size(); ++ii) {  
    ui_pp[ii] = gMatrix_pp_U.data_[ii].cpi;
    ux_pp[ii] = gMatrix_pp_U.data_[ii].value;
  }
  
  //debug print   
  /*for (i=0; i<gMatrix_pp_L.data_.size(); ++i)
  {
     std::cout<< i << ",lx_pp,"<< lx_pp[i] << ",li_pp,"<< li_pp[i] << std::endl;  
  } 
  for (i=0; i<n+1; ++i){
     std::cout<< i << ",lp_pp,"<< lp_pp[i] << std::endl;
  } 
    for (i=0; i<gMatrix_pp_U.data_.size(); ++i)
  {
     std::cout<< i << ",ux_pp,"<< ux_pp[i] << ",ui_pp,"<< ui_pp[i] << std::endl;  
  } 
  for (i=0; i<n+1; ++i){
     std::cout<< i << ",up_pp,"<< up_pp[i] << std::endl;
  }*/      	    	
  // Done converting all input HeapAccums to arrays
  
	// Get the time to convert the data between containers and arrays
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;

	printf("\n\n============================================================================================== ");
	std::cout << "Time to convert data to GRAPHLU time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");		

  //continue to do the iterations only if there is no error from factorization
 	printf("\n\n============================================================================================== ");
 	std::cout << "\n Start iteratively updating deltaP and deltaQ " << std::endl;
 	printf("\n================================================================================================\n ");
 
 	gettimeofday(&t2_st, 0);
  
 	for (iter=0; iter<maxiter; ++iter){
 		// update deltaP and deltaQ
 		maxDeltaP=0;
 		maxDeltaQ=0;
 
 		// std::cout << " ======================== Update deltaP and deltaQ ========================"<<std::endl;
     gettimeofday(&t3_st, 0);
 		// Calculate deltaP and deltaQ to update Va
 		for (i=0; i<n; ++i){
 			deltaP[i] = 0;
 			deltaQ[i] = 0;
 
 			if (btype[i]!=3){
 				deltaP[i] = Pn[i];
 				if (btype[i]<2)
 					deltaQ[i] = Qn[i];
 			}
 
 			// Calculate network injections
 			if (btype[i]!=3){
 				for (j=ep[i]; j<ep[i+1]; ++j){
 					p=ei[j];
           if (iter == 0){        
             //if this is the first iteraton, calculate the cosine and sine terms        
             cosine_array[j] = cos(Va[i]-Va[p]);
             sine_array[j] = sin(Va[i]-Va[p]);
           }    
           deltaP[i] += - Vm[i]*Vm[p]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);        
 					if (btype[i]<2) // calculate Q for PQ buses
             deltaQ[i] += - Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]); 
 				}
 			}  
       
 			// Get max P and max Q
 			if (abs(deltaP[i]) > maxDeltaP)
 				maxDeltaP = abs(deltaP[i]);
 			if (abs(deltaQ[i]) > maxDeltaQ)
 				maxDeltaQ = abs(deltaQ[i]);
 
 			deltaP[i] /= Vm[i];
 			deltaQ[i] /= Vm[i];
 
 			// check output
			  //std::cout << "Iter" << iter << ", deltaP" << i << ", " << deltaP[i]<< ", deltaQ" << i << ", " << deltaQ[i]<< std::endl;
 		}
 
 		std::cout << "Iter" << iter << " Updated for Va, maxDeltaP, " << maxDeltaP << ", maxDeltaQ, " << maxDeltaQ << std::endl;
  	  
    gettimeofday(&t3_end, 0);
 	  seconds=t3_end.tv_sec  - t3_st.tv_sec; 
 	  useconds = t3_end.tv_usec  - t3_st.tv_usec;
 	  printf("\n\n==============================================================================================\n ");
 	  std::cout << "Time for updating deltaP and deltaQ (iteration):: " << iter << " :" << (seconds*1000000 + useconds) << " us" << std::endl;
 	  printf("\n================================================================================================\n ");
     
     // Decide if the program converges	
 		if ( maxDeltaP < max_change_ex && maxDeltaQ < max_change_ex){
       result = "OK";
 			break;
 		}
     
    //reset the max mismatche
    maxDeltaP = 0;
    maxDeltaQ = 0;
     
 		// solve for deltaVa
 		// std::cout << " ======================== Solve for V angle ========================"<<std::endl;
     
    // A*x = b
    // multiply deltaP with row scaling (rows) to get b
   	for (i=0; i<n; ++i){
      b[i]=deltaP[rp[i]]*rows[rp[i]];
   	}
    
   	// Full forward substitution 
   	for (i=0; i<n; ++i)
   	{
   		sum=0.;
   		diag=0.;
   		for(p=lp[i]; p< lp[i+1]; ++p){
   			if (i!=li[p]){ // if it is not diagnal element
   				j=li[p];
   				sum += lx[p] * b[j];
   			}
   			else{
   				diag = lx[p];
   			}
   		}
   		b[i] -= sum; 
   		b[i] /= diag; 
   	} 
    
   	// Full backward substitution 
   	for (i=n-1; i>=0; --i)
   	{
   		sum=0.;
   		for(p=up[i]; p< up[i+1]; ++p){
   			if (i!=ui[p]){ // if it is not diagnal element
   				sum += ux[p] * b[ui[p]]; 
   			}
   		}
   
   		b[i] -= sum; 
   	}      
   	// permute the array back to the original order
   	//for (i=0; i<n; ++i)
     //{
   	//	delta[i]=b[cpi[i]];
     //  //std::cout << "delta: " << i << ", " << delta[i] << std::endl;           
 		//}
       
    // Update V angle (Va)
 		for (i=0; i<n; ++i)
    { 
      //Va[i] -= delta[i];  
      Va[i] -= b[cpi[i]]*cols[cpi[i]];     
    }
    
 		// std::cout << " ======================== Update deltaP and deltaQ ========================"<<std::endl;
     gettimeofday(&t3_st, 0);
 		// Calculate deltaP and deltaQ to update Vm
 		for (i=0; i<n; ++i){
 			deltaP[i] = 0;
 			deltaQ[i] = 0;
 
 			if (btype[i]!=3){
 				deltaP[i] = Pn[i];
 				if (btype[i]<2)
 					deltaQ[i] = Qn[i];
 			}
 
 			// Calculate network injections
 			if (btype[i]!=3){
 				for (j=ep[i]; j<ep[i+1]; ++j){
 					p=ei[j];
           // Va has been updated, update the cosine and sine terms 
           cosine_array[j] = cos(Va[i]-Va[p]);
           sine_array[j] = sin(Va[i]-Va[p]);        
           deltaP[i] += - Vm[i]*Vm[p]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);        
 					if (btype[i]<2) // calculate Q for PQ buses
             deltaQ[i] += - Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]); 
 				}
 			}
 
 			// Get max P and max Q
 			if (abs(deltaP[i]) > maxDeltaP)
 				maxDeltaP = abs(deltaP[i]);
 			if (abs(deltaQ[i]) > maxDeltaQ)
 				maxDeltaQ = abs(deltaQ[i]);
 
 			deltaP[i] /= Vm[i];
 			deltaQ[i] /= Vm[i];
 
 			// check output
 			// std::cout << "Iter" << iter << ", deltaP" << i << ", " << deltaP[i]<< ", deltaQ" << i << ", " << deltaQ[i]<< std::endl;
     }
 
 		std::cout << "Iter" << iter << " Updated for Vm, maxDeltaP, " << maxDeltaP << ", maxDeltaQ, " << maxDeltaQ << std::endl;
 
     gettimeofday(&t3_end, 0);
 	  seconds=t3_end.tv_sec  - t3_st.tv_sec; 
 	  useconds = t3_end.tv_usec  - t3_st.tv_usec;
 	  printf("\n\n==============================================================================================\n ");
 	  std::cout << "Time for updating deltaP and deltaQ (iteration):: " << iter << " :" << (seconds*1000000 + useconds) << " us" << std::endl;
 	  printf("\n================================================================================================\n ");
      
 		// Decide if the program converges	
 		if ( maxDeltaP < max_change_ex && maxDeltaQ < max_change_ex){
       result = "OK";
 			break;
 		}

 		// solve for deltaVm
 		// std::cout << " ======================== Solve for V magnitude ========================"<<std::endl;
    
     // A*x = b
     // multiply deltaQ with row scaling (rows_pp) to get b
   	for (i=0; i<n; ++i){
      b[i]=deltaQ[rp_pp[i]]*rows_pp[rp_pp[i]];
   	}
   	
     // Full forward substitution 
   	for (i=0; i<n; ++i)
   	{
   		sum=0.;
   		diag=0.;
   		for(p=lp_pp[i]; p< lp_pp[i+1]; ++p){
   			if (i!=li_pp[p]){ // if it is not diagnal element
   				j=li_pp[p];
   				sum += lx_pp[p] * b[j]; 
   			}
   			else{
   				diag = lx_pp[p];
   			}
   		}
   
   		b[i] -= sum; 
   		b[i] /= diag;
   	} 
   	
     // Full backward substitution 
   	for (i=n-1; i>=0; --i)
   	{
   		sum=0.;
   		for(p=up_pp[i]; p< up_pp[i+1]; ++p){
   			if (i!=ui_pp[p]){ // if it is not diagnal element
   				sum += ux_pp[p] * b[ui_pp[p]]; 
   			}
   		}
   
   		b[i] -= sum; 
   	}      
   	
     // permute the array back to the original order
   	//for (i=0; i<n; ++i)
     //{
   	//	delta[i]=b[cpi_pp[i]];
     //  //std::cout << "delta: " << i << ", " << delta[i] << std::endl;           
 		//}
 		
     // Update V magnitude (Vm)
 		for (i=0; i<n; ++i)
     {
       //Vm[i] -= delta[i];
       Vm[i] -= b[cpi_pp[i]]*cols_pp[cpi_pp[i]];
     }       
 	}
  
 	gettimeofday(&t2_end, 0);
 	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
 	useconds = t2_end.tv_usec  - t2_st.tv_usec;
 	
 	printf("\n\n============================================================================================== ");
 	std::cout << "Time for performing power flow iteration: " << (seconds*1000000 + useconds) << " us" << std::endl;
 	printf("\n================================================================================================ ");
 
 	// ----------------------------------------------------------------------------------------------------
 	// 								Store the Result back to the array
 	// ----------------------------------------------------------------------------------------------------
 	// Get solved Vm and Va back into HeapAccum
 	for (ii=0; ii<n; ++ii) {
       gVertex_Ybus.data_[ii].Va = Va[ii];  // in radian
       gVertex_Ybus.data_[ii].Vm = Vm[ii];
       //std::cout << "Final, " << ii+1 << ", Vm, " << Vm[ii]<< ", Va" << ii+1 << ", " << Va[ii]/pi_value*180<< std::endl;
 	}

  
EXIT:

	// std::cout << "\n free3 " << std::endl;
		
	free(eG);
	free(eB);
	free(ei);
	free(ep);

	free(deltaP);
	free(deltaQ);
	free(Vm);
	free(Va);		
	free(Pn);
	free(Qn);	
  
 	free(lx);
	free(li);
	free(lp);
  free(ux);
	free(ui);
	free(up); 
  free(rows);
  free(cols);
  free(rp);
  free(cpi); 
 	free(lx_pp);
	free(li_pp);
	free(lp_pp);
 	free(ux_pp);
	free(ui_pp);
	free(up_pp);
  free(rows_pp);
  free(cols_pp);
  free(rp_pp);
  free(cpi_pp);    
	
  printf("\n\n----------------------------------------------------------------------------------------");
	printf("\t\t End of Running GRAPHLU_fdpf_LU_solve_4_static_mod C function!");
	printf("\t\t ----------------------------------------------------------------------------------------\n\n");	
 
  return result;
}




// // **********************************************************************************************************************************
// // Created by: Chen Yuan, chen.yuan@geirina.net
// // Date: 06/08/2018
// // This code performs fast-decoupled power flow analysis. It is used as a benchmark to assess the time it takes to run a fast-decoupled
// // power flow in C++ with GRAPHLU. This version does NOT perform LU factorization. B' and B" are assumed to be previously factorized and
// // the resulting L and U matrices are taken as inputs. Power flow iterations are solved with forward/backward substitution.
// // In this function, the Ybus matrix  and factorized LU matrices are assumed to be built and sorted in the past. 
// // Sorting of HeapAccums is only done for the one that contains state variables.
// // History: 
// // 06/08/2018 [Chen Yuan]: Get B' and B'' matrix from static class
// //              !!!!!!!!!!!!!!!!!!! even Ybus in the future. !!!!!!!!!!!!!!!!!!
// // **********************************************************************************************************************************
// template <typename T_vertex_Ybus, typename vertex_Ybus_comp, 
          // typename T_matrix_Ybus, typename matrix_Ybus_comp>
// inline string GRAPHLU_fdpf_LU_solve_EMS (HeapAccum<T_vertex_Ybus, vertex_Ybus_comp>& gVertex_Ybus, HeapAccum<T_matrix_Ybus, matrix_Ybus_comp>& gMatrix_Ybus, MaxAccum<int> maxiter, MaxAccum<double> maxchange ) {


	// // extern size_t g_si, g_sd, g_sp;
	// printf("\n\n------------------------------------------------------------------------------------------- ");
	// printf("\nStart Running GRAPHLU_fdpf_LU_solve_EMS function!\n");
	// std::cout << "Lp Number of rows:" << gVertex_p.data_.size() << ",\tNumber of nonzeros:" << gMatrix_p_L.size() << std::endl;
	// std::cout << "Up Number of rows:" << gVertex_p.data_.size() << ",\tNumber of nonzeros:" << gMatrix_p_U.size() << std::endl;
	// std::cout << "Lpp Number of rows:" << gVertex_pp.data_.size() << ",\tNumber of nonzeros:" << gMatrix_pp_L.size() << std::endl;
	// std::cout << "Upp Number of rows:" << gVertex_pp.data_.size() << ",\tNumber of nonzeros:" << gMatrix_pp_U.size() << std::endl;
	// std::cout << "Y bus Number of rows:" << gVertex_Ybus.data_.size() << ",\tNumber of nonzeros:" << gMatrix_Ybus.data_.size() << std::endl;
	// printf("-------------------------------------------------------------------------------------------- \n\n");	
	// // ------------------------------------------------------------------------------------------------
	// // 				Initialize variables and arrays
	// // ------------------------------------------------------------------------------------------------
	
	// string result = "FAILED";
	// // Initialize arrays and variables
	// uint__t n, nnz_p_L, nnz_p_U, nnz_pp_L, nnz_pp_U, n_e, nnz_e;
	// int ret, i, j, p, iter;
	// double maxDeltaP=0, maxDeltaQ=0, max_change_ex=maxchange;

	// const double pi_value = 3.141592653589793;

	
	// // Get the dimension and the nnz for the LU of B' and B"
	// n=gVertex_Ybus.data_.size();	
	// nnz_p_L=gMatrix_p_L.size(); nnz_p_U=gMatrix_p_U.size();
	// nnz_pp_L=gMatrix_pp_L.size(); nnz_pp_U=gMatrix_pp_U.size();
	// n_e=gVertex_Ybus.data_.size(); nnz_e=gMatrix_Ybus.data_.size(); // get the size of the Y bus matrix
  
	
	// //// For Y-bus
	// double *eG, *eB, *deltaP, *deltaQ, *Vm, *Va, *Pn, *Qn; 
	// uint *ei, *piv, *btype; 
	// uint *ep; 
  
  // // For L' and U'
  // double *lx, *ux, *rows, *cols; 
  // uint *li, *ui, *rp, *cpi; 
  // size_t *lp, *up; 
 	// //uint__t fctnnz; // number of nonzeros after the matrix is factorized
	// //size_t l_nnz = gLp_edge.data_.size(), u_nnz = gUp_edge.data_.size(); // number of nonzeros in L and U factors 

	// lx = (double *)malloc(sizeof(double)*(nnz_p_L));
	// li = (uint *)malloc(sizeof(uint)*(nnz_p_L)); // column indices 
	// lp = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer
	// ux = (double *)malloc(sizeof(double)*(nnz_p_U));
	// ui = (uint *)malloc(sizeof(uint)*(nnz_p_U)); // column indices 
	// up = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer 
  // rows = (double *)malloc(sizeof(double)*(n));
  // cols = (double *)malloc(sizeof(double)*(n));
  // rp = (uint *)malloc(sizeof(uint)*(n));
  // cpi = (uint *)malloc(sizeof(uint)*(n));
  
  // // for L" and U"
  // double *lx_pp, *ux_pp, *rows_pp, *cols_pp; 
  // uint *li_pp, *ui_pp, *rp_pp, *cpi_pp; 
  // size_t *lp_pp, *up_pp; 
 	// //uint__t fctnnz_pp; // number of nonzeros after the matrix is factorized
	// //size_t l_nnz_pp, u_nnz_pp; // number of nonzeros in L and U factors
 
	// lx_pp = (double *)malloc(sizeof(double)*(nnz_pp_L));
	// li_pp = (uint *)malloc(sizeof(uint)*(nnz_pp_L)); // column indices 
	// lp_pp = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer
	// ux_pp = (double *)malloc(sizeof(double)*(nnz_pp_U));
	// ui_pp = (uint *)malloc(sizeof(uint)*(nnz_pp_U)); // column indices 
	// up_pp = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer
  // rows_pp = (double *)malloc(sizeof(double)*(n));
  // cols_pp = (double *)malloc(sizeof(double)*(n));
  // rp_pp = (uint *)malloc(sizeof(uint)*(n));
  // cpi_pp = (uint *)malloc(sizeof(uint)*(n));    
 
  // // arrays to store the sine and cosine terms of the angle difference
  // // values are computed during Jacobian initialization and each iteration 
  // double *cosine_array, *sine_array;
  // cosine_array = (double *)malloc(sizeof(double)*(nnz_e));
  // sine_array = (double *)malloc(sizeof(double)*(nnz_e)); 

	// eG = (double *)malloc(sizeof(double)*(nnz_e)); // G values in Y 
	// eB = (double *)malloc(sizeof(double)*(nnz_e)); // B values in Y
	// ei = (uint *)malloc(sizeof(uint)*(nnz_e)); // column indices of Y
	// ep = (uint *)malloc(sizeof(uint)*(n_e+1)); // initial row pointers	

	// deltaP = (double *)malloc(sizeof(double)*(n)); // b in the Ax=b
	// deltaQ = (double *)malloc(sizeof(double)*(n)); // b in the Ax=b

	// Vm = (double *)malloc(sizeof(double)*(n)); 
	// Va = (double *)malloc(sizeof(double)*(n)); 

	// Pn = (double *)malloc(sizeof(double)*(n)); 
	// Qn = (double *)malloc(sizeof(double)*(n));

	// btype = (uint *)malloc(sizeof(uint)*(n));
 
  // double sum, diag;
  // double *delta, *b;
  // delta = (double *)malloc(sizeof(double)*(n));
  // b = (double *)malloc(sizeof(double)*(n));
  
	// std::cout << " ======================== Initialization of ararys used to store the factorized matrix and LU ========================"<< std::endl;

	// struct timeval t2_st, t2_end, t3_st, t3_end; long seconds, useconds;

  // //initialization to 0
  // // [tc] use memset here
  // memset(Vm, 0, sizeof(double)*(n));
  // memset(Va, 0, sizeof(double)*(n));
  // memset(deltaP, 0, sizeof(double)*(n));
  // memset(deltaQ, 0, sizeof(double)*(n));
	
  // // =================================== Sort all input HeapAccum =================================
  // gettimeofday(&t3_st, 0);

  // // Input HeapAccums are assumed to be unsorted. Sort them before assigning values to
  // // local arrays
  // gettimeofday(&t2_st, 0);
  // gVertex_Ybus.sort_heap();
	// gettimeofday(&t2_end, 0);
	// seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	// useconds = t2_end.tv_usec  - t2_st.tv_usec;
	// printf("\n\n============================================================================================== ");
	// std::cout << "Time to sort gVertex_Ybus HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	// printf("\n================================================================================================ ");    
  
  // /*gettimeofday(&t2_st, 0); 
  // gMatrix_Ybus.sort_heap();
	// gettimeofday(&t2_end, 0);
	// seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	// useconds = t2_end.tv_usec  - t2_st.tv_usec;
	// printf("\n\n============================================================================================== ");
	// std::cout << "Time to sort gMatrix_Ybus HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	// printf("\n================================================================================================ ");  

  // gettimeofday(&t2_st, 0);  
  // gVertex_p.sort_heap(); 
	// gettimeofday(&t2_end, 0);
	// seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	// useconds = t2_end.tv_usec  - t2_st.tv_usec;
	// printf("\n\n============================================================================================== ");
	// std::cout << "Time to sort gVertex_p HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	// printf("\n================================================================================================ "); 

  // gettimeofday(&t2_st, 0);  
  // gMatrix_p.sort_heap();
	// gettimeofday(&t2_end, 0);
	// seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	// useconds = t2_end.tv_usec  - t2_st.tv_usec;
	// printf("\n\n============================================================================================== ");
	// std::cout << "Time to sort gMatrix_p HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	// printf("\n================================================================================================ "); 
 
  // gettimeofday(&t2_st, 0);  
  // gVertex_pp.sort_heap(); 
	// gettimeofday(&t2_end, 0);
	// seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	// useconds = t2_end.tv_usec  - t2_st.tv_usec;
	// printf("\n\n============================================================================================== ");
	// std::cout << "Time to sort gVertex_pp HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	// printf("\n================================================================================================ ");  
  
  // gettimeofday(&t2_st, 0);    
  // gMatrix_pp.sort_heap();
	// gettimeofday(&t2_end, 0);
	// seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	// useconds = t2_end.tv_usec  - t2_st.tv_usec;
	// printf("\n\n============================================================================================== ");
	// std::cout << "Time to sort gMatrix_pp HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	// printf("\n================================================================================================ ");*/ 
     
  // // get the time for sorting all HeapAccums 
	// gettimeofday(&t3_end, 0);
	// seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	// useconds = t3_end.tv_usec  - t3_st.tv_usec;
	// printf("\n\n============================================================================================== ");
	// std::cout << "Time to sort all HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	// printf("\n================================================================================================ "); 
	
	// // ========================================= Convert pointers and the Vertex =========================================
	// gettimeofday(&t2_st, 0); 
  // int i_p, i_pp;
	// int ii = 1; // counter for the array  
  // // Assign value to the first position of the pointer array 
  // lp[0] = 0;
  // up[0] = 0;
  // lp_pp[0] = 0;
  // up_pp[0] = 0;
  // ep[0] = 0;
	// for (i=0; i<gVertex_Ybus.size(); ++i){
    // //process pointers for the matrices
    // lp[ii] = gVertex_p.data_[i].Lp + lp[ii-1];
    // up[ii] = gVertex_p.data_[i].Up + up[ii-1];
    // lp_pp[ii] = gVertex_pp.data_[i].Lp + lp_pp[ii-1];
    // up_pp[ii] = gVertex_pp.data_[i].Up + up_pp[ii-1];    
    // ep[ii] = gVertex_Ybus.data_[i].ep + ep[ii-1];
    // ii++;
    // // [tc] removed 02/08/2017. Cannot directly use pointers stored on vertex
    // // see modifications history for details
    // //lp[i] = gVertex_p.data_[i].Lp;
    // //up[i] = gVertex_p.data_[i].Up;
    // //lp_pp[i] = gVertex_pp.data_[i].Lp;
    // //up_pp[i] = gVertex_pp.data_[i].Up;
    // //process state variables
    // Vm[i] = gVertex_Ybus.data_[i].Vm;
    // Va[i] = gVertex_Ybus.data_[i].Va;
    // Pn[i] = gVertex_Ybus.data_[i].Pn;
    // Qn[i] = gVertex_Ybus.data_[i].Qn;
    // btype[i] = gVertex_Ybus.data_[i].bustype;;
    // //process factorized information for B'
    // //i_p = gVertex_p.data_[i].exId - 1;
    // rows[i]= gVertex_p.data_[i].row_scaling;
    // cols[i]= gVertex_p.data_[i].col_scaling;
    // rp[i] = gVertex_p.data_[i].rp;
    // cpi[i] = gVertex_p.data_[i].cpi;  
    // //process factorized information for B"  
    // //i_pp = gVertex_pp.data_[i].exId - 1;
    // rows_pp[i]= gVertex_pp.data_[i].row_scaling;
    // cols_pp[i]= gVertex_pp.data_[i].col_scaling;
    // rp_pp[i] = gVertex_pp.data_[i].rp;
    // cpi_pp[i] = gVertex_pp.data_[i].cpi;    
	// }
  // //debug print
  // //for (i=0; i<n; ++i){
 	// // 	std::cout<< i << ",rp," << rp[i]  << ",cpi," << cpi[i] << ",rows,"<< rows[i] << ",cols," << cols[i] <<std::endl;
  // //}
  // //for (i=0; i<n; ++i){
 	// // 	std::cout<< i << ",rp_pp," << rp_pp[i]  << ",cpi_pp," << cpi_pp[i] << ",rows_pp,"<< rows_pp[i] << ",cols_pp," << cols_pp[i] <<std::endl;
  // //}   
  // // [tc] removed 02/08/2017. Cannot directly use pointers stored on vertex
  // // see modifications history for details       
  // // Assign total number of non-zeros to the last slot of the pointer arrays
  // //lp[n] = gLp;
  // //up[n] = gUp;
  // //lp_pp[n] = gLpp;
  // //up_pp[n] = gUpp;  
 
 	// // ========================================= Convert Ybus (ei, eG and eB) =========================================
  // for (ii=0; ii<gMatrix_Ybus.size(); ++ii) {
    // ei[ii] = gMatrix_Ybus.data_[ii].ei;
    // eG[ii] = gMatrix_Ybus.data_[ii].eG;
    // eB[ii] = gMatrix_Ybus.data_[ii].eB; 
  // } 
 	// // ========================================= Convert L' and U' =========================================
  // int i_l = 0, i_u = 0;

  // for (ii=0; ii<gMatrix_p_L.data_.size(); ++ii) {
    // li[ii] = gMatrix_p_L.data_[ii].cpi;
    // lx[ii] = gMatrix_p_L.data_[ii].value;      
  // } 
  
  // for (ii=0; ii<gMatrix_p_U.data_.size(); ++ii) {  
    // ui[ii] = gMatrix_p_U.data_[ii].cpi;
    // ux[ii] = gMatrix_p_U.data_[ii].value;   
  // } 
  
  // //debug print
  // /*for (i=0; i<gMatrix_p_L.data_.size(); ++i)
  // {
     // std::cout<< i << ",lx,"<< lx[i] << ",li,"<< li[i] << std::endl;  
  // } 
  // for (i=0; i<n+1; ++i){
     // std::cout<< i << ",lp,"<< lp[i] << std::endl;
  // } 
  // for (i=0; i<gMatrix_p_U.data_.size(); ++i)
  // {
     // std::cout<< i << ",ux,"<< ux[i] << ",ui,"<< ui[i] << std::endl;  
  // } 
  // for (i=0; i<n+1; ++i){
     // std::cout<< i << ",up,"<< up[i] << std::endl;
  // }*/
 	// // ========================================= Convert L" and U" =========================================
  // i_l = 0; 
  // i_u = 0;
  
  // for (ii=0; ii<gMatrix_pp_L.data_.size(); ++ii) {
    // li_pp[ii] = gMatrix_pp_L.data_[ii].cpi;
    // lx_pp[ii] = gMatrix_pp_L.data_[ii].value;
  // }
  
  // for (ii=0; ii<gMatrix_pp_U.data_.size(); ++ii) {  
    // ui_pp[ii] = gMatrix_pp_U.data_[ii].cpi;
    // ux_pp[ii] = gMatrix_pp_U.data_[ii].value;
  // }
  
  // //debug print   
  // /*for (i=0; i<gMatrix_pp_L.data_.size(); ++i)
  // {
     // std::cout<< i << ",lx_pp,"<< lx_pp[i] << ",li_pp,"<< li_pp[i] << std::endl;  
  // } 
  // for (i=0; i<n+1; ++i){
     // std::cout<< i << ",lp_pp,"<< lp_pp[i] << std::endl;
  // } 
    // for (i=0; i<gMatrix_pp_U.data_.size(); ++i)
  // {
     // std::cout<< i << ",ux_pp,"<< ux_pp[i] << ",ui_pp,"<< ui_pp[i] << std::endl;  
  // } 
  // for (i=0; i<n+1; ++i){
     // std::cout<< i << ",up_pp,"<< up_pp[i] << std::endl;
  // }*/      	    	
  // // Done converting all input HeapAccums to arrays
  
	// // Get the time to convert the data between containers and arrays
	// gettimeofday(&t2_end, 0);
	// seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	// useconds = t2_end.tv_usec  - t2_st.tv_usec;

	// printf("\n\n============================================================================================== ");
	// std::cout << "Time to convert data to GRAPHLU time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	// printf("\n================================================================================================ ");		

  // //continue to do the iterations only if there is no error from factorization
 	// printf("\n\n============================================================================================== ");
 	// std::cout << "\n Start iteratively updating deltaP and deltaQ " << std::endl;
 	// printf("\n================================================================================================\n ");
 
 	// gettimeofday(&t2_st, 0);
  
 	// for (iter=0; iter<maxiter; ++iter){
 		// // update deltaP and deltaQ
 		// maxDeltaP=0;
 		// maxDeltaQ=0;
 
 		// // std::cout << " ======================== Update deltaP and deltaQ ========================"<<std::endl;
     // gettimeofday(&t3_st, 0);
 		// // Calculate deltaP and deltaQ to update Va
 		// for (i=0; i<n; ++i){
 			// deltaP[i] = 0;
 			// deltaQ[i] = 0;
 
 			// if (btype[i]!=3){
 				// deltaP[i] = Pn[i];
 				// if (btype[i]<2)
 					// deltaQ[i] = Qn[i];
 			// }
 
 			// // Calculate network injections
 			// if (btype[i]!=3){
 				// for (j=ep[i]; j<ep[i+1]; ++j){
 					// p=ei[j];
           // if (iter == 0){        
             // //if this is the first iteraton, calculate the cosine and sine terms        
             // cosine_array[j] = cos(Va[i]-Va[p]);
             // sine_array[j] = sin(Va[i]-Va[p]);
           // }    
           // deltaP[i] += - Vm[i]*Vm[p]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);        
 					// if (btype[i]<2) // calculate Q for PQ buses
             // deltaQ[i] += - Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]); 
 				// }
 			// }  
       
 			// // Get max P and max Q
 			// if (abs(deltaP[i]) > maxDeltaP)
 				// maxDeltaP = abs(deltaP[i]);
 			// if (abs(deltaQ[i]) > maxDeltaQ)
 				// maxDeltaQ = abs(deltaQ[i]);
 
 			// deltaP[i] /= Vm[i];
 			// deltaQ[i] /= Vm[i];
 
 			// // check output
			  // //std::cout << "Iter" << iter << ", deltaP" << i << ", " << deltaP[i]<< ", deltaQ" << i << ", " << deltaQ[i]<< std::endl;
 		// }
 
 		// std::cout << "Iter" << iter << " Updated for Va, maxDeltaP, " << maxDeltaP << ", maxDeltaQ, " << maxDeltaQ << std::endl;
  	  
    // gettimeofday(&t3_end, 0);
 	  // seconds=t3_end.tv_sec  - t3_st.tv_sec; 
 	  // useconds = t3_end.tv_usec  - t3_st.tv_usec;
 	  // printf("\n\n==============================================================================================\n ");
 	  // std::cout << "Time for updating deltaP and deltaQ (iteration):: " << iter << " :" << (seconds*1000000 + useconds) << " us" << std::endl;
 	  // printf("\n================================================================================================\n ");
     
     // // Decide if the program converges	
 		// if ( maxDeltaP < max_change_ex && maxDeltaQ < max_change_ex){
       // result = "OK";
 			// break;
 		// }
     
    // //reset the max mismatche
    // maxDeltaP = 0;
    // maxDeltaQ = 0;
     
 		// // solve for deltaVa
 		// // std::cout << " ======================== Solve for V angle ========================"<<std::endl;
     
    // // A*x = b
    // // multiply deltaP with row scaling (rows) to get b
   	// for (i=0; i<n; ++i){
      // b[i]=deltaP[rp[i]]*rows[rp[i]];
   	// }
    
   	// // Full forward substitution 
   	// for (i=0; i<n; ++i)
   	// {
   		// sum=0.;
   		// diag=0.;
   		// for(p=lp[i]; p< lp[i+1]; ++p){
   			// if (i!=li[p]){ // if it is not diagnal element
   				// j=li[p];
   				// sum += lx[p] * b[j];
   			// }
   			// else{
   				// diag = lx[p];
   			// }
   		// }
   		// b[i] -= sum; 
   		// b[i] /= diag; 
   	// } 
    
   	// // Full backward substitution 
   	// for (i=n-1; i>=0; --i)
   	// {
   		// sum=0.;
   		// for(p=up[i]; p< up[i+1]; ++p){
   			// if (i!=ui[p]){ // if it is not diagnal element
   				// sum += ux[p] * b[ui[p]]; 
   			// }
   		// }
   
   		// b[i] -= sum; 
   	// }      
   	// // permute the array back to the original order
   	// //for (i=0; i<n; ++i)
     // //{
   	// //	delta[i]=b[cpi[i]];
     // //  //std::cout << "delta: " << i << ", " << delta[i] << std::endl;           
 		// //}
       
    // // Update V angle (Va)
 		// for (i=0; i<n; ++i)
    // { 
      // //Va[i] -= delta[i];  
      // Va[i] -= b[cpi[i]]*cols[cpi[i]];     
    // }
    
 		// // std::cout << " ======================== Update deltaP and deltaQ ========================"<<std::endl;
     // gettimeofday(&t3_st, 0);
 		// // Calculate deltaP and deltaQ to update Vm
 		// for (i=0; i<n; ++i){
 			// deltaP[i] = 0;
 			// deltaQ[i] = 0;
 
 			// if (btype[i]!=3){
 				// deltaP[i] = Pn[i];
 				// if (btype[i]<2)
 					// deltaQ[i] = Qn[i];
 			// }
 
 			// // Calculate network injections
 			// if (btype[i]!=3){
 				// for (j=ep[i]; j<ep[i+1]; ++j){
 					// p=ei[j];
           // // Va has been updated, update the cosine and sine terms 
           // cosine_array[j] = cos(Va[i]-Va[p]);
           // sine_array[j] = sin(Va[i]-Va[p]);        
           // deltaP[i] += - Vm[i]*Vm[p]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);        
 					// if (btype[i]<2) // calculate Q for PQ buses
             // deltaQ[i] += - Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]); 
 				// }
 			// }
 
 			// // Get max P and max Q
 			// if (abs(deltaP[i]) > maxDeltaP)
 				// maxDeltaP = abs(deltaP[i]);
 			// if (abs(deltaQ[i]) > maxDeltaQ)
 				// maxDeltaQ = abs(deltaQ[i]);
 
 			// deltaP[i] /= Vm[i];
 			// deltaQ[i] /= Vm[i];
 
 			// // check output
 			// // std::cout << "Iter" << iter << ", deltaP" << i << ", " << deltaP[i]<< ", deltaQ" << i << ", " << deltaQ[i]<< std::endl;
     // }
 
 		// std::cout << "Iter" << iter << " Updated for Vm, maxDeltaP, " << maxDeltaP << ", maxDeltaQ, " << maxDeltaQ << std::endl;
 
     // gettimeofday(&t3_end, 0);
 	  // seconds=t3_end.tv_sec  - t3_st.tv_sec; 
 	  // useconds = t3_end.tv_usec  - t3_st.tv_usec;
 	  // printf("\n\n==============================================================================================\n ");
 	  // std::cout << "Time for updating deltaP and deltaQ (iteration):: " << iter << " :" << (seconds*1000000 + useconds) << " us" << std::endl;
 	  // printf("\n================================================================================================\n ");
      
 		// // Decide if the program converges	
 		// if ( maxDeltaP < max_change_ex && maxDeltaQ < max_change_ex){
       // result = "OK";
 			// break;
 		// }

 		// // solve for deltaVm
 		// // std::cout << " ======================== Solve for V magnitude ========================"<<std::endl;
    
     // // A*x = b
     // // multiply deltaQ with row scaling (rows_pp) to get b
   	// for (i=0; i<n; ++i){
      // b[i]=deltaQ[rp_pp[i]]*rows_pp[rp_pp[i]];
   	// }
   	
     // // Full forward substitution 
   	// for (i=0; i<n; ++i)
   	// {
   		// sum=0.;
   		// diag=0.;
   		// for(p=lp_pp[i]; p< lp_pp[i+1]; ++p){
   			// if (i!=li_pp[p]){ // if it is not diagnal element
   				// j=li_pp[p];
   				// sum += lx_pp[p] * b[j]; 
   			// }
   			// else{
   				// diag = lx_pp[p];
   			// }
   		// }
   
   		// b[i] -= sum; 
   		// b[i] /= diag;
   	// } 
   	
     // // Full backward substitution 
   	// for (i=n-1; i>=0; --i)
   	// {
   		// sum=0.;
   		// for(p=up_pp[i]; p< up_pp[i+1]; ++p){
   			// if (i!=ui_pp[p]){ // if it is not diagnal element
   				// sum += ux_pp[p] * b[ui_pp[p]]; 
   			// }
   		// }
   
   		// b[i] -= sum; 
   	// }      
   	
     // // permute the array back to the original order
   	// //for (i=0; i<n; ++i)
     // //{
   	// //	delta[i]=b[cpi_pp[i]];
     // //  //std::cout << "delta: " << i << ", " << delta[i] << std::endl;           
 		// //}
 		
     // // Update V magnitude (Vm)
 		// for (i=0; i<n; ++i)
     // {
       // //Vm[i] -= delta[i];
       // Vm[i] -= b[cpi_pp[i]]*cols_pp[cpi_pp[i]];
     // }       
 	// }
  
 	// gettimeofday(&t2_end, 0);
 	// seconds=t2_end.tv_sec  - t2_st.tv_sec; 
 	// useconds = t2_end.tv_usec  - t2_st.tv_usec;
 	
 	// printf("\n\n============================================================================================== ");
 	// std::cout << "Time for performing power flow iteration: " << (seconds*1000000 + useconds) << " us" << std::endl;
 	// printf("\n================================================================================================ ");
 
 	// // ----------------------------------------------------------------------------------------------------
 	// // 								Store the Result back to the array
 	// // ----------------------------------------------------------------------------------------------------
 	// // Get solved Vm and Va back into HeapAccum
 	// for (ii=0; ii<n; ++ii) {
       // gVertex_Ybus.data_[ii].Va = Va[ii];  // in radian
       // gVertex_Ybus.data_[ii].Vm = Vm[ii];
       // //std::cout << "Final, " << ii+1 << ", Vm, " << Vm[ii]<< ", Va" << ii+1 << ", " << Va[ii]/pi_value*180<< std::endl;
 	// }

  
// EXIT:

	// // std::cout << "\n free3 " << std::endl;
		
	// free(eG);
	// free(eB);
	// free(ei);
	// free(ep);

	// free(deltaP);
	// free(deltaQ);
	// free(Vm);
	// free(Va);		
	// free(Pn);
	// free(Qn);	
  
 	// free(lx);
	// free(li);
	// free(lp);
  // free(ux);
	// free(ui);
	// free(up); 
  // free(rows);
  // free(cols);
  // free(rp);
  // free(cpi); 
 	// free(lx_pp);
	// free(li_pp);
	// free(lp_pp);
 	// free(ux_pp);
	// free(ui_pp);
	// free(up_pp);
  // free(rows_pp);
  // free(cols_pp);
  // free(rp_pp);
  // free(cpi_pp);    
	
  // printf("\n\n----------------------------------------------------------------------------------------");
	// printf("\t\t End of Running GRAPHLU_fdpf_LU_solve_EMS C function!");
	// printf("\t\t ----------------------------------------------------------------------------------------\n\n");	
 
  // return result;
// }




// **********************************************************************************************************************************
// Created by: Chen Yuan, chen.yuan@geirina.net
// Date: 08/08/2017
// This code performs fast-decoupled power flow analysis with PV-PQ conversion. It is used as a benchmark to assess the time it takes to run a fast-decoupled
// power flow in C++ with GRAPHLU. This version is based on the previous version GRAPHLU_fdpf_LU_solve_4_static_mod. In this function, B' and B" are passed
// from gsql and they have the same structure. In other words, B' and B" are both n*n matrices, and only the row and column, which slack bus seats in, have
// all zero elements except the diagonal element (for keeping the matrix's insingularity)
// Then B' is analyzed by ordering and scaling, and also factorized with symbolic and numeric factorization. B" is modified into a matrix which have all zero
// elements in rows and columns of PV and PQ buses, except their diagonal elements which are kept the original values in B". Then the modified B" use the same
// ordering and scaling as B' matrix, since they have the same structure. So only factorization is done for the modified B" matrix.
// In each iteration, PV buses' reactive power will be checked to see if the Q limits are violated on PV buses. If so, change the corresponding PV bus to PQ bus
// and keep its reactive power at its Q limit. After PV-PQ conversion, partial elements in the modified B" bus may need changes so rebuilding and refactorizing 
// the modified B" matrix is needed, using the same ordering and scaling values.
// History: 
// 08/08/2017	[Chen Yuan] Pass same structure B' and B", meaning only swing bus' row and column have all zero non-diagonal elements, into C function and 
//							convert B" to a modified B", whose non-diagonal elements in the rows and columns of PV and swing buses are zero. LU decomposition
//							are separately applied for both B' and modified B" matrices.
// 08/09/2017	[Chen Yuan] Modified code for deltaQ update and PV buses' node reactive power calculation.
// 08/10/2017	[Chen Yuan] Apply B' ordering and scaling for both B' and B", saving B" ordering and scaling time. Use the function GraphLU_ReFactorize
//							to do refactorize during iterations if there exists PV-PQ conversion. Implemented PV-PQ conversion function: If PV-PQ conversion
//							happens, the modified B" matrix needs to be updated based on the original B" matrix and also refactorization is needed. Otherwise,
//							previous iteration's LU decomposition elements can be continued to use.
// **********************************************************************************************************************************
template <typename T_vertex, typename vertex_comp, 
          typename T_matrix_all, typename matrix_all_comp,
          typename T_p_L_matrix, typename p_L_matrix_comp,
          typename T_p_U_matrix, typename p_U_matrix_comp, 
          typename T_pp_L_matrix, typename pp_L_matrix_comp,
          typename T_pp_U_matrix, typename pp_U_matrix_comp,
          typename T_vertex_p, typename vertex_p_comp,
          typename T_vertex_pp, typename vertex_pp_comp>
inline string fdpf_LU_PQPV_Conv (int64_t& gBp, int64_t& gBpp, 
              HeapAccum<T_vertex, vertex_comp>& gVertex, HeapAccum<T_matrix_all, matrix_all_comp>& gMatrix_all,
              HeapAccum<T_p_L_matrix, p_L_matrix_comp>& gMatrix_p_L, HeapAccum<T_p_U_matrix, p_U_matrix_comp>& gMatrix_p_U, HeapAccum<T_vertex_p, vertex_p_comp>& gVertex_p,
              HeapAccum<T_pp_L_matrix, pp_L_matrix_comp>& gMatrix_pp_L, HeapAccum<T_pp_U_matrix, pp_U_matrix_comp>& gMatrix_pp_U, HeapAccum<T_vertex_pp, vertex_pp_comp>& gVertex_pp, 
              MaxAccum<int> maxiter, MaxAccum<double> maxchange) {

	// extern size_t g_si, g_sd, g_sp;
	printf("\n\n------------------------------------------------------------------------------------------- ");
	printf("\nStart Running fdpf_LU_PQPV_Conv function!\n");
  std::cout << "Bp Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gBp << std::endl;
  std::cout << "Bpp Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gBpp << std::endl;
  std::cout << "Y bus Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gMatrix_all.data_.size() << std::endl;
	printf("-------------------------------------------------------------------------------------------- \n\n");	
	// ------------------------------------------------------------------------------------------------
	// 				Initialize variables and arrays
	// ------------------------------------------------------------------------------------------------
	
 
	// Initialize arrays and variables
	uint__t n, nnz, n_pp, nnz_pp, n_e, nnz_e;
	int ret, i, j, p, pv, iter;
	real__t maxDeltaP=0, maxDeltaQ=0, max_change_ex=maxchange;
	SGraphLU *graphlu, *graphlu_pp;
  
  string result = "FAILED";
  
	const double pi_value = 3.141592653589793;
  const int64_t LUflag = 1; 
	// real__t *x, err;	
	// uint__t fctnnz; // number of nonzeros after the matrix is factorized

	// Get the dimension and the nnz of the matrix B' and B"
	n=gVertex.data_.size();	nnz=gBp; n_pp=gVertex.data_.size(); nnz_pp=gBpp;
  n_e=gVertex.data_.size(); 	nnz_e=gMatrix_all.data_.size(); // get the size of the Y bus matrix
  
	
	//// Convert vectors to GRAPHLU Data Structure (pointers)
	real__t *ax, *ax_pp, *ax_pp_mod, *eG, *eB, *deltaP, *deltaQ, *Vm, *Va, *Pn, *Qn, *qUp, *qLower, *Q_PV; 
	uint__t *ai, *ai_pp, *ei, *piv, *btype; 
	uint__t *ap, *ap_pp, *ep; 
 
  // arrays to store the sine and cosine terms of the angle difference
  // values are computed during Jacobian initialization and each iteration 
  real__t *cosine_array, *sine_array;
  cosine_array = (real__t *)malloc(sizeof(real__t)*(nnz_e));
  sine_array = (real__t *)malloc(sizeof(real__t)*(nnz_e)); 
	
	ax = (real__t *)malloc(sizeof(real__t)*(nnz)); // values in B' 
	ai = (uint__t *)malloc(sizeof(uint__t)*(nnz)); // column indices of B'
	ap = (uint__t *)malloc(sizeof(uint__t)*(n+1)); // initial row pointers
	// b = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b

	ax_pp = (real__t *)malloc(sizeof(real__t)*(nnz_pp)); // values in B" (n-1)*(n-1)
	ai_pp = (uint__t *)malloc(sizeof(uint__t)*(nnz_pp)); // column indices of B"
	ap_pp = (uint__t *)malloc(sizeof(uint__t)*(n_pp+1)); // initial row pointers
	ax_pp_mod = (real__t *)malloc(sizeof(real__t)*(nnz_pp)); // values in B" (n-m-1)*(n-m-1)

	eG = (real__t *)malloc(sizeof(real__t)*(nnz_e)); // G values in Y 
	eB = (real__t *)malloc(sizeof(real__t)*(nnz_e)); // B values in Y
	ei = (uint__t *)malloc(sizeof(uint__t)*(nnz_e)); // column indices of Y
	ep = (uint__t *)malloc(sizeof(uint__t)*(n_e+1)); // initial row pointers	

	deltaP = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b
	deltaQ = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b
	Q_PV = (real__t *)malloc(sizeof(real__t)*(n));

	Vm = (real__t *)malloc(sizeof(real__t)*(n)); 
	Va = (real__t *)malloc(sizeof(real__t)*(n)); 

	Pn = (real__t *)malloc(sizeof(real__t)*(n)); 
	Qn = (real__t *)malloc(sizeof(real__t)*(n));
	
	qUp = (real__t *)malloc(sizeof(real__t)*(n));
	qLower = (real__t *)malloc(sizeof(real__t)*(n));

	btype = (uint__t *)malloc(sizeof(uint__t)*(n));
 
  double sum, diag;
  // real__t *delta, *b, *deltaV;
  real__t *b;
  //delta = (real__t *)malloc(sizeof(real__t)*(n));
  b = (real__t *)malloc(sizeof(real__t)*(n));
  //deltaV = (real__t *)malloc(sizeof(real__t)*(n));
  
	std::cout << " ======================== Initialization of ararys used to store the factorized matrix and LU ========================"<< std::endl;

	struct timeval t2_st, t2_end, t3_st, t3_end; long seconds, useconds;

  //initialization to 0
  // [tc] use memset here
  memset(Vm, 0, sizeof(real__t)*(n));
  memset(Va, 0, sizeof(real__t)*(n));
  memset(deltaP, 0, sizeof(real__t)*(n));
  memset(deltaQ, 0, sizeof(real__t)*(n));
  memset(Q_PV, 0, sizeof(real__t)*(n));
	
  // =================================== Sort all input HeapAccum =================================
  gettimeofday(&t3_st, 0);
  gettimeofday(&t2_st, 0);
  // Input HeapAccums are assumed to be unsorted. Sort them before assigning values to
  // local arrays
  gVertex.sort_heap();
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gVertex HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
  
  gettimeofday(&t2_st, 0);  
  gMatrix_all.sort_heap();
 	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gMatrix_all HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
    
  // get the time for sorting all HeapAccums 
	gettimeofday(&t3_end, 0);
	seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	useconds = t3_end.tv_usec  - t3_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort all HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
	
	// ========================================= Convert Ap, Ap_pp, ep and the Vertex =========================================
	gettimeofday(&t2_st, 0); 
  int i_p, i_pp;
	int ii = 1; // counter for the array  
  // Assign value to the first position of the pointer array 
  ap[0] = 0;
  ap_pp[0] = 0;
  ep[0] = 0;
	for (i=0;i<gVertex.size();i++){
    ap[ii] = gVertex.data_[i].Bp_p + ap[ii-1];
    //std::cout<< ii << ",ap,"<< ap[ii] << std::endl;
    ap_pp[ii] = gVertex.data_[i].Bpp_p + ap_pp[ii-1];
    ep[ii] = gVertex.data_[i].ep + ep[ii-1];
    ii++;
    Vm[i] = gVertex.data_[i].Vm;
    Va[i] = gVertex.data_[i].Va;
    Pn[i] = gVertex.data_[i].Pn;
    Qn[i] = gVertex.data_[i].Qn;
    btype[i] = gVertex.data_[i].bustype;
	qUp[i] = gVertex.data_[i].qUp;
	qLower[i] = gVertex.data_[i].qLower;
	}  
 	// ========================================= Convert Ybus (ei, eG and eB), B' (ei and Bp_x) and B" (ei and Bpp_x) =========================================
	i_p = 0;
  i_pp = 0;
  for (ii=0; ii<gMatrix_all.size(); ++ii) {
    ei[ii] = gMatrix_all.data_[ii].ei;
    eG[ii] = gMatrix_all.data_[ii].eG;
    eB[ii] = gMatrix_all.data_[ii].eB; 
    if(gMatrix_all.data_[ii].Bp_x != 0)
    {
      ai[i_p] = gMatrix_all.data_[ii].ei;
      ax[i_p] = gMatrix_all.data_[ii].Bp_x;
      //std::cout<< i_p << ",ax,"<< ax[i_p] << ",ai,"<< ai[i_p] << std::endl;
      i_p++;
    }
    if(gMatrix_all.data_[ii].Bpp_x != 0)
    {
      ai_pp[i_pp] = gMatrix_all.data_[ii].ei;
      ax_pp[i_pp] = gMatrix_all.data_[ii].Bpp_x;
	  //ax_pp_mod[i_pp] = gMatrix_all.data_[ii].Bpp_x;
      i_pp++;
    }
  } 	
  // Done converting all input HeapAccums to arrays
  
	// Get the time to convert the data between containers and arrays
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to convert data to GRAPHLU time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");		
	
	// ----------------------------------------------------------------------------------------------------
	// 								Call GRAPHLU and Factorize B' Matrix
	// ----------------------------------------------------------------------------------------------------
	int error_p = 0, error_pp = 0;
	gettimeofday(&t2_st, 0);
	std::cout << "\n ======================== Start factorizing B' Matirx ======================== \n"<< std::endl;
	// Initialize the structure for GRAPHLU
	graphlu = (SGraphLU *)malloc(sizeof(SGraphLU));
	GraphLU_Initialize(graphlu);

	// Create Matrix
	GraphLU_CreateMatrix(graphlu, n, nnz, ax, ai, ap);
	// graphlu->cfgf[0] = 1.;

	// // Set control parameters
	//graphlu->cfgi[1] = 0; // 0 is no MC64 scaling
	// 					 // 1 is with MC64 scaling

	// Analyze (column/row ordering, scaling. For details please refer to GRAPHLU user's guide)
	GraphLU_Analyze(graphlu);
	printf("analysis time: %.8g\n", graphlu->stat[0]);
	

	// creates the task scheduler for parallel LU factorization. If you want to run 
	// parallel factorization or parallel re-factorization, it should be called after GraphLU Analyze.
	ret = GraphLU_CreateScheduler(graphlu);
	printf("time of creating scheduler: %.8g\n", graphlu->stat[4]);
	printf("suggestion: %s.\n", ret==0?"parallel":"sequential");
	//std::cout << "GraphLU MC64 scaling 4th:" << graphlu->cfgi[1] <<std::endl; 
	// This function creates threads for parallel computation. The second argument (thread)
	// specifies the number of threads, including the main thread. The last argument (check)
	// specifies whether to check the number of threads or not.
  if (!ret){ // parallel factorization 
	  GraphLU_CreateThreads(graphlu, 2, TRUE);
	  printf("total cores: %d, threads created: %d\n", (int)(graphlu->stat[9]), (int)(graphlu->cfgi[5]));
	
	  // This function binds threads to cores (unbind = FALSE) or unbinds threads from cores (unbind = TRUE).
	  GraphLU_BindThreads(graphlu, FALSE);
	
	  // Numerical LU factorization with partial pivoting, parallel
	  error_p = GraphLU_Factorize_MT(graphlu);
	  printf("factorization time: %.8g\n", graphlu->stat[1]);
    if (error_p < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_p <<std::endl;  
  }   
  else{  // Sequential factorization
	  error_p = GraphLU_Factorize(graphlu);
	  printf("Factorization time: %.8g\n", graphlu->stat[1]);
    if (error_p < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_p <<std::endl; 
	}
 
	real__t *lx, *ux; 
	uint__t *li, *ui; 
	size_t *lp, *up; 
	uint__t fctnnz; // number of nonzeros after the matrix is factorized
	size_t l_nnz, u_nnz; // number of nonzeros in L and U factors
	// get the permutation arrays, please refer to graphlu.h for details
	//uint__t *rp, *cp, *rpi, *cpi; //row (column) permutation, row (column) permutation inverse
	uint__t *rp, *cp, *cpi; //row (column) permutation, row (column) permutation inverse
	//real__t *ldiag, *cscale, *rows, *cols;
	real__t *rows, *cols;
	//int__t *pivot, *pivot_inv;

	lx = ux = NULL; 
	li = ui = NULL; 
	lp = up = NULL; 

	int row, col, k;
	int LUrow, LUcol;

	GraphLU_DumpLU(graphlu, &lx, &li, &lp, &ux, &ui, &up); 
     
   // Get the number of nonzeros in the factorized matrix
 	fctnnz = graphlu->lu_nnz;
 	l_nnz = graphlu->l_nnz;
 	u_nnz = graphlu->u_nnz;
 
 	std::cout << " ======================== Number of Total nonzeros after factorization is: "<< graphlu->lu_nnz << "========================"<<std::endl;
 	std::cout << " ======================== Number of nonzeros in L: "<< graphlu->l_nnz << "========================"<<std::endl;
 	std::cout << " ======================== Number of nonzeros in U: "<< graphlu->u_nnz << "========================"<<std::endl;
 	
   // get the permutation arrays, rp and cp may be different if the matrix is not symmetric
	rp = graphlu->row_perm;
	//rpi = graphlu->row_perm_inv;
 	cp = graphlu->col_perm;
	cpi = graphlu->col_perm_inv;

	//ldiag = graphlu->ldiag; // diagnal elements
	//cscale = graphlu->cscale; // scaling factors
	rows = graphlu->row_scale; 
	cols = graphlu->col_scale_perm;
	//pivot = graphlu->pivot; 
	//pivot_inv = graphlu->pivot_inv; // pivoting array   
    
 	// print out and check

  
    //get the factorized LU values and find the row and col before permutation
    for (i = 0; i < n; ++i){
      row = rp[i];
      LUrow = row+1;
      
      gVertex_p += T_vertex_p(i+1, lp[i+1]-lp[i], up[i+1]-up[i], rp[i], cpi[i], rows[i], cols[i], rp[i]+1); 
      // process all non-zeros in L matrix on row i
      for (j = lp[i]; j < lp[i+1]; ++j){
        col = cp[li[j]];             
        LUcol = col+1;
        
        gMatrix_p_L += T_p_L_matrix((i+1)*100000+(li[j]+1), li[j], lx[j]); 
      } 
         
      // process all non-zeros in U matrix on row i
      for (j = up[i]; j < up[i+1]; ++j){
        col = cp[ui[j]];
        LUcol = col+1; 
        
        gMatrix_p_U += T_p_U_matrix((i+1)*100000+(ui[j]+1), ui[j], ux[j]);          
      }   
    }
    gVertex_p.sort_heap();
    gMatrix_p_L.sort_heap();
    gMatrix_p_U.sort_heap();
	
	
	real__t *lx_pp, *ux_pp; 
	uint__t *li_pp, *ui_pp; 
	size_t *lp_pp, *up_pp; 
	uint__t fctnnz_pp; // number of nonzeros after the matrix is factorized
	size_t l_nnz_pp, u_nnz_pp; // number of nonzeros in L and U factors
	// get the permutation arrays, please refer to graphlu.h for details
	//uint__t *rp_pp, *cp_pp, *rpi_pp, *cpi_pp; //row (column) permutation, row (column) permutation inverse
	uint__t *rp_pp, *cpi_pp; //row (column) permutation, row (column) permutation inverse
	//real__t *ldiag_pp, *cscale_pp, *rows_pp, *cols_pp;
	real__t *rows_pp, *cols_pp;
	//int__t *pivot_pp, *pivot_inv_pp; 
	
	// ----------------------------------------------------------------------------------------------------
	// 								Call GRAPHLU and Factorize B" Matirx
	// ----------------------------------------------------------------------------------------------------
	std::cout << "\n ======================== Start factorizing B'' Matirx ======================== \n"<< std::endl;
	// Initialize the structure for GRAPHLU
	graphlu_pp = (SGraphLU *)malloc(sizeof(SGraphLU));
	GraphLU_Initialize(graphlu_pp);	
	
	if(error_p >= 0){
		int PVPQ = 1; // for the first iteration, needs to build the modified B" matrix, so set PVPV = 1, you can freely set it to other non-zerovalues.
		for (iter=0; iter<maxiter; ++iter){
			
			if(PVPQ != 0) // Decide if PV-PQ conversion happened in the previous iteration, PVPQ = 0 indicates no PV-PQ conversion happened in the previous iteration, else happens.
			// if PV-PQ conversion happened in the previous iteration or the first iteration, modified B" matrix needs update and also a refactorization is required for the updated modified B" matrix
			{	
				PVPQ = 0; // if PV-PQ conversion happens in the previous iteration, reset PVPQ to zero for this iteration's PV-PQ conversion check
				for (i = 0; i < n; ++i)  // Pass ax_pp to ax_pp_mod
				{
					for (j = ap_pp[i]; j < ap_pp[i+1]; ++j)
					{
						ax_pp_mod[j] = ax_pp[j];
					}
				}
				
				for (i = 0; i < n; ++i) // update the modified B" matrix if PV-PQ conversion happens
				{
					//pv = -10;
					if (btype[i] == 2) // check if it is PV bus
					{
						// pv = i; // assign the row # of PV node to pv
						// for (j = ap_pp[i]; j < ap_pp[i+1]; ++j)
						// {
							// if (ai[j] != i)
							// {
								// ax_pp_mod[j] = 0;
							// }
						// }
						
						for (ii = 0; ii < n; ++ii) // traverse the whole matrix, change nonzero non-diagonal elements values to zeros in the rows and columns of PV nodes
						{
							for (j = ap_pp[ii]; j < ap_pp[ii+1]; ++j)
							{
								if ((ii == i && ai_pp[j] != ii) || (ai_pp[j] == i && ai_pp[j] != ii))
								{
									ax_pp_mod[j] = 0;
								}
							}
						}
					
					
					}
					
					// if (pv >= 0) // change nonzero non-diagonal elements to zeros in the columns of PV nodes
					// {
						// for (ii = 0; ii < n; ++ii)
						// {
							// for (j = ap_pp[ii]; j < ap_pp[ii+1]; ++j)
							// {
								// if (ai[j] == pv and ai[j] != ii)
								// {
									// ax_pp_mod[j] = 0;
								// }
							// }
						// }

					// }		
				}
				
				
			
			
	 			/*//debugging; Print out ax_pp_mod
				for (i = 0; i < n; ++i)
				{
					for (j = ap_pp[i]; j < ap_pp[i+1]; ++j)
					{
						std::cout << "j " << j << ", ax_pp " << ax_pp_mod[j] << ", ai_pp " << ai_pp[j] << std::endl;
					}
				}
				for (i = 0; i < n; ++i)
				{
					std::cout << "i " << i << " ap_pp[i]" << ap_pp[i] << std::endl;
				} 
				std::cout << "Iter" << iter << std::endl;	 */

		 
				// Create Matrix
				//std::cout << "Iter" << iter << std::endl;	
				//GraphLU_CreateMatrix(graphlu_pp, n_pp, nnz_pp, ax_pp_mod, ai_pp, ap_pp);
				// graphlu_pp->cfgf[0] = 1.;

				//std::cout << "Iter" << iter << std::endl;	
				// // Set control parameters
				//graphlu_pp->cfgi[1] = 0; // 0 is no MC64 scaling
				// 					 	  // 1 is with MC64 scaling
			 
				// Analyze (column/row ordering, scaling. For details please refer to GRAPHLU user's guide)
				//GraphLU_Analyze(graphlu_pp);
				//printf("analysis time: %.8g\n", graphlu_pp->stat[0]); 
			
			
				if (iter == 0)
				{
					GraphLU_CreateMatrix(graphlu_pp, n_pp, nnz_pp, ax_pp_mod, ai_pp, ap_pp);
					// graphlu_pp->cfgf[0] = 1.;
					// std::cout << "Iter" << iter << std::endl;	
					// graphlu_pp->flag[1] = true;
					// graphlu_pp->row_perm = graphlu->row_perm;
					// graphlu_pp->row_perm_inv = graphlu->row_perm_inv;
					// graphlu_pp->col_perm = graphlu->col_perm;
					// graphlu_pp->col_perm_inv = graphlu->col_perm_inv;
					// graphlu_pp->row_scale = graphlu->row_scale; 
					// graphlu_pp->col_scale_perm = graphlu->col_scale_perm;
					// permute_and_scale(graphlu_pp);
					// std::cout << "Iter" << iter << std::endl; 
					// Analyze (column/row ordering, scaling. For details please refer to GRAPHLU user's guide)
					GraphLU_Analyze(graphlu_pp);
					printf("analysis time: %.8g\n", graphlu_pp->stat[0]); 
				
				
					//graphlu_pp->flag[1] = true;
					//permute_and_scale(graphlu_pp);
					// creates the task scheduler for parallel LU factorization. If you want to run 
					// parallel factorization or parallel re-factorization, it should be called after GraphLU Analyze.
					 // ret = GraphLU_CreateScheduler(graphlu_pp);
					 // printf("time of creating scheduler: %.8g\n", graphlu_pp->stat[4]);
					 // printf("suggestion: %s.\n", ret==0?"parallel":"sequential");

					// // This function creates threads for parallel computation. The second argument (thread)
					// // specifies the number of threads, including the main thread. The last argument (check)
					// // specifies whether to check the number of threads or not.
					 // if (!ret){ // parallel factorization
						// GraphLU_CreateThreads(graphlu_pp, 2, TRUE);
						// printf("total cores: %d, threads created: %d\n", (int)(graphlu_pp->stat[9]), (int)(graphlu_pp->cfgi[5]));

						 // // This function binds threads to cores (unbind = FALSE) or unbinds threads from cores (unbind = TRUE).
						// GraphLU_BindThreads(graphlu_pp, FALSE);

						// // Numerical LU factorization with partial pivoting, parallel
						// error_pp = GraphLU_Factorize_MT(graphlu_pp);
						// printf("factorization time: %.8g\n", graphlu_pp->stat[1]);
						// if (error_pp < 0) //there is an error, print the code 
							// std::cout << "GraphLU_Factorize error code:" << error_pp <<std::endl;      
					// }
					//else
					//{  //Sequential factorization 
						error_pp = GraphLU_Factorize(graphlu_pp);
						printf("Factorization time: %.8g\n", graphlu_pp->stat[1]);
						if (error_pp < 0) //there is an error, print the code 
							std::cout << "GraphLU_Factorize error code:" << error_pp <<std::endl;
					//}						
				}
				else
				{
					error_pp = GraphLU_ReFactorize(graphlu_pp, ax_pp_mod);
					printf("ReFactorization time: %.8g\n", graphlu_pp->stat[1]);
					if (error_pp < 0) //there is an error, print the code 
					std::cout << "GraphLU_ReFactorize error code:" << error_pp <<std::endl; 
				}
			
	

				if(error_pp >= 0){
  
					lx_pp = ux_pp = NULL; 
					li_pp = ui_pp = NULL; 
					lp_pp = up_pp = NULL; 
				  
					GraphLU_DumpLU(graphlu_pp, &lx_pp, &li_pp, &lp_pp, &ux_pp, &ui_pp, &up_pp); 
				   
				   // Get the number of nonzeros in the factorized matrix
					fctnnz_pp = graphlu_pp->lu_nnz;
					l_nnz_pp = graphlu_pp->l_nnz;
					u_nnz_pp = graphlu_pp->u_nnz;
				 
					std::cout << " ======================== Number of Total nonzeros after factorization is: "<< graphlu_pp->lu_nnz << "========================"<<std::endl;
					std::cout << " ======================== Number of nonzeros in L: "<< graphlu_pp->l_nnz << "========================"<<std::endl;
					std::cout << " ======================== Number of nonzeros in U: "<< graphlu_pp->u_nnz << "========================"<<std::endl;
					
				  // get the permutation arrays, rp and cp may be different if the matrix is not symmetric
					rp_pp = graphlu_pp->row_perm;
					//rpi_pp = graphlu_pp->row_perm_inv;
					//cp_pp = graphlu_pp->col_perm;
					cpi_pp = graphlu_pp->col_perm_inv;
				  
					//ldiag_pp = graphlu_pp->ldiag; // diagnal elements
					//cscale_pp = graphlu_pp->cscale; // sclaing factors
					rows_pp = graphlu_pp->row_scale; 
					cols_pp = graphlu_pp->col_scale_perm;
					//pivot_pp = graphlu_pp->pivot; 
					//pivot_inv_pp = graphlu_pp->pivot_inv; // pivoting array   
				}
				else 
				{
					//B'' factorization failed
					result = "FAILED";
					printf("\n\n============================================================================================== ");
					std::cout << "B'' Factorization FAILED" << std::endl;
					printf("\n================================================================================================ ");
					break;
				}
				
			}
			
    //get the factorized LU values and find the row and col before permutation
	
/*     for (i = 0; i < n; ++i)
	{
      row = rp_pp[i];
      LUrow = row+1;
      
      gVertex_pp += T_vertex_pp(i+1, lp_pp[i+1]-lp_pp[i], up_pp[i+1]-up_pp[i], rp_pp[i], cpi_pp[i], rows_pp[i], cols_pp[i], rp_pp[i]+1); 
      // process all non-zeros in L matrix on row i
      for (j = lp_pp[i]; j < lp_pp[i+1]; ++j){
        col = cp_pp[li_pp[j]];
        LUcol = col+1;
        
        gMatrix_pp_L += T_pp_L_matrix((i+1)*100000+(li_pp[j]+1), li_pp[j], lx_pp[j]); 
      } 
      
      // process all non-zeros in U matrix on row i
      for (j = up_pp[i]; j < up_pp[i+1]; ++j){
        col = cp_pp[ui_pp[j]];
        LUcol = col+1;
        
        gMatrix_pp_U += T_pp_U_matrix((i+1)*100000+(ui_pp[j]+1), ui_pp[j], ux_pp[j]);
      }    
    } */

  
  // Get the time for factorizing B' and B"
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	
	printf("\n\n============================================================================================== ");
	std::cout << "Time for GRAPHLU to factorize B' and B'':: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");
  //double Bii = 0.0;
  //if(error_p >= 0 && error_pp >= 0){
    //continue to do the iterations only if there is no error from factorization
  	printf("\n\n============================================================================================== ");
  	std::cout << "\n Start iteratively updating deltaP and deltaQ " << std::endl;
  	printf("\n================================================================================================\n ");
  
  	gettimeofday(&t2_st, 0);
   
  	//for (iter=0; iter<maxiter; ++iter){
  		// update deltaP and deltaQ
  		maxDeltaP=0;
  		maxDeltaQ=0;
  
  		// std::cout << " ======================== Update deltaP and deltaQ ========================"<<std::endl;
      gettimeofday(&t3_st, 0);
  		// Calculate deltaP and deltaQ to update Va
  		for (i=0; i<n; ++i){
  			deltaP[i] = 0;
  			deltaQ[i] = 0;
  
  			if (btype[i]!=3){
  				deltaP[i] = Pn[i];
  				if (btype[i]<2)
					deltaQ[i] = Qn[i];
  			}
  
  			// Calculate network injections
  			if (btype[i]!=3){
  				for (j=ep[i]; j<ep[i+1]; ++j){
  					p=ei[j];
					//if (p == i){
						//Bii = eB[j];
					//}
					if (iter == 0){        
						//if this is the first iteraton, calculate the cosine and sine terms        
						cosine_array[j] = cos(Va[i]-Va[p]);
						sine_array[j] = sin(Va[i]-Va[p]);
					}    
					deltaP[i] += - Vm[i]*Vm[p]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);        
					if (btype[i]<2) // calculate Q for PQ buses
					{
						deltaQ[i] += - Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]); 
					}
					
					if (btype[i] == 2) // calculate reactive power in each PV bus
					{
						Q_PV[i] += Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]); // calculate Qcal for PV bus and save it for future PVPQ conversion usage
					}
					//else  // calculate Q for PQ buses
					//{
						//deltaQ[i] += - Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]) - Bii*deltaV[i];
					//}
					
				}
				//Bii = 0.0;
  			}
		
        
  			// Get max P and max Q
  			if (abs(deltaP[i]) > maxDeltaP)
  				maxDeltaP = abs(deltaP[i]);
  			if (btype[i]<2)
			{
				if (abs(deltaQ[i]) > maxDeltaQ)
				{
					maxDeltaQ = abs(deltaQ[i]);
				}
  				
			}

  			deltaP[i] /= Vm[i];
  			deltaQ[i] /= Vm[i];
		}			
		
		for (i=0; i<n; ++i){
			
			if (btype[i] == 2 && maxDeltaQ < 1.5*max_change_ex)  // PV convert to PQ if violating Q limits
			{
				if(deltaQ[i] < qLower[i])
				//if(Q_PV[i] < -1000) // PV convert to PQ if violating Q lower limit
				{
					Qn[i] = qLower[i];  // set Q to its lower limit
					btype[i] = 0;  // convert it to PQ bus
					PVPQ +=1; // counting the number of PV to PQ conversion
				}
				if(deltaQ[i] > qUp[i])
				//if(Q_PV[i] > 1000) // PV convert to PQ if violating Q lower limit
				{
					Qn[i] = qUp[i]; // set Q to its upper limit
					btype[i] = 0; // convert it to PQ bus
					PVPQ +=1; // counting the number of PV to PQ conversion
					
				}
			}
			Q_PV[i] = 0;  // reset (PV bus reactive power) to zero since following forward/backward substitution need to use the deltaQ values for buses.
			
		}
  
  			// check output
 			  //std::cout << "Iter" << iter << ", deltaP" << i << ", " << deltaP[i]<< ", deltaQ" << i << ", " << deltaQ[i]<< std::endl;

  
  		std::cout << "Iter" << iter << " Updated for Va, maxDeltaP, " << maxDeltaP << ", maxDeltaQ, " << maxDeltaQ << std::endl;
   	  
      gettimeofday(&t3_end, 0);
  	  seconds=t3_end.tv_sec  - t3_st.tv_sec; 
  	  useconds = t3_end.tv_usec  - t3_st.tv_usec;
  	  printf("\n\n==============================================================================================\n ");
  	  std::cout << "Time for updating deltaP and deltaQ (iteration):: " << iter << " :" << (seconds*1000000 + useconds) << " us" << std::endl;
  	  printf("\n================================================================================================\n ");
      
      // Decide if the program converges	
  		if ( maxDeltaP < max_change_ex && maxDeltaQ < max_change_ex){
			result = "OK";
  			break;
  		}
		
		std::cout << "PVPQ " << PVPQ << std::endl;
		if (PVPQ != 0) // Decide if there exists PVPQ conversion
		{
			continue;
		}
      
      //reset the max mismatch
      maxDeltaP = 0;
      maxDeltaQ = 0;
      
   	  // solve for deltaVa
  	  // std::cout << " ======================== Solve for V angle ========================"<<std::endl;
      
      // A*x = b
      // multiply deltaP with row scaling (rows) to get b
    	for (i=0; i<n; ++i){
        b[i]=deltaP[rp[i]]*rows[rp[i]];
    	}
     
    	// Full forward substitution 
    	for (i=0; i<n; ++i)
    	{
    		sum=0.;
    		diag=0.;
    		for(p=lp[i]; p< lp[i+1]; ++p){
    			if (i!=li[p]){ // if it is not diagnal element
    				j=li[p];
    				sum += lx[p] * b[j];
    			}
    			else{
    				diag = lx[p];
    			}
    		}

    		b[i] -= sum;
    		b[i] /= diag;
    	} 
     
    	// Full backward substitution 
    	for (i=n-1; i>=0; --i)
    	{
    		sum=0.;
    		for(p=up[i]; p< up[i+1]; ++p){
    			if (i!=ui[p]){ // if it is not diagnal element
    				sum += ux[p] * b[ui[p]]; 
    			}
    		}  
    		b[i] -= sum; 
    	}      
    	// permute the array back to the original order
    	//for (i=0; i<n; ++i)
      //{
    	//	delta[i]=b[cpi[i]];
      //  //std::cout << "delta: " << i << ", " << delta[i] << std::endl;           
  		//}
        
      // Update V angle (Va)
  		for (i=0; i<n; ++i)
      { 
        //Va[i] -= delta[i];  
        Va[i] -= b[cpi[i]]*cols[cpi[i]];      
      }
     
  		// std::cout << " ======================== Update deltaP and deltaQ ========================"<<std::endl;
      gettimeofday(&t3_st, 0);
  		// Calculate deltaP and deltaQ to update Vm
  		for (i=0; i<n; ++i){
  			deltaP[i] = 0;
  			deltaQ[i] = 0;
  
  			if (btype[i]!=3){
  				deltaP[i] = Pn[i];
  				if (btype[i]<2)
  					deltaQ[i] = Qn[i];
  			}
  
  			// Calculate network injections
  			if (btype[i]!=3){
  				for (j=ep[i]; j<ep[i+1]; ++j){
  					p=ei[j];
					//if (p == i){
						//Bii = eB[j];
					//}
					// Va has been updated, update the cosine and sine terms 
					cosine_array[j] = cos(Va[i]-Va[p]);
					sine_array[j] = sin(Va[i]-Va[p]);        
					deltaP[i] += - Vm[i]*Vm[p]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);        
					if (btype[i]<2) // calculate Q for PQ buses
					{
						deltaQ[i] += - Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]); 
					}
					if (btype[i] == 2) // when btype[i] = 2
					{
						Q_PV[i] += Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]); // calculate Qcal for PV bus and save it for future PVPQ conversion usage
					}
					//else  // calculate Q for PQ buses
					//{
						//deltaQ[i] = - Bii*deltaV[i];
						//deltaQ[i] += - Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]) - Bii*deltaV[i];
						//std::cout << "#" << i << " deltaV: " << deltaV[i] << " deltaQ" << deltaQ[i] << " Bii" << Bii << std::endl;   
					//}
					//Bii = 0.0;	
				}
				
  			}
  
  			// Get max P and max Q
  			if (abs(deltaP[i]) > maxDeltaP)
  				maxDeltaP = abs(deltaP[i]);
  			//if (abs(deltaQ[i]) > maxDeltaQ)
  				//maxDeltaQ = abs(deltaQ[i]);
			
			//if (btype[i]<2)
			//{
  			if (btype[i]<2)
			{
				if (abs(deltaQ[i]) > maxDeltaQ)
				{
					maxDeltaQ = abs(deltaQ[i]);
				}
  				
			}
			  
  			deltaP[i] /= Vm[i];
  			deltaQ[i] /= Vm[i];
		}
				//}
  		for (i=0; i<n; ++i)
		{		
			//} 
			if (btype[i] == 2 && maxDeltaQ < 1.5*max_change_ex)  // PV convert to PQ if violating Q limits
			{
				if(deltaQ[i] < qLower[i]) // PV convert to PQ if violating Q lower limit
				//if(deltaQ[i] < -1000) // PV convert to PQ if violating Q lower limit
				{
					Qn[i] = qLower[i]; // set Q to its lower limit
					btype[i] = 0; // convert it to PQ bus
					PVPQ +=1; // counting the number of PV to PQ conversion
					
				}
				if(deltaQ[i] > qUp[i]) // PV convert to PQ if violating Q upper limit
				//if(deltaQ[i] > 1000) // PV convert to PQ if violating Q lower limit
				{
					Qn[i] = qUp[i]; // set Q to its upper limit
					btype[i] = 0; // convert it to PQ bus
					PVPQ +=1; // counting the number of PV to PQ conversion
					
				}
			}
				Q_PV[i] = 0; // reset it (PV bus reactive power) to zero since following forward/backward substitution need to use the deltaQ values for buses.
		

		}
  
  			// check output
  			//std::cout << "Iter" << iter << ", deltaP" << i << ", " << deltaP[i]<< ", deltaQ" << i << ", " << deltaQ[i]<< std::endl;
      
  
  		std::cout << "Iter" << iter << " Updated for Vm, maxDeltaP, " << maxDeltaP << ", maxDeltaQ, " << maxDeltaQ << std::endl;
  
      gettimeofday(&t3_end, 0);
  	  seconds=t3_end.tv_sec  - t3_st.tv_sec; 
  	  useconds = t3_end.tv_usec  - t3_st.tv_usec;
  	  printf("\n\n==============================================================================================\n ");
  	  std::cout << "Time for updating deltaP and deltaQ (iteration):: " << iter << " :" << (seconds*1000000 + useconds) << " us" << std::endl;
  	  printf("\n================================================================================================\n ");
       
  		// Decide if the program converges	
  		if ( maxDeltaP < max_change_ex && maxDeltaQ < max_change_ex){
        result = "OK";
  			break;
  		}
		std::cout << "PVPQ " << PVPQ << std::endl;
		if (PVPQ != 0) // Decide if there exists PVPQ conversion
		{
			continue;
		}

  		// solve for deltaVm
  		// std::cout << " ======================== Solve for V magnitude ========================"<<std::endl;
     
      // A*x = b
      // multiply deltaQ with row scaling (rows_pp) to get b
    	for (i=0; i<n; ++i){
			b[i]=deltaQ[rp_pp[i]]*rows_pp[rp_pp[i]];
    	}
     
    	// Full forward substitution 
    	for (i=0; i<n; ++i)
    	{
    		//if (btype[rp_pp[i]]<2)  // PQ bus
			//{
				sum=0.;
				diag=0.;
				for(p=lp_pp[i]; p< lp_pp[i+1]; ++p){
					if (i!=li_pp[p]){ // if it is not diagnal element
						j=li_pp[p];
						sum += lx_pp[p] * b[j]; 
					}
					else{
						diag = lx_pp[p];
					}
				}

				b[i] -= sum; 
				b[i] /= diag;
			//}
			//else
			//{
				//b[i] = 0;
			//}
			//std::cout << "i " << i << " b[i], " << b[i] << std::endl;
    	} 
     
    	// Full backward substitution 
    	for (i=n-1; i>=0; --i)
    	{
			//if (btype[rp_pp[i]]<2)  // PQ bus
			//{
				sum=0.;
				for(p=up_pp[i]; p< up_pp[i+1]; ++p)
				{
					if (i!=ui_pp[p])
					{ // if it is not diagnal element
						sum += ux_pp[p] * b[ui_pp[p]]; 
					}
				}
			
    		
				//if (btype[rp_pp[i]]<2)  // PQ bus
				//{
					b[i] -= sum;
				//}
				//else 
				//{
					//b[i] = 0;
				//}
			//}
			//else  // PV bus or slack bus
			//{
				/* sum=0.;
				for(p=up_pp[i]; p< up_pp[i+1]; ++p)
				{
					if (i!=ui_pp[p])
					{ // if it is not diagnal element
						sum += ux_pp[p] * b[ui_pp[p]]; 
					}
				}
			
				b[i] -= sum;
				deltaV[i] = b[i]; */
				//b[i] = 0;  //deltaVm = 0 for PV bus and slack bus
				
			//}
    		//std::cout << "i " << i << " b[i], " << b[i] << std::endl;
    	}      
    	// permute the array back to the original order
    	//for (i=0; i<n; ++i)
      //{
    	//	delta[i]=b[cpi_pp[i]];
      //  //std::cout << "delta: " << i << ", " << delta[i] << std::endl;           
  		//}
  
  		// Update V magnitude (Vm)
  		for (i=0; i<n; ++i)
      {
        //Vm[i] -= delta[i];
        Vm[i] -= b[cpi_pp[i]]*cols_pp[cpi_pp[i]];
      }

	//free(graphlu_pp->nnz);
	//free(graphlu_pp->col_perm_inv);
	//GraphLU_Destroy(graphlu_pp);
	
  	}
   
  	gettimeofday(&t2_end, 0);
  	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
  	useconds = t2_end.tv_usec  - t2_st.tv_usec;
  	
  	printf("\n\n============================================================================================== ");
  	std::cout << "Time for performing power flow iteration: " << (seconds*1000000 + useconds) << " us" << std::endl;
	std::cout << "Number of iterations: " << iter << std::endl;
  	printf("\n================================================================================================ ");
  
  	// ----------------------------------------------------------------------------------------------------
  	// 								Store the Result back to the array
  	// ----------------------------------------------------------------------------------------------------
  	// Get solved Vm and Va back into HeapAccum
  	for (ii=0; ii<n; ++ii) {
        gVertex.data_[ii].Va = Va[ii]; // in radian
        gVertex.data_[ii].Vm = Vm[ii];
        //std::cout << "Final, " << ii+1 << ", Vm, " << Vm[ii]<< ", Va" << ii+1 << ", " << Va[ii]/pi_value*180<< std::endl;
		//std::cout << "\nFinal" << std::endl;
  	}
  } else {
    //B' factorization failed
 	  result = "FAILED";
    printf("\n\n============================================================================================== ");
  	std::cout << "B' Factorization FAILED" << std::endl;
  	printf("\n================================================================================================ ");  
  }
EXIT:
	//std::cout << "\n free1 " << std::endl;
	GraphLU_Destroy(graphlu);
	//std::cout << "\n free1_pp " << std::endl;
	GraphLU_Destroy(graphlu_pp);
	//std::cout << "\n free2 " << std::endl;
	free(ax);
	free(ai);
	free(ap);
	free(ax_pp);
	free(ax_pp_mod);
	free(ai_pp);
	free(ap_pp);
	free(graphlu);
	free(graphlu_pp);
	//std::cout << "\n free3 " << std::endl;
		
	free(eG);
	free(eB);
	free(ei);
	free(ep);
	//std::cout << "\n free4 " << std::endl;

	free(deltaP);
	free(deltaQ);
	free(qUp);
	free(qLower);
	free(Vm);
	free(Va);		
	free(Pn);
	free(Qn);
	free(btype);
	free(cosine_array);
	free(sine_array);
	free(b);
	//std::cout << "\n free5 " << std::endl;
  
 	free(lx);
	free(li);
	free(lp);
	free(ux);
	free(ui);
	free(up);  
 	free(lx_pp);
	free(li_pp);
	free(lp_pp);
 	free(ux_pp);
	free(ui_pp);
	free(up_pp); 
	//std::cout << "\n free6 " << std::endl;

	/* free(rp);
	free(cp);
	//free(rpi);
	free(cpi);
	free(rp_pp);
	//free(cp_pp);
	//free(rpi_pp);
	free(cpi_pp);
	free(rows);
	free(cols);
	free(rows_pp);
	free(cols_pp); */
	//std::cout << "\n free7 " << std::endl;

	std::cout << "\n\n----------------------------------------------------------------------------------------" << std::endl;
	std::cout << "\t\t End of Running fdpf_LU_PQPV_Conv C function!" << std::endl;
	std::cout << "\t\t ----------------------------------------------------------------------------------------\n\n"<< std::endl;
 
	return result;
}


// **********************************************************************************************************************************
// Created by: Chen Yuan, chen.yuan@geirina.net
// Date: 08/08/2017
// This code performs fast-decoupled power flow analysis with PV-PQ conversion. It is used as a benchmark to assess the time it takes to run a fast-decoupled
// power flow in C++ with GRAPHLU. This version is based on the previous version GRAPHLU_fdpf_LU_solve_4_static_mod. In this function, B' and B" are passed
// from gsql and they have the same structure. In other words, B' and B" are both n*n matrices, and only the row and column, which slack bus seats in, have
// all zero elements except the diagonal element (for keeping the matrix's insingularity)
// Then B' is analyzed by ordering and scaling, and also factorized with symbolic and numeric factorization. B" is modified into a matrix which have all zero
// elements in rows and columns of PV and PQ buses, except their diagonal elements which are kept the original values in B". Then the modified B" use the same
// ordering and scaling as B' matrix, since they have the same structure. So only factorization is done for the modified B" matrix.
// In each iteration, PV buses' reactive power will be checked to see if the Q limits are violated on PV buses. If so, change the corresponding PV bus to PQ bus
// and keep its reactive power at its Q limit. After PV-PQ conversion, partial elements in the modified B" bus may need changes so rebuilding and refactorizing 
// the modified B" matrix is needed, using the same ordering and scaling values.
// History: 
// 08/08/2017	[Chen Yuan] Pass same structure B' and B", meaning only swing bus' row and column have all zero non-diagonal elements, into C function and 
//							convert B" to a modified B", whose non-diagonal elements in the rows and columns of PV and swing buses are zero. LU decomposition
//							are separately applied for both B' and modified B" matrices.
// 08/09/2017	[Chen Yuan] Modified code for deltaQ update and PV buses' node reactive power calculation.
// 08/10/2017	[Chen Yuan] Apply B' ordering and scaling for both B' and B", saving B" ordering and scaling time. Use the function GraphLU_ReFactorize
//							to do refactorize during iterations if there exists PV-PQ conversion. Implemented PV-PQ conversion function: If PV-PQ conversion
//							happens, the modified B" matrix needs to be updated based on the original B" matrix and also refactorization is needed. Otherwise,
//							previous iteration's LU decomposition elements can be continued to use.
// 02/09/2018	[Chen Yuan] Made modifications to avoid PV-PQ conversion in each iteration, only conducts Var limits violation check and PV-PQ conversion when the
//							power flow calculation converges. If Var limit violation happens, converts the PV bus to PQ bus and go back to redo power flow.
// **********************************************************************************************************************************
template <typename T_vertex, typename vertex_comp, 
          typename T_matrix_all, typename matrix_all_comp>
		  
inline string fdpf_LU_PQPV_Conv_only (int64_t& gBp, int64_t& gBpp, 
              HeapAccum<T_vertex, vertex_comp>& gVertex, HeapAccum<T_matrix_all, matrix_all_comp>& gMatrix_all,
              MaxAccum<int> maxiter, MaxAccum<double> maxchange) {

	// extern size_t g_si, g_sd, g_sp;
	printf("\n\n------------------------------------------------------------------------------------------- ");
	printf("\nStart Running fdpf_LU_PQPV_Conv_only function!\n");
    std::cout << "Bp Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gBp << std::endl;
	std::cout << "Bpp Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gBpp << std::endl;
	std::cout << "Y bus Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gMatrix_all.data_.size() << std::endl;
	printf("-------------------------------------------------------------------------------------------- \n\n");	
	// ------------------------------------------------------------------------------------------------
	// 				Initialize variables and arrays
	// ------------------------------------------------------------------------------------------------
 
	// Initialize arrays and variables
	uint__t n, nnz, n_pp, nnz_pp, n_e, nnz_e;
	int ret, i, j, p, pv, iter;
	real__t maxDeltaP=0, maxDeltaQ=0, max_change_ex=maxchange;
	SGraphLU *graphlu, *graphlu_pp;
	string result = "FAILED";
	const double pi_value = 3.141592653589793;
	//const int64_t LUflag = 1; 
	// real__t *x, err;	
	// uint__t fctnnz; // number of nonzeros after the matrix is factorized

	// Get the dimension and the nnz of the matrix B' and B"
	n=gVertex.data_.size();	nnz=gBp; n_pp=gVertex.data_.size(); nnz_pp=gBpp;
	n_e=gVertex.data_.size(); 	nnz_e=gMatrix_all.data_.size(); // get the size of the Y bus matrix
  
	//// Convert vectors to GRAPHLU Data Structure (pointers)
	real__t *ax, *ax_pp, *ax_pp_mod, *eG, *eB, *deltaP, *deltaQ, *Vm, *Va, *Vm_init, *Va_init, *Pn, *Qn, *qUp, *qLower, *Q_PV; 
	uint__t *ai, *ai_pp, *ei, *piv, *btype, *exId; 
	uint__t *ap, *ap_pp, *ep; 

	// arrays to store the sine and cosine terms of the angle difference
	// values are computed during Jacobian initialization and each iteration 
	real__t *cosine_array, *sine_array;
	cosine_array = (real__t *)malloc(sizeof(real__t)*(nnz_e));
	sine_array = (real__t *)malloc(sizeof(real__t)*(nnz_e)); 
	
	ax = (real__t *)malloc(sizeof(real__t)*(nnz)); // values in B' 
	ai = (uint__t *)malloc(sizeof(uint__t)*(nnz)); // column indices of B'
	ap = (uint__t *)malloc(sizeof(uint__t)*(n+1)); // initial row pointers
	// b = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b

	ax_pp = (real__t *)malloc(sizeof(real__t)*(nnz_pp)); // values in B" (n-1)*(n-1)
	ai_pp = (uint__t *)malloc(sizeof(uint__t)*(nnz_pp)); // column indices of B"
	ap_pp = (uint__t *)malloc(sizeof(uint__t)*(n_pp+1)); // initial row pointers
	ax_pp_mod = (real__t *)malloc(sizeof(real__t)*(nnz_pp)); // values in B" (n-m-1)*(n-m-1)

	eG = (real__t *)malloc(sizeof(real__t)*(nnz_e)); // G values in Y 
	eB = (real__t *)malloc(sizeof(real__t)*(nnz_e)); // B values in Y
	ei = (uint__t *)malloc(sizeof(uint__t)*(nnz_e)); // column indices of Y
	ep = (uint__t *)malloc(sizeof(uint__t)*(n_e+1)); // initial row pointers	

	deltaP = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b
	deltaQ = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b
	Q_PV = (real__t *)malloc(sizeof(real__t)*(n));

	Vm = (real__t *)malloc(sizeof(real__t)*(n)); 
	Va = (real__t *)malloc(sizeof(real__t)*(n)); 
	Vm_init = (real__t *)malloc(sizeof(real__t)*(n)); 
	Va_init = (real__t *)malloc(sizeof(real__t)*(n)); 

	Pn = (real__t *)malloc(sizeof(real__t)*(n)); 
	Qn = (real__t *)malloc(sizeof(real__t)*(n));
	
	qUp = (real__t *)malloc(sizeof(real__t)*(n));
	qLower = (real__t *)malloc(sizeof(real__t)*(n));

	btype = (uint__t *)malloc(sizeof(uint__t)*(n));
	exId = (uint__t *)malloc(sizeof(uint__t)*(n));
 
	double sum, diag;
	real__t *b;
	b = (real__t *)malloc(sizeof(real__t)*(n));
  
	std::cout << " ======================== Initialization of ararys used to store the factorized matrix and LU ========================"<< std::endl;

	struct timeval t1_st, t1_end, t2_st, t2_end, t3_st, t3_end; long seconds, useconds;

	//initialization to 0
	// [tc] use memset here
	memset(Vm, 0, sizeof(real__t)*(n));
	memset(Va, 0, sizeof(real__t)*(n));
	memset(Vm_init, 0, sizeof(real__t)*(n));
	memset(Va_init, 0, sizeof(real__t)*(n));
	memset(deltaP, 0, sizeof(real__t)*(n));
	memset(deltaQ, 0, sizeof(real__t)*(n));
	memset(Q_PV, 0, sizeof(real__t)*(n));

	// =================================== Sort all input HeapAccum =================================
	gettimeofday(&t3_st, 0);
	gettimeofday(&t2_st, 0);
	// Input HeapAccums are assumed to be unsorted. Sort them before assigning values to
	// local arrays
	gVertex.sort_heap();
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gVertex HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
  
	gettimeofday(&t2_st, 0);  
	gMatrix_all.sort_heap();
 	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gMatrix_all HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
    
	// get the time for sorting all HeapAccums 
	gettimeofday(&t3_end, 0);
	seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	useconds = t3_end.tv_usec  - t3_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort all HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
	
	// ========================================= Convert Ap, Ap_pp, ep and the Vertex =========================================
	gettimeofday(&t2_st, 0); 
	int i_p, i_pp;
	int ii = 1; // counter for the array  
	// Assign value to the first position of the pointer array 
	ap[0] = 0;
	ap_pp[0] = 0;
	ep[0] = 0;
	for (i=0;i<gVertex.size();i++)
	{
		ap[ii] = gVertex.data_[i].Bp_p + ap[ii-1];
		//std::cout<< ii << ",ap,"<< ap[ii] << std::endl;
		ap_pp[ii] = gVertex.data_[i].Bpp_p + ap_pp[ii-1];
		ep[ii] = gVertex.data_[i].ep + ep[ii-1];
		ii++;
		Vm[i] = gVertex.data_[i].Vm;
		Va[i] = gVertex.data_[i].Va;
		Vm_init[i] = gVertex.data_[i].Vm;
		Va_init[i] = gVertex.data_[i].Va;
		Pn[i] = gVertex.data_[i].Pn;
		Qn[i] = gVertex.data_[i].Qn;
		btype[i] = gVertex.data_[i].bustype;
		exId[i] = gVertex.data_[i].key;
		qUp[i] = gVertex.data_[i].qUp;
		qLower[i] = gVertex.data_[i].qLower;
	}  
 	// ========================================= Convert Ybus (ei, eG and eB), B' (ei and Bp_x) and B" (ei and Bpp_x) =========================================
	i_p = 0;
	i_pp = 0;
	for (ii=0; ii<gMatrix_all.size(); ++ii) {
		ei[ii] = gMatrix_all.data_[ii].ei;
		eG[ii] = gMatrix_all.data_[ii].eG;
		eB[ii] = gMatrix_all.data_[ii].eB; 
		if(gMatrix_all.data_[ii].Bp_x != 0)
		{
		  ai[i_p] = gMatrix_all.data_[ii].ei;
		  ax[i_p] = gMatrix_all.data_[ii].Bp_x;
		  //std::cout<< i_p << ",ax,"<< ax[i_p] << ",ai,"<< ai[i_p] << std::endl;
		  i_p++;
		}
		if(gMatrix_all.data_[ii].Bpp_x != 0)
		{
		  ai_pp[i_pp] = gMatrix_all.data_[ii].ei;
		  ax_pp[i_pp] = gMatrix_all.data_[ii].Bpp_x;
		  ax_pp_mod[i_pp] = gMatrix_all.data_[ii].Bpp_x_mod;
		  // std::cout<< i_pp << ",ax_pp_mod,"<< ax_pp_mod[i_pp] << "Matrix_all.Bpp_x_mod" << gMatrix_all.data_[ii].Bpp_x_mod << std::endl;
		  i_pp++;
		}
	} 	
	// Done converting all input HeapAccums to arrays
  
	// Get the time to convert the data between containers and arrays
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to convert data to GRAPHLU time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");		
	
	// ----------------------------------------------------------------------------------------------------
	// 								Call GRAPHLU and Factorize B' Matrix
	// ----------------------------------------------------------------------------------------------------
	int error_p = 0, error_pp = 0;
	gettimeofday(&t2_st, 0);
	std::cout << "\n ======================== Start factorizing B' Matirx ======================== \n"<< std::endl;
	// Initialize the structure for GRAPHLU
	graphlu = (SGraphLU *)malloc(sizeof(SGraphLU));
	GraphLU_Initialize(graphlu);

	// Create Matrix
	GraphLU_CreateMatrix(graphlu, n, nnz, ax, ai, ap);
	// graphlu->cfgf[0] = 1.;

	// // Set control parameters
	//graphlu->cfgi[1] = 0; // 0 is no MC64 scaling
	// 					 // 1 is with MC64 scaling

	// Analyze (column/row ordering, scaling. For details please refer to GRAPHLU user's guide)
	GraphLU_Analyze(graphlu);
	printf("analysis time: %.8g\n", graphlu->stat[0]);
	

	// creates the task scheduler for parallel LU factorization. If you want to run 
	// parallel factorization or parallel re-factorization, it should be called after GraphLU Analyze.
	ret = GraphLU_CreateScheduler(graphlu);
	printf("time of creating scheduler: %.8g\n", graphlu->stat[4]);
	printf("suggestion: %s.\n", ret==0?"parallel":"sequential");
	//std::cout << "GraphLU MC64 scaling 4th:" << graphlu->cfgi[1] <<std::endl; 
	// This function creates threads for parallel computation. The second argument (thread)
	// specifies the number of threads, including the main thread. The last argument (check)
	// specifies whether to check the number of threads or not.
	if (!ret)
	{ // parallel factorization 
		GraphLU_CreateThreads(graphlu, 2, TRUE);
		printf("total cores: %d, threads created: %d\n", (int)(graphlu->stat[9]), (int)(graphlu->cfgi[5]));

		// This function binds threads to cores (unbind = FALSE) or unbinds threads from cores (unbind = TRUE).
		GraphLU_BindThreads(graphlu, FALSE);

		// Numerical LU factorization with partial pivoting, parallel
		error_p = GraphLU_Factorize_MT(graphlu);
		printf("factorization time: %.8g\n", graphlu->stat[1]);
		if (error_p < 0) //there is an error, print the code 
		{
			std::cout << "GraphLU_Factorize error code:" << error_p <<std::endl;  
		}
	}   
	else
	{  // Sequential factorization
		error_p = GraphLU_Factorize(graphlu);
		printf("Factorization time: %.8g\n", graphlu->stat[1]);
		if (error_p < 0) //there is an error, print the code 
		{
			std::cout << "GraphLU_Factorize error code:" << error_p <<std::endl; 
		}
	}
	
	// // debugging for B" refactorization
	// error_pp = GraphLU_ReFactorize(graphlu, ax_pp_mod);
			// printf("ReFactorization time: %.8g\n", graphlu->stat[2]);
			// if (error_pp < 0) //there is an error, print the code 
			// std::cout << "GraphLU_ReFactorize error code:" << error_pp <<std::endl; 
	// real__t *ldiag_pp;
	// ldiag_pp = graphlu->ldiag;
	// for(i = 0; i < n; ++i)
	// {
		// std::cout << "i " << i << "diagonal element  " << ldiag_pp[i] << std::endl;
	// }
	
	// //////////////////////////// degbugging ends ////////////////////////////////////
	
	real__t *lx, *ux; 
	uint__t *li, *ui; 
	size_t *lp, *up; 
	uint__t fctnnz; // number of nonzeros after the matrix is factorized
	size_t l_nnz, u_nnz; // number of nonzeros in L and U factors
	uint__t *rp, *cp, *cpi; //row (column) permutation, row (column) permutation inverse
	real__t *rows, *cols;


	lx = ux = NULL; 
	li = ui = NULL; 
	lp = up = NULL; 

	int row, col, k;
	int LUrow, LUcol;

	GraphLU_DumpLU(graphlu, &lx, &li, &lp, &ux, &ui, &up); 
     
    // Get the number of nonzeros in the factorized matrix
 	fctnnz = graphlu->lu_nnz;
 	l_nnz = graphlu->l_nnz;
 	u_nnz = graphlu->u_nnz;
 
 	std::cout << " ======================== Number of Total nonzeros after factorization is: "<< graphlu->lu_nnz << "========================"<<std::endl;
 	std::cout << " ======================== Number of nonzeros in L: "<< graphlu->l_nnz << "========================"<<std::endl;
 	std::cout << " ======================== Number of nonzeros in U: "<< graphlu->u_nnz << "========================"<<std::endl;
 	
    // get the permutation arrays, rp and cp may be different if the matrix is not symmetric
	rp = graphlu->row_perm;
	//rpi = graphlu->row_perm_inv;
 	cp = graphlu->col_perm;
	cpi = graphlu->col_perm_inv;


	rows = graphlu->row_scale; 
	cols = graphlu->col_scale_perm;
 	// print out and check
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	
	printf("\n\n============================================================================================== ");
	std::cout << "Time for GRAPHLU to factorize B' :: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");
	
	
	real__t *lx_pp, *ux_pp; 
	uint__t *li_pp, *ui_pp; 
	size_t *lp_pp, *up_pp; 
	uint__t fctnnz_pp; // number of nonzeros after the matrix is factorized
	size_t l_nnz_pp, u_nnz_pp; // number of nonzeros in L and U factors
	// get the permutation arrays, please refer to graphlu.h for details
	//uint__t *rp_pp, *cp_pp, *rpi_pp, *cpi_pp; //row (column) permutation, row (column) permutation inverse
	uint__t *rp_pp, *cpi_pp; //row (column) permutation, row (column) permutation inverse
	//real__t *ldiag_pp, *cscale_pp, *rows_pp, *cols_pp;
	real__t *rows_pp, *cols_pp;
	//int__t *pivot_pp, *pivot_inv_pp; 
	gettimeofday(&t1_st, 0);
	// ----------------------------------------------------------------------------------------------------
	// 								Call GRAPHLU and Factorize B" Matirx
	// ----------------------------------------------------------------------------------------------------
	if(error_p >= 0){

		// Initialize the structure for GRAPHLU
		int PVPQ = 1; // for the first iteration, needs to build the modified B" matrix, so set PVPV = 1, you can freely set it to other non-zerovalues.
		int iteration  = 0;
		int slack_bus_change = 0;
		for (iter=0; iter<maxiter; ++iter){
			
			if(PVPQ != 0) // Decide if PV-PQ conversion happened in the previous iteration, PVPQ = 0 indicates no PV-PQ conversion happened in the previous iteration, else happens.
			// if PV-PQ conversion happened in the previous iteration or the first iteration, modified B" matrix needs update and also a refactorization is required for the updated modified B" matrix
			{	
				std::cout << "\n ======================== Start developing modified B'' Matirx ======================== \n"<< std::endl;
				gettimeofday(&t2_st, 0);
				PVPQ = 0; // if PV-PQ conversion happens in the previous iteration, reset PVPQ to zero for this iteration's PV-PQ conversion check
				if (iter != 0)
				{
					for (i = 0; i < n; ++i) // update the modified B" matrix if PV-PQ conversion happens
					{
						//pv = -10;
						if (btype[i] == 10) // check if it is newly converted PQ bus
						{
							//std::cout << "row " << i << std::endl;
							// pv = i; // assign the row # of PV node to pv
							for (j = ap_pp[i]; j < ap_pp[i+1]; ++j)
							{
								if (ai[j] != i)  // if not diagonal element
								{
									ax_pp_mod[j] = ax_pp[j]; // assign specific element's value in B" matrix to the the corresponding element in the modified B"
									for (ii = ap_pp[ai[j]]; ii < ap_pp[ai[j]+1]; ++ii) // find corresponding column
									{
										if(ai[ii] == i) // find corresponding row
										{
											ax_pp_mod[ii] = ax_pp[ii]; // assign specific element's value in B" matrix to the the corresponding element in the modified B"
										}
									}
								}
							
							}
							btype[i] = 0;
						}		
					}
				}
				
				gettimeofday(&t2_end, 0);
				seconds = t2_end.tv_sec  - t2_st.tv_sec; 
				useconds = t2_end.tv_usec  - t2_st.tv_usec;
				printf("\n\n============================================================================================== ");
				std::cout << "Time to form modified B'':: " << (seconds*1000000 + useconds) << " us" << std::endl;
				printf("\n================================================================================================ "); 

			
				gettimeofday(&t2_st, 0);
				std::cout << "\n ======================== Start factorizing B'' Matirx ======================== \n"<< std::endl;			
				
				if (iter == 0)
				{					
					graphlu_pp = (SGraphLU *)malloc(sizeof(SGraphLU));
					GraphLU_Initialize(graphlu_pp);	
					GraphLU_CreateMatrix(graphlu_pp, n_pp, nnz_pp, ax_pp_mod, ai_pp, ap_pp);
					// graphlu_pp->cfgf[0] = 1.;
					// std::cout << "Iter" << iter << std::endl;	
					// graphlu_pp->flag[1] = true;
					// graphlu_pp->row_perm = graphlu->row_perm;
					// graphlu_pp->row_perm_inv = graphlu->row_perm_inv;
					// graphlu_pp->col_perm = graphlu->col_perm;
					// graphlu_pp->col_perm_inv = graphlu->col_perm_inv;
					// graphlu_pp->row_scale = graphlu->row_scale; 
					// graphlu_pp->col_scale_perm = graphlu->col_scale_perm;
					// permute_and_scale(graphlu_pp);
					// std::cout << "Iter" << iter << std::endl; 
					// Analyze (column/row ordering, scaling. For details please refer to GRAPHLU user's guide)
					 GraphLU_Analyze(graphlu_pp);
					 printf("analysis time: %.8g\n", graphlu_pp->stat[0]); 
				
			
					error_pp = GraphLU_Factorize(graphlu_pp);
					printf("Factorization time: %.8g\n", graphlu_pp->stat[1]);
					if (error_pp < 0) //there is an error, print the code
					{
						std::cout << "GraphLU_Factorize error code:" << error_pp <<std::endl;
					}			
				}
				else
				{
					error_pp = GraphLU_ReFactorize(graphlu_pp, ax_pp_mod);
					printf("ReFactorization time: %.8g\n", graphlu_pp->stat[1]);
					if (error_pp < 0) //there is an error, print the code 
					std::cout << "GraphLU_ReFactorize error code:" << error_pp <<std::endl; 
				}

				if(error_pp >= 0){
  
					lx_pp = ux_pp = NULL; 
					li_pp = ui_pp = NULL; 
					lp_pp = up_pp = NULL; 
				  
					GraphLU_DumpLU(graphlu_pp, &lx_pp, &li_pp, &lp_pp, &ux_pp, &ui_pp, &up_pp); 
				   
				   // Get the number of nonzeros in the factorized matrix
					fctnnz_pp = graphlu_pp->lu_nnz;
					l_nnz_pp = graphlu_pp->l_nnz;
					u_nnz_pp = graphlu_pp->u_nnz;
				 
					std::cout << " ======================== Number of Total nonzeros after factorization is: "<< graphlu_pp->lu_nnz << "========================"<<std::endl;
					std::cout << " ======================== Number of nonzeros in L: "<< graphlu_pp->l_nnz << "========================"<<std::endl;
					std::cout << " ======================== Number of nonzeros in U: "<< graphlu_pp->u_nnz << "========================"<<std::endl;
					
				  // get the permutation arrays, rp and cp may be different if the matrix is not symmetric
					rp_pp = graphlu_pp->row_perm;
					cpi_pp = graphlu_pp->col_perm_inv;
					rows_pp = graphlu_pp->row_scale; 
					cols_pp = graphlu_pp->col_scale_perm; 
				}
				else 
				{
					//B'' factorization failed
					result = "FAILED";
					printf("\n\n============================================================================================== ");
					std::cout << "B'' Factorization FAILED" << std::endl;
					printf("\n================================================================================================ ");
					break;
				}
				gettimeofday(&t2_end, 0);
				seconds=t2_end.tv_sec  - t2_st.tv_sec; 
				useconds = t2_end.tv_usec  - t2_st.tv_usec;
				
				printf("\n\n============================================================================================== ");
				std::cout << "Time for GRAPHLU to factorize B'':: " << (seconds*1000000 + useconds) << " us" << std::endl;
				printf("\n================================================================================================ ");
				for (i=0;i<gVertex.size();i++)
				{
					Vm[i] = Vm_init[i];
					Va[i] = Va_init[i];
				} 
				iteration = 0;
			}
			
			// Check if slack bus exists; If not, choose a PV bus to be a new slack bus;
			if (slack_bus_change == 1)
			{
				for (i = 0; i < n; ++i) // update the modified B" matrix if PV-PQ conversion happens
				{
					if(btype[i] == 2)
					{
						btype[i] = 3;
						break;
					}
				}
			}

			//double Bii = 0.0;
			//if(error_p >= 0 && error_pp >= 0){
			//continue to do the iterations only if there is no error from factorization
			printf("\n\n============================================================================================== ");
			std::cout << "\n Start iteratively updating deltaP and deltaQ " << std::endl;
			printf("\n================================================================================================\n ");
		  
			gettimeofday(&t2_st, 0);
	   
			//for (iter=0; iter<maxiter; ++iter){
			// update deltaP and deltaQ
			maxDeltaP=0;
			maxDeltaQ=0;
	  
			// std::cout << " ======================== Update deltaP and deltaQ ========================"<<std::endl;
			gettimeofday(&t3_st, 0);
			// Calculate deltaP and deltaQ to update Va
			for (i=0; i<n; ++i){
				deltaP[i] = 0;
				deltaQ[i] = 0;
	  
				if (btype[i]!=3){
					deltaP[i] = Pn[i];
					if (btype[i]<2)
						deltaQ[i] = Qn[i];
				}
	  
				// Calculate network injections
				// if (btype[i]!=3){
					for (j=ep[i]; j<ep[i+1]; ++j){
						p=ei[j];
						//if (p == i){
							//Bii = eB[j];
						//}
						if (iteration == 0){        
							//if this is the first iteraton, calculate the cosine and sine terms        
							cosine_array[j] = cos(Va[i]-Va[p]);
							sine_array[j] = sin(Va[i]-Va[p]);
						}
						
						// calculate active power mismatch of PV bus and PQ bus, and reactive power mismatch of PQ bus
						if (btype[i]!=3){
							deltaP[i] += - Vm[i]*Vm[p]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);        
							if (btype[i]<2) // calculate Q for PQ buses
							{
								deltaQ[i] += - Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]); 
							}
						}
						
						// calculate reactive power in each PV bus and slack bus
						if (btype[i] == 2 or btype[i] == 3) 
						{
							Q_PV[i] += Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]); // calculate Qcal for PV/slack bus and save it for future PVPQ conversion usage
						}
						
					}
				// }
				// Get max P and max Q
				if (abs(deltaP[i]) > maxDeltaP)
					maxDeltaP = abs(deltaP[i]);
				if (btype[i]<2)
				{
					if (abs(deltaQ[i]) > maxDeltaQ)
					{
						maxDeltaQ = abs(deltaQ[i]);
					}
				}

				deltaP[i] /= Vm[i];
				deltaQ[i] /= Vm[i];
			}			
  
			std::cout << "Iter" << iter << "Iteration" <<iteration  << " Updated for Va, maxDeltaP, " << maxDeltaP << ", maxDeltaQ, " << maxDeltaQ << std::endl;
		  
			gettimeofday(&t3_end, 0);
			seconds=t3_end.tv_sec  - t3_st.tv_sec; 
			useconds = t3_end.tv_usec  - t3_st.tv_usec;
			printf("\n\n==============================================================================================\n ");
			std::cout << "Time for updating deltaP and deltaQ (iteration):: " << iter << "Iteration" << iteration << " :" << (seconds*1000000 + useconds) << " us" << std::endl;
			printf("\n================================================================================================\n ");
		  
		  
		    if (maxDeltaP < max_change_ex && maxDeltaQ < max_change_ex)
			{
				for (i=0; i<n; ++i)
				{
					if (btype[i] == 2 or btype[i] == 3)  // PV convert to PQ if violating Q limits
					{
						//if(Q_PV[i] < qLower[i]*100)
						if(Q_PV[i] < qLower[i]) // PV convert to PQ if violating Q lower limit
						{
							if (btype[i] == 3){
								slack_bus_change = 1;
							}
							std::cout << "Lower Q violation bus " << exId[i] << ", Q_PV " << Q_PV[i] << std::endl;
							//Qn[i] = qLower[i]*1000;  // set Q to its lower limit
							Qn[i] = qLower[i];
							btype[i] = 10;  //convert it to PQ bus, mark its type as 10 to indicate this bus is converted from PV to PQ
							PVPQ +=1; // counting the number of PV to PQ conversion
						}
						
						//if(Q_PV[i] > qUp[i]*100)
						if(Q_PV[i] > qUp[i]) // PV convert to PQ if violating Q lower limit
						{
							if (btype[i] == 3){
								slack_bus_change = 1;
							}
							std::cout << "Upper Q violation bus " << exId[i] << ", Q_PV " << Q_PV[i] << std::endl;
							//Qn[i] = qUp[i]*1000; // set Q to its upper limit
							Qn[i] = qUp[i];
							btype[i] = 10; // convert it to PQ bus
							PVPQ +=1; // counting the number of PV to PQ conversion
							
						}
					}
				Q_PV[i] = 0;  // reset (PV bus reactive power) to zero since following forward/backward substitution need to use the deltaQ values for buses.
				
				}
			}
			
			std::cout << "PVPQ " << PVPQ << std::endl;
			if (PVPQ != 0) // Decide if there exists PVPQ conversion
			{
				continue;
			}
			else if ( maxDeltaP < max_change_ex && maxDeltaQ < max_change_ex)
			{
				result = "OK";
				break;
			}
			
			// Decide if the program converges	
			
			
			// for (i=0; i<n; ++i)
			// {
				// if (btype[i] == 2 && maxDeltaQ < 2*max_change_ex && maxDeltaP < 2*max_change_ex)  // PV convert to PQ if violating Q limits
				// {
					// //if(Q_PV[i] < qLower[i]*100)
					// if(Q_PV[i] < qLower[i]) // PV convert to PQ if violating Q lower limit
					// {
						// std::cout << "Lower Q violation bus " << exId[i] << ", Q_PV " << Q_PV[i] << std::endl;
						// //Qn[i] = qLower[i]*1000;  // set Q to its lower limit
						// Qn[i] = qLower[i];
						// btype[i] = 10;  //convert it to PQ bus, mark its type as 10 to indicate this bus is converted from PV to PQ
						// PVPQ +=1; // counting the number of PV to PQ conversion
					// }
					
					// //if(Q_PV[i] > qUp[i]*100)
					// if(Q_PV[i] > qUp[i]) // PV convert to PQ if violating Q lower limit
					// {
						// std::cout << "Upper Q violation bus " << exId[i] << ", Q_PV " << Q_PV[i] << std::endl;
						// //Qn[i] = qUp[i]*1000; // set Q to its upper limit
						// Qn[i] = qUp[i];
						// btype[i] = 10; // convert it to PQ bus
						// PVPQ +=1; // counting the number of PV to PQ conversion
						
					// }
				// }
				// Q_PV[i] = 0;  // reset (PV bus reactive power) to zero since following forward/backward substitution need to use the deltaQ values for buses.
				
			// }	  
		  
			// std::cout << "PVPQ " << PVPQ << std::endl;
			// if (PVPQ != 0) // Decide if there exists PVPQ conversion
			// {
				// continue;
			// }
			// else if ( maxDeltaP < max_change_ex && maxDeltaQ < max_change_ex)
			// {
				// result = "OK";
				// break;
			// }
      
			// Decide if the program converges	
  		
		

			//reset the max mismatch
			maxDeltaP = 0;
			maxDeltaQ = 0;
			  
			// solve for deltaVa
			// std::cout << " ======================== Solve for V angle ========================"<<std::endl;
			  
			// A*x = b
			// multiply deltaP with row scaling (rows) to get b
			for (i=0; i<n; ++i){
			b[i]=deltaP[rp[i]]*rows[rp[i]];
			}
		 
			// Full forward substitution 
			for (i=0; i<n; ++i)
			{
				sum=0.;
				diag=0.;
				for(p=lp[i]; p< lp[i+1]; ++p){
					if (i!=li[p]){ // if it is not diagnal element
						j=li[p];
						sum += lx[p] * b[j];
					}
					else{
						diag = lx[p];
					}
				}

				b[i] -= sum;
				b[i] /= diag;
			} 
		 
			// Full backward substitution 
			for (i=n-1; i>=0; --i)
			{
				sum=0.;
				for(p=up[i]; p< up[i+1]; ++p){
					if (i!=ui[p]){ // if it is not diagnal element
						sum += ux[p] * b[ui[p]]; 
					}
				}  
				b[i] -= sum; 
			}      
			// Update V angle (Va)
			for (i=0; i<n; ++i)
			{ 
			//Va[i] -= delta[i];  
			Va[i] -= b[cpi[i]]*cols[cpi[i]];      
			}
     
			// std::cout << " ======================== Update deltaP and deltaQ ========================"<<std::endl;
			gettimeofday(&t3_st, 0);
			// Calculate deltaP and deltaQ to update Vm
			for (i=0; i<n; ++i){
				deltaP[i] = 0;
				deltaQ[i] = 0;
	  
				if (btype[i]!=3){
					deltaP[i] = Pn[i];
					if (btype[i]<2)
						deltaQ[i] = Qn[i];
				}
  
				// Calculate network injections
				//if (btype[i]!=3){
					for (j=ep[i]; j<ep[i+1]; ++j){
						p=ei[j];
						// Va has been updated, update the cosine and sine terms 
						cosine_array[j] = cos(Va[i]-Va[p]);
						sine_array[j] = sin(Va[i]-Va[p]); 
						
						// calculate active power mismatch of PV bus and PQ bus, and reactive power mismatch of PQ bus
						if (btype[i]!=3){						
							deltaP[i] += - Vm[i]*Vm[p]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);        
							if (btype[i]<2) // calculate Q for PQ buses
							{
								deltaQ[i] += - Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]); 
							}
						}
						if (btype[i] == 2 or btype[i] == 3) // calculate reactive power in each PV bus and slack bus
						{
							Q_PV[i] += Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]); // calculate Qcal for PV bus and save it for future PVPQ conversion usage
						}
					}
					
				//}
  
				// Get max P and max Q
				if (abs(deltaP[i]) > maxDeltaP)
					maxDeltaP = abs(deltaP[i]);

				if (btype[i]<2)
				{
					if (abs(deltaQ[i]) > maxDeltaQ)
					{
						maxDeltaQ = abs(deltaQ[i]);
					}
					
				}
				  
				deltaP[i] /= Vm[i];
				deltaQ[i] /= Vm[i];
			}

      
  
			std::cout << "Iter" << iter << "Iteration" << iteration << " Updated for Vm, maxDeltaP, " << maxDeltaP << ", maxDeltaQ, " << maxDeltaQ << std::endl;
  
			gettimeofday(&t3_end, 0);
			seconds=t3_end.tv_sec  - t3_st.tv_sec; 
			useconds = t3_end.tv_usec  - t3_st.tv_usec;
			printf("\n\n==============================================================================================\n ");
			std::cout << "Time for updating deltaP and deltaQ (iteration):: " << iter << "Iteration" << iteration << " :" << (seconds*1000000 + useconds) << " us" << std::endl;
			printf("\n================================================================================================\n ");
       
	   
	   
	   
			if (maxDeltaP < max_change_ex && maxDeltaQ < max_change_ex)
			{
				for (i=0; i<n; ++i)
				{
					if (btype[i] == 2 or btype[i] == 3)  // PV convert to PQ if violating Q limits
					{
						//if(Q_PV[i] < qLower[i]*100)
						if(Q_PV[i] < qLower[i]) // PV convert to PQ if violating Q lower limit
						{
							if (btype[i] == 3){
								slack_bus_change = 1;
							}
							std::cout << "Lower Q violation bus " << exId[i] << ", Q_PV " << Q_PV[i] << std::endl;
							//Qn[i] = qLower[i]*1000;  // set Q to its lower limit
							Qn[i] = qLower[i];
							btype[i] = 10;  //convert it to PQ bus, mark its type as 10 to indicate this bus is converted from PV to PQ
							PVPQ +=1; // counting the number of PV to PQ conversion
						}
						
						//if(Q_PV[i] > qUp[i]*100)
						if(Q_PV[i] > qUp[i]) // PV convert to PQ if violating Q lower limit
						{
							if (btype[i] == 3){
								slack_bus_change = 1;
							}
							std::cout << "Upper Q violation bus " << exId[i] << ", Q_PV " << Q_PV[i] << std::endl;
							//Qn[i] = qUp[i]*1000; // set Q to its upper limit
							Qn[i] = qUp[i];
							btype[i] = 10; // convert it to PQ bus
							PVPQ +=1; // counting the number of PV to PQ conversion
							
						}
					}
				Q_PV[i] = 0;  // reset (PV bus reactive power) to zero since following forward/backward substitution need to use the deltaQ values for buses.
				
				}
			}
			
			std::cout << "PVPQ " << PVPQ << std::endl;
			if (PVPQ != 0) // Decide if there exists PVPQ conversion
			{
				continue;
			}
			else if ( maxDeltaP < max_change_ex && maxDeltaQ < max_change_ex)
			{
				result = "OK";
				break;
			}
			
			// Decide if the program converges	
	   
			// for (i=0; i<n; ++i)
			// {		
				// //} 
				// if (btype[i] == 2 && maxDeltaQ < 2*max_change_ex && maxDeltaP < 2*max_change_ex)  // PV convert to PQ if violating Q limits
				// {
					// //if(Q_PV[i] < qLower[i]*100) // PV convert to PQ if violating Q lower limit
					// if(Q_PV[i] < qLower[i]) // PV convert to PQ if violating Q lower limit
					// //if(Q_PV[i] < -1000) // PV convert to PQ if violating Q lower limit
					// {
						// std::cout << "Lower Q violation bus " << exId[i] << ", Q_PV " << Q_PV[i] << std::endl;
						// //Qn[i] = qLower[i]*1000; // set Q to its lower limit
						// Qn[i] = qLower[i];
						// btype[i] = 10; // convert it to PQ bus, mark its type as 10 to indicate this bus is converted from PV to PQ
						// PVPQ +=1; // counting the number of PV to PQ conversion
						
					// }
					// //if(Q_PV[i] > qUp[i]*100) // for sichuan case, PV convert to PQ if violating Q upper limit
					// if(Q_PV[i] > qUp[i]) // PV convert to PQ if violating Q upper limit
					// //if(Q_PV[i] > 1000) // PV convert to PQ if violating Q lower limit
					// {
						// std::cout << "Upper Q violation bus " << exId[i] << ", Q_PV " << Q_PV[i] << std::endl;
						// //Qn[i] = qUp[i]; // set Q to its upper limit
						// Qn[i] = qUp[i];
						// btype[i] = 10; // convert it to PQ bus
						// PVPQ +=1; // counting the number of PV to PQ conversion
						
					// }
				// }
				// Q_PV[i] = 0; // reset it (PV bus reactive power) to zero since following forward/backward substitution need to use the deltaQ values for buses.
			// }
		
			// std::cout << "PVPQ " << PVPQ << std::endl;
			// if (PVPQ != 0) // Decide if there exists PVPQ conversion
			// {
				// continue;
			// }
			// // Decide if the program converges	
			// else if ( maxDeltaP < max_change_ex && maxDeltaQ < max_change_ex)
			// {
				// result = "OK";
				// break;
			// }
		
		

			// solve for deltaVm
			// std::cout << " ======================== Solve for V magnitude ========================"<<std::endl;
		 
			// A*x = b
			// multiply deltaQ with row scaling (rows_pp) to get b
			for (i=0; i<n; ++i){
				b[i]=deltaQ[rp_pp[i]]*rows_pp[rp_pp[i]];
			}
		 
			// Full forward substitution 
			for (i=0; i<n; ++i)
			{
				//if (btype[rp_pp[i]]<2)  // PQ bus
				//{
					sum=0.;
					diag=0.;
					for(p=lp_pp[i]; p< lp_pp[i+1]; ++p){
						if (i!=li_pp[p]){ // if it is not diagnal element
							j=li_pp[p];
							sum += lx_pp[p] * b[j]; 
						}
						else{
							diag = lx_pp[p];
						}
					}

					b[i] -= sum; 
					b[i] /= diag;
			} 
		 
			// Full backward substitution 
			for (i=n-1; i>=0; --i)
			{
				//if (btype[rp_pp[i]]<2)  // PQ bus
				//{
					sum=0.;
					for(p=up_pp[i]; p< up_pp[i+1]; ++p)
					{
						if (i!=ui_pp[p])
						{ // if it is not diagnal element
							sum += ux_pp[p] * b[ui_pp[p]]; 
						}
					}
						b[i] -= sum;
			}      
	  
			// Update V magnitude (Vm)
			for (i=0; i<n; ++i)
			{
			//Vm[i] -= delta[i];
			Vm[i] -= b[cpi_pp[i]]*cols_pp[cpi_pp[i]];
			}
	  
			gettimeofday(&t2_end, 0);
			seconds=t2_end.tv_sec  - t2_st.tv_sec; 
			useconds = t2_end.tv_usec  - t2_st.tv_usec;
			
			printf("\n\n============================================================================================== ");
			std::cout << "Time for performing each power flow iteration: " << (seconds*1000000 + useconds) << " us" << std::endl;
			printf("\n================================================================================================ ");
			//free(graphlu_pp->nnz);
			//free(graphlu_pp->col_perm_inv);
			//GraphLU_Destroy(graphlu_pp);
			iteration++;
		}
	   
		gettimeofday(&t1_end, 0);
		seconds=t1_end.tv_sec  - t1_st.tv_sec; 
		useconds = t1_end.tv_usec  - t1_st.tv_usec;
		
		printf("\n\n============================================================================================== ");
		std::cout << "Time for performing power flow iteration: " << (seconds*1000000 + useconds) << " us" << std::endl;
		std::cout << "Number of iterations: " << iter << "Iteration" << iteration << std::endl;
		printf("\n================================================================================================ ");
	  
		// ----------------------------------------------------------------------------------------------------
		// 								Store the Result back to the array
		// ----------------------------------------------------------------------------------------------------
		// Get solved Vm and Va back into HeapAccum
		for (ii=0; ii<n; ++ii) {
			gVertex.data_[ii].Va = Va[ii]; // in radian
			gVertex.data_[ii].Vm = Vm[ii];
			//std::cout << "exId: " << exId[ii] << ", Q_PV: " << Q_PV[ii] << std::endl;
			//std::cout << "Final, " << ii+1 << ", Vm, " << Vm[ii]<< ", Va" << ii+1 << ", " << Va[ii]/pi_value*180<< std::endl;
			//std::cout << "\nFinal" << std::endl;
		}
	} else {
	//B' factorization failed
	  result = "FAILED";
	printf("\n\n============================================================================================== ");
	std::cout << "B' Factorization FAILED" << std::endl;
	printf("\n================================================================================================ ");  
	}
EXIT:
	//std::cout << "\n free1 " << std::endl;
	GraphLU_Destroy(graphlu);
	//std::cout << "\n free1_pp " << std::endl;
	//GraphLU_Destroy(graphlu_pp);
	//std::cout << "\n free2 " << std::endl;
	free(ax);
	free(ai);
	free(ap);
	free(ax_pp);
	free(ax_pp_mod);
	free(ai_pp);
	free(ap_pp);
	free(graphlu);
	//free(graphlu_pp);
	//std::cout << "\n free3 " << std::endl;
		
	free(eG);
	free(eB);
	free(ei);
	free(ep);
	//std::cout << "\n free4 " << std::endl;

	free(deltaP);
	free(deltaQ);
	free(qUp);
	free(qLower);
	free(Vm);
	free(Va);		
	free(Pn);
	free(Qn);
	free(btype);
	free(cosine_array);
	free(sine_array);
	free(b);
	//std::cout << "\n free5 " << std::endl;
  
 	/* free(lx);
	free(li);
	free(lp);
	free(ux);
	free(ui);
	free(up);  
 	free(lx_pp);
	free(li_pp);
	free(lp_pp);
 	free(ux_pp);
	free(ui_pp);
	free(up_pp); */ 
	//std::cout << "\n free6 " << std::endl;

	/* free(rp);
	free(cp);
	//free(rpi);
	free(cpi);
	free(rp_pp);
	//free(cp_pp);
	//free(rpi_pp);
	free(cpi_pp);
	free(rows);
	free(cols);
	free(rows_pp);
	free(cols_pp); */
	//std::cout << "\n free7 " << std::endl;

	std::cout << "\n\n----------------------------------------------------------------------------------------" << std::endl;
	std::cout << "\t\t End of Running fdpf_LU_PQPV_Conv_only C function!" << std::endl;
	std::cout << "\t\t ----------------------------------------------------------------------------------------\n\n"<< std::endl;
 
	return result;
}


//**********************************************************************************************************************************
// Created by: Chen Yuan, chen.yuan@geirina.net
// Date: 08/27/2017
// This code performs to assign continuous ID to nodes. 
// History: 
// 08/27/2017 [Chen Yuan] 
// **********************************************************************************************************************************

// //template <typename T_node_exId,
		  // //typename T_exId, typename T_counter>
// inline void Node_ID (ListAccum<int64_t>& gnode_exId, MapAccum<int64_t, int64_t>& gid_counter)
// {
	 // printf("\n\n----------------------------------------------------------------------------------------");
	 // printf("\n Start to run Node_ID C function");
	 // std::cout << "\n Node Size: " << gnode_exId.data_.size() << std::endl;
     // // ------------------------------------------------------------------------------------------------
	 // // 				Initialize variables and arrays
	 // // ------------------------------------------------------------------------------------------------
	 // // //uint__t *exId;
	 // long counter = 0;
	 // int i;
	// // string result = "FAILED";
	
	 // for (i=0; i<gnode_exId.data_.size(); ++i)
	 // {
		 // counter += 1;
		 // gid_counter.data_.insert(gnode_exId.get(i), counter);
	 // }
// }
	// // result = "Node_ID Assignment Finish";
	
// // EXIT:

	// // //free(exId);    
	
	// // printf("\n\n----------------------------------------------------------------------------------------");
	// // printf("\t\t End of Running Node_ID C function!");
	// // printf("\t\t ----------------------------------------------------------------------------------------\n\n");	
 
  // // return result;
// // }


inline void setCounterMap (MapAccum<int64_t, int64_t>& counterMap, ListAccum<int64_t>& nodeList) {
  for (int i=1; i <= nodeList.size(); i++) {
    counterMap.data_.insert({nodeList.get(i-1), i});
  }
  
  //for(auto it = counterMap.begin(); it != counterMap.end(); ++it){
	  //std::cout << "TOPOID, " << it->first << ", " << "exId, " << it->second << std::endl;
  //}
}

inline EDGE changeEdgeType (const EDGE &edge_old) {
  EDGE edge_new = edge_old;
  edge_new.eType = 1;
  return edge_new;
}


//**********************************************************************************************************************************
// Created by: Chen Yuan, chen.yuan@geirina.net
// Date: 10/24/2017
// This code performs part of state estimation Gain matrix formulation. This version assigns gain matrix off-diagonal elements from mapaccum to listaccum for later system gain matrix built-up
// History: 
// 10/24/2017 [Chen Yuan] 
// **********************************************************************************************************************************
template <typename key, typename value,
          typename T_P,
		  typename T_Q>
inline void Map2List (MapAccum<key, value>& gMap_P_1step, MapAccum<key, value>& gMap_Q_1step, MapAccum<key, value>& gMap_P_2step, MapAccum<key, value>& gMap_Q_2step, ListAccum<T_P>& Gip, ListAccum<T_Q>& Giq) {
	
	int i;
	Gip.data_.reserve(Gip.data_.size()+gMap_P_1step.size()+gMap_P_2step.size());
	Giq.data_.reserve(Giq.data_.size()+gMap_Q_1step.size()+gMap_Q_2step.size());
	
	// struct timeval t2_st, t2_end, t3_st, t3_end; 
	// long seconds, useconds;
	// gettimeofday(&t2_st, 0);
	// gettimeofday(&t3_st, 0);
	//for(i=0; i<gMap.data_.size(); i++){
	for (const auto &map1: gMap_P_1step) {
		//Gip.data_.push_back(T(map1.first, map1.second));
		Gip.data_.emplace_back(T_P(map1.first, map1.second));
	}
	// gettimeofday(&t3_end, 0);
	// seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	// useconds = t3_end.tv_usec  - t3_st.tv_usec;
	// printf("\n\n============================================================================================== ");
	// std::cout << "Time1:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	// printf("\n================================================================================================ "); 
	
	// gettimeofday(&t3_st, 0);
	for (const auto &map2: gMap_Q_1step) {
		//Giq.data_.push_back(T(map2.first, map2.second));
		Giq.data_.emplace_back(T_Q(map2.first, map2.second));
	}
	// gettimeofday(&t3_end, 0);
	// seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	// useconds = t3_end.tv_usec  - t3_st.tv_usec;
	// printf("\n\n============================================================================================== ");
	// std::cout << "Time2:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	// printf("\n================================================================================================ "); 
	
	// gettimeofday(&t3_st, 0);
	
	for (const auto &map3: gMap_P_2step) {
		//Gip.data_.push_back(T(map3.first, map3.second));
		Gip.data_.emplace_back(T_P(map3.first, map3.second));
	}
	// gettimeofday(&t3_end, 0);
	// seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	// useconds = t3_end.tv_usec  - t3_st.tv_usec;
	// printf("\n\n============================================================================================== ");
	// std::cout << "Time3:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	// printf("\n================================================================================================ "); 
	
	// gettimeofday(&t3_st, 0);
		
	for (const auto &map4: gMap_Q_2step) {
		//Giq.data_.push_back(T(map4.first, map4.second));
		Giq.data_.emplace_back(T_Q(map4.first, map4.second));
	}	
	// gettimeofday(&t3_end, 0);
	// seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	// useconds = t3_end.tv_usec  - t3_st.tv_usec;
	// printf("\n\n============================================================================================== ");
	// std::cout << "Time3:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	// printf("\n================================================================================================ "); 
	
	
	// gettimeofday(&t2_end, 0);
	// seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	// useconds = t2_end.tv_usec  - t2_st.tv_usec;
	// printf("\n\n============================================================================================== ");
	// std::cout << "Total Time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	// printf("\n================================================================================================ "); 
	//}

	
}


template <typename T>
inline void node_gain (ListAccum<T>& gain_neighbor_P, ListAccum<T>& gain_neighbor_Q, ListAccum<T>& Gip, ListAccum<T>& Giq){
	struct Gain_sort
	{
            inline bool operator() (const T& tuple1, const T& tuple2)
            {
                return (tuple1.index1 < tuple2.index1);
            }
	};
	
	std::sort(gain_neighbor_P.data_.begin(), gain_neighbor_P.data_.end(), Gain_sort());

	std::sort(gain_neighbor_Q.data_.begin(), gain_neighbor_Q.data_.end(), Gain_sort());

// Iteration throught gain_neighbor_P and merge the same 2-step neighbors
	for(int i=0; i<gain_neighbor_P.data_.size()-1; ++i){
		if (gain_neighbor_P.data_[i].index1 == gain_neighbor_P.data_[i+1].index1){
			gain_neighbor_P.data_[i+1].value1 = gain_neighbor_P.data_[i].value1 + gain_neighbor_P.data_[i+1].value1;
			gain_neighbor_P.data_.erase(gain_neighbor_P.data_.begin()+i);
		}	
	}  
	
	
	Gip.data_.reserve(Gip.data_.size() + gain_neighbor_P.data_.size());
	Gip.data_.insert (Gip.data_.end(), gain_neighbor_P.data_.begin(), gain_neighbor_P.data_.end());
	

	
	for(int i=0; i<gain_neighbor_Q.data_.size()-1; ++i){
		if (gain_neighbor_Q.data_[i].index1 == gain_neighbor_Q.data_[i+1].index1){
			gain_neighbor_Q.data_[i+1].value1 = gain_neighbor_Q.data_[i].value1 + gain_neighbor_Q.data_[i+1].value1;
			gain_neighbor_Q.data_.erase(gain_neighbor_Q.data_.begin()+i);
		}
			
	} 
	
	Giq.data_.reserve(Giq.data_.size() + gain_neighbor_Q.data_.size());
	Giq.data_.insert (Giq.data_.end(), gain_neighbor_Q.data_.begin(), gain_neighbor_Q.data_.end());
	
}




//**********************************************************************************************************************************
// Created by: Chen Yuan, chen.yuan@geirina.net
// Date: 08/01/2017
// This code performs state estimation Gain matrix factorization. This version performs LU factorization on the Gain_P and Gain_Q matrix
// After performing LU factorization, the resulting B' and B" LU factors are stored in ListAccums. 
// History: 
// 08/01/2017 [Chen Yuan] Added the GainMatrix_factorize C/C++ function to implement LU decomposition for matrices Gain_P and Gain_Q
// **********************************************************************************************************************************
template <typename T, 
          //typename T_GainQ, 
          typename T_GainP_L_matrix, typename GainP_L_matrix_comp,
          typename T_GainP_U_matrix, typename GainP_U_matrix_comp, 
          typename T_GainQ_L_matrix, typename GainQ_L_matrix_comp,
          typename T_GainQ_U_matrix, typename GainQ_U_matrix_comp,
          typename T_vertex_GainP, typename vertex_GainP_comp,
          typename T_vertex_GainQ, typename vertex_GainQ_comp>
inline string GainMatrix_factorize (int64_t &gslackbus, int64_t &gnnz_GainP, int64_t &gnnz_GainQ, ArrayAccum<SumAccum<int64_t>>& gGainP_p, ArrayAccum<SumAccum<int64_t>>& gGainQ_p, ArrayAccum<ListAccum<T>>& gGainP, ArrayAccum<ListAccum<T>>& gGainQ,
              HeapAccum<T_GainP_L_matrix, GainP_L_matrix_comp>& gMatrix_GainP_L, HeapAccum<T_GainP_U_matrix, GainP_U_matrix_comp>& gMatrix_GainP_U, HeapAccum<T_vertex_GainP, vertex_GainP_comp>& gVertex_GainP,
              HeapAccum<T_GainQ_L_matrix, GainQ_L_matrix_comp>& gMatrix_GainQ_L, HeapAccum<T_GainQ_U_matrix, GainQ_U_matrix_comp>& gMatrix_GainQ_U, HeapAccum<T_vertex_GainQ, vertex_GainQ_comp>& gVertex_GainQ) 
			  {

	// extern size_t g_si, g_sd, g_sp;
	printf("\n\n------------------------------------------------------------------------------------------- ");
	printf("\n Start Running GRAPHLU_fdpf_LU_factorize_only function!\n");
	std::cout << "GainP Number of rows:" << gGainP.data_.size() << ",\tNumber of nonzeros:" << gnnz_GainP << std::endl;
	std::cout << "GainQ Number of rows:" << gGainQ.data_.size() << ",\tNumber of nonzeros:" << gnnz_GainQ << std::endl;
	//std::cout << "Y bus Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gMatrix_all.data_.size() << std::endl;
	printf("-------------------------------------------------------------------------------------------- \n\n");	
	// ------------------------------------------------------------------------------------------------
	// 				Initialize variables and arrays
	// ------------------------------------------------------------------------------------------------
	
 
	// Initialize arrays and variables
	uint__t n_GainP, nnz_GainP, n_GainQ, nnz_GainQ, slackbus; //n_e, nnz_e;
	int ret, i, j; // p, iter;
	SGraphLU *graphlu_GainP, *graphlu_GainQ;
  
	string result = "FAILED";

	// Get the dimension, n, and the nnz of the matrix GainP and GainQ
	n_GainP = gGainP.data_.size();	nnz_GainP = gnnz_GainP; n_GainQ = gGainQ.data_.size(); nnz_GainQ = gnnz_GainQ;
	slackbus = gslackbus;
	//n_e=gVertex.data_.size(); 	nnz_e=gMatrix_all.data_.size(); // get the size of the Y bus matrix
	
	//// Convert vectors to GRAPHLU Data Structure (pointers)
	real__t *ax_GainP, *ax_GainQ; //*deltaP, *deltaQ, *Vm, *Va; *Pn, *Qn; *eG, *eB,
	uint__t *ai_GainP, *ai_GainQ; // *ei, *piv; *btype; 
	//uint__t *ap, *ap_pp, *ep;
    uint__t *ap_GainP, *ap_GainQ; 	
 	
	ax_GainP = (real__t *)malloc(sizeof(real__t)*(nnz_GainP)); // values in B' 
	ai_GainP = (uint__t *)malloc(sizeof(uint__t)*(nnz_GainP)); // column indices of B'
	ap_GainP = (uint__t *)malloc(sizeof(uint__t)*(n_GainP+1)); // initial row pointers
	// b = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b

	ax_GainQ = (real__t *)malloc(sizeof(real__t)*(nnz_GainQ)); // values in B"
	ai_GainQ = (uint__t *)malloc(sizeof(uint__t)*(nnz_GainQ)); // column indices of B"
	ap_GainQ = (uint__t *)malloc(sizeof(uint__t)*(n_GainQ+1)); // initial row pointers

  
	std::cout << " ======================== Initialization of ararys used to store the factorized matrix and LU ========================"<< std::endl;

	struct timeval t2_st, t2_end, t3_st, t3_end; long seconds, useconds;
	
	// =================================== Sort both GainP and GainQ =================================
	gettimeofday(&t3_st, 0);
	gettimeofday(&t2_st, 0);
	// Input ArrayAccums' ListAccums are assumed to be unsorted. Sort them before assigning values to local arrays
    struct Gain_sort
	{
            inline bool operator() (const T& tuple1, const T& tuple2)
            {
                return (tuple1.index1 < tuple2.index1);
            }
	};
	
    for(i=0;i<gGainP.data_.size(); ++i){
	   std::sort(gGainP.data_[i].begin(), gGainP.data_[i].end(), Gain_sort());
    }
	
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort GainP ArrayAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
	
	
	gettimeofday(&t2_st, 0); 
	
    // struct GainQ_sort
	// {
            // inline bool operator() (const T_GainQ& tuple3, const T_GainQ& tuple4)
            // {
                // return (tuple3.index < tuple4.index);
            // }
	// };
	
     for(i=0;i<gGainQ.data_.size(); ++i){
	    std::sort(gGainQ.data_[i].begin(), gGainQ.data_[i].end(), Gain_sort());
     }
	
 	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort GainQ ArrayAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
    
	// get the time for sorting all HeapAccums 
	gettimeofday(&t3_end, 0);
	seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	useconds = t3_end.tv_usec  - t3_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort both GainP and GainQ ArrayAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
	
	// ========================================= Convert ap_GainP, ap_GainQ =========================================
	gettimeofday(&t2_st, 0); 
	int ii = 1; // counter for the array  
	// Assign value to the first position of the pointer array 
	ap_GainP[0] = 0;
	ap_GainQ[0] = 0;
	//ep[0] = 0;
	for (i=0;i<gGainP_p.size();i++){
		ap_GainP[ii] = gGainP_p.data_[i] + ap_GainP[ii-1];
		//std::cout<< ii << ",ap_GainP,"<< ap_GainP[ii] <<  std::endl;
		ii++;
	}
	
	ii = 1;
	for (i=0;i<gGainQ_p.size();i++){
		ap_GainQ[ii] = gGainQ_p.data_[i] + ap_GainQ[ii-1];
		//std::cout<< ii << ",ap_GainQ,"<< ap_GainQ[ii] <<  std::endl;
		ii++;
	}
 
 	// ========================================= Convert GainP (ai_GainP, ax_GainP) and GainQ (ai_GainQ, ax_GainQ) =========================================
	int i_GainP = 0, i_GainQ = 0;
	for (i=0; i<gGainP.data_.size(); ++i) {
		for (ii=0; ii<gGainP.data_[i].size(); ++ii) {
	//for (ii=0; ii<gGainP.size(); ++ii) {
    //ei[ii] = gMatrix_all.data_[ii].ei;
    //eG[ii] = gMatrix_all.data_[ii].eG;
    //eB[ii] = gMatrix_all.data_[ii].eB; 
		//if(gMatrix_all.data_[ii].Bp_x != 0)
		//{
			if (gGainP.data_[i].get(ii).index1 < slackbus){
				ai_GainP[i_GainP] = gGainP.data_[i].get(ii).index1 - 1;
			}
			else if (gGainP.data_[i].get(ii).index1 > slackbus){
				ai_GainP[i_GainP] = gGainP.data_[i].get(ii).index1 - 2;
			}
			
			ax_GainP[i_GainP] = gGainP.data_[i].get(ii).value1;
      //std::cout<< i_GainP << ",ax,"<< ax_GainP[i_GainP] << ",ai,"<< ai_GainP[i_GainP] << std::endl;
			i_GainP++;
		}
	}
    //}
    //if(gMatrix_all.data_[ii].Bpp_x != 0)
    //{
	for (i=0; i<gGainQ.data_.size(); ++i) {
		for (ii=0; ii<gGainQ.data_[i].size(); ++ii) {
			ai_GainQ[i_GainQ] = gGainQ.data_[i].get(ii).index1 - 1;
			ax_GainQ[i_GainQ] = gGainQ.data_[i].get(ii).value1;
			//std::cout<< i_GainQ << ",ax,"<< ax_GainQ[i_GainQ] << ",ai,"<< ai_GainQ[i_GainQ] << std::endl;
			i_GainQ++;
		}
	} 	
	// Done converting all input HeapAccums to arrays
  
	// Get the time to convert the data between containers and arrays
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to convert data to GRAPHLU time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");		
	
	// ----------------------------------------------------------------------------------------------------
	// 								Call GRAPHLU and Factorize GainP Matirx
	// ----------------------------------------------------------------------------------------------------
	int error_p = 0, error_pp = 0;
	gettimeofday(&t2_st, 0);
	std::cout << "\n ======================== Start factorizing GainP Matirx ======================== \n"<< std::endl;
	// Initialize the structure for GRAPHLU
	graphlu_GainP = (SGraphLU *)malloc(sizeof(SGraphLU));
	GraphLU_Initialize(graphlu_GainP);
	
	// Create Matrix
	GraphLU_CreateMatrix(graphlu_GainP, n_GainP, nnz_GainP, ax_GainP, ai_GainP, ap_GainP);
	// graphlu->cfgf[0] = 1.;
	// // Set control parameters
	//graphlu->cfgi[1] = 0; // 0 is no MC64 scaling
	// 					 // 1 is with MC64 scaling

	// Analyze (column/row ordering, scaling. For details please refer to GRAPHLU user's guide)
	GraphLU_Analyze(graphlu_GainP);
	printf("analysis time: %.8g\n", graphlu_GainP->stat[0]);
	
	// creates the task scheduler for parallel LU factorization. If you want to run 
	// parallel factorization or parallel re-factorization, it should be called after GraphLU Analyze.
	ret = GraphLU_CreateScheduler(graphlu_GainP);
	printf("time of creating scheduler: %.8g\n", graphlu_GainP->stat[4]);
	printf("suggestion: %s.\n", ret==0?"parallel":"sequential");
	//std::cout << "GraphLU MC64 scaling 4th:" << graphlu->cfgi[1] <<std::endl; 
	// This function creates threads for parallel computation. The second argument (thread)
	// specifies the number of threads, including the main thread. The last argument (check)
	// specifies whether to check the number of threads or not.
  if (!ret){ // parallel factorization 
	  GraphLU_CreateThreads(graphlu_GainP, 2, TRUE);
	  printf("total cores: %d, threads created: %d\n", (int)(graphlu_GainP->stat[9]), (int)(graphlu_GainP->cfgi[5]));
	
	  // This function binds threads to cores (unbind = FALSE) or unbinds threads from cores (unbind = TRUE).
	  GraphLU_BindThreads(graphlu_GainP, FALSE);
	
	  // Numerical LU factorization with partial pivoting, parallel
	  error_p = GraphLU_Factorize_MT(graphlu_GainP);
	  printf("factorization time: %.8g\n", graphlu_GainP->stat[1]);
    if (error_p < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_p <<std::endl;  
  }   
  else{  // Sequential factorization
	  error_p = GraphLU_Factorize(graphlu_GainP);
	  printf("Factorization time: %.8g\n", graphlu_GainP->stat[1]);
    if (error_p < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_p <<std::endl; 
	}
 
  real__t *lx_GainP, *ux_GainP; 
  uint__t *li_GainP, *ui_GainP; 
  size_t *lp_GainP, *up_GainP; 
 	//uint__t fctnnz_GainP; // number of nonzeros after the matrix is factorized
	//size_t l_nnz_GainP, u_nnz_GainP; // number of nonzeros in L and U factors
  // get the permutation arrays, please refer to graphlu.h for details
  uint__t *rp_GainP, *cp_GainP, *rpi_GainP, *cpi_GainP; //row (column) permutation, row (column) permutation inverse
  real__t *rows_GainP, *cols_GainP; //*ldiag, *cscale,
  //int__t *pivot, *pivot_inv;
  // rpi = (real__t *)malloc(sizeof(real__t)*(n));
  
  lx_GainP = ux_GainP = NULL; 
  li_GainP = ui_GainP = NULL; 
  lp_GainP = up_GainP = NULL; 

  int row, col, k;
  int LUrow, LUcol;
    
  GraphLU_DumpLU(graphlu_GainP, &lx_GainP, &li_GainP, &lp_GainP, &ux_GainP, &ui_GainP, &up_GainP); 
     
   // Get the number of nonzeros in the factorized matrix
 	//fctnnz = graphlu->lu_nnz;
 	//l_nnz = graphlu->l_nnz;
 	//u_nnz = graphlu->u_nnz;
 
 	std::cout << " ======================== Number of Total nonzeros after factorization is: "<< graphlu_GainP->lu_nnz << "========================"<<std::endl;
 	std::cout << " ======================== Number of nonzeros in L: "<< graphlu_GainP->l_nnz << "========================"<<std::endl;
 	std::cout << " ======================== Number of nonzeros in U: "<< graphlu_GainP->u_nnz << "========================"<<std::endl;
 	
   // get the permutation arrays, rp and cp may be different if the matrix is not symmetric
  rp_GainP = graphlu_GainP->row_perm; // row permutation, permuted row # --> original row #  (original row)
  rpi_GainP = graphlu_GainP->row_perm_inv;  // row permutation inverse, original row # --> permuted row # (permuted row)
  cp_GainP = graphlu_GainP->col_perm;
  cpi_GainP = graphlu_GainP->col_perm_inv;

  //ldiag = graphlu->ldiag; // diagnal elements
  //cscale = graphlu->cscale; // sclaing factors
  rows_GainP = graphlu_GainP->row_scale; 
  cols_GainP = graphlu_GainP->col_scale_perm;
  //pivot = graphlu->pivot; 
  //pivot_inv = graphlu->pivot_inv; // pivoting array   
    
 	// print out and check
 	// for (i=0; i<n_GainP; ++i){ // check the permutation arrays
 	 	// std::cout << i << ",rp_GainP," << rp_GainP[i] << ",rpi_GainP," << rpi_GainP[i] << ",cp_GainP," << cp_GainP[i] << ",cpi_GainP," << cpi_GainP[i] << std::endl;
 	// } 
  
  //std::cout << " ======================== Get L Factor ========================"<<std::endl;
 	/*for (i=0; i<l_nnz; ++i){
     std::cout<< i << ",lx,"<< lx[i] << ",li,"<< li[i] << std::endl; 
  }
   
  for (i=0; i<n+1; ++i){
     std::cout<< i << ",lp,"<< lp[i] << std::endl;
  }
   
  //std::cout << "\n ======================== Get U Factor ========================"<<std::endl;
 	for (i=0; i<u_nnz; ++i){
     std::cout<< i << ",ux,"<< ux[i] << ",ui,"<< ui[i] << std::endl;
  }
   
  for (i=0; i<n+1; ++i){
 		std::cout<< i << ",up,"<< up[i] << std::endl;
 	}*/

 	// check arrays
  //for (i=0; i<n; ++i){
 	// 	std::cout<< i << ",\t diag:"<< ldiag[i] << ",\t cscale:"<< cscale[i] << ",\t rows:"<< rows[i] << ",\t cols:"<< cols[i] << std::endl;
  //}
  //for (i=0; i<n; ++i){
 	// 	std::cout<< i << ",rp," << rp[i]  << ",cpi," << cpi[i] << ",rows,"<< rows[i] << ",cols," << cols[i] <<std::endl;
  //}  
 
 	// // multiply U factor by cols (which is )
 	// for (i=0; i<n; ++i){
 	// 	for (j=up[i]; j<up[i+1]; ++j){
 	// 		p=rp[piv[ui[j]]]; // this may be rp!!!!!!!!!!!
 	// 		ux[j] = ux[j]/cols[p]; // include the scaling factor into L factor
 	// 	}
 	// }
 
 	// ================================= Get the pivoting vectors =================================
 	//for (i=0; i<n; ++i){
 	//	std::cout<< i << ",\t pivot:"<< pivot[i] << ",\t pivot_inv:"<< pivot_inv[i] << std::endl;
 	//}

  
    //get the factorized LU values and find the row and col before permutation
    for (i = 0; i < n_GainP; ++i){
      row = rp_GainP[i];
      LUrow = row+1;
      
      gVertex_GainP += T_vertex_GainP(i+1, lp_GainP[i+1]-lp_GainP[i], up_GainP[i+1]-up_GainP[i], rp_GainP[i], cpi_GainP[i], rows_GainP[i], cols_GainP[i]); 
      // process all non-zeros in L matrix on row i
      for (j = lp_GainP[i]; j < lp_GainP[i+1]; ++j){
        col = cp_GainP[li_GainP[j]];             
        LUcol = col+1;
        
        gMatrix_GainP_L += T_GainP_L_matrix((i+1)*100000+(li_GainP[j]+1), li_GainP[j], lx_GainP[j]); 
      } 
         
      // process all non-zeros in U matrix on row i
      for (j = up_GainP[i]; j < up_GainP[i+1]; ++j){
        col = cp_GainP[ui_GainP[j]];
        LUcol = col+1; 
        
        gMatrix_GainP_U += T_GainP_U_matrix((i+1)*100000+(ui_GainP[j]+1), ui_GainP[j], ux_GainP[j]);          
      }   
    }
    gVertex_GainP.sort_heap();
    gMatrix_GainP_L.sort_heap();
    gMatrix_GainP_U.sort_heap();

  
  //gVertex_p.sort_heap();
  //gMatrix_p_L.sort_heap();
  //gMatrix_p_U.sort_heap();
	// ----------------------------------------------------------------------------------------------------
	// 								Call GRAPHLU and Factorize GainQ Matirx
	// ----------------------------------------------------------------------------------------------------
	std::cout << "\n ======================== Start factorizing GainQ Matirx ======================== \n"<< std::endl;
	// Initialize the structure for GRAPHLU
	graphlu_GainQ = (SGraphLU *)malloc(sizeof(SGraphLU));
	GraphLU_Initialize(graphlu_GainQ);
 
	// Create Matrix
	GraphLU_CreateMatrix(graphlu_GainQ, n_GainQ, nnz_GainQ, ax_GainQ, ai_GainQ, ap_GainQ);
	// graphlu_pp->cfgf[0] = 1.;
	
	// // Set control parameters
	//graphlu_pp->cfgi[1] = 0; // 0 is no MC64 scaling
	// 					 // 1 is with MC64 scaling
 
	// Analyze (column/row ordering, scaling. For details please refer to GRAPHLU user's guide)
	GraphLU_Analyze(graphlu_GainQ);
	printf("analysis time: %.8g\n", graphlu_GainQ->stat[0]);
  
	// creates the task scheduler for parallel LU factorization. If you want to run 
	// parallel factorization or parallel re-factorization, it should be called after GraphLU Analyze.
	ret = GraphLU_CreateScheduler(graphlu_GainQ);
	printf("time of creating scheduler: %.8g\n", graphlu_GainQ->stat[4]);
	printf("suggestion: %s.\n", ret==0?"parallel":"sequential");
	
	// This function creates threads for parallel computation. The second argument (thread)
	// specifies the number of threads, including the main thread. The last argument (check)
	// specifies whether to check the number of threads or not.
  if (!ret){ // parallel factorization
	  GraphLU_CreateThreads(graphlu_GainQ, 2, TRUE);
	  printf("total cores: %d, threads created: %d\n", (int)(graphlu_GainQ->stat[9]), (int)(graphlu_GainQ->cfgi[5]));
	
	  // This function binds threads to cores (unbind = FALSE) or unbinds threads from cores (unbind = TRUE).
	  GraphLU_BindThreads(graphlu_GainQ, FALSE);
	
	  // Numerical LU factorization with partial pivoting, parallel
	  error_pp = GraphLU_Factorize_MT(graphlu_GainQ);
	  printf("factorization time: %.8g\n", graphlu_GainQ->stat[1]);
    if (error_pp < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_pp <<std::endl;      
  }
  else{  //Sequential factorization 
	  error_pp = GraphLU_Factorize(graphlu_GainQ);
    printf("Factorization time: %.8g\n", graphlu_GainQ->stat[1]);
    if (error_pp < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_pp <<std::endl; 
  }

  real__t *lx_GainQ, *ux_GainQ; 
  uint__t *li_GainQ, *ui_GainQ; 
  size_t *lp_GainQ, *up_GainQ; 
  //uint__t fctnnz_pp; // number of nonzeros after the matrix is factorized
  //size_t l_nnz_pp, u_nnz_pp; // number of nonzeros in L and U factors
  // get the permutation arrays, please refer to graphlu.h for details
  uint__t *rp_GainQ, *cp_GainQ, *rpi_GainQ, *cpi_GainQ; //row (column) permutation, row (column) permutation inverse
  real__t *rows_GainQ, *cols_GainQ;  //*ldiag_pp, *cscale_pp,
  //int__t *pivot_pp, *pivot_inv_pp; 
  
  lx_GainQ = ux_GainQ = NULL; 
  li_GainQ = ui_GainQ = NULL; 
  lp_GainQ = up_GainQ = NULL; 
  
  
  GraphLU_DumpLU(graphlu_GainQ, &lx_GainQ, &li_GainQ, &lp_GainQ, &ux_GainQ, &ui_GainQ, &up_GainQ); 
   
   // Get the number of nonzeros in the factorized matrix
 	//fctnnz_pp = graphlu_pp->lu_nnz;
 	//l_nnz_pp = graphlu_pp->l_nnz;
 	//u_nnz_pp = graphlu_pp->u_nnz;
 
 	std::cout << " ======================== Number of Total nonzeros after factorization is: "<< graphlu_GainQ->lu_nnz << "========================"<<std::endl;
 	std::cout << " ======================== Number of nonzeros in L: "<< graphlu_GainQ->l_nnz << "========================"<<std::endl;
 	std::cout << " ======================== Number of nonzeros in U: "<< graphlu_GainQ->u_nnz << "========================"<<std::endl;
 	
  // get the permutation arrays, rp and cp may be different if the matrix is not symmetric
  rp_GainQ = graphlu_GainQ->row_perm;
  rpi_GainQ = graphlu_GainQ->row_perm_inv;
  cp_GainQ = graphlu_GainQ->col_perm;
  cpi_GainQ = graphlu_GainQ->col_perm_inv;
  
  //ldiag_pp = graphlu_pp->ldiag; // diagnal elements
  //cscale_pp = graphlu_pp->cscale; // sclaing factors
  rows_GainQ = graphlu_GainQ->row_scale; 
  cols_GainQ = graphlu_GainQ->col_scale_perm;
  //pivot_pp = graphlu_pp->pivot; 
  //pivot_inv_pp = graphlu_pp->pivot_inv; // pivoting array   
    
 	// print out and check
 	// for (i=0; i<n_GainQ; ++i){ // check the permutation arrays
 	 	// std::cout << "rp_GainQ," << rp_GainQ[i] << ",rpi_GainQ," << rpi_GainQ[i] << ",cp_GainQ,"<< cp_GainQ[i] << ",cpi_GainQ," << cpi_GainQ[i] << std::endl;
 	// } 
  
  //std::cout << " ======================== Get L Factor ========================"<<std::endl;
 	/*for (i=0; i<l_nnz_pp; ++i){
    std::cout<< i << ",lx_pp,"<< lx_pp[i] << ",li_pp,"<< li_pp[i] << std::endl; 
  }
   
  for (i=0; i<n+1; ++i){
    std::cout<< i << ",lp_pp,"<< lp_pp[i] << std::endl;
  }
   
   //std::cout << "\n ======================== Get U Factor ========================"<<std::endl;
 	for (i=0; i<u_nnz_pp; ++i){
    std::cout<< i << ",ux_pp,"<< ux_pp[i] << ",ui_pp,"<< ui_pp[i] << std::endl;
  }
   
 	for (i=0; i<n+1; ++i){
 		std::cout<< i << ",up_pp,"<< up_pp[i] << std::endl;
 	}*/
 
 	// check arrays
  //for (i=0; i<n; ++i){
 	// 	std::cout<< i << ",\t diag_pp:"<< ldiag_pp[i] << ",\t cscale_pp:"<< cscale_pp[i] << ",\t rows_pp:"<< rows_pp[i] << ",\t cols_pp:"<< cols_pp[i] << std::endl;
  //}
  //for (i=0; i<n; ++i){
 	// 	std::cout<< i << ",rp_pp," << rp_pp[i]  << ",cpi_pp," << cpi_pp[i] << ",rows_pp,"<< rows_pp[i] << ",cols_pp," << cols_pp[i] <<std::endl;
  //}   
 
 	// // multiply U factor by cols (which is )
 	// for (i=0; i<n; ++i){
 	// 	for (j=up[i]; j<up[i+1]; ++j){
 	// 		p=rp[piv[ui[j]]]; // this may be rp!!!!!!!!!!!
 	// 		ux[j] = ux[j]/cols[p]; // include the scaling factor into L factor
 	// 	}
 	// }
 
 	// ================================= Get the pivoting vectors =================================
 	//for (i=0; i<n; ++i){
 	//	std::cout<< i << ",\t pivot_pp:"<< pivot_pp[i] << ",\t pivot_inv_pp:"<< pivot_inv_pp[i] << std::endl;
 	//}
  
      	
    //get the factorized LU values and find the row and col before permutation
    for (i = 0; i < n_GainQ; ++i){
      row = rp_GainQ[i];
      LUrow = row+1;
      
      gVertex_GainQ += T_vertex_GainQ(i+1, lp_GainQ[i+1]-lp_GainQ[i], up_GainQ[i+1]-up_GainQ[i], rp_GainQ[i], cpi_GainQ[i], rows_GainQ[i], cols_GainQ[i]); 
      // process all non-zeros in L matrix on row i
      for (j = lp_GainQ[i]; j < lp_GainQ[i+1]; ++j){
        col = cp_GainQ[li_GainQ[j]];
        LUcol = col+1;
        
        gMatrix_GainQ_L += T_GainQ_L_matrix((i+1)*100000+(li_GainQ[j]+1), li_GainQ[j], lx_GainQ[j]); 
      } 
      
      // process all non-zeros in U matrix on row i
      for (j = up_GainQ[i]; j < up_GainQ[i+1]; ++j){
        col = cp_GainQ[ui_GainQ[j]];
        LUcol = col+1;
        
        gMatrix_GainQ_U += T_GainQ_U_matrix((i+1)*100000+(ui_GainQ[j]+1), ui_GainQ[j], ux_GainQ[j]);
      }    
    }
    gVertex_GainQ.sort_heap();
    gMatrix_GainQ_L.sort_heap();
    gMatrix_GainQ_U.sort_heap();          

  
  // Get the time for factorizing B' and B"
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	
	printf("\n\n============================================================================================== ");
	std::cout << "Time for GRAPHLU to factorize GainP and GainQ:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");
  
  if(error_p >= 0 && error_pp >= 0){
      result = "Factorization Completed";
      printf("\n\n============================================================================================== ");
  	  std::cout << "Factorization Completed" << std::endl;
  	  printf("\n================================================================================================ "); 
    //continue to do the iterations only if there is no error from factorization
//  	printf("\n\n============================================================================================== ");
//  	std::cout << "\n Start iteratively updating deltaP and deltaQ " << std::endl;
//  	printf("\n================================================================================================\n ");
//  

//  	// ----------------------------------------------------------------------------------------------------
//  	// 								Store the Result back to the array
//  	// ----------------------------------------------------------------------------------------------------
//  	// Get solved Vm and Va back into HeapAccum
  	//for (ii=0; ii<n; ++ii) {
        //gVertex.data_[ii].Va = Va[ii]/pi_value*180;
        //gVertex.data_[ii].Vm = Vm[ii];
        //gVertex.data_[ii].rpi = rpi[ii];
		//gVertex.data_[ii].rpi_pp = rpi_pp[ii];
        //std::cout << "Final, " << ii+1 << ", Vm, " << Vm[ii]<< ", Va" << ii+1 << ", " << Va[ii]/pi_value*180<< std::endl;
  	//}

  } else {
    //factorization failed
 	  result = "Factorization FAILED";
    printf("\n\n============================================================================================== ");
  	std::cout << "Factorization FAILED" << std::endl;
  	printf("\n================================================================================================ ");  
  }
EXIT:
	// std::cout << "\n free1 " << std::endl;
	GraphLU_Destroy(graphlu_GainP);
	GraphLU_Destroy(graphlu_GainQ);
	// std::cout << "\n free2 " << std::endl;
	free(ax_GainP);
	free(ai_GainP);
	free(ap_GainP);
	free(ax_GainQ);
	free(ai_GainQ);
	free(ap_GainQ);
	free(graphlu_GainP);
	free(graphlu_GainQ);
	// std::cout << "\n free3 " << std::endl;
		
	//free(eG);
	//free(eB);
	//free(ei);
	//free(ep);

	//free(deltaP);
	//free(deltaQ);
	//free(Vm);
	//free(Va);		
	//free(Pn);
	//free(Qn);	
  
 	free(lx_GainP);
	free(li_GainP);
	free(lp_GainP);
    free(ux_GainP);
	free(ui_GainP);
	free(up_GainP);  
 	free(lx_GainQ);
	free(li_GainQ);
	free(lp_GainQ);
 	free(ux_GainQ);
	free(ui_GainQ);
	free(up_GainQ);    
	
  printf("\n\n----------------------------------------------------------------------------------------");
	printf("\t\t End of Running GainMatrix_factorize C function!");
	printf("\t\t ----------------------------------------------------------------------------------------\n\n");	
 
  return result;
}


template <typename T>
inline void check_symetric(ListAccum<T>& gGainP, ListAccum<T>& gGainQ, int64_t& count_P, int64_t& count_Q)
{
	//int count_P = 0;
	//int count_Q = 0;
	int i = 0;
	struct Gain_sort
	{
            inline bool operator() (const T& tuple1, const T& tuple2)
            {
                return (tuple1.value1 <= tuple2.value1);
            }
	};
	
    for(i=0;i<gGainP.data_.size(); ++i){
	   std::sort(gGainP.data_.begin(), gGainP.data_.end(), Gain_sort());
    }
	
	for(i=0;i<gGainQ.data_.size(); ++i){
	   std::sort(gGainQ.data_.begin(), gGainQ.data_.end(), Gain_sort());
    }
	
	for(i=0;i<gGainP.data_.size(); ++i){
		if (gGainP.data_[i].value1 != gGainP.data_[i+1].value1){
			count_P = count_P + 1;
			++i;
		}
	}
	
	for(i=0;i<gGainQ.data_.size(); ++i){
		if (gGainQ.data_[i].value1 != gGainQ.data_[i+1].value1){
			count_Q = count_Q + 1;
			++i;
		}
	}
	
	std::cout << "count_P: " << count_P << std::endl;
	std::cout << "count_Q: " << count_Q << std::endl;
}


//**********************************************************************************************************************************
// Created by: Chen Yuan, chen.yuan@geirina.net
// Date: 08/01/2017
// This code performs state estimation Gain matrix factorization. This version performs LU factorization on the Gain_P and Gain_Q matrix
// After performing LU factorization, the resulting B' and B" LU factors are stored in ListAccums. 
// History: 
// 08/01/2017 [Chen Yuan] Added the GainMatrix_factorize C/C++ function to implement LU decomposition for matrices Gain_P and Gain_Q
// **********************************************************************************************************************************
template <typename T> 
          //typename T_GainQ, 
          //typename T_GainP_L_matrix, typename GainP_L_matrix_comp,
          //typename T_GainP_U_matrix, typename GainP_U_matrix_comp, 
          //typename T_GainQ_L_matrix, typename GainQ_L_matrix_comp,
          //typename T_GainQ_U_matrix, typename GainQ_U_matrix_comp,
          //typename T_vertex_GainP, typename vertex_GainP_comp,
          //typename T_vertex_GainQ, typename vertex_GainQ_comp>
inline string GainMatrix_factorize_static (int64_t &gslackbus, int64_t &gnnz_GainP, int64_t &gnnz_GainQ, ArrayAccum<SumAccum<int64_t>>& gGainP_p, ArrayAccum<SumAccum<int64_t>>& gGainQ_p, ArrayAccum<ListAccum<T>>& gGainP, ArrayAccum<ListAccum<T>>& gGainQ)
              //HeapAccum<T_GainP_L_matrix, GainP_L_matrix_comp>& gMatrix_GainP_L, HeapAccum<T_GainP_U_matrix, GainP_U_matrix_comp>& gMatrix_GainP_U, HeapAccum<T_vertex_GainP, vertex_GainP_comp>& gVertex_GainP,
              //HeapAccum<T_GainQ_L_matrix, GainQ_L_matrix_comp>& gMatrix_GainQ_L, HeapAccum<T_GainQ_U_matrix, GainQ_U_matrix_comp>& gMatrix_GainQ_U, HeapAccum<T_vertex_GainQ, vertex_GainQ_comp>& gVertex_GainQ) 
			  {

	// extern size_t g_si, g_sd, g_sp;
	printf("\n\n------------------------------------------------------------------------------------------- ");
	printf("\n Start Running GainMatrix_factorize_static function!\n");
	std::cout << "GainP Number of rows:" << gGainP.data_.size() << ",\tNumber of nonzeros:" << gnnz_GainP << std::endl;
	std::cout << "GainQ Number of rows:" << gGainQ.data_.size() << ",\tNumber of nonzeros:" << gnnz_GainQ << std::endl;
	//std::cout << "Y bus Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gMatrix_all.data_.size() <<------------------------------------------------------------ \n\n");	
	// ------------------------------------------------------------------------------------------------
	// 				Initialize variables and arrays
	// ------------------------------------------------------------------------------------------------
	
 
	// Initialize arrays and variables
	uint__t n_GainP, nnz_GainP, n_GainQ, nnz_GainQ, slackbus; //n_e, nnz_e;
	int ret, i, j; // p, iter;
	SGraphLU *graphlu_GainP, *graphlu_GainQ;
  
	string result = "FAILED";

	// Get the dimension, n, and the nnz of the matrix GainP and GainQ
	n_GainP = gGainP.data_.size();	nnz_GainP = gnnz_GainP; n_GainQ = gGainQ.data_.size(); nnz_GainQ = gnnz_GainQ;
	slackbus = gslackbus;
	//n_e=gVertex.data_.size(); 	nnz_e=gMatrix_all.data_.size(); // get the size of the Y bus matrix
	
	std::cout << "GainP size: " << n_GainP << std::endl;
	std::cout << "GainQ size: " << n_GainQ << std::endl;
	
	//// Convert vectors to GRAPHLU Data Structure (pointers)
	real__t *ax_GainP, *ax_GainQ; //*deltaP, *deltaQ, *Vm, *Va; *Pn, *Qn; *eG, *eB,
	uint__t *ai_GainP, *ai_GainQ; // *ei, *piv; *btype; 
	//uint__t *ap, *ap_pp, *ep;
    uint__t *ap_GainP, *ap_GainQ; 	
 	
	ax_GainP = (real__t *)malloc(sizeof(real__t)*(nnz_GainP)); // values in B' 
	ai_GainP = (uint__t *)malloc(sizeof(uint__t)*(nnz_GainP)); // column indices of B'
	ap_GainP = (uint__t *)malloc(sizeof(uint__t)*(n_GainP+1)); // initial row pointers
	// b = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b

	ax_GainQ = (real__t *)malloc(sizeof(real__t)*(nnz_GainQ)); // values in B"
	ai_GainQ = (uint__t *)malloc(sizeof(uint__t)*(nnz_GainQ)); // column indices of B"
	ap_GainQ = (uint__t *)malloc(sizeof(uint__t)*(n_GainQ+1)); // initial row pointers

  
	std::cout << " ======================== Initialization of ararys used to store the factorized matrix and LU ========================"<< std::endl;

	struct timeval t2_st, t2_end, t3_st, t3_end; long seconds, useconds;
	
	// =================================== Sort both GainP and GainQ =================================
	gettimeofday(&t3_st, 0);
	gettimeofday(&t2_st, 0);
	// Input ArrayAccums' ListAccums are assumed to be unsorted. Sort them before assigning values to local arrays
    struct Gain_sort
	{
            inline bool operator() (const T& tuple1, const T& tuple2)
            {
                return (tuple1.index1 < tuple2.index1);
            }
	};
	
    for(i=0;i<gGainP.data_.size(); ++i){
	   std::sort(gGainP.data_[i].begin(), gGainP.data_[i].end(), Gain_sort());
    }
	
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort GainP ArrayAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
	
	
	gettimeofday(&t2_st, 0); 
	
    // struct GainQ_sort
	// {
            // inline bool operator() (const T_GainQ& tuple3, const T_GainQ& tuple4)
            // {
                // return (tuple3.index < tuple4.index);
            // }
	// };
	
     for(i=0;i<gGainQ.data_.size(); ++i){
	    std::sort(gGainQ.data_[i].begin(), gGainQ.data_[i].end(), Gain_sort());
     }
	
 	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort GainQ ArrayAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
    
	// get the time for sorting all HeapAccums 
	gettimeofday(&t3_end, 0);
	seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	useconds = t3_end.tv_usec  - t3_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort both GainP and GainQ ArrayAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
	
	// ========================================= Convert ap_GainP, ap_GainQ =========================================
	gettimeofday(&t2_st, 0); 
	int ii = 1; // counter for the array  
	// Assign value to the first position of the pointer array 
	ap_GainP[0] = 0;
	ap_GainQ[0] = 0;
	//ep[0] = 0;
	for (i=0;i<gGainP_p.size();i++){
		ap_GainP[ii] = gGainP_p.data_[i] + ap_GainP[ii-1];
		//std::cout<< ii << ",ap_GainP,"<< ap_GainP[ii] <<  std::endl;
		ii++;
	}
	
	ii = 1;
	for (i=0;i<gGainQ_p.size();i++){
		ap_GainQ[ii] = gGainQ_p.data_[i] + ap_GainQ[ii-1];
		//std::cout<< ii << ",ap_GainQ,"<< ap_GainQ[ii] <<  std::endl;
		ii++;
	}
 
 	// ========================================= Convert GainP (ai_GainP, ax_GainP) and GainQ (ai_GainQ, ax_GainQ) =========================================
	int i_GainP = 0, i_GainQ = 0;
	for (i=0; i<gGainP.data_.size(); ++i) {
		for (ii=0; ii<gGainP.data_[i].size(); ++ii) {
	//for (ii=0; ii<gGainP.size(); ++ii) {
    //ei[ii] = gMatrix_all.data_[ii].ei;
    //eG[ii] = gMatrix_all.data_[ii].eG;
    //eB[ii] = gMatrix_all.data_[ii].eB; 
		//if(gMatrix_all.data_[ii].Bp_x != 0)
		//{
			if (gGainP.data_[i].get(ii).index1 < slackbus){
				ai_GainP[i_GainP] = gGainP.data_[i].get(ii).index1 - 1;
			}
			else if (gGainP.data_[i].get(ii).index1 > slackbus){
				ai_GainP[i_GainP] = gGainP.data_[i].get(ii).index1 - 2;
			}
			ax_GainP[i_GainP] = gGainP.data_[i].get(ii).value1;
      //std::cout<< i_GainP << ",ax,"<< ax_GainP[i_GainP] << ",ai,"<< ai_GainP[i_GainP] << std::endl;
			i_GainP++;
		}
	}
    //}
    //if(gMatrix_all.data_[ii].Bpp_x != 0)
    //{
	for (i=0; i<gGainQ.data_.size(); ++i) {
		for (ii=0; ii<gGainQ.data_[i].size(); ++ii) {
			ai_GainQ[i_GainQ] = gGainQ.data_[i].get(ii).index1 - 1;
			ax_GainQ[i_GainQ] = gGainQ.data_[i].get(ii).value1;
			//std::cout<< i_GainQ << ",ax,"<< ax_GainQ[i_GainQ] << ",ai,"<< ai_GainQ[i_GainQ] << std::endl;
			i_GainQ++;
		}
	} 	
	// Done converting all input HeapAccums to arrays
  
	// Get the time to convert the data between containers and arrays
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to convert data to GRAPHLU time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");		
	
	

	
// ------------------------------------------ [Debug] print out gain matrix and check --------------------------------------------
// -------------------------------------------------------------------------------------------------------------------

	//// Convert vectors to GRAPHLU Data Structure (pointers)
	// real__t *GainP, *GainQ; //*deltaP, *deltaQ, *Vm, *Va; *Pn, *Qn; *eG, *eB,
	
	// GainP = (real__t *)malloc(sizeof(real__t)*(n_GainP*n_GainP)); 
	// GainQ = (real__t *)malloc(sizeof(real__t)*(n_GainQ*n_GainQ));

	// int temp_ap;
	// for (i=0; i<n_GainP; ){
		// temp_ap = ap_GainP[i];
		// //std::cout << i << ", ap: " << ap_GainP[i] << std::endl;
		// for(ii=0; ii<n_GainP; ) {
			// if (ii == ai_GainP[temp_ap]) {
				// GainP[i*n_GainP + ii] = ax_GainP[temp_ap];
				// temp_ap++;
			// } 
			// else {
				// GainP[i*n_GainP + ii] = 0;
			// }
			// std::cout << GainP[i*n_GainP + ii] << ",";
			// ii++;
		// }
		// std::cout << "" << std::endl;
		// i++;
	// }
	
	
	
	// ----------------------------------------------------------------------------------------------------
	// 								Call GRAPHLU and Factorize GainP Matirx
	// ----------------------------------------------------------------------------------------------------
	int error_p = 0, error_pp = 0;
	gettimeofday(&t2_st, 0);
	std::cout << "\n ======================== Start factorizing GainP Matirx ======================== \n"<< std::endl;
	// Initialize the structure for GRAPHLU
	graphlu_GainP = (SGraphLU *)malloc(sizeof(SGraphLU));
	ret = GraphLU_Initialize(graphlu_GainP);
	std::cout<<"Initialize: " << ret << std::endl; 
	
	// Create Matrix
	ret = GraphLU_CreateMatrix(graphlu_GainP, n_GainP, nnz_GainP, ax_GainP, ai_GainP, ap_GainP);
	// graphlu->cfgf[0] = 1.;
	// // Set control parameters
	//graphlu->cfgi[1] = 0; // 0 is no MC64 scaling
	// 					 // 1 is with MC64 scaling
	std::cout<<"CreateMatrix: " << ret << std::endl; 

	// Analyze (column/row ordering, scaling. For details please refer to GRAPHLU user's guide)
	GraphLU_Analyze(graphlu_GainP);
	printf("analysis time: %.8g\n", graphlu_GainP->stat[0]);
	
	// creates the task scheduler for parallel LU factorization. If you want to run 
	// parallel factorization or parallel re-factorization, it should be called after GraphLU Analyze.
	ret = GraphLU_CreateScheduler(graphlu_GainP);
	printf("time of creating scheduler: %.8g\n", graphlu_GainP->stat[4]);
	printf("suggestion: %s.\n", ret==0?"parallel":"sequential");
	//std::cout << "GraphLU MC64 scaling 4th:" << graphlu->cfgi[1] <<std::endl; 
	// This function creates threads for parallel computation. The second argument (thread)
	// specifies the number of threads, including the main thread. The last argument (check)
	// specifies whether to check the number of threads or not.
  if (!ret){ // parallel factorization 
	  GraphLU_CreateThreads(graphlu_GainP, 2, TRUE);
	  printf("total cores: %d, threads created: %d\n", (int)(graphlu_GainP->stat[9]), (int)(graphlu_GainP->cfgi[5]));
	
	  // This function binds threads to cores (unbind = FALSE) or unbinds threads from cores (unbind = TRUE).
	  GraphLU_BindThreads(graphlu_GainP, FALSE);
	
	  // Numerical LU factorization with partial pivoting, parallel
	  error_p = GraphLU_Factorize_MT(graphlu_GainP);
	  printf("factorization time: %.8g\n", graphlu_GainP->stat[1]);
    if (error_p < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_p <<std::endl;  
  }   
  else{  // Sequential factorization
	  error_p = GraphLU_Factorize(graphlu_GainP);
	  printf("Factorization time: %.8g\n", graphlu_GainP->stat[1]);
    if (error_p < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_p <<std::endl; 
	}
 
  real__t *lx_GainP, *ux_GainP; 
  uint__t *li_GainP, *ui_GainP; 
  size_t *lp_GainP, *up_GainP; 
  
 	//uint__t fctnnz_GainP; // number of nonzeros after the matrix is factorized
	//size_t l_nnz_GainP, u_nnz_GainP; // number of nonzeros in L and U factors
  // get the permutation arrays, please refer to graphlu.h for details
  uint__t *rp_GainP, *cp_GainP, *rpi_GainP, *cpi_GainP, nnz_LGainP, nnz_UGainP; //row (column) permutation, row (column) permutation inverse
  real__t *rows_GainP, *cols_GainP; //*ldiag, *cscale,
  //int__t *pivot, *pivot_inv;
  // rpi = (real__t *)malloc(sizeof(real__t)*(n));
  
  lx_GainP = ux_GainP = NULL; 
  li_GainP = ui_GainP = NULL; 
  lp_GainP = up_GainP = NULL; 

  int row, col, k;
  int LUrow, LUcol;
    
  GraphLU_DumpLU(graphlu_GainP, &lx_GainP, &li_GainP, &lp_GainP, &ux_GainP, &ui_GainP, &up_GainP); 
     
   // Get the number of nonzeros in the factorized matrix
 	//fctnnz = graphlu->lu_nnz;
 	//l_nnz = graphlu->l_nnz;
 	//u_nnz = graphlu->u_nnz;
 
 	std::cout << " ======================== Number of Total nonzeros after factorization is: "<< graphlu_GainP->lu_nnz << "========================"<<std::endl;
 	std::cout << " ======================== Number of nonzeros in L: "<< graphlu_GainP->l_nnz << "========================"<<std::endl;
 	std::cout << " ======================== Number of nonzeros in U: "<< graphlu_GainP->u_nnz << "========================"<<std::endl;
 	
   // get the permutation arrays, rp and cp may be different if the matrix is not symmetric
  rp_GainP = graphlu_GainP->row_perm; // row permutation, permuted row # --> original row #  (original row)
  rpi_GainP = graphlu_GainP->row_perm_inv;  // row permutation inverse, original row # --> permuted row # (permuted row)
  cp_GainP = graphlu_GainP->col_perm;
  cpi_GainP = graphlu_GainP->col_perm_inv;

  //ldiag = graphlu->ldiag; // diagnal elements
  //cscale = graphlu->cscale; // sclaing factors
  rows_GainP = graphlu_GainP->row_scale; 
  cols_GainP = graphlu_GainP->col_scale_perm;
  nnz_LGainP = graphlu_GainP->l_nnz;
  nnz_UGainP = graphlu_GainP->u_nnz;
  //pivot = graphlu->pivot; 
  //pivot_inv = graphlu->pivot_inv; // pivoting array  
	
	gettimeofday(&t3_st, 0);
	std::cout << "\n ======================== Save L_GainP and U_GainP ======================== \n"<< std::endl;

	
	// free MatrixGainP_L() and MatrixGainP_U();
	SingletonMatrixInterface::deleteMatrixGainP_U();
	SingletonMatrixInterface::deleteMatrixGainP_L();
	
	// static testing [Chen Yuan]  
	CSRMatrix *L_GainP = SingletonMatrixInterface::getMatrixGainP_L();
	CSRMatrix *U_GainP = SingletonMatrixInterface::getMatrixGainP_U();
	L_GainP->loadMatrix(n_GainP, nnz_LGainP, lp_GainP, li_GainP, lx_GainP, rp_GainP, cpi_GainP, rows_GainP, cols_GainP);
	U_GainP->loadMatrix(n_GainP, nnz_UGainP, up_GainP, ui_GainP, ux_GainP);
	// static testing end

	gettimeofday(&t3_end, 0);
	seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	useconds = t3_end.tv_usec  - t3_st.tv_usec;
	
	printf("\n\n============================================================================================== ");
	std::cout << "Time for Save L_GainP and U_GainP:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");


    
// ------------------------------------------ [Debug] print out and check --------------------------------------------
// -------------------------------------------------------------------------------------------------------------------

 	// for (i=0; i<n_GainP; ++i){ // check the permutation arrays
 	 	// std::cout << i << ",rp_GainP," << rp_GainP[i] << ",rpi_GainP," << rpi_GainP[i] << ",cp_GainP," << cp_GainP[i] << ",cpi_GainP," << cpi_GainP[i] << std::endl;
 	// } 
  
  //std::cout << " ======================== Get L Factor ========================"<<std::endl;
 	/*for (i=0; i<l_nnz; ++i){
     std::cout<< i << ",lx,"<< lx[i] << ",li,"<< li[i] << std::endl; 
  }
   
  for (i=0; i<n+1; ++i){
     std::cout<< i << ",lp,"<< lp[i] << std::endl;
  }
   
  //std::cout << "\n ======================== Get U Factor ========================"<<std::endl;
 	for (i=0; i<u_nnz; ++i){
     std::cout<< i << ",ux,"<< ux[i] << ",ui,"<< ui[i] << std::endl;
  }
   
  for (i=0; i<n+1; ++i){
 		std::cout<< i << ",up,"<< up[i] << std::endl;
 	}*/

 	// check arrays
  //for (i=0; i<n; ++i){
 	// 	std::cout<< i << ",\t diag:"<< ldiag[i] << ",\t cscale:"<< cscale[i] << ",\t rows:"<< rows[i] << ",\t cols:"<< cols[i] << std::endl;
  //}
  //for (i=0; i<n; ++i){
 	// 	std::cout<< i << ",rp," << rp[i]  << ",cpi," << cpi[i] << ",rows,"<< rows[i] << ",cols," << cols[i] <<std::endl;
  //}  
 
 	// // multiply U factor by cols (which is )
 	// for (i=0; i<n; ++i){
 	// 	for (j=up[i]; j<up[i+1]; ++j){
 	// 		p=rp[piv[ui[j]]]; // this may be rp!!!!!!!!!!!
 	// 		ux[j] = ux[j]/cols[p]; // include the scaling factor into L factor
 	// 	}
 	// }
 
 	// ================================= Get the pivoting vectors =================================
 	//for (i=0; i<n; ++i){
 	//	std::cout<< i << ",\t pivot:"<< pivot[i] << ",\t pivot_inv:"<< pivot_inv[i] << std::endl;
 	//}

  
    //get the factorized LU values and find the row and col before permutation
    // for (i = 0; i < n_GainP; ++i){
      // row = rp_GainP[i];
      // LUrow = row+1;
      
      // gVertex_GainP += T_vertex_GainP(i+1, lp_GainP[i+1]-lp_GainP[i], up_GainP[i+1]-up_GainP[i], rp_GainP[i], cpi_GainP[i], rows_GainP[i], cols_GainP[i]); 
      // // process all non-zeros in L matrix on row i
      // for (j = lp_GainP[i]; j < lp_GainP[i+1]; ++j){
        // col = cp_GainP[li_GainP[j]];             
        // LUcol = col+1;
        
        // gMatrix_GainP_L += T_GainP_L_matrix((i+1)*100000+(li_GainP[j]+1), li_GainP[j], lx_GainP[j]); 
      // } 
         
      // // process all non-zeros in U matrix on row i
      // for (j = up_GainP[i]; j < up_GainP[i+1]; ++j){
        // col = cp_GainP[ui_GainP[j]];
        // LUcol = col+1; 
        
        // gMatrix_GainP_U += T_GainP_U_matrix((i+1)*100000+(ui_GainP[j]+1), ui_GainP[j], ux_GainP[j]);          
      // }   
    // }
    // gVertex_GainP.sort_heap();
    // gMatrix_GainP_L.sort_heap();
    // gMatrix_GainP_U.sort_heap();

  
  //gVertex_p.sort_heap();
  //gMatrix_p_L.sort_heap();
  //gMatrix_p_U.sort_heap();
	// ----------------------------------------------------------------------------------------------------
	// 								Call GRAPHLU and Factorize GainQ Matirx
	// ----------------------------------------------------------------------------------------------------
	std::cout << "\n ======================== Start factorizing GainQ Matirx ======================== \n"<< std::endl;
	// Initialize the structure for GRAPHLU
	graphlu_GainQ = (SGraphLU *)malloc(sizeof(SGraphLU));
	GraphLU_Initialize(graphlu_GainQ);
 
	// Create Matrix
	GraphLU_CreateMatrix(graphlu_GainQ, n_GainQ, nnz_GainQ, ax_GainQ, ai_GainQ, ap_GainQ);
	// graphlu_pp->cfgf[0] = 1.;
	
	// // Set control parameters
	//graphlu_pp->cfgi[1] = 0; // 0 is no MC64 scaling
	// 					 // 1 is with MC64 scaling
 
	// Analyze (column/row ordering, scaling. For details please refer to GRAPHLU user's guide)
	GraphLU_Analyze(graphlu_GainQ);
	printf("analysis time: %.8g\n", graphlu_GainQ->stat[0]);
  
	// creates the task scheduler for parallel LU factorization. If you want to run 
	// parallel factorization or parallel re-factorization, it should be called after GraphLU Analyze.
	ret = GraphLU_CreateScheduler(graphlu_GainQ);
	printf("time of creating scheduler: %.8g\n", graphlu_GainQ->stat[4]);
	printf("suggestion: %s.\n", ret==0?"parallel":"sequential");
	
	// This function creates threads for parallel computation. The second argument (thread)
	// specifies the number of threads, including the main thread. The last argument (check)
	// specifies whether to check the number of threads or not.
  if (!ret){ // parallel factorization
	  GraphLU_CreateThreads(graphlu_GainQ, 2, TRUE);
	  printf("total cores: %d, threads created: %d\n", (int)(graphlu_GainQ->stat[9]), (int)(graphlu_GainQ->cfgi[5]));
	
	  // This function binds threads to cores (unbind = FALSE) or unbinds threads from cores (unbind = TRUE).
	  GraphLU_BindThreads(graphlu_GainQ, FALSE);
	
	  // Numerical LU factorization with partial pivoting, parallel
	  error_pp = GraphLU_Factorize_MT(graphlu_GainQ);
	  printf("factorization time: %.8g\n", graphlu_GainQ->stat[1]);
    if (error_pp < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_pp <<std::endl;      
  }
  else{  //Sequential factorization 
	  error_pp = GraphLU_Factorize(graphlu_GainQ);
    printf("Factorization time: %.8g\n", graphlu_GainQ->stat[1]);
    if (error_pp < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_pp <<std::endl; 
  }

  real__t *lx_GainQ, *ux_GainQ; 
  uint__t *li_GainQ, *ui_GainQ; 
  size_t *lp_GainQ, *up_GainQ; 
  //uint__t fctnnz_pp; // number of nonzeros after the matrix is factorized
  //size_t l_nnz_pp, u_nnz_pp; // number of nonzeros in L and U factors
  // get the permutation arrays, please refer to graphlu.h for details
  uint__t *rp_GainQ, *cp_GainQ, *rpi_GainQ, *cpi_GainQ, nnz_LGainQ, nnz_UGainQ; //row (column) permutation, row (column) permutation inverse
  real__t *rows_GainQ, *cols_GainQ;  //*ldiag_pp, *cscale_pp,
  //int__t *pivot_pp, *pivot_inv_pp; 
  
  lx_GainQ = ux_GainQ = NULL; 
  li_GainQ = ui_GainQ = NULL; 
  lp_GainQ = up_GainQ = NULL; 
  
  
  GraphLU_DumpLU(graphlu_GainQ, &lx_GainQ, &li_GainQ, &lp_GainQ, &ux_GainQ, &ui_GainQ, &up_GainQ); 
   
   // Get the number of nonzeros in the factorized matrix
 	//fctnnz_pp = graphlu_pp->lu_nnz;
 	//l_nnz_pp = graphlu_pp->l_nnz;
 	//u_nnz_pp = graphlu_pp->u_nnz;
 
 	std::cout << " ======================== Number of Total nonzeros after factorization is: "<< graphlu_GainQ->lu_nnz << "========================"<<std::endl;
 	std::cout << " ======================== Number of nonzeros in L: "<< graphlu_GainQ->l_nnz << "========================"<<std::endl;
 	std::cout << " ======================== Number of nonzeros in U: "<< graphlu_GainQ->u_nnz << "========================"<<std::endl;
 	
  // get the permutation arrays, rp and cp may be different if the matrix is not symmetric
  rp_GainQ = graphlu_GainQ->row_perm;
  rpi_GainQ = graphlu_GainQ->row_perm_inv;
  cp_GainQ = graphlu_GainQ->col_perm;
  cpi_GainQ = graphlu_GainQ->col_perm_inv;
  
  //ldiag_pp = graphlu_pp->ldiag; // diagnal elements
  //cscale_pp = graphlu_pp->cscale; // sclaing factors
  rows_GainQ = graphlu_GainQ->row_scale; 
  cols_GainQ = graphlu_GainQ->col_scale_perm;
  nnz_LGainQ = graphlu_GainQ->l_nnz;
  nnz_UGainQ = graphlu_GainQ->u_nnz;
  //pivot_pp = graphlu_pp->pivot; 
  //pivot_inv_pp = graphlu_pp->pivot_inv; // pivoting array   
  
	gettimeofday(&t3_st, 0);
	std::cout << "\n ======================== Save L_GainQ and U_GainQ ======================== \n"<< std::endl;
  	// static testing [Chen Yuan]  
	
	// free MatrixGainP_L() and MatrixGainP_U();
	SingletonMatrixInterface::deleteMatrixGainQ_U();
	SingletonMatrixInterface::deleteMatrixGainQ_L();
	
	
	CSRMatrix *L_GainQ = SingletonMatrixInterface::getMatrixGainQ_L();
	CSRMatrix *U_GainQ = SingletonMatrixInterface::getMatrixGainQ_U();
	L_GainQ->loadMatrix(n_GainQ, nnz_LGainQ, lp_GainQ, li_GainQ, lx_GainQ, rp_GainQ, cpi_GainQ, rows_GainQ, cols_GainQ);
	U_GainQ->loadMatrix(n_GainQ, nnz_UGainQ, up_GainQ, ui_GainQ, ux_GainQ);
	// static testing end
	
	gettimeofday(&t3_end, 0);
	seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	useconds = t3_end.tv_usec  - t3_st.tv_usec;
	
	printf("\n\n============================================================================================== ");
	std::cout << "Time for Save L_GainQ and U_GainQ:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");
  
  
  
    
// ------------------------------------------ [Debug] print out and check --------------------------------------------
// -------------------------------------------------------------------------------------------------------------------
	
 	// for (i=0; i<n_GainQ; ++i){ // check the permutation arrays
 	 	// std::cout << "rp_GainQ," << rp_GainQ[i] << ",rpi_GainQ," << rpi_GainQ[i] << ",cp_GainQ,"<< cp_GainQ[i] << ",cpi_GainQ," << cpi_GainQ[i] << std::endl;
 	// } 
  
  //std::cout << " ======================== Get L Factor ========================"<<std::endl;
 	/*for (i=0; i<l_nnz_pp; ++i){
    std::cout<< i << ",lx_pp,"<< lx_pp[i] << ",li_pp,"<< li_pp[i] << std::endl; 
  }
   
  for (i=0; i<n+1; ++i){
    std::cout<< i << ",lp_pp,"<< lp_pp[i] << std::endl;
  }
   
   //std::cout << "\n ======================== Get U Factor ========================"<<std::endl;
 	for (i=0; i<u_nnz_pp; ++i){
    std::cout<< i << ",ux_pp,"<< ux_pp[i] << ",ui_pp,"<< ui_pp[i] << std::endl;
  }
   
 	for (i=0; i<n+1; ++i){
 		std::cout<< i << ",up_pp,"<< up_pp[i] << std::endl;
 	}*/
 
 	// check arrays
  //for (i=0; i<n; ++i){
 	// 	std::cout<< i << ",\t diag_pp:"<< ldiag_pp[i] << ",\t cscale_pp:"<< cscale_pp[i] << ",\t rows_pp:"<< rows_pp[i] << ",\t cols_pp:"<< cols_pp[i] << std::endl;
  //}
  //for (i=0; i<n; ++i){
 	// 	std::cout<< i << ",rp_pp," << rp_pp[i]  << ",cpi_pp," << cpi_pp[i] << ",rows_pp,"<< rows_pp[i] << ",cols_pp," << cols_pp[i] <<std::endl;
  //}   
 
 	// // multiply U factor by cols (which is )
 	// for (i=0; i<n; ++i){
 	// 	for (j=up[i]; j<up[i+1]; ++j){
 	// 		p=rp[piv[ui[j]]]; // this may be rp!!!!!!!!!!!
 	// 		ux[j] = ux[j]/cols[p]; // include the scaling factor into L factor
 	// 	}
 	// }
 
 	// ================================= Get the pivoting vectors =================================
 	//for (i=0; i<n; ++i){
 	//	std::cout<< i << ",\t pivot_pp:"<< pivot_pp[i] << ",\t pivot_inv_pp:"<< pivot_inv_pp[i] << std::endl;
 	//}
  
      	
    //get the factorized LU values and find the row and col before permutation
    // for (i = 0; i < n_GainQ; ++i){
      // row = rp_GainQ[i];
      // LUrow = row+1;
      
      // gVertex_GainQ += T_vertex_GainQ(i+1, lp_GainQ[i+1]-lp_GainQ[i], up_GainQ[i+1]-up_GainQ[i], rp_GainQ[i], cpi_GainQ[i], rows_GainQ[i], cols_GainQ[i]); 
      // // process all non-zeros in L matrix on row i
      // for (j = lp_GainQ[i]; j < lp_GainQ[i+1]; ++j){
        // col = cp_GainQ[li_GainQ[j]];
        // LUcol = col+1;
        
        // gMatrix_GainQ_L += T_GainQ_L_matrix((i+1)*100000+(li_GainQ[j]+1), li_GainQ[j], lx_GainQ[j]); 
      // } 
      
      // // process all non-zeros in U matrix on row i
      // for (j = up_GainQ[i]; j < up_GainQ[i+1]; ++j){
        // col = cp_GainQ[ui_GainQ[j]];
        // LUcol = col+1;
        
        // gMatrix_GainQ_U += T_GainQ_U_matrix((i+1)*100000+(ui_GainQ[j]+1), ui_GainQ[j], ux_GainQ[j]);
      // }    
    // }
    // gVertex_GainQ.sort_heap();
    // gMatrix_GainQ_L.sort_heap();
    // gMatrix_GainQ_U.sort_heap();          

  
  // Get the time for factorizing B' and B"
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	
	printf("\n\n============================================================================================== ");
	std::cout << "Time for GRAPHLU to factorize GainP and GainQ:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");
  
  if(error_p >= 0 && error_pp >= 0){
      result = "Factorization Completed";
      printf("\n\n============================================================================================== ");
  	  std::cout << "Factorization Completed" << std::endl;
  	  printf("\n================================================================================================ "); 
    //continue to do the iterations only if there is no error from factorization
//  	printf("\n\n============================================================================================== ");
//  	std::cout << "\n Start iteratively updating deltaP and deltaQ " << std::endl;
//  	printf("\n================================================================================================\n ");
//  


  } else {
    //factorization failed
 	  result = "Factorization FAILED";
    printf("\n\n============================================================================================== ");
  	std::cout << "Factorization FAILED" << std::endl;
  	printf("\n================================================================================================ ");  
  }
EXIT:
	// std::cout << "\n free1 " << std::endl;
	GraphLU_Destroy(graphlu_GainP);
	GraphLU_Destroy(graphlu_GainQ);
	// std::cout << "\n free2 " << std::endl;
	free(ax_GainP);
	free(ai_GainP);
	free(ap_GainP);
	free(ax_GainQ);
	free(ai_GainQ);
	free(ap_GainQ);
	free(graphlu_GainP);
	free(graphlu_GainQ);
	// std::cout << "\n free3 " << std::endl;
		
	//free(eG);
	//free(eB);
	//free(ei);
	//free(ep);

	//free(deltaP);
	//free(deltaQ);
	//free(Vm);
	//free(Va);		
	//free(Pn);
	//free(Qn);	
  
 	free(lx_GainP);
	free(li_GainP);
	free(lp_GainP);
    free(ux_GainP);
	free(ui_GainP);
	free(up_GainP);
	
 	//free(rp_GainP);
	//free(cp_GainP);
	//free(rpi_GainP);
 	//free(cpi_GainP);
	//free(rows_GainP);
	//free(cols_GainP); 
	

   	
 	free(lx_GainQ);
	free(li_GainQ);
	free(lp_GainQ);
 	free(ux_GainQ);
	free(ui_GainQ);
	free(up_GainQ);   
 	
	//free(rp_GainQ);
	//free(cp_GainQ);
	//free(rpi_GainQ);
 	//free(cpi_GainQ);
	//free(rows_GainQ);
	//free(cols_GainQ); 	

	
  printf("\n\n----------------------------------------------------------------------------------------");
	printf("\t\t End of Running GainMatrix_factorize_static C function!");
	printf("\t\t ----------------------------------------------------------------------------------------\n\n");	
 
  return result;
}



// //**********************************************************************************************************************************
// // Created by: Chen Yuan, chen.yuan@geirina.net
// // Date: 08/01/2017
// // This code performs state estimation Gain matrix factorization. This version performs LU factorization on the Gain_P and Gain_Q matrix
// // After performing LU factorization, the resulting B' and B" LU factors are stored in ListAccums. 
// // History: 
// // 08/01/2017 [Chen Yuan] Added the GainMatrix_factorize C/C++ function to implement LU decomposition for matrices Gain_P and Gain_Q
// // **********************************************************************************************************************************
// template <typename T_Gain, 
          // //typename T_GainQ, 
          // //typename T_GainP_L_matrix, typename GainP_L_matrix_comp,
          // //typename T_GainP_U_matrix, typename GainP_U_matrix_comp, 
          // //typename T_GainQ_L_matrix, typename GainQ_L_matrix_comp,
          // //typename T_GainQ_U_matrix, typename GainQ_U_matrix_comp,
          // //typename T_vertex_GainP, typename vertex_GainP_comp,
          // //typename T_vertex_GainQ, typename vertex_GainQ_comp,
		  // typename T_vertex_g,
		  // typename T_matrix_g>
// inline string GainMatrix_factorize_div (ServiceAPI* _serviceapi, EngineServiceRequest& _request, int64_t &gslackbus, int64_t &gnnz_GainP, int64_t &gnnz_GainQ, ArrayAccum<SumAccum<int64_t>>& gGainP_p, ArrayAccum<SumAccum<int64_t>>& gGainQ_p, ArrayAccum<ListAccum<T_Gain>>& gGainP, ArrayAccum<ListAccum<T_Gain>>& gGainQ,
              // ListAccum<T_matrix_g>& gMatrix_GainP_L, ListAccum<T_matrix_g>& gMatrix_GainP_U, ListAccum<T_vertex_g>& gVertex_GainP,
              // ListAccum<T_matrix_g>& gMatrix_GainQ_L, ListAccum<T_matrix_g>& gMatrix_GainQ_U, ListAccum<T_vertex_g>& gVertex_GainQ) 
			  // {

	// // extern size_t g_si, g_sd, g_sp;
	// printf("\n\n------------------------------------------------------------------------------------------- ");
	// printf("\n Start Running GRAPHLU_fdpf_LU_factorize_only function!\n");
	// std::cout << "GainP Number of rows:" << gGainP.data_.size() << ",\tNumber of nonzeros:" << gnnz_GainP << std::endl;
	// std::cout << "GainQ Number of rows:" << gGainQ.data_.size() << ",\tNumber of nonzeros:" << gnnz_GainQ << std::endl;
	// //std::cout << "Y bus Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gMatrix_all.data_.size() << std::endl;
	// printf("-------------------------------------------------------------------------------------------- \n\n");	
	// // ------------------------------------------------------------------------------------------------
	// // 				Initialize variables and arrays
	// // ------------------------------------------------------------------------------------------------
	
 
	// // Initialize arrays and variables
	// uint__t n_GainP, nnz_GainP, n_GainQ, nnz_GainQ, slackbus; //n_e, nnz_e;
	// int ret, i, j; // p, iter;
	// SGraphLU *graphlu_GainP, *graphlu_GainQ;
  
	// string result = "FAILED";

	// // Get the dimension, n, and the nnz of the matrix GainP and GainQ
	// n_GainP = gGainP.data_.size();	nnz_GainP = gnnz_GainP; n_GainQ = gGainQ.data_.size(); nnz_GainQ = gnnz_GainQ;
	// slackbus = gslackbus;
	// //n_e=gVertex.data_.size(); 	nnz_e=gMatrix_all.data_.size(); // get the size of the Y bus matrix
	
	// //// Convert vectors to GRAPHLU Data Structure (pointers)
	// real__t *ax_GainP, *ax_GainQ; //*deltaP, *deltaQ, *Vm, *Va; *Pn, *Qn; *eG, *eB,
	// uint__t *ai_GainP, *ai_GainQ; // *ei, *piv; *btype; 
	// //uint__t *ap, *ap_pp, *ep;
    // uint__t *ap_GainP, *ap_GainQ; 	
 	
	// ax_GainP = (real__t *)malloc(sizeof(real__t)*(nnz_GainP)); // values in B' 
	// ai_GainP = (uint__t *)malloc(sizeof(uint__t)*(nnz_GainP)); // column indices of B'
	// ap_GainP = (uint__t *)malloc(sizeof(uint__t)*(n_GainP+1)); // initial row pointers
	// // b = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b

	// ax_GainQ = (real__t *)malloc(sizeof(real__t)*(nnz_GainQ)); // values in B"
	// ai_GainQ = (uint__t *)malloc(sizeof(uint__t)*(nnz_GainQ)); // column indices of B"
	// ap_GainQ = (uint__t *)malloc(sizeof(uint__t)*(n_GainQ+1)); // initial row pointers
	
	
	// // [Chen Yuan] 11/10/2017 delclare graphupdates
	 // EngineServiceRequest* request = &_request;
	 // topology4::GraphUpdatesPointer graphupdates;
     // graphupdates = _serviceapi->CreateGraphUpdates(request);

  
	// std::cout << " ======================== Initialization of ararys used to store the factorized matrix and LU ========================"<< std::endl;

	// struct timeval t2_st, t2_end, t3_st, t3_end; long seconds, useconds;

	// // =================================== Sort both GainP and GainQ =================================
	// gettimeofday(&t3_st, 0);
	// gettimeofday(&t2_st, 0);
	// // Input ArrayAccums' ListAccums are assumed to be unsorted. Sort them before assigning values to local arrays
    // struct Gain_sort
	// {
            // inline bool operator() (const T_Gain& tuple1, const T_Gain& tuple2)
            // {
                // return (tuple1.index < tuple2.index);
            // }
	// };
	
    // for(i=0;i<gGainP.data_.size(); ++i){
	   // std::sort(gGainP.data_[i].begin(), gGainP.data_[i].end(), Gain_sort());
    // }
	
	// struct matrix_g_sort
	// {
            // inline bool operator() (const T_matrix_g& tuple3, const T_matrix_g& tuple4)
            // {
                // return (tuple3.G_i < tuple4.G_i);
            // }
	// };

	
	// gettimeofday(&t2_end, 0);
	// seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	// useconds = t2_end.tv_usec  - t2_st.tv_usec;
	// printf("\n\n============================================================================================== ");
	// std::cout << "Time to sort GainP ArrayAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	// printf("\n================================================================================================ "); 
	
	
	// gettimeofday(&t2_st, 0); 
	

	
    // for(i=0;i<gGainQ.data_.size(); ++i){
	   // std::sort(gGainQ.data_[i].begin(), gGainQ.data_[i].end(), Gain_sort());
    // }
	
 	// gettimeofday(&t2_end, 0);
	// seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	// useconds = t2_end.tv_usec  - t2_st.tv_usec;
	// printf("\n\n============================================================================================== ");
	// std::cout << "Time to sort GainQ ArrayAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	// printf("\n================================================================================================ "); 
    
	// // get the time for sorting all HeapAccums 
	// gettimeofday(&t3_end, 0);
	// seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	// useconds = t3_end.tv_usec  - t3_st.tv_usec;
	// printf("\n\n============================================================================================== ");
	// std::cout << "Time to sort both GainP and GainQ ArrayAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	// printf("\n================================================================================================ "); 
	
	// // ========================================= Convert ap_GainP, ap_GainQ =========================================
	// gettimeofday(&t2_st, 0); 
	// int ii = 1; // counter for the array  
	// // Assign value to the first position of the pointer array 
	// ap_GainP[0] = 0;
	// ap_GainQ[0] = 0;
	// //ep[0] = 0;
	// for (i=0;i<gGainP_p.size();i++){
		// ap_GainP[ii] = gGainP_p.data_[i] + ap_GainP[ii-1];
		// //std::cout<< ii << ",ap_GainP,"<< ap_GainP[ii] <<  std::endl;
		// ii++;
	// }
	
	// ii = 1;
	// for (i=0;i<gGainQ_p.size();i++){
		// ap_GainQ[ii] = gGainQ_p.data_[i] + ap_GainQ[ii-1];
		// //std::cout<< ii << ",ap_GainQ,"<< ap_GainQ[ii] <<  std::endl;
		// ii++;
	// }
 
 	// // ========================================= Convert GainP (ai_GainP, ax_GainP) and GainQ (ai_GainQ, ax_GainQ) =========================================
	// int i_GainP = 0, i_GainQ = 0;
	// for (i=0; i<gGainP.data_.size(); ++i) {
		// for (ii=0; ii<gGainP.data_[i].size(); ++ii) {
	// //for (ii=0; ii<gGainP.size(); ++ii) {
    // //ei[ii] = gMatrix_all.data_[ii].ei;
    // //eG[ii] = gMatrix_all.data_[ii].eG;
    // //eB[ii] = gMatrix_all.data_[ii].eB; 
		// //if(gMatrix_all.data_[ii].Bp_x != 0)
		// //{
			// if (gGainP.data_[i].get(ii).index < slackbus){
				// ai_GainP[i_GainP] = gGainP.data_[i].get(ii).index - 1;
			// }
			// else if (gGainP.data_[i].get(ii).index > slackbus){
				// ai_GainP[i_GainP] = gGainP.data_[i].get(ii).index - 2;
			// }
				
			// ax_GainP[i_GainP] = gGainP.data_[i].get(ii).value;
      // //std::cout<< i_GainP << ",ax,"<< ax_GainP[i_GainP] << ",ai,"<< ai_GainP[i_GainP] << std::endl;
			// i_GainP++;
		// }
	// }
    // //}
    // //if(gMatrix_all.data_[ii].Bpp_x != 0)
    // //{
	// for (i=0; i<gGainQ.data_.size(); ++i) {
		// for (ii=0; ii<gGainQ.data_[i].size(); ++ii) {
			// ai_GainQ[i_GainQ] = gGainQ.data_[i].get(ii).index - 1;
			// ax_GainQ[i_GainQ] = gGainQ.data_[i].get(ii).value;
			// //std::cout<< i_GainQ << ",ax,"<< ax_GainQ[i_GainQ] << ",ai,"<< ai_GainQ[i_GainQ] << std::endl;
			// i_GainQ++;
		// }
	// } 	
	// // Done converting all input HeapAccums to arrays
  
	// // Get the time to convert the data between containers and arrays
	// gettimeofday(&t2_end, 0);
	// seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	// useconds = t2_end.tv_usec  - t2_st.tv_usec;
	// printf("\n\n============================================================================================== ");
	// std::cout << "Time to convert data to GRAPHLU time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	// printf("\n================================================================================================ ");		
	
	// // ----------------------------------------------------------------------------------------------------
	// // 								Call GRAPHLU and Factorize GainP Matirx
	// // ----------------------------------------------------------------------------------------------------
	// int error_p = 0, error_pp = 0;
	// gettimeofday(&t2_st, 0);
	// std::cout << "\n ======================== Start factorizing GainP Matirx ======================== \n"<< std::endl;
	// // Initialize the structure for GRAPHLU
	// graphlu_GainP = (SGraphLU *)malloc(sizeof(SGraphLU));
	// GraphLU_Initialize(graphlu_GainP);
	
	// // Create Matrix
	// GraphLU_CreateMatrix(graphlu_GainP, n_GainP, nnz_GainP, ax_GainP, ai_GainP, ap_GainP);
	// // graphlu->cfgf[0] = 1.;
	// // // Set control parameters
	// //graphlu->cfgi[1] = 0; // 0 is no MC64 scaling
	// // 					 // 1 is with MC64 scaling

	// // Analyze (column/row ordering, scaling. For details please refer to GRAPHLU user's guide)
	// GraphLU_Analyze(graphlu_GainP);
	// printf("analysis time: %.8g\n", graphlu_GainP->stat[0]);
	
	// // creates the task scheduler for parallel LU factorization. If you want to run 
	// // parallel factorization or parallel re-factorization, it should be called after GraphLU Analyze.
	// ret = GraphLU_CreateScheduler(graphlu_GainP);
	// printf("time of creating scheduler: %.8g\n", graphlu_GainP->stat[4]);
	// printf("suggestion: %s.\n", ret==0?"parallel":"sequential");
	// //std::cout << "GraphLU MC64 scaling 4th:" << graphlu->cfgi[1] <<std::endl; 
	// // This function creates threads for parallel computation. The second argument (thread)
	// // specifies the number of threads, including the main thread. The last argument (check)
	// // specifies whether to check the number of threads or not.
  // if (!ret){ // parallel factorization 
	  // GraphLU_CreateThreads(graphlu_GainP, 2, TRUE);
	  // printf("total cores: %d, threads created: %d\n", (int)(graphlu_GainP->stat[9]), (int)(graphlu_GainP->cfgi[5]));
	
	  // // This function binds threads to cores (unbind = FALSE) or unbinds threads from cores (unbind = TRUE).
	  // GraphLU_BindThreads(graphlu_GainP, FALSE);
	
	  // // Numerical LU factorization with partial pivoting, parallel
	  // error_p = GraphLU_Factorize_MT(graphlu_GainP);
	  // printf("factorization time: %.8g\n", graphlu_GainP->stat[1]);
    // if (error_p < 0) //there is an error, print the code 
      // std::cout << "GraphLU_Factorize error code:" << error_p <<std::endl;  
  // }   
  // else{  // Sequential factorization
	  // error_p = GraphLU_Factorize(graphlu_GainP);
	  // printf("Factorization time: %.8g\n", graphlu_GainP->stat[1]);
    // if (error_p < 0) //there is an error, print the code 
      // std::cout << "GraphLU_Factorize error code:" << error_p <<std::endl; 
	// }
 
  // real__t *lx_GainP, *ux_GainP; 
  // uint__t *li_GainP, *ui_GainP; 
  // size_t *lp_GainP, *up_GainP; 
 	// //uint__t fctnnz_GainP; // number of nonzeros after the matrix is factorized
	// //size_t l_nnz_GainP, u_nnz_GainP; // number of nonzeros in L and U factors
  // // get the permutation arrays, please refer to graphlu.h for details
  // uint__t *rp_GainP, *cp_GainP, *rpi_GainP, *cpi_GainP; //row (column) permutation, row (column) permutation inverse
  // real__t *rows_GainP, *cols_GainP; //*ldiag, *cscale,
  // //int__t *pivot, *pivot_inv;
  // // rpi = (real__t *)malloc(sizeof(real__t)*(n));
  
  // lx_GainP = ux_GainP = NULL; 
  // li_GainP = ui_GainP = NULL; 
  // lp_GainP = up_GainP = NULL; 

  // int row, col, k;
  // int LUrow, LUcol;
    
  // GraphLU_DumpLU(graphlu_GainP, &lx_GainP, &li_GainP, &lp_GainP, &ux_GainP, &ui_GainP, &up_GainP); 
     
   // // Get the number of nonzeros in the factorized matrix
 	// //fctnnz = graphlu->lu_nnz;
 	// //l_nnz = graphlu->l_nnz;
 	// //u_nnz = graphlu->u_nnz;
 
 	// std::cout << " ======================== Number of Total nonzeros after factorization is: "<< graphlu_GainP->lu_nnz << "========================"<<std::endl;
 	// std::cout << " ======================== Number of nonzeros in L: "<< graphlu_GainP->l_nnz << "========================"<<std::endl;
 	// std::cout << " ======================== Number of nonzeros in U: "<< graphlu_GainP->u_nnz << "========================"<<std::endl;
 	
   // // get the permutation arrays, rp and cp may be different if the matrix is not symmetric
  // rp_GainP = graphlu_GainP->row_perm; // row permutation, permuted row # --> original row #  (original row)
  // rpi_GainP = graphlu_GainP->row_perm_inv;  // row permutation inverse, original row # --> permuted row # (permuted row)
  // cp_GainP = graphlu_GainP->col_perm;
  // cpi_GainP = graphlu_GainP->col_perm_inv;

  // //ldiag = graphlu->ldiag; // diagnal elements
  // //cscale = graphlu->cscale; // sclaing factors
  // rows_GainP = graphlu_GainP->row_scale; 
  // cols_GainP = graphlu_GainP->col_scale_perm;
  // //pivot = graphlu->pivot; 
  // //pivot_inv = graphlu->pivot_inv; // pivoting array   
    
 	// // print out and check
 	// // for (i=0; i<n_GainP; ++i){ // check the permutation arrays
 	 	// // std::cout << i << ",rp_GainP," << rp_GainP[i] << ",rpi_GainP," << rpi_GainP[i] << ",cp_GainP," << cp_GainP[i] << ",cpi_GainP," << cpi_GainP[i] << std::endl;
 	// // } 
  
  // //std::cout << " ======================== Get L Factor ========================"<<std::endl;
 	// /*for (i=0; i<l_nnz; ++i){
     // std::cout<< i << ",lx,"<< lx[i] << ",li,"<< li[i] << std::endl; 
  // }
   
  // for (i=0; i<n+1; ++i){
     // std::cout<< i << ",lp,"<< lp[i] << std::endl;
  // }
   
  // //std::cout << "\n ======================== Get U Factor ========================"<<std::endl;
 	// for (i=0; i<u_nnz; ++i){
     // std::cout<< i << ",ux,"<< ux[i] << ",ui,"<< ui[i] << std::endl;
  // }
   
  // for (i=0; i<n+1; ++i){
 		// std::cout<< i << ",up,"<< up[i] << std::endl;
 	// }*/

 	// // check arrays
  // //for (i=0; i<n; ++i){
 	// // 	std::cout<< i << ",\t diag:"<< ldiag[i] << ",\t cscale:"<< cscale[i] << ",\t rows:"<< rows[i] << ",\t cols:"<< cols[i] << std::endl;
  // //}
  // //for (i=0; i<n; ++i){
 	// // 	std::cout<< i << ",rp," << rp[i]  << ",cpi," << cpi[i] << ",rows,"<< rows[i] << ",cols," << cols[i] <<std::endl;
  // //}  
 
 	// // // multiply U factor by cols (which is )
 	// // for (i=0; i<n; ++i){
 	// // 	for (j=up[i]; j<up[i+1]; ++j){
 	// // 		p=rp[piv[ui[j]]]; // this may be rp!!!!!!!!!!!
 	// // 		ux[j] = ux[j]/cols[p]; // include the scaling factor into L factor
 	// // 	}
 	// // }
 
 	// // ================================= Get the pivoting vectors =================================
 	// //for (i=0; i<n; ++i){
 	// //	std::cout<< i << ",\t pivot:"<< pivot[i] << ",\t pivot_inv:"<< pivot_inv[i] << std::endl;
 	// //}

  // topology4::AttributeUpdatePointer attrUpdate_vP = graphupdates->GetVertexAttributeUpdate (0);
    // //get the factorized LU values and find the row and col before permutation
    // for (i = 0; i < n_GainP; ++i){
      
		// row = rp_GainP[i];
		// LUrow = row+1;



		// //gVertex_GainP += T_vertex_GainP(lp_GainP[i+1]-lp_GainP[i], up_GainP[i+1]-up_GainP[i], rp_GainP[i], cpi_GainP[i], rows_GainP[i], cols_GainP[i]); 
		
		// gVertex_GainP.data_.push_back(T_vertex_g(lp_GainP[i+1]-lp_GainP[i], up_GainP[i+1]-up_GainP[i], rp_GainP[i], cpi_GainP[i], rows_GainP[i], cols_GainP[i]));

		// if (i < slackbus) {
		// //gVertex_GainP.sort_heap();
		// attrUpdate_vP->Set(9, gVertex_GainP.data_); 
		// // update the attr for each node by exId
		
			// topology4::DeltaVertexId vVid_vP (0, to_string(i+1));
			// graphupdates->UpsertVertex (vVid_vP, attrUpdate_vP);
		// }
		// else if (i > slackbus) {
			// //gVertex_GainP.sort_heap();
			// attrUpdate_vP->Set(9, gVertex_GainP.data_); 
			// // update the attr for each node by exId
			// topology4::DeltaVertexId vVid_vP (0, to_string(i+2));
			// graphupdates->UpsertVertex (vVid_vP, attrUpdate_vP);
		// }
		 		 
		// // clear heap
		// gVertex_GainP.clear();


		// // process all non-zeros in L matrix on row i
		// for (j = lp_GainP[i]; j < lp_GainP[i+1]; ++j){
			// col = cp_GainP[li_GainP[j]];             
			// LUcol = col+1;

			// //gMatrix_GainP_L += T_GainP_L_matrix(li_GainP[j], lx_GainP[j]); 
			// gMatrix_GainP_L.data_.push_back(T_matrix_g(li_GainP[j], lx_GainP[j]));  
		// } 
	  

		// std::sort(gMatrix_GainP_L.data_.begin(), gMatrix_GainP_L.data_.end(), matrix_g_sort());

	    // if (i < slackbus) {
			// //gMatrix_GainP_L.sort_heap(); 
			// // update Lp
			// attrUpdate_vP->Set(7, gMatrix_GainP_L.data_);  
			// // update the attr for each node by exId
			// topology4::DeltaVertexId vVid_lP (0, to_string(i+1));
			// graphupdates->UpsertVertex (vVid_lP, attrUpdate_vP); 	
		// }
		// else if (i > slackbus) {
			// //gMatrix_GainP_L.sort_heap(); 
			// // update Lp
			// attrUpdate_vP->Set(7, gMatrix_GainP_L.data_);  
			// // update the attr for each node by exId
			// topology4::DeltaVertexId vVid_lP (0, to_string(i+2));
			// graphupdates->UpsertVertex (vVid_lP, attrUpdate_vP); 
		// }
		// // clear heap
		// gMatrix_GainP_L.clear();	
	  
         
      // // process all non-zeros in U matrix on row i
      // for (j = up_GainP[i]; j < up_GainP[i+1]; ++j){
        // col = cp_GainP[ui_GainP[j]];
        // LUcol = col+1; 
        
        // //gMatrix_GainP_U += T_GainP_U_matrix(ui_GainP[j], ux_GainP[j]);       
		// gMatrix_GainP_U.data_.push_back(T_matrix_g(ui_GainP[j], ux_GainP[j]));		
      // }
	  
	  // std::sort(gMatrix_GainP_U.data_.begin(), gMatrix_GainP_U.data_.end(), matrix_g_sort());

	  
	  // if (i < slackbus) {
		// //gMatrix_GainP_U.sort_heap(); 
		// // update Lp
		// attrUpdate_vP->Set(8, gMatrix_GainP_U.data_);  
        // // update the attr for each node by exId
        // topology4::DeltaVertexId vVid_uP (0, to_string(i+1));
        // graphupdates->UpsertVertex (vVid_uP, attrUpdate_vP); 	
	  // }
	  // else if (i > slackbus) {
		// //gMatrix_GainP_U.sort_heap(); 
		// // update Lp
		// attrUpdate_vP->Set(8, gMatrix_GainP_U.data_);  
        // // update the attr for each node by exId
        // topology4::DeltaVertexId vVid_uP (0, to_string(i+2));
        // graphupdates->UpsertVertex (vVid_uP, attrUpdate_vP); 
	  // }
		// // clear heap
		// gMatrix_GainP_U.clear();	
	  	  
    // }
	
  
  // //gVertex_p.sort_heap();
  // //gMatrix_p_L.sort_heap();
  // //gMatrix_p_U.sort_heap();
	// // ----------------------------------------------------------------------------------------------------
	// // 								Call GRAPHLU and Factorize GainQ Matirx
	// // ----------------------------------------------------------------------------------------------------
	// std::cout << "\n ======================== Start factorizing GainQ Matirx ======================== \n"<< std::endl;
	// // Initialize the structure for GRAPHLU
	// graphlu_GainQ = (SGraphLU *)malloc(sizeof(SGraphLU));
	// GraphLU_Initialize(graphlu_GainQ);
 
	// // Create Matrix
	// GraphLU_CreateMatrix(graphlu_GainQ, n_GainQ, nnz_GainQ, ax_GainQ, ai_GainQ, ap_GainQ);
	// // graphlu_pp->cfgf[0] = 1.;
	
	// // // Set control parameters
	// //graphlu_pp->cfgi[1] = 0; // 0 is no MC64 scaling
	// // 					 // 1 is with MC64 scaling
 
	// // Analyze (column/row ordering, scaling. For details please refer to GRAPHLU user's guide)
	// GraphLU_Analyze(graphlu_GainQ);
	// printf("analysis time: %.8g\n", graphlu_GainQ->stat[0]);
  
	// // creates the task scheduler for parallel LU factorization. If you want to run 
	// // parallel factorization or parallel re-factorization, it should be called after GraphLU Analyze.
	// ret = GraphLU_CreateScheduler(graphlu_GainQ);
	// printf("time of creating scheduler: %.8g\n", graphlu_GainQ->stat[4]);
	// printf("suggestion: %s.\n", ret==0?"parallel":"sequential");
	
	// // This function creates threads for parallel computation. The second argument (thread)
	// // specifies the number of threads, including the main thread. The last argument (check)
	// // specifies whether to check the number of threads or not.
  // if (!ret){ // parallel factorization
	  // GraphLU_CreateThreads(graphlu_GainQ, 2, TRUE);
	  // printf("total cores: %d, threads created: %d\n", (int)(graphlu_GainQ->stat[9]), (int)(graphlu_GainQ->cfgi[5]));
	
	  // // This function binds threads to cores (unbind = FALSE) or unbinds threads from cores (unbind = TRUE).
	  // GraphLU_BindThreads(graphlu_GainQ, FALSE);
	
	  // // Numerical LU factorization with partial pivoting, parallel
	  // error_pp = GraphLU_Factorize_MT(graphlu_GainQ);
	  // printf("factorization time: %.8g\n", graphlu_GainQ->stat[1]);
    // if (error_pp < 0) //there is an error, print the code 
      // std::cout << "GraphLU_Factorize error code:" << error_pp <<std::endl;      
  // }
  // else{  //Sequential factorization 
	  // error_pp = GraphLU_Factorize(graphlu_GainQ);
    // printf("Factorization time: %.8g\n", graphlu_GainQ->stat[1]);
    // if (error_pp < 0) //there is an error, print the code 
      // std::cout << "GraphLU_Factorize error code:" << error_pp <<std::endl; 
  // }

  // real__t *lx_GainQ, *ux_GainQ; 
  // uint__t *li_GainQ, *ui_GainQ; 
  // size_t *lp_GainQ, *up_GainQ; 
  // //uint__t fctnnz_pp; // number of nonzeros after the matrix is factorized
  // //size_t l_nnz_pp, u_nnz_pp; // number of nonzeros in L and U factors
  // // get the permutation arrays, please refer to graphlu.h for details
  // uint__t *rp_GainQ, *cp_GainQ, *rpi_GainQ, *cpi_GainQ; //row (column) permutation, row (column) permutation inverse
  // real__t *rows_GainQ, *cols_GainQ;  //*ldiag_pp, *cscale_pp,
  // //int__t *pivot_pp, *pivot_inv_pp; 
  
  // lx_GainQ = ux_GainQ = NULL; 
  // li_GainQ = ui_GainQ = NULL; 
  // lp_GainQ = up_GainQ = NULL; 
  
  
  // GraphLU_DumpLU(graphlu_GainQ, &lx_GainQ, &li_GainQ, &lp_GainQ, &ux_GainQ, &ui_GainQ, &up_GainQ); 
   
   // // Get the number of nonzeros in the factorized matrix
 	// //fctnnz_pp = graphlu_pp->lu_nnz;
 	// //l_nnz_pp = graphlu_pp->l_nnz;
 	// //u_nnz_pp = graphlu_pp->u_nnz;
 
 	// std::cout << " ======================== Number of Total nonzeros after factorization is: "<< graphlu_GainQ->lu_nnz << "========================"<<std::endl;
 	// std::cout << " ======================== Number of nonzeros in L: "<< graphlu_GainQ->l_nnz << "========================"<<std::endl;
 	// std::cout << " ======================== Number of nonzeros in U: "<< graphlu_GainQ->u_nnz << "========================"<<std::endl;
 	
  // // get the permutation arrays, rp and cp may be different if the matrix is not symmetric
  // rp_GainQ = graphlu_GainQ->row_perm;
  // rpi_GainQ = graphlu_GainQ->row_perm_inv;
  // cp_GainQ = graphlu_GainQ->col_perm;
  // cpi_GainQ = graphlu_GainQ->col_perm_inv;
  
  // //ldiag_pp = graphlu_pp->ldiag; // diagnal elements
  // //cscale_pp = graphlu_pp->cscale; // sclaing factors
  // rows_GainQ = graphlu_GainQ->row_scale; 
  // cols_GainQ = graphlu_GainQ->col_scale_perm;
  // //pivot_pp = graphlu_pp->pivot; 
  // //pivot_inv_pp = graphlu_pp->pivot_inv; // pivoting array   
    
 	// // print out and check
 	// // for (i=0; i<n_GainQ; ++i){ // check the permutation arrays
 	 	// // std::cout << "rp_GainQ," << rp_GainQ[i] << ",rpi_GainQ," << rpi_GainQ[i] << ",cp_GainQ,"<< cp_GainQ[i] << ",cpi_GainQ," << cpi_GainQ[i] << std::endl;
 	// // } 
  
  // //std::cout << " ======================== Get L Factor ========================"<<std::endl;
 	// /*for (i=0; i<l_nnz_pp; ++i){
    // std::cout<< i << ",lx_pp,"<< lx_pp[i] << ",li_pp,"<< li_pp[i] << std::endl; 
  // }
   
  // for (i=0; i<n+1; ++i){
    // std::cout<< i << ",lp_pp,"<< lp_pp[i] << std::endl;
  // }
   
   // //std::cout << "\n ======================== Get U Factor ========================"<<std::endl;
 	// for (i=0; i<u_nnz_pp; ++i){
    // std::cout<< i << ",ux_pp,"<< ux_pp[i] << ",ui_pp,"<< ui_pp[i] << std::endl;
  // }
   
 	// for (i=0; i<n+1; ++i){
 		// std::cout<< i << ",up_pp,"<< up_pp[i] << std::endl;
 	// }*/
 
 	// // check arrays
  // //for (i=0; i<n; ++i){
 	// // 	std::cout<< i << ",\t diag_pp:"<< ldiag_pp[i] << ",\t cscale_pp:"<< cscale_pp[i] << ",\t rows_pp:"<< rows_pp[i] << ",\t cols_pp:"<< cols_pp[i] << std::endl;
  // //}
  // //for (i=0; i<n; ++i){
 	// // 	std::cout<< i << ",rp_pp," << rp_pp[i]  << ",cpi_pp," << cpi_pp[i] << ",rows_pp,"<< rows_pp[i] << ",cols_pp," << cols_pp[i] <<std::endl;
  // //}   
 
 	// // // multiply U factor by cols (which is )
 	// // for (i=0; i<n; ++i){
 	// // 	for (j=up[i]; j<up[i+1]; ++j){
 	// // 		p=rp[piv[ui[j]]]; // this may be rp!!!!!!!!!!!
 	// // 		ux[j] = ux[j]/cols[p]; // include the scaling factor into L factor
 	// // 	}
 	// // }
 
 	// // ================================= Get the pivoting vectors =================================
 	// //for (i=0; i<n; ++i){
 	// //	std::cout<< i << ",\t pivot_pp:"<< pivot_pp[i] << ",\t pivot_inv_pp:"<< pivot_inv_pp[i] << std::endl;
 	// //}
  
    // topology4::AttributeUpdatePointer attrUpdate_vQ = graphupdates->GetVertexAttributeUpdate (0);  	
    // //get the factorized LU values and find the row and col before permutation
    // for (i = 0; i < n_GainQ; ++i){
		// row = rp_GainQ[i];
		// LUrow = row+1;

		// //gVertex_GainQ += T_vertex_GainQ(lp_GainQ[i+1]-lp_GainQ[i], up_GainQ[i+1]-up_GainQ[i], rp_GainQ[i], cpi_GainQ[i], rows_GainQ[i], cols_GainQ[i]); 

		// gVertex_GainQ.data_.push_back(T_vertex_g(lp_GainQ[i+1]-lp_GainQ[i], up_GainQ[i+1]-up_GainQ[i], rp_GainQ[i], cpi_GainQ[i], rows_GainQ[i], cols_GainQ[i]));
		
		
		// //gVertex_GainQ.sort_heap();
		// attrUpdate_vQ->Set(12, gVertex_GainQ.data_); 
		// // update the attr for each node by exId
		// topology4::DeltaVertexId vVid_vQ (0, to_string(i+1));
		// graphupdates->UpsertVertex (vVid_vQ, attrUpdate_vQ); 		 
		// // clear heap
		// gVertex_GainQ.clear();


		// // process all non-zeros in L matrix on row i
		// for (j = lp_GainQ[i]; j < lp_GainQ[i+1]; ++j){
			// col = cp_GainQ[li_GainQ[j]];
			// LUcol = col+1;

			// //gMatrix_GainQ_L += T_GainQ_L_matrix(li_GainQ[j], lx_GainQ[j]); 
			
			// gMatrix_GainQ_L.data_.push_back(T_matrix_g(li_GainQ[j], lx_GainQ[j]));
		// } 
      
		// std::sort(gMatrix_GainQ_L.data_.begin(), gMatrix_GainQ_L.data_.end(), matrix_g_sort());
		// //gMatrix_GainQ_L.sort_heap(); 
		// // update Lp
		// attrUpdate_vQ->Set(10, gMatrix_GainQ_L.data_);  
		// // update the attr for each node by exId
		 // topology4::DeltaVertexId vVid_lQ (0, to_string(i+1));
		 // graphupdates->UpsertVertex (vVid_lQ, attrUpdate_vQ); 	
		// // clear heap
		// gMatrix_GainQ_L.clear();		  
	  
	  
      // // process all non-zeros in U matrix on row i
      // for (j = up_GainQ[i]; j < up_GainQ[i+1]; ++j){
        // col = cp_GainQ[ui_GainQ[j]];
        // LUcol = col+1;
        
        // gMatrix_GainQ_U.data_.push_back(T_matrix_g(ui_GainQ[j], ux_GainQ[j]));
		
		// //gMatrix_GainQ_U += T_GainQ_U_matrix(ui_GainQ[j], ux_GainQ[j]);
      // } 
	  
	  // std::sort(gMatrix_GainQ_U.data_.begin(), gMatrix_GainQ_U.data_.end(), matrix_g_sort());
	  // //gMatrix_GainQ_U.sort_heap(); 
	  // // update Lp
	  // attrUpdate_vQ->Set(11, gMatrix_GainQ_U.data_);  
        // // update the attr for each node by exId
         // topology4::DeltaVertexId vVid_uQ (0, to_string(i+1));
         // graphupdates->UpsertVertex (vVid_uQ, attrUpdate_vQ); 	
		// // clear heap
		// gMatrix_GainQ_U.clear();		  

	  
    // }
            
	// if (graphupdates->SendGraphUpdates(true,true) == false) {
		// //show error msg
		// std::cout << " if (graphupdates->SendGraphUpdates(true,true) == false) " << std::endl;
	// } 
	// graphupdates->Commit(true);

  // // Get the time for factorizing B' and B"
	// gettimeofday(&t2_end, 0);
	// seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	// useconds = t2_end.tv_usec  - t2_st.tv_usec;
	
	// printf("\n\n============================================================================================== ");
	// std::cout << "Time for GRAPHLU to factorize GainP and GainQ:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	// printf("\n================================================================================================ ");
  
  // if(error_p >= 0 && error_pp >= 0){
      // result = "Factorization Completed";
      // printf("\n\n============================================================================================== ");
  	  // std::cout << "Factorization Completed" << std::endl;
  	  // printf("\n================================================================================================ "); 


  // } else {
    // //factorization failed
 	  // result = "Factorization FAILED";
    // printf("\n\n============================================================================================== ");
  	// std::cout << "Factorization FAILED" << std::endl;
  	// printf("\n================================================================================================ ");  
  // }
// EXIT:
	// // std::cout << "\n free1 " << std::endl;
	// GraphLU_Destroy(graphlu_GainP);
	// GraphLU_Destroy(graphlu_GainQ);
	// // std::cout << "\n free2 " << std::endl;
	// free(ax_GainP);
	// free(ai_GainP);
	// free(ap_GainP);
	// free(ax_GainQ);
	// free(ai_GainQ);
	// free(ap_GainQ);
	// free(graphlu_GainP);
	// free(graphlu_GainQ);
	// // std::cout << "\n free3 " << std::endl;
		
	// //free(eG);
	// //free(eB);
	// //free(ei);
	// //free(ep);

	// //free(deltaP);
	// //free(deltaQ);
	// //free(Vm);
	// //free(Va);		
	// //free(Pn);
	// //free(Qn);	
  
 	// free(lx_GainP);
	// free(li_GainP);
	// free(lp_GainP);
    // free(ux_GainP);
	// free(ui_GainP);
	// free(up_GainP);  
 	// free(lx_GainQ);
	// free(li_GainQ);
	// free(lp_GainQ);
 	// free(ux_GainQ);
	// free(ui_GainQ);
	// free(up_GainQ);    
	
  // printf("\n\n----------------------------------------------------------------------------------------");
	// printf("\t\t End of Running GainMatrix_factorize_div C function!");
	// printf("\t\t ----------------------------------------------------------------------------------------\n\n");	
 
  // return result;
// }





// // ****** Testing [Chen Yuan] **********


// //**********************************************************************************************************************************
// // Created by: Chen Yuan, chen.yuan@geirina.net
// // Date: 08/01/2017
// // This code performs state estimation Gain matrix factorization and equation solve. 

// // History: 
// // 11/08/2017 [Chen Yuan] Gain matrix factorization and state estimation equation solve
// // **********************************************************************************************************************************
// template <typename T_GainP, 
          // typename T_GainQ, 
          // typename T_GainP_L_matrix, typename GainP_L_matrix_comp,
          // typename T_GainP_U_matrix, typename GainP_U_matrix_comp, 
          // typename T_GainQ_L_matrix, typename GainQ_L_matrix_comp,
          // typename T_GainQ_U_matrix, typename GainQ_U_matrix_comp,
          // typename T_vertex_GainP, typename vertex_GainP_comp,
          // typename T_vertex_GainQ, typename vertex_GainQ_comp,
		  // typename T_HP_transpose,
		  // typename T_HQ_transpose>
// inline string SE_factorize_solve (int64_t &gslackbus, int64_t &gnnz_GainP, int64_t &gnnz_GainQ, ArrayAccum<SumAccum<int64_t>>& gGainP_p, ArrayAccum<SumAccum<int64_t>>& gGainQ_p, ArrayAccum<ListAccum<T_GainP>>& gGainP, ArrayAccum<ListAccum<T_GainQ>>& gGainQ,
              // HeapAccum<T_GainP_L_matrix, GainP_L_matrix_comp>& gMatrix_GainP_L, HeapAccum<T_GainP_U_matrix, GainP_U_matrix_comp>& gMatrix_GainP_U, HeapAccum<T_vertex_GainP, vertex_GainP_comp>& gVertex_GainP,
              // HeapAccum<T_GainQ_L_matrix, GainQ_L_matrix_comp>& gMatrix_GainQ_L, HeapAccum<T_GainQ_U_matrix, GainQ_U_matrix_comp>& gMatrix_GainQ_U, HeapAccum<T_vertex_GainQ, vertex_GainQ_comp>& gVertex_GainQ, ArrayAccum<SumAccum<double>>& gVa, ArrayAccum<SumAccum<double>>& gVm, ArrayAccum<ListAccum<T_HP_transpose>>& gHP_transpose, ArrayAccum<ListAccum<T_HQ_transpose>>& gHQ_transpose) 
			  // {

	// // extern size_t g_si, g_sd, g_sp;
	// printf("\n\n------------------------------------------------------------------------------------------- ");
	// printf("\n Start Running GRAPHLU_fdpf_LU_factorize_only function!\n");
	// std::cout << "GainP Number of rows:" << gGainP.data_.size() << ",\tNumber of nonzeros:" << gnnz_GainP << std::endl;
	// std::cout << "GainQ Number of rows:" << gGainQ.data_.size() << ",\tNumber of nonzeros:" << gnnz_GainQ << std::endl;
	// //std::cout << "Y bus Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gMatrix_all.data_.size() << std::endl;
	// printf("-------------------------------------------------------------------------------------------- \n\n");	
	// // ------------------------------------------------------------------------------------------------
	// // 				Initialize variables and arrays
	// // ------------------------------------------------------------------------------------------------
	
 
	// // Initialize arrays and variables
	// uint__t n_GainP, nnz_GainP, n_GainQ, nnz_GainQ, slackbus; //n_e, nnz_e;
	// int ret, i, j; // p, iter;
	// SGraphLU *graphlu_GainP, *graphlu_GainQ;
  
	// string result = "FAILED";

	// // Get the dimension, n, and the nnz of the matrix GainP and GainQ
	// n_GainP = gGainP.data_.size();	nnz_GainP = gnnz_GainP; n_GainQ = gGainQ.data_.size(); nnz_GainQ = gnnz_GainQ;
	// slackbus = gslackbus;
	// //n_e=gVertex.data_.size(); 	nnz_e=gMatrix_all.data_.size(); // get the size of the Y bus matrix
	
	// //// Convert vectors to GRAPHLU Data Structure (pointers)
	// real__t *ax_GainP, *ax_GainQ; //*deltaP, *deltaQ, *Vm, *Va; *Pn, *Qn; *eG, *eB,
	// uint__t *ai_GainP, *ai_GainQ; // *ei, *piv; *btype; 
	// //uint__t *ap, *ap_pp, *ep;
    // uint__t *ap_GainP, *ap_GainQ; 	
 	
	// ax_GainP = (real__t *)malloc(sizeof(real__t)*(nnz_GainP)); // values in B' 
	// ai_GainP = (uint__t *)malloc(sizeof(uint__t)*(nnz_GainP)); // column indices of B'
	// ap_GainP = (uint__t *)malloc(sizeof(uint__t)*(n_GainP+1)); // initial row pointers
	// // b = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b

	// ax_GainQ = (real__t *)malloc(sizeof(real__t)*(nnz_GainQ)); // values in B"
	// ai_GainQ = (uint__t *)malloc(sizeof(uint__t)*(nnz_GainQ)); // column indices of B"
	// ap_GainQ = (uint__t *)malloc(sizeof(uint__t)*(n_GainQ+1)); // initial row pointers

  
	// std::cout << " ======================== Initialization of ararys used to store the factorized matrix and LU ========================"<< std::endl;

	// struct timeval t2_st, t2_end, t3_st, t3_end; long seconds, useconds;
	
	// // =================================== Sort both GainP and GainQ =================================
	// gettimeofday(&t3_st, 0);
	// gettimeofday(&t2_st, 0);
	// // Input ArrayAccums' ListAccums are assumed to be unsorted. Sort them before assigning values to local arrays
    // struct GainP_sort
	// {
            // inline bool operator() (const T_GainP& tuple1, const T_GainP& tuple2)
            // {
                // return (tuple1.index < tuple2.index);
            // }
	// };
	
    // for(i=0;i<gGainP.data_.size(); ++i){
	   // std::sort(gGainP.data_[i].begin(), gGainP.data_[i].end(), GainP_sort());
    // }
	
	// gettimeofday(&t2_end, 0);
	// seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	// useconds = t2_end.tv_usec  - t2_st.tv_usec;
	// printf("\n\n============================================================================================== ");
	// std::cout << "Time to sort GainP ArrayAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	// printf("\n================================================================================================ "); 
	
	
	// gettimeofday(&t2_st, 0); 
	
    // struct GainQ_sort
	// {
            // inline bool operator() (const T_GainQ& tuple3, const T_GainQ& tuple4)
            // {
                // return (tuple3.index < tuple4.index);
            // }
	// };
	
    // for(i=0;i<gGainQ.data_.size(); ++i){
	   // std::sort(gGainQ.data_[i].begin(), gGainQ.data_[i].end(), GainQ_sort());
    // }
	
 	// gettimeofday(&t2_end, 0);
	// seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	// useconds = t2_end.tv_usec  - t2_st.tv_usec;
	// printf("\n\n============================================================================================== ");
	// std::cout << "Time to sort GainQ ArrayAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	// printf("\n================================================================================================ "); 
    
	// // get the time for sorting all HeapAccums 
	// gettimeofday(&t3_end, 0);
	// seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	// useconds = t3_end.tv_usec  - t3_st.tv_usec;
	// printf("\n\n============================================================================================== ");
	// std::cout << "Time to sort both GainP and GainQ ArrayAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	// printf("\n================================================================================================ "); 
	
	// // ========================================= Convert ap_GainP, ap_GainQ =========================================
	// gettimeofday(&t2_st, 0); 
	// int ii = 1; // counter for the array  
	// // Assign value to the first position of the pointer array 
	// ap_GainP[0] = 0;
	// ap_GainQ[0] = 0;
	// //ep[0] = 0;
	// for (i=0;i<gGainP_p.size();i++){
		// ap_GainP[ii] = gGainP_p.data_[i] + ap_GainP[ii-1];
		// //std::cout<< ii << ",ap_GainP,"<< ap_GainP[ii] <<  std::endl;
		// ii++;
	// }
	
	// ii = 1;
	// for (i=0;i<gGainQ_p.size();i++){
		// ap_GainQ[ii] = gGainQ_p.data_[i] + ap_GainQ[ii-1];
		// //std::cout<< ii << ",ap_GainQ,"<< ap_GainQ[ii] <<  std::endl;
		// ii++;
	// }
 
 	// // ========================================= Convert GainP (ai_GainP, ax_GainP) and GainQ (ai_GainQ, ax_GainQ) =========================================
	// int i_GainP = 0, i_GainQ = 0;
	// for (i=0; i<gGainP.data_.size(); ++i) {
		// for (ii=0; ii<gGainP.data_[i].size(); ++ii) {
	
			// if (gGainP.data_[i].get(ii).index < slackbus){
				// ai_GainP[i_GainP] = gGainP.data_[i].get(ii).index - 1;
			// }
			// else if (gGainP.data_[i].get(ii).index > slackbus){
				// ai_GainP[i_GainP] = gGainP.data_[i].get(ii).index - 2;
			// }
				
			// ax_GainP[i_GainP] = gGainP.data_[i].get(ii).value;
      // //std::cout<< i_GainP << ",ax,"<< ax_GainP[i_GainP] << ",ai,"<< ai_GainP[i_GainP] << std::endl;
			// i_GainP++;
		// }
	// }
 
    
	// for (i=0; i<gGainQ.data_.size(); ++i) {
		// for (ii=0; ii<gGainQ.data_[i].size(); ++ii) {
			// ai_GainQ[i_GainQ] = gGainQ.data_[i].get(ii).index - 1;
			// ax_GainQ[i_GainQ] = gGainQ.data_[i].get(ii).value;
			// //std::cout<< i_GainQ << ",ax,"<< ax_GainQ[i_GainQ] << ",ai,"<< ai_GainQ[i_GainQ] << std::endl;
			// i_GainQ++;
		// }
	// } 	
	// // Done converting all input HeapAccums to arrays
  
	// // Get the time to convert the data between containers and arrays
	// gettimeofday(&t2_end, 0);
	// seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	// useconds = t2_end.tv_usec  - t2_st.tv_usec;
	// printf("\n\n============================================================================================== ");
	// std::cout << "Time to convert data to GRAPHLU time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	// printf("\n================================================================================================ ");		
	
	// // ----------------------------------------------------------------------------------------------------
	// // 								Call GRAPHLU and Factorize GainP Matirx
	// // ----------------------------------------------------------------------------------------------------
	// int error_p = 0, error_pp = 0;
	// gettimeofday(&t2_st, 0);
	// std::cout << "\n ======================== Start factorizing GainP Matirx ======================== \n"<< std::endl;
	// // Initialize the structure for GRAPHLU
	// graphlu_GainP = (SGraphLU *)malloc(sizeof(SGraphLU));
	// GraphLU_Initialize(graphlu_GainP);
	
	// // Create Matrix
	// GraphLU_CreateMatrix(graphlu_GainP, n_GainP, nnz_GainP, ax_GainP, ai_GainP, ap_GainP);
	// // graphlu->cfgf[0] = 1.;
	// // // Set control parameters
	// //graphlu->cfgi[1] = 0; // 0 is no MC64 scaling
	// // 					 // 1 is with MC64 scaling

	// // Analyze (column/row ordering, scaling. For details please refer to GRAPHLU user's guide)
	// GraphLU_Analyze(graphlu_GainP);
	// printf("analysis time: %.8g\n", graphlu_GainP->stat[0]);
	
	// // creates the task scheduler for parallel LU factorization. If you want to run 
	// // parallel factorization or parallel re-factorization, it should be called after GraphLU Analyze.
	// ret = GraphLU_CreateScheduler(graphlu_GainP);
	// printf("time of creating scheduler: %.8g\n", graphlu_GainP->stat[4]);
	// printf("suggestion: %s.\n", ret==0?"parallel":"sequential");
	// //std::cout << "GraphLU MC64 scaling 4th:" << graphlu->cfgi[1] <<std::endl; 
	// // This function creates threads for parallel computation. The second argument (thread)
	// // specifies the number of threads, including the main thread. The last argument (check)
	// // specifies whether to check the number of threads or not.
  // if (!ret){ // parallel factorization 
	  // GraphLU_CreateThreads(graphlu_GainP, 2, TRUE);
	  // printf("total cores: %d, threads created: %d\n", (int)(graphlu_GainP->stat[9]), (int)(graphlu_GainP->cfgi[5]));
	
	  // // This function binds threads to cores (unbind = FALSE) or unbinds threads from cores (unbind = TRUE).
	  // GraphLU_BindThreads(graphlu_GainP, FALSE);
	
	  // // Numerical LU factorization with partial pivoting, parallel
	  // error_p = GraphLU_Factorize_MT(graphlu_GainP);
	  // printf("factorization time: %.8g\n", graphlu_GainP->stat[1]);
    // if (error_p < 0) //there is an error, print the code 
      // std::cout << "GraphLU_Factorize error code:" << error_p <<std::endl;  
  // }   
  // else{  // Sequential factorization
	  // error_p = GraphLU_Factorize(graphlu_GainP);
	  // printf("Factorization time: %.8g\n", graphlu_GainP->stat[1]);
    // if (error_p < 0) //there is an error, print the code 
      // std::cout << "GraphLU_Factorize error code:" << error_p <<std::endl; 
	// }
 
  // real__t *lx_GainP, *ux_GainP; 
  // uint__t *li_GainP, *ui_GainP; 
  // size_t *lp_GainP, *up_GainP; 
 	// //uint__t fctnnz_GainP; // number of nonzeros after the matrix is factorized
	// //size_t l_nnz_GainP, u_nnz_GainP; // number of nonzeros in L and U factors
  // // get the permutation arrays, please refer to graphlu.h for details
  // uint__t *rp_GainP, *cp_GainP, *rpi_GainP, *cpi_GainP; //row (column) permutation, row (column) permutation inverse
  // real__t *rows_GainP, *cols_GainP; //*ldiag, *cscale,
  // //int__t *pivot, *pivot_inv;
  // // rpi = (real__t *)malloc(sizeof(real__t)*(n));
  
  // lx_GainP = ux_GainP = NULL; 
  // li_GainP = ui_GainP = NULL; 
  // lp_GainP = up_GainP = NULL; 

  // int row, col, k;
  // int LUrow, LUcol;
    
  // GraphLU_DumpLU(graphlu_GainP, &lx_GainP, &li_GainP, &lp_GainP, &ux_GainP, &ui_GainP, &up_GainP); 
     
   // // Get the number of nonzeros in the factorized matrix
 	// //fctnnz = graphlu->lu_nnz;
 	// //l_nnz = graphlu->l_nnz;
 	// //u_nnz = graphlu->u_nnz;
 
 	// std::cout << " ======================== Number of Total nonzeros after factorization is: "<< graphlu_GainP->lu_nnz << "========================"<<std::endl;
 	// std::cout << " ======================== Number of nonzeros in L: "<< graphlu_GainP->l_nnz << "========================"<<std::endl;
 	// std::cout << " ======================== Number of nonzeros in U: "<< graphlu_GainP->u_nnz << "========================"<<std::endl;
 	
   // // get the permutation arrays, rp and cp may be different if the matrix is not symmetric
  // rp_GainP = graphlu_GainP->row_perm; // row permutation, permuted row # --> original row #  (original row)
  // rpi_GainP = graphlu_GainP->row_perm_inv;  // row permutation inverse, original row # --> permuted row # (permuted row)
  // cp_GainP = graphlu_GainP->col_perm;
  // cpi_GainP = graphlu_GainP->col_perm_inv;

  // //ldiag = graphlu->ldiag; // diagnal elements
  // //cscale = graphlu->cscale; // sclaing factors
  // rows_GainP = graphlu_GainP->row_scale; 
  // cols_GainP = graphlu_GainP->col_scale_perm;
 
 	// // ================================= Get the pivoting vectors =================================
 	// //for (i=0; i<n; ++i){
 	// //	std::cout<< i << ",\t pivot:"<< pivot[i] << ",\t pivot_inv:"<< pivot_inv[i] << std::endl;
 	// //}

  
    // //get the factorized LU values and find the row and col before permutation
    // for (i = 0; i < n_GainP; ++i){
      // row = rp_GainP[i];
      // LUrow = row+1;
      
      // gVertex_GainP += T_vertex_GainP(i+1, lp_GainP[i+1]-lp_GainP[i], up_GainP[i+1]-up_GainP[i], rp_GainP[i], cpi_GainP[i], rows_GainP[i], cols_GainP[i]); 
      // // process all non-zeros in L matrix on row i
      // for (j = lp_GainP[i]; j < lp_GainP[i+1]; ++j){
        // col = cp_GainP[li_GainP[j]];             
        // LUcol = col+1;
        
        // gMatrix_GainP_L += T_GainP_L_matrix((i+1)*100000+(li_GainP[j]+1), li_GainP[j], lx_GainP[j]); 
      // } 
         
      // // process all non-zeros in U matrix on row i
      // for (j = up_GainP[i]; j < up_GainP[i+1]; ++j){
        // col = cp_GainP[ui_GainP[j]];
        // LUcol = col+1; 
        
        // gMatrix_GainP_U += T_GainP_U_matrix((i+1)*100000+(ui_GainP[j]+1), ui_GainP[j], ux_GainP[j]);          
      // }   
    // }
    // gVertex_GainP.sort_heap();
    // gMatrix_GainP_L.sort_heap();
    // gMatrix_GainP_U.sort_heap();

  
  // //gVertex_p.sort_heap();
  // //gMatrix_p_L.sort_heap();
  // //gMatrix_p_U.sort_heap();
	// // ----------------------------------------------------------------------------------------------------
	// // 								Call GRAPHLU and Factorize GainQ Matirx
	// // ----------------------------------------------------------------------------------------------------
	// std::cout << "\n ======================== Start factorizing GainQ Matirx ======================== \n"<< std::endl;
	// // Initialize the structure for GRAPHLU
	// graphlu_GainQ = (SGraphLU *)malloc(sizeof(SGraphLU));
	// GraphLU_Initialize(graphlu_GainQ);
 
	// // Create Matrix
	// GraphLU_CreateMatrix(graphlu_GainQ, n_GainQ, nnz_GainQ, ax_GainQ, ai_GainQ, ap_GainQ);
	// // graphlu_pp->cfgf[0] = 1.;
	
	// // // Set control parameters
	// //graphlu_pp->cfgi[1] = 0; // 0 is no MC64 scaling
	// // 					 // 1 is with MC64 scaling
 
	// // Analyze (column/row ordering, scaling. For details please refer to GRAPHLU user's guide)
	// GraphLU_Analyze(graphlu_GainQ);
	// printf("analysis time: %.8g\n", graphlu_GainQ->stat[0]);
  
	// // creates the task scheduler for parallel LU factorization. If you want to run 
	// // parallel factorization or parallel re-factorization, it should be called after GraphLU Analyze.
	// ret = GraphLU_CreateScheduler(graphlu_GainQ);
	// printf("time of creating scheduler: %.8g\n", graphlu_GainQ->stat[4]);
	// printf("suggestion: %s.\n", ret==0?"parallel":"sequential");
	
	// // This function creates threads for parallel computation. The second argument (thread)
	// // specifies the number of threads, including the main thread. The last argument (check)
	// // specifies whether to check the number of threads or not.
  // if (!ret){ // parallel factorization
	  // GraphLU_CreateThreads(graphlu_GainQ, 2, TRUE);
	  // printf("total cores: %d, threads created: %d\n", (int)(graphlu_GainQ->stat[9]), (int)(graphlu_GainQ->cfgi[5]));
	
	  // // This function binds threads to cores (unbind = FALSE) or unbinds threads from cores (unbind = TRUE).
	  // GraphLU_BindThreads(graphlu_GainQ, FALSE);
	
	  // // Numerical LU factorization with partial pivoting, parallel
	  // error_pp = GraphLU_Factorize_MT(graphlu_GainQ);
	  // printf("factorization time: %.8g\n", graphlu_GainQ->stat[1]);
    // if (error_pp < 0) //there is an error, print the code 
      // std::cout << "GraphLU_Factorize error code:" << error_pp <<std::endl;      
  // }
  // else{  //Sequential factorization 
	  // error_pp = GraphLU_Factorize(graphlu_GainQ);
    // printf("Factorization time: %.8g\n", graphlu_GainQ->stat[1]);
    // if (error_pp < 0) //there is an error, print the code 
      // std::cout << "GraphLU_Factorize error code:" << error_pp <<std::endl; 
  // }

  // real__t *lx_GainQ, *ux_GainQ; 
  // uint__t *li_GainQ, *ui_GainQ; 
  // size_t *lp_GainQ, *up_GainQ; 
  // //uint__t fctnnz_pp; // number of nonzeros after the matrix is factorized
  // //size_t l_nnz_pp, u_nnz_pp; // number of nonzeros in L and U factors
  // // get the permutation arrays, please refer to graphlu.h for details
  // uint__t *rp_GainQ, *cp_GainQ, *rpi_GainQ, *cpi_GainQ; //row (column) permutation, row (column) permutation inverse
  // real__t *rows_GainQ, *cols_GainQ;  //*ldiag_pp, *cscale_pp,
  // //int__t *pivot_pp, *pivot_inv_pp; 
  
  // lx_GainQ = ux_GainQ = NULL; 
  // li_GainQ = ui_GainQ = NULL; 
  // lp_GainQ = up_GainQ = NULL; 
  
  
  // GraphLU_DumpLU(graphlu_GainQ, &lx_GainQ, &li_GainQ, &lp_GainQ, &ux_GainQ, &ui_GainQ, &up_GainQ); 
   
   // // Get the number of nonzeros in the factorized matrix
 	// //fctnnz_pp = graphlu_pp->lu_nnz;
 	// //l_nnz_pp = graphlu_pp->l_nnz;
 	// //u_nnz_pp = graphlu_pp->u_nnz;
 
 	// std::cout << " ======================== Number of Total nonzeros after factorization is: "<< graphlu_GainQ->lu_nnz << "========================"<<std::endl;
 	// std::cout << " ======================== Number of nonzeros in L: "<< graphlu_GainQ->l_nnz << "========================"<<std::endl;
 	// std::cout << " ======================== Number of nonzeros in U: "<< graphlu_GainQ->u_nnz << "========================"<<std::endl;
 	
  // // get the permutation arrays, rp and cp may be different if the matrix is not symmetric
  // rp_GainQ = graphlu_GainQ->row_perm;
  // rpi_GainQ = graphlu_GainQ->row_perm_inv;
  // cp_GainQ = graphlu_GainQ->col_perm;
  // cpi_GainQ = graphlu_GainQ->col_perm_inv;
  
  // //ldiag_pp = graphlu_pp->ldiag; // diagnal elements
  // //cscale_pp = graphlu_pp->cscale; // sclaing factors
  // rows_GainQ = graphlu_GainQ->row_scale; 
  // cols_GainQ = graphlu_GainQ->col_scale_perm;

 
 	// // ================================= Get the pivoting vectors =================================
 	// //for (i=0; i<n; ++i){
 	// //	std::cout<< i << ",\t pivot_pp:"<< pivot_pp[i] << ",\t pivot_inv_pp:"<< pivot_inv_pp[i] << std::endl;
 	// //}
  
      	
    // //get the factorized LU values and find the row and col before permutation
    // for (i = 0; i < n_GainQ; ++i){
      // row = rp_GainQ[i];
      // LUrow = row+1;
      
      // gVertex_GainQ += T_vertex_GainQ(i+1, lp_GainQ[i+1]-lp_GainQ[i], up_GainQ[i+1]-up_GainQ[i], rp_GainQ[i], cpi_GainQ[i], rows_GainQ[i], cols_GainQ[i]); 
      // // process all non-zeros in L matrix on row i
      // for (j = lp_GainQ[i]; j < lp_GainQ[i+1]; ++j){
        // col = cp_GainQ[li_GainQ[j]];
        // LUcol = col+1;
        
        // gMatrix_GainQ_L += T_GainQ_L_matrix((i+1)*100000+(li_GainQ[j]+1), li_GainQ[j], lx_GainQ[j]); 
      // } 
      
      // // process all non-zeros in U matrix on row i
      // for (j = up_GainQ[i]; j < up_GainQ[i+1]; ++j){
        // col = cp_GainQ[ui_GainQ[j]];
        // LUcol = col+1;
        
        // gMatrix_GainQ_U += T_GainQ_U_matrix((i+1)*100000+(ui_GainQ[j]+1), ui_GainQ[j], ux_GainQ[j]);
      // }    
    // }
    // gVertex_GainQ.sort_heap();
    // gMatrix_GainQ_L.sort_heap();
    // gMatrix_GainQ_U.sort_heap();          

  
  // // Get the time for factorizing B' and B"
	// gettimeofday(&t2_end, 0);
	// seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	// useconds = t2_end.tv_usec  - t2_st.tv_usec;
	
	// printf("\n\n============================================================================================== ");
	// std::cout << "Time for GRAPHLU to factorize GainP and GainQ:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	// printf("\n================================================================================================ ");
  
  // if(error_p >= 0 && error_pp >= 0){
      // result = "Factorization Completed";
      // printf("\n\n============================================================================================== ");
  	  // std::cout << "Factorization Completed" << std::endl;
  	  // printf("\n================================================================================================ "); 
    // //continue to do the iterations only if there is no error from factorization
// //  	printf("\n\n============================================================================================== ");
// //  	std::cout << "\n Start iteratively updating deltaP and deltaQ " << std::endl;
// //  	printf("\n================================================================================================\n ");
// //  



  // } else {
    // //factorization failed
 	  // result = "Factorization FAILED";
    // printf("\n\n============================================================================================== ");
  	// std::cout << "Factorization FAILED" << std::endl;
  	// printf("\n================================================================================================ ");  
  // }
// EXIT:
	// // std::cout << "\n free1 " << std::endl;
	// GraphLU_Destroy(graphlu_GainP);
	// GraphLU_Destroy(graphlu_GainQ);
	// // std::cout << "\n free2 " << std::endl;
	// free(ax_GainP);
	// free(ai_GainP);
	// free(ap_GainP);
	// free(ax_GainQ);
	// free(ai_GainQ);
	// free(ap_GainQ);
	// free(graphlu_GainP);
	// free(graphlu_GainQ);
	// // std::cout << "\n free3 " << std::endl;
		
  
 	// free(lx_GainP);
	// free(li_GainP);
	// free(lp_GainP);
    // free(ux_GainP);
	// free(ui_GainP);
	// free(up_GainP);  
 	// free(lx_GainQ);
	// free(li_GainQ);
	// free(lp_GainQ);
 	// free(ux_GainQ);
	// free(ui_GainQ);
	// free(up_GainQ);    
	
  // printf("\n\n----------------------------------------------------------------------------------------");
	// printf("\t\t End of Running GainMatrix_factorize C function!");
	// printf("\t\t ----------------------------------------------------------------------------------------\n\n");	
 
  // return result;
// }


// ****************** Testing Ends [Chen Yuan] ***************************





// **********************************************************************************************************************************
 // Created by: Chen Yuan, chen.yuan@geirina.net
 // Date: 08/24/2017
 // This code performs to solve state estimation GainP * deltaVa = H_P' * r_P. This version does NOT perform LU factorization. GainP is assumed to be previously factorized and
 // the resulting L and U matrices are taken as inputs. The equation is solved with forward/backward substitution.
 // History: 
 // 08/24/2017: [Chen Yuan] GainP matrix has been previously factorized. Built this function to solve GainP * deltaVa = H_P'*r_P, by employing forward/backward substitution, and update voltage angle, Va. 
 
template <//typename T_deltaP_vertex, typename deltaP_vertex_comp,
          //typename T_vertex_Va,  
          typename GainP_L_matrix, typename GainP_L_matrix_comp,  
          typename GainP_U_matrix, typename GainP_U_matrix_comp,
          typename GainP_LU_vertex, typename GainP_LU_vertex_comp>
inline string SE_solve_GainP (ArrayAccum<SumAccum<double>>& gVa, ArrayAccum<SumAccum<double>>& gH_r_P,
              HeapAccum<GainP_L_matrix, GainP_L_matrix_comp>& gGainP_L, HeapAccum<GainP_U_matrix, GainP_U_matrix_comp>& gGainP_U, HeapAccum<GainP_LU_vertex, GainP_LU_vertex_comp>& gGainP_LU_vertex, double& gmax_dVa) {

	//printf(xxx.c_str());

	// extern size_t g_si, g_sd, g_sp;
	printf("\n\n------------------------------------------------------------------------------------------- ");
	printf("\nStart Running SE_solve_GainP function!\n");
	std::cout << "Lp Number of rows:" << gGainP_LU_vertex.data_.size() << ",\tNumber of nonzeros:" << gGainP_L.size() << std::endl;
	std::cout << "Up Number of rows:" << gGainP_LU_vertex.data_.size() << ",\tNumber of nonzeros:" << gGainP_U.size() << std::endl;
	
	printf("-------------------------------------------------------------------------------------------- \n\n");	
	// ------------------------------------------------------------------------------------------------
	// 				Initialize variables and arrays
	// ------------------------------------------------------------------------------------------------


	// Initialize arrays and variables
	uint__t n, nnz_p_L, nnz_p_U, n_e, nnz_e;  // nnz_pp_L, nnz_pp_U,
	int i, j, p; // ret, iter
	//double maxDeltaP=0, maxDeltaQ=0, max_change_ex=maxchange;

	const double pi_value = 3.141592653589793;

	// Get the dimension and the nnz for the LU of B' and B"
	n=gGainP_LU_vertex.data_.size();	
	nnz_p_L=gGainP_L.size(); nnz_p_U=gGainP_U.size();
	//nnz_pp_L=gMatrix_pp_L.size(); nnz_pp_U=gMatrix_pp_U.size();
	//n_e=gVertex_Va.data_.size(); // nnz_e=gMatrix_Ybus.data_.size(); // get the size of the Y bus matrix


	//// For Y-bus
	double *H_r_P, *Va; // *Pn, *Qn; *eG, *eB, *deltaQ, *Vm, 
	//uint *ei, *piv, *btype; 
	//uint *ep; 

	// For L' and U'
	double *lx, *ux, *rows, *cols; 
	uint *li, *ui, *rp, *cpi; 
	size_t *lp, *up; 
	//uint__t fctnnz; // number of nonzeros after the matrix is factorized
	//size_t l_nnz = gLp_edge.data_.size(), u_nnz = gUp_edge.data_.size(); // number of nonzeros in L and U factors 

	lx = (double *)malloc(sizeof(double)*(nnz_p_L));
	li = (uint *)malloc(sizeof(uint)*(nnz_p_L)); // column indices 
	lp = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer
	ux = (double *)malloc(sizeof(double)*(nnz_p_U));
	ui = (uint *)malloc(sizeof(uint)*(nnz_p_U)); // column indices 
	up = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer 
	rows = (double *)malloc(sizeof(double)*(n));
	cols = (double *)malloc(sizeof(double)*(n));
	rp = (uint *)malloc(sizeof(uint)*(n));
	cpi = (uint *)malloc(sizeof(uint)*(n));


	H_r_P = (double *)malloc(sizeof(double)*(n)); // b in the Ax=b
	//deltaQ = (double *)malloc(sizeof(double)*(n)); // b in the Ax=b

	//Vm = (double *)malloc(sizeof(double)*(n)); 
	Va = (double *)malloc(sizeof(double)*(n)); 

	//Pn = (double *)malloc(sizeof(double)*(n)); 
	//Qn = (double *)malloc(sizeof(double)*(n));

	//btype = (uint *)malloc(sizeof(uint)*(n));

	double sum, diag;
	double *b;  // *delta, 
	//delta = (double *)malloc(sizeof(double)*(n));
	b = (double *)malloc(sizeof(double)*(n));

	std::cout << " ======================== Initialization of ararys used to store the factorized matrix and LU ========================"<< std::endl;

	struct timeval t2_st, t2_end, t3_st, t3_end; long seconds, useconds;

	//initialization to 0
	// [tc] use memset here
	//memset(Vm, 0, sizeof(double)*(n));
	memset(Va, 0, sizeof(double)*(n));
	memset(H_r_P, 0, sizeof(double)*(n));
	//memset(deltaQ, 0, sizeof(double)*(n));


	// ========================================= Convert pointers and the Vertex =========================================
	gettimeofday(&t2_st, 0); 
	int i_p; // i_pp
	int ii = 1; // counter for the array  
	// Assign value to the first position of the pointer array 
	lp[0] = 0;
	up[0] = 0;
	//lp_pp[0] = 0;
	//up_pp[0] = 0;    
	//ep[0] = 0;
	for (i=0; i<gGainP_LU_vertex.size(); ++i){
	//process pointers for the matrices
	lp[ii] = gGainP_LU_vertex.data_[i].Lp + lp[ii-1];
	up[ii] = gGainP_LU_vertex.data_[i].Up + up[ii-1];
	//lp_pp[ii] = gVertex_pp.data_[i].Lp + lp_pp[ii-1];
	//up_pp[ii] = gVertex_pp.data_[i].Up + up_pp[ii-1];    
	//ep[ii] = gVertex_Ybus.data_[i].ep + ep[ii-1];
	ii++;
	
	Va[i] = gVa.data_[i];
	H_r_P[i] = gH_r_P.data_[i];
	  
	// debug print


	//std::cout<< i << ",deltaP,"<< deltaP[i] << ",deltaQ,"<< deltaQ[i] << std::endl;  

	//process factorized information for B'
	//i_p = gVertex_p.data_[i].exId - 1;
	rows[i]= gGainP_LU_vertex.data_[i].row_scaling;
	cols[i]= gGainP_LU_vertex.data_[i].col_scaling;
	rp[i] = gGainP_LU_vertex.data_[i].rp;
	cpi[i] = gGainP_LU_vertex.data_[i].cpi;  

	}

	// ========================================= Convert L' and U' =========================================
	//int i_l = 0, i_u = 0;

	for (ii=0; ii<gGainP_L.data_.size(); ++ii) {
	li[ii] = gGainP_L.data_[ii].cpi;
	lx[ii] = gGainP_L.data_[ii].value1; 
	//std::cout<< i << ",li,"<< deltaP[i] << ",deltaQ,"<< deltaQ[i] << std::endl;  	
	} 

	for (ii=0; ii<gGainP_U.data_.size(); ++ii) {  
	ui[ii] = gGainP_U.data_[ii].cpi;
	ux[ii] = gGainP_U.data_[ii].value1; 
	//std::cout<< i << ",deltaP,"<< deltaP[i] << ",deltaQ,"<< deltaQ[i] << std::endl;  	
	} 


	// Done converting all input HeapAccums to arrays

	// Get the time to convert the data between containers and arrays
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;

	printf("\n\n============================================================================================== ");
	std::cout << "Time to convert data to GRAPHLU time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");		

	//continue to do the iterations only if there is no error from factorization
	printf("\n\n============================================================================================== ");
	std::cout << "\n Start iteratively updating deltaP and deltaQ " << std::endl;
	printf("\n================================================================================================\n ");

	gettimeofday(&t2_st, 0);

 		// solve for deltaVa
 		// std::cout << " ======================== Solve for V angle ========================"<<std::endl;
     
    // A*x = b
    // multiply deltaP with row scaling (rows) to get b
   	for (i=0; i<n; ++i){
      b[i]=H_r_P[rp[i]]*rows[rp[i]];
   	}
    
   	// Full forward substitution 
   	for (i=0; i<n; ++i)
   	{
   		sum=0.;
   		diag=0.;
   		for(p=lp[i]; p< lp[i+1]; ++p){
   			if (i!=li[p]){ // if it is not diagnal element
   				j=li[p];
   				sum += lx[p] * b[j];
   			}
   			else{
   				diag = lx[p];
   			}
   		}
   		b[i] -= sum; 
   		b[i] /= diag; 
   	} 
    
   	// Full backward substitution 
   	for (i=n-1; i>=0; --i)
   	{
   		sum=0.;
   		for(p=up[i]; p< up[i+1]; ++p){
   			if (i!=ui[p]){ // if it is not diagnal element
   				sum += ux[p] * b[ui[p]]; 
   			}
   		}
   
   		b[i] -= sum; 
		
		//if(gmax_dVa < abs(b[i]*cols[i]))
		//{
		//	gmax_dVa = abs(b[i]*cols[i]);
		//}
		//std::cout << "gmax_dVa: " << gmax_dVa << std::endl;
		
   	}      
   	// permute the array back to the original order
   	//for (i=0; i<n; ++i)
     //{
   	//	delta[i]=b[cpi[i]];
     //  //std::cout << "delta: " << i << ", " << delta[i] << std::endl;           
 		//}
       
    // Update V angle (Va)
 		for (i=0; i<n; ++i)
		{ 
      //Va[i] -= delta[i];  
			Va[i] += b[cpi[i]]*cols[cpi[i]];    // x(k+1) = x(k) + delta_x 
	  	
			if(gmax_dVa < abs(b[cpi[i]]*cols[cpi[i]]))
			{
				gmax_dVa = abs(b[cpi[i]]*cols[cpi[i]]);
			}
			//std::cout << "gmax_dVa: " << gmax_dVa << std::endl;
	  
	  
		}
    
 	gettimeofday(&t2_end, 0);
 	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
 	useconds = t2_end.tv_usec  - t2_st.tv_usec;
 	
 	printf("\n\n============================================================================================== ");
 	std::cout << "Time for solving V angle: " << (seconds*1000000 + useconds) << " us" << std::endl;
 	printf("\n================================================================================================ ");
 
  string result = "Finished Va Forward/Backward Substitution";
 	// ----------------------------------------------------------------------------------------------------
 	// 								Store the Result back to the array
 	// ----------------------------------------------------------------------------------------------------
 	// Get solved Vm and Va back into HeapAccum
 	for (ii=0; ii<n; ++ii) {
       gVa.data_[ii] = Va[ii];  // in radian
       //gVertex_Vm.data_[ii].Vm = Vm[ii];
       //std::cout << "Final, " << ii+1 << ", Vm, " << Vm[ii]<< ", Va" << ii+1 << ", " << Va[ii]/pi_value*180<< std::endl;
 	}

  
EXIT:

	free(H_r_P);
	//free(deltaQ);
	//free(Vm);
	free(Va);		
	//free(Pn);
	//free(Qn);	

	free(lx);
	free(li);
	free(lp);
	free(ux);
	free(ui);
	free(up); 
	free(rows);
	free(cols);
	free(rp);
	free(cpi); 


	printf("\n\n----------------------------------------------------------------------------------------");
	printf("\t\t End of Running SE_solve_GainP C function!");
	printf("\t\t ----------------------------------------------------------------------------------------\n\n");	
 
  return result;
}





// **********************************************************************************************************************************
 // Created by: Chen Yuan, chen.yuan@geirina.net
 // Date: 08/25/2017
 // This code performs to solve state estimation GainQ * deltaVm = H_Q' * r_Q. This version does NOT perform LU factorization. GainQ is assumed to be previously factorized and
 // the resulting L and U matrices are taken as inputs. The equation is solved with forward/backward substitution.
 // History: 
 // 08/25/2017: [Chen Yuan] GainP matrix has been previously factorized. Built this function to solve GainQ * deltaVa = H_Q'*r_Q, by employing forward/backward substitution, and update voltage angle, Vm. 
 
template <//typename T_deltaP_vertex, typename deltaP_vertex_comp,
          //typename T_vertex_Va,  
          typename GainQ_L_matrix, typename GainQ_L_matrix_comp,  
          typename GainQ_U_matrix, typename GainQ_U_matrix_comp,
          typename GainQ_LU_vertex, typename GainQ_LU_vertex_comp>
inline string SE_solve_GainQ (ArrayAccum<SumAccum<double>>& gVm, ArrayAccum<SumAccum<double>>& gH_r_Q,
              HeapAccum<GainQ_L_matrix, GainQ_L_matrix_comp>& gGainQ_L, HeapAccum<GainQ_U_matrix, GainQ_U_matrix_comp>& gGainQ_U, HeapAccum<GainQ_LU_vertex, GainQ_LU_vertex_comp>& gGainQ_LU_vertex, double& gmax_dVm) {


	// extern size_t g_si, g_sd, g_sp;
	printf("\n\n------------------------------------------------------------------------------------------- ");
	printf("\n Start Running SE_solve_GainQ function!\n");
	std::cout << "Lp Number of rows:" << gGainQ_LU_vertex.data_.size() << ",\tNumber of nonzeros:" << gGainQ_L.size() << std::endl;
	std::cout << "Up Number of rows:" << gGainQ_LU_vertex.data_.size() << ",\tNumber of nonzeros:" << gGainQ_U.size() << std::endl;
	
	printf("-------------------------------------------------------------------------------------------- \n\n");	
	// ------------------------------------------------------------------------------------------------
	// 				Initialize variables and arrays
	// ------------------------------------------------------------------------------------------------


	// Initialize arrays and variables
	uint__t n, nnz_p_L, nnz_p_U, n_e, nnz_e;  // nnz_pp_L, nnz_pp_U,
	int i, j, p; // ret, iter
	//double maxDeltaP=0, maxDeltaQ=0, max_change_ex=maxchange;

	const double pi_value = 3.141592653589793;

	// Get the dimension and the nnz for the LU of B' and B"
	n=gGainQ_LU_vertex.data_.size();	
	nnz_p_L=gGainQ_L.size(); nnz_p_U=gGainQ_U.size();
	//nnz_pp_L=gMatrix_pp_L.size(); nnz_pp_U=gMatrix_pp_U.size();
	//n_e=gVertex_Va.data_.size(); // nnz_e=gMatrix_Ybus.data_.size(); // get the size of the Y bus matrix


	//// For Y-bus
	double *H_r_Q, *Vm; // *Pn, *Qn; *eG, *eB, *deltaQ, *Vm, 
	//uint *ei, *piv, *btype; 
	//uint *ep; 

	// For L' and U'
	double *lx, *ux, *rows, *cols; 
	uint *li, *ui, *rp, *cpi; 
	size_t *lp, *up; 
	//uint__t fctnnz; // number of nonzeros after the matrix is factorized
	//size_t l_nnz = gLp_edge.data_.size(), u_nnz = gUp_edge.data_.size(); // number of nonzeros in L and U factors 

	lx = (double *)malloc(sizeof(double)*(nnz_p_L));
	li = (uint *)malloc(sizeof(uint)*(nnz_p_L)); // column indices 
	lp = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer
	ux = (double *)malloc(sizeof(double)*(nnz_p_U));
	ui = (uint *)malloc(sizeof(uint)*(nnz_p_U)); // column indices 
	up = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer 
	rows = (double *)malloc(sizeof(double)*(n));
	cols = (double *)malloc(sizeof(double)*(n));
	rp = (uint *)malloc(sizeof(uint)*(n));
	cpi = (uint *)malloc(sizeof(uint)*(n));


	H_r_Q = (double *)malloc(sizeof(double)*(n)); // b in the Ax=b
	//deltaQ = (double *)malloc(sizeof(double)*(n)); // b in the Ax=b

	//Vm = (double *)malloc(sizeof(double)*(n)); 
	Vm = (double *)malloc(sizeof(double)*(n)); 

	//Pn = (double *)malloc(sizeof(double)*(n)); 
	//Qn = (double *)malloc(sizeof(double)*(n));

	//btype = (uint *)malloc(sizeof(uint)*(n));

	double sum, diag;
	double *b;  // *delta, 
	//delta = (double *)malloc(sizeof(double)*(n));
	b = (double *)malloc(sizeof(double)*(n));

	std::cout << " ======================== Initialization of ararys used to store the factorized matrix and LU ========================"<< std::endl;

	struct timeval t2_st, t2_end, t3_st, t3_end; long seconds, useconds;

	//initialization to 0
	// [tc] use memset here
	//memset(Vm, 0, sizeof(double)*(n));
	memset(Vm, 0, sizeof(double)*(n));
	memset(H_r_Q, 0, sizeof(double)*(n));
	//memset(deltaQ, 0, sizeof(double)*(n));


	// ========================================= Convert pointers and the Vertex =========================================
	gettimeofday(&t2_st, 0); 
	int i_p; // i_pp
	int ii = 1; // counter for the array  
	// Assign value to the first position of the pointer array 
	lp[0] = 0;
	up[0] = 0;
	//lp_pp[0] = 0;
	//up_pp[0] = 0;    
	//ep[0] = 0;
	for (i=0; i<gGainQ_LU_vertex.size(); ++i){
	//process pointers for the matrices
	lp[ii] = gGainQ_LU_vertex.data_[i].Lp + lp[ii-1];
	up[ii] = gGainQ_LU_vertex.data_[i].Up + up[ii-1];
	//lp_pp[ii] = gVertex_pp.data_[i].Lp + lp_pp[ii-1];
	//up_pp[ii] = gVertex_pp.data_[i].Up + up_pp[ii-1];    
	//ep[ii] = gVertex_Ybus.data_[i].ep + ep[ii-1];
	ii++;
	
	Vm[i] = gVm.data_[i];
	H_r_Q[i] = gH_r_Q.data_[i];
	  
	// debug print


	//std::cout<< i << ",deltaP,"<< deltaP[i] << ",deltaQ,"<< deltaQ[i] << std::endl;  

	//process factorized information for B'
	//i_p = gVertex_p.data_[i].exId - 1;
	rows[i]= gGainQ_LU_vertex.data_[i].row_scaling;
	cols[i]= gGainQ_LU_vertex.data_[i].col_scaling;
	rp[i] = gGainQ_LU_vertex.data_[i].rp;
	cpi[i] = gGainQ_LU_vertex.data_[i].cpi;  

	}

	// ========================================= Convert L' and U' =========================================
	//int i_l = 0, i_u = 0;

	for (ii=0; ii<gGainQ_L.data_.size(); ++ii) {
	li[ii] = gGainQ_L.data_[ii].cpi;
	lx[ii] = gGainQ_L.data_[ii].value1; 
	//std::cout<< i << ",li,"<< deltaP[i] << ",deltaQ,"<< deltaQ[i] << std::endl;  	
	} 

	for (ii=0; ii<gGainQ_U.data_.size(); ++ii) {  
	ui[ii] = gGainQ_U.data_[ii].cpi;
	ux[ii] = gGainQ_U.data_[ii].value1; 
	//std::cout<< i << ",deltaP,"<< deltaP[i] << ",deltaQ,"<< deltaQ[i] << std::endl;  	
	} 


	// Done converting all input HeapAccums to arrays

	// Get the time to convert the data between containers and arrays
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;

	printf("\n\n============================================================================================== ");
	std::cout << "Time to convert data to GRAPHLU time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");		

	//continue to do the iterations only if there is no error from factorization
	printf("\n\n============================================================================================== ");
	std::cout << "\n Start iteratively updating deltaP and deltaQ " << std::endl;
	printf("\n================================================================================================\n ");

	gettimeofday(&t2_st, 0);

 		// solve for deltaVa
 		// std::cout << " ======================== Solve for V angle ========================"<<std::endl;
     
    // A*x = b
    // multiply deltaP with row scaling (rows) to get b
   	for (i=0; i<n; ++i){
      b[i]=H_r_Q[rp[i]]*rows[rp[i]];
   	}
    
   	// Full forward substitution 
   	for (i=0; i<n; ++i)
   	{
   		sum=0.;
   		diag=0.;
   		for(p=lp[i]; p< lp[i+1]; ++p){
   			if (i!=li[p]){ // if it is not diagnal element
   				j=li[p];
   				sum += lx[p] * b[j];
   			}
   			else{
   				diag = lx[p];
   			}
   		}
   		b[i] -= sum; 
   		b[i] /= diag; 
   	} 
    
   	// Full backward substitution 
   	for (i=n-1; i>=0; --i)
   	{
   		sum=0.;
   		for(p=up[i]; p< up[i+1]; ++p){
   			if (i!=ui[p]){ // if it is not diagnal element
   				sum += ux[p] * b[ui[p]]; 
   			}
   		}
   
   		b[i] -= sum; 
		
		if(gmax_dVm < abs(b[i]*cols[i]))
		{
			gmax_dVm = abs(b[i]*cols[i]);
		}
		//std::cout << "gmax_dVm: " << gmax_dVm << std::endl;
		
   	}      
   	// permute the array back to the original order
   	//for (i=0; i<n; ++i)
     //{
   	//	delta[i]=b[cpi[i]];
     //  //std::cout << "delta: " << i << ", " << delta[i] << std::endl;           
 		//}
       
    // Update V angle (Va)
 		for (i=0; i<n; ++i)
		{ 
		  //Va[i] -= delta[i];  
		  Vm[i] += b[cpi[i]]*cols[cpi[i]];    // x(k+1) = x(k) + delta_x
		  if(gmax_dVm < abs(b[cpi[i]]*cols[cpi[i]]))
			{
				gmax_dVm = abs(b[cpi[i]]*cols[cpi[i]]);
			}
			//std::cout << "gmax_dVm: " << gmax_dVm << std::endl;
		  
		  
		}
    
 	gettimeofday(&t2_end, 0);
 	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
 	useconds = t2_end.tv_usec  - t2_st.tv_usec;
 	
 	printf("\n\n============================================================================================== ");
 	std::cout << "Time for solving V angle: " << (seconds*1000000 + useconds) << " us" << std::endl;
 	printf("\n================================================================================================ ");
 
  string result = "Finished Vm Forward/Backward Substitution";
 	// ----------------------------------------------------------------------------------------------------
 	// 								Store the Result back to the array
 	// ----------------------------------------------------------------------------------------------------
 	// Get solved Vm and Va back into HeapAccum
 	for (ii=0; ii<n; ++ii) {
       gVm.data_[ii] = Vm[ii];  // in radian
       //gVertex_Vm.data_[ii].Vm = Vm[ii];
       //std::cout << "Final, " << ii+1 << ", Vm, " << Vm[ii]<< ", Va" << ii+1 << ", " << Va[ii]/pi_value*180<< std::endl;
 	}

  
EXIT:

	free(H_r_Q);
	//free(deltaQ);
	//free(Vm);
	free(Vm);		
	//free(Pn);
	//free(Qn);	

	free(lx);
	free(li);
	free(lp);
	free(ux);
	free(ui);
	free(up); 
	free(rows);
	free(cols);
	free(rp);
	free(cpi); 


	printf("\n\n----------------------------------------------------------------------------------------");
	printf("\t\t End of Running SE_solve_GainQ C function!");
	printf("\t\t ----------------------------------------------------------------------------------------\n\n");	
 
  return result;
}



// **********************************************************************************************************************************
 // Created by: Chen Yuan, chen.yuan@geirina.net
 // Date: 11/14/2017
 // This code performs to solve state estimation GainP * deltaVa = H_P' * r_P. This version does NOT perform LU factorization. GainP is assumed to be previously factorized and
 // the resulting L and U matrices are taken as inputs. The equation is solved with forward/backward substitution.
 // History: 
 // 08/24/2017: [Chen Yuan] GainP matrix has been previously factorized. Built this function to solve GainP * deltaVa = H_P'*r_P, by employing forward/backward substitution, and update voltage angle, Va. 
 
//template <//typename T_deltaP_vertex, typename deltaP_vertex_comp,
          //typename T_vertex_Va,  
          //typename GainP_L_matrix, typename GainP_L_matrix_comp,  
          //typename GainP_U_matrix, typename GainP_U_matrix_comp,
          //typename GainP_LU_vertex, typename GainP_LU_vertex_comp>
//inline string SE_solve_GainP (ArrayAccum<SumAccum<double>>& gVa, ArrayAccum<SumAccum<double>>& gH_r_P,
              //HeapAccum<GainP_L_matrix, GainP_L_matrix_comp>& gGainP_L, HeapAccum<GainP_U_matrix, GainP_U_matrix_comp>& gGainP_U, HeapAccum<GainP_LU_vertex, GainP_LU_vertex_comp>& gGainP_LU_vertex, double& gmax_dVa) {
inline string SE_solve_GainP_static (ArrayAccum<SumAccum<double>>& gVa, ArrayAccum<SumAccum<double>>& gH_r_P, double& gmax_dVa) {

	struct timeval t1_st, t1_end;
	gettimeofday(&t1_st, 0); 
	//printf(xxx.c_str());
	
	// extern size_t g_si, g_sd, g_sp;
	printf("\n\n------------------------------------------------------------------------------------------- ");
	printf("\nStart Running SE_solve_GainP_static function!\n");	
	printf("-------------------------------------------------------------------------------------------- \n\n");	
	// ------------------------------------------------------------------------------------------------
	// 				Initialize variables and arrays
	// ------------------------------------------------------------------------------------------------


	CSRMatrix *L_GainP = SingletonMatrixInterface::getMatrixGainP_L();
	CSRMatrix *U_GainP = SingletonMatrixInterface::getMatrixGainP_U();

		
	
	// For L' and U'
	double *lx, *ux, *rows, *cols; 
	uint *li, *ui, *rp, *cpi; 
	size_t *lp, *up; 
	
	lx = L_GainP->getX();
	li = L_GainP->getI();
	lp = L_GainP->getP();
		
	ux = U_GainP->getX();
	ui = U_GainP->getI();
	up = U_GainP->getP();
	
	rp = L_GainP->getRp();
	cpi = L_GainP->getCpi();
	rows = L_GainP->getRows();
	cols = L_GainP->getCols();	// Initialize arrays and variables
	
	uint__t n, nnz_p_L, nnz_p_U, n_e, nnz_e;  // nnz_pp_L, nnz_pp_U,
	int i, j, p; // ret, iter
	//double maxDeltaP=0, maxDeltaQ=0, max_change_ex=maxchange;

	const double pi_value = 3.141592653589793;

	// Get the dimension and the nnz for the LU of B' and B"
	n = L_GainP->getn();	// get number of rows or vertices
	nnz_p_L=L_GainP->getnnz(); nnz_p_U=U_GainP->getnnz();
	//nnz_pp_L=gMatrix_pp_L.size(); nnz_pp_U=gMatrix_pp_U.size();
	//n_e=gVertex_Va.data_.size(); // nnz_e=gMatrix_Ybus.data_.size(); // get the size of the Y bus matrix


	
	printf("\n\n------------------------------------------------------------------------------------------- ");
	std::cout << "Lp Number of rows:" << n << ",\tNumber of nonzeros:" << nnz_p_L << std::endl;
	std::cout << "Up Number of rows:" << n << ",\tNumber of nonzeros:" << nnz_p_U << std::endl;
	printf("-------------------------------------------------------------------------------------------- \n\n");
	
	
	//// For Y-bus
	double *H_r_P, *Va; // *Pn, *Qn; *eG, *eB, *deltaQ, *Vm, 
	//uint *ei, *piv, *btype; 
	//uint *ep; 

	// For L' and U'
	//double *lx, *ux, *rows, *cols; 
	//uint *li, *ui, *rp, *cpi; 
	//size_t *lp, *up; 
	//uint__t fctnnz; // number of nonzeros after the matrix is factorized
	//size_t l_nnz = gLp_edge.data_.size(), u_nnz = gUp_edge.data_.size(); // number of nonzeros in L and U factors 
	
	// lx = (double *)malloc(sizeof(double)*(nnz_p_L));
	// li = (uint *)malloc(sizeof(uint)*(nnz_p_L)); // column indices 
	// lp = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer
	// ux = (double *)malloc(sizeof(double)*(nnz_p_U));
	// ui = (uint *)malloc(sizeof(uint)*(nnz_p_U)); // column indices 
	// up = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer 
	// rows = (double *)malloc(sizeof(double)*(n));
	// cols = (double *)malloc(sizeof(double)*(n));
	// rp = (uint *)malloc(sizeof(uint)*(n));
	// cpi = (uint *)malloc(sizeof(uint)*(n));


	H_r_P = (double *)malloc(sizeof(double)*(n)); // b in the Ax=b
	//deltaQ = (double *)malloc(sizeof(double)*(n)); // b in the Ax=b

	//Vm = (double *)malloc(sizeof(double)*(n)); 
	Va = (double *)malloc(sizeof(double)*(n)); 

	//Pn = (double *)malloc(sizeof(double)*(n)); 
	//Qn = (double *)malloc(sizeof(double)*(n));

	//btype = (uint *)malloc(sizeof(uint)*(n));

	double sum, diag;
	double *b;  // *delta, 
	//delta = (double *)malloc(sizeof(double)*(n));
	b = (double *)malloc(sizeof(double)*(n));

	std::cout << " ======================== Initialization of ararys used to store the factorized matrix and LU ========================"<< std::endl;

	struct timeval t2_st, t2_end, t3_st, t3_end; long seconds, useconds;

	//initialization to 0
	// [tc] use memset here
	//memset(Vm, 0, sizeof(double)*(n));
	memset(Va, 0, sizeof(double)*(n));
	memset(H_r_P, 0, sizeof(double)*(n));
	//memset(deltaQ, 0, sizeof(double)*(n));


	// ========================================= Convert pointers and the Vertex =========================================
	gettimeofday(&t2_st, 0); 
	//int i_p; // i_pp
	int ii = 1; // counter for the array  
	// Assign value to the first position of the pointer array 
	//lp[0] = 0;
	//up[0] = 0;
	//lp_pp[0] = 0;
	//up_pp[0] = 0;    
	//ep[0] = 0;
	for (i=0; i<n; ++i){
	//process pointers for the matrices
	
	//lp[ii] = gGainP_LU_vertex.data_[i].Lp + lp[ii-1];
	//up[ii] = gGainP_LU_vertex.data_[i].Up + up[ii-1];
	
	//lp_pp[ii] = gVertex_pp.data_[i].Lp + lp_pp[ii-1];
	//up_pp[ii] = gVertex_pp.data_[i].Up + up_pp[ii-1];    
	//ep[ii] = gVertex_Ybus.data_[i].ep + ep[ii-1];
	//ii++;
	
	Va[i] = gVa.data_[i];
	H_r_P[i] = gH_r_P.data_[i];
	  
	// debug print


	//std::cout<< i << ",deltaP,"<< deltaP[i] << ",deltaQ,"<< deltaQ[i] << std::endl;  

	//process factorized information for B'
	//i_p = gVertex_p.data_[i].exId - 1;
	//rows[i]= gGainP_LU_vertex.data_[i].row_scaling;
	//cols[i]= gGainP_LU_vertex.data_[i].col_scaling;
	//rp[i] = gGainP_LU_vertex.data_[i].rp;
	//cpi[i] = gGainP_LU_vertex.data_[i].cpi;  

	}

	// ========================================= Convert L' and U' =========================================
	//int i_l = 0, i_u = 0;

	// for (ii=0; ii<gGainP_L.data_.size(); ++ii) {
	// li[ii] = gGainP_L.data_[ii].cpi;
	// lx[ii] = gGainP_L.data_[ii].value1; 
	// //std::cout<< i << ",li,"<< deltaP[i] << ",deltaQ,"<< deltaQ[i] << std::endl;  	
	// } 

	// for (ii=0; ii<gGainP_U.data_.size(); ++ii) {  
	// ui[ii] = gGainP_U.data_[ii].cpi;
	// ux[ii] = gGainP_U.data_[ii].value1; 
	// //std::cout<< i << ",deltaP,"<< deltaP[i] << ",deltaQ,"<< deltaQ[i] << std::endl;  	
	// } 


	// Done converting all input HeapAccums to arrays

	// Get the time to convert the data between containers and arrays
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;

	printf("\n\n============================================================================================== ");
	std::cout << "Time to convert data to GRAPHLU time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");		

	//continue to do the iterations only if there is no error from factorization
	printf("\n\n============================================================================================== ");
	std::cout << "\n Start iteratively updating deltaP and deltaQ " << std::endl;
	printf("\n================================================================================================\n ");

	gettimeofday(&t2_st, 0);

 		// solve for deltaVa
 		// std::cout << " ======================== Solve for V angle ========================"<<std::endl;
     
    // A*x = b
    // multiply deltaP with row scaling (rows) to get b
   	for (i=0; i<n; ++i){
      b[i]=H_r_P[rp[i]]*rows[rp[i]];
   	}
    
   	// Full forward substitution 
   	for (i=0; i<n; ++i)
   	{
   		sum=0.;
   		diag=0.;
   		for(p=lp[i]; p< lp[i+1]; ++p){
   			if (i!=li[p]){ // if it is not diagnal element
   				j=li[p];
   				sum += lx[p] * b[j];
   			}
   			else{
   				diag = lx[p];
   			}
   		}
   		b[i] -= sum; 
   		b[i] /= diag; 
   	} 
    
   	// Full backward substitution 
   	for (i=n-1; i>=0; --i)
   	{
   		sum=0.;
   		for(p=up[i]; p< up[i+1]; ++p){
   			if (i!=ui[p]){ // if it is not diagnal element
   				sum += ux[p] * b[ui[p]]; 
   			}
   		}
   
   		b[i] -= sum; 
		
		//if(gmax_dVa < abs(b[i]*cols[i]))
		//{
		//	gmax_dVa = abs(b[i]*cols[i]);
		//}
		//std::cout << "gmax_dVa: " << gmax_dVa << std::endl;
		
   	}      
   	// permute the array back to the original order
   	//for (i=0; i<n; ++i)
     //{
   	//	delta[i]=b[cpi[i]];
     //  //std::cout << "delta: " << i << ", " << delta[i] << std::endl;           
 		//}
       
    // Update V angle (Va)
 		for (i=0; i<n; ++i)
		{ 
      //Va[i] -= delta[i];  
			Va[i] += b[cpi[i]]*cols[cpi[i]];    // x(k+1) = x(k) + delta_x 
	  	
			if(gmax_dVa < abs(b[cpi[i]]*cols[cpi[i]]))
			{
				gmax_dVa = abs(b[cpi[i]]*cols[cpi[i]]);
			}
			//std::cout << "gmax_dVa: " << gmax_dVa << std::endl;
	  
	  
		}
    
 	gettimeofday(&t2_end, 0);
 	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
 	useconds = t2_end.tv_usec  - t2_st.tv_usec;
 	
 	printf("\n\n============================================================================================== ");
 	std::cout << "Time for solving V angle: " << (seconds*1000000 + useconds) << " us" << std::endl;
 	printf("\n================================================================================================ ");
 
  string result = "Finished Va Forward/Backward Substitution";
 	// ----------------------------------------------------------------------------------------------------
 	// 								Store the Result back to the array
 	// ----------------------------------------------------------------------------------------------------
 	// Get solved Vm and Va back into HeapAccum
 	for (ii=0; ii<n; ++ii) {
       gVa.data_[ii] = Va[ii];  // in radian
       //gVertex_Vm.data_[ii].Vm = Vm[ii];
       //std::cout << "Final, " << ii+1 << ", Vm, " << Vm[ii]<< ", Va" << ii+1 << ", " << Va[ii]/pi_value*180<< std::endl;
 	}
	gettimeofday(&t1_end, 0);
	seconds=t1_end.tv_sec  - t1_st.tv_sec; 
	useconds = t1_end.tv_usec  - t1_st.tv_usec;

	printf("\n\n============================================================================================== ");
	std::cout << "Time to solve Gain_P_static time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");		

  
EXIT:

	free(H_r_P);
	//free(deltaQ);
	//free(Vm);
	free(Va);
	free(b);
	//free(Pn);
	//free(Qn);	
	//free(L_GainP);
	//free(U_GainP);

	//free(lx);
	//free(li);
	//free(lp);
	//free(ux);
	//free(ui);
	//free(up); 
	//free(rows);
	//free(cols);
	//free(rp);
	//free(cpi); 
	



	printf("\n\n----------------------------------------------------------------------------------------");
	printf("\t\t End of Running SE_solve_GainP_static C function!");
	printf("\t\t ----------------------------------------------------------------------------------------\n\n");	
 
  return result;
}



// **********************************************************************************************************************************
 // Created by: Chen Yuan, chen.yuan@geirina.net
 // Date: 11/15/2017
 // This code performs to solve state estimation GainQ * deltaVm = H_Q' * r_Q. This version does NOT perform LU factorization. GainQ is assumed to be previously factorized and
 // the resulting L and U matrices are taken as inputs. The equation is solved with forward/backward substitution.
 // History: 
 // 08/25/2017: [Chen Yuan] GainP matrix has been previously factorized. Built this function to solve GainQ * deltaVa = H_Q'*r_Q, by employing forward/backward substitution, and update voltage angle, Vm. 
 
//template <//typename T_deltaP_vertex, typename deltaP_vertex_comp,
          //typename T_vertex_Va,  
          //typename GainQ_L_matrix, typename GainQ_L_matrix_comp,  
          //typename GainQ_U_matrix, typename GainQ_U_matrix_comp,
          //typename GainQ_LU_vertex, typename GainQ_LU_vertex_comp>
//inline string SE_solve_GainQ (ArrayAccum<SumAccum<double>>& gVm, ArrayAccum<SumAccum<double>>& gH_r_Q,
              //HeapAccum<GainQ_L_matrix, GainQ_L_matrix_comp>& gGainQ_L, HeapAccum<GainQ_U_matrix, GainQ_U_matrix_comp>& gGainQ_U, HeapAccum<GainQ_LU_vertex, GainQ_LU_vertex_comp>& gGainQ_LU_vertex, double& gmax_dVm) {
inline string SE_solve_GainQ_static (ArrayAccum<SumAccum<double>>& gVm, ArrayAccum<SumAccum<double>>& gH_r_Q, double& gmax_dVm) {

	struct timeval t1_st, t1_end;
	gettimeofday(&t1_st, 0);
	// extern size_t g_si, g_sd, g_sp;
	printf("\n\n------------------------------------------------------------------------------------------- ");
	printf("\n Start Running SE_solve_GainQ_static function!\n");
	printf("-------------------------------------------------------------------------------------------- \n\n");	
	// ------------------------------------------------------------------------------------------------
	// 				Initialize variables and arrays
	// ------------------------------------------------------------------------------------------------
	
	CSRMatrix *L_GainQ = SingletonMatrixInterface::getMatrixGainQ_L();
	CSRMatrix *U_GainQ = SingletonMatrixInterface::getMatrixGainQ_U();
	
	
	// For L' and U'
	double *lx, *ux, *rows, *cols; 
	uint *li, *ui, *rp, *cpi; 
	size_t *lp, *up; 
	
	
	lx = L_GainQ->getX();
	li = L_GainQ->getI();
	lp = L_GainQ->getP();
		
	ux = U_GainQ->getX();
	ui = U_GainQ->getI();
	up = U_GainQ->getP();
	
	rp = L_GainQ->getRp();
	cpi = L_GainQ->getCpi();
	rows = L_GainQ->getRows();
	cols = L_GainQ->getCols();	// Initialize arrays and variables
	
	


	// Initialize arrays and variables
	uint__t n, nnz_p_L, nnz_p_U, n_e, nnz_e;  // nnz_pp_L, nnz_pp_U,
	int i, j, p; // ret, iter
	//double maxDeltaP=0, maxDeltaQ=0, max_change_ex=maxchange;

	const double pi_value = 3.141592653589793;

	// Get the dimension and the nnz for the LU of B' and B"
	n=L_GainQ->getn();	
	nnz_p_L=L_GainQ->getnnz(); nnz_p_U=U_GainQ->getnnz();
	//nnz_pp_L=gMatrix_pp_L.size(); nnz_pp_U=gMatrix_pp_U.size();
	//n_e=gVertex_Va.data_.size(); // nnz_e=gMatrix_Ybus.data_.size(); // get the size of the Y bus matrix

	
	printf("\n\n------------------------------------------------------------------------------------------- ");
	std::cout << "Lp Number of rows:" << n << ",\tNumber of nonzeros:" << nnz_p_L << std::endl;
	std::cout << "Up Number of rows:" << n << ",\tNumber of nonzeros:" << nnz_p_U << std::endl;
	
	printf("-------------------------------------------------------------------------------------------- \n\n");	

	//// For Y-bus
	double *H_r_Q, *Vm; // *Pn, *Qn; *eG, *eB, *deltaQ, *Vm, 
	//uint *ei, *piv, *btype; 
	//uint *ep; 

	// For L' and U'
	//double *lx, *ux, *rows, *cols; 
	//uint *li, *ui, *rp, *cpi; 
	//size_t *lp, *up; 
	//uint__t fctnnz; // number of nonzeros after the matrix is factorized
	//size_t l_nnz = gLp_edge.data_.size(), u_nnz = gUp_edge.data_.size(); // number of nonzeros in L and U factors 

	// lx = (double *)malloc(sizeof(double)*(nnz_p_L));
	// li = (uint *)malloc(sizeof(uint)*(nnz_p_L)); // column indices 
	// lp = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer
	// ux = (double *)malloc(sizeof(double)*(nnz_p_U));
	// ui = (uint *)malloc(sizeof(uint)*(nnz_p_U)); // column indices 
	// up = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer 
	// rows = (double *)malloc(sizeof(double)*(n));
	// cols = (double *)malloc(sizeof(double)*(n));
	// rp = (uint *)malloc(sizeof(uint)*(n));
	// cpi = (uint *)malloc(sizeof(uint)*(n));


	H_r_Q = (double *)malloc(sizeof(double)*(n)); // b in the Ax=b
	//deltaQ = (double *)malloc(sizeof(double)*(n)); // b in the Ax=b

	//Vm = (double *)malloc(sizeof(double)*(n)); 
	Vm = (double *)malloc(sizeof(double)*(n)); 

	//Pn = (double *)malloc(sizeof(double)*(n)); 
	//Qn = (double *)malloc(sizeof(double)*(n));

	//btype = (uint *)malloc(sizeof(uint)*(n));

	double sum, diag;
	double *b;  // *delta, 
	//delta = (double *)malloc(sizeof(double)*(n));
	b = (double *)malloc(sizeof(double)*(n));

	std::cout << " ======================== Initialization of ararys used to store the factorized matrix and LU ========================"<< std::endl;

	struct timeval t2_st, t2_end, t3_st, t3_end; long seconds, useconds;

	//initialization to 0
	// [tc] use memset here
	//memset(Vm, 0, sizeof(double)*(n));
	memset(Vm, 0, sizeof(double)*(n));
	memset(H_r_Q, 0, sizeof(double)*(n));
	//memset(deltaQ, 0, sizeof(double)*(n));


	// ========================================= Convert pointers and the Vertex =========================================
	gettimeofday(&t2_st, 0); 
	//int i_p; // i_pp
	int ii = 1; // counter for the array  
	// Assign value to the first position of the pointer array 
	//lp[0] = 0;
	//up[0] = 0;
	//lp_pp[0] = 0;
	//up_pp[0] = 0;    
	//ep[0] = 0;
	for (i=0; i<n; ++i){
	//process pointers for the matrices
	//lp[ii] = gGainQ_LU_vertex.data_[i].Lp + lp[ii-1];
	//up[ii] = gGainQ_LU_vertex.data_[i].Up + up[ii-1];
	//lp_pp[ii] = gVertex_pp.data_[i].Lp + lp_pp[ii-1];
	//up_pp[ii] = gVertex_pp.data_[i].Up + up_pp[ii-1];    
	//ep[ii] = gVertex_Ybus.data_[i].ep + ep[ii-1];
	//ii++;
	
	Vm[i] = gVm.data_[i];
	H_r_Q[i] = gH_r_Q.data_[i];
	  
	// debug print


	//std::cout<< i << ",deltaP,"<< deltaP[i] << ",deltaQ,"<< deltaQ[i] << std::endl;  

	//process factorized information for B'
	//i_p = gVertex_p.data_[i].exId - 1;
	//rows[i]= gGainQ_LU_vertex.data_[i].row_scaling;
	//cols[i]= gGainQ_LU_vertex.data_[i].col_scaling;
	//rp[i] = gGainQ_LU_vertex.data_[i].rp;
	//cpi[i] = gGainQ_LU_vertex.data_[i].cpi;  

	}

	// ========================================= Convert L' and U' =========================================
	//int i_l = 0, i_u = 0;

	//for (ii=0; ii<gGainQ_L.data_.size(); ++ii) {
	//li[ii] = gGainQ_L.data_[ii].cpi;
	//lx[ii] = gGainQ_L.data_[ii].value1; 
	////std::cout<< i << ",li,"<< deltaP[i] << ",deltaQ,"<< deltaQ[i] << std::endl;  	
	//} 

	//for (ii=0; ii<gGainQ_U.data_.size(); ++ii) {  
	//ui[ii] = gGainQ_U.data_[ii].cpi;
	//ux[ii] = gGainQ_U.data_[ii].value1; 
	////std::cout<< i << ",deltaP,"<< deltaP[i] << ",deltaQ,"<< deltaQ[i] << std::endl;  	
	//} 


	// Done converting all input HeapAccums to arrays

	// Get the time to convert the data between containers and arrays
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;

	printf("\n\n============================================================================================== ");
	std::cout << "Time to convert data to GRAPHLU time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");		

	//continue to do the iterations only if there is no error from factorization
	printf("\n\n============================================================================================== ");
	std::cout << "\n Start iteratively updating deltaP and deltaQ " << std::endl;
	printf("\n================================================================================================\n ");

	gettimeofday(&t2_st, 0);

 		// solve for deltaVa
 		// std::cout << " ======================== Solve for V angle ========================"<<std::endl;
     
    // A*x = b
    // multiply deltaP with row scaling (rows) to get b
   	for (i=0; i<n; ++i){
      b[i]=H_r_Q[rp[i]]*rows[rp[i]];
   	}
    
   	// Full forward substitution 
   	for (i=0; i<n; ++i)
   	{
   		sum=0.;
   		diag=0.;
   		for(p=lp[i]; p< lp[i+1]; ++p){
   			if (i!=li[p]){ // if it is not diagnal element
   				j=li[p];
   				sum += lx[p] * b[j];
   			}
   			else{
   				diag = lx[p];
   			}
   		}
   		b[i] -= sum; 
   		b[i] /= diag; 
   	} 
    
   	// Full backward substitution 
   	for (i=n-1; i>=0; --i)
   	{
   		sum=0.;
   		for(p=up[i]; p< up[i+1]; ++p){
   			if (i!=ui[p]){ // if it is not diagnal element
   				sum += ux[p] * b[ui[p]]; 
   			}
   		}
   
   		b[i] -= sum; 
		
		if(gmax_dVm < abs(b[i]*cols[i]))
		{
			gmax_dVm = abs(b[i]*cols[i]);
		}
		//std::cout << "gmax_dVm: " << gmax_dVm << std::endl;
		
   	}      
   	// permute the array back to the original order
   	//for (i=0; i<n; ++i)
     //{
   	//	delta[i]=b[cpi[i]];
     //  //std::cout << "delta: " << i << ", " << delta[i] << std::endl;           
 		//}
       
    // Update V angle (Va)
 		for (i=0; i<n; ++i)
		{ 
		  //Va[i] -= delta[i];  
		  Vm[i] += b[cpi[i]]*cols[cpi[i]];    // x(k+1) = x(k) + delta_x
		  if(gmax_dVm < abs(b[cpi[i]]*cols[cpi[i]]))
			{
				gmax_dVm = abs(b[cpi[i]]*cols[cpi[i]]);
			}
			//std::cout << "gmax_dVm: " << gmax_dVm << std::endl;
		  
		  
		}
    
 	gettimeofday(&t2_end, 0);
 	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
 	useconds = t2_end.tv_usec  - t2_st.tv_usec;
 	
 	printf("\n\n============================================================================================== ");
 	std::cout << "Time for solving V angle: " << (seconds*1000000 + useconds) << " us" << std::endl;
 	printf("\n================================================================================================ ");
 
  string result = "Finished Vm Forward/Backward Substitution";
 	// ----------------------------------------------------------------------------------------------------
 	// 								Store the Result back to the array
 	// ----------------------------------------------------------------------------------------------------
 	// Get solved Vm and Va back into HeapAccum
 	for (ii=0; ii<n; ++ii) {
       gVm.data_[ii] = Vm[ii];  // in radian
       //gVertex_Vm.data_[ii].Vm = Vm[ii];
       //std::cout << "Final, " << ii+1 << ", Vm, " << Vm[ii]<< ", Va" << ii+1 << ", " << Va[ii]/pi_value*180<< std::endl;
 	}

	gettimeofday(&t1_end, 0);
	seconds=t1_end.tv_sec  - t1_st.tv_sec; 
	useconds = t1_end.tv_usec  - t1_st.tv_usec;

	printf("\n\n============================================================================================== ");
	std::cout << "Time to solve Gain_Q static time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");		
  
EXIT:

	free(H_r_Q);
	//free(deltaQ);
	//free(Vm);
	free(Vm);		
	//free(Pn);
	//free(Qn);	
	
	//free(L_GainQ);
	//free(U_GainQ);

	//free(lx);
	//free(li);
	//free(lp);
	//free(ux);
	//free(ui);
	//free(up); 
	//free(rows);
	//free(cols);
	//free(rp);
	//free(cpi);	


	printf("\n\n----------------------------------------------------------------------------------------");
	printf("\t\t End of Running SE_solve_GainQ_static C function!");
	printf("\t\t ----------------------------------------------------------------------------------------\n\n");	
 
  return result;
}






// **********************************************************************************************************************************
// Created by: Chen Yuan, chen.yuan@geirina.net
// Date: 08/24/2017
// This function takes an index and an ArrayAccum<SumAccum<>> as inputs and returns Va (voltage magnitude) as output. Specified for gVa ArrayAccum<SumAccum<>>
// History:
// **********************************************************************************************************************************

inline double SE_getVa (uint64_t& index, const ArrayAccum<SumAccum<double>> &gVa) {
  return gVa.data_[index];
} 

// **********************************************************************************************************************************
// Created by: Chen Yuan, chen.yuan@geirina.net
// Date: 08/25/2017
// This function takes an index and an ArrayAccum<SumAccum<>> as inputs and returns Vm (voltage magnitude) as output. Specified for gVm ArrayAccum<SumAccum<>>
// History:
// **********************************************************************************************************************************

inline double SE_getVm (uint64_t& index, const ArrayAccum<SumAccum<double>> &gVm) {
  return gVm.data_[index];
} 




template <typename T_vertex_tuple, typename vertex_comp>
inline double GRAPHLU_getVm (uint64_t& index, const HeapAccum<T_vertex_tuple, vertex_comp> &gVertex) {
  return gVertex.data_[index-1].Vm;
} 

template <typename T_vertex_tuple, typename vertex_comp>
inline double GRAPHLU_getVm_EMS (uint64_t& index, const HeapAccum<T_vertex_tuple, vertex_comp> &gVertex) {
  return gVertex.data_[index-1].Vm;
} 

// **********************************************************************************************************************************
// Created by: Chen Yuan, chen.yuan@geirina.net
// Date: 04/03/2017
// This function takes an index and an ArrayAccum as inputs and returns Vm (voltage magnitude) as output. Specified for gVertex ArrayAccum
// History:
// **********************************************************************************************************************************
template <typename T_vertex_tuple>
inline double GRAPHLU_array_getVm (uint64_t& index, const ArrayAccum<ListAccum<T_vertex_tuple>> &gVertex) {
  return gVertex.data_[index-1].get(0).Vm;
} 

// **********************************************************************************************************************************
// Created by: Chen Yuan, chen.yuan@geirina.net
// Date: 03/07/2017
// This function takes an index and a HeapAccum as inputs and returns Vm (voltage magnitude) as output. Specified for gVertex_VmDeltaQ
// History:
// **********************************************************************************************************************************
template <typename T_vertex_VmDeltaQ_tuple, typename vertex_VmDeltaQ_comp>
inline double GRAPHLU_getVm1 (uint64_t& index, const HeapAccum<T_vertex_VmDeltaQ_tuple, vertex_VmDeltaQ_comp> &gVertex_VmDeltaQ) {
  return gVertex_VmDeltaQ.data_[index-1].VValue;
} 

// **********************************************************************************************************************************
// Created by: Chen Yuan, chen.yuan@geirina.net
// Date: 04/04/2017
// This function takes an index and an ArrayAccum as inputs and returns Vm (voltage magnitude) as output.Specified for gVertex_Ybus ArrayAccum
// History:
// **********************************************************************************************************************************
template <typename T_vertex_Ybus_tuple>
inline double GRAPHLU_array_getVm1 (uint64_t& index, const ArrayAccum<ListAccum<T_vertex_Ybus_tuple>> &gVertex_Ybus) {
  return gVertex_Ybus.data_[index-1].get(0).Vm;
} 

// **********************************************************************************************************************************
// Created by: Chen Yuan, chen.yuan@geirina.net
// Date: 04/20/2017
// This function takes an index and an ArrayAccum as inputs and returns Vm (voltage magnitude) as output.Specified for gVertex_VmDeltaQ ArrayAccum
// History:
// **********************************************************************************************************************************
template <typename T_vertex_VmDeltaQ_tuple>
inline double GRAPHLU_getVm1_array (uint64_t& index, const ArrayAccum<ListAccum<T_vertex_VmDeltaQ_tuple>> &gVertex_VmDeltaQ) {
  return gVertex_VmDeltaQ.data_[index-1].get(0).VValue;
} 

// **********************************************************************************************************************************
// Created by: Ting Chan, ting.chan@geirina.net
// Date: 9/21/2016
// This function takes an index and a HeapAccum as inputs and returns Va (voltage angle) as output. Specified for gVertex
// History:
// **********************************************************************************************************************************
template <typename T_vertex_tuple, typename vertex_comp>
inline double GRAPHLU_getVa (uint64_t& index, const HeapAccum<T_vertex_tuple, vertex_comp> &gVertex) {
  return gVertex.data_[index-1].Va;
} 


// **********************************************************************************************************************************
// Created by: Ting Chan, ting.chan@geirina.net
// Date: 9/21/2016
// This function takes an index and a HeapAccum as inputs and returns Va (voltage angle) as output. Specified for gVertex
// History:
// **********************************************************************************************************************************
template <typename T_vertex_tuple, typename vertex_comp>
inline double GRAPHLU_getVa_EMS (uint64_t& index, const HeapAccum<T_vertex_tuple, vertex_comp> &gVertex) {
  return gVertex.data_[index-1].Va;
} 

// **********************************************************************************************************************************
// Created by: Qingxin Shi
// Date: 7/12/2018
// This function takes an index and a HeapAccum as inputs and returns Zth (Thevenin equivalent impedance) as output. Specified for gVertex
// History:
// **********************************************************************************************************************************
template <typename T_vertex_tuple, typename vertex_comp>
inline double GRAPHLU_getZth_EMS (uint64_t& index, const HeapAccum<T_vertex_tuple, vertex_comp> &gVertex) {
  return gVertex.data_[index-1].Zth;
} 

// **********************************************************************************************************************************
// Created by: Qingxin Shi
// Date: 7/12/2018
// This function takes an index and a HeapAccum as inputs and returns STI (sensitivity-based Thevenin index) as output. Specified for gVertex
// History:
// **********************************************************************************************************************************
template <typename T_vertex_tuple, typename vertex_comp>
inline double GRAPHLU_getSTI_EMS (uint64_t& index, const HeapAccum<T_vertex_tuple, vertex_comp> &gVertex) {
  return gVertex.data_[index-1].STI;
} 


// **********************************************************************************************************************************
// Created by: Chen Yuan, chen.yuan@geirina.net
// Date: 04/03/2017
// This function takes an index and an ArrayAccum as inputs and returns Va (voltage magnitude) as output. Specified for gVertex ArrayAccum
// History:
// **********************************************************************************************************************************
template <typename T_vertex_tuple>
inline double GRAPHLU_array_getVa (uint64_t& index, const ArrayAccum<ListAccum<T_vertex_tuple>> &gVertex) {
  return gVertex.data_[index-1].get(0).Va;
} 


// **********************************************************************************************************************************
// Created by: Chen Yuan, chen.yuan@geirina.net
// Date: 03/07/2017
// This function takes an index and a HeapAccum as inputs and returns Va (voltage magnitude) as output. Specified for gVertex_Ybus
// History:
// **********************************************************************************************************************************
template <typename T_vertex_VaDeltaP_tuple, typename vertex_VaDeltaP_comp>
inline double GRAPHLU_getVa1 (uint64_t& index, const HeapAccum<T_vertex_VaDeltaP_tuple, vertex_VaDeltaP_comp> &gVertex_VaDeltaP) {
  return gVertex_VaDeltaP.data_[index-1].VValue;
} 

// **********************************************************************************************************************************
// Created by: Chen Yuan, chen.yuan@geirina.net
// Date: 04/04/2017
// This function takes an index and an arrayaccum as inputs and returns Va (voltage magnitude) as output. Specified for gVertex_Ybus ArrayAccum
// History:
// **********************************************************************************************************************************
template <typename T_vertex_Ybus_tuple>
inline double GRAPHLU_array_getVa1 (uint64_t& index, const ArrayAccum<ListAccum<T_vertex_Ybus_tuple>> &gVertex_Ybus) {
  return gVertex_Ybus.data_[index-1].get(0).Va;
} 

// **********************************************************************************************************************************
// Created by: Chen Yuan, chen.yuan@geirina.net
// Date: 04/20/2017
// This function takes an index and a HeapAccum as inputs and returns Va (voltage magnitude) as output. Specified for gVertex_VaDeltaP ArrayAccum
// History:
// **********************************************************************************************************************************
template <typename T_vertex_VaDeltaP_tuple>
inline double GRAPHLU_getVa1_array (uint64_t& index, const ArrayAccum<ListAccum<T_vertex_VaDeltaP_tuple>> &gVertex_VaDeltaP) {
  return gVertex_VaDeltaP.data_[index-1].get(0).VValue;
} 


// **********************************************************************************************************************************
// Created by: Ting Chan, ting.chan@geirina.net
// Date: 01/18/2017
// This function takes an index and a HeapAccum as inputs and returns Lvalue (factorized value on L matrix diagonal) as output.
// History:
// **********************************************************************************************************************************
template <typename T_vertex_tuple, typename vertex_comp>
inline double getLvalue (uint64_t& index, const HeapAccum<T_vertex_tuple, vertex_comp> &gVertex) {
  return gVertex.data_[index-1].Lvalue;
} 

// **********************************************************************************************************************************
// Created by: Ting Chan, ting.chan@geirina.net
// Date: 01/18/2017
// This function takes an index and a HeapAccum as inputs and returns Uvalue (factorized value on L matrix diagonal) as output.
// History:
// **********************************************************************************************************************************
template <typename T_vertex_tuple, typename vertex_comp>
inline double getUvalue (uint64_t& index, const HeapAccum<T_vertex_tuple, vertex_comp> &gVertex) {
  return gVertex.data_[index-1].Uvalue;
} 

// **********************************************************************************************************************************
// Created by: Ting Chan, ting.chan@geirina.net
// Date: 01/18/2017
// This function takes an index and a HeapAccum as inputs and returns row_scale (row scaling factor) as output.
// History:
// **********************************************************************************************************************************
template <typename T_vertex_tuple, typename vertex_comp>
inline double getRow_scale (uint64_t& index, const HeapAccum<T_vertex_tuple, vertex_comp> &gVertex) {
  //std::cout << "getRow_scale: index = " << index << std::endl;
  return gVertex.data_[index-1].row_scale;
} 

// **********************************************************************************************************************************
// Created by: Ting Chan, ting.chan@geirina.net
// Date: 01/18/2017
// This function takes an index and a HeapAccum as inputs and returns col_scale (column scaling factor) as output.
// History:
// **********************************************************************************************************************************
template <typename T_vertex_tuple, typename vertex_comp>
inline double getCol_scale (uint64_t& index, const HeapAccum<T_vertex_tuple, vertex_comp> &gVertex) {
  return gVertex.data_[index-1].col_scale;
} 

// **********************************************************************************************************************************
// Created by: Ting Chan, ting.chan@geirina.net
// Date: 01/18/2017
// This function takes an index and a HeapAccum as inputs and returns rp (row permutation) as output.
// History:
// **********************************************************************************************************************************
template <typename T_vertex_tuple, typename vertex_comp>
inline int getRp (uint64_t& index, const HeapAccum<T_vertex_tuple, vertex_comp> &gVertex) {
  return gVertex.data_[index-1].rp;
} 

// **********************************************************************************************************************************
// Created by: Ting Chan, ting.chan@geirina.net
// Date: 01/18/2017
// This function takes an index and a HeapAccum as inputs and returns rpi (row permutation inverse) as output.
// History:
// **********************************************************************************************************************************
template <typename T_vertex_tuple, typename vertex_comp>
inline int GRAPHLU_getRpi (uint64_t& index, const HeapAccum<T_vertex_tuple, vertex_comp> &gVertex) {
  return gVertex.data_[index-1].rpi;
}

// **********************************************************************************************************************************
// Created by: Chen Yuan, chen.yuan@geirina.net
// Date: 03/06/2017
// This function takes an index and a HeapAccum as inputs and returns rpi_pp (row permutation inverse) as output.
// History:
// **********************************************************************************************************************************

template <typename T_vertex_tuple, typename vertex_comp>
inline double GRAPHLU_getRpi_pp (uint64_t& index, const HeapAccum<T_vertex_tuple, vertex_comp> &gVertex) {
  return gVertex.data_[index-1].rpi_pp;
} 

// **********************************************************************************************************************************
// Created by: Ting Chan, ting.chan@geirina.net
// Date: 01/26/2017
// This function takes an index and a HeapAccum as inputs and returns Lpointer as output.
// History:
// **********************************************************************************************************************************
template <typename T_vertex_tuple, typename vertex_comp>
inline int getLpointer (uint64_t& index, const HeapAccum<T_vertex_tuple, vertex_comp> &gVertex) {
  return gVertex.data_[index-1].Lpointer;
}

// **********************************************************************************************************************************
// Created by: Ting Chan, ting.chan@geirina.net
// Date: 01/26/2017
// This function takes an index and a HeapAccum as inputs and returns Upointer as output.
// History:
// **********************************************************************************************************************************
template <typename T_vertex_tuple, typename vertex_comp>
inline int getUpointer (uint64_t& index, const HeapAccum<T_vertex_tuple, vertex_comp> &gVertex) {
  return gVertex.data_[index-1].Upointer;
}

// **********************************************************************************************************************************
// Created by: Ting Chan, ting.chan@geirina.net
// Date: 01/26/2017
// This function takes an index and a HeapAccum as inputs and returns Lflag as output.
// History:
// **********************************************************************************************************************************
template <typename T_vertex_tuple, typename vertex_comp>
inline int getLflag (uint64_t& index, const HeapAccum<T_vertex_tuple, vertex_comp> &gVertex) {
  return gVertex.data_[index-1].Lflag;
}

// **********************************************************************************************************************************
// Created by: Ting Chan, ting.chan@geirina.net
// Date: 01/26/2017
// This function takes an index and a HeapAccum as inputs and returns Uflag as output.
// History:
// **********************************************************************************************************************************
template <typename T_vertex_tuple, typename vertex_comp>
inline int getUflag (uint64_t& index, const HeapAccum<T_vertex_tuple, vertex_comp> &gVertex) {
  return gVertex.data_[index-1].Uflag;
}

// **********************************************************************************************************************************
// Created by: Ting Chan, ting.chan@geirina.net
// Date: 01/18/2017
// This function takes an index and a HeapAccum as inputs and returns cp (row permutation inverse) as output.
// History:
// **********************************************************************************************************************************
template <typename T_vertex_tuple, typename vertex_comp>
inline int getCp (uint64_t& index, const HeapAccum<T_vertex_tuple, vertex_comp> &gVertex) {
  return gVertex.data_[index-1].cperm;
}

// **********************************************************************************************************************************
// Created by: Ting Chan, ting.chan@geirina.net
// Date: 01/18/2017
// This function takes an index and a HeapAccum as inputs and returns cpi (row permutation inverse) as output.
// History:
// **********************************************************************************************************************************
template <typename T_vertex_tuple, typename vertex_comp>
inline int getCpi (uint64_t& index, const HeapAccum<T_vertex_tuple, vertex_comp> &gVertex) {
  return gVertex.data_[index-1].cpi;
}

// **********************************************************************************************************************************
// Created by: Yiting Zhao   yitingzhao6777@gmail.com
// Date: 07/14/2017
// This code performs fast-decoupled power flow analysis. It is used as a benchmark to assess the time it takes to run a fast-decoupled
// power flow in C++ with GRAPHLU. This version (v6) takes HeapAccum as inputs instead of MapAccum. The input HeapAcuum is assumed to be
// unsorted, so sorting them is needed before assigning the values to local arrays. Differences between this version (v6) and v2 is 
// the number of input HeapAccum. In this version, only 4 heapAccums are needed (vertex, Bp, Bpp and e). With fewer HeapAccums, the sorting
// time is reduced. This version does not require calling GRAPHLU_build_Ap2 for the pointer structures (i.e. Bp_p, Bpp_p and ep).
// This version (v7) is based on v6 but there are only two input HeapAccums - one for vertex, another one for Ybus, B' and B". 
// History: 
// 07/14/2017: [Yiting Zhao] Factorize the Bp matrix only and return success or fail
// **********************************************************************************************************************************
template <typename T_vertex_tuple, typename vertex_comp, typename T_edge_all_tuple, typename edge_all_comp>
inline string GRAPHLU_Bp_only (int64_t& gBp, HeapAccum<T_vertex_tuple, vertex_comp>& gVertex, 
              HeapAccum<T_edge_all_tuple, edge_all_comp>& gEdge_all) {

    struct timeval t1_st, t1_end, t2_st, t2_end, t3_st, t3_end; long seconds, useconds;
	gettimeofday(&t1_st, 0);
	printf("\n\n------------------------------------------------------------------------------------------- ");
	printf("\nStart Running GRAPHLU_Bp_only C function!\n");
  std::cout << "Bp Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gBp << std::endl;
  // std::cout << "Bpp Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gBpp << std::endl;
  std::cout << "Y bus Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gEdge_all.data_.size() << std::endl;
	printf("-------------------------------------------------------------------------------------------- \n\n");	
	// ------------------------------------------------------------------------------------------------
	// 				Initialize variables and arrays
	// ------------------------------------------------------------------------------------------------
	
	// Initialize arrays and variables
	uint__t n, nnz, n_pp, nnz_pp, n_e, nnz_e;
	
	int ret, i, j, p, iter;

	SGraphLU *graphlu;
  
    string result;

	// Get the dimension and the nnz of the matrix B' and B"
	n=gVertex.data_.size();	nnz=gBp; n_pp=gVertex.data_.size(); 
	
	//// Convert vectors to GRAPHLU Data Structure (pointers)
	real__t *ax; 
	uint__t *ai; 
	uint__t *ap; 

	ax = (real__t *)malloc(sizeof(real__t)*(nnz)); // values in B' 
	ai = (uint__t *)malloc(sizeof(uint__t)*(nnz)); // column indices of B'
	ap = (uint__t *)malloc(sizeof(uint__t)*(n+1)); // initial row pointers

  
	std::cout << " ======================== Initialization of ararys used to store the factorized matrix and LU ========================"<< std::endl;
	
  // =================================== Sort all input HeapAccum =================================
  gettimeofday(&t3_st, 0);
  gettimeofday(&t2_st, 0);
  // Input HeapAccums are assumed to be unsorted. Sort them before assigning values to
  // local arrays
  gVertex.sort_heap();
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gVertex HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
  
  gettimeofday(&t2_st, 0);  
  gEdge_all.sort_heap();
 	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gEdge_all HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
     
  // get the time for sorting the HeapAccums 
	gettimeofday(&t3_end, 0);
	seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	useconds = t3_end.tv_usec  - t3_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort all HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================\n "); 
	
	// ========================================= Convert Ap, Ap_pp, ep and the Vertex =========================================
	gettimeofday(&t2_st, 0); 
  int i_p, i_pp;
	int ii = 1; // counter for the array  
  // Assign value to the first position of the pointer array 
  ap[0] = 0;

	for (i=0;i<gVertex.size();i++){
    ap[ii] = gVertex.data_[i].Bp_p + ap[ii-1];

    ii++;

	}  
 	// ============================ Convert Ybus (ei, eG and eB), B' (ei and Bp_x) and B" (ei and Bpp_x) ==============================
	i_p = 0;
  // i_pp = 0;
  for (ii=0; ii<gEdge_all.size(); ++ii) {

    if(gEdge_all.data_[ii].Bp_x != 0)
    {
      ai[i_p] = gEdge_all.data_[ii].ei;
      ax[i_p] = gEdge_all.data_[ii].Bp_x;
      i_p++;
    }
  } 	
  // Done converting all input HeapAccums to arrays
  
std::cout << "\n ======================== Get Bp input ========================"<<std::endl;
 for (i=0;i<gBp;i++){
     std::cout<< i << ",ax,"<< ax[i] << ",ai,"<< ai[i] << std::endl;
  }
   
  for (i=0; i<n+1; ++i){
 		std::cout<< i << ",ap,"<< ap[i] << std::endl;
 	}
  
	// Get the time to convert the data between containers and arrays
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to convert data to GRAPHLU time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");		
	
	// ----------------------------------------------------------------------------------------------------
	// 								Call GRAPHLU and Factorize B' Matirx
	// ----------------------------------------------------------------------------------------------------
  int error_p = 0;
	gettimeofday(&t2_st, 0);
	std::cout << "\n ======================== Start factorizing B' Matirx ======================== \n"<< std::endl;
	// Initialize the structure for GRAPHLU
	graphlu = (SGraphLU *)malloc(sizeof(SGraphLU));
	
	GraphLU_Initialize(graphlu);

	// Create Matrix
	GraphLU_CreateMatrix(graphlu, n, nnz, ax, ai, ap);
	// graphlu->cfgf[0] = 1.;

	// // Set control parameters
	 graphlu->cfgi[1] = 0; // 0 is no MC64 scaling
	// 					 // 1 is with MC64 scaling

	// Analyze (column/row ordering, scaling. For details please refer to GRAPHLU user's guide)
	GraphLU_Analyze(graphlu);
	printf("analysis time: %.8g\n", graphlu->stat[0]);
	
  // [Ting][begin] comment out if to try sequential factorization 
	// creates the task scheduler for parallel LU factorization. If you want to run 
	// parallel factorization or parallel re-factorization, it should be called after GraphLU Analyze.
	ret = GraphLU_CreateScheduler(graphlu);
	printf("time of creating scheduler: %.8g\n", graphlu->stat[4]);
	printf("suggestion: %s.\n", ret==0?"parallel":"sequential");
	
	// This function creates threads for parallel computation. The second argument (thread)
	// specifies the number of threads, including the main thread. The last argument (check)
	// specifies whether to check the number of threads or not.
  if (!ret){ // parallel factorization 
	  GraphLU_CreateThreads(graphlu, 2, TRUE);
	  printf("total cores: %d, threads created: %d\n", (int)(graphlu->stat[9]), (int)(graphlu->cfgi[5]));
	
	  // This function binds threads to cores (unbind = FALSE) or unbinds threads from cores (unbind = TRUE).
	  GraphLU_BindThreads(graphlu, FALSE);
	
	  // Numerical LU factorization with partial pivoting, parallel
	  error_p = GraphLU_Factorize_MT(graphlu);
	  printf("factorization time: %.8g\n", graphlu->stat[1]);
    if (error_p < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_p <<std::endl;  
  }   
  // [Ting][end] comment out if to try sequential factorization 
  else{
    // Sequential factorization
	  error_p = GraphLU_Factorize(graphlu);
	  printf("Factorization time: %.8g\n", graphlu->stat[1]);
    if (error_p < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_p <<std::endl; 
	}

  std::cout << "\n ======================== Get permuted Bp ========================"<<std::endl;
  GraphLU_DumpA(graphlu, &ax, &ai, &ap);


  for (i=0;i<gBp;i++){
     std::cout<< i << ",Bx,"<< ax[i] << ",Bi,"<< ai[i] << std::endl;
  }
   
  for (i=0; i<n+1; ++i){
 		std::cout<< i << ",Bp,"<< ap[i] << std::endl;
 	}
  
  real__t *lx, *ux; 
  uint__t *li, *ui; 
  size_t *lp, *up; 
  uint__t fctnnz; // number of nonzeros after the matrix is factorized
  size_t l_nnz, u_nnz; // number of nonzeros in L and U factors
  // get the permutation arrays, please refer to graphlu.h for details
  uint__t *rp, *cp, *rpi, *cpi; //row (column) permutation, row (column) permutation inverse
  real__t *ldiag, *cscale, *rows, *cols;
  int__t *pivot, *pivot_inv;
  
  lx = ux = NULL; 
  li = ui = NULL; 
  lp = up = NULL; 
  
    GraphLU_DumpLU(graphlu, &lx, &li, &lp, &ux, &ui, &up); 
  
   // Get the number of nonzeros in the factorized matrix
 	fctnnz = graphlu->lu_nnz;
 	l_nnz = graphlu->l_nnz;
 	u_nnz = graphlu->u_nnz;
	   // get the permutation arrays, rp and cp may be different if the matrix is not symmetric
 	rp = graphlu->row_perm;
    rpi = graphlu->row_perm_inv;
 	cp = graphlu->col_perm;
    cpi = graphlu->col_perm_inv;

    ldiag = graphlu->ldiag; // diagnal elements
    cscale = graphlu->cscale; // sclaing factors
    rows = graphlu->row_scale; 
    cols = graphlu->col_scale_perm;
    pivot = graphlu->pivot; 
    pivot_inv = graphlu->pivot_inv; // pivoting array  
  
    std::cout << "\n ======================== Get Factorized L ========================"<<std::endl;
  // check the factorized L U
     for (i=0; i<l_nnz; ++i){
	 	 std::cout << "Li["<<i<<"] = "<< li[i]  << ", Lx["<<i<<"] = "<< lx[i]<<std::endl;
	 }
	 for (i=0;i<n+1;++i){
		 std::cout << " Lp["<<i<<"] = "<<lp[i]<<std::endl;
	 }
  std::cout << "\n ======================== Get Factorized U ========================"<<std::endl;
	for (i=0; i<u_nnz; ++i){
	 	 std::cout << "Ui["<<i<<"] = "<< ui[i] << ", Ux["<<i<<"] = "<< ux[i]<<std::endl;
	 }
	 for (i=0;i<n+1;++i){
		 std::cout << " Up["<<i<<"] = "<<up[i]<<std::endl;
	 }

	printf("\n\n------------------------------------------------------------------------------------------- ");
    std::cout << "L Number of nnz:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << l_nnz << std::endl;
    std::cout << "U Number of nnz:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << u_nnz << std::endl;
	printf("-------------------------------------------------------------------------------------------- \n\n");	
	 
  // Get the time for factorizing B' and B"
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	
	printf("\n\n============================================================================================== ");
	std::cout << "Time for GRAPHLU to factorize B' and B'':: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");
  
  if(error_p >= 0){
    //factorization failed
 	  result = "OK";
    printf("\n\n============================================================================================== ");
  	std::cout << "Factorization Success! " << std::endl;
  	printf("\n================================================================================================ ");  
  
  } else {
    //factorization failed
 	  result = "failed";
    printf("\n\n============================================================================================== ");
  	std::cout << "Factorization FAILED" << std::endl;
  	printf("\n================================================================================================ ");  
  }
EXIT:
	// std::cout << "\n free1 " << std::endl;
	GraphLU_Destroy(graphlu);

	// std::cout << "\n free2 " << std::endl;
	free(ax);
	free(ai);
	free(ap);

	free(graphlu);

    printf("\n\n----------------------------------------------------------------------------------------");
	printf("\t\t End of Running GRAPHLU_Bp_only function!");
	std::cout << "Time for GRAPHLU_Bp_only: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\t\t ----------------------------------------------------------------------------------------\n\n");		
  return result;
}

// **********************************************************************************************************************************
// Created by: Yiting Zhao   yitingzhao6777@gmail.com
// Date: 07/14/2017
// This code performs fast-decoupled power flow analysis. It is used as a benchmark to assess the time it takes to run a fast-decoupled
// power flow in C++ with GRAPHLU. This version (v6) takes HeapAccum as inputs instead of MapAccum. The input HeapAcuum is assumed to be
// unsorted, so sorting them is needed before assigning the values to local arrays. Differences between this version (v6) and v2 is 
// the number of input HeapAccum. In this version, only 4 heapAccums are needed (vertex, Bp, Bpp and e). With fewer HeapAccums, the sorting
// time is reduced. This version does not require calling GRAPHLU_build_Ap2 for the pointer structures (i.e. Bp_p, Bpp_p and ep).
// This version (v7) is based on v6 but there are only two input HeapAccums - one for vertex, another one for Ybus, B' and B". 
// History: 
// 07/14/2017: [Yiting Zhao] Factorize the Bp matrix only and return success or fail
// **********************************************************************************************************************************
template <typename T_vertex_tuple, typename vertex_comp, typename T_edge_all_tuple, typename edge_all_comp,
          typename T_p_B_matrix, typename p_B_matrix_comp, typename T_p_L_matrix, typename p_L_matrix_comp,
          typename T_p_U_matrix, typename p_U_matrix_comp, typename T_vertex_p, typename vertex_p_comp >
inline string GRAPHLU_Bp_only_static (int64_t& gBp, HeapAccum<T_vertex_tuple, vertex_comp>& gVertex, 
              HeapAccum<T_edge_all_tuple, edge_all_comp>& gEdge_all,
			  HeapAccum<T_p_B_matrix, p_B_matrix_comp>& gMatrix_p_B,
              HeapAccum<T_p_L_matrix, p_L_matrix_comp>& gMatrix_p_L, 
			  HeapAccum<T_p_U_matrix, p_U_matrix_comp>& gMatrix_p_U, 
			  HeapAccum<T_vertex_p, vertex_p_comp>& gVertex_p) {

    struct timeval t1_st, t1_end, t2_st, t2_end, t3_st, t3_end; long seconds, useconds;
	gettimeofday(&t1_st, 0);
	printf("\n\n------------------------------------------------------------------------------------------- ");
	printf("\nStart Running GRAPHLU_Bp_only C function!\n");
  std::cout << "Bp Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gBp << std::endl;
  // std::cout << "Bpp Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gBpp << std::endl;
  std::cout << "Y bus Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gEdge_all.data_.size() << std::endl;
	printf("-------------------------------------------------------------------------------------------- \n\n");	
	// ------------------------------------------------------------------------------------------------
	// 				Initialize variables and arrays
	// ------------------------------------------------------------------------------------------------
	
	// Initialize arrays and variables
	uint__t n, nnz, n_pp, nnz_pp, n_e, nnz_e;
	
	int ret, i, j, p, iter;

	SGraphLU *graphlu;
  
    string result;

	// Get the dimension and the nnz of the matrix B' and B"
	n=gVertex.data_.size();	nnz=gBp; n_pp=gVertex.data_.size(); 
	
	//// Convert vectors to GRAPHLU Data Structure (pointers)
	real__t *ax; 
	uint__t *ai; 
	uint__t *ap; 

	ax = (real__t *)malloc(sizeof(real__t)*(nnz)); // values in B' 
	ai = (uint__t *)malloc(sizeof(uint__t)*(nnz)); // column indices of B'
	ap = (uint__t *)malloc(sizeof(uint__t)*(n+1)); // initial row pointers

  
	std::cout << " ============== Initialization of ararys used to store the factorized matrix and LU ================="<< std::endl;
	
  // =================================== Sort all input HeapAccum =================================
  gettimeofday(&t3_st, 0);
  gettimeofday(&t2_st, 0);
  // Input HeapAccums are assumed to be unsorted. Sort them before assigning values to
  // local arrays
  gVertex.sort_heap();
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gVertex HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
  
  gettimeofday(&t2_st, 0);  
  gEdge_all.sort_heap();
 	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gEdge_all HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
     
  // get the time for sorting the HeapAccums 
	gettimeofday(&t3_end, 0);
	seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	useconds = t3_end.tv_usec  - t3_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort all HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================\n "); 
	
	// ========================================= Convert Ap, Ap_pp, ep and the Vertex =========================================
	gettimeofday(&t2_st, 0); 
  int i_p, i_pp;
	int ii = 1; // counter for the array  
  // Assign value to the first position of the pointer array 
  ap[0] = 0;

	for (i=0;i<gVertex.size();i++){
    ap[ii] = gVertex.data_[i].Bp_p + ap[ii-1];

    ii++;

	}  
 	// ============================ Convert Ybus (ei, eG and eB), B' (ei and Bp_x) and B" (ei and Bpp_x) ==============================
	i_p = 0;
  // i_pp = 0;
  for (ii=0; ii<gEdge_all.size(); ++ii) {

    if(gEdge_all.data_[ii].Bp_x != 0)
    {
      ai[i_p] = gEdge_all.data_[ii].ei;
      ax[i_p] = gEdge_all.data_[ii].Bp_x;
      i_p++;
    }
  } 	
  // Done converting all input HeapAccums to arrays
  
std::cout << "\n ======================== Get Bp input ========================"<<std::endl;
 for (i=0;i<gBp;i++){
     std::cout<< i << ",ax,"<< ax[i] << ",ai,"<< ai[i] << std::endl;
  }
   
  for (i=0; i<n+1; ++i){
 		std::cout<< i << ",ap,"<< ap[i] << std::endl;
 	}
  
	// Get the time to convert the data between containers and arrays
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to convert data to GRAPHLU time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");		
	
	// ----------------------------------------------------------------------------------------------------
	// 								Call GRAPHLU and Factorize B' Matirx
	// ----------------------------------------------------------------------------------------------------
  int error_p = 0;
	gettimeofday(&t2_st, 0);
	std::cout << "\n ======================== Start factorizing B' Matirx ======================== \n"<< std::endl;
	// Initialize the structure for GRAPHLU
	graphlu = (SGraphLU *)malloc(sizeof(SGraphLU));
	
	GraphLU_Initialize(graphlu);

	// Create Matrix
	GraphLU_CreateMatrix(graphlu, n, nnz, ax, ai, ap);
	// graphlu->cfgf[0] = 1.;

	// // Set control parameters
	 graphlu->cfgi[1] = 0; // 0 is no MC64 scaling
	// 					 // 1 is with MC64 scaling

	// Analyze (column/row ordering, scaling. For details please refer to GRAPHLU user's guide)
	GraphLU_Analyze(graphlu);
	printf("analysis time: %.8g\n", graphlu->stat[0]);
	
  // [Ting][begin] comment out if to try sequential factorization 
	// creates the task scheduler for parallel LU factorization. If you want to run 
	// parallel factorization or parallel re-factorization, it should be called after GraphLU Analyze.
	ret = GraphLU_CreateScheduler(graphlu);
	printf("time of creating scheduler: %.8g\n", graphlu->stat[4]);
	printf("suggestion: %s.\n", ret==0?"parallel":"sequential");
	
	// This function creates threads for parallel computation. The second argument (thread)
	// specifies the number of threads, including the main thread. The last argument (check)
	// specifies whether to check the number of threads or not.
  if (!ret){ // parallel factorization 
	  GraphLU_CreateThreads(graphlu, 2, TRUE);
	  printf("total cores: %d, threads created: %d\n", (int)(graphlu->stat[9]), (int)(graphlu->cfgi[5]));
	
	  // This function binds threads to cores (unbind = FALSE) or unbinds threads from cores (unbind = TRUE).
	  GraphLU_BindThreads(graphlu, FALSE);
	
	  // Numerical LU factorization with partial pivoting, parallel
	  error_p = GraphLU_Factorize_MT(graphlu);
	  printf("factorization time: %.8g\n", graphlu->stat[1]);
    if (error_p < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_p <<std::endl;  
  }   
  // [Ting][end] comment out if to try sequential factorization 
  else{
    // Sequential factorization
	  error_p = GraphLU_Factorize(graphlu);
	  printf("Factorization time: %.8g\n", graphlu->stat[1]);
    if (error_p < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_p <<std::endl; 
	}

  std::cout << "\n ======================== Get permuted Bp ========================"<<std::endl;
  GraphLU_DumpA(graphlu, &ax, &ai, &ap);


  for (i=0;i<gBp;i++){
     std::cout<< i << ",Bx,"<< ax[i] << ",Bi,"<< ai[i] << std::endl;
  }
   
  for (i=0; i<n+1; ++i){
 		std::cout<< i << ",Bp,"<< ap[i] << std::endl;
 	}
  
  real__t *lx, *ux; 
  uint__t *li, *ui; 
  size_t *lp, *up; 
  uint__t fctnnz; // number of nonzeros after the matrix is factorized
  size_t l_nnz, u_nnz; // number of nonzeros in L and U factors
  // get the permutation arrays, please refer to graphlu.h for details
  uint__t *rp, *cp, *rpi, *cpi; //row (column) permutation, row (column) permutation inverse
  real__t *ldiag, *cscale, *rows, *cols;
  int__t *pivot, *pivot_inv;
  
  lx = ux = NULL; 
  li = ui = NULL; 
  lp = up = NULL; 
  
    GraphLU_DumpLU(graphlu, &lx, &li, &lp, &ux, &ui, &up); 
  
   // Get the number of nonzeros in the factorized matrix
 	fctnnz = graphlu->lu_nnz;
 	l_nnz = graphlu->l_nnz;
 	u_nnz = graphlu->u_nnz;
	   // get the permutation arrays, rp and cp may be different if the matrix is not symmetric
 	rp = graphlu->row_perm;
    rpi = graphlu->row_perm_inv;
 	cp = graphlu->col_perm;
    cpi = graphlu->col_perm_inv;

    ldiag = graphlu->ldiag; // diagnal elements
    cscale = graphlu->cscale; // sclaing factors
    rows = graphlu->row_scale; 
    cols = graphlu->col_scale_perm;
    pivot = graphlu->pivot; 
    pivot_inv = graphlu->pivot_inv; // pivoting array  
  
    std::cout << "\n ======================== Get Factorized L ========================"<<std::endl;
  // check the factorized L U
     for (i=0; i<l_nnz; ++i){
	 	 std::cout << "Li["<<i<<"] = "<< li[i]  << ", Lx["<<i<<"] = "<< lx[i]<<std::endl;
	 }
	 for (i=0;i<n+1;++i){
		 std::cout << " Lp["<<i<<"] = "<<lp[i]<<std::endl;
	 }
  std::cout << "\n ======================== Get Factorized U ========================"<<std::endl;
	for (i=0; i<u_nnz; ++i){
	 	 std::cout << "Ui["<<i<<"] = "<< ui[i] << ", Ux["<<i<<"] = "<< ux[i]<<std::endl;
	 }
	 for (i=0;i<n+1;++i){
		 std::cout << " Up["<<i<<"] = "<<up[i]<<std::endl;
	 }

	printf("\n\n------------------------------------------------------------------------------------------- ");
    std::cout << "L Number of nnz:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << l_nnz << std::endl;
    std::cout << "U Number of nnz:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << u_nnz << std::endl;
	printf("-------------------------------------------------------------------------------------------- \n\n");	
	 
  // Get the time for factorizing B' and B"
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	
	printf("\n\n============================================================================================== ");
	std::cout << "Time for GRAPHLU to factorize B' and B'':: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");
	
	gettimeofday(&t2_st, 0);
    //Store the factorized and permuted LU values 
    for (i = 0; i < n; ++i){

      gVertex_p += T_vertex_p(i+1, ap[i+1]-ap[i], lp[i+1]-lp[i], up[i+1]-up[i], rp[i], cpi[i]); 
      // process all non-zeros in Bp matrix on row i	  
	  for (j = ap[i]; j < ap[i+1]; ++j){        
        gMatrix_p_B += T_p_B_matrix((i+1)*100000+(ai[j]+1), ai[j], ax[j]); 
      }
	  // process all non-zeros in L matrix on row i
      for (j = lp[i]; j < lp[i+1]; ++j){        
        gMatrix_p_L += T_p_L_matrix((i+1)*100000+(li[j]+1), li[j], lx[j]); 
      }          
      // process all non-zeros in U matrix on row i
      for (j = up[i]; j < up[i+1]; ++j){
        gMatrix_p_U += T_p_U_matrix((i+1)*100000+(ui[j]+1), ui[j], ux[j]);          
      }   
    }
    gVertex_p.sort_heap();
	gMatrix_p_B.sort_heap();
    gMatrix_p_L.sort_heap();
    gMatrix_p_U.sort_heap();
	
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	
	printf("\n\n============================================================================================== ");
	std::cout << "Time for Store the factorized and permuted Bp, LU values:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");
  
	
  
  if(error_p >= 0){
    //factorization failed
 	  result = "OK";
    printf("\n\n============================================================================================== ");
  	std::cout << "Factorization Success! " << std::endl;
  	printf("\n================================================================================================ ");  
  
  } else {
    //factorization failed
 	  result = "failed";
    printf("\n\n============================================================================================== ");
  	std::cout << "Factorization FAILED" << std::endl;
  	printf("\n================================================================================================ ");  
  }
EXIT:
	// std::cout << "\n free1 " << std::endl;
	GraphLU_Destroy(graphlu);

	// std::cout << "\n free2 " << std::endl;
	free(ax);
	free(ai);
	free(ap);

	free(graphlu);

    printf("\n\n----------------------------------------------------------------------------------------");
	printf("\t\t End of Running GRAPHLU_Bp_only_static function!");
	std::cout << "Time for GRAPHLU_Bp_only_static: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\t\t ----------------------------------------------------------------------------------------\n\n");		
  return result;
}	
	
// **********************************************************************************************************************************
// Created by: Chen Yuan, chen.yuan@geirina.net
// Date: 03/02/2017
// This code performs fast-decoupled power flow analysis. It is used as a benchmark to assess the time it takes to run a fast-decoupled
// power flow in C++ with GRAPHLU. This version performs LU factorization on the B' and B" matrix, then solve power flow iterations
// with forward/backward substitution on the LU matrices.
// After performing LU factorization, the resulting B' and B" LU factors are stored in SetAccums. In the query, new edges (fill-ins)
// are created, and existing edges are updated with factorized values. 
// History: 
// 11/15/2016: [Ting Chan] added another SetAccum for Bpp fill-in edges
// 01/09/2017: [Ting Chan] added a control parameter (fillin) to specify whether we want to find the fill-ins.
//             1 - find fill-ins
//             0 - otherwise
// 01/17/2017: [Ting Chan] Replaced GraphLU_Solve with our own implementation of forward/backward substitution. 
// 01/18/2017: [Ting Chan] Added two new HeapAccums to store factorized L values (diagonal), scaling factors, permutation and permutation
//             inverse.
// 01/25/2017: [Ting Chan] Replace input parameter "fillin" by "insert_LU". 
//             1 - Dump LU after factorization and store them in Set/HeapAccums
//             0 - Only factorization is performed 
// 01/26/2017: [Ting Chan] 
//             In an attempt to reduce some complexity, try to store the factorized pointer information onto the vertex as well
// 01/30/2017: [Ting Chan] Fixed forward/backward substitution to use cpi instead of rpi
// 02/01/2017: [Ting Chan] Created to be used with schema version flowSchema_LU7_graphsql44.ddl 
// 02/06/2017: [Ting Chan] Updated logic for storing the pointer information
// 02/06/2017: [Ting Chan] Added the update/insert of edges and attributes for factorized LU using "upsert"
// 02/07/2017: [Ting Chan] Added the update of vertex attributes for factorized LU using "upsert"
// 02/22/2017: [Ting Chan] Added STATIC HeapAccums for storing the factorized LU matrices as inputs. After LU factorization, the LUs 
//             are stored in these HeapAccums.
//             Removed "upsert" and all necessary code for inserting/updating vertex/edge. These are no longer needed as the factorized
//             LU are stored using the STATIC HeapAccums. In sequential runs, solve power flow directly using these HeapAccums. 
// 03/02/2017: [Chen Yuan]
//             Divided the expression funtion into two: factorization and forward/backward substitution. In this part, only factorization left
// 04/27/2017: [Yiting Zhao] Remove MC64 scaling and added the ILU filter 
// **********************************************************************************************************************************
template <typename T_vertex, typename vertex_comp, 
          typename T_matrix_all, typename matrix_all_comp,
		  typename T_p_B_matrix, typename p_B_matrix_comp,
          typename T_p_L_matrix, typename p_L_matrix_comp,
          typename T_p_U_matrix, typename p_U_matrix_comp, 
		  typename T_pp_B_matrix, typename pp_B_matrix_comp,
          typename T_pp_L_matrix, typename pp_L_matrix_comp,
          typename T_pp_U_matrix, typename pp_U_matrix_comp,
          typename T_vertex_p, typename vertex_p_comp,
          typename T_vertex_pp, typename vertex_pp_comp>
inline string GRAPHLU_fdpf_ILU_factorize6 (int64_t& gBp, int64_t& gBpp, 
              HeapAccum<T_vertex, vertex_comp>& gVertex, 
			  HeapAccum<T_matrix_all, matrix_all_comp>& gMatrix_all,
			  HeapAccum<T_p_B_matrix, p_B_matrix_comp>& gMatrix_p_B,
              HeapAccum<T_p_L_matrix, p_L_matrix_comp>& gMatrix_p_L, 
			  HeapAccum<T_p_U_matrix, p_U_matrix_comp>& gMatrix_p_U, 
			  HeapAccum<T_vertex_p, vertex_p_comp>& gVertex_p,
			  HeapAccum<T_pp_B_matrix, pp_B_matrix_comp>& gMatrix_pp_B,
              HeapAccum<T_pp_L_matrix, pp_L_matrix_comp>& gMatrix_pp_L,
			  HeapAccum<T_pp_U_matrix, pp_U_matrix_comp>& gMatrix_pp_U, 
			  HeapAccum<T_vertex_pp, vertex_pp_comp>& gVertex_pp,
			  MaxAccum<double> iLU_threshold )
			  {       
        struct timeval t2_st, t2_end, t3_st, t3_end; long seconds, useconds;
		gettimeofday(&t3_st, 0);

	// extern size_t g_si, g_sd, g_sp;
	printf("\n\n------------------------------------------------------------------------------------------- ");
	printf("\nStart Running GRAPHLU_fdpf_ILU_factorize6 function!\n");
	std::cout << "Bp Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gBp << std::endl;
	std::cout << "Bpp Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gBpp << std::endl;
	std::cout << "Y bus Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gMatrix_all.data_.size() << std::endl;
	printf("-------------------------------------------------------------------------------------------- \n\n");	
	// ------------------------------------------------------------------------------------------------
	// 				Initialize variables and arrays
	// ------------------------------------------------------------------------------------------------
	
 
	// Initialize arrays and variables
	uint__t n, nnz, n_pp, nnz_pp; //n_e, nnz_e;
	int ret, i, j; // p, iter;
	real__t ilu_threshold=iLU_threshold;
	SGraphLU *graphlu, *graphlu_pp;
  
	string result = "FAILED";

	// Get the dimension and the nnz of the matrix B' and B"
	n=gVertex.data_.size();	nnz=gBp; n_pp=gVertex.data_.size(); nnz_pp=gBpp;
	
	//// Convert vectors to GRAPHLU Data Structure (pointers)
	real__t *ax, *ax_pp; //*deltaP, *deltaQ, *Vm, *Va; *Pn, *Qn; *eG, *eB,
	uint__t *ai, *ai_pp; // *ei, *piv; *btype; 
	//uint__t *ap, *ap_pp, *ep;
    uint__t *ap, *ap_pp; 	
	
	ax = (real__t *)malloc(sizeof(real__t)*(nnz)); // values in B' 
	ai = (uint__t *)malloc(sizeof(uint__t)*(nnz)); // column indices of B'
	ap = (uint__t *)malloc(sizeof(uint__t)*(n+1)); // initial row pointers
	// b = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b

	ax_pp = (real__t *)malloc(sizeof(real__t)*(nnz_pp)); // values in B"
	ai_pp = (uint__t *)malloc(sizeof(uint__t)*(nnz_pp)); // column indices of B"
	ap_pp = (uint__t *)malloc(sizeof(uint__t)*(n_pp+1)); // initial row pointers

 
	std::cout << " ======================== Initialization of ararys used to store the factorized matrix and LU ========================"<< std::endl;

	
	
	// =================================== Sort all input HeapAccum =================================
//	gettimeofday(&t3_st, 0);
	gettimeofday(&t2_st, 0);
	// Input HeapAccums are assumed to be unsorted. Sort them before assigning values to
	// local arrays
	gVertex.sort_heap();
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gVertex HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
  
	gettimeofday(&t2_st, 0);  
	gMatrix_all.sort_heap();
 	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gMatrix_all HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
    
	// get the time for sorting all HeapAccums 
/*	gettimeofday(&t3_end, 0);
	seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	useconds = t3_end.tv_usec  - t3_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort all HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
*/	
	// ========================================= Convert Ap, Ap_pp, ep and the Vertex =========================================
	gettimeofday(&t2_st, 0); 
	int i_p, i_pp;
	int ii = 1; // counter for the array  
	// Assign value to the first position of the pointer array 
	ap[0] = 0;
	ap_pp[0] = 0;
	//ep[0] = 0;
	for (i=0;i<gVertex.size();i++){
    ap[ii] = gVertex.data_[i].Bp_p + ap[ii-1];
    //std::cout<< ii << ",ap,"<< ap[ii] << std::endl;
    ap_pp[ii] = gVertex.data_[i].Bpp_p + ap_pp[ii-1];
    //ep[ii] = gVertex.data_[i].ep + ep[ii-1];
    ii++;
	}  
 	// ========================================= Convert Ybus (ei, eG and eB), B' (ei and Bp_x) and B" (ei and Bpp_x) =========================================
	i_p = 0;
	i_pp = 0;
	for (ii=0; ii<gMatrix_all.size(); ++ii) {
    if(gMatrix_all.data_[ii].Bp_x != 0)
    {
      ai[i_p] = gMatrix_all.data_[ii].ei;
      ax[i_p] = gMatrix_all.data_[ii].Bp_x;
      //std::cout<< i_p << ",ax,"<< ax[i_p] << ",ai,"<< ai[i_p] << std::endl;
      i_p++;
    }
    if(gMatrix_all.data_[ii].Bpp_x != 0)
    {
      ai_pp[i_pp] = gMatrix_all.data_[ii].ei;
      ax_pp[i_pp] = gMatrix_all.data_[ii].Bpp_x;
      i_pp++;
    }
  } 	
	// Done converting all input HeapAccums to arrays
  
	// Get the time to convert the data between containers and arrays
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to convert data to GRAPHLU time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");		
	
	// ----------------------------------------------------------------------------------------------------
	// 								Call GRAPHLU and Factorize B' Matirx
	// ----------------------------------------------------------------------------------------------------
  int error_p = 0, error_pp = 0;
	gettimeofday(&t2_st, 0);
	std::cout << "\n ======================== Start factorizing B' Matirx ======================== \n"<< std::endl;
	// Initialize the structure for GRAPHLU
	graphlu = (SGraphLU *)malloc(sizeof(SGraphLU));
	GraphLU_Initialize(graphlu);

	// Create Matrix
	GraphLU_CreateMatrix(graphlu, n, nnz, ax, ai, ap);
	// graphlu->cfgf[0] = 1.;

	// // Set control parameters
	graphlu->cfgi[1] = 0; // 0 is no MC64 scaling
	// 					 // 1 is with MC64 scaling

	// Analyze (column/row ordering, scaling. For details please refer to GRAPHLU user's guide)
	GraphLU_Analyze(graphlu);
	printf("analysis time: %.8g\n", graphlu->stat[0]);

	// creates the task scheduler for parallel LU factorization. If you want to run 
	// parallel factorization or parallel re-factorization, it should be called after GraphLU Analyze.
	ret = GraphLU_CreateScheduler(graphlu);
	printf("time of creating scheduler: %.8g\n", graphlu->stat[4]);
	printf("suggestion: %s.\n", ret==0?"parallel":"sequential");
	//std::cout << "GraphLU MC64 scaling 4th:" << graphlu->cfgi[1] <<std::endl; 
	// This function creates threads for parallel computation. The second argument (thread)
	// specifies the number of threads, including the main thread. The last argument (check)
	// specifies whether to check the number of threads or not.
  if (!ret){ // parallel factorization 
	  GraphLU_CreateThreads(graphlu, 2, TRUE);
	  printf("total cores: %d, threads created: %d\n", (int)(graphlu->stat[9]), (int)(graphlu->cfgi[5]));
	
	  // This function binds threads to cores (unbind = FALSE) or unbinds threads from cores (unbind = TRUE).
	  GraphLU_BindThreads(graphlu, FALSE);
	
	  // Numerical LU factorization with partial pivoting, parallel
	  error_p = GraphLU_Factorize_MT(graphlu);
	  printf("Parallel factorization time: %.8g\n", graphlu->stat[1]);
    if (error_p < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_p <<std::endl;  
   }   
   else{  // Sequential factorization
	   error_p = GraphLU_Factorize(graphlu);
	   printf("Sequential Factorization time: %.8g\n", graphlu->stat[1]);
     if (error_p < 0) //there is an error, print the code 
       std::cout << "GraphLU_Factorize error code:" << error_p <<std::endl; 
	 }
 
  real__t *lx, *ux; 
  uint__t *li, *ui; 
  size_t *lp, *up; 
 	uint__t fctnnz; // number of nonzeros after the matrix is factorized
	size_t l_nnz, u_nnz; // number of nonzeros in L and U factors
  // get the permutation arrays, please refer to graphlu.h for details
	uint__t *rp, *cp, *rpi, *cpi; //row (column) permutation, row (column) permutation inverse
  real__t *ldiag, *cscale, *rows, *cols;
  int__t *pivot, *pivot_inv;
  
  lx = ux = NULL; 
  li = ui = NULL; 
  lp = up = NULL; 

  int row, col, k;
  int LUrow, LUcol;
    
   // Get the number of nonzeros in the factorized matrix
 	fctnnz = graphlu->lu_nnz;
 	l_nnz = graphlu->l_nnz;
 	u_nnz = graphlu->u_nnz;

  // Dump factorized Lp & Up matrix 
  GraphLU_DumpLU(graphlu, &lx, &li, &lp, &ux, &ui, &up);

	// Dump permuted Bp matrix 
  GraphLU_DumpA(graphlu, &ax, &ai, &ap); 
 	
   // get the permutation arrays, rp and cp may be different if the matrix is not symmetric
  rp = graphlu->row_perm;
  rpi = graphlu->row_perm_inv;
  cp = graphlu->col_perm;
  cpi = graphlu->col_perm_inv;

  ldiag = graphlu->ldiag; // diagnal elements
  cscale = graphlu->cscale; // sclaing factors
  // rows = graphlu->row_scale; 
  // cols = graphlu->col_scale_perm;
  pivot = graphlu->pivot; 
  pivot_inv = graphlu->pivot_inv; // pivoting array   
    
	// print out and check
 	// for (i=0; i<n; ++i){ // check the permutation arrays
 	//  	std::cout << i << ",rp," << rp[i] << ",rpi," << rpi[i] << ",cp," << cp[i] << ",cpi," << cpi[i] << std::endl;
 	// } 

/*	 std::cout << " ======================== Get permuted Bp nonzreo elements ========================"<<std::endl;
   for (i=0; i<nnz; ++i){
	 	 std::cout << "Bp_i["<<i<<"] = "<< ai[i]<< " Bp_x["<<i<<"] = "<< ax[i] <<std::endl;
	 }
	 for (i=0;i<n+1;++i){
		 std::cout << " Bp_p["<<i<<"] = "<<ap[i]<<std::endl;
	 }

  std::cout << " ======================== Get L Factor ========================"<<std::endl;
 	for (i=0; i<l_nnz; ++i){
    std::cout << "Lp_i["<<i<<"] = "<< li[i]<< " Lp_x["<<i<<"] = "<< lx[i] <<std::endl; 
  }
   
  for (i=0; i<n+1; ++i){
     std::cout << " Lp_p["<<i<<"] = "<<lp[i]<<std::endl;
  }
   
  std::cout << "\n ======================== Get U Factor ========================"<<std::endl;
 	for (i=0; i<u_nnz; ++i){
     std::cout << "	Up_i["<<i<<"] = "<< ui[i]<< " Up_x["<<i<<"] = "<< ux[i] <<std::endl; 
  }
   
 	for (i=0; i<n+1; ++i){
 		 std::cout << " Up_p["<<i<<"] = "<<up[i]<<std::endl;
 	}
*/
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to Graphlu Factorize Bp: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");		
	
	gettimeofday(&t2_st, 0);
 // ============================= Filter fill-ins in L & U ===================================
	// construct the Bp L and U matrix into unorderes_set for next step
       int__t *lfillins_flag, *ufillins_flag;
	   lfillins_flag = (uint__t *)malloc(sizeof(uint__t)*(l_nnz)); // column indices of B'
	   ufillins_flag = (uint__t *)malloc(sizeof(uint__t)*(u_nnz)); // initial row pointers
	   int ilu_l_nnz = l_nnz;
       int ilu_u_nnz = u_nnz;

	 std :: unordered_set<int> Bp_nofillins;

	 for(int i =0;i<n; i++ )
	 {
		 int arow = i;
		 int astart = ap[i];
		 int aend = ap[i+1];

		 for( int j =astart;j<aend;j++)
		 {
			  int acol =ai[j];
              Bp_nofillins.insert((arow+1)*100000+acol+1);
		 }
	 }

   for (int i =0;i<n;i++)
	 {
		 int lrow = i;
		 int lstart = lp[i];
		 int lend = lp[i+1];
		 for (j = lstart;j<lend;j++)
		 {
			 int lcol = li[j];
			 int lindex = (lrow+1)*100000+lcol+1; 
			 lfillins_flag[j]=Bp_nofillins.count(lindex);
			 //  std::cout << " L_fillin_flag ["<< j << "] = "<< lfillins_flag[j]  << std::endl;
		 }
	 }


	    for (int i =0;i<n;i++)
	 {
		 int urow = i;
		 int ustart = up[i];
		 int uend = up[i+1];
		 for (j = ustart;j<uend;j++)
		 {
			 int ucol = ui[j];
			 int uindex = (urow+1)*100000+ucol+1; 
			 ufillins_flag[j]=Bp_nofillins.count(uindex);
			 // std::cout << " U_fillin_flag ["<< j << "] = "<< ufillins_flag[j]  << std::endl;
		 }
	 }

	 // filter the fillins 
	 for (int i = 0; i<n; i ++)
	 {
		 int lrow = i;
		 int lstart = lp[i];
		 int lend = lp[i+1];
		 // std::cout << " Debug ====== lstart = " << lp[i]  << "; lend = " << lp[i+1] << std::endl;
		 for (j = lstart;j<lend;j++)
		 {
			 int lcol = li[j];
			 double templx = lx[j]/ldiag[lcol];  // LDLT
			 // std::cout << " LDLT: temp Lx["<<j<<"] = "<< templx <<std::endl;
		 if ((lfillins_flag[j] == 0) && (abs(templx))<= ilu_threshold )
		   {
				 lx[j]=0;
				 ilu_l_nnz--;
				 // std::cout << "Lx["<<j<<"] = "<< lx[j]<<std::endl;
			 }
			// std::cout << "filtered Lx["<<j<<"] = "<< lx[j]<<std::endl;
		 }
	 }
	 	 for (int i = 0; i<u_nnz; i ++)
	 {
		 if ((ufillins_flag[i] == 0) && (abs(ux[i]))<= ilu_threshold )
		   {
				 ux[i]=0;
				 ilu_u_nnz--;
				 // std::cout << "Ux["<<i<<"] = "<< ux[i]<<std::endl;
			 }
			  //std::cout << "filtered Ux["<<i<<"] = "<< ux[i]<<std::endl;
	 }
 	// ================================= Get the pivoting vectors =================================
 	//for (i=0; i<n; ++i){
 	//	std::cout<< i << ",\t pivot:"<< pivot[i] << ",\t pivot_inv:"<< pivot_inv[i] << std::endl;
 	//}
	
  	std::cout << " ======================== Number of Total nonzeros after factorization is: "<< graphlu->lu_nnz << "========================"<<std::endl;
 	std::cout << " ======================== Number of nonzeros in L: "<< graphlu->l_nnz << "========================"<<std::endl;
 	std::cout << " ======================== Number of nonzeros in U: "<< graphlu->u_nnz << "========================"<<std::endl;
	std::cout << " ======================== Number of nonzeros in ILU filtered L: "<< ilu_l_nnz << "========================"<<std::endl;
 	std::cout << " ======================== Number of nonzeros in ILU filtered U: "<< ilu_u_nnz << "========================"<<std::endl;
	
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to ILU filter Bp: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");		
	
	gettimeofday(&t2_st, 0);
    //Store the factorized and permuted LU values 
    for (i = 0; i < n; ++i){

      gVertex_p += T_vertex_p(i+1, ap[i+1]-ap[i], lp[i+1]-lp[i], up[i+1]-up[i], rp[i], cpi[i]); 
      // process all non-zeros in Bp matrix on row i	  
	  for (j = ap[i]; j < ap[i+1]; ++j){        
        gMatrix_p_B += T_p_B_matrix((i+1)*100000+(ai[j]+1), ai[j], ax[j]); 
      }
	  // process all non-zeros in L matrix on row i
      for (j = lp[i]; j < lp[i+1]; ++j){        
        gMatrix_p_L += T_p_L_matrix((i+1)*100000+(li[j]+1), li[j], lx[j]); 
      }          
      // process all non-zeros in U matrix on row i
      for (j = up[i]; j < up[i+1]; ++j){
        gMatrix_p_U += T_p_U_matrix((i+1)*100000+(ui[j]+1), ui[j], ux[j]);          
      }   
    }
    gVertex_p.sort_heap();
	gMatrix_p_B.sort_heap();
    gMatrix_p_L.sort_heap();
    gMatrix_p_U.sort_heap();
	
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	
	printf("\n\n============================================================================================== ");
	std::cout << "Time for Store the factorized and permuted Bp, LU values:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");
  
	// ----------------------------------------------------------------------------------------------------
	// 								Call GRAPHLU and Factorize B" Matirx
	// ----------------------------------------------------------------------------------------------------
	gettimeofday(&t2_st, 0);
	
	std::cout << "\n ======================== Start factorizing B'' Matirx ======================== \n"<< std::endl;
	// Initialize the structure for GRAPHLU
	graphlu_pp = (SGraphLU *)malloc(sizeof(SGraphLU));
	GraphLU_Initialize(graphlu_pp);
 
	// Create Matrix
	GraphLU_CreateMatrix(graphlu_pp, n_pp, nnz_pp, ax_pp, ai_pp, ap_pp);
	// graphlu_pp->cfgf[0] = 1.;

	// // Set control parameters
	graphlu_pp->cfgi[1] = 0; // 0 is no MC64 scaling
	// 					 // 1 is with MC64 scaling
 
	// Analyze (column/row ordering, scaling. For details please refer to GRAPHLU user's guide)
	GraphLU_Analyze(graphlu_pp);
	printf("analysis time: %.8g\n", graphlu_pp->stat[0]);
  
	// creates the task scheduler for parallel LU factorization. If you want to run 
	// parallel factorization or parallel re-factorization, it should be called after GraphLU Analyze.
	ret = GraphLU_CreateScheduler(graphlu_pp);
	printf("time of creating scheduler: %.8g\n", graphlu_pp->stat[4]);
	printf("suggestion: %s.\n", ret==0?"parallel":"sequential");
	
	// This function creates threads for parallel computation. The second argument (thread)
	// specifies the number of threads, including the main thread. The last argument (check)
	// specifies whether to check the number of threads or not.
   if (!ret){ // parallel factorization
	  GraphLU_CreateThreads(graphlu_pp, 2, TRUE);
	  printf("total cores: %d, threads created: %d\n", (int)(graphlu_pp->stat[9]), (int)(graphlu_pp->cfgi[5]));
	
	  // This function binds threads to cores (unbind = FALSE) or unbinds threads from cores (unbind = TRUE).
	  GraphLU_BindThreads(graphlu_pp, FALSE);
	
	  // Numerical LU factorization with partial pivoting, parallel
	  error_pp = GraphLU_Factorize_MT(graphlu_pp);
	  printf("Parallel factorization time: %.8g\n", graphlu_pp->stat[1]);
    if (error_pp < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_pp <<std::endl;      
   }
   else{  //Sequential factorization 
	   error_pp = GraphLU_Factorize(graphlu_pp);
     printf("Sequential Factorization time: %.8g\n", graphlu_pp->stat[1]);
     if (error_pp < 0) //there is an error, print the code 
      std::cout << "GraphLU_Factorize error code:" << error_pp <<std::endl; 
   }

    // ----------------------------------------------------------------------------------------------------
	// 								Filter the Lpp and Upp from Bpp Matirx
	// ----------------------------------------------------------------------------------------------------

  real__t *lx_pp, *ux_pp; 
  uint__t *li_pp, *ui_pp; 
  size_t *lp_pp, *up_pp; 
 	uint__t fctnnz_pp; // number of nonzeros after the matrix is factorized
	size_t l_nnz_pp, u_nnz_pp; // number of nonzeros in L and U factors
  // get the permutation arrays, please refer to graphlu.h for details
	uint__t *rp_pp, *cp_pp, *rpi_pp, *cpi_pp; //row (column) permutation, row (column) permutation inverse
  real__t *ldiag_pp, *cscale_pp, *rows_pp, *cols_pp;
  int__t *pivot_pp, *pivot_inv_pp; 
  
  lx_pp = ux_pp = NULL; 
  li_pp = ui_pp = NULL; 
  lp_pp = up_pp = NULL; 
  
  GraphLU_DumpLU(graphlu_pp, &lx_pp, &li_pp, &lp_pp, &ux_pp, &ui_pp, &up_pp); 
   
   	// Dump permuted Bpp matrix 
  GraphLU_DumpA(graphlu_pp, &ax_pp, &ai_pp, &ap_pp); 
   
   // Get the number of nonzeros in the factorized matrix
 	fctnnz_pp = graphlu_pp->lu_nnz;
 	l_nnz_pp = graphlu_pp->l_nnz;
 	u_nnz_pp = graphlu_pp->u_nnz;
 	
  // get the permutation arrays, rp and cp may be different if the matrix is not symmetric
 	rp_pp = graphlu_pp->row_perm;
  rpi_pp = graphlu_pp->row_perm_inv;
 	cp_pp = graphlu_pp->col_perm;
  cpi_pp = graphlu_pp->col_perm_inv;
  
  ldiag_pp = graphlu_pp->ldiag; // diagnal elements
  cscale_pp = graphlu_pp->cscale; // sclaing factors
  rows_pp = graphlu_pp->row_scale; 
  cols_pp = graphlu_pp->col_scale_perm;
  pivot_pp = graphlu_pp->pivot; 
  pivot_inv_pp = graphlu_pp->pivot_inv; // pivoting array   
    
 	// print out and check
 	//for (i=0; i<n; ++i){ // check the permutation arrays
 	 //	std::cout << "rp_pp," << rp_pp[i] << ",rpi_pp," << rpi_pp[i] << ",cp_pp,"<< cp_pp[i] << ",cpi_pp," << cpi_pp[i] << std::endl;
 	// } 
  
  //std::cout << " ======================== Get L Factor ========================"<<std::endl;
 	/*for (i=0; i<l_nnz_pp; ++i){
    std::cout<< i << ",lx_pp,"<< lx_pp[i] << ",li_pp,"<< li_pp[i] << std::endl; 
  }
   
  for (i=0; i<n+1; ++i){
    std::cout<< i << ",lp_pp,"<< lp_pp[i] << std::endl;
  }
   
   //std::cout << "\n ======================== Get U Factor ========================"<<std::endl;
 	for (i=0; i<u_nnz_pp; ++i){
    std::cout<< i << ",ux_pp,"<< ux_pp[i] << ",ui_pp,"<< ui_pp[i] << std::endl;
  }
   
 	for (i=0; i<n+1; ++i){
 		std::cout<< i << ",up_pp,"<< up_pp[i] << std::endl;
 	}*/
 
 	// check arrays
  //for (i=0; i<n; ++i){
 	// 	std::cout<< i << ",\t diag_pp:"<< ldiag_pp[i] << ",\t cscale_pp:"<< cscale_pp[i] << ",\t rows_pp:"<< rows_pp[i] << ",\t cols_pp:"<< cols_pp[i] << std::endl;
  //}
//  for (i=0; i<n; ++i){
// 	 	std::cout<< i << ",rp_pp," << rp_pp[i]  << ",cpi_pp," << cpi_pp[i] << ",rows_pp,"<< rows_pp[i] << ",cols_pp," << cols_pp[i] <<std::endl;
//  }   
 
 	// // multiply U factor by cols (which is )
 	// for (i=0; i<n; ++i){
 	// 	for (j=up[i]; j<up[i+1]; ++j){
 	// 		p=rp[piv[ui[j]]]; // this may be rp!!!!!!!!!!!
 	// 		ux[j] = ux[j]/cols[p]; // include the scaling factor into L factor
 	// 	}
 	// }
    gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to ILU Factorize Bpp :: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");	
 gettimeofday(&t2_st, 0);
  // ============================= Filter fill-ins in L & U ===================================
	// construct the Bp L and U matrix into unorderes_set for next step
       int__t *lfillins_flag_pp, *ufillins_flag_pp;
	   lfillins_flag_pp = (uint__t *)malloc(sizeof(uint__t)*(l_nnz_pp)); // column indices of B'
	   ufillins_flag_pp = (uint__t *)malloc(sizeof(uint__t)*(u_nnz_pp)); // initial row pointers
	   int ilu_lpp_nnz = l_nnz_pp;
       int ilu_upp_nnz = u_nnz_pp;

	 std :: unordered_set<int> Bpp_nofillins;

	 for(int i =0;i<n; i++ )
	 {
		 int arow = i;
		 int astart = ap_pp[i];
		 int aend = ap_pp[i+1];

		 for( int j =astart;j<aend;j++)
		 {
			  int acol =ai_pp[j];
              Bpp_nofillins.insert((arow+1)*100000+acol+1);
			  // std::cout << " Debug ====== Bp_nofillins.size() =  " << Bp_nofillins.size() << std::endl;
		 }
	 }

   for (int i =0;i<n;i++)
	 {
		 int lrow = i;
		 int lstart = lp_pp[i];
		 int lend = lp_pp[i+1];
		 // std::cout << " Debug ====== lstart = " << lp[i]  << "; lend = " << lp[i+1] << std::endl;
		 for (j = lstart;j<lend;j++)
		 {
			 int lcol = li_pp[j];
			 int lindex = (lrow+1)*100000+lcol+1; 
			 //  std::cout << " L matrix ====== lindex = " << lindex << " Lx = "<< lx[j] << std::endl;
			 lfillins_flag_pp[j]=Bpp_nofillins.count(lindex);
			 //  std::cout << " L_fillin_flag_pp ["<< j << "] = "<< lfillins_flag_pp[j]  << std::endl;
		 }
	 }


	    for (int i =0;i<n;i++)
	 {
		 int urow = i;
		 int ustart = up_pp[i];
		 int uend = up_pp[i+1];
		 // std::cout << " Debug ====== ustart = " << up[i]  << "uend = " << up[i+1] << std::endl;
		 for (j = ustart;j<uend;j++)
		 {
			 int ucol = ui_pp[j];
			 int uindex = (urow+1)*100000+ucol+1; 
			 // std::cout << " U matgrix ====== uindex = " << uindex  << " Ux = "<< ux[j] << std::endl;
			 ufillins_flag_pp[j]=Bpp_nofillins.count(uindex);
			 // std::cout << " U_fillin_flag_pp ["<< j << "] = "<< ufillins_flag_pp[j]  << std::endl;
		 }
	 }

	 // filter the fillins 
	 for (int i = 0; i<n; i++)
	 {
		 int lrow = i;
		 int lstart = lp_pp[i];
		 int lend = lp_pp[i+1];
		 // std::cout << " Debug ====== lstart = " << lp[i]  << "; lend = " << lp[i+1] << std::endl;
		 for (j = lstart;j<lend;j++)
		 {
			 int lcol = li_pp[j];
			 double templx_pp = lx_pp[j]/ldiag_pp[lcol];  // LDLT
			 // std::cout << " LDLT: temp Lx["<<j<<"] = "<< templx <<std::endl;
		 if ((lfillins_flag_pp[j] == 0) && (abs(templx_pp))<= ilu_threshold )
		   {
				 lx_pp[j]=0;
				 ilu_lpp_nnz--;
				 // std::cout << "Lx["<<j<<"] = "<< lx[j]<<std::endl;
			 }
			// std::cout << "filtered Lx["<<j<<"] = "<< lx[j]<<std::endl;
		 }
	 }
	 	 for (int i = 0; i<u_nnz_pp; i++)
	 {
		 if ((ufillins_flag_pp[i] == 0) && (abs(ux_pp[i]))<= ilu_threshold )
		   {
				 ux_pp[i]=0;
				 ilu_upp_nnz--;
				 // std::cout << "Ux["<<i<<"] = "<< ux[i]<<std::endl;
			 }
			  //std::cout << "filtered Ux["<<i<<"] = "<< ux[i]<<std::endl;
	 }

  	std::cout << " ======================== Number of Total nonzeros after factorization is: "<< graphlu_pp->lu_nnz << "========================"<<std::endl;
 	std::cout << " ======================== Number of nonzeros in L: "<< graphlu_pp->l_nnz << "========================"<<std::endl;
 	std::cout << " ======================== Number of nonzeros in U: "<< graphlu_pp->u_nnz << "========================"<<std::endl;
	std::cout << " ======================== Number of nonzeros in ILU filtered L: "<< ilu_lpp_nnz << "========================"<<std::endl;
 	std::cout << " ======================== Number of nonzeros in ILU filtered U: "<< ilu_upp_nnz << "========================"<<std::endl;	 
    
   gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to ILU Filter Bpp :: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");	
 gettimeofday(&t2_st, 0);
	//Store the factorized and permuted LU values 
    for (i = 0; i < n; ++i){
      
      gVertex_pp += T_vertex_pp(i+1, ap_pp[i+1]-ap_pp[i], lp_pp[i+1]-lp_pp[i], up_pp[i+1]-up_pp[i], rp_pp[i], cpi_pp[i]);
	  
	  // process all non-zeros in B matrix on row i
      for (j = ap_pp[i]; j < ap_pp[i+1]; ++j){
        gMatrix_pp_B += T_pp_B_matrix((i+1)*100000+(ai_pp[j]+1), ai_pp[j], ax_pp[j]); 
      } 
	  
      // process all non-zeros in L matrix on row i
      for (j = lp_pp[i]; j < lp_pp[i+1]; ++j){
        gMatrix_pp_L += T_pp_L_matrix((i+1)*100000+(li_pp[j]+1), li_pp[j], lx_pp[j]); 
      } 
      
      // process all non-zeros in U matrix on row i
      for (j = up_pp[i]; j < up_pp[i+1]; ++j){       
        gMatrix_pp_U += T_pp_U_matrix((i+1)*100000+(ui_pp[j]+1), ui_pp[j], ux_pp[j]);
      }    
    }
    gVertex_pp.sort_heap();
	gMatrix_pp_B.sort_heap();
    gMatrix_pp_L.sort_heap();
    gMatrix_pp_U.sort_heap();          

  
  // Get the time for factorizing B' and B"
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	
	printf("\n\n============================================================================================== ");
	std::cout << "Time for Store the factorized and permuted Bp, LU values:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");
  
  if(error_p >= 0 && error_pp >= 0){
      result = "Factorization Completed";
      printf("\n\n============================================================================================== ");
  	  std::cout << "Factorization Completed" << std::endl;
  	  printf("\n================================================================================================ "); 

  } else {
    //factorization failed
 	  result = "Factorization FAILED";
    printf("\n\n============================================================================================== ");
  	std::cout << "Factorization FAILED" << std::endl;
  	printf("\n================================================================================================ ");  
  }
EXIT:
	// std::cout << "\n free1 " << std::endl;
	GraphLU_Destroy(graphlu);
	GraphLU_Destroy(graphlu_pp);
	// std::cout << "\n free2 " << std::endl;
	free(ax);
	free(ai);
	free(ap);
	free(ax_pp);
	free(ai_pp);
	free(ap_pp);
	free(graphlu);
	free(graphlu_pp);

 	free(lx);
	free(li);
	free(lp);
    free(ux);
	free(ui);
	free(up);  
 	free(lx_pp);
	free(li_pp);
	free(lp_pp);
 	free(ux_pp);
	free(ui_pp);
	free(up_pp);    
	
  printf("\n\n----------------------------------------------------------------------------------------");
	printf("\t\t End of Running GRAPHLU_fdpf_ILU_factorize6 C function!");
	printf("\t\t ----------------------------------------------------------------------------------------\n\n");	
 gettimeofday(&t3_end, 0);
	seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	useconds = t3_end.tv_usec  - t3_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time of GRAPHLU_fdpf_ILU_factorize6:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");
  return result;
}

 // **********************************************************************************************************************************
 // Created by: Yiting Zhao, yitingzhao6777@gmail
 // Date: 05/02/2017
 // This code performs fast-decoupled power flow analysis. It is used as a benchmark to assess the time it takes to run a fast-decoupled
 // power flow in C++ with GRAPHLU. This version does NOT perform LU factorization. B' and B" are assumed to be previously factorized and
 // the resulting L and U matrices are taken as inputs. Power flow iterations are solved with forward/backward substitution.
 // In this function, the HeapAccum for Ybus matrix  and factorized LU matrices are assumed to be built and sorted in the past. 
 // Sorting of HeapAccums is only done for the one that contains state variables.
 // History: 
 // 11/15/2016: [Ting Chan] added another SetAccum for Bpp fill-in edges
 // 01/09/2017: [Ting Chan] added a control parameter (fillin) to specify whether we want to find the fill-ins.
 //             1 - find fill-ins
 //             0 - otherwise
 // 01/17/2017: [Ting Chan] Replaced GraphLU_Solve with our own implementation of forward/backward substitution. 
 // 01/18/2017: [Ting Chan] Added two new HeapAccums to store factorized L values (diagonal), scaling factors, permutation and permutation
 //             inverse.
 // 01/20/2017: [Ting Chan] B' and B" have previously been factorized. HeapAccums for the factorized values, index and pointers are 
 //             taken as inputs. Peform sorting of the HeapAccum first, then convert them into arrays for power flow iteration and
 //             forward/backward substitution.
 // 01/23/2017: [Ting Chan] Consolidated HeapAccums to reduce sorting time.
 // 01/30/2017: [Ting Chan] Fixed forward/backward substitution to use cpi instead of rpi
 // 01/31/2017: [Ting Chan] Added sizes of L', U' L", and U" HeapAccums as inputs
 // 02/01/2017: [Ting Chan] Created to be used with schema version flowSchema_LU7_graphsql44.ddl 
 // 02/03/2017: [Ting Chan] Added cols and cols_pp for column scaling factor
 // 02/06/2017: [Ting Chan] Use Lflag and Uflag to decide whether an non-zero element should be added to lx or ux
 //             (I think this step might not be necessary and can probably be implemented in a better way by directly using
 //              Lvalue and U value, but do this for now)
 // 02/08/2017: [Ting Chan] Use Lvalue and Uvalue to decide whether an non-zero element should be added to lx or ux instead of
 //             Lflag and Uflag. (see previous comment)
 // 02/08/2017: [Ting Chan] With the previous change, we can no longer use Lpointer and Upointer stored on the vertex directly.
 //             This is discovered during testing of the sc_20160818_1120 case as there are zero elements in the output LU
 //             structure from GraphLU_DumpLU. This should be investigated. At the meantime, update the logic to get the pointer
 //             information.
 // 02/17/2017: [Ting Chan] All HeapAccums except the Ybus vertex which contains state variables are static and have been built and sorted
 //             so there is no need to perform sorting.
 // 02/22/2017: [Ting Chan] L' and U' are represented by two seperate HeapAccums. Updated array creation logic.
 // 02/23/2017: [Ting Chan] Removed the LU sizing input parameters. The LU number of non-zeros can be obtained from the HeapAccums directly.
 // 03/08/2017: [Chen Yuan] 
 //             Removed B' and B'' factorization and update of deltaP and deltaQ. Only do V angle forward/backward subsitution. 
 //             All static HeapAccums obtained from GRAPHLU_fdpf_LU_factorize_only after it ran in GraphSQL. HeapAccums for updated deltaP and deltaQ  //             are built in GraphSQL and sorted in this function.
 // 04/27/2017: [Yiting Zhao] Modified for ILU filter; removed MC64 scaling
 // 05/03/2017: [Yiting Zhao] Added deltaPn calculation and converted back to GSQL. 			  
 // **********************************************************************************************************************************

template <//typename T_deltaP_vertex, typename deltaP_vertex_comp,
          typename T_vertex_VaDeltaP, typename vertex_VaDeltaP_comp,  
		  typename T_p_B_matrix, typename p_B_matrix_comp,
          typename T_p_L_matrix, typename p_L_matrix_comp,
          typename T_p_U_matrix, typename p_U_matrix_comp,  
          //typename T_pp_L_matrix, typename pp_L_matrix_comp,
          //typename T_pp_U_matrix, typename pp_U_matrix_comp,
          typename T_vertex_p, typename vertex_p_comp>
          //typename T_vertex_pp, typename vertex_pp_comp>
inline string GRAPHLU_fdpf_ILU_solve6_Bp (
              HeapAccum<T_vertex_VaDeltaP, vertex_VaDeltaP_comp>& gVertex_VaDeltaP,HeapAccum<T_p_B_matrix, p_B_matrix_comp>& gMatrix_p_B,
              HeapAccum<T_p_L_matrix, p_L_matrix_comp>& gMatrix_p_L, HeapAccum<T_p_U_matrix, p_U_matrix_comp>& gMatrix_p_U, HeapAccum<T_vertex_p, vertex_p_comp>& gVertex_p
              //HeapAccum<T_pp_L_matrix, pp_L_matrix_comp>& gMatrix_pp_L, HeapAccum<T_pp_U_matrix, pp_U_matrix_comp>& gMatrix_pp_U, HeapAccum<T_vertex_pp, vertex_pp_comp>& gVertex_pp,
              ) {
        struct timeval t2_st, t2_end, t3_st, t3_end; long seconds, useconds;
		gettimeofday(&t3_st, 0);

	// extern size_t g_si, g_sd, g_sp;
	printf("\n\n------------------------------------------------------------------------------------------- ");
	printf("\nStart Running GRAPHLU_fdpf_ILU_solve6_Bp function!\n");
    std::cout << "Bp Number of rows:" << gVertex_p.data_.size() << ",\tNumber of nonzeros:" << gMatrix_p_B.size() << std::endl;
    std::cout << "Lp Number of rows:" << gVertex_p.data_.size() << ",\tNumber of nonzeros:" << gMatrix_p_L.size() << std::endl;
    std::cout << "Up Number of rows:" << gVertex_p.data_.size() << ",\tNumber of nonzeros:" << gMatrix_p_U.size() << std::endl;
    std::cout << "Y bus Number of rows:" << gVertex_VaDeltaP.data_.size() <<  std::endl;
	printf("-------------------------------------------------------------------------------------------- \n\n");	
	// ------------------------------------------------------------------------------------------------
	// 				Initialize variables and arrays
	// ------------------------------------------------------------------------------------------------
	

	// Initialize arrays and variables
	uint__t n, nnz_p_L, nnz_p_U, n_e, nnz_e, nnz_p_B;  // nnz_pp_L, nnz_pp_U,
	int i, j, p; // ret, iter
	//double maxDeltaP=0, maxDeltaQ=0, max_change_ex=maxchange;
  
	const double pi_value = 3.141592653589793;

	string result = "Bp SOLVE FAILED";
	
	// Get the dimension and the nnz for the LU of B' and B"
	n=gVertex_VaDeltaP.data_.size();	
	nnz_p_B=gMatrix_p_B.size();
    nnz_p_L=gMatrix_p_L.size(); 
    nnz_p_U=gMatrix_p_U.size();
  //nnz_pp_L=gMatrix_pp_L.size(); nnz_pp_U=gMatrix_pp_U.size();
  //n_e=gVertex_Va.data_.size(); // nnz_e=gMatrix_Ybus.data_.size(); // get the size of the Y bus matrix
  
	
	//// For Y-bus
	double *deltaP, *Va, *deltaPn, *deltaPhi; 
  
  // For L' and U'
  double *ax, *lx, *ux; 
  uint *ai, *li, *ui, *rp, *cpi; 
  size_t *ap, *lp, *up; 
  
    ax = (double *)malloc(sizeof(double)*(nnz_p_B));
	ai = (uint *)malloc(sizeof(uint)*(nnz_p_B)); // column indices 
	ap = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer
  
	lx = (double *)malloc(sizeof(double)*(nnz_p_L));
	li = (uint *)malloc(sizeof(uint)*(nnz_p_L)); // column indices 
	lp = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer
	
	ux = (double *)malloc(sizeof(double)*(nnz_p_U));
	ui = (uint *)malloc(sizeof(uint)*(nnz_p_U)); // column indices 
	up = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer 
    rp = (uint *)malloc(sizeof(uint)*(n));
    cpi = (uint *)malloc(sizeof(uint)*(n));
  
	deltaP = (double *)malloc(sizeof(double)*(n)); // b in the Ax=b
	deltaPn = (double *)malloc(sizeof(double)*(n)); // b in the Ax=b

	deltaPhi = (double *)malloc(sizeof(double)*(n)); 
	Va = (double *)malloc(sizeof(double)*(n)); 

  double sum, diag;
  double *b;  // *delta, 
  //delta = (double *)malloc(sizeof(double)*(n));
  b = (double *)malloc(sizeof(double)*(n));
  
	std::cout << " ======================== Initialization of ararys used to store the factorized matrix and LU ========================"<< std::endl;

	

  //initialization to 0
  // [tc] use memset here
  //memset(Vm, 0, sizeof(double)*(n));
  memset(Va, 0, sizeof(double)*(n));
  memset(deltaP, 0, sizeof(double)*(n));
  memset(deltaPhi, 0, sizeof(double)*(n));
  memset(deltaPn, 0, sizeof(double)*(n));
  //memset(deltaQ, 0, sizeof(double)*(n));
	
  // =================================== Sort all input HeapAccum =================================

  // Input HeapAccums are assumed to be unsorted. Sort them before assigning values to
  // local arrays
  gettimeofday(&t2_st, 0);
  gVertex_VaDeltaP.sort_heap();   
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gVertex_VaDeltaP HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");    

	
	// ========================================= Convert pointers and the Vertex =========================================
	gettimeofday(&t2_st, 0); 
  int i_p; // i_pp
	int ii = 1; // counter for the array  
  // Assign value to the first position of the pointer array 
	ap[0] = 0;
    lp[0] = 0;
    up[0] = 0;

	for (i=0; i<gVertex_VaDeltaP.size(); ++i){
    //process pointers for the matrices
	ap[ii] = gVertex_p.data_[i].B_p + ap[ii-1];	
    lp[ii] = gVertex_p.data_[i].Lp + lp[ii-1];
    up[ii] = gVertex_p.data_[i].Up + up[ii-1];

    ii++;

    Va[i] = gVertex_VaDeltaP.data_[i].VValue;	
	deltaP[i] = gVertex_VaDeltaP.data_[i].deltaValue;
	  
    rp[i] = gVertex_p.data_[i].rp;
    cpi[i] = gVertex_p.data_[i].cpi;  
   
	}
  //debug print
  //for (i=0; i<n; ++i){
 	 	//std::cout<< i << ",rp," << rp[i]  << ",cpi," << cpi[i] << std::endl;
  //}  

 	// ========================================= Convert Bp, L' and U' =========================================
  //int i_l = 0, i_u = 0;
  for (ii=0; ii<gMatrix_p_B.data_.size(); ++ii) {
    ai[ii] = gMatrix_p_B.data_[ii].cpi;
    ax[ii] = gMatrix_p_B.data_[ii].value; 
    // std::cout<< ii << ",ai,"<< ai[ii] << ",ax,"<< ax[ii] << std::endl;  	
  }
		
  for (ii=0; ii<gMatrix_p_L.data_.size(); ++ii) {
    li[ii] = gMatrix_p_L.data_[ii].cpi;
    lx[ii] = gMatrix_p_L.data_[ii].value; 
    //std::cout<< i << ",li,"<< deltaP[i] << ",deltaQ,"<< deltaQ[i] << std::endl;  	
  } 
  
  for (ii=0; ii<gMatrix_p_U.data_.size(); ++ii) {  
    ui[ii] = gMatrix_p_U.data_[ii].cpi;
    ux[ii] = gMatrix_p_U.data_[ii].value; 
    //std::cout<< i << ",deltaP,"<< deltaP[i] << ",deltaQ,"<< deltaQ[i] << std::endl;  	
  } 
  
    //  std::cout<< i << ",deltaP,"<< deltaP[i] << ",deltaQ,"<< deltaQ[i] << std::endl;  
  
  
  //debug print
/*	for (i=0; i<n+1; ++i){
     std::cout<< i << ",ap,"<< ap[i] <<",lp,"<< lp[i] <<",up,"<< up[i] << std::endl;
  }
	
    for (i=0; i<gMatrix_p_B.data_.size(); ++i)
  {
     std::cout<< i << ",ax,"<< ax[i] << ",ai,"<< ai[i] << std::endl;  
  } 
  for (i=0; i<gMatrix_p_L.data_.size(); ++i)
  {
     std::cout<< i << ",lx,"<< lx[i] << ",li,"<< li[i] << std::endl;  
  }    
  for (i=0; i<gMatrix_p_U.data_.size(); ++i)
  {
     std::cout<< i << ",ux,"<< ux[i] << ",ui,"<< ui[i] << std::endl;  
  } 
*/ 
  
	// Get the time to convert the data between containers and arrays
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;

	printf("\n\n============================================================================================== ");
	std::cout << "Time to convert data to GRAPHLU time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");		

  //continue to do the iterations only if there is no error from factorization
 	printf("\n\n============================================================================================== ");
 	std::cout << "\n Start iteratively updating deltaP and deltaQ " << std::endl;
 	printf("\n================================================================================================\n ");
 
 	gettimeofday(&t2_st, 0);
  
	// solve for deltaVa
	// std::cout << " ======================== Solve for V angle ========================"<<std::endl;
     
    // A*x = b
    // multiply deltaP with row scaling (rows) to get b
   	for (i=0; i<n; ++i){
	  b[i]=deltaP[rp[i]];  
   	}
    
   	// Full forward substitution 
   	for (i=0; i<n; ++i)
   	{
   		sum=0.;
   		diag=0.;
   		for(p=lp[i]; p< lp[i+1]; ++p){
   			if (i!=li[p]){ // if it is not diagnal element
   				j=li[p];
   				sum += lx[p] * b[j];
   			}
   			else{
   				diag = lx[p];
   			}
   		}
   		b[i] -= sum; 
   		b[i] /= diag; 
   	} 
    
   	// Full backward substitution 
   	for (i=n-1; i>=0; --i)
   	{
   		sum=0.;
   		for(p=up[i]; p< up[i+1]; ++p){
   			if (i!=ui[p]){ // if it is not diagnal element
   				sum += ux[p] * b[ui[p]]; 
   			}
   		}
   
   		b[i] -= sum; 
   	}      

	// Update V angle (Va)
  		for (i=0; i<n; ++i)
      {               
          deltaPhi[i] = b[i]; // save b[i] to deltaphi calculate the deltaPn = B'*(deltaphi) 
		  Va[i] -= b[cpi[i]];
          // std::cout << "Va " << i << ", " << Va[i] << std::endl; 		  
      }

   
 		// std::cout << " ======================== Update deltaPn ========================"<<std::endl;

	  
	  	  // Calculate deltaPn to update delta Phi
	  for (i=0; i<n; ++i){
		  b[i] = 0;
		  for (j=ap[i]; j<ap[i+1]; ++j){
			  p = ai[j];			  
  			  b[i] += ax[j]* deltaPhi[p];
  		  } 
	  }

	 // Update deltaP
  		for (i=0; i<n; ++i)
      {   
          deltaPn[i] = b[cpi[i]];   // [YT] This deltaPn has been divided by Vm		  
      }
  
  	// ----------------------------------------------------------------------------------------------------
  	// 								Store the Result back to the array
  	// ----------------------------------------------------------------------------------------------------

  	// Get solved Vm and Va back into HeapAccum
  	for (ii=0; ii<n; ++ii) {
         gVertex_VaDeltaP.data_[ii].VValue = Va[ii];
		 gVertex_VaDeltaP.data_[ii].deltaValue = deltaPn[ii]; 
         // std::cout << "\n Factorized result, " << ii+1 << ", Vm, " << Vm[ii]<< ", Va" << ii+1 << ", " << Va[ii]/pi_value*180<< std::endl;
  	}
 result = "Successfully Solved";
	
EXIT:
	// std::cout << "\n free3 " << std::endl;
		
	//free(eG);
	free(ax);
	free(ai);
	free(ap);

	free(deltaP);
	free(deltaPn);
	free(deltaPhi);
	free(Va);		
	//free(Pn);
	//free(Qn);	
  
 	free(lx);
	free(li);
	free(lp);
    free(ux);
	free(ui);
	free(up); 
    free(rp);
    free(cpi);     
	
  printf("\n\n----------------------------------------------------------------------------------------");
	printf("\t\t End of Running GRAPHLU_fdpf_ILU_solve6_Bp C function!");
	printf("\t\t ----------------------------------------------------------------------------------------\n\n");
	      gettimeofday(&t3_end, 0);
  	  seconds=t3_end.tv_sec  - t3_st.tv_sec; 
  	  useconds = t3_end.tv_usec  - t3_st.tv_usec;
  	  printf("\n\n===============================================================================\n ");
  	  std::cout << "\n Time for GRAPHLU_fdpf_ILU_solve6_Bp:" << (seconds*1000000 + useconds) << " us" << std::endl;
  	  printf("\n=================================================================================\n ");	
 return result;
}


 // **********************************************************************************************************************************
 // Created by: Yiting Zhao, yitingzhao6777@gmail
 // Date: 05/02/2017
 // This code performs fast-decoupled power flow analysis. It is used as a benchmark to assess the time it takes to run a fast-decoupled
 // power flow in C++ with GRAPHLU. This version does NOT perform LU factorization. B' and B" are assumed to be previously factorized and
 // the resulting L and U matrices are taken as inputs. Power flow iterations are solved with forward/backward substitution.
 // In this function, the HeapAccum for Ybus matrix  and factorized LU matrices are assumed to be built and sorted in the past. 
 // Sorting of HeapAccums is only done for the one that contains state variables.
 // History: 
 // 11/15/2016: [Ting Chan] added another SetAccum for Bpp fill-in edges
 // 01/09/2017: [Ting Chan] added a control parameter (fillin) to specify whether we want to find the fill-ins.
 //             1 - find fill-ins
 //             0 - otherwise
 // 01/17/2017: [Ting Chan] Replaced GraphLU_Solve with our own implementation of forward/backward substitution. 
 // 01/18/2017: [Ting Chan] Added two new HeapAccums to store factorized L values (diagonal), scaling factors, permutation and permutation
 //             inverse.
 // 01/20/2017: [Ting Chan] B' and B" have previously been factorized. HeapAccums for the factorized values, index and pointers are 
 //             taken as inputs. Peform sorting of the HeapAccum first, then convert them into arrays for power flow iteration and
 //             forward/backward substitution.
 // 01/23/2017: [Ting Chan] Consolidated HeapAccums to reduce sorting time.
 // 01/30/2017: [Ting Chan] Fixed forward/backward substitution to use cpi instead of rpi
 // 01/31/2017: [Ting Chan] Added sizes of L', U' L", and U" HeapAccums as inputs
 // 02/01/2017: [Ting Chan] Created to be used with schema version flowSchema_LU7_graphsql44.ddl 
 // 02/03/2017: [Ting Chan] Added cols and cols_pp for column scaling factor
 // 02/06/2017: [Ting Chan] Use Lflag and Uflag to decide whether an non-zero element should be added to lx or ux
 //             (I think this step might not be necessary and can probably be implemented in a better way by directly using
 //              Lvalue and U value, but do this for now)
 // 02/08/2017: [Ting Chan] Use Lvalue and Uvalue to decide whether an non-zero element should be added to lx or ux instead of
 //             Lflag and Uflag. (see previous comment)
 // 02/08/2017: [Ting Chan] With the previous change, we can no longer use Lpointer and Upointer stored on the vertex directly.
 //             This is discovered during testing of the sc_20160818_1120 case as there are zero elements in the output LU
 //             structure from GraphLU_DumpLU. This should be investigated. At the meantime, update the logic to get the pointer
 //             information.
 // 02/17/2017: [Ting Chan] All HeapAccums except the Ybus vertex which contains state variables are static and have been built and sorted
 //             so there is no need to perform sorting.
 // 02/22/2017: [Ting Chan] L' and U' are represented by two seperate HeapAccums. Updated array creation logic.
 // 02/23/2017: [Ting Chan] Removed the LU sizing input parameters. The LU number of non-zeros can be obtained from the HeapAccums directly.
 // 03/08/2017: [Chen Yuan] 
 //             Removed B' and B'' factorization and update of deltaP and deltaQ. Only do V magnitude forward/backward subsitution. 
 //             All static HeapAccums obtained from GRAPHLU_fdpf_LU_factorize_only after it ran in GraphSQL. HeapAccums for updated deltaP and deltaQ  //             are built in GraphSQL and sorted in this function. 
 // 04/27/2017: [Yiting Zhao] Modified for ILU filter; removed MC64 scaling
 // 05/03/2017: [Yiting Zhao] Added deltaPn calculation and converted back to GSQL. 
 // **********************************************************************************************************************************
template <//typename T_deltaQ_vertex, typename deltaQ_vertex_comp,
         // typename T_deltaQ_vertex, typename deltaQ_vertex_comp,
          typename T_vertex_VmDeltaQ, typename vertex_VmDeltaQ_comp, 
         // typename T_matrix_Ybus, typename matrix_Ybus_comp, 
          //typename T_p_L_matrix, typename p_L_matrix_comp,
          //typename T_p_U_matrix, typename p_U_matrix_comp,
		  typename T_pp_B_matrix, typename pp_B_matrix_comp,
          typename T_pp_L_matrix, typename pp_L_matrix_comp,
          typename T_pp_U_matrix, typename pp_U_matrix_comp,
          //typename T_vertex_p, typename vertex_p_comp,
          typename T_vertex_pp, typename vertex_pp_comp>
inline string GRAPHLU_fdpf_ILU_solve6_Bpp (//HeapAccum<T_deltaQ_vertex, deltaQ_vertex_comp>& gdeltaQ_vertex, 
              HeapAccum<T_vertex_VmDeltaQ, vertex_VmDeltaQ_comp>& gVertex_VmDeltaQ, 
			  HeapAccum<T_pp_B_matrix, pp_B_matrix_comp>& gMatrix_pp_B,
              HeapAccum<T_pp_L_matrix, pp_L_matrix_comp>& gMatrix_pp_L,
			  HeapAccum<T_pp_U_matrix, pp_U_matrix_comp>& gMatrix_pp_U, 
			  HeapAccum<T_vertex_pp, vertex_pp_comp>& gVertex_pp ) {
	struct timeval t2_st, t2_end, t3_st, t3_end; long seconds, useconds;
    gettimeofday(&t3_st, 0);
	
	// extern size_t g_si, g_sd, g_sp;
	printf("\n\n------------------------------------------------------------------------------------------- ");
	printf("\nStart Running GRAPHLU_fdpf_ILU_solve6_Bpp function!\n");
  std::cout << "Bpp Number of rows:" << gVertex_pp.data_.size() << ",\tNumber of nonzeros:" << gMatrix_pp_B.size() << std::endl;
  std::cout << "Lpp Number of rows:" << gVertex_pp.data_.size() << ",\tNumber of nonzeros:" << gMatrix_pp_L.size() << std::endl;
  std::cout << "Upp Number of rows:" << gVertex_pp.data_.size() << ",\tNumber of nonzeros:" << gMatrix_pp_U.size() << std::endl;
  std::cout << "Y bus Number of rows:" << gVertex_VmDeltaQ.data_.size() <<  std::endl;
	printf("-------------------------------------------------------------------------------------------- \n\n");	
	// ------------------------------------------------------------------------------------------------
	// 				Initialize variables and arrays
	// ------------------------------------------------------------------------------------------------
	

	// Initialize arrays and variables
	uint__t n, nnz_pp_L, nnz_pp_U, nnz_pp_B; // nnz_e;n_e, , nnz_p_L, nnz_p_U
	int i, j, p;  // ret, iter
	//double maxDeltaP=0, maxDeltaQ=0, max_change_ex=maxchange;
  
	const double pi_value = 3.141592653589793;  
	string result = "Bpp SOLVE FAILED";

	// Get the dimension and the nnz for the LU of B' and B"
	n=gVertex_VmDeltaQ.data_.size();
	nnz_pp_B=gMatrix_pp_B.size();
    nnz_pp_L=gMatrix_pp_L.size(); 
    nnz_pp_U=gMatrix_pp_U.size();  
	
	//// For Y-bus
	double *deltaQ, *Vm, *deltaQn, *deltaV; 
  
  // for L" and U"
  double *ax_pp, *lx_pp, *ux_pp; 
  uint *ai_pp, *li_pp, *ui_pp, *rp_pp, *cpi_pp; 
  size_t *ap_pp, *lp_pp, *up_pp; 
 
    ax_pp = (double *)malloc(sizeof(double)*(nnz_pp_B));
	ai_pp = (uint *)malloc(sizeof(uint)*(nnz_pp_B)); // column indices 
	ap_pp = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer
  
	lx_pp = (double *)malloc(sizeof(double)*(nnz_pp_L));
	li_pp = (uint *)malloc(sizeof(uint)*(nnz_pp_L)); // column indices 
	lp_pp = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer
	
	ux_pp = (double *)malloc(sizeof(double)*(nnz_pp_U));
	ui_pp = (uint *)malloc(sizeof(uint)*(nnz_pp_U)); // column indices 
	up_pp = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer

    rp_pp = (uint *)malloc(sizeof(uint)*(n));
    cpi_pp = (uint *)malloc(sizeof(uint)*(n));    
 
	deltaQ = (double *)malloc(sizeof(double)*(n)); // b in the Ax=b
	deltaQn = (double *)malloc(sizeof(double)*(n)); // b in the Ax=b

	Vm = (double *)malloc(sizeof(double)*(n)); 
	deltaV = (double *)malloc(sizeof(double)*(n)); 

  double sum, diag;
  double  *b; 
  b = (double *)malloc(sizeof(double)*(n));
  
	std::cout << " ======================== Initialization of ararys used to store the factorized matrix and LU ========================"<< std::endl;



  //initialization to 0
  // [tc] use memset here
  memset(Vm, 0, sizeof(double)*(n));
  memset(deltaV, 0, sizeof(double)*(n));
  memset(deltaQ, 0, sizeof(double)*(n));
  memset(deltaQn, 0, sizeof(double)*(n));
	
  // =================================== Sort all input HeapAccum =================================

  // Input HeapAccums are assumed to be unsorted. Sort them before assigning values to
  // local arrays
  gettimeofday(&t2_st, 0);
  gVertex_VmDeltaQ.sort_heap();   
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gVertex_VmDeltaQ HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");    

	// ========================================= Convert pointers and the Vertex =========================================
	gettimeofday(&t2_st, 0); 
    int i_pp; // i_p, 
	int ii = 1; // counter for the array  
  // Assign value to the first position of the pointer array 
  ap_pp[0] = 0;
  lp_pp[0] = 0;
  up_pp[0] = 0;
  //ep[0] = 0;    
	for (i=0; i<gVertex_VmDeltaQ.size(); ++i){
    //process pointers for the matrices
	ap_pp[ii] = gVertex_pp.data_[i].B_p + ap_pp[ii-1];
    lp_pp[ii] = gVertex_pp.data_[i].Lp + lp_pp[ii-1];
    up_pp[ii] = gVertex_pp.data_[i].Up + up_pp[ii-1];    
    ii++;

    //process state variables
    Vm[i] = gVertex_VmDeltaQ.data_[i].VValue;
	deltaQ[i] = gVertex_VmDeltaQ.data_[i].deltaValue;

    // debug print
	// std::cout<< i << ",deltaP,"<< deltaP[i] << ",deltaQ,"<< deltaQ[i] << std::endl;  

    //process factorized information for B''
    rp_pp[i] = gVertex_pp.data_[i].rp;
    cpi_pp[i] = gVertex_pp.data_[i].cpi;    
	}
  //debug print
  //for (i=0; i<n; ++i){
 	// 	std::cout<< i << ",rp_pp," << rp_pp[i]  << ",cpi_pp," << cpi_pp[i] <<std::endl;
  //}   

 	// ========================================= Convert Bpp, L" and U" =========================================
  //i_l = 0; 
  //i_u = 0;
  
  for (ii=0; ii<gMatrix_pp_B.data_.size(); ++ii) {
    ai_pp[ii] = gMatrix_pp_B.data_[ii].cpi;
    ax_pp[ii] = gMatrix_pp_B.data_[ii].value;
  }
	
  for (ii=0; ii<gMatrix_pp_L.data_.size(); ++ii) {
    li_pp[ii] = gMatrix_pp_L.data_[ii].cpi;
    lx_pp[ii] = gMatrix_pp_L.data_[ii].value;
  }
  
  for (ii=0; ii<gMatrix_pp_U.data_.size(); ++ii) {  
    ui_pp[ii] = gMatrix_pp_U.data_[ii].cpi;
    ux_pp[ii] = gMatrix_pp_U.data_[ii].value;
  }
  
  //debug print   
  /*
  for (i=0; i<n+1; ++i){
     std::cout<< i << ",ap_pp,"<< ap_pp[i] <<",lp_pp,"<< lp_pp[i] <<",up_pp,"<< up_pp[i] << std::endl;
  }
  for (i=0; i<gMatrix_pp_B.data_.size(); ++i)
  {
     std::cout<< i << ",ax_pp,"<< ax_pp[i] << ",ai_pp,"<< ai_pp[i] << std::endl;  
  }
  for (i=0; i<gMatrix_pp_L.data_.size(); ++i)
  {
     std::cout<< i << ",lx_pp,"<< lx_pp[i] << ",li_pp,"<< li_pp[i] << std::endl;  
  } 
 
    for (i=0; i<gMatrix_pp_U.data_.size(); ++i)
  {
     std::cout<< i << ",ux_pp,"<< ux_pp[i] << ",ui_pp,"<< ui_pp[i] << std::endl;  
  } 
  */      	    	
  // Done converting all input HeapAccums to arrays
  
	// Get the time to convert the data between containers and arrays
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;

	printf("\n\n============================================================================================== ");
	std::cout << "Time to convert data to GRAPHLU time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");		

  //continue to do the iterations only if there is no error from factorization
 	printf("\n\n============================================================================================== ");
 	std::cout << "\n Start iteratively updating deltaP and deltaQ " << std::endl;
 	printf("\n================================================================================================\n ");
 
 	gettimeofday(&t2_st, 0);
  
 	// solve for deltaVm
 	// std::cout << " ======================== Solve for V magnitude ========================"<<std::endl;
    
     // A*x = b
     // multiply deltaQ with row scaling (rows_pp) to get b
   	for (i=0; i<n; ++i){
		b[i]=deltaQ[rp_pp[i]];   
   	}
   	
     // Full forward substitution 
   	for (i=0; i<n; ++i)
   	{
   		sum=0.;
   		diag=0.;
   		for(p=lp_pp[i]; p< lp_pp[i+1]; ++p){
   			if (i!=li_pp[p]){ // if it is not diagnal element
   				j=li_pp[p];
   				sum += lx_pp[p] * b[j]; 
   			}
   			else{
   				diag = lx_pp[p];
   			}
   		}
   
   		b[i] -= sum; 
   		b[i] /= diag;
   	} 
   	
     // Full backward substitution 
   	for (i=n-1; i>=0; --i)
   	{
   		sum=0.;
   		for(p=up_pp[i]; p< up_pp[i+1]; ++p){
   			if (i!=ui_pp[p]){ // if it is not diagnal element
   				sum += ux_pp[p] * b[ui_pp[p]]; 
   			}
   		}
   
   		b[i] -= sum; 
   	}      
   	
	// Update V magnitude (Vm)
  		for (i=0; i<n; ++i)
      {
          deltaV[i] = b[i]; 	  
      }       
	  
      // std::cout << " ======================== Update deltaP and deltaQ ========================"<<std::endl;
 
      // Calculate deltaQn to update delta V
	  for (i=0; i<n; ++i){
		  b[i] = 0;
		  for (j=ap_pp[i]; j<ap_pp[i+1]; ++j){
			  p = ai_pp[j];			  
  			  b[i] += ax_pp[j]* deltaV[p];
  		  } 
	  }

	 // Update deltaQ and Vm
  		for (i=0; i<n; ++i)
      {  
           deltaQn[i] = b[cpi_pp[i]];    
		   Vm[i] -= deltaV[cpi_pp[i]]; 
      }

  	//	std::cout << "Iter" << iter << " Updated for Vm, maxDeltaP, " << maxDeltaP << ", maxDeltaQ, " << maxDeltaQ << std::endl;
   
  	gettimeofday(&t2_end, 0);
  	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
  	useconds = t2_end.tv_usec  - t2_st.tv_usec;
  	
  	printf("\n\n============================================================================================== ");
  	std::cout << "Time for Solve mode: " << (seconds*1000000 + useconds) << " us" << std::endl;
  	printf("\n================================================================================================ ");
	
  	// ----------------------------------------------------------------------------------------------------
  	// 								Store the Result back to the array
  	// ----------------------------------------------------------------------------------------------------

  	// Get solved Vm and Va back into HeapAccum
  	for (ii=0; ii<n; ++ii) {
         gVertex_VmDeltaQ.data_[ii].VValue = Vm[ii];  // Because the deltaPn and deltaQn in F/B iteration are divided by Vm.
         gVertex_VmDeltaQ.data_[ii].deltaValue = deltaQn[ii];	// When convert to gsql for CG inner-iteration, deltaPn should recover to original 
         // std::cout << "\n solved Bpp result, " << ii+1 << ", Vm, " << Vm[ii]<< ", Va" << ii+1 << ", " << Va[ii]/pi_value*180<< std::endl;
  	}
result = "Successfully Solved";
  
EXIT:
	// std::cout << "\n free3 " << std::endl;

	free(deltaQ);
	free(deltaQn);
	free(Vm);
	free(deltaV);		
 
	free(ax_pp);
	free(ai_pp);
	free(ap_pp);
 	free(lx_pp);
	free(li_pp);
	free(lp_pp);
 	free(ux_pp);
	free(ui_pp);
	free(up_pp);

    free(rp_pp);
    free(cpi_pp);    
	
    printf("\n\n----------------------------------------------------------------------------------------");
	printf("\t\t End of Running GRAPHLU_fdpf_ILU_solve6_Bpp C function!");
	printf("\t\t ----------------------------------------------------------------------------------------\n\n");	
	
	gettimeofday(&t3_end, 0);
  	  seconds=t3_end.tv_sec  - t3_st.tv_sec; 
  	  useconds = t3_end.tv_usec  - t3_st.tv_usec;
  	  printf("\n\n===============================================================================\n ");
  	  std::cout << "\n Time for GRAPHLU_fdpf_ILU_solve6_Bpp:" << (seconds*1000000 + useconds) << " us" << std::endl;
  	  printf("\n=================================================================================\n ");	
 
  return result;
}




// **********************************************************************************************************************************
// Created by: Chen Yuan, chen.yuan@geirina.net
// Date: 03/07/2017
// This function takes an index and a HeapAccum as inputs and returns Va (voltage magnitude) as output. Specified for gVertex_Ybus
// History:
// **********************************************************************************************************************************
template <typename T_vertex_VaDeltaP_tuple, typename vertex_VaDeltaP_comp>
inline double GRAPHLU_getdeltaPn (uint64_t& index, const HeapAccum<T_vertex_VaDeltaP_tuple, vertex_VaDeltaP_comp> &gVertex_VaDeltaP) {
  return gVertex_VaDeltaP.data_[index-1].deltaValue;
} 

// **********************************************************************************************************************************
// Created by: Chen Yuan, chen.yuan@geirina.net
// Date: 03/07/2017
// This function takes an index and a HeapAccum as inputs and returns Vm (voltage magnitude) as output. Specified for gVertex_VmDeltaQ
// History:
// **********************************************************************************************************************************
template <typename T_vertex_VmDeltaQ_tuple, typename vertex_VmDeltaQ_comp>
inline double GRAPHLU_getdeltaQn (uint64_t& index, const HeapAccum<T_vertex_VmDeltaQ_tuple, vertex_VmDeltaQ_comp> &gVertex_VmDeltaQ) {
  return gVertex_VmDeltaQ.data_[index-1].deltaValue;
} 




// **********************************************************************************************************************************
// Created by: Chen Yuan, chen.yuan@geirina.net
// Date: 03/08/2017
// This code performs fast-decoupled power flow analysis. It is used as a benchmark to assess the time it takes to run a fast-decoupled
// power flow in C++ with GRAPHLU. This version does NOT perform LU factorization. B' and B" are assumed to be previously factorized and
// the resulting L and U matrices are taken as inputs. Power flow iterations are solved with forward/backward substitution.
// In this function, the HeapAccum for Ybus matrix  and factorized LU matrices are assumed to be built and sorted in the past. 
// Sorting of HeapAccums is only done for the one that contains state variables.
// History: 
// 11/15/2016: [Ting Chan] added another SetAccum for Bpp fill-in edges
// 01/09/2017: [Ting Chan] added a control parameter (fillin) to specify whether we want to find the fill-ins.
//             1 - find fill-ins
//             0 - otherwise
// 01/17/2017: [Ting Chan] Replaced GraphLU_Solve with our own implementation of forward/backward substitution. 
// 01/18/2017: [Ting Chan] Added two new HeapAccums to store factorized L values (diagonal), scaling factors, permutation and permutation
//             inverse.
// 01/20/2017: [Ting Chan] B' and B" have previously been factorized. HeapAccums for the factorized values, index and pointers are 
//             taken as inputs. Peform sorting of the HeapAccum first, then convert them into arrays for power flow iteration and
//             forward/backward substitution.
// 01/23/2017: [Ting Chan] Consolidated HeapAccums to reduce sorting time.
// 01/30/2017: [Ting Chan] Fixed forward/backward substitution to use cpi instead of rpi
// 01/31/2017: [Ting Chan] Added sizes of L', U' L", and U" HeapAccums as inputs
// 02/01/2017: [Ting Chan] Created to be used with schema version flowSchema_LU7_graphsql44.ddl 
// 02/03/2017: [Ting Chan] Added cols and cols_pp for column scaling factor
// 02/06/2017: [Ting Chan] Use Lflag and Uflag to decide whether an non-zero element should be added to lx or ux
//             (I think this step might not be necessary and can probably be implemented in a better way by directly using
//              Lvalue and U value, but do this for now)
// 02/08/2017: [Ting Chan] Use Lvalue and Uvalue to decide whether an non-zero element should be added to lx or ux instead of
//             Lflag and Uflag. (see previous comment)
// 02/08/2017: [Ting Chan] With the previous change, we can no longer use Lpointer and Upointer stored on the vertex directly.
//             This is discovered during testing of the sc_20160818_1120 case as there are zero elements in the output LU
//             structure from GraphLU_DumpLU. This should be investigated. At the meantime, update the logic to get the pointer
//             information.
// 02/17/2017: [Ting Chan] All HeapAccums except the Ybus vertex which contains state variables are static and have been built and sorted
//             so there is no need to perform sorting.
// 02/22/2017: [Ting Chan] L' and U' are represented by two seperate HeapAccums. Updated array creation logic.
// 02/23/2017: [Ting Chan] Removed the LU sizing input parameters. The LU number of non-zeros can be obtained from the HeapAccums directly.
// 03/08/2017: [Chen Yuan] 
//             Removed B' and B'' factorization and update of deltaP and deltaQ. Only do V angle forward/backward subsitution. 
//             All static HeapAccums obtained from GRAPHLU_fdpf_LU_factorize_only after it ran in GraphSQL. HeapAccums for updated deltaP and deltaQ  //             are built in GraphSQL and sorted in this function.

template <//typename T_deltaP_vertex, typename deltaP_vertex_comp,
         typename T_vertex_VaDeltaP, typename vertex_VaDeltaP_comp,  
         typename T_p_L_matrix, typename p_L_matrix_comp,
         typename T_p_U_matrix, typename p_U_matrix_comp,  
         //typename T_pp_L_matrix, typename pp_L_matrix_comp,
         //typename T_pp_U_matrix, typename pp_U_matrix_comp,
         typename T_vertex_p, typename vertex_p_comp>
         //typename T_vertex_pp, typename vertex_pp_comp>
inline string GRAPHLU_fdpf_LU_solve_fb_Va (
             HeapAccum<T_vertex_VaDeltaP, vertex_VaDeltaP_comp>& gVertex_VaDeltaP,
             HeapAccum<T_p_L_matrix, p_L_matrix_comp>& gMatrix_p_L, HeapAccum<T_p_U_matrix, p_U_matrix_comp>& gMatrix_p_U, HeapAccum<T_vertex_p, vertex_p_comp>& gVertex_p
             //HeapAccum<T_pp_L_matrix, pp_L_matrix_comp>& gMatrix_pp_L, HeapAccum<T_pp_U_matrix, pp_U_matrix_comp>& gMatrix_pp_U, HeapAccum<T_vertex_pp, vertex_pp_comp>& gVertex_pp,
             ) {


	// extern size_t g_si, g_sd, g_sp;
	printf("\n\n------------------------------------------------------------------------------------------- ");
	printf("\nStart Running GRAPHLU_fdpf_LU_solve_fb_Va function!\n");
 std::cout << "Lp Number of rows:" << gVertex_p.data_.size() << ",\tNumber of nonzeros:" << gMatrix_p_L.size() << std::endl;
 std::cout << "Up Number of rows:" << gVertex_p.data_.size() << ",\tNumber of nonzeros:" << gMatrix_p_U.size() << std::endl;
 //std::cout << "Lpp Number of rows:" << gVertex_pp.data_.size() << ",\tNumber of nonzeros:" << gMatrix_pp_L.size() << std::endl;
 //std::cout << "Upp Number of rows:" << gVertex_pp.data_.size() << ",\tNumber of nonzeros:" << gMatrix_pp_U.size() << std::endl;
 std::cout << "Y bus Number of rows:" << gVertex_VaDeltaP.data_.size() <<  std::endl;
	printf("-------------------------------------------------------------------------------------------- \n\n");	
	// ------------------------------------------------------------------------------------------------
	// 				Initialize variables and arrays
	// ------------------------------------------------------------------------------------------------
	

	// Initialize arrays and variables
	uint__t n, nnz_p_L, nnz_p_U, n_e, nnz_e;  // nnz_pp_L, nnz_pp_U,
	int i, j, p; // ret, iter
	//double maxDeltaP=0, maxDeltaQ=0, max_change_ex=maxchange;
 
	const double pi_value = 3.141592653589793;

	// Get the dimension and the nnz for the LU of B' and B"
	n=gVertex_VaDeltaP.data_.size();	
 nnz_p_L=gMatrix_p_L.size(); nnz_p_U=gMatrix_p_U.size();
 //nnz_pp_L=gMatrix_pp_L.size(); nnz_pp_U=gMatrix_pp_U.size();
 //n_e=gVertex_Va.data_.size(); // nnz_e=gMatrix_Ybus.data_.size(); // get the size of the Y bus matrix
 
	
	//// For Y-bus
	double *deltaP, *Va; // *Pn, *Qn; *eG, *eB, *deltaQ, *Vm, 
	//uint *ei, *piv, *btype; 
	//uint *ep; 
 
 // For L' and U'
 double *lx, *ux, *rows, *cols; 
 uint *li, *ui, *rp, *cpi; 
 size_t *lp, *up; 
	//uint__t fctnnz; // number of nonzeros after the matrix is factorized
	//size_t l_nnz = gLp_edge.data_.size(), u_nnz = gUp_edge.data_.size(); // number of nonzeros in L and U factors 

	lx = (double *)malloc(sizeof(double)*(nnz_p_L));
	li = (uint *)malloc(sizeof(uint)*(nnz_p_L)); // column indices 
	lp = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer
	ux = (double *)malloc(sizeof(double)*(nnz_p_U));
	ui = (uint *)malloc(sizeof(uint)*(nnz_p_U)); // column indices 
	up = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer 
 rows = (double *)malloc(sizeof(double)*(n));
 cols = (double *)malloc(sizeof(double)*(n));
 rp = (uint *)malloc(sizeof(uint)*(n));
 cpi = (uint *)malloc(sizeof(uint)*(n));
 
/*  // for L" and U"
 double *lx_pp, *ux_pp, *rows_pp, *cols_pp; 
 uint *li_pp, *ui_pp, *rp_pp, *cpi_pp; 
 size_t *lp_pp, *up_pp; 
	//uint__t fctnnz_pp; // number of nonzeros after the matrix is factorized
	//size_t l_nnz_pp, u_nnz_pp; // number of nonzeros in L and U factors

	lx_pp = (double *)malloc(sizeof(double)*(nnz_pp_L));
	li_pp = (uint *)malloc(sizeof(uint)*(nnz_pp_L)); // column indices 
	lp_pp = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer
	ux_pp = (double *)malloc(sizeof(double)*(nnz_pp_U));
	ui_pp = (uint *)malloc(sizeof(uint)*(nnz_pp_U)); // column indices 
	up_pp = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer
 rows_pp = (double *)malloc(sizeof(double)*(n));
 cols_pp = (double *)malloc(sizeof(double)*(n));
 rp_pp = (uint *)malloc(sizeof(uint)*(n));
 cpi_pp = (uint *)malloc(sizeof(uint)*(n));    */

 // arrays to store the sine and cosine terms of the angle difference
 // values are computed during Jacobian initialization and each iteration 
/*  double *cosine_array, *sine_array;
 cosine_array = (double *)malloc(sizeof(double)*(nnz_e));
 sine_array = (double *)malloc(sizeof(double)*(nnz_e)); 

	eG = (double *)malloc(sizeof(double)*(nnz_e)); // G values in Y 
	eB = (double *)malloc(sizeof(double)*(nnz_e)); // B values in Y
	ei = (uint *)malloc(sizeof(uint)*(nnz_e)); // column indices of Y  */
	//ep = (uint *)malloc(sizeof(uint)*(n_e+1)); // initial row pointers	

	deltaP = (double *)malloc(sizeof(double)*(n)); // b in the Ax=b
	//deltaQ = (double *)malloc(sizeof(double)*(n)); // b in the Ax=b

	//Vm = (double *)malloc(sizeof(double)*(n)); 
	Va = (double *)malloc(sizeof(double)*(n)); 

	//Pn = (double *)malloc(sizeof(double)*(n)); 
	//Qn = (double *)malloc(sizeof(double)*(n));

	//btype = (uint *)malloc(sizeof(uint)*(n));

 double sum, diag;
 double *b;  // *delta, 
 //delta = (double *)malloc(sizeof(double)*(n));
 b = (double *)malloc(sizeof(double)*(n));
 
	std::cout << " ======================== Initialization of ararys used to store the factorized matrix and LU ========================"<< std::endl;

	struct timeval t2_st, t2_end, t3_st, t3_end; long seconds, useconds;

 //initialization to 0
 // [tc] use memset here
 //memset(Vm, 0, sizeof(double)*(n));
 memset(Va, 0, sizeof(double)*(n));
 memset(deltaP, 0, sizeof(double)*(n));
 //memset(deltaQ, 0, sizeof(double)*(n));
	
 // =================================== Sort all input HeapAccum =================================
 gettimeofday(&t3_st, 0);

 // Input HeapAccums are assumed to be unsorted. Sort them before assigning values to
 // local arrays
 gettimeofday(&t2_st, 0);
 gVertex_VaDeltaP.sort_heap();   
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gVertex_VaDeltaP HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");    
	
  /* gettimeofday(&t2_st, 0);
   gdeltaP_vertex.sort_heap();
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gdeltaP_vertex HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); */
	
	
/*  gettimeofday(&t2_st, 0);
 gdeltaQ_vertex.sort_heap();
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gdeltaQ_vertex HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");  */
 
 // get the time for sorting all HeapAccums 
	gettimeofday(&t3_end, 0);
	seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	useconds = t3_end.tv_usec  - t3_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort all HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
	
	// ========================================= Convert pointers and the Vertex =========================================
	gettimeofday(&t2_st, 0); 
 int i_p; // i_pp
	int ii = 1; // counter for the array  
 // Assign value to the first position of the pointer array 
 lp[0] = 0;
 up[0] = 0;
 //lp_pp[0] = 0;
 //up_pp[0] = 0;    
 //ep[0] = 0;
	for (i=0; i<gVertex_VaDeltaP.size(); ++i){
   //process pointers for the matrices
   lp[ii] = gVertex_p.data_[i].Lp + lp[ii-1];
   up[ii] = gVertex_p.data_[i].Up + up[ii-1];
   //lp_pp[ii] = gVertex_pp.data_[i].Lp + lp_pp[ii-1];
   //up_pp[ii] = gVertex_pp.data_[i].Up + up_pp[ii-1];    
   //ep[ii] = gVertex_Ybus.data_[i].ep + ep[ii-1];
   ii++;
   // [tc] removed 02/08/2017. Cannot directly use pointers stored on vertex
   // see modifications history for details
   //lp[i] = gVertex_p.data_[i].Lp;
   //up[i] = gVertex_p.data_[i].Up;
   //lp_pp[i] = gVertex_pp.data_[i].Lp;
   //up_pp[i] = gVertex_pp.data_[i].Up;
   //process state variables
   //Vm[i] = gVertex_Vm.data_[i].Vm;
   Va[i] = gVertex_VaDeltaP.data_[i].VValue;
	//std::cout<< i << ",Vm,"<< Vm[i] << ",Va,"<< Va[i] << std::endl;  
   //Pn[i] = gVertex_Ybus.data_[i].Pn;
   //Qn[i] = gVertex_Ybus.data_[i].Qn;
   //btype[i] = gVertex_Ybus.data_[i].bustype;
	
	deltaP[i] = gVertex_VaDeltaP.data_[i].deltaValue;
	  
	// debug print
	
	
   //std::cout<< i << ",deltaP,"<< deltaP[i] << ",deltaQ,"<< deltaQ[i] << std::endl;  

   //process factorized information for B'
   //i_p = gVertex_p.data_[i].exId - 1;
   rows[i]= gVertex_p.data_[i].row_scaling;
   cols[i]= gVertex_p.data_[i].col_scaling;
   rp[i] = gVertex_p.data_[i].rp;
   cpi[i] = gVertex_p.data_[i].cpi;  
   //process factorized information for B"  
   //i_pp = gVertex_pp.data_[i].exId - 1;
   //rows_pp[i]= gVertex_pp.data_[i].row_scaling;
   //cols_pp[i]= gVertex_pp.data_[i].col_scaling;
   //rp_pp[i] = gVertex_pp.data_[i].rp;
   //cpi_pp[i] = gVertex_pp.data_[i].cpi;    
	}
 //debug print
 //for (i=0; i<n; ++i){
	 	//std::cout<< i << ",rp," << rp[i]  << ",cpi," << cpi[i] << ",rows,"<< rows[i] << ",cols," << cols[i] <<std::endl;
 //}
 //for (i=0; i<n; ++i){
	 	//std::cout<< i << ",rp_pp," << rp_pp[i]  << ",cpi_pp," << cpi_pp[i] << ",rows_pp,"<< rows_pp[i] << ",cols_pp," << cols_pp[i] <<std::endl;
 //}   
 // [tc] removed 02/08/2017. Cannot directly use pointers stored on vertex
 // see modifications history for details       
 // Assign total number of non-zeros to the last slot of the pointer arrays
 //lp[n] = gLp;
 //up[n] = gUp;
 //lp_pp[n] = gLpp;
 //up_pp[n] = gUpp;  

	// ========================================= Convert Ybus (ei, eG and eB) =========================================
/*  for (ii=0; ii<gMatrix_Ybus.size(); ++ii) {
   ei[ii] = gMatrix_Ybus.data_[ii].ei;
   eG[ii] = gMatrix_Ybus.data_[ii].eG;
   eB[ii] = gMatrix_Ybus.data_[ii].eB; 
 }  */
	// ========================================= Convert L' and U' =========================================
 //int i_l = 0, i_u = 0;

 for (ii=0; ii<gMatrix_p_L.data_.size(); ++ii) {
   li[ii] = gMatrix_p_L.data_[ii].cpi;
   lx[ii] = gMatrix_p_L.data_[ii].value; 
   //std::cout<< i << ",li,"<< deltaP[i] << ",deltaQ,"<< deltaQ[i] << std::endl;  	
 } 
 
 for (ii=0; ii<gMatrix_p_U.data_.size(); ++ii) {  
   ui[ii] = gMatrix_p_U.data_[ii].cpi;
   ux[ii] = gMatrix_p_U.data_[ii].value; 
   //std::cout<< i << ",deltaP,"<< deltaP[i] << ",deltaQ,"<< deltaQ[i] << std::endl;  	
 } 
 
   //  std::cout<< i << ",deltaP,"<< deltaP[i] << ",deltaQ,"<< deltaQ[i] << std::endl;  
 
 
/*  //debug print
 for (i=0; i<gMatrix_p_L.data_.size(); ++i)
 {
    std::cout<< i << ",lx,"<< lx[i] << ",li,"<< li[i] << std::endl;  
 } 
 for (i=0; i<n+1; ++i){
    std::cout<< i << ",lp,"<< lp[i] << std::endl;
 } 
 for (i=0; i<gMatrix_p_U.data_.size(); ++i)
 {
    std::cout<< i << ",ux,"<< ux[i] << ",ui,"<< ui[i] << std::endl;  
 } 
 for (i=0; i<n+1; ++i){
    std::cout<< i << ",up,"<< up[i] << std::endl;
 }  */
	// ========================================= Convert L" and U" =========================================
 //i_l = 0; 
 //i_u = 0;
 
/*  for (ii=0; ii<gMatrix_pp_L.data_.size(); ++ii) {
   li_pp[ii] = gMatrix_pp_L.data_[ii].cpi;
   lx_pp[ii] = gMatrix_pp_L.data_[ii].value;
 }
 
 for (ii=0; ii<gMatrix_pp_U.data_.size(); ++ii) {  
   ui_pp[ii] = gMatrix_pp_U.data_[ii].cpi;
   ux_pp[ii] = gMatrix_pp_U.data_[ii].value;
 }   */
 
/*  //debug print   
 for (i=0; i<gMatrix_pp_L.data_.size(); ++i)
 {
    std::cout<< i << ",lx_pp,"<< lx_pp[i] << ",li_pp,"<< li_pp[i] << std::endl;  
 } 
 for (i=0; i<n+1; ++i){
    std::cout<< i << ",lp_pp,"<< lp_pp[i] << std::endl;
 } 
   for (i=0; i<gMatrix_pp_U.data_.size(); ++i)
 {
    std::cout<< i << ",ux_pp,"<< ux_pp[i] << ",ui_pp,"<< ui_pp[i] << std::endl;  
 } 
 for (i=0; i<n+1; ++i){
    std::cout<< i << ",up_pp,"<< up_pp[i] << std::endl;
 } */  	
 // Done converting all input HeapAccums to arrays
 
	// Get the time to convert the data between containers and arrays
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;

	printf("\n\n============================================================================================== ");
	std::cout << "Time to convert data to GRAPHLU time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");		

 //continue to do the iterations only if there is no error from factorization
	printf("\n\n============================================================================================== ");
	std::cout << "\n Start iteratively updating deltaP and deltaQ " << std::endl;
	printf("\n================================================================================================\n ");

	gettimeofday(&t2_st, 0);
 
		// solve for deltaVa
		// std::cout << " ======================== Solve for V angle ========================"<<std::endl;
    
   // A*x = b
   // multiply deltaP with row scaling (rows) to get b
  	for (i=0; i<n; ++i){
     b[i]=deltaP[rp[i]]*rows[rp[i]];
  	}
   
  	// Full forward substitution 
  	for (i=0; i<n; ++i)
  	{
  		sum=0.;
  		diag=0.;
  		for(p=lp[i]; p< lp[i+1]; ++p){
  			if (i!=li[p]){ // if it is not diagnal element
  				j=li[p];
  				sum += lx[p] * b[j];
  			}
  			else{
  				diag = lx[p];
  			}
  		}
  		b[i] -= sum; 
  		b[i] /= diag; 
  	} 
   
  	// Full backward substitution 
  	for (i=n-1; i>=0; --i)
  	{
  		sum=0.;
  		for(p=up[i]; p< up[i+1]; ++p){
  			if (i!=ui[p]){ // if it is not diagnal element
  				sum += ux[p] * b[ui[p]]; 
  			}
  		}
  
  		b[i] -= sum; 
  	}      
  	// permute the array back to the original order
  	//for (i=0; i<n; ++i)
    //{
  	//	delta[i]=b[cpi[i]];
    //  //std::cout << "delta: " << i << ", " << delta[i] << std::endl;           
		//}
      
   // Update V angle (Va)
		for (i=0; i<n; ++i)
   { 
     //Va[i] -= delta[i];  
     Va[i] -= b[cpi[i]]*cols[cpi[i]];     
   }
   
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	
	printf("\n\n============================================================================================== ");
	std::cout << "Time for solving V angle: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");

 string result = "Finished Va Forward/Backward Substitution";
	// ----------------------------------------------------------------------------------------------------
	// 								Store the Result back to the array
	// ----------------------------------------------------------------------------------------------------
	// Get solved Vm and Va back into HeapAccum
	for (ii=0; ii<n; ++ii) {
      gVertex_VaDeltaP.data_[ii].VValue = Va[ii];  // in radian
      //gVertex_Vm.data_[ii].Vm = Vm[ii];
      //std::cout << "Final, " << ii+1 << ", Vm, " << Vm[ii]<< ", Va" << ii+1 << ", " << Va[ii]/pi_value*180<< std::endl;
	}

 
EXIT:

	// std::cout << "\n free3 " << std::endl;
		
	//free(eG);
	//free(eB);
	//free(ei);
	//free(ep);

	free(deltaP);
	//free(deltaQ);
	//free(Vm);
	free(Va);		
	//free(Pn);
	//free(Qn);	
 
	free(lx);
	free(li);
	free(lp);
 free(ux);
	free(ui);
	free(up); 
 free(rows);
 free(cols);
 free(rp);
 free(cpi); 
	//free(lx_pp);
	//free(li_pp);
	//free(lp_pp);
	//free(ux_pp);
	//free(ui_pp);
	//free(up_pp);
 //free(rows_pp);
 //free(cols_pp);
 //free(rp_pp);
 //free(cpi_pp);    
	
 printf("\n\n----------------------------------------------------------------------------------------");
	printf("\t\t End of Running GRAPHLU_fdpf_LU_solve_fb_Va C function!");
	printf("\t\t ----------------------------------------------------------------------------------------\n\n");	

 return result;
}

// **********************************************************************************************************************************
// Created by: Chen Yuan, chen.yuan@geirina.net
// Date: 03/08/2017
// This code performs fast-decoupled power flow analysis. It is used as a benchmark to assess the time it takes to run a fast-decoupled
// power flow in C++ with GRAPHLU. This version does NOT perform LU factorization. B' and B" are assumed to be previously factorized and
// the resulting L and U matrices are taken as inputs. Power flow iterations are solved with forward/backward substitution.
// In this function, the HeapAccum for Ybus matrix  and factorized LU matrices are assumed to be built and sorted in the past. 
// Sorting of HeapAccums is only done for the one that contains state variables.
// History: 
// 11/15/2016: [Ting Chan] added another SetAccum for Bpp fill-in edges
// 01/09/2017: [Ting Chan] added a control parameter (fillin) to specify whether we want to find the fill-ins.
//             1 - find fill-ins
//             0 - otherwise
// 01/17/2017: [Ting Chan] Replaced GraphLU_Solve with our own implementation of forward/backward substitution. 
// 01/18/2017: [Ting Chan] Added two new HeapAccums to store factorized L values (diagonal), scaling factors, permutation and permutation
//             inverse.
// 01/20/2017: [Ting Chan] B' and B" have previously been factorized. HeapAccums for the factorized values, index and pointers are 
//             taken as inputs. Peform sorting of the HeapAccum first, then convert them into arrays for power flow iteration and
//             forward/backward substitution.
// 01/23/2017: [Ting Chan] Consolidated HeapAccums to reduce sorting time.
// 01/30/2017: [Ting Chan] Fixed forward/backward substitution to use cpi instead of rpi
// 01/31/2017: [Ting Chan] Added sizes of L', U' L", and U" HeapAccums as inputs
// 02/01/2017: [Ting Chan] Created to be used with schema version flowSchema_LU7_graphsql44.ddl 
// 02/03/2017: [Ting Chan] Added cols and cols_pp for column scaling factor
// 02/06/2017: [Ting Chan] Use Lflag and Uflag to decide whether an non-zero element should be added to lx or ux
//             (I think this step might not be necessary and can probably be implemented in a better way by directly using
//              Lvalue and U value, but do this for now)
// 02/08/2017: [Ting Chan] Use Lvalue and Uvalue to decide whether an non-zero element should be added to lx or ux instead of
//             Lflag and Uflag. (see previous comment)
// 02/08/2017: [Ting Chan] With the previous change, we can no longer use Lpointer and Upointer stored on the vertex directly.
//             This is discovered during testing of the sc_20160818_1120 case as there are zero elements in the output LU
//             structure from GraphLU_DumpLU. This should be investigated. At the meantime, update the logic to get the pointer
//             information.
// 02/17/2017: [Ting Chan] All HeapAccums except the Ybus vertex which contains state variables are static and have been built and sorted
//             so there is no need to perform sorting.
// 02/22/2017: [Ting Chan] L' and U' are represented by two seperate HeapAccums. Updated array creation logic.
// 02/23/2017: [Ting Chan] Removed the LU sizing input parameters. The LU number of non-zeros can be obtained from the HeapAccums directly.
// 03/08/2017: [Chen Yuan] 
//             Removed B' and B'' factorization and update of deltaP and deltaQ. Only do V magnitude forward/backward subsitution. 
//             All static HeapAccums obtained from GRAPHLU_fdpf_LU_factorize_only after it ran in GraphSQL. HeapAccums for updated deltaP and deltaQ  //             are built in GraphSQL and sorted in this function. 
// **********************************************************************************************************************************
template <//typename T_deltaQ_vertex, typename deltaQ_vertex_comp,
        // typename T_deltaQ_vertex, typename deltaQ_vertex_comp,
         typename T_vertex_VmDeltaQ, typename vertex_VmDeltaQ_comp, 
        // typename T_matrix_Ybus, typename matrix_Ybus_comp, 
         //typename T_p_L_matrix, typename p_L_matrix_comp,
         //typename T_p_U_matrix, typename p_U_matrix_comp,  
         typename T_pp_L_matrix, typename pp_L_matrix_comp,
         typename T_pp_U_matrix, typename pp_U_matrix_comp,
         //typename T_vertex_p, typename vertex_p_comp,
         typename T_vertex_pp, typename vertex_pp_comp>
inline string GRAPHLU_fdpf_LU_solve_fb_Vm (//HeapAccum<T_deltaQ_vertex, deltaQ_vertex_comp>& gdeltaQ_vertex, 
             HeapAccum<T_vertex_VmDeltaQ, vertex_VmDeltaQ_comp>& gVertex_VmDeltaQ, // HeapAccum<T_matrix_Ybus, matrix_Ybus_comp>& gMatrix_Ybus, 
             //HeapAccum<T_p_L_matrix, p_L_matrix_comp>& gMatrix_p_L, HeapAccum<T_p_U_matrix, p_U_matrix_comp>& gMatrix_p_U, HeapAccum<T_vertex_p, vertex_p_comp>& gVertex_p,
             HeapAccum<T_pp_L_matrix, pp_L_matrix_comp>& gMatrix_pp_L, HeapAccum<T_pp_U_matrix, pp_U_matrix_comp>& gMatrix_pp_U, HeapAccum<T_vertex_pp, vertex_pp_comp>& gVertex_pp
             ) {


	// extern size_t g_si, g_sd, g_sp;
	printf("\n\n------------------------------------------------------------------------------------------- ");
	printf("\nStart Running GRAPHLU_fdpf_LU_solve_fb_Vm function!\n");
 //std::cout << "Lp Number of rows:" << gVertex_p.data_.size() << ",\tNumber of nonzeros:" << gMatrix_p_L.size() << std::endl;
 //std::cout << "Up Number of rows:" << gVertex_p.data_.size() << ",\tNumber of nonzeros:" << gMatrix_p_U.size() << std::endl;
 std::cout << "Lpp Number of rows:" << gVertex_pp.data_.size() << ",\tNumber of nonzeros:" << gMatrix_pp_L.size() << std::endl;
 std::cout << "Upp Number of rows:" << gVertex_pp.data_.size() << ",\tNumber of nonzeros:" << gMatrix_pp_U.size() << std::endl;
 std::cout << "Y bus Number of rows:" << gVertex_VmDeltaQ.data_.size() <<  std::endl;
	printf("-------------------------------------------------------------------------------------------- \n\n");	
	// ------------------------------------------------------------------------------------------------
	// 				Initialize variables and arrays
	// ------------------------------------------------------------------------------------------------
	

	// Initialize arrays and variables
	uint__t n, nnz_pp_L, nnz_pp_U; // nnz_e;n_e, , nnz_p_L, nnz_p_U
	int i, j, p;  // ret, iter
	//double maxDeltaP=0, maxDeltaQ=0, max_change_ex=maxchange;
 
	const double pi_value = 3.141592653589793;  

	// Get the dimension and the nnz for the LU of B' and B"
	n=gVertex_VmDeltaQ.data_.size();	
//nnz_p_L=gMatrix_p_L.size(); nnz_p_U=gMatrix_p_U.size();
 nnz_pp_L=gMatrix_pp_L.size(); nnz_pp_U=gMatrix_pp_U.size();
 //n_e=gVertex_Ybus.data_.size(); // nnz_e=gMatrix_Ybus.data_.size(); // get the size of the Y bus matrix
 
	
	//// For Y-bus
	double *deltaQ, *Vm; // *Pn, *Qn; *eG, *eB, *deltaP, *Va
	// uint *ei, *piv, *btype; 
	//uint *ep; 
 
/*  // For L' and U'
 double *lx, *ux, *rows, *cols; 
 uint *li, *ui, *rp, *cpi; 
 size_t *lp, *up; 
	//uint__t fctnnz; // number of nonzeros after the matrix is factorized
	//size_t l_nnz = gLp_edge.data_.size(), u_nnz = gUp_edge.data_.size(); // number of nonzeros in L and U factors 

	lx = (double *)malloc(sizeof(double)*(nnz_p_L));
	li = (uint *)malloc(sizeof(uint)*(nnz_p_L)); // column indices 
	lp = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer
	ux = (double *)malloc(sizeof(double)*(nnz_p_U));
	ui = (uint *)malloc(sizeof(uint)*(nnz_p_U)); // column indices 
	up = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer 
 rows = (double *)malloc(sizeof(double)*(n));
 cols = (double *)malloc(sizeof(double)*(n));
 rp = (uint *)malloc(sizeof(uint)*(n));
 cpi = (uint *)malloc(sizeof(uint)*(n));  */
 
 // for L" and U"
 double *lx_pp, *ux_pp, *rows_pp, *cols_pp; 
 uint *li_pp, *ui_pp, *rp_pp, *cpi_pp; 
 size_t *lp_pp, *up_pp; 
	//uint__t fctnnz_pp; // number of nonzeros after the matrix is factorized
	//size_t l_nnz_pp, u_nnz_pp; // number of nonzeros in L and U factors

	lx_pp = (double *)malloc(sizeof(double)*(nnz_pp_L));
	li_pp = (uint *)malloc(sizeof(uint)*(nnz_pp_L)); // column indices 
	lp_pp = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer
	ux_pp = (double *)malloc(sizeof(double)*(nnz_pp_U));
	ui_pp = (uint *)malloc(sizeof(uint)*(nnz_pp_U)); // column indices 
	up_pp = (size_t *)malloc(sizeof(size_t)*(n+1)); // initial pointer
 rows_pp = (double *)malloc(sizeof(double)*(n));
 cols_pp = (double *)malloc(sizeof(double)*(n));
 rp_pp = (uint *)malloc(sizeof(uint)*(n));
 cpi_pp = (uint *)malloc(sizeof(uint)*(n));    

 // arrays to store the sine and cosine terms of the angle difference
 // values are computed during Jacobian initialization and each iteration 
 //double *cosine_array, *sine_array;
 //cosine_array = (double *)malloc(sizeof(double)*(nnz_e));
 //sine_array = (double *)malloc(sizeof(double)*(nnz_e)); 

	//eG = (double *)malloc(sizeof(double)*(nnz_e)); // G values in Y 
	//eB = (double *)malloc(sizeof(double)*(nnz_e)); // B values in Y
	//ei = (uint *)malloc(sizeof(uint)*(nnz_e)); // column indices of Y
	//ep = (uint *)malloc(sizeof(uint)*(n_e+1)); // initial row pointers	

	//deltaP = (double *)malloc(sizeof(double)*(n)); // b in the Ax=b
	deltaQ = (double *)malloc(sizeof(double)*(n)); // b in the Ax=b

	Vm = (double *)malloc(sizeof(double)*(n)); 
	//Va = (double *)malloc(sizeof(double)*(n)); 

	//Pn = (double *)malloc(sizeof(double)*(n)); 
	//Qn = (double *)malloc(sizeof(double)*(n));

	//btype = (uint *)malloc(sizeof(uint)*(n));

 double sum, diag;
 double  *b;  // *delta,
 //delta = (double *)malloc(sizeof(double)*(n));
 b = (double *)malloc(sizeof(double)*(n));
 
	std::cout << " ======================== Initialization of ararys used to store the factorized matrix and LU ========================"<< std::endl;

	struct timeval t2_st, t2_end, t3_st, t3_end; long seconds, useconds;

 //initialization to 0
 // [tc] use memset here
 memset(Vm, 0, sizeof(double)*(n));
 // memset(Va, 0, sizeof(double)*(n));
 // memset(deltaP, 0, sizeof(double)*(n));
 memset(deltaQ, 0, sizeof(double)*(n));
	
 // =================================== Sort all input HeapAccum =================================
 gettimeofday(&t3_st, 0);

 // Input HeapAccums are assumed to be unsorted. Sort them before assigning values to
 // local arrays
 gettimeofday(&t2_st, 0);
 gVertex_VmDeltaQ.sort_heap();   
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gVertex_VmDeltaQ HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");    
	
/*    gettimeofday(&t2_st, 0);
   gdeltaQ_vertex.sort_heap();
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gdeltaQ_vertex HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");   */
	
	
/*  gettimeofday(&t2_st, 0);
   gdeltaQ_vertex.sort_heap();
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gdeltaQ_vertex HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");   */
 
 // get the time for sorting all HeapAccums 
	gettimeofday(&t3_end, 0);
	seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	useconds = t3_end.tv_usec  - t3_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort all HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
	
	// ========================================= Convert pointers and the Vertex =========================================
	gettimeofday(&t2_st, 0); 
   int i_pp; // i_p, 
	int ii = 1; // counter for the array  
 // Assign value to the first position of the pointer array 
 //lp[0] = 0;
 //up[0] = 0;
 lp_pp[0] = 0;
 up_pp[0] = 0;
 //ep[0] = 0;    
	for (i=0; i<gVertex_VmDeltaQ.size(); ++i){
   //process pointers for the matrices
   //lp[ii] = gVertex_p.data_[i].Lp + lp[ii-1];
   //up[ii] = gVertex_p.data_[i].Up + up[ii-1];
   lp_pp[ii] = gVertex_pp.data_[i].Lp + lp_pp[ii-1];
   up_pp[ii] = gVertex_pp.data_[i].Up + up_pp[ii-1];    
   //ep[ii] = gVertex_Ybus.data_[i].ep + ep[ii-1];
   ii++;
   // [tc] removed 02/08/2017. Cannot directly use pointers stored on vertex
   // see modifications history for details
   //lp[i] = gVertex_p.data_[i].Lp;
   //up[i] = gVertex_p.data_[i].Up;
   //lp_pp[i] = gVertex_pp.data_[i].Lp;
   //up_pp[i] = gVertex_pp.data_[i].Up;
   //process state variables
   Vm[i] = gVertex_VmDeltaQ.data_[i].VValue;
   //Va[i] = gVertex_Ybus.data_[i].Va;
   //Pn[i] = gVertex_Ybus.data_[i].Pn;
   //Qn[i] = gVertex_Ybus.data_[i].Qn;
   //btype[i] = gVertex_Ybus.data_[i].bustype;
	
	  //deltaP[i] = gdeltaPQ_vertex.data_[i].deltaPn;
	  deltaQ[i] = gVertex_VmDeltaQ.data_[i].deltaValue;
	
 
   // debug print
	  // std::cout<< i << ",deltaP,"<< deltaP[i] << ",deltaQ,"<< deltaQ[i] << std::endl;  

   //process factorized information for B'
   //i_p = gVertex_p.data_[i].exId - 1;
   //rows[i]= gVertex_p.data_[i].row_scaling;
   //cols[i]= gVertex_p.data_[i].col_scaling;
   //rp[i] = gVertex_p.data_[i].rp;
   //cpi[i] = gVertex_p.data_[i].cpi;  
   //process factorized information for B"  
   //i_pp = gVertex_pp.data_[i].exId - 1;
   rows_pp[i]= gVertex_pp.data_[i].row_scaling;
   cols_pp[i]= gVertex_pp.data_[i].col_scaling;
   rp_pp[i] = gVertex_pp.data_[i].rp;
   cpi_pp[i] = gVertex_pp.data_[i].cpi;    
	}
 //debug print
 //for (i=0; i<n; ++i){
	// 	std::cout<< i << ",rp," << rp[i]  << ",cpi," << cpi[i] << ",rows,"<< rows[i] << ",cols," << cols[i] <<std::endl;
 //}
 //for (i=0; i<n; ++i){
	// 	std::cout<< i << ",rp_pp," << rp_pp[i]  << ",cpi_pp," << cpi_pp[i] << ",rows_pp,"<< rows_pp[i] << ",cols_pp," << cols_pp[i] <<std::endl;
 //}   
 // [tc] removed 02/08/2017. Cannot directly use pointers stored on vertex
 // see modifications history for details       
 // Assign total number of non-zeros to the last slot of the pointer arrays
 //lp[n] = gLp;
 //up[n] = gUp;
 //lp_pp[n] = gLpp;
 //up_pp[n] = gUpp;  

	// ========================================= Convert Ybus (ei, eG and eB) =========================================
/*  for (ii=0; ii<gMatrix_Ybus.size(); ++ii) {
   ei[ii] = gMatrix_Ybus.data_[ii].ei;
   eG[ii] = gMatrix_Ybus.data_[ii].eG;
   eB[ii] = gMatrix_Ybus.data_[ii].eB; 
 }   */
	// ========================================= Convert L' and U' =========================================
/*  int i_l = 0, i_u = 0;

 for (ii=0; ii<gMatrix_p_L.data_.size(); ++ii) {
   li[ii] = gMatrix_p_L.data_[ii].cpi;
   lx[ii] = gMatrix_p_L.data_[ii].value;      
 } 
 
 for (ii=0; ii<gMatrix_p_U.data_.size(); ++ii) {  
   ui[ii] = gMatrix_p_U.data_[ii].cpi;
   ux[ii] = gMatrix_p_U.data_[ii].value;   
 }   */
 
 //debug print
 /*for (i=0; i<gMatrix_p_L.data_.size(); ++i)
 {
    std::cout<< i << ",lx,"<< lx[i] << ",li,"<< li[i] << std::endl;  
 } 
 for (i=0; i<n+1; ++i){
    std::cout<< i << ",lp,"<< lp[i] << std::endl;
 } 
 for (i=0; i<gMatrix_p_U.data_.size(); ++i)
 {
    std::cout<< i << ",ux,"<< ux[i] << ",ui,"<< ui[i] << std::endl;  
 } 
 for (i=0; i<n+1; ++i){
    std::cout<< i << ",up,"<< up[i] << std::endl;
 }*/
	// ========================================= Convert L" and U" =========================================
 //i_l = 0; 
 //i_u = 0;
 
 for (ii=0; ii<gMatrix_pp_L.data_.size(); ++ii) {
   li_pp[ii] = gMatrix_pp_L.data_[ii].cpi;
   lx_pp[ii] = gMatrix_pp_L.data_[ii].value;
 }
 
 for (ii=0; ii<gMatrix_pp_U.data_.size(); ++ii) {  
   ui_pp[ii] = gMatrix_pp_U.data_[ii].cpi;
   ux_pp[ii] = gMatrix_pp_U.data_[ii].value;
 }
 
 //debug print   
 /*for (i=0; i<gMatrix_pp_L.data_.size(); ++i)
 {
    std::cout<< i << ",lx_pp,"<< lx_pp[i] << ",li_pp,"<< li_pp[i] << std::endl;  
 } 
 for (i=0; i<n+1; ++i){
    std::cout<< i << ",lp_pp,"<< lp_pp[i] << std::endl;
 } 
   for (i=0; i<gMatrix_pp_U.data_.size(); ++i)
 {
    std::cout<< i << ",ux_pp,"<< ux_pp[i] << ",ui_pp,"<< ui_pp[i] << std::endl;  
 } 
 for (i=0; i<n+1; ++i){
    std::cout<< i << ",up_pp,"<< up_pp[i] << std::endl;
 }*/      	    	
 // Done converting all input HeapAccums to arrays
 
	// Get the time to convert the data between containers and arrays
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;

	printf("\n\n============================================================================================== ");
	std::cout << "Time to convert data to GRAPHLU time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");		

 //continue to do the iterations only if there is no error from factorization
	printf("\n\n============================================================================================== ");
	std::cout << "\n Start iteratively updating deltaP and deltaQ " << std::endl;
	printf("\n================================================================================================\n ");

	gettimeofday(&t2_st, 0);
 
		// solve for deltaVm
		// std::cout << " ======================== Solve for V magnitude ========================"<<std::endl;
   
    // A*x = b
    // multiply deltaQ with row scaling (rows_pp) to get b
  	for (i=0; i<n; ++i){
     b[i]=deltaQ[rp_pp[i]]*rows_pp[rp_pp[i]];
  	}
  	
    // Full forward substitution 
  	for (i=0; i<n; ++i)
  	{
  		sum=0.;
  		diag=0.;
  		for(p=lp_pp[i]; p< lp_pp[i+1]; ++p){
  			if (i!=li_pp[p]){ // if it is not diagnal element
  				j=li_pp[p];
  				sum += lx_pp[p] * b[j]; 
  			}
  			else{
  				diag = lx_pp[p];
  			}
  		}
  
  		b[i] -= sum; 
  		b[i] /= diag;
  	} 
  	
    // Full backward substitution 
  	for (i=n-1; i>=0; --i)
  	{
  		sum=0.;
  		for(p=up_pp[i]; p< up_pp[i+1]; ++p){
  			if (i!=ui_pp[p]){ // if it is not diagnal element
  				sum += ux_pp[p] * b[ui_pp[p]]; 
  			}
  		}
  
  		b[i] -= sum; 
  	}      
  	
    // permute the array back to the original order
  	//for (i=0; i<n; ++i)
    //{
  	//	delta[i]=b[cpi_pp[i]];
    //  //std::cout << "delta: " << i << ", " << delta[i] << std::endl;           
		//}
		
    // Update V magnitude (Vm)
		for (i=0; i<n; ++i)
    {
      //Vm[i] -= delta[i];
      Vm[i] -= b[cpi_pp[i]]*cols_pp[cpi_pp[i]];
    }       
	//}
 
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	
	printf("\n\n============================================================================================== ");
	std::cout << "Time for performing power flow iteration: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");

 string result = "Finished Vm Forward/Backward Substitution";

	// ----------------------------------------------------------------------------------------------------
	// 								Store the Result back to the array
	// ----------------------------------------------------------------------------------------------------
	// Get solved Vm and Va back into HeapAccum
	for (ii=0; ii<n; ++ii) {
      //gVertex_Ybus.data_[ii].Va = Va[ii];  // in radian
      gVertex_VmDeltaQ.data_[ii].VValue = Vm[ii];
      //std::cout << "Final, " << ii+1 << ", Vm, " << Vm[ii]<< ", Va" << ii+1 << ", " << Va[ii]/pi_value*180<< std::endl;
	}

 
EXIT:

	// std::cout << "\n free3 " << std::endl;
		
	//free(eG);
	//free(eB);
	//free(ei);
	//free(ep);

	//free(deltaP);
	free(deltaQ);
	free(Vm);
	//free(Va);		
	//free(Pn);
	//free(Qn);	
 
/* 	free(lx);
	free(li);
	free(lp);
 free(ux);
	free(ui);
	free(up); 
 free(rows);
 free(cols);
 free(rp);
 free(cpi);   */ 
	free(lx_pp);
	free(li_pp);
	free(lp_pp);
	free(ux_pp);
	free(ui_pp);
	free(up_pp);
 free(rows_pp);
 free(cols_pp);
 free(rp_pp);
 free(cpi_pp);    
	
 printf("\n\n----------------------------------------------------------------------------------------");
	printf("\t\t End of Running GRAPHLU_fdpf_LU_solve_fb_Vm C function!");
	printf("\t\t ----------------------------------------------------------------------------------------\n\n");	

 return result;
}




// **********************************************************************************************************************************
// Created by: Chen Yuan, chen.yuan@geirina.net
// Date: 03/02/2017
// This code performs fast-decoupled power flow analysis. It is used as a benchmark to assess the time it takes to run a fast-decoupled
// power flow in C++ with GRAPHLU. This version performs LU factorization on the B' and B" matrix, then solve power flow iterations
// with forward/backward substitution on the LU matrices.
// After performing LU factorization, the resulting B' and B" LU factors are stored in SetAccums. In the query, new edges (fill-ins)
// are created, and existing edges are updated with factorized values. 
// History: 
// 11/15/2016: [Ting Chan] added another SetAccum for Bpp fill-in edges
// 01/09/2017: [Ting Chan] added a control parameter (fillin) to specify whether we want to find the fill-ins.
//             1 - find fill-ins
//             0 - otherwise
// 01/17/2017: [Ting Chan] Replaced GraphLU_Solve with our own implementation of forward/backward substitution. 
// 01/18/2017: [Ting Chan] Added two new HeapAccums to store factorized L values (diagonal), scaling factors, permutation and permutation
//             inverse.
// 01/25/2017: [Ting Chan] Replace input parameter "fillin" by "insert_LU". 
//             1 - Dump LU after factorization and store them in Set/HeapAccums
//             0 - Only factorization is performed 
// 01/26/2017: [Ting Chan] 
//             In an attempt to reduce some complexity, try to store the factorized pointer information onto the vertex as well
// 01/30/2017: [Ting Chan] Fixed forward/backward substitution to use cpi instead of rpi
// 02/01/2017: [Ting Chan] Created to be used with schema version flowSchema_LU7_graphsql44.ddl 
// 02/06/2017: [Ting Chan] Updated logic for storing the pointer information
// 02/06/2017: [Ting Chan] Added the update/insert of edges and attributes for factorized LU using "upsert"
// 02/07/2017: [Ting Chan] Added the update of vertex attributes for factorized LU using "upsert"
// 02/22/2017: [Ting Chan] Added STATIC HeapAccums for storing the factorized LU matrices as inputs. After LU factorization, the LUs 
//             are stored in these HeapAccums.
//             Removed "upsert" and all necessary code for inserting/updating vertex/edge. These are no longer needed as the factorized
//             LU are stored using the STATIC HeapAccums. In sequential runs, solve power flow directly using these HeapAccums. 
// 03/02/2017: [Chen Yuan]
//             Divided the expression funtion into two: factorization and forward/backward substitution. In this part, only factorization left
// **********************************************************************************************************************************
template <typename T_vertex, typename vertex_comp, 
         typename T_matrix_all, typename matrix_all_comp,
         typename T_p_L_matrix, typename p_L_matrix_comp,
         typename T_p_U_matrix, typename p_U_matrix_comp, 
         typename T_pp_L_matrix, typename pp_L_matrix_comp,
         typename T_pp_U_matrix, typename pp_U_matrix_comp,
         typename T_vertex_p, typename vertex_p_comp,
         typename T_vertex_pp, typename vertex_pp_comp>
inline string GRAPHLU_fdpf_LU_factorize_only (int64_t& gBp, int64_t& gBpp, 
             HeapAccum<T_vertex, vertex_comp>& gVertex, HeapAccum<T_matrix_all, matrix_all_comp>& gMatrix_all,
             HeapAccum<T_p_L_matrix, p_L_matrix_comp>& gMatrix_p_L, HeapAccum<T_p_U_matrix, p_U_matrix_comp>& gMatrix_p_U, HeapAccum<T_vertex_p, vertex_p_comp>& gVertex_p,
             HeapAccum<T_pp_L_matrix, pp_L_matrix_comp>& gMatrix_pp_L, HeapAccum<T_pp_U_matrix, pp_U_matrix_comp>& gMatrix_pp_U, HeapAccum<T_vertex_pp, vertex_pp_comp>& gVertex_pp) 
			  {

	// extern size_t g_si, g_sd, g_sp;
	printf("\n\n------------------------------------------------------------------------------------------- ");
	printf("\nStart Running GRAPHLU_fdpf_LU_factorize_only function!\n");
	std::cout << "Bp Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gBp << std::endl;
	std::cout << "Bpp Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gBpp << std::endl;
	std::cout << "Y bus Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gMatrix_all.data_.size() << std::endl;
	printf("-------------------------------------------------------------------------------------------- \n\n");	
	// ------------------------------------------------------------------------------------------------
	// 				Initialize variables and arrays
	// ------------------------------------------------------------------------------------------------
	

	// Initialize arrays and variables
	uint__t n, nnz, n_pp, nnz_pp; //n_e, nnz_e;
	int ret, i, j; // p, iter;
	// real__t maxDeltaP=0, maxDeltaQ=0, max_change_ex=maxchange;
	SGraphLU *graphlu, *graphlu_pp;
 
	string result = "FAILED";
 
	//const double pi_value = 3.141592653589793;
	//const int64_t LUflag = 1; 
	// real__t *x, err;	
	// uint__t fctnnz; // number of nonzeros after the matrix is factorized

	// Get the dimension and the nnz of the matrix B' and B"
	n=gVertex.data_.size();	nnz=gBp; n_pp=gVertex.data_.size(); nnz_pp=gBpp;
	//n_e=gVertex.data_.size(); 	nnz_e=gMatrix_all.data_.size(); // get the size of the Y bus matrix
	
	//// Convert vectors to GRAPHLU Data Structure (pointers)
	real__t *ax, *ax_pp; //*deltaP, *deltaQ, *Vm, *Va; *Pn, *Qn; *eG, *eB,
	uint__t *ai, *ai_pp; // *ei, *piv; *btype; 
	//uint__t *ap, *ap_pp, *ep;
   uint__t *ap, *ap_pp; 	

	// arrays to store the sine and cosine terms of the angle difference
	// values are computed during Jacobian initialization and each iteration 
	//real__t *cosine_array, *sine_array;
	//cosine_array = (real__t *)malloc(sizeof(real__t)*(nnz_e));
	//sine_array = (real__t *)malloc(sizeof(real__t)*(nnz_e)); 
	
	ax = (real__t *)malloc(sizeof(real__t)*(nnz)); // values in B' 
	ai = (uint__t *)malloc(sizeof(uint__t)*(nnz)); // column indices of B'
	ap = (uint__t *)malloc(sizeof(uint__t)*(n+1)); // initial row pointers
	// b = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b

	ax_pp = (real__t *)malloc(sizeof(real__t)*(nnz_pp)); // values in B"
	ai_pp = (uint__t *)malloc(sizeof(uint__t)*(nnz_pp)); // column indices of B"
	ap_pp = (uint__t *)malloc(sizeof(uint__t)*(n_pp+1)); // initial row pointers

	//eG = (real__t *)malloc(sizeof(real__t)*(nnz_e)); // G values in Y 
	//eB = (real__t *)malloc(sizeof(real__t)*(nnz_e)); // B values in Y
	//ei = (uint__t *)malloc(sizeof(uint__t)*(nnz_e)); // column indices of Y
	//ep = (uint__t *)malloc(sizeof(uint__t)*(n_e+1)); // initial row pointers	

	//deltaP = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b
	//deltaQ = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b

	//Vm = (real__t *)malloc(sizeof(real__t)*(n)); 
	//Va = (real__t *)malloc(sizeof(real__t)*(n)); 
	

	//Pn = (real__t *)malloc(sizeof(real__t)*(n)); 
	//Qn = (real__t *)malloc(sizeof(real__t)*(n));

	//btype = (uint__t *)malloc(sizeof(uint__t)*(n));

	//double sum, diag;
	//real__t *delta, *b;
	//delta = (real__t *)malloc(sizeof(real__t)*(n));
	//b = (real__t *)malloc(sizeof(real__t)*(n));
 
	std::cout << " ======================== Initialization of ararys used to store the factorized matrix and LU ========================"<< std::endl;

	struct timeval t2_st, t2_end, t3_st, t3_end; long seconds, useconds;

	//initialization to 0
	// [tc] use memset here
	//memset(Vm, 0, sizeof(real__t)*(n));
	//memset(Va, 0, sizeof(real__t)*(n));
	//memset(deltaP, 0, sizeof(real__t)*(n));
	//memset(deltaQ, 0, sizeof(real__t)*(n));
	
	// =================================== Sort all input HeapAccum =================================
	gettimeofday(&t3_st, 0);
	gettimeofday(&t2_st, 0);
	// Input HeapAccums are assumed to be unsorted. Sort them before assigning values to
	// local arrays
	gVertex.sort_heap();
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gVertex HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
 
	gettimeofday(&t2_st, 0);  
	gMatrix_all.sort_heap();
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gMatrix_all HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
   
	// get the time for sorting all HeapAccums 
	gettimeofday(&t3_end, 0);
	seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	useconds = t3_end.tv_usec  - t3_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort all HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 
	
	// ========================================= Convert Ap, Ap_pp, ep and the Vertex =========================================
	gettimeofday(&t2_st, 0); 
	int i_p, i_pp;
	int ii = 1; // counter for the array  
	// Assign value to the first position of the pointer array 
	ap[0] = 0;
	ap_pp[0] = 0;
	//ep[0] = 0;
	for (i=0;i<gVertex.size();i++){
   ap[ii] = gVertex.data_[i].Bp_p + ap[ii-1];
   //std::cout<< ii << ",ap,"<< ap[ii] << std::endl;
   ap_pp[ii] = gVertex.data_[i].Bpp_p + ap_pp[ii-1];
   //ep[ii] = gVertex.data_[i].ep + ep[ii-1];
   ii++;
   //Vm[i] = gVertex.data_[i].Vm;
   //Va[i] = gVertex.data_[i].Va;
   //Pn[i] = gVertex.data_[i].Pn;
   //Qn[i] = gVertex.data_[i].Qn;
   //btype[i] = gVertex.data_[i].bustype;
	}  
	// ========================================= Convert Ybus (ei, eG and eB), B' (ei and Bp_x) and B" (ei and Bpp_x) =========================================
	i_p = 0;
	i_pp = 0;
	for (ii=0; ii<gMatrix_all.size(); ++ii) {
   //ei[ii] = gMatrix_all.data_[ii].ei;
   //eG[ii] = gMatrix_all.data_[ii].eG;
   //eB[ii] = gMatrix_all.data_[ii].eB; 
   if(gMatrix_all.data_[ii].Bp_x != 0)
   {
     ai[i_p] = gMatrix_all.data_[ii].ei;
     ax[i_p] = gMatrix_all.data_[ii].Bp_x;
     //std::cout<< i_p << ",ax,"<< ax[i_p] << ",ai,"<< ai[i_p] << std::endl;
     i_p++;
   }
   if(gMatrix_all.data_[ii].Bpp_x != 0)
   {
     ai_pp[i_pp] = gMatrix_all.data_[ii].ei;
     ax_pp[i_pp] = gMatrix_all.data_[ii].Bpp_x;
     i_pp++;
   }
 } 	
	// Done converting all input HeapAccums to arrays
	
	// get ax, ai, ap, ax_pp, ai_pp, ap_pp information
	for (ii=0; ii<nnz; ++ii){
		std::cout << ii << ", ai, " << ai[ii] << ", ax, " << ax[ii] << std::endl;
	}
	
	for (ii=0; ii<nnz_pp; ++ii){
		std::cout << ii << ", ai_pp, " << ai_pp[ii] << ", ax, " << ax[ii] << std::endl;
	}
	
	for (ii=0; ii<n+1; ++ii){
		std::cout << ii << ", ap, " << ap[ii] <<  std::endl;
	}
	
	for (ii=0; ii<n+1; ++ii){
		std::cout << ii << ", ap_pp, " << ap_pp[ii] <<  std::endl;
	}
 
	// Get the time to convert the data between containers and arrays
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to convert data to GRAPHLU time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");		
	
	// ----------------------------------------------------------------------------------------------------
	// 								Call GRAPHLU and Factorize B' Matirx
	// ----------------------------------------------------------------------------------------------------
	int error_p = 0, error_pp = 0;
	gettimeofday(&t2_st, 0);
	std::cout << "\n ======================== Start factorizing B' Matirx ======================== \n"<< std::endl;
	// Initialize the structure for GRAPHLU
	graphlu = (SGraphLU *)malloc(sizeof(SGraphLU));
	GraphLU_Initialize(graphlu);

	// Create Matrix
	GraphLU_CreateMatrix(graphlu, n, nnz, ax, ai, ap);
	// graphlu->cfgf[0] = 1.;

	// // Set control parameters
	//graphlu->cfgi[1] = 0; // 0 is no MC64 scaling
	// 					 // 1 is with MC64 scaling

	// Analyze (column/row ordering, scaling. For details please refer to GRAPHLU user's guide)
	GraphLU_Analyze(graphlu);
	printf("analysis time: %.8g\n", graphlu->stat[0]);

	// creates the task scheduler for parallel LU factorization. If you want to run 
	// parallel factorization or parallel re-factorization, it should be called after GraphLU Analyze.
	ret = GraphLU_CreateScheduler(graphlu);
	printf("time of creating scheduler: %.8g\n", graphlu->stat[4]);
	printf("suggestion: %s.\n", ret==0?"parallel":"sequential");
	//std::cout << "GraphLU MC64 scaling 4th:" << graphlu->cfgi[1] <<std::endl; 
	// This function creates threads for parallel computation. The second argument (thread)
	// specifies the number of threads, including the main thread. The last argument (check)
	// specifies whether to check the number of threads or not.
 if (!ret){ // parallel factorization 
	  GraphLU_CreateThreads(graphlu, 2, TRUE);
	  printf("total cores: %d, threads created: %d\n", (int)(graphlu->stat[9]), (int)(graphlu->cfgi[5]));
	
	  // This function binds threads to cores (unbind = FALSE) or unbinds threads from cores (unbind = TRUE).
	  GraphLU_BindThreads(graphlu, FALSE);
	
	  // Numerical LU factorization with partial pivoting, parallel
	  error_p = GraphLU_Factorize_MT(graphlu);
	  printf("factorization time: %.8g\n", graphlu->stat[1]);
   if (error_p < 0) //there is an error, print the code 
     std::cout << "GraphLU_Factorize error code:" << error_p <<std::endl;  
 }   
 else{  // Sequential factorization
	  error_p = GraphLU_Factorize(graphlu);
	  printf("Factorization time: %.8g\n", graphlu->stat[1]);
   if (error_p < 0) //there is an error, print the code 
     std::cout << "GraphLU_Factorize error code:" << error_p <<std::endl; 
	}

 real__t *lx, *ux; 
 uint__t *li, *ui; 
 size_t *lp, *up; 
	uint__t fctnnz; // number of nonzeros after the matrix is factorized
	size_t l_nnz, u_nnz; // number of nonzeros in L and U factors
 // get the permutation arrays, please refer to graphlu.h for details
 uint__t *rp, *cp, *rpi, *cpi; //row (column) permutation, row (column) permutation inverse
 real__t *rows, *cols; //*ldiag, *cscale,
 //int__t *pivot, *pivot_inv;
 // rpi = (real__t *)malloc(sizeof(real__t)*(n));
 
 lx = ux = NULL; 
 li = ui = NULL; 
 lp = up = NULL; 

 int row, col, k;
 int LUrow, LUcol;
   
 GraphLU_DumpLU(graphlu, &lx, &li, &lp, &ux, &ui, &up); 
    
  // Get the number of nonzeros in the factorized matrix
	//fctnnz = graphlu->lu_nnz;
	//l_nnz = graphlu->l_nnz;
	//u_nnz = graphlu->u_nnz;

	std::cout << " ======================== Number of Total nonzeros after factorization is: "<< graphlu->lu_nnz << "========================"<<std::endl;
	std::cout << " ======================== Number of nonzeros in L: "<< graphlu->l_nnz << "========================"<<std::endl;
	std::cout << " ======================== Number of nonzeros in U: "<< graphlu->u_nnz << "========================"<<std::endl;
	
  // get the permutation arrays, rp and cp may be different if the matrix is not symmetric
 rp = graphlu->row_perm; // row permutation, permuted row # --> original row #  (original row)
 rpi = graphlu->row_perm_inv;  // row permutation inverse, original row # --> permuted row # (permuted row)
 cp = graphlu->col_perm;
 cpi = graphlu->col_perm_inv;

 //ldiag = graphlu->ldiag; // diagnal elements
 //cscale = graphlu->cscale; // sclaing factors
 rows = graphlu->row_scale; 
 cols = graphlu->col_scale_perm;
 //pivot = graphlu->pivot; 
 //pivot_inv = graphlu->pivot_inv; // pivoting array   
   
	// print out and check
	//for (i=0; i<n; ++i){ // check the permutation arrays
	 //	std::cout << i << ",rp," << rp[i] << ",rpi," << rpi[i] << ",cp," << cp[i] << ",cpi," << cpi[i] << std::endl;
	//} 
 
 //std::cout << " ======================== Get L Factor ========================"<<std::endl;
	/*for (i=0; i<l_nnz; ++i){
    std::cout<< i << ",lx,"<< lx[i] << ",li,"<< li[i] << std::endl; 
 }
  
 for (i=0; i<n+1; ++i){
    std::cout<< i << ",lp,"<< lp[i] << std::endl;
 }
  
 //std::cout << "\n ======================== Get U Factor ========================"<<std::endl;
	for (i=0; i<u_nnz; ++i){
    std::cout<< i << ",ux,"<< ux[i] << ",ui,"<< ui[i] << std::endl;
 }
  
 for (i=0; i<n+1; ++i){
		std::cout<< i << ",up,"<< up[i] << std::endl;
	}*/

	// check arrays
 //for (i=0; i<n; ++i){
	// 	std::cout<< i << ",\t diag:"<< ldiag[i] << ",\t cscale:"<< cscale[i] << ",\t rows:"<< rows[i] << ",\t cols:"<< cols[i] << std::endl;
 //}
 //for (i=0; i<n; ++i){
	// 	std::cout<< i << ",rp," << rp[i]  << ",cpi," << cpi[i] << ",rows,"<< rows[i] << ",cols," << cols[i] <<std::endl;
 //}  

	// // multiply U factor by cols (which is )
	// for (i=0; i<n; ++i){
	// 	for (j=up[i]; j<up[i+1]; ++j){
	// 		p=rp[piv[ui[j]]]; // this may be rp!!!!!!!!!!!
	// 		ux[j] = ux[j]/cols[p]; // include the scaling factor into L factor
	// 	}
	// }

	// ================================= Get the pivoting vectors =================================
	//for (i=0; i<n; ++i){
	//	std::cout<< i << ",\t pivot:"<< pivot[i] << ",\t pivot_inv:"<< pivot_inv[i] << std::endl;
	//}

 
   //get the factorized LU values and find the row and col before permutation
   for (i = 0; i < n; ++i){
     row = rp[i];
     LUrow = row+1;
     
     gVertex_p += T_vertex_p(i+1, lp[i+1]-lp[i], up[i+1]-up[i], rp[i], cpi[i], rows[i], cols[i]); 
     // process all non-zeros in L matrix on row i
     for (j = lp[i]; j < lp[i+1]; ++j){
       col = cp[li[j]];             
       LUcol = col+1;
       
       gMatrix_p_L += T_p_L_matrix((i+1)*100000+(li[j]+1), li[j], lx[j]); 
     } 
        
     // process all non-zeros in U matrix on row i
     for (j = up[i]; j < up[i+1]; ++j){
       col = cp[ui[j]];
       LUcol = col+1; 
       
       gMatrix_p_U += T_p_U_matrix((i+1)*100000+(ui[j]+1), ui[j], ux[j]);          
     }   
   }
   gVertex_p.sort_heap();
   gMatrix_p_L.sort_heap();
   gMatrix_p_U.sort_heap();

 
 //gVertex_p.sort_heap();
 //gMatrix_p_L.sort_heap();
 //gMatrix_p_U.sort_heap();
	// ----------------------------------------------------------------------------------------------------
	// 								Call GRAPHLU and Factorize B" Matirx
	// ----------------------------------------------------------------------------------------------------
	std::cout << "\n ======================== Start factorizing B'' Matirx ======================== \n"<< std::endl;
	// Initialize the structure for GRAPHLU
	graphlu_pp = (SGraphLU *)malloc(sizeof(SGraphLU));
	GraphLU_Initialize(graphlu_pp);

	// Create Matrix
	GraphLU_CreateMatrix(graphlu_pp, n_pp, nnz_pp, ax_pp, ai_pp, ap_pp);
	// graphlu_pp->cfgf[0] = 1.;

	// // Set control parameters
	//graphlu_pp->cfgi[1] = 0; // 0 is no MC64 scaling
	// 					 // 1 is with MC64 scaling

	// Analyze (column/row ordering, scaling. For details please refer to GRAPHLU user's guide)
	GraphLU_Analyze(graphlu_pp);
	printf("analysis time: %.8g\n", graphlu_pp->stat[0]);
 
	// creates the task scheduler for parallel LU factorization. If you want to run 
	// parallel factorization or parallel re-factorization, it should be called after GraphLU Analyze.
	ret = GraphLU_CreateScheduler(graphlu_pp);
	printf("time of creating scheduler: %.8g\n", graphlu_pp->stat[4]);
	printf("suggestion: %s.\n", ret==0?"parallel":"sequential");
	
	// This function creates threads for parallel computation. The second argument (thread)
	// specifies the number of threads, including the main thread. The last argument (check)
	// specifies whether to check the number of threads or not.
 if (!ret){ // parallel factorization
	  GraphLU_CreateThreads(graphlu_pp, 2, TRUE);
	  printf("total cores: %d, threads created: %d\n", (int)(graphlu_pp->stat[9]), (int)(graphlu_pp->cfgi[5]));
	
	  // This function binds threads to cores (unbind = FALSE) or unbinds threads from cores (unbind = TRUE).
	  GraphLU_BindThreads(graphlu_pp, FALSE);
	
	  // Numerical LU factorization with partial pivoting, parallel
	  error_pp = GraphLU_Factorize_MT(graphlu_pp);
	  printf("factorization time: %.8g\n", graphlu_pp->stat[1]);
   if (error_pp < 0) //there is an error, print the code 
     std::cout << "GraphLU_Factorize error code:" << error_pp <<std::endl;      
 }
 else{  //Sequential factorization 
	  error_pp = GraphLU_Factorize(graphlu_pp);
   printf("Factorization time: %.8g\n", graphlu_pp->stat[1]);
   if (error_pp < 0) //there is an error, print the code 
     std::cout << "GraphLU_Factorize error code:" << error_pp <<std::endl; 
 }

 real__t *lx_pp, *ux_pp; 
 uint__t *li_pp, *ui_pp; 
 size_t *lp_pp, *up_pp; 
 //uint__t fctnnz_pp; // number of nonzeros after the matrix is factorized
 //size_t l_nnz_pp, u_nnz_pp; // number of nonzeros in L and U factors
 // get the permutation arrays, please refer to graphlu.h for details
 uint__t *rp_pp, *cp_pp, *rpi_pp, *cpi_pp; //row (column) permutation, row (column) permutation inverse
 real__t *rows_pp, *cols_pp;  //*ldiag_pp, *cscale_pp,
 //int__t *pivot_pp, *pivot_inv_pp; 
 
 lx_pp = ux_pp = NULL; 
 li_pp = ui_pp = NULL; 
 lp_pp = up_pp = NULL; 
 
 
 GraphLU_DumpLU(graphlu_pp, &lx_pp, &li_pp, &lp_pp, &ux_pp, &ui_pp, &up_pp); 
  
  // Get the number of nonzeros in the factorized matrix
	//fctnnz_pp = graphlu_pp->lu_nnz;
	//l_nnz_pp = graphlu_pp->l_nnz;
	//u_nnz_pp = graphlu_pp->u_nnz;

	std::cout << " ======================== Number of Total nonzeros after factorization is: "<< graphlu_pp->lu_nnz << "========================"<<std::endl;
	std::cout << " ======================== Number of nonzeros in L: "<< graphlu_pp->l_nnz << "========================"<<std::endl;
	std::cout << " ======================== Number of nonzeros in U: "<< graphlu_pp->u_nnz << "========================"<<std::endl;
	
 // get the permutation arrays, rp and cp may be different if the matrix is not symmetric
 rp_pp = graphlu_pp->row_perm;
 rpi_pp = graphlu_pp->row_perm_inv;
 cp_pp = graphlu_pp->col_perm;
 cpi_pp = graphlu_pp->col_perm_inv;
 
 //ldiag_pp = graphlu_pp->ldiag; // diagnal elements
 //cscale_pp = graphlu_pp->cscale; // sclaing factors
 rows_pp = graphlu_pp->row_scale; 
 cols_pp = graphlu_pp->col_scale_perm;
 //pivot_pp = graphlu_pp->pivot; 
 //pivot_inv_pp = graphlu_pp->pivot_inv; // pivoting array   
   
	// print out and check
	//for (i=0; i<n; ++i){ // check the permutation arrays
	// 	std::cout << "rp_pp," << rp_pp[i] << ",rpi_pp," << rpi_pp[i] << ",cp_pp,"<< cp_pp[i] << ",cpi_pp," << cpi_pp[i] << std::endl;
	//} 
 
 //std::cout << " ======================== Get L Factor ========================"<<std::endl;
	/*for (i=0; i<l_nnz_pp; ++i){
   std::cout<< i << ",lx_pp,"<< lx_pp[i] << ",li_pp,"<< li_pp[i] << std::endl; 
 }
  
 for (i=0; i<n+1; ++i){
   std::cout<< i << ",lp_pp,"<< lp_pp[i] << std::endl;
 }
  
  //std::cout << "\n ======================== Get U Factor ========================"<<std::endl;
	for (i=0; i<u_nnz_pp; ++i){
   std::cout<< i << ",ux_pp,"<< ux_pp[i] << ",ui_pp,"<< ui_pp[i] << std::endl;
 }
  
	for (i=0; i<n+1; ++i){
		std::cout<< i << ",up_pp,"<< up_pp[i] << std::endl;
	}*/

	// check arrays
 //for (i=0; i<n; ++i){
	// 	std::cout<< i << ",\t diag_pp:"<< ldiag_pp[i] << ",\t cscale_pp:"<< cscale_pp[i] << ",\t rows_pp:"<< rows_pp[i] << ",\t cols_pp:"<< cols_pp[i] << std::endl;
 //}
 //for (i=0; i<n; ++i){
	// 	std::cout<< i << ",rp_pp," << rp_pp[i]  << ",cpi_pp," << cpi_pp[i] << ",rows_pp,"<< rows_pp[i] << ",cols_pp," << cols_pp[i] <<std::endl;
 //}   

	// // multiply U factor by cols (which is )
	// for (i=0; i<n; ++i){
	// 	for (j=up[i]; j<up[i+1]; ++j){
	// 		p=rp[piv[ui[j]]]; // this may be rp!!!!!!!!!!!
	// 		ux[j] = ux[j]/cols[p]; // include the scaling factor into L factor
	// 	}
	// }

	// ================================= Get the pivoting vectors =================================
	//for (i=0; i<n; ++i){
	//	std::cout<< i << ",\t pivot_pp:"<< pivot_pp[i] << ",\t pivot_inv_pp:"<< pivot_inv_pp[i] << std::endl;
	//}
 
     	
   //get the factorized LU values and find the row and col before permutation
   for (i = 0; i < n; ++i){
     row = rp_pp[i];
     LUrow = row+1;
     
     gVertex_pp += T_vertex_pp(i+1, lp_pp[i+1]-lp_pp[i], up_pp[i+1]-up_pp[i], rp_pp[i], cpi_pp[i], rows_pp[i], cols_pp[i]); 
     // process all non-zeros in L matrix on row i
     for (j = lp_pp[i]; j < lp_pp[i+1]; ++j){
       col = cp_pp[li_pp[j]];
       LUcol = col+1;
       
       gMatrix_pp_L += T_pp_L_matrix((i+1)*100000+(li_pp[j]+1), li_pp[j], lx_pp[j]); 
     } 
     
     // process all non-zeros in U matrix on row i
     for (j = up_pp[i]; j < up_pp[i+1]; ++j){
       col = cp_pp[ui_pp[j]];
       LUcol = col+1;
       
       gMatrix_pp_U += T_pp_U_matrix((i+1)*100000+(ui_pp[j]+1), ui_pp[j], ux_pp[j]);
     }    
   }
   gVertex_pp.sort_heap();
   gMatrix_pp_L.sort_heap();
   gMatrix_pp_U.sort_heap();          

 
 // Get the time for factorizing B' and B"
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	
	printf("\n\n============================================================================================== ");
	std::cout << "Time for GRAPHLU to factorize B' and B'':: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");
 
 if(error_p >= 0 && error_pp >= 0){
     result = "Factorization Completed";
     printf("\n\n============================================================================================== ");
 	  std::cout << "Factorization Completed" << std::endl;
 	  printf("\n================================================================================================ "); 
   //continue to do the iterations only if there is no error from factorization
//  	printf("\n\n============================================================================================== ");
//  	std::cout << "\n Start iteratively updating deltaP and deltaQ " << std::endl;
//  	printf("\n================================================================================================\n ");
//  

//  	// ----------------------------------------------------------------------------------------------------
//  	// 								Store the Result back to the array
//  	// ----------------------------------------------------------------------------------------------------
//  	// Get solved Vm and Va back into HeapAccum
 	//for (ii=0; ii<n; ++ii) {
       //gVertex.data_[ii].Va = Va[ii]/pi_value*180;
       //gVertex.data_[ii].Vm = Vm[ii];
       //gVertex.data_[ii].rpi = rpi[ii];
		//gVertex.data_[ii].rpi_pp = rpi_pp[ii];
       //std::cout << "Final, " << ii+1 << ", Vm, " << Vm[ii]<< ", Va" << ii+1 << ", " << Va[ii]/pi_value*180<< std::endl;
 	//}

 } else {
   //factorization failed
	  result = "Factorization FAILED";
   printf("\n\n============================================================================================== ");
 	std::cout << "Factorization FAILED" << std::endl;
 	printf("\n================================================================================================ ");  
 }
EXIT:
	// std::cout << "\n free1 " << std::endl;
	GraphLU_Destroy(graphlu);
	GraphLU_Destroy(graphlu_pp);
	// std::cout << "\n free2 " << std::endl;
	free(ax);
	free(ai);
	free(ap);
	free(ax_pp);
	free(ai_pp);
	free(ap_pp);
	free(graphlu);
	free(graphlu_pp);
	// std::cout << "\n free3 " << std::endl;
		
	//free(eG);
	//free(eB);
	//free(ei);
	//free(ep);

	//free(deltaP);
	//free(deltaQ);
	//free(Vm);
	//free(Va);		
	//free(Pn);
	//free(Qn);	
 
	free(lx);
	free(li);
	free(lp);
   free(ux);
	free(ui);
	free(up);  
	free(lx_pp);
	free(li_pp);
	free(lp_pp);
	free(ux_pp);
	free(ui_pp);
	free(up_pp);    
	
 printf("\n\n----------------------------------------------------------------------------------------");
	printf("\t\t End of Running GRAPHLU_fdpf_LU_factorize_only C function!");
	printf("\t\t ----------------------------------------------------------------------------------------\n\n");	

 return result;
 
}



// **********************************************************************************************************************************
 // Created by: Yiting Zhao, yitingzhao6777@gmail
 // Date: 05/02/2017
 // This code performs fast-decoupled power flow analysis. It is used as a benchmark to assess the time it takes to run a fast-decoupled
 // power flow in C++ with GRAPHLU. This version does NOT perform LU factorization. B' and B" are assumed to be previously factorized and
 // the resulting L and U matrices are taken as inputs. Power flow iterations are solved with forward/backward substitution.
 // In this function, the HeapAccum for Ybus matrix  and factorized LU matrices are assumed to be built and sorted in the past. 
 // Sorting of HeapAccums is only done for the one that contains state variables.
 // History: 
 // 11/15/2016: [Ting Chan] added another SetAccum for Bpp fill-in edges
 // 01/09/2017: [Ting Chan] added a control parameter (fillin) to specify whether we want to find the fill-ins.
 //             1 - find fill-ins
 //             0 - otherwise
 // 01/17/2017: [Ting Chan] Replaced GraphLU_Solve with our own implementation of forward/backward substitution. 
 // 01/18/2017: [Ting Chan] Added two new HeapAccums to store factorized L values (diagonal), scaling factors, permutation and permutation
 //             inverse.
 // 01/20/2017: [Ting Chan] B' and B" have previously been factorized. HeapAccums for the factorized values, index and pointers are 
 //             taken as inputs. Peform sorting of the HeapAccum first, then convert them into arrays for power flow iteration and
 //             forward/backward substitution.
 // 01/23/2017: [Ting Chan] Consolidated HeapAccums to reduce sorting time.
 // 01/30/2017: [Ting Chan] Fixed forward/backward substitution to use cpi instead of rpi
 // 01/31/2017: [Ting Chan] Added sizes of L', U' L", and U" HeapAccums as inputs
 // 02/01/2017: [Ting Chan] Created to be used with schema version flowSchema_LU7_graphsql44.ddl 
 // 02/03/2017: [Ting Chan] Added cols and cols_pp for column scaling factor
 // 02/06/2017: [Ting Chan] Use Lflag and Uflag to decide whether an non-zero element should be added to lx or ux
 //             (I think this step might not be necessary and can probably be implemented in a better way by directly using
 //              Lvalue and U value, but do this for now)
 // 02/08/2017: [Ting Chan] Use Lvalue and Uvalue to decide whether an non-zero element should be added to lx or ux instead of
 //             Lflag and Uflag. (see previous comment)
 // 02/08/2017: [Ting Chan] With the previous change, we can no longer use Lpointer and Upointer stored on the vertex directly.
 //             This is discovered during testing of the sc_20160818_1120 case as there are zero elements in the output LU
 //             structure from GraphLU_DumpLU. This should be investigated. At the meantime, update the logic to get the pointer
 //             information.
 // 02/17/2017: [Ting Chan] All HeapAccums except the Ybus vertex which contains state variables are static and have been built and sorted
 //             so there is no need to perform sorting.
 // 02/22/2017: [Ting Chan] L' and U' are represented by two seperate HeapAccums. Updated array creation logic.
 // 02/23/2017: [Ting Chan] Removed the LU sizing input parameters. The LU number of non-zeros can be obtained from the HeapAccums directly.
 // 03/08/2017: [Chen Yuan] 
 //             Removed B' and B'' factorization and update of deltaP and deltaQ. Only do V angle forward/backward subsitution. 
 //             All static HeapAccums obtained from GRAPHLU_fdpf_LU_factorize_only after it ran in GraphSQL. HeapAccums for updated deltaP and deltaQ  //             are built in GraphSQL and sorted in this function.
 // 04/27/2017: [Yiting Zhao] Modified for ILU filter; removed MC64 scaling
 // 05/03/2017: [Yiting Zhao] Added deltaPn calculation and converted back to GSQL. 	
 // 09/16/2017:[Yiting Zhao] copy the GRAPHLU_fdpf_ILU_solve6_Bp and turn on the MC64. This query will be used for the fdpf_ca_v5, 
 //                          and download the L&U matrix from the graph which uploaded after the fdpf_ca_v5_basecase_fac.
 // 02/13/2018: [Yiting Zhao] Download the matrices Bp, Lp, Up from basecase factorized by the fdpf_ca_v6_basecase_fac.gsql.
 // 02/20/2018: [Yiting Zhao] Directly call this ExprFunction from CA main-query to avoid data converting and processing.
 // **********************************************************************************************************************************
/*
inline double fdpf_ca_superposition_solve7 (int from_bus, int to_bus, double delta_phi, double delta_b) {
				  
		printf("\nStart Running fdpf_ca_superposition_solve7 function!\n");
        struct timeval t1_st, t1_end, t2_st, t2_end, t3_st, t3_end; long seconds, useconds;
		gettimeofday(&t1_st, 0);
		string result = "fdpf_ca_superposition_solve7 failed";
	// ------------------------------------------------------------------------------------------------
	// 				Initialize variables and arrays
	// ------------------------------------------------------------------------------------------------
    gettimeofday(&t2_st, 0);
	 CSRMatrix *Bp = SingletonMatrixInterface::getMatrixBp();
	 CSRMatrix *Bp_LineQ1 = SingletonMatrixInterface::getMatrixLineQ1();
	 CSRMatrix *Bp_L = SingletonMatrixInterface::getMatrixLp();
	 CSRMatrix *Bp_U = SingletonMatrixInterface::getMatrixUp();
	

	 double *lx, *ux, *rows, *cols, *Va, *Va_base, *line_Q1; 
	 real__t *ax_basecase, *ax;
	 uint__t  *li, *ui, *rp, *cpi, *ap,*ai; 
	 size_t *lp, *up; 
	 uint__t n, nnz, nnz_p_L, nnz_p_U, n_e, nnz_e; //, nnz_p_B; 
	 int i, j, p, iter;
	 
	 ax = Bp->getX();
	 ai = Bp->getI();
	 ap = Bp->getBp();
	 Va_base = Bp->getVa();	
	 nnz=Bp->getnnz();
     line_Q1 = Bp_LineQ1->getX();
	
	 lx = Bp_L->getX();
	 li = Bp_L->getI();
	 lp = Bp_L->getP();
		
	 ux = Bp_U->getX();
	 ui = Bp_U->getI();
	 up = Bp_U->getP();
	
	 rp = Bp_L->getRp();
	 cpi = Bp_L->getCpi();
	 rows = Bp_L->getRows();
	 cols = Bp_L->getCols();
	 
	 n=Bp_L->getn();	
	 nnz_p_L=Bp_L->getnnz(); 
	 nnz_p_U=Bp_U->getnnz();
	 
	 Va = (double *)malloc(sizeof(double)*(n));
	 memcpy(Va, Va_base, sizeof(double)*(n));

	 
	 // [Chen Yuan] Debug
	// for(int i = 0; i < n; i++)
         // {
          // std::cout << "i" << i << "Va_base" << Va_base[i] << std::endl;
	  // std::cout << "Va" << Va[i] << std::endl;
         // }
	 // for(int i = 0; i < nnz_p_L; i++)
          // {
           // std::cout << "li" << li[i] << std::endl;
           // std::cout << "lx" << lx[i] << std::endl;
          // }

	  // for(int i = 0; i < nnz_p_U; i++)
          // {
           // std::cout << "ui" << ui[i] << std::endl;
           // std::cout << "ux" << ux[i] << std::endl;
          // }
 
	gettimeofday(&t2_end, 0);
	 seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	 useconds = t2_end.tv_usec  - t2_st.tv_usec;
	 // printf("\n\n============================================================================================== ");
	 std::cout << "Total Time to convert L&U matrix:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	 // printf("\n================================================================================================ ");    
	 // printf("\n\n------------------------------------------------------------------------------------------- ");
     // std::cout << "Bp Number of rows:" << n << ",\tNumber of nonzeros:" << gMatrix_p_B.size() << std::endl;
     // std::cout << "Lp Number of rows:" << n << ",\tNumber of nonzeros:" << nnz_p_L << std::endl;
     // std::cout << "Up Number of rows:" << n << ",\tNumber of nonzeros:" << nnz_p_U << std::endl;
     // std::cout << "Y bus Number of rows:" << n << ",\tNumber of nonzeros:" << nnz_e << std::endl;
	 // printf("-------------------------------------------------------------------------------------------- \n\n");	

     std::cout<< "n,"<< n <<  ",nnz,"<< nnz <<  ",nnz_e,"<< nnz_e << std::endl; 	
	 
	 // std::cout << " \n======================== Get Bp Factor ========================\n"<<std::endl;	
	 // for (i=0; i<nnz; ++i){
       // std::cout<< i << ",download_ai,"<< ai[i] << ",download_ax,"<< ax[i] << ",download_lineQ1,"<< line_Q1[i] << std::endl; 	
     // }
   
     // for (i=0; i<n; ++i){
      // std::cout<< i << ",download_ap,"<< ap[i] << std::endl;
     // }	 
	
// 	  std::cout<< "nnz_p_L,"<< nnz_p_L << ",nnz_p_U,"<< nnz_p_U << std::endl; 	
	 
//	 std::cout << "\n ======================== Get L Factor ========================\n"<<std::endl;	
//	 for (i=0; i<nnz_p_L; ++i){
 //      std::cout<< i << ",download_lx,"<< lx[i] << ",li,"<< li[i] << std::endl; 	
 //    }
   
 //    for (i=0; i<n; ++i){
 //     std::cout<< i << ",download_lp,"<< lp[i+1] << std::endl;
//     }
   
 //    std::cout << "\n ======================== Get U Factor ========================\n"<<std::endl;
 //	 for (i=0; i<nnz_p_U; ++i){
 //     std::cout<< i << ",download_ux,"<< ux[i] << ",ui,"<< ui[i] << std::endl;
 //    }
   
//     for (i=0; i<n; ++i){
// 	  std::cout<< i << ",download_up,"<< up[i+1] << std::endl;
// 	 } 
  	
     double sum, diag;
     double *b;
     b = (double *)malloc(sizeof(double)*(n));
	 memset(b, 0, sizeof(double)*(n));

	
	// ==============================calculate the compensation deltaP
	gettimeofday(&t2_st, 0); 
  
    // double delta_phi = 0.0;
	double tmpMBM = 0.0;
	double comp_P = 0.0;

  	// =================== calculate M * inv(L) matrix ======================== 
	  	// Full forward substitution 
   	for (i=0; i<n; ++i)
   	{
		if (rp[i]==from_bus-1){
            b[i]= 1; 
		}else if (rp[i]==to_bus-1){
			b[i]= -1; 
		}
        // std::cout<< i <<", Va,"<< Va[i] << ", Va_base,"<< Va_base[i] << std::endl;
 		// std::cout<< rp[i] <<", Mpq,"<< b[i] << std::endl;
   		sum=0.;
   		diag=0.;
   		for(p=lp[i]; p< lp[i+1]; ++p){
   			if (i!=li[p]){ // if it is not diagnal element
   				j=li[p];
   				sum += lx[p] * b[j];
   			}
   			else{
   				diag = lx[p];
   			}
   		}
   		b[i] -= sum; 
   		b[i] /= diag; 
		tmpMBM -= b[i] * diag * b[i]; 
   	} 

    // std::cout << "======= Outage from_bus: " << from_bus << " => to_bus :" << to_bus << std::endl;	
	// std::cout << "delta_phi " << delta_phi << ", delta_b =" << delta_b  << ", tmpMBM =" << tmpMBM <<std::endl; 
	
	comp_P = delta_phi/(-1.0/delta_b + tmpMBM);
	
	std::cout << " Outage from_bus: " << from_bus << " => to_bus :" << to_bus <<" comp_P =" << comp_P <<std::endl;
	
	gettimeofday(&t2_end, 0);
 	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
 	useconds = t2_end.tv_usec  - t2_st.tv_usec;
 	
 	// printf("\n\n============================================================================================== ");
 	std::cout << "Time for the compensation deltaP :"<< (seconds*1000000 + useconds) << " us" << std::endl;
 	// printf("\n================================================================================================ ");
	
	  // std::cout << " ======================== Solve for V angle ========================"<<std::endl;
	gettimeofday(&t2_st, 0); 

   	// Full forward substitution 
   	for (i=0; i<n; ++i)
   	{
	    if (rp[i]==from_bus-1){
             b[i]= (-1)*comp_P; 
		}else if (rp[i]==to_bus-1){
			 b[i]= comp_P; 
		}else{
			 b[i] = 0.0;
		}
        // std::cout<< rp[i] << ",comp_P,"<< b[i] << std::endl;	
		
   		sum=0.;
   		diag=0.;
   		for(p=lp[i]; p< lp[i+1]; ++p){
   			if (i!=li[p]){ // if it is not diagnal element
   				j=li[p];
   				sum += lx[p] * b[j];
   			}
   			else{
   				diag = lx[p];
   			}
   		}
   		b[i] -= sum; 
   		b[i] /= diag; 
   	} 
    
   	// Full backward substitution 
   	for (i=n-1; i>=0; --i)
   	{
   		sum=0.;
   		for(p=up[i]; p< up[i+1]; ++p){
   			if (i!=ui[p]){ // if it is not diagnal element
   				sum += ux[p] * b[ui[p]]; 
   			}
   		}
   
   		b[i] -= sum; 
		// gVertex_VaDeltaP += T_vertex_VaDeltaP(rp[i]+1, b[i]); 
	    //std::cout <<  i << " Solved, " << rp[i]+1 << ", deltaPhi " << b[i] << std::endl;
   	}      
 
    result = "fdpf_ca_superposition_solve7 Successfully Solved";	
 	gettimeofday(&t2_end, 0);
 	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
 	useconds = t2_end.tv_usec  - t2_st.tv_usec;
 	std::cout << "Time for V angle Solve:"<< (seconds*1000000 + useconds) << " us" << std::endl;

	for (i =0; i<n;i++) {
		Va[i] -= b[cpi[i]];	
		// std::cout<< i <<", Va,"<< Va[i] << std::endl;
	}
	
	//  count the branch violations
	double BranchViolation = 0.0;
	for (i =0; i<n;i++) {
		for (j=ap[i];j<ap[i+1];j++){
         p=ai[j];
		 if (i!=p){
         double s = ax[j]*(Va[i] - Va[p])/line_Q1[j];
//		 std::cout << "s, " << s << "ax, " << ax[j] << "Va[i]" << Va[i] << "Va[p]" << Va[p] << std::endl;	 
         // std::cout << "branch violations: i=>p,"<< i <<","<< p << ",ax," <<ax[j]<<",Va[i]," <<Va[i]<<",Va[p]," <<Va[p]<< ",s," << s <<std::endl;		 
		 if (s>1){
		    BranchViolation += s;
		 }
		 }		 
		}				 
	 }

EXIT:
		
	free(b);
	free(Va);
	
    // printf("\n\n----------------------------------------------------------------------------------------");
	// printf("\t\t End of Running fdpf_ca_superposition_solve7 C function!");
	// printf("\t\t ----------------------------------------------------------------------------------------\n\n");
	  
	  gettimeofday(&t1_end, 0);
  	  seconds=t1_end.tv_sec  - t1_st.tv_sec; 
  	  useconds = t1_end.tv_usec  - t1_st.tv_usec;
  	  // printf("\n\n===============================================================================\n ");
  	  std::cout << "\n Time for fdpf_ca_superposition_solve7:" << (seconds*1000000 + useconds) << " us" << std::endl;
	  std::cout << "\t\t End of Running fdpf_ca_superposition_solve7 C function ==========================================================================\n!" << std::endl;
  	  // printf("\n=================================================================================\n ");	
 // return result;
  return BranchViolation;
}
*/

inline ListAccum<UDIMPL::ca_linedetail> fdpf_ca_superposition_solve7 (int64_t from_bus, int64_t to_bus, double delta_phi, double delta_b) {
	 
		printf("\nStart Running fdpf_ca_superposition_solve7 function!\n");  
        struct timeval t1_st, t1_end, t2_st, t2_end, t3_st, t3_end; long seconds, useconds;
		gettimeofday(&t1_st, 0);
		string result = "fdpf_ca_superposition_solve7 failed";
	// ------------------------------------------------------------------------------------------------
	// 				Initialize variables and arrays
	// ------------------------------------------------------------------------------------------------
    gettimeofday(&t2_st, 0);
	 CSRMatrix *Bp = SingletonMatrixInterface::getMatrixBp();
	 CSRMatrix *Bp_LineQ1 = SingletonMatrixInterface::getMatrixLineQ1();
	 CSRMatrix *Bp_L = SingletonMatrixInterface::getMatrixLp();
	 CSRMatrix *Bp_U = SingletonMatrixInterface::getMatrixUp();
	
	//[Xuan Zhang] define return list ca_detail
	//ca_tuplelist ca_detail;
	ListAccum<UDIMPL::ca_linedetail> ca_detail;
	
	 double *lx, *ux, *rows, *cols, *Va, *Va_base, *line_Q1; 
	 real__t *ax_basecase, *ax;
	 uint__t  *li, *ui, *rp, *cpi, *ap,*ai; 
	 size_t *lp, *up; 
	 uint__t n, nnz, nnz_p_L, nnz_p_U, n_e, nnz_e; //, nnz_p_B; 
	 int i, j, p, iter;
	 
	 ax = Bp->getX();
	 ai = Bp->getI();
	 ap = Bp->getBp();
	 Va_base = Bp->getVa();	
	 nnz=Bp->getnnz();
     	 line_Q1 = Bp_LineQ1->getX();
	
	 lx = Bp_L->getX();
	 li = Bp_L->getI();
	 lp = Bp_L->getP();
		
	 ux = Bp_U->getX();
	 ui = Bp_U->getI();
	 up = Bp_U->getP();
	
	 rp = Bp_L->getRp();
	 cpi = Bp_L->getCpi();
	 rows = Bp_L->getRows();
	 cols = Bp_L->getCols();
	 
	 n=Bp_L->getn();	
	 nnz_p_L=Bp_L->getnnz(); 
	 nnz_p_U=Bp_U->getnnz();
	 
	 Va = (double *)malloc(sizeof(double)*(n));
	 memcpy(Va, Va_base, sizeof(double)*(n));
/* 
	 for(int i = 0; i < n; i++)
         {
          std::cout << "line_Q1" << line_Q1[i] << std::endl;
         }
*/

/*
	for(int i = 0; i < n; i++)
         {
          std::cout << "Va_base" << Va_base[i] << std::endl;
	  std::cout << "Va" << Va[i] << std::endl;
         }
	for(int i = 0; i < nnz_p_L; i++)
         {
          std::cout << "li" << li[i] << std::endl;
          std::cout << "lx" << lx[i] << std::endl;
         }

	 for(int i = 0; i < nnz_p_U; i++)
         {
          std::cout << "ui" << ui[i] << std::endl;
          std::cout << "ux" << ux[i] << std::endl;
         }
*/	 
	gettimeofday(&t2_end, 0);
	 seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	 useconds = t2_end.tv_usec  - t2_st.tv_usec;
	 // printf("\n\n============================================================================================== ");
	 std::cout << "Total Time to convert L&U matrix:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	 // printf("\n================================================================================================ ");    
	 // printf("\n\n------------------------------------------------------------------------------------------- ");
     // std::cout << "Bp Number of rows:" << n << ",\tNumber of nonzeros:" << gMatrix_p_B.size() << std::endl;
     // std::cout << "Lp Number of rows:" << n << ",\tNumber of nonzeros:" << nnz_p_L << std::endl;
     // std::cout << "Up Number of rows:" << n << ",\tNumber of nonzeros:" << nnz_p_U << std::endl;
     // std::cout << "Y bus Number of rows:" << n << ",\tNumber of nonzeros:" << nnz_e << std::endl;
	 // printf("-------------------------------------------------------------------------------------------- \n\n");	

     std::cout<< "n,"<< n <<  ",nnz,"<< nnz <<  ",nnz_e,"<< nnz_e << std::endl; 	
	 
	 // std::cout << " \n======================== Get Bp Factor ========================\n"<<std::endl;	
	 // for (i=0; i<nnz; ++i){
       // std::cout<< i << ",download_ai,"<< ai[i] << ",download_ax,"<< ax[i] << ",download_lineQ1,"<< line_Q1[i] << std::endl; 	
     // }
   
     // for (i=0; i<n; ++i){
      // std::cout<< i << ",download_ap,"<< ap[i] << std::endl;
     // }	 
	
/* 	  std::cout<< "nnz_p_L,"<< nnz_p_L << ",nnz_p_U,"<< nnz_p_U << std::endl; 	
	 
	 std::cout << "\n ======================== Get L Factor ========================\n"<<std::endl;	
	 for (i=0; i<nnz_p_L; ++i){
       std::cout<< i << ",download_lx,"<< lx[i] << ",li,"<< li[i] << std::endl; 	
     }
   
     for (i=0; i<n; ++i){
      std::cout<< i << ",download_lp,"<< lp[i+1] << std::endl;
     }
   
     std::cout << "\n ======================== Get U Factor ========================\n"<<std::endl;
 	 for (i=0; i<nnz_p_U; ++i){
      std::cout<< i << ",download_ux,"<< ux[i] << ",ui,"<< ui[i] << std::endl;
     }
   
     for (i=0; i<n; ++i){
 	  std::cout<< i << ",download_up,"<< up[i+1] << std::endl;
 	 } */
  	
     double sum, diag;
     double *b;
     b = (double *)malloc(sizeof(double)*(n));
	 memset(b, 0, sizeof(double)*(n));

	
	// ==============================calculate the compensation deltaP
	gettimeofday(&t2_st, 0); 
  
    // double delta_phi = 0.0;
	double tmpMBM = 0.0;
	double comp_P = 0.0;

  	// =================== calculate M * inv(L) matrix ======================== 
	  	// Full forward substitution 
   	for (i=0; i<n; ++i)
   	{
		if (rp[i]==from_bus-1){
            b[i]= 1; 
		}else if (rp[i]==to_bus-1){
			b[i]= -1; 
		}
        // std::cout<< i <<", Va,"<< Va[i] << ", Va_base,"<< Va_base[i] << std::endl;
 		// std::cout<< rp[i] <<", Mpq,"<< b[i] << std::endl;
   		sum=0.;
   		diag=0.;
   		for(p=lp[i]; p< lp[i+1]; ++p){
   			if (i!=li[p]){ // if it is not diagnal element
   				j=li[p];
   				sum += lx[p] * b[j];
   			}
   			else{
   				diag = lx[p];
   			}
   		}
   		b[i] -= sum; 
   		b[i] /= diag; 
		tmpMBM -= b[i] * diag * b[i]; 
   	} 

    // std::cout << "======= Outage from_bus: " << from_bus << " => to_bus :" << to_bus << std::endl;	
	// std::cout << "delta_phi " << delta_phi << ", delta_b =" << delta_b  << ", tmpMBM =" << tmpMBM <<std::endl; 
	
	comp_P = delta_phi/(-1.0/delta_b + tmpMBM);
	
	std::cout << " Outage from_bus: " << from_bus << " => to_bus :" << to_bus <<" comp_P =" << comp_P <<std::endl;
	
	gettimeofday(&t2_end, 0);
 	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
 	useconds = t2_end.tv_usec  - t2_st.tv_usec;
 	
 	// printf("\n\n============================================================================================== ");
 	std::cout << "Time for the compensation deltaP :"<< (seconds*1000000 + useconds) << " us" << std::endl;
 	// printf("\n================================================================================================ ");
	
	  // std::cout << " ======================== Solve for V angle ========================"<<std::endl;
	gettimeofday(&t2_st, 0); 

   	// Full forward substitution 
   	for (i=0; i<n; ++i)
   	{
	    if (rp[i]==from_bus-1){
             b[i]= (-1)*comp_P; 
		}else if (rp[i]==to_bus-1){
			 b[i]= comp_P; 
		}else{
			 b[i] = 0.0;
		}
        // std::cout<< rp[i] << ",comp_P,"<< b[i] << std::endl;	
		
   		sum=0.;
   		diag=0.;
   		for(p=lp[i]; p< lp[i+1]; ++p){
   			if (i!=li[p]){ // if it is not diagnal element
   				j=li[p];
   				sum += lx[p] * b[j];
   			}
   			else{
   				diag = lx[p];
   			}
   		}
   		b[i] -= sum; 
   		b[i] /= diag; 
   	} 
    
   	// Full backward substitution 
   	for (i=n-1; i>=0; --i)
   	{
   		sum=0.;
   		for(p=up[i]; p< up[i+1]; ++p){
   			if (i!=ui[p]){ // if it is not diagnal element
   				sum += ux[p] * b[ui[p]]; 
   			}
   		}
    
   		b[i] -= sum; 
		// gVertex_VaDeltaP += T_vertex_VaDeltaP(rp[i]+1, b[i]); 
	    //std::cout <<  i << " Solved, " << rp[i]+1 << ", deltaPhi " << b[i] << std::endl;
   	}      
 
    result = "fdpf_ca_superposition_solve7 Successfully Solved";	
 	gettimeofday(&t2_end, 0);
 	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
 	useconds = t2_end.tv_usec  - t2_st.tv_usec;
 	std::cout << "Time for V angle Solve:"<< (seconds*1000000 + useconds) << " us" << std::endl;

	for (i =0; i<n;i++) {
		Va[i] -= b[cpi[i]];	
		// std::cout<< i <<", Va,"<< Va[i] << std::endl;
	}
	
	//  count the branch violations
	double BranchViolation = 0.0;
	//SI: severity index
	double SI = 0.0;
	int counter = 0;
	for (i =0; i<n;i++) {
		for (j=ap[i];j<ap[i+1];j++){
         p=ai[j];
		 if (i!=p){
//         double s = ax[j]*(Va[i] - Va[p])/line_Q1[j];
	 double s = ax[j]*(Va[i] - Va[p])/20;
		 //std::cout << "s_value, " << s << std::endl;
		 //std::cout << "ax, " << ax[j] << "Va[i]" << Va[i] << "Va[p]" << Va[p] << std::endl;	 
         // std::cout << "branch violations: i=>p,"<< i <<","<< p << ",ax," <<ax[j]<<",Va[i]," <<Va[i]<<",Va[p]," <<Va[p]<< ",s," << s <<std::endl;		
		//std::cout << "s, " << s << std::endl;	 		 
		 if (s>1 and counter <= 10){
		    //BranchViolation += s;
			counter ++ ;
			BranchViolation += 1;
			SI = (s-20)*(s-20);
//			SI = (s-line_Q1[j])*(s-line_Q1[j]);
			//ca_detail.push_back(tuple<int, int, int, int, double>(from_bus, to_bus, i, j, s));
			ca_detail.data_.push_back(UDIMPL::ca_linedetail(from_bus, to_bus, i, p, s));
			//Gip.data_.emplace_back(T_P(map1.first, map1.second));  
			
		 }
		 }		 
		}				 
	 } 
//	 printf("Test print out ca tuple ==========================================================================\n!");
//	 for ( const auto& tuple : ca_detail ) {
//		std::cout << "from_bus:"<< get<0>(tuple) << ", to_bus:" <<get<1>(tuple)<< ", cafrom_bus:" << get<2>(tuple)<< ", cato_bus:"<< get<3>(tuple)<< ", SI:"<< get<4>(tuple) << std::endl;
//	 }

EXIT:
		
	free(b);
        free(Va);	
    // printf("\n\n----------------------------------------------------------------------------------------");
	// printf("\t\t End of Running fdpf_ca_superposition_solve7 C function!");
	// printf("\t\t ----------------------------------------------------------------------------------------\n\n");
	  
	  gettimeofday(&t1_end, 0);
  	  seconds=t1_end.tv_sec  - t1_st.tv_sec; 
  	  useconds = t1_end.tv_usec  - t1_st.tv_usec;
  	  // printf("\n\n===============================================================================\n ");
  	  std::cout << "\n Time for fdpf_ca_superposition_solve7:" << (seconds*1000000 + useconds) << " us" << std::endl;
	  printf("\t\t End of Running fdpf_ca_superposition_solve7 C function ==========================================================================\n!");
  	  // printf("\n=================================================================================\n ");	
 // return result;
 //return BranchViolation;
 return ca_detail;
}

// **********************************************************************************************************************************
// Created by: Jingjin Wu
// Date: 08/22/2017
// This function is to show the critical edge results which will be used in UI.
// **********************************************************************************************************************************
template <typename T_vertex_tuple, typename vertex_comp,
		  typename T_edge_all_tuple, typename edge_all_comp,
          typename T_Bridge_vertex, typename Bridge_vertex_comp>
inline string bridge_UI (HeapAccum<T_vertex_tuple, vertex_comp>& gVertex,
              HeapAccum<T_edge_all_tuple, edge_all_comp>& gEdge_all,
	          HeapAccum<T_Bridge_vertex, Bridge_vertex_comp>& gVertex_bridge) {

	// extern size_t g_si, g_sd, g_sp;
	printf("\n\n------------------------------------------------------------------------------------------- ");
	printf("\nStart Running bridge_UI C function!\n");
    std::cout << "Y bus Number of rows:" << gVertex.data_.size() << ",\tNumber of nonzeros:" << gEdge_all.data_.size() << std::endl;
	printf("-------------------------------------------------------------------------------------------- \n\n");
	// ------------------------------------------------------------------------------------------------
	// 				Initialize variables and arrays
	// ------------------------------------------------------------------------------------------------

	int i;
	string result;
	// Initialize arrays and variables
	uint__t n, n_e, nnz_e;

	n=gVertex.data_.size();
	n_e=gVertex.data_.size(); 	nnz_e=gEdge_all.data_.size(); // get the size of the Y bus addmittance

	uint__t *ei;
	uint__t *ep;

	ei = (uint__t *)malloc(sizeof(uint__t)*(nnz_e)); // column indices of Y
	ep = (uint__t *)malloc(sizeof(uint__t)*(n_e+1)); // initial row pointers

	std::cout << " ======================== Initialization of ararys used to store the factorized matrix and LU ========================"<< std::endl;

	struct timeval t2_st, t2_end, t3_st, t3_end; long seconds, useconds;

  // =================================== Sort all input HeapAccum =================================
  gettimeofday(&t3_st, 0);
  gettimeofday(&t2_st, 0);
  // Input HeapAccums are assumed to be unsorted. Sort them before assigning values to
  // local arrays
  gVertex.sort_heap();
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec;
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gVertex HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");

  gettimeofday(&t2_st, 0);
  gEdge_all.sort_heap();
 	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec;
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort gEdge_all HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");

  // get the time for sorting the HeapAccums
	gettimeofday(&t3_end, 0);
	seconds=t3_end.tv_sec  - t3_st.tv_sec;
	useconds = t3_end.tv_usec  - t3_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to sort all HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================\n ");

	// ========================================= Convert  ep and the Vertex =========================================
	gettimeofday(&t2_st, 0);
  int i_p, i_pp;
  int ii = 1; // counter for the array
  // Assign value to the first position of the pointer array
  ep[0] = 0;
  for (i=0;i<gVertex.size();i++){
      ep[ii] = gVertex.data_[i].ep + ep[ii-1];
      ii++;
	}
 	// ========================================= Convert Ybus (ei) =========================================
  i_p = 0;
  i_pp = 0;
  for (ii=0; ii<gEdge_all.size(); ++ii) {
    ei[ii] = gEdge_all.data_[ii].ei;
  }

  // Done converting all input HeapAccums to arrays

	// Get the time to convert the data between containers and arrays
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec;
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
//	printf("\n\n============================================================================================== ");
	std::cout << "Time to convert data to NICSLU time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
//	printf("\n================================================================================================ ");

     vector<int> singleDegreeNodes;
     vector<pair<int, int>> edges;
  //   MatrixGraph Ybus(n, ei, ep);
   //  Ybus.findSingleDegreeNodes(singleDegreeNodes);
   //  Ybus.findBridgesWithoutSingleDegreeNodes(edges);
     // Ybus.findBridges(edges);

     for (i = 0; i < edges.size(); ++i){
	  gVertex_bridge += T_Bridge_vertex((edges[i].first+1)*100000+edges[i].second+1, edges[i].first+1, edges[i].second+1);
     }


     gVertex_bridge.sort_heap();

     for(int i = 0; i < singleDegreeNodes.size(); i++) {
       std::cout << "one degree nodes: " << singleDegreeNodes[i] << std::endl;
     }

     for(int i = 0; i < edges.size(); i++) {
        if (edges[i].first != edges[i].second)
	std::cout << "bridge first node: " << edges[i].first << " second node: " << edges[i].second << std::endl;
     }

    result = "OK";

	free(ei);
	free(ep);

    printf("\n\n----------------------------------------------------------------------------------------");
	std::cout << "result: " << result << std::endl;
	printf("\t\t End of Running bridge_UI C function!");
	printf("\t\t ----------------------------------------------------------------------------------------\n\n");

  return result;
}

#endif /* EXPRFUNCTIONS_HPP_ */

