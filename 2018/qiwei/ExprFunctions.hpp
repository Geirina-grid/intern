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

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <algorithm> // Header file needed for ListAccum sorting
#include <gle/engine/cpplib/headers.hpp>

/**     XXX Warning!! Put self-defined struct in ExprUtil.hpp **
 *  No user defined struct, helper functions (that will not be directly called
 *  in the GQuery scripts) etc. are allowed in this file. This file only
 *  contains user-defined expression function's signature and body.
 *  Please put user defined structs, helper functions etc. in ExprUtil.hpp
 */
#include "ExprUtil.hpp"
//================ Header files needed for GLM =============
#include <fstream>
#include <vector>
//================ Header files needed for BFS =============
//============================================================
//// ============= Header files needed for NISCLU =============

#include "graphlu.h"
#include "graphlu_util.h"
#include <ctime>
#include <unistd.h>
#include <sys/time.h>
#include <unordered_set> // use for ILU filter

// #include <chrono>
// typedef std::chrono::high_resolution_clock Clock;
//// ==========================================================
//
//=======================Header file needed for Wenlei's Demo===============
 #include<iostream>
 #include <math.h>
 #include "matrix_h"
//===================================================================

namespace UDIMPL {
  typedef std::string string; //XXX DON'T REMOVE

  /****** BIULT-IN FUNCTIONS **************/
  /****** XXX DON'T REMOVE ****************/
  inline int str_to_int (string str) {
    return atoi(str.c_str());
  }

  inline int float_to_int (float val) {
    return (int) val;
  }

  inline string to_string (double val) {
    char result[200];
    sprintf(result, "%g", val);
    return string(result);
  }
  
// **********************************************************************************************************************************
// Created by: Wenlei Bai, baiwenlei123@gmail.com
// Date: 07/06/2017
// This function calculates the N by N complex matrix inverse and multiplication.
// History: 
// **********************************************************************************************************************************
template <typename T > // define a generic typename T so that any types can be used. 
inline void matCalculateA(ArrayAccum<T>&Areal, ArrayAccum<T>&Aimg, ArrayAccum<T>&areal, ArrayAccum<T>&aimg)
{
	double content = 0;
	bool success = true;
	int i,j;
	int N = 3;
	double temp_V;
	matrix <double> inputreal(N,N);  // 'matrix' is a defined class from 'matrix_h' file. This code is to instaniate new objects of the class.
	matrix<double> inputimg(N,N);
	matrix <double> outputreal(N,N); 
	matrix<double> outputimg(N,N);
	matrix <double> m(2*N,2*N);
	
	// set inputreal matrix value:	
	for(int i=0; i<N;i++)
	{
		for(int j=0;j<N;j++)
		{
			temp_V = areal.getValue({i,j});
			inputreal.setvalue(i,j,temp_V);
		}
	}
	// set inputimg matrix value:
	for(int i=0; i<N;i++)
	{
		for(int j=0;j<N;j++)
		{
			temp_V = aimg.getValue({i,j});
			inputimg.setvalue(i,j,temp_V);
		}
	}
	
	
	// combine inputreal and inputimg to a big matrix 'm' to be calculated for its inverse.	
	for(int i=0;i<N;i++)
	{ 
		for(int j=0;j<N;j++)
		{
			inputreal.getvalue(i,j, content, success);
		    m.setvalue(i,j, content);
			inputimg.getvalue(i,j, content, success);
			m.setvalue(i,j+N,content);
			inputimg.getvalue(i,j, content, success);
			m.setvalue(i+N,j,-content);
			inputreal.getvalue(i,j, content, success);
			m.setvalue(i+N,j+N,content);
		}
	}
	
	m.invert();  //calculate matrix inversion
	
	// split the big 'm' matrix to get the outputreal and outputimg matrix.
	for(i=0;i<N;i++)
	{
		for(j=0;j<N;j++)
		{
			m.getvalue(i,j,content, success);
			outputreal.setvalue(i,j,content);
			m.getvalue(i,j+N,content,success);
			outputimg.setvalue(i,j,content);
		}
	}
	
	// transfer data in outputreal and outputimg to Areal and Aimg as ArrayAccum
	for (i = 0; i < N; i++) 
	{
		for (j = 0; j < N; j++)
		{
			outputreal.getvalue(i,j,content,success);
			Areal.data_[i*3+j] = content;
		}
	}
	
	for (i = 0; i < N; i++) 
	{
		for (j = 0; j < N; j++)
		{
			outputimg.getvalue(i,j,content,success);
			Aimg.data_[i*3+j] = content;
		}
	}
}	


//Multiplying two matrices A and b to output B;
template <typename TT > // define arbitrary typename TT so that 
inline void matCalculateB(ArrayAccum<TT>&Areal, ArrayAccum<TT>&Aimg, ArrayAccum<TT>&breal, ArrayAccum<TT>&bimg, ArrayAccum<TT>&Breal, ArrayAccum<TT>&Bimg)
{
	double sum = 0;
	double content = 0;
	double content2 = 0;
	double content3 = 0;
	double content4 = 0;
	bool success = true;
	int N = 3;
	int i, j, k;
	matrix <double> temp1(N,N);  // invoke the header file of matrix_h which contains 'matrix' class 
	matrix <double> temp2(N,N);
	matrix <double> temp3(N,N);  // invoke the header file of matrix_h which contains 'matrix' class 
	matrix <double> temp4(N,N);
	
	// calculate Areal * breal
	for(i=0; i<N; i++)
	{
		for(j=0; j<N; j++)
		{
			sum=0;
			for(k=0; k<N; k++)
			{
				//std::cout << "old sum:" << sum << std::endl;
				sum = sum + Areal.getValue({i,k}) * breal.getValue({k,j});
			}
			temp1.setvalue(i,j,sum);
			//B.data_[i*3+j] = sum;
		}
	}
	
	// calculate Aimg * bimg
	for(i=0; i<N; i++)
	{
		for(j=0; j<N; j++)
		{
			sum=0;
			for(k=0; k<N; k++)
			{
				//std::cout << "old sum:" << sum << std::endl;
				sum = sum + Aimg.getValue({i,k}) * bimg.getValue({k,j});
			}
			temp2.setvalue(i,j,sum);
		}
	}
	
		// calculate Aimg * breal
	for(i=0; i<N; i++)
	{
		for(j=0; j<N; j++)
		{
			sum=0;
			for(k=0; k<N; k++)
			{
				//std::cout << "old sum:" << sum << std::endl;
				sum = sum + Aimg.getValue({i,k}) * breal.getValue({k,j});
			}
			temp3.setvalue(i,j,sum);
		}
	}
	
		// calculate Areal * bimg
	for(i=0; i<N; i++)
	{
		for(j=0; j<N; j++)
		{
			sum=0;
			for(k=0; k<N; k++)
			{
				//std::cout << "old sum:" << sum << std::endl;
				sum = sum + Areal.getValue({i,k}) * bimg.getValue({k,j});
			}
			temp4.setvalue(i,j,sum);
		}
	}
	
	for (i = 0; i < N; i++)
	{
		for(j = 0; j < N; j++)
		{
			temp1.getvalue(i,j,content,success);
			temp2.getvalue(i,j,content2,success);
			Breal.data_[i*3+j] = content - content2;
			temp3.getvalue(i,j,content3,success);
			temp4.getvalue(i,j,content4,success);
			Bimg.data_[i*3+j] = content3 + content4;
		}
	}	
}  
//************************************************************************************************************
//Functions for forward-backward sweep *****************************
//Developed by Jun Tan, juntan@geirina.net*************************
//Forward backward sweep
template<typename sort_edge, typename edge_comp, typename sort_vertex, typename vertex_comp>
inline void FBS(HeapAccum<sort_edge, edge_comp>& Edge_D, HeapAccum<sort_vertex, vertex_comp>& Vertex_D, int64_t Maxit, double Maxchange, int64_t SNindex, int64_t &Final_iter, double &Final_accuracy)
{
	// Initialize arrays and variables
	uint__t ne, nv;
	int i, j, k, f, c, n, in, tn, lv, iter, endflag, CRflag;
	int *TapA, *TapB, *TapC;
	double Change, Accuracy, tempVregAR, tempVregAI, tempVregBR, tempVregBI, tempVregCR, tempVregCI, tempIcompAR, tempIcompAI, tempIcompBR, tempIcompBI, tempIcompCR, tempIcompCI, VrelyAR, VrelyAI, VrelyBR, VrelyBI, VrelyCR, VrelyCI, VrelyAmag, VrelyBmag, VrelyCmag, TapVlow_A, TapVhigh_A, TapVlow_B, TapVhigh_B, TapVlow_C, TapVhigh_C;
	const double pi_value = 3.141592653589793;	
	// For radio network ne=nv-1
	ne = Edge_D.data_.size(); 
	nv = Vertex_D.data_.size();	
	uint__t *Terminalbuses, *Links, *Childbuses, *Fatherbuses, *tempfather, *bustype, *edgetype, *countf, *countc, *branchf, *ID, *IDF, *IDT, *LM, *control_phase_reg;
	real__t *VphaseAreal, *VphaseAimg, *VphaseBreal, *VphaseBimg, *VphaseCreal, *VphaseCimg, *VAmag, *VBmag, *VCmag, *VAang, *VBang, *VCang, *VABmag, *VBCmag, *VCAmag, *VphaseABreal, *VphaseABimg, *VphaseBCreal, *VphaseBCimg, *VphaseCAreal, *VphaseCAimg, *Vbase, *CapA, *CapB, *CapC, *CapA_cal, *CapB_cal, *CapC_cal, *PdA, *QdA, *PdB, *QdB, *PdC, *QdC, *PdA_cal, *QdA_cal, *PdB_cal, *QdB_cal, *PdC_cal, *QdC_cal, *PdAB_cal, *QdAB_cal, *PdBC_cal, *QdBC_cal, *PdCA_cal, *QdCA_cal, *aRa, *aRb, *aRc, *dRa, *dRb, *dRc, *IphaseAreal, *IphaseAimg, *IphaseBreal, *IphaseBimg, *IphaseCreal, *IphaseCimg, *IphaseABreal, *IphaseABimg, *IphaseBCreal, *IphaseBCimg, *IphaseCAreal, *IphaseCAimg, *Raa, *Rab, *Rac, *Rba, *Rbb, *Rbc, *Rca, *Rcb, *Rcc, *Xaa, *Xab, *Xac, *Xba, *Xbb, *Xbc, *Xca, *Xcb, *Xcc, *Nt, *PT_reg, *CT_reg, *BW_reg, *vol_reg_A, *vol_reg_B, *vol_reg_C;
	 
	Terminalbuses = (uint__t *)malloc(sizeof(uint__t)*(nv-1)); 
	Childbuses = (uint__t *)malloc(sizeof(uint__t)*(nv-1)); 
	Fatherbuses = (uint__t *)malloc(sizeof(uint__t)*(nv-1)); 
	tempfather = (uint__t *)malloc(sizeof(uint__t)*(nv-1)); 
	countf = (uint__t *)malloc(sizeof(uint__t)*(nv)); 
	countc = (uint__t *)malloc(sizeof(uint__t)*(nv)); 
    branchf = (uint__t *)malloc(sizeof(uint__t)*(nv));
	bustype = (uint__t *)malloc(sizeof(uint__t)*(nv));
	ID = (uint__t *)malloc(sizeof(uint__t)*(nv));
	IDF = (uint__t *)malloc(sizeof(uint__t)*(nv));
	IDT = (uint__t *)malloc(sizeof(uint__t)*(nv));
    VphaseAreal = (real__t *)malloc(sizeof(real__t)*(nv)); 
	VphaseAimg = (real__t *)malloc(sizeof(real__t)*(nv));
	VphaseBreal = (real__t *)malloc(sizeof(real__t)*(nv)); 
	VphaseBimg = (real__t *)malloc(sizeof(real__t)*(nv)); 
	VphaseCreal = (real__t *)malloc(sizeof(real__t)*(nv));
	VphaseCimg = (real__t *)malloc(sizeof(real__t)*(nv));
	VAmag = (real__t *)malloc(sizeof(real__t)*(nv)); 
	VBmag = (real__t *)malloc(sizeof(real__t)*(nv));
	VCmag = (real__t *)malloc(sizeof(real__t)*(nv)); 
	VAang = (real__t *)malloc(sizeof(real__t)*(nv)); 
	VBang = (real__t *)malloc(sizeof(real__t)*(nv));
	VCang = (real__t *)malloc(sizeof(real__t)*(nv)); 
	VABmag = (real__t *)malloc(sizeof(real__t)*(nv)); 
	VBCmag = (real__t *)malloc(sizeof(real__t)*(nv));
	VCAmag = (real__t *)malloc(sizeof(real__t)*(nv));
	VphaseABreal = (real__t *)malloc(sizeof(real__t)*(nv)); 
	VphaseABimg = (real__t *)malloc(sizeof(real__t)*(nv));
	VphaseBCreal = (real__t *)malloc(sizeof(real__t)*(nv)); 
	VphaseBCimg = (real__t *)malloc(sizeof(real__t)*(nv)); 
	VphaseCAreal = (real__t *)malloc(sizeof(real__t)*(nv));
	VphaseCAimg = (real__t *)malloc(sizeof(real__t)*(nv)); 
	Vbase = (real__t *)malloc(sizeof(real__t)*(nv)); 
	CapA = (real__t *)malloc(sizeof(real__t)*(nv)); 
	CapB = (real__t *)malloc(sizeof(real__t)*(nv));
	CapC = (real__t *)malloc(sizeof(real__t)*(nv)); 
	CapA_cal = (real__t *)malloc(sizeof(real__t)*(nv)); 
	CapB_cal = (real__t *)malloc(sizeof(real__t)*(nv));
	CapC_cal = (real__t *)malloc(sizeof(real__t)*(nv));
	PdA = (real__t *)malloc(sizeof(real__t)*(nv)); 
	QdA = (real__t *)malloc(sizeof(real__t)*(nv));
	PdB = (real__t *)malloc(sizeof(real__t)*(nv)); 
	QdB = (real__t *)malloc(sizeof(real__t)*(nv)); 
	PdC = (real__t *)malloc(sizeof(real__t)*(nv));
	QdC = (real__t *)malloc(sizeof(real__t)*(nv));
	PdA_cal = (real__t *)malloc(sizeof(real__t)*(nv)); 
	QdA_cal = (real__t *)malloc(sizeof(real__t)*(nv));
	PdB_cal = (real__t *)malloc(sizeof(real__t)*(nv)); 
	QdB_cal = (real__t *)malloc(sizeof(real__t)*(nv)); 
	PdC_cal = (real__t *)malloc(sizeof(real__t)*(nv));
	QdC_cal = (real__t *)malloc(sizeof(real__t)*(nv));
	PdAB_cal = (real__t *)malloc(sizeof(real__t)*(nv)); 
	QdAB_cal = (real__t *)malloc(sizeof(real__t)*(nv));
	PdBC_cal = (real__t *)malloc(sizeof(real__t)*(nv)); 
	QdBC_cal = (real__t *)malloc(sizeof(real__t)*(nv)); 
	PdCA_cal = (real__t *)malloc(sizeof(real__t)*(nv));
	QdCA_cal = (real__t *)malloc(sizeof(real__t)*(nv));
    TapA = (int *)malloc(sizeof(int)*(nv));
	TapB = (int *)malloc(sizeof(int)*(nv));
	TapC = (int *)malloc(sizeof(int)*(nv));
	LM = (uint__t *)malloc(sizeof(uint__t)*(nv));
    aRa = (real__t *)malloc(sizeof(real__t)*(nv));
	aRb = (real__t *)malloc(sizeof(real__t)*(nv));
	aRc = (real__t *)malloc(sizeof(real__t)*(nv));
	dRa = (real__t *)malloc(sizeof(real__t)*(nv));
	dRb = (real__t *)malloc(sizeof(real__t)*(nv));
	dRc = (real__t *)malloc(sizeof(real__t)*(nv));
	PT_reg = (real__t *)malloc(sizeof(real__t)*(nv));	
	CT_reg = (real__t *)malloc(sizeof(real__t)*(nv));
	BW_reg = (real__t *)malloc(sizeof(real__t)*(nv));
	vol_reg_A = (real__t *)malloc(sizeof(real__t)*(nv));
	vol_reg_B = (real__t *)malloc(sizeof(real__t)*(nv));
	vol_reg_C = (real__t *)malloc(sizeof(real__t)*(nv));
	control_phase_reg = (uint__t *)malloc(sizeof(uint__t)*(nv));
	IphaseAreal = (real__t *)malloc(sizeof(real__t)*(nv));
	IphaseAimg = (real__t *)malloc(sizeof(real__t)*(nv));
	IphaseBreal = (real__t *)malloc(sizeof(real__t)*(nv));
	IphaseBimg = (real__t *)malloc(sizeof(real__t)*(nv));
	IphaseCreal = (real__t *)malloc(sizeof(real__t)*(nv));
	IphaseCimg = (real__t *)malloc(sizeof(real__t)*(nv));
	IphaseABreal = (real__t *)malloc(sizeof(real__t)*(nv));
	IphaseABimg = (real__t *)malloc(sizeof(real__t)*(nv));
	IphaseBCreal = (real__t *)malloc(sizeof(real__t)*(nv));
	IphaseBCimg = (real__t *)malloc(sizeof(real__t)*(nv));
	IphaseCAreal = (real__t *)malloc(sizeof(real__t)*(nv));
	IphaseCAimg = (real__t *)malloc(sizeof(real__t)*(nv));
	edgetype = (uint__t *)malloc(sizeof(uint__t)*(ne));
    Links = (uint__t *)malloc(sizeof(uint__t)*(ne));
	Raa = (real__t *)malloc(sizeof(real__t)*(ne));
	Rab = (real__t *)malloc(sizeof(real__t)*(ne));
	Rac = (real__t *)malloc(sizeof(real__t)*(ne));
	Rba = (real__t *)malloc(sizeof(real__t)*(ne));
	Rbb = (real__t *)malloc(sizeof(real__t)*(ne));
	Rbc = (real__t *)malloc(sizeof(real__t)*(ne));
	Rca = (real__t *)malloc(sizeof(real__t)*(ne));
	Rcb = (real__t *)malloc(sizeof(real__t)*(ne));
	Rcc = (real__t *)malloc(sizeof(real__t)*(ne));
	Xaa = (real__t *)malloc(sizeof(real__t)*(ne));
	Xab = (real__t *)malloc(sizeof(real__t)*(ne));
	Xac = (real__t *)malloc(sizeof(real__t)*(ne));
	Xba = (real__t *)malloc(sizeof(real__t)*(ne));
	Xbb = (real__t *)malloc(sizeof(real__t)*(ne));
	Xbc = (real__t *)malloc(sizeof(real__t)*(ne));
	Xca = (real__t *)malloc(sizeof(real__t)*(ne));
	Xcb = (real__t *)malloc(sizeof(real__t)*(ne));
	Xcc = (real__t *)malloc(sizeof(real__t)*(ne));
	Nt = (real__t *)malloc(sizeof(real__t)*(ne));
	std::vector<std::vector<std::vector<double>>>areal(ne,vector<vector<double>>(3,vector<double>(3,0)));
	std::vector<std::vector<std::vector<double>>>aimg(ne,vector<vector<double>>(3,vector<double>(3,0)));
	std::vector<std::vector<std::vector<double>>>breal(ne,vector<vector<double>>(3,vector<double>(3,0)));
	std::vector<std::vector<std::vector<double>>>bimg(ne,vector<vector<double>>(3,vector<double>(3,0)));
	std::vector<std::vector<std::vector<double>>>creal(ne,vector<vector<double>>(3,vector<double>(3,0)));
	std::vector<std::vector<std::vector<double>>>cimg(ne,vector<vector<double>>(3,vector<double>(3,0)));
	std::vector<std::vector<std::vector<double>>>Areal(ne,vector<vector<double>>(3,vector<double>(3,0)));
	std::vector<std::vector<std::vector<double>>>Aimg(ne,vector<vector<double>>(3,vector<double>(3,0)));
	std::vector<std::vector<std::vector<double>>>Breal(ne,vector<vector<double>>(3,vector<double>(3,0)));
	std::vector<std::vector<std::vector<double>>>Bimg(ne,vector<vector<double>>(3,vector<double>(3,0)));
	std::vector<std::vector<std::vector<double>>>at(ne,vector<vector<double>>(3,vector<double>(3,0)));
    std::vector<std::vector<std::vector<double>>>btreal(ne,vector<vector<double>>(3,vector<double>(3,0)));
    std::vector<std::vector<std::vector<double>>>btimg(ne,vector<vector<double>>(3,vector<double>(3,0)));
	std::vector<std::vector<std::vector<double>>>dt(ne,vector<vector<double>>(3,vector<double>(3,0)));
    std::vector<std::vector<std::vector<double>>>At(ne,vector<vector<double>>(3,vector<double>(3,0)));
	
    // Initialization to zero
	memset(countf, 0, sizeof(uint__t)*(nv));
	memset(countc, 0, sizeof(uint__t)*(nv));
	memset(branchf, 0, sizeof(uint__t)*(nv));
	memset(bustype, 0, sizeof(uint__t)*(nv));
	memset(TapA, 0, sizeof(int)*(nv));
	memset(TapB, 0, sizeof(int)*(nv));
	memset(TapC, 0, sizeof(int)*(nv));
	memset(LM, 0, sizeof(uint__t)*(nv));
	memset(aRa, 1, sizeof(real__t)*(nv));
	memset(aRb, 1, sizeof(real__t)*(nv));
	memset(aRc, 1, sizeof(real__t)*(nv));
	memset(dRa, 1, sizeof(real__t)*(nv));
	memset(dRb, 1, sizeof(real__t)*(nv));
	memset(dRc, 1, sizeof(real__t)*(nv));
	memset(PT_reg, 0, sizeof(real__t)*(nv));
    memset(CT_reg, 0, sizeof(real__t)*(nv));
	memset(BW_reg, 0, sizeof(real__t)*(nv));
	memset(vol_reg_A, 0, sizeof(real__t)*(nv));
	memset(vol_reg_B, 0, sizeof(real__t)*(nv));
	memset(vol_reg_C, 0, sizeof(real__t)*(nv));
	memset(control_phase_reg, 0, sizeof(uint__t)*(nv));
	memset(VphaseAreal, 0, sizeof(real__t)*(nv));
	memset(VphaseAimg, 0, sizeof(real__t)*(nv));
	memset(VphaseBreal, 0, sizeof(real__t)*(nv));
	memset(VphaseBimg, 0, sizeof(real__t)*(nv));
	memset(VphaseCreal, 0, sizeof(real__t)*(nv));
	memset(VphaseCimg, 0, sizeof(real__t)*(nv));
	memset(IphaseAreal, 0, sizeof(real__t)*(nv));
	memset(IphaseAimg, 0, sizeof(real__t)*(nv));
	memset(IphaseBreal, 0, sizeof(real__t)*(nv));
	memset(IphaseBimg, 0, sizeof(real__t)*(nv));
	memset(IphaseCreal, 0, sizeof(real__t)*(nv));
	memset(IphaseCimg, 0, sizeof(real__t)*(nv));
    memset(Terminalbuses, 0, sizeof(uint__t)*(nv-1));
	memset(Childbuses, 0, sizeof(uint__t)*(nv-1));
	memset(Fatherbuses, 0, sizeof(uint__t)*(nv-1));
	memset(tempfather, 0, sizeof(uint__t)*(nv-1));
	memset(Links, 0, sizeof(uint__t)*(ne));
	memset(edgetype, 0, sizeof(uint__t)*(ne));
	
	Edge_D.sort_heap();  
	Vertex_D.sort_heap();
	
	
	for (i=0; i<nv; i++)
	{
		ID[i] = Vertex_D.data_[i].IDNO;
		Vbase[i] = Vertex_D.data_[i].Vbase;
		CapA[i] = Vertex_D.data_[i].CapA;
		CapB[i] = Vertex_D.data_[i].CapB;
		CapC[i] = Vertex_D.data_[i].CapC;
		PdA[i] = Vertex_D.data_[i].PdA;
		QdA[i] = Vertex_D.data_[i].QdA;
		PdB[i] = Vertex_D.data_[i].PdB;
		QdB[i] = Vertex_D.data_[i].QdB;
		PdC[i] = Vertex_D.data_[i].PdC;
		QdC[i] = Vertex_D.data_[i].QdC;
		TapA[i] = Vertex_D.data_[i].TapA;
		TapB[i] = Vertex_D.data_[i].TapB;
		TapC[i]	 = Vertex_D.data_[i].TapC;
		branchf[i] = Vertex_D.data_[i].branchf;
		bustype[i] = Vertex_D.data_[i].bustype;
        PT_reg[i] = Vertex_D.data_[i].PT_reg;
		CT_reg[i] = Vertex_D.data_[i].CT_reg;
		BW_reg[i] = Vertex_D.data_[i].BW_reg;
		vol_reg_A[i] = Vertex_D.data_[i].vol_reg_A;
		vol_reg_B[i] = Vertex_D.data_[i].vol_reg_B;
		vol_reg_C[i] = Vertex_D.data_[i].vol_reg_C;
		if (strcmp(Vertex_D.data_[i].control_phase_reg.c_str(),"A")==0)
		{
			control_phase_reg[i] = 1;
		}
		else if (strcmp(Vertex_D.data_[i].control_phase_reg.c_str(),"B")==0)
		{
			control_phase_reg[i] = 2;
		}
		else if (strcmp(Vertex_D.data_[i].control_phase_reg.c_str(),"C")==0)
		{
			control_phase_reg[i] = 3;
		}
		if (strcmp(Vertex_D.data_[i].Load_Model.c_str(),"Y-PQ")==0)
		{
			LM[i] = 1;
		}
		else if (strcmp(Vertex_D.data_[i].Load_Model.c_str(),"Y-Z")==0)
		{
			LM[i] = 2;
		}
		else if (strcmp(Vertex_D.data_[i].Load_Model.c_str(),"Y-I")==0)
		{
			LM[i] = 3;
		}
		else if (strcmp(Vertex_D.data_[i].Load_Model.c_str(),"D-PQ")==0)
		{
			LM[i] = 4;
		}
		else if (strcmp(Vertex_D.data_[i].Load_Model.c_str(),"D-Z")==0)
		{
			LM[i] = 5;
		}else if (strcmp(Vertex_D.data_[i].Load_Model.c_str(),"D-I")==0)
		{
			LM[i] = 6;
		}
		else
		{
			LM[i] = 0;
		}
	}
	
	for (i=0; i<ne; i++)
	{
		IDF[i] = Edge_D.data_[i].IDF;
		IDT[i] = Edge_D.data_[i].IDT;
		Raa[i] = Edge_D.data_[i].Raa;
		Rab[i] = Edge_D.data_[i].Rab;
		Rac[i] = Edge_D.data_[i].Rac;
		Rba[i] = Edge_D.data_[i].Rba;
		Rbb[i] = Edge_D.data_[i].Rbb;
		Rbc[i] = Edge_D.data_[i].Rbc;
		Rca[i] = Edge_D.data_[i].Rca;
		Rcb[i] = Edge_D.data_[i].Rcb;
		Rcc[i] = Edge_D.data_[i].Rcc;
		Xaa[i] = Edge_D.data_[i].Xaa;
		Xab[i] = Edge_D.data_[i].Xab;
		Xac[i] = Edge_D.data_[i].Xac;
		Xba[i] = Edge_D.data_[i].Xba;
		Xbb[i] = Edge_D.data_[i].Xbb;
		Xbc[i] = Edge_D.data_[i].Xbc;
		Xca[i] = Edge_D.data_[i].Xca;
		Xcb[i] = Edge_D.data_[i].Xcb;
		Xcc[i] = Edge_D.data_[i].Xcc;
		Nt[i] = Edge_D.data_[i].Nt;
		edgetype[i] = Edge_D.data_[i].edgetype;
		if (edgetype[i] == 0)
		{
			//ignore the impact of B, get matrix for feeders
			areal[i][0][0] = 1; areal[i][0][1] = 0; areal[i][0][2] = 0;
			areal[i][1][0] = 0; areal[i][1][1] = 1; areal[i][1][2] = 0;
			areal[i][2][0] = 0; areal[i][2][1] = 0; areal[i][2][2] = 1;

			breal[i][0][0] = Raa[i]; breal[i][0][1] = Rab[i]; breal[i][0][2] = Rac[i];
			breal[i][1][0] = Rba[i]; breal[i][1][1] = Rbb[i]; breal[i][1][2] = Rbc[i];
			breal[i][2][0] = Rca[i]; breal[i][2][1] = Rcb[i]; breal[i][2][2] = Rcc[i];
			bimg[i][0][0] = Xaa[i]; bimg[i][0][1] = Xab[i]; bimg[i][0][2] = Xac[i];
			bimg[i][1][0] = Xba[i]; bimg[i][1][1] = Xbb[i]; bimg[i][1][2] = Xbc[i];
			bimg[i][2][0] = Xca[i]; bimg[i][2][1] = Xcb[i]; bimg[i][2][2] = Xcc[i];
			
			Areal[i][0][0] = 1; Areal[i][0][1] = 0; Areal[i][0][2] = 0;
			Areal[i][1][0] = 0; Areal[i][1][1] = 1; Areal[i][1][2] = 0;
			Areal[i][2][0] = 0; Areal[i][2][1] = 0; Areal[i][2][2] = 1;

			Breal[i][0][0] = Raa[i]; Breal[i][0][1] = Rab[i]; Breal[i][0][2] = Rac[i];
			Breal[i][1][0] = Rba[i]; Breal[i][1][1] = Rbb[i]; Breal[i][1][2] = Rbc[i];
			Breal[i][2][0] = Rca[i]; Breal[i][2][1] = Rcb[i]; Breal[i][2][2] = Rcc[i];
			Bimg[i][0][0] = Xaa[i]; Bimg[i][0][1] = Xab[i]; Bimg[i][0][2] = Xac[i];
			Bimg[i][1][0] = Xba[i]; Bimg[i][1][1] = Xbb[i]; Bimg[i][1][2] = Xbc[i];
			Bimg[i][2][0] = Xca[i]; Bimg[i][2][1] = Xcb[i]; Bimg[i][2][2] = Xcc[i];
			
/* 			printf("\n\n================================================================================================\n ");
			for (j=0;j<3;j++)
			{
				for (k=0;k<3;k++)
				{
					std::cout << "Fid: " << IDF[i]<<" Tid "<<IDT[i] <<" areal: "<<areal[i][j][k]<<" aimg "<<aimg[i][j][k]<<" breal: "<<breal[i][j][k]<<" bimg "<<bimg[i][j][k]<<" creal: "<<creal[i][j][k]<<" cimg "<<cimg[i][j][k] << std::endl;

				}
			}
			printf("\n================================================================================================ \n\n"); 
 */		}
		//get matrix for transformers,Delta-Delta 
		else if (edgetype[i] == 1)
		{
			at[i][0][0] = 2*Nt[i]/3; at[i][0][1] = -Nt[i]/3; at[i][0][2] = -Nt[i]/3;
			at[i][1][0] = -Nt[i]/3; at[i][1][1] = 2*Nt[i]/3; at[i][1][2] = -Nt[i]/3;
			at[i][2][0] = -Nt[i]/3; at[i][2][1] = -Nt[i]/3; at[i][2][2] = 2*Nt[i]/3;
			
			btreal[i][0][0] = Nt[i]*Raa[i]; btreal[i][0][1] = 0; btreal[i][0][2] = 0;
			btreal[i][1][0] = 0; btreal[i][1][1] = Nt[i]*Rbb[i]; btreal[i][1][2] = 0;
			btreal[i][2][0] = 0; btreal[i][2][1] = 0; btreal[i][2][2] = Nt[i]*Rcc[i];
			btimg[i][0][0] = Nt[i]*Xaa[i]; btimg[i][0][1] = 0; btimg[i][0][2] = 0;
			btimg[i][1][0] = 0; btimg[i][1][1] = Nt[i]*Xbb[i]; btimg[i][1][2] = 0;
			btimg[i][2][0] = 0; btimg[i][2][1] = 0; btimg[i][2][2] = Nt[i]*Xcc[i];
			
			dt[i][0][0] = 1/Nt[i]; dt[i][0][1] = 0; dt[i][0][2] = 0;
			dt[i][1][0] = 0; dt[i][1][1] = 1/Nt[i]; dt[i][1][2] = 0;
			dt[i][2][0] = 0; dt[i][2][1] = 0; dt[i][2][2] = 1/Nt[i];
			
			At[i][0][0] = 2/(3*Nt[i]); At[i][0][1] = -1/(3*Nt[i]); At[i][0][2] = -1/(3*Nt[i]);
			At[i][1][0] = -1/(3*Nt[i]); At[i][1][1] = 2/(3*Nt[i]); At[i][1][2] = -1/(3*Nt[i]);
			At[i][2][0] = -1/(3*Nt[i]); At[i][2][1] = -1/(3*Nt[i]); At[i][2][2] = 2/(3*Nt[i]);
		}
		
	}
		
//Find Terminal Buses by using the fact that a Terminal has no input bus
	k = 0;//use to store the number of child nodes in each step
	for (i=0; i<nv; i++)
	{
		if (branchf[i] == 0)
		{
			Terminalbuses[k] = ID[i]; //store the id of the terminal nodes.
			k +=1;
		}		
	}
	tn = k;//store the number of terminal nodes
	 
// forward backward iteration
	for (iter=0; iter<Maxit; ++iter)
	{
		lv=0;
		if (iter == 0)//initialize the voltages at the terminal nodes
		{
			for (i=0; i<tn; i++)
			{
				j = Terminalbuses[i] - 1;//get indexes of terminal buses
				VphaseAreal[j] = Vbase[j];
				VphaseAimg[j] = 0;
				VphaseBreal[j] = Vbase[j]*(-0.5);
				VphaseBimg[j] = Vbase[j]*(-0.866025);
				VphaseCreal[j] = Vbase[j]*(-0.5);
				VphaseCimg[j] = Vbase[j]*(0.866025);			
			}
		}
	/* printf("\n\n================================================================================================\n ");
 	for (i=0;i<nv;i++)
	{
		std::cout << "ID:" << i+1 <<" VAr: "<<VphaseAreal[i]<<" VAi "<<VphaseAimg[i]<<" VBr: "<<VphaseBreal[i]<<" VBi "<<VphaseBimg[i]<<" VCr: "<<VphaseCreal[i]<<" VCi "<<VphaseCimg[i] << std::endl;
	}
	std::cout << "Maxit : "<<Maxit <<" it "<<iter<< std::endl; 
	printf("\n================================================================================================ \n\n"); */
		// forward sweep, update currents
				
        while (countf[SNindex]!=branchf[SNindex] && lv<100)	
		{	
	        if (lv == 0) //select the terminal buses
		    {
				in = tn; //initial the number of child nodes at first selection
				for (i=0; i<in; i++)
				{
					Childbuses[i] = Terminalbuses[i];
					c = Terminalbuses[i] - 1;
					//calculate the currents at terminal nodes
					//get phase to ground and phase to phase voltage magnitude	
					VAmag[c] = sqrt(VphaseAreal[c]*VphaseAreal[c] + VphaseAimg[c]*VphaseAimg[c]);
					VBmag[c] = sqrt(VphaseBreal[c]*VphaseBreal[c] + VphaseBimg[c]*VphaseBimg[c]);
					VCmag[c] = sqrt(VphaseCreal[c]*VphaseCreal[c] + VphaseCimg[c]*VphaseCimg[c]);
					VphaseABreal[c] = VphaseAreal[c] - VphaseBreal[c];
					VphaseBCreal[c] = VphaseBreal[c] - VphaseCreal[c];
					VphaseCAreal[c] = VphaseCreal[c] - VphaseAreal[c];
					VphaseABimg[c] = VphaseAimg[c] - VphaseBimg[c];
					VphaseBCimg[c] = VphaseBimg[c] - VphaseCimg[c];
					VphaseCAimg[c] = VphaseCimg[c] - VphaseAimg[c];
					VABmag[c] = sqrt(VphaseABreal[c]*VphaseABreal[c] + VphaseABimg[c]*VphaseABimg[c]);
					VBCmag[c] = sqrt(VphaseBCreal[c]*VphaseBCreal[c] + VphaseBCimg[c]*VphaseBCimg[c]);
					VCAmag[c] = sqrt(VphaseCAreal[c]*VphaseCAreal[c] + VphaseCAimg[c]*VphaseCAimg[c]);
					//get actual capacitor reactive power 
					CapA_cal[c] = CapA[c]*(VAmag[c]/Vbase[c])*(VAmag[c]/Vbase[c]);
					CapB_cal[c] = CapB[c]*(VBmag[c]/Vbase[c])*(VBmag[c]/Vbase[c]);
					CapC_cal[c] = CapC[c]*(VCmag[c]/Vbase[c])*(VCmag[c]/Vbase[c]);
					//get actual load based on voltages
					
					if (LM[c] == 1)
					{
						PdA_cal[c] = PdA[c];
						QdA_cal[c] = QdA[c];
						PdB_cal[c] = PdB[c];
						QdB_cal[c] = QdB[c];
						PdC_cal[c] = PdC[c];
						QdC_cal[c] = QdC[c];
						IphaseAreal[c] = (PdA_cal[c]*VphaseAreal[c] + (QdA_cal[c]-CapA_cal[c])*VphaseAimg[c])/(VphaseAreal[c]*VphaseAreal[c] + VphaseAimg[c]*VphaseAimg[c]);
						IphaseBreal[c] = (PdB_cal[c]*VphaseBreal[c] + (QdB_cal[c]-CapB_cal[c])*VphaseBimg[c])/(VphaseBreal[c]*VphaseBreal[c] + VphaseBimg[c]*VphaseBimg[c]);
						IphaseCreal[c] = (PdC_cal[c]*VphaseCreal[c] + (QdC_cal[c]-CapC_cal[c])*VphaseCimg[c])/(VphaseCreal[c]*VphaseCreal[c] + VphaseCimg[c]*VphaseCimg[c]);
						IphaseAimg[c] = (PdA_cal[c]*VphaseAimg[c]-(QdA_cal[c]-CapA_cal[c])*VphaseAreal[c])/(VphaseAreal[c]*VphaseAreal[c] + VphaseAimg[c]*VphaseAimg[c]);
						IphaseBimg[c] = (PdB_cal[c]*VphaseBimg[c]-(QdB_cal[c]-CapB_cal[c])*VphaseBreal[c])/(VphaseBreal[c]*VphaseBreal[c] + VphaseBimg[c]*VphaseBimg[c]);
						IphaseCimg[c] = (PdC_cal[c]*VphaseCimg[c]-(QdC_cal[c]-CapC_cal[c])*VphaseCreal[c])/(VphaseCreal[c]*VphaseCreal[c] + VphaseCimg[c]*VphaseCimg[c]);
						
					}
					else if (LM[c] == 2)
					{
						PdA_cal[c] = PdA[c]*(VAmag[c]/Vbase[c])*(VAmag[c]/Vbase[c]);
						QdA_cal[c] = QdA[c]*(VAmag[c]/Vbase[c])*(VAmag[c]/Vbase[c]);
						PdB_cal[c] = PdB[c]*(VBmag[c]/Vbase[c])*(VBmag[c]/Vbase[c]);
						QdB_cal[c] = QdB[c]*(VBmag[c]/Vbase[c])*(VBmag[c]/Vbase[c]);
						PdC_cal[c] = PdC[c]*(VCmag[c]/Vbase[c])*(VCmag[c]/Vbase[c]);
						QdC_cal[c] = QdC[c]*(VCmag[c]/Vbase[c])*(VCmag[c]/Vbase[c]);
						IphaseAreal[c] = (PdA_cal[c]*VphaseAreal[c] + (QdA_cal[c]-CapA_cal[c])*VphaseAimg[c])/(VphaseAreal[c]*VphaseAreal[c] + VphaseAimg[c]*VphaseAimg[c]);
						IphaseBreal[c] = (PdB_cal[c]*VphaseBreal[c] + (QdB_cal[c]-CapB_cal[c])*VphaseBimg[c])/(VphaseBreal[c]*VphaseBreal[c] + VphaseBimg[c]*VphaseBimg[c]);
						IphaseCreal[c] = (PdC_cal[c]*VphaseCreal[c] + (QdC_cal[c]-CapC_cal[c])*VphaseCimg[c])/(VphaseCreal[c]*VphaseCreal[c] + VphaseCimg[c]*VphaseCimg[c]);
						IphaseAimg[c] = (PdA_cal[c]*VphaseAimg[c]-(QdA_cal[c]-CapA_cal[c])*VphaseAreal[c])/(VphaseAreal[c]*VphaseAreal[c] + VphaseAimg[c]*VphaseAimg[c]);
						IphaseBimg[c] = (PdB_cal[c]*VphaseBimg[c]-(QdB_cal[c]-CapB_cal[c])*VphaseBreal[c])/(VphaseBreal[c]*VphaseBreal[c] + VphaseBimg[c]*VphaseBimg[c]);
						IphaseCimg[c] = (PdC_cal[c]*VphaseCimg[c]-(QdC_cal[c]-CapC_cal[c])*VphaseCreal[c])/(VphaseCreal[c]*VphaseCreal[c] + VphaseCimg[c]*VphaseCimg[c]);
					}
					else if (LM[c] == 3)
					{
						PdA_cal[c] = PdA[c]*(VAmag[c]/Vbase[c]);
						QdA_cal[c] = QdA[c]*(VAmag[c]/Vbase[c]);
						PdB_cal[c] = PdB[c]*(VBmag[c]/Vbase[c]);
						QdB_cal[c] = QdB[c]*(VBmag[c]/Vbase[c]);
						PdC_cal[c] = PdC[c]*(VCmag[c]/Vbase[c]);
						QdC_cal[c] = QdC[c]*(VCmag[c]/Vbase[c]);
						IphaseAreal[c] = (PdA_cal[c]*VphaseAreal[c] + (QdA_cal[c]-CapA_cal[c])*VphaseAimg[c])/(VphaseAreal[c]*VphaseAreal[c] + VphaseAimg[c]*VphaseAimg[c]);
						IphaseBreal[c] = (PdB_cal[c]*VphaseBreal[c] + (QdB_cal[c]-CapB_cal[c])*VphaseBimg[c])/(VphaseBreal[c]*VphaseBreal[c] + VphaseBimg[c]*VphaseBimg[c]);
						IphaseCreal[c] = (PdC_cal[c]*VphaseCreal[c] + (QdC_cal[c]-CapC_cal[c])*VphaseCimg[c])/(VphaseCreal[c]*VphaseCreal[c] + VphaseCimg[c]*VphaseCimg[c]);
						IphaseAimg[c] = (PdA_cal[c]*VphaseAimg[c]-(QdA_cal[c]-CapA_cal[c])*VphaseAreal[c])/(VphaseAreal[c]*VphaseAreal[c] + VphaseAimg[c]*VphaseAimg[c]);
						IphaseBimg[c] = (PdB_cal[c]*VphaseBimg[c]-(QdB_cal[c]-CapB_cal[c])*VphaseBreal[c])/(VphaseBreal[c]*VphaseBreal[c] + VphaseBimg[c]*VphaseBimg[c]);
						IphaseCimg[c] = (PdC_cal[c]*VphaseCimg[c]-(QdC_cal[c]-CapC_cal[c])*VphaseCreal[c])/(VphaseCreal[c]*VphaseCreal[c] + VphaseCimg[c]*VphaseCimg[c]);
					}
					else if (LM[c] == 4)
					{
						PdAB_cal[c] = PdA[c];
						QdAB_cal[c] = QdA[c];
						PdBC_cal[c] = PdB[c];
						QdBC_cal[c] = QdB[c];
						PdCA_cal[c] = PdC[c];
						QdCA_cal[c] = QdC[c];
						IphaseABreal[c] = (PdAB_cal[c]*VphaseABreal[c] + QdAB_cal[c]*VphaseABimg[c])/(VphaseABreal[c]*VphaseABreal[c] + VphaseABimg[c]*VphaseABimg[c]);
						IphaseABimg[c] = (PdAB_cal[c]*VphaseABimg[c] - QdAB_cal[c]*VphaseABreal[c])/(VphaseABreal[c]*VphaseABreal[c] + VphaseABimg[c]*VphaseABimg[c]);
						IphaseBCreal[c] = (PdBC_cal[c]*VphaseBCreal[c] + QdBC_cal[c]*VphaseBCimg[c])/(VphaseBCreal[c]*VphaseBCreal[c] + VphaseBCimg[c]*VphaseBCimg[c]);
						IphaseBCimg[c] = (PdBC_cal[c]*VphaseBCimg[c] - QdBC_cal[c]*VphaseBCreal[c])/(VphaseBCreal[c]*VphaseBCreal[c] + VphaseBCimg[c]*VphaseBCimg[c]);
						IphaseCAreal[c] = (PdCA_cal[c]*VphaseCAreal[c] + QdCA_cal[c]*VphaseCAimg[c])/(VphaseCAreal[c]*VphaseCAreal[c] + VphaseCAimg[c]*VphaseCAimg[c]);
						IphaseCAimg[c] = (PdCA_cal[c]*VphaseCAimg[c] - QdCA_cal[c]*VphaseCAreal[c])/(VphaseCAreal[c]*VphaseCAreal[c] + VphaseCAimg[c]*VphaseCAimg[c]);
						IphaseAreal[c] = IphaseABreal[c]-IphaseCAreal[c]-CapA_cal[c]*VphaseAimg[c]/(VphaseAreal[c]*VphaseAreal[c] + VphaseAimg[c]*VphaseAimg[c]);
						IphaseAimg[c] = IphaseABimg[c]-IphaseCAimg[c]+CapA_cal[c]*VphaseAreal[c]/(VphaseAreal[c]*VphaseAreal[c] + VphaseAimg[c]*VphaseAimg[c]);
						IphaseBreal[c] = IphaseBCreal[c]-IphaseABreal[c]-CapB_cal[c]*VphaseBimg[c]/(VphaseBreal[c]*VphaseBreal[c] + VphaseBimg[c]*VphaseBimg[c]);
						IphaseBimg[c] = IphaseBCimg[c]-IphaseABimg[c]+CapB_cal[c]*VphaseBreal[c]/(VphaseBreal[c]*VphaseBreal[c] + VphaseBimg[c]*VphaseBimg[c]);
						IphaseCreal[c] = IphaseCAreal[c]-IphaseBCreal[c]-CapC_cal[c]*VphaseCimg[c]/(VphaseCreal[c]*VphaseCreal[c] + VphaseCimg[c]*VphaseCimg[c]);
						IphaseCimg[c] = IphaseCAimg[c]-IphaseBCimg[c]+CapC_cal[c]*VphaseCreal[c]/(VphaseCreal[c]*VphaseCreal[c] + VphaseCimg[c]*VphaseCimg[c]);
					}
					else if (LM[c] == 5)
					{
						PdAB_cal[c] = PdA[c]*(VABmag[c]/Vbase[c]/sqrt(3))*(VABmag[c]/Vbase[c]/sqrt(3));
						QdAB_cal[c] = QdA[c]*(VABmag[c]/Vbase[c]/sqrt(3))*(VABmag[c]/Vbase[c]/sqrt(3));
						PdBC_cal[c] = PdB[c]*(VBCmag[c]/Vbase[c]/sqrt(3))*(VBCmag[c]/Vbase[c]/sqrt(3));
						QdBC_cal[c] = QdB[c]*(VBCmag[c]/Vbase[c]/sqrt(3))*(VBCmag[c]/Vbase[c]/sqrt(3));
						PdCA_cal[c] = PdC[c]*(VCAmag[c]/Vbase[c]/sqrt(3))*(VCAmag[c]/Vbase[c]/sqrt(3));
						QdCA_cal[c] = QdC[c]*(VCAmag[c]/Vbase[c]/sqrt(3))*(VCAmag[c]/Vbase[c]/sqrt(3));
						IphaseABreal[c] = (PdAB_cal[c]*VphaseABreal[c] + QdAB_cal[c]*VphaseABimg[c])/(VphaseABreal[c]*VphaseABreal[c] + VphaseABimg[c]*VphaseABimg[c]);
						IphaseABimg[c] = (PdAB_cal[c]*VphaseABimg[c] - QdAB_cal[c]*VphaseABreal[c])/(VphaseABreal[c]*VphaseABreal[c] + VphaseABimg[c]*VphaseABimg[c]);
						IphaseBCreal[c] = (PdBC_cal[c]*VphaseBCreal[c] + QdBC_cal[c]*VphaseBCimg[c])/(VphaseBCreal[c]*VphaseBCreal[c] + VphaseBCimg[c]*VphaseBCimg[c]);
						IphaseBCimg[c] = (PdBC_cal[c]*VphaseBCimg[c] - QdBC_cal[c]*VphaseBCreal[c])/(VphaseBCreal[c]*VphaseBCreal[c] + VphaseBCimg[c]*VphaseBCimg[c]);
						IphaseCAreal[c] = (PdCA_cal[c]*VphaseCAreal[c] + QdCA_cal[c]*VphaseCAimg[c])/(VphaseCAreal[c]*VphaseCAreal[c] + VphaseCAimg[c]*VphaseCAimg[c]);
						IphaseCAimg[c] = (PdCA_cal[c]*VphaseCAimg[c] - QdCA_cal[c]*VphaseCAreal[c])/(VphaseCAreal[c]*VphaseCAreal[c] + VphaseCAimg[c]*VphaseCAimg[c]);
						IphaseAreal[c] = IphaseABreal[c]-IphaseCAreal[c]-CapA_cal[c]*VphaseAimg[c]/(VphaseAreal[c]*VphaseAreal[c] + VphaseAimg[c]*VphaseAimg[c]);
						IphaseAimg[c] = IphaseABimg[c]-IphaseCAimg[c]+CapA_cal[c]*VphaseAreal[c]/(VphaseAreal[c]*VphaseAreal[c] + VphaseAimg[c]*VphaseAimg[c]);
						IphaseBreal[c] = IphaseBCreal[c]-IphaseABreal[c]-CapB_cal[c]*VphaseBimg[c]/(VphaseBreal[c]*VphaseBreal[c] + VphaseBimg[c]*VphaseBimg[c]);
						IphaseBimg[c] = IphaseBCimg[c]-IphaseABimg[c]+CapB_cal[c]*VphaseBreal[c]/(VphaseBreal[c]*VphaseBreal[c] + VphaseBimg[c]*VphaseBimg[c]);
						IphaseCreal[c] = IphaseCAreal[c]-IphaseBCreal[c]-CapC_cal[c]*VphaseCimg[c]/(VphaseCreal[c]*VphaseCreal[c] + VphaseCimg[c]*VphaseCimg[c]);
						IphaseCimg[c] = IphaseCAimg[c]-IphaseBCimg[c]+CapC_cal[c]*VphaseCreal[c]/(VphaseCreal[c]*VphaseCreal[c] + VphaseCimg[c]*VphaseCimg[c]);
					}
					else if (LM[c] == 6)
					{
						PdAB_cal[c] = PdA[c]*(VABmag[c]/Vbase[c]/sqrt(3));
						QdAB_cal[c] = QdA[c]*(VABmag[c]/Vbase[c]/sqrt(3));
						PdBC_cal[c] = PdB[c]*(VBCmag[c]/Vbase[c]/sqrt(3));
						QdBC_cal[c] = QdB[c]*(VBCmag[c]/Vbase[c]/sqrt(3));
						PdCA_cal[c] = PdC[c]*(VCAmag[c]/Vbase[c]/sqrt(3));
						QdCA_cal[c] = QdC[c]*(VCAmag[c]/Vbase[c]/sqrt(3));
						IphaseABreal[c] = (PdAB_cal[c]*VphaseABreal[c] + QdAB_cal[c]*VphaseABimg[c])/(VphaseABreal[c]*VphaseABreal[c] + VphaseABimg[c]*VphaseABimg[c]);
						IphaseABimg[c] = (PdAB_cal[c]*VphaseABimg[c] - QdAB_cal[c]*VphaseABreal[c])/(VphaseABreal[c]*VphaseABreal[c] + VphaseABimg[c]*VphaseABimg[c]);
						IphaseBCreal[c] = (PdBC_cal[c]*VphaseBCreal[c] + QdBC_cal[c]*VphaseBCimg[c])/(VphaseBCreal[c]*VphaseBCreal[c] + VphaseBCimg[c]*VphaseBCimg[c]);
						IphaseBCimg[c] = (PdBC_cal[c]*VphaseBCimg[c] - QdBC_cal[c]*VphaseBCreal[c])/(VphaseBCreal[c]*VphaseBCreal[c] + VphaseBCimg[c]*VphaseBCimg[c]);
						IphaseCAreal[c] = (PdCA_cal[c]*VphaseCAreal[c] + QdCA_cal[c]*VphaseCAimg[c])/(VphaseCAreal[c]*VphaseCAreal[c] + VphaseCAimg[c]*VphaseCAimg[c]);
						IphaseCAimg[c] = (PdCA_cal[c]*VphaseCAimg[c] - QdCA_cal[c]*VphaseCAreal[c])/(VphaseCAreal[c]*VphaseCAreal[c] + VphaseCAimg[c]*VphaseCAimg[c]);
						IphaseAreal[c] = IphaseABreal[c]-IphaseCAreal[c]-CapA_cal[c]*VphaseAimg[c]/(VphaseAreal[c]*VphaseAreal[c] + VphaseAimg[c]*VphaseAimg[c]);
						IphaseAimg[c] = IphaseABimg[c]-IphaseCAimg[c]+CapA_cal[c]*VphaseAreal[c]/(VphaseAreal[c]*VphaseAreal[c] + VphaseAimg[c]*VphaseAimg[c]);
						IphaseBreal[c] = IphaseBCreal[c]-IphaseABreal[c]-CapB_cal[c]*VphaseBimg[c]/(VphaseBreal[c]*VphaseBreal[c] + VphaseBimg[c]*VphaseBimg[c]);
						IphaseBimg[c] = IphaseBCimg[c]-IphaseABimg[c]+CapB_cal[c]*VphaseBreal[c]/(VphaseBreal[c]*VphaseBreal[c] + VphaseBimg[c]*VphaseBimg[c]);
						IphaseCreal[c] = IphaseCAreal[c]-IphaseBCreal[c]-CapC_cal[c]*VphaseCimg[c]/(VphaseCreal[c]*VphaseCreal[c] + VphaseCimg[c]*VphaseCimg[c]);
						IphaseCimg[c] = IphaseCAimg[c]-IphaseBCimg[c]+CapC_cal[c]*VphaseCreal[c]/(VphaseCreal[c]*VphaseCreal[c] + VphaseCimg[c]*VphaseCimg[c]);
					}
						
					std::cout << "cid : "<<c+1<<" PdA "<<PdA[c]<<" QdA "<<QdA[c]<<" PdB "<<PdB[c]<<" QdB "<<QdB[c]<<" PdC "<<PdC[c]<<" QdC "<<QdC[c]<<" VAmag "<<VAmag[c]<<" VBmag "<<VBmag[c]<<" VCmag "<<VCmag[c]<<" Vbase "<<Vbase[c]<<" PdA_cal "<<PdA_cal[c]<<" QdA_cal "<<QdA_cal[c]<<" PdB_cal "<<PdB_cal[c]<<" QdB_cal "<<QdB_cal[c]<<" PdC_cal "<<PdC_cal[c]<<" QdC_cal "<<QdC_cal[c]<<" loadmodel "<<LM[c]<<" "<<Vertex_D.data_[c].Load_Model.c_str()<<std::endl; 

				}
			}
			for (i=0; i<in; i++)
	        {
		        for (j=0; j<ne; j++)
		        {   
	                if(IDT[j] == Childbuses[i])
		            {
			            Links[i] = j; //store the link index between the child nodes and father nodes.
						Fatherbuses[i] = IDF[j]; //store the id of father nodes.
						countf[IDF[j]-1] += 1; //Array_id = bus_id -1
		            }
		        }
	        }

			for (i=0; i<in; i++)//calculation at the links between the child nodes and father nodes
			{
				j = Links[i]; //selected link index
				f = IDF[j]-1; //selected father node index
				c = IDT[j]-1; //selected child node index
				
				std::cout << "fid : "<<f+1<<" cid "<<c+1<<" edgeid "<<j+1<<" edgetype "<<edgetype[j]<<std::endl; 
				
				if (edgetype[j]==0) //if the selected link is a feeder
				{
					// calculate real part of voltage
					VphaseAreal[f] = VphaseAreal[c]*areal[j][0][0] + VphaseBreal[c]*areal[j][0][1] + VphaseCreal[c]*areal[j][0][2] - VphaseAimg[c]*aimg[j][0][0] - VphaseBimg[c]*aimg[j][0][1] - VphaseCimg[c]*aimg[j][0][2] + IphaseAreal[c]*breal[j][0][0] + IphaseBreal[c]*breal[j][0][1] + IphaseCreal[c]*breal[j][0][2] -IphaseAimg[c]*bimg[j][0][0] -IphaseBimg[c]*bimg[j][0][1] - IphaseCimg[c]*bimg[j][0][2];
					VphaseBreal[f] = VphaseAreal[c]*areal[j][1][0] + VphaseBreal[c]*areal[j][1][1] + VphaseCreal[c]*areal[j][1][2] - VphaseAimg[c]*aimg[j][1][0] - VphaseBimg[c]*aimg[j][1][1] - VphaseCimg[c]*aimg[j][1][2] + IphaseAreal[c]*breal[j][1][0] + IphaseBreal[c]*breal[j][1][1] + IphaseCreal[c]*breal[j][1][2] -IphaseAimg[c]*bimg[j][1][0] -IphaseBimg[c]*bimg[j][1][1] - IphaseCimg[c]*bimg[j][1][2];
					VphaseCreal[f] = VphaseAreal[c]*areal[j][2][0] + VphaseBreal[c]*areal[j][2][1] + VphaseCreal[c]*areal[j][2][2] - VphaseAimg[c]*aimg[j][2][0] - VphaseBimg[c]*aimg[j][2][1] - VphaseCimg[c]*aimg[j][2][2]+ IphaseAreal[c]*breal[j][2][0] + IphaseBreal[c]*breal[j][2][1] + IphaseCreal[c]*breal[j][2][2] -IphaseAimg[c]*bimg[j][2][0] -IphaseBimg[c]*bimg[j][2][1] - IphaseCimg[c]*bimg[j][2][2];
				    // calculate imaginary part of voltage
					VphaseAimg[f] = VphaseAreal[c]*aimg[j][0][0] + VphaseBreal[c]*aimg[j][0][1] + VphaseCreal[c]*aimg[j][0][2] + VphaseAimg[c]*areal[j][0][0] + VphaseBimg[c]*areal[j][0][1] + VphaseCimg[c]*areal[j][0][2] + IphaseAreal[c]*bimg[j][0][0] + IphaseBreal[c]*bimg[j][0][1] + IphaseCreal[c]*bimg[j][0][2] + IphaseAimg[c]*breal[j][0][0] + IphaseBimg[c]*breal[j][0][1] + IphaseCimg[c]*breal[j][0][2];
					VphaseBimg[f] = VphaseAreal[c]*aimg[j][1][0] + VphaseBreal[c]*aimg[j][1][1] + VphaseCreal[c]*aimg[j][1][2] + VphaseAimg[c]*areal[j][1][0] + VphaseBimg[c]*areal[j][1][1] + VphaseCimg[c]*areal[j][1][2] + IphaseAreal[c]*bimg[j][1][0] + IphaseBreal[c]*bimg[j][1][1] + IphaseCreal[c]*bimg[j][1][2] + IphaseAimg[c]*breal[j][1][0] + IphaseBimg[c]*breal[j][1][1] + IphaseCimg[c]*breal[j][1][2];
					VphaseCimg[f] = VphaseAreal[c]*aimg[j][2][0] + VphaseBreal[c]*aimg[j][2][1] + VphaseCreal[c]*aimg[j][2][2] + VphaseAimg[c]*areal[j][2][0] + VphaseBimg[c]*areal[j][2][1] + VphaseCimg[c]*areal[j][2][2] + IphaseAreal[c]*bimg[j][2][0] + IphaseBreal[c]*bimg[j][2][1] + IphaseCreal[c]*bimg[j][2][2] + IphaseAimg[c]*breal[j][2][0] + IphaseBimg[c]*breal[j][2][1] + IphaseCimg[c]*breal[j][2][2];
				    // calculate real part of current
					IphaseAreal[f] += VphaseAreal[c]*creal[j][0][0] + VphaseBreal[c]*creal[j][0][1] + VphaseCreal[c]*creal[j][0][2] - VphaseAimg[c]*cimg[j][0][0] - VphaseBimg[c]*cimg[j][0][1] - VphaseCimg[c]*cimg[j][0][2] + IphaseAreal[c]*areal[j][0][0] + IphaseBreal[c]*areal[j][0][1] + IphaseCreal[c]*areal[j][0][2] -IphaseAimg[c]*aimg[j][0][0] -IphaseBimg[c]*aimg[j][0][1] - IphaseCimg[c]*aimg[j][0][2];
					IphaseBreal[f] += VphaseAreal[c]*creal[j][1][0] + VphaseBreal[c]*creal[j][1][1] + VphaseCreal[c]*creal[j][1][2] - VphaseAimg[c]*cimg[j][1][0] - VphaseBimg[c]*cimg[j][1][1] - VphaseCimg[c]*cimg[j][1][2]  + IphaseAreal[c]*areal[j][1][0] + IphaseBreal[c]*areal[j][1][1] + IphaseCreal[c]*areal[j][1][2] -IphaseAimg[c]*aimg[j][1][0] -IphaseBimg[c]*aimg[j][1][1] - IphaseCimg[c]*aimg[j][1][2];
					IphaseCreal[f] += VphaseAreal[c]*creal[j][2][0] + VphaseBreal[c]*creal[j][2][1] + VphaseCreal[c]*creal[j][2][2] - VphaseAimg[c]*cimg[j][2][0] - VphaseBimg[c]*cimg[j][2][1] - VphaseCimg[c]*cimg[j][2][2] + IphaseAreal[c]*areal[j][2][0] + IphaseBreal[c]*areal[j][2][1] + IphaseCreal[c]*areal[j][2][2] -IphaseAimg[c]*aimg[j][2][0] -IphaseBimg[c]*aimg[j][2][1] - IphaseCimg[c]*aimg[j][2][2];
				    // calculate imaginary part of current
					IphaseAimg[f] += VphaseAreal[c]*cimg[j][0][0] + VphaseBreal[c]*cimg[j][0][1] + VphaseCreal[c]*cimg[j][0][2] + VphaseAimg[c]*creal[j][0][0] + VphaseBimg[c]*creal[j][0][1] + VphaseCimg[c]*creal[j][0][2] + IphaseAreal[c]*aimg[j][0][0] + IphaseBreal[c]*aimg[j][0][1] + IphaseCreal[c]*aimg[j][0][2] + IphaseAimg[c]*areal[j][0][0] + IphaseBimg[c]*areal[j][0][1] + IphaseCimg[c]*areal[j][0][2];
					IphaseBimg[f] += VphaseAreal[c]*cimg[j][1][0] + VphaseBreal[c]*cimg[j][1][1] + VphaseCreal[c]*cimg[j][1][2] + VphaseAimg[c]*creal[j][1][0] + VphaseBimg[c]*creal[j][1][1] + VphaseCimg[c]*creal[j][1][2] + IphaseAreal[c]*aimg[j][1][0] + IphaseBreal[c]*aimg[j][1][1] + IphaseCreal[c]*aimg[j][1][2] + IphaseAimg[c]*areal[j][1][0] + IphaseBimg[c]*areal[j][1][1] + IphaseCimg[c]*areal[j][1][2];
					IphaseCimg[f] += VphaseAreal[c]*cimg[j][2][0] + VphaseBreal[c]*cimg[j][2][1] + VphaseCreal[c]*cimg[j][2][2] + VphaseAimg[c]*creal[j][2][0] + VphaseBimg[c]*creal[j][2][1] + VphaseCimg[c]*creal[j][2][2] + IphaseAreal[c]*aimg[j][2][0] + IphaseBreal[c]*aimg[j][2][1] + IphaseCreal[c]*aimg[j][2][2] + IphaseAimg[c]*areal[j][2][0] + IphaseBimg[c]*areal[j][2][1] + IphaseCimg[c]*areal[j][2][2];
					//add load currents at the father nodes
				}
				else if (edgetype[j]==1) //if the selected link is a transformer
				{
					// calculate real part of voltage
					VphaseAreal[f] = VphaseAreal[c]*at[j][0][0] + VphaseBreal[c]*at[j][0][1] + VphaseCreal[c]*at[j][0][2] + IphaseAreal[c]*btreal[j][0][0] + IphaseBreal[c]*btreal[j][0][1] + IphaseCreal[c]*btreal[j][0][2] - IphaseAimg[c]*btimg[j][0][0] - IphaseBimg[c]*btimg[j][0][1] - IphaseCimg[c]*btimg[j][0][2];
					VphaseBreal[f] = VphaseAreal[c]*at[j][1][0] + VphaseBreal[c]*at[j][1][1] + VphaseCreal[c]*at[j][1][2] + IphaseAreal[c]*btreal[j][1][0] + IphaseBreal[c]*btreal[j][1][1] + IphaseCreal[c]*btreal[j][1][2] - IphaseAimg[c]*btimg[j][1][0] - IphaseBimg[c]*btimg[j][1][1] - IphaseCimg[c]*btimg[j][1][2];
					VphaseCreal[f] = VphaseAreal[c]*at[j][2][0] + VphaseBreal[c]*at[j][2][1] + VphaseCreal[c]*at[j][2][2] + IphaseAreal[c]*btreal[j][2][0] + IphaseBreal[c]*btreal[j][2][1] + IphaseCreal[c]*btreal[j][2][2] - IphaseAimg[c]*btimg[j][2][0] - IphaseBimg[c]*btimg[j][2][1] - IphaseCimg[c]*btimg[j][2][2];
				    // calculate imaginary part of voltage
					VphaseAimg[f] = VphaseAimg[c]*at[j][0][0] + VphaseBimg[c]*at[j][0][1] + VphaseCimg[c]*at[j][0][2] + IphaseAreal[c]*btimg[j][0][0] + IphaseBreal[c]*btimg[j][0][1] + IphaseCreal[c]*btimg[j][0][2] + IphaseAimg[c]*btreal[j][0][0] + IphaseBimg[c]*btreal[j][0][1] + IphaseCimg[c]*btreal[j][0][2];
					VphaseBimg[f] = VphaseAimg[c]*at[j][1][0] + VphaseBimg[c]*at[j][1][1] + VphaseCimg[c]*at[j][1][2] + IphaseAreal[c]*btimg[j][1][0] + IphaseBreal[c]*btimg[j][1][1] + IphaseCreal[c]*btimg[j][1][2] + IphaseAimg[c]*btreal[j][1][0] + IphaseBimg[c]*btreal[j][1][1] + IphaseCimg[c]*btreal[j][1][2];
					VphaseCimg[f] = VphaseAimg[c]*at[j][2][0] + VphaseBimg[c]*at[j][2][1] + VphaseCimg[c]*at[j][2][2] + IphaseAreal[c]*btimg[j][2][0] + IphaseBreal[c]*btimg[j][2][1] + IphaseCreal[c]*btimg[j][2][2] + IphaseAimg[c]*btreal[j][2][0] + IphaseBimg[c]*btreal[j][2][1] + IphaseCimg[c]*btreal[j][2][2];
				    // calculate real part of current
					IphaseAreal[f] += IphaseAreal[c]*dt[j][0][0] + IphaseBreal[c]*dt[j][0][1] + IphaseCreal[c]*dt[j][0][2];
					IphaseBreal[f] += IphaseAreal[c]*dt[j][1][0] + IphaseBreal[c]*dt[j][1][1] + IphaseCreal[c]*dt[j][1][2];
					IphaseCreal[f] += IphaseAreal[c]*dt[j][2][0] + IphaseBreal[c]*dt[j][2][1] + IphaseCreal[c]*dt[j][2][2];		
			     	// calculate imaginary part of current
					IphaseAimg[f] += IphaseAimg[c]*dt[j][0][0] + IphaseBimg[c]*dt[j][0][1] + IphaseCimg[c]*dt[j][0][2];
					IphaseBimg[f] += IphaseAimg[c]*dt[j][1][0] + IphaseBimg[c]*dt[j][1][1] + IphaseCimg[c]*dt[j][1][2];
					IphaseCimg[f] += IphaseAimg[c]*dt[j][2][0] + IphaseBimg[c]*dt[j][2][1] + IphaseCimg[c]*dt[j][2][2];
				}
				else if (edgetype[j]==2) //if the selected link is a regulator
				{
					aRa[c] = 1 + 0.00625*TapA[c];
					aRb[c] = 1 + 0.00625*TapB[c];
					aRc[c] = 1 + 0.00625*TapC[c];
					dRa[c] = 1/aRa[c];
					dRb[c] = 1/aRb[c];
					dRc[c] = 1/aRc[c];
				    //calculate the father node voltage
					VphaseAreal[f] = VphaseAreal[c]*dRa[c];
					VphaseAimg[f] = VphaseAimg[c]*dRa[c];
					VphaseBreal[f] = VphaseBreal[c]*dRb[c];
					VphaseBimg[f] = VphaseBimg[c]*dRb[c];
					VphaseCreal[f] = VphaseCreal[c]*dRc[c];
					VphaseCimg[f] = VphaseCimg[c]*dRc[c];
				    //calculate the father node current
					IphaseAreal[f] += IphaseAreal[c]*aRa[c];
					IphaseAimg[f] += IphaseAimg[c]*aRa[c];
					IphaseBreal[f] += IphaseBreal[c]*aRb[c];
					IphaseBimg[f] += IphaseBimg[c]*aRb[c];
					IphaseCreal[f] += IphaseCreal[c]*aRc[c];
					IphaseCimg[f] += IphaseCimg[c]*aRc[c];
				}
			}	
			//update the child nodes, set the father nodes who have finished traversing all their child nodes as new child nodes in the next step 
			n = 0; k = 0;
			for (i=0; i<in; i++)
			{
				j = Fatherbuses[i] - 1;//get the indexes of the father nodes
				if (countf[j] == branchf[j])//all the child nodes of bus_i+1 have been calculated, so it can be selected as child node in the next step 
				{
					tempfather[n] = Fatherbuses[i]; //reduplicated father nodes needs to be reduced 
                    n += 1;					
				}		
			}
			for (i=0; i<n; i++)
			{
				CRflag = 0;
				for (j=i+1; j<n; j++)
				{
					if (tempfather[i] == tempfather[j])
					{
						CRflag = 1;
					}
				}			
				if (CRflag == 0)
				{
					Childbuses[k] = tempfather[i];
					k += 1;
				}
			}
			in = k;				
            //calculations at the father nodes, add the contribution of load current to those father nodes who have finished traversing all their child nodes.
            for (i=0; i<in; i++)
			{
                f = Childbuses[i] - 1; //get index			
				//get phase to ground and phase to phase voltage magnitude	
				VAmag[f] = sqrt(VphaseAreal[f]*VphaseAreal[f] + VphaseAimg[f]*VphaseAimg[f]);
				VBmag[f] = sqrt(VphaseBreal[f]*VphaseBreal[f] + VphaseBimg[f]*VphaseBimg[f]);
				VCmag[f] = sqrt(VphaseCreal[f]*VphaseCreal[f] + VphaseCimg[f]*VphaseCimg[f]);
				VphaseABreal[f] = VphaseAreal[f] - VphaseBreal[f];
				VphaseBCreal[f] = VphaseBreal[f] - VphaseCreal[f];
				VphaseCAreal[f] = VphaseCreal[f] - VphaseAreal[f];
				VphaseABimg[f] = VphaseAimg[f] - VphaseBimg[f];
				VphaseBCimg[f] = VphaseBimg[f] - VphaseCimg[f];
				VphaseCAimg[f] = VphaseCimg[f] - VphaseAimg[f];
				VABmag[f] = sqrt(VphaseABreal[f]*VphaseABreal[f] + VphaseABimg[f]*VphaseABimg[f]);
				VBCmag[f] = sqrt(VphaseBCreal[f]*VphaseBCreal[f] + VphaseBCimg[f]*VphaseBCimg[f]);
				VCAmag[f] = sqrt(VphaseCAreal[f]*VphaseCAreal[f] + VphaseCAimg[f]*VphaseCAimg[f]);
			    //get actual capacitor reactive power 
				CapA_cal[f] = CapA[f]*(VAmag[f]/Vbase[f])*(VAmag[f]/Vbase[f]);
				CapB_cal[f] = CapB[f]*(VBmag[f]/Vbase[f])*(VBmag[f]/Vbase[f]);
				CapC_cal[f] = CapC[f]*(VCmag[f]/Vbase[f])*(VCmag[f]/Vbase[f]);
			    //get actual load and current based on voltages
				if (LM[f] == 1)
				{
					PdA_cal[f] = PdA[f];
					QdA_cal[f] = QdA[f];
					PdB_cal[f] = PdB[f];
					QdB_cal[f] = QdB[f];
					PdC_cal[f] = PdC[f];
					QdC_cal[f] = QdC[f];
					IphaseAreal[f] += (PdA_cal[f]*VphaseAreal[f] + (QdA_cal[f]-CapA_cal[f])*VphaseAimg[f])/(VphaseAreal[f]*VphaseAreal[f] + VphaseAimg[f]*VphaseAimg[f]);
					IphaseBreal[f] += (PdB_cal[f]*VphaseBreal[f] + (QdB_cal[f]-CapB_cal[f])*VphaseBimg[f])/(VphaseBreal[f]*VphaseBreal[f] + VphaseBimg[f]*VphaseBimg[f]);
					IphaseCreal[f] += (PdC_cal[f]*VphaseCreal[f] + (QdC_cal[f]-CapC_cal[f])*VphaseCimg[f])/(VphaseCreal[f]*VphaseCreal[f] + VphaseCimg[f]*VphaseCimg[f]);
					IphaseAimg[f] += (PdA_cal[f]*VphaseAimg[f]-(QdA_cal[f]-CapA_cal[f])*VphaseAreal[f])/(VphaseAreal[f]*VphaseAreal[f] + VphaseAimg[f]*VphaseAimg[f]);
					IphaseBimg[f] += (PdB_cal[f]*VphaseBimg[f]-(QdB_cal[f]-CapB_cal[f])*VphaseBreal[f])/(VphaseBreal[f]*VphaseBreal[f] + VphaseBimg[f]*VphaseBimg[f]);
					IphaseCimg[f] += (PdC_cal[f]*VphaseCimg[f]-(QdC_cal[f]-CapC_cal[f])*VphaseCreal[f])/(VphaseCreal[f]*VphaseCreal[f] + VphaseCimg[f]*VphaseCimg[f]);
					
				}
				else if (LM[f] == 2)
				{
					PdA_cal[f] = PdA[f]*(VAmag[f]/Vbase[f])*(VAmag[f]/Vbase[f]);
					QdA_cal[f] = QdA[f]*(VAmag[f]/Vbase[f])*(VAmag[f]/Vbase[f]);
					PdB_cal[f] = PdB[f]*(VBmag[f]/Vbase[f])*(VBmag[f]/Vbase[f]);
					QdB_cal[f] = QdB[f]*(VBmag[f]/Vbase[f])*(VBmag[f]/Vbase[f]);
					PdC_cal[f] = PdC[f]*(VCmag[f]/Vbase[f])*(VCmag[f]/Vbase[f]);
					QdC_cal[f] = QdC[f]*(VCmag[f]/Vbase[f])*(VCmag[f]/Vbase[f]);
					IphaseAreal[f] += (PdA_cal[f]*VphaseAreal[f] + (QdA_cal[f]-CapA_cal[f])*VphaseAimg[f])/(VphaseAreal[f]*VphaseAreal[f] + VphaseAimg[f]*VphaseAimg[f]);
					IphaseBreal[f] += (PdB_cal[f]*VphaseBreal[f] + (QdB_cal[f]-CapB_cal[f])*VphaseBimg[f])/(VphaseBreal[f]*VphaseBreal[f] + VphaseBimg[f]*VphaseBimg[f]);
					IphaseCreal[f] += (PdC_cal[f]*VphaseCreal[f] + (QdC_cal[f]-CapC_cal[f])*VphaseCimg[f])/(VphaseCreal[f]*VphaseCreal[f] + VphaseCimg[f]*VphaseCimg[f]);
					IphaseAimg[f] += (PdA_cal[f]*VphaseAimg[f]-(QdA_cal[f]-CapA_cal[f])*VphaseAreal[f])/(VphaseAreal[f]*VphaseAreal[f] + VphaseAimg[f]*VphaseAimg[f]);
					IphaseBimg[f] += (PdB_cal[f]*VphaseBimg[f]-(QdB_cal[f]-CapB_cal[f])*VphaseBreal[f])/(VphaseBreal[f]*VphaseBreal[f] + VphaseBimg[f]*VphaseBimg[f]);
					IphaseCimg[f] += (PdC_cal[f]*VphaseCimg[f]-(QdC_cal[f]-CapC_cal[f])*VphaseCreal[f])/(VphaseCreal[f]*VphaseCreal[f] + VphaseCimg[f]*VphaseCimg[f]);
				}
				else if (LM[f] == 3)
				{
					PdA_cal[f] = PdA[f]*(VAmag[f]/Vbase[f]);
					QdA_cal[f] = QdA[f]*(VAmag[f]/Vbase[f]);
					PdB_cal[f] = PdB[f]*(VBmag[f]/Vbase[f]);
					QdB_cal[f] = QdB[f]*(VBmag[f]/Vbase[f]);
					PdC_cal[f] = PdC[f]*(VCmag[f]/Vbase[f]);
					QdC_cal[f] = QdC[f]*(VCmag[f]/Vbase[f]);
					IphaseAreal[f] += (PdA_cal[f]*VphaseAreal[f] + (QdA_cal[f]-CapA_cal[f])*VphaseAimg[f])/(VphaseAreal[f]*VphaseAreal[f] + VphaseAimg[f]*VphaseAimg[f]);
					IphaseBreal[f] += (PdB_cal[f]*VphaseBreal[f] + (QdB_cal[f]-CapB_cal[f])*VphaseBimg[f])/(VphaseBreal[f]*VphaseBreal[f] + VphaseBimg[f]*VphaseBimg[f]);
					IphaseCreal[f] += (PdC_cal[f]*VphaseCreal[f] + (QdC_cal[f]-CapC_cal[f])*VphaseCimg[f])/(VphaseCreal[f]*VphaseCreal[f] + VphaseCimg[f]*VphaseCimg[f]);
					IphaseAimg[f] += (PdA_cal[f]*VphaseAimg[f]-(QdA_cal[f]-CapA_cal[f])*VphaseAreal[f])/(VphaseAreal[f]*VphaseAreal[f] + VphaseAimg[f]*VphaseAimg[f]);
					IphaseBimg[f] += (PdB_cal[f]*VphaseBimg[f]-(QdB_cal[f]-CapB_cal[f])*VphaseBreal[f])/(VphaseBreal[f]*VphaseBreal[f] + VphaseBimg[f]*VphaseBimg[f]);
					IphaseCimg[f] += (PdC_cal[f]*VphaseCimg[f]-(QdC_cal[f]-CapC_cal[f])*VphaseCreal[f])/(VphaseCreal[f]*VphaseCreal[f] + VphaseCimg[f]*VphaseCimg[f]);
				}
				else if (LM[f] == 4)
				{
					PdAB_cal[f] = PdA[f];
					QdAB_cal[f] = QdA[f];
					PdBC_cal[f] = PdB[f];
					QdBC_cal[f] = QdB[f];
					PdCA_cal[f] = PdC[f];
					QdCA_cal[f] = QdC[f];
					IphaseABreal[f] = (PdAB_cal[f]*VphaseABreal[f] + QdAB_cal[f]*VphaseABimg[f])/(VphaseABreal[f]*VphaseABreal[f] + VphaseABimg[f]*VphaseABimg[f]);
					IphaseABimg[f] = (PdAB_cal[f]*VphaseABimg[f] - QdAB_cal[f]*VphaseABreal[f])/(VphaseABreal[f]*VphaseABreal[f] + VphaseABimg[f]*VphaseABimg[f]);
					IphaseBCreal[f] = (PdBC_cal[f]*VphaseBCreal[f] + QdBC_cal[f]*VphaseBCimg[f])/(VphaseBCreal[f]*VphaseBCreal[f] + VphaseBCimg[f]*VphaseBCimg[f]);
					IphaseBCimg[f] = (PdBC_cal[f]*VphaseBCimg[f] - QdBC_cal[f]*VphaseBCreal[f])/(VphaseBCreal[f]*VphaseBCreal[f] + VphaseBCimg[f]*VphaseBCimg[f]);
					IphaseCAreal[f] = (PdCA_cal[f]*VphaseCAreal[f] + QdCA_cal[f]*VphaseCAimg[f])/(VphaseCAreal[f]*VphaseCAreal[f] + VphaseCAimg[f]*VphaseCAimg[f]);
					IphaseCAimg[f] = (PdCA_cal[f]*VphaseCAimg[f] - QdCA_cal[f]*VphaseCAreal[f])/(VphaseCAreal[f]*VphaseCAreal[f] + VphaseCAimg[f]*VphaseCAimg[f]);
					IphaseAreal[f] += IphaseABreal[f]-IphaseCAreal[f]-CapA_cal[f]*VphaseAimg[f]/(VphaseAreal[f]*VphaseAreal[f] + VphaseAimg[f]*VphaseAimg[f]);
					IphaseAimg[f] += IphaseABimg[f]-IphaseCAimg[f]+CapA_cal[f]*VphaseAreal[f]/(VphaseAreal[f]*VphaseAreal[f] + VphaseAimg[f]*VphaseAimg[f]);
					IphaseBreal[f] += IphaseBCreal[f]-IphaseABreal[f]-CapB_cal[f]*VphaseBimg[f]/(VphaseBreal[f]*VphaseBreal[f] + VphaseBimg[f]*VphaseBimg[f]);
					IphaseBimg[f] += IphaseBCimg[f]-IphaseABimg[f]+CapB_cal[f]*VphaseBreal[f]/(VphaseBreal[f]*VphaseBreal[f] + VphaseBimg[f]*VphaseBimg[f]);
					IphaseCreal[f] += IphaseCAreal[f]-IphaseBCreal[f]-CapC_cal[f]*VphaseCimg[f]/(VphaseCreal[f]*VphaseCreal[f] + VphaseCimg[f]*VphaseCimg[f]);
					IphaseCimg[f] += IphaseCAimg[f]-IphaseBCimg[f]+CapC_cal[f]*VphaseCreal[f]/(VphaseCreal[f]*VphaseCreal[f] + VphaseCimg[f]*VphaseCimg[f]);
				}
				else if (LM[f] == 5)
				{
					PdAB_cal[f] = PdA[f]*(VABmag[f]/Vbase[f]/sqrt(3))*(VABmag[f]/Vbase[f]/sqrt(3));
					QdAB_cal[f] = QdA[f]*(VABmag[f]/Vbase[f]/sqrt(3))*(VABmag[f]/Vbase[f]/sqrt(3));
					PdBC_cal[f] = PdB[f]*(VBCmag[f]/Vbase[f]/sqrt(3))*(VBCmag[f]/Vbase[f]/sqrt(3));
					QdBC_cal[f] = QdB[f]*(VBCmag[f]/Vbase[f]/sqrt(3))*(VBCmag[f]/Vbase[f]/sqrt(3));
					PdCA_cal[f] = PdC[f]*(VCAmag[f]/Vbase[f]/sqrt(3))*(VCAmag[f]/Vbase[f]/sqrt(3));
					QdCA_cal[f] = QdC[f]*(VCAmag[f]/Vbase[f]/sqrt(3))*(VCAmag[f]/Vbase[f]/sqrt(3));
					IphaseABreal[f] = (PdAB_cal[f]*VphaseABreal[f] + QdAB_cal[f]*VphaseABimg[f])/(VphaseABreal[f]*VphaseABreal[f] + VphaseABimg[f]*VphaseABimg[f]);
					IphaseABimg[f] = (PdAB_cal[f]*VphaseABimg[f] - QdAB_cal[f]*VphaseABreal[f])/(VphaseABreal[f]*VphaseABreal[f] + VphaseABimg[f]*VphaseABimg[f]);
					IphaseBCreal[f] = (PdBC_cal[f]*VphaseBCreal[f] + QdBC_cal[f]*VphaseBCimg[f])/(VphaseBCreal[f]*VphaseBCreal[f] + VphaseBCimg[f]*VphaseBCimg[f]);
					IphaseBCimg[f] = (PdBC_cal[f]*VphaseBCimg[f] - QdBC_cal[f]*VphaseBCreal[f])/(VphaseBCreal[f]*VphaseBCreal[f] + VphaseBCimg[f]*VphaseBCimg[f]);
					IphaseCAreal[f] = (PdCA_cal[f]*VphaseCAreal[f] + QdCA_cal[f]*VphaseCAimg[f])/(VphaseCAreal[f]*VphaseCAreal[f] + VphaseCAimg[f]*VphaseCAimg[f]);
					IphaseCAimg[f] = (PdCA_cal[f]*VphaseCAimg[f] - QdCA_cal[f]*VphaseCAreal[f])/(VphaseCAreal[f]*VphaseCAreal[f] + VphaseCAimg[f]*VphaseCAimg[f]);
					IphaseAreal[f] += IphaseABreal[f]-IphaseCAreal[f]-CapA_cal[f]*VphaseAimg[f]/(VphaseAreal[f]*VphaseAreal[f] + VphaseAimg[f]*VphaseAimg[f]);
					IphaseAimg[f] += IphaseABimg[f]-IphaseCAimg[f]+CapA_cal[f]*VphaseAreal[f]/(VphaseAreal[f]*VphaseAreal[f] + VphaseAimg[f]*VphaseAimg[f]);
					IphaseBreal[f] += IphaseBCreal[f]-IphaseABreal[f]-CapB_cal[f]*VphaseBimg[f]/(VphaseBreal[f]*VphaseBreal[f] + VphaseBimg[f]*VphaseBimg[f]);
					IphaseBimg[f] += IphaseBCimg[f]-IphaseABimg[f]+CapB_cal[f]*VphaseBreal[f]/(VphaseBreal[f]*VphaseBreal[f] + VphaseBimg[f]*VphaseBimg[f]);
					IphaseCreal[f] += IphaseCAreal[f]-IphaseBCreal[f]-CapC_cal[f]*VphaseCimg[f]/(VphaseCreal[f]*VphaseCreal[f] + VphaseCimg[f]*VphaseCimg[f]);
					IphaseCimg[f] += IphaseCAimg[f]-IphaseBCimg[f]+CapC_cal[f]*VphaseCreal[f]/(VphaseCreal[f]*VphaseCreal[f] + VphaseCimg[f]*VphaseCimg[f]);
				}
				else if (LM[f] == 6)
				{
					PdAB_cal[f] = PdA[f]*(VABmag[f]/Vbase[f]/sqrt(3));
					QdAB_cal[f] = QdA[f]*(VABmag[f]/Vbase[f]/sqrt(3));
					PdBC_cal[f] = PdB[f]*(VBCmag[f]/Vbase[f]/sqrt(3));
					QdBC_cal[f] = QdB[f]*(VBCmag[f]/Vbase[f]/sqrt(3));
					PdCA_cal[f] = PdC[f]*(VCAmag[f]/Vbase[f]/sqrt(3));
					QdCA_cal[f] = QdC[f]*(VCAmag[f]/Vbase[f]/sqrt(3));
					IphaseABreal[f] = (PdAB_cal[f]*VphaseABreal[f] + QdAB_cal[f]*VphaseABimg[f])/(VphaseABreal[f]*VphaseABreal[f] + VphaseABimg[f]*VphaseABimg[f]);
					IphaseABimg[f] = (PdAB_cal[f]*VphaseABimg[f] - QdAB_cal[f]*VphaseABreal[f])/(VphaseABreal[f]*VphaseABreal[f] + VphaseABimg[f]*VphaseABimg[f]);
					IphaseBCreal[f] = (PdBC_cal[f]*VphaseBCreal[f] + QdBC_cal[f]*VphaseBCimg[f])/(VphaseBCreal[f]*VphaseBCreal[f] + VphaseBCimg[f]*VphaseBCimg[f]);
					IphaseBCimg[f] = (PdBC_cal[f]*VphaseBCimg[f] - QdBC_cal[f]*VphaseBCreal[f])/(VphaseBCreal[f]*VphaseBCreal[f] + VphaseBCimg[f]*VphaseBCimg[f]);
					IphaseCAreal[f] = (PdCA_cal[f]*VphaseCAreal[f] + QdCA_cal[f]*VphaseCAimg[f])/(VphaseCAreal[f]*VphaseCAreal[f] + VphaseCAimg[f]*VphaseCAimg[f]);
					IphaseCAimg[f] = (PdCA_cal[f]*VphaseCAimg[f] - QdCA_cal[f]*VphaseCAreal[f])/(VphaseCAreal[f]*VphaseCAreal[f] + VphaseCAimg[f]*VphaseCAimg[f]);
					IphaseAreal[f] += IphaseABreal[f]-IphaseCAreal[f]-CapA_cal[f]*VphaseAimg[f]/(VphaseAreal[f]*VphaseAreal[f] + VphaseAimg[f]*VphaseAimg[f]);
					IphaseAimg[f] += IphaseABimg[f]-IphaseCAimg[f]+CapA_cal[f]*VphaseAreal[f]/(VphaseAreal[f]*VphaseAreal[f] + VphaseAimg[f]*VphaseAimg[f]);
					IphaseBreal[f] += IphaseBCreal[f]-IphaseABreal[f]-CapB_cal[f]*VphaseBimg[f]/(VphaseBreal[f]*VphaseBreal[f] + VphaseBimg[f]*VphaseBimg[f]);
					IphaseBimg[f] += IphaseBCimg[f]-IphaseABimg[f]+CapB_cal[f]*VphaseBreal[f]/(VphaseBreal[f]*VphaseBreal[f] + VphaseBimg[f]*VphaseBimg[f]);
					IphaseCreal[f] += IphaseCAreal[f]-IphaseBCreal[f]-CapC_cal[f]*VphaseCimg[f]/(VphaseCreal[f]*VphaseCreal[f] + VphaseCimg[f]*VphaseCimg[f]);
					IphaseCimg[f] += IphaseCAimg[f]-IphaseBCimg[f]+CapC_cal[f]*VphaseCreal[f]/(VphaseCreal[f]*VphaseCreal[f] + VphaseCimg[f]*VphaseCimg[f]);
				}
				
			    
				// calculation at the start node
				if (f == SNindex)
				{
					Change = (abs(VAmag[f] - Vbase[f]) + abs(VBmag[f] - Vbase[f]) + abs(VCmag[f] - Vbase[f]))/3;
					Accuracy = Change/Vbase[f]; // accuracy in p.u.
				}
		    }
			
	 printf("\n\n================================================================================================\n ");
 	for (i=0;i<nv;i++)
	{
		std::cout << "ID:" << i+1 <<" VAr: "<<VphaseAreal[i]<<" VAi "<<VphaseAimg[i]<<" VBr: "<<VphaseBreal[i]<<" VBi "<<VphaseBimg[i]<<" VCr: "<<VphaseCreal[i]<<" VCi "<<VphaseCimg[i] <<" Vbase "<<Vbase[i]<<" IAr: "<<IphaseAreal[i]<<" IAi "<<IphaseAimg[i]<<" IBr: "<<IphaseBreal[i]<<" IBi "<<IphaseBimg[i]<<" ICr: "<<IphaseCreal[i]<<" ICi "<<IphaseCimg[i]<<" loadmodel "<<LM[i] << std::endl;
	}
	std::cout << "Maxit : "<<Maxit <<" it "<<iter<<" Maxchange :" << Maxchange <<" Accuracy "<<Accuracy<< std::endl; 
	std::cout << "SNcount:" << countf[SNindex] <<" SNbranch: "<<branchf[SNindex]<<" lv "<<lv <<" f "<<f<< std::endl;	

	printf("\n================================================================================================ \n\n"); 
		
		lv++;
				
		}
		lv = 0;	
        endflag = 0;	
		// backward sweep, update voltages 
        while (endflag == 0 && lv<100)		
		{	
	        if (lv == 0) //first step 
			{
				in = 1;
				Fatherbuses[in-1]=SNindex+1;
				VphaseAreal[SNindex] = Vbase[SNindex];
				VphaseAimg[SNindex] = 0;
				VphaseBreal[SNindex] = Vbase[SNindex]*(-0.5);
				VphaseBimg[SNindex] = Vbase[SNindex]*(-0.866025);
				VphaseCreal[SNindex] = Vbase[SNindex]*(-0.5);
				VphaseCimg[SNindex] = Vbase[SNindex]*(0.866025);
				
			}
	        k = 0;
		    for (i=0; i<in; i++)
			{
				for (j=0; j<ne; j++)
					{   
						if(IDF[j] == Fatherbuses[i])
						{
							Links[k] = j; //store the link index between the child nodes and father nodes.
							Childbuses[k] = IDT[j]; //store the id of child nodes.
							countc[IDT[j]-1] += 1; //Array_id = bus_id -1
							k += 1;//number of child nodes
							
						

						}
					}
			}
			in = k;
			for (i=0; i<in; i++)
			{
				j = Links[i]; //selected link index
				f = IDF[j]-1; //selected father node index
				c = IDT[j]-1; //selected child node index
				//std::cout<<"inid "<<in<<" fid "<<i << " Fatherbuses :" << IDF[j] <<" cid "<< k<<" Childbuses "<<IDT[j]<<" countf "<<countc[c]<< std::endl;

				if (edgetype[j]==0) //if the selected link is a feeder
				{
					VphaseAreal[c] = Areal[j][0][0]*VphaseAreal[f] + Areal[j][0][1]*VphaseBreal[f] + Areal[j][0][2]*VphaseCreal[f] - Aimg[j][0][0]*VphaseAimg[f] - Aimg[j][0][1]*VphaseBimg[f] - Aimg[j][0][2]*VphaseCimg[f] - Breal[j][0][0]*IphaseAreal[c] - Breal[j][0][1]*IphaseBreal[c] - Breal[j][0][2]*IphaseCreal[c] + Bimg[j][0][0]*IphaseAimg[c] + Bimg[j][0][1]*IphaseBimg[c] + Bimg[j][0][2]*IphaseCimg[c];
					VphaseBreal[c] = Areal[j][1][0]*VphaseAreal[f] + Areal[j][1][1]*VphaseBreal[f] + Areal[j][1][2]*VphaseCreal[f] - Aimg[j][1][0]*VphaseAimg[f] - Aimg[j][1][1]*VphaseBimg[f] - Aimg[j][1][2]*VphaseCimg[f] - Breal[j][1][0]*IphaseAreal[c] - Breal[j][1][1]*IphaseBreal[c] - Breal[j][1][2]*IphaseCreal[c] + Bimg[j][1][0]*IphaseAimg[c] + Bimg[j][1][1]*IphaseBimg[c] + Bimg[j][1][2]*IphaseCimg[c];
					VphaseCreal[c] = Areal[j][2][0]*VphaseAreal[f] + Areal[j][2][1]*VphaseBreal[f] + Areal[j][2][2]*VphaseCreal[f] - Aimg[j][2][0]*VphaseAimg[f] - Aimg[j][2][1]*VphaseBimg[f] - Aimg[j][2][2]*VphaseCimg[f] - Breal[j][2][0]*IphaseAreal[c] - Breal[j][2][1]*IphaseBreal[c] - Breal[j][2][2]*IphaseCreal[c] + Bimg[j][2][0]*IphaseAimg[c] + Bimg[j][2][1]*IphaseBimg[c] + Bimg[j][2][2]*IphaseCimg[c];
					
					VphaseAimg[c] = Aimg[j][0][0]*VphaseAreal[f] + Aimg[j][0][1]*VphaseBreal[f] + Aimg[j][0][2]*VphaseCreal[f] + Areal[j][0][0]*VphaseAimg[f] + Areal[j][0][1]*VphaseBimg[f] + Areal[j][0][2]*VphaseCimg[f] - Breal[j][0][0]*IphaseAimg[c] - Breal[j][0][1]*IphaseBimg[c] - Breal[j][0][2]*IphaseCimg[c] - Bimg[j][0][0]*IphaseAreal[c] - Bimg[j][0][1]*IphaseBreal[c] - Bimg[j][0][2]*IphaseCreal[c];
					VphaseBimg[c] = Aimg[j][1][0]*VphaseAreal[f] + Aimg[j][1][1]*VphaseBreal[f] + Aimg[j][1][2]*VphaseCreal[f] + Areal[j][1][0]*VphaseAimg[f] + Areal[j][1][1]*VphaseBimg[f] + Areal[j][1][2]*VphaseCimg[f] - Breal[j][1][0]*IphaseAimg[c] - Breal[j][1][1]*IphaseBimg[c] - Breal[j][1][2]*IphaseCimg[c] - Bimg[j][1][0]*IphaseAreal[c] - Bimg[j][1][1]*IphaseBreal[c] - Bimg[j][1][2]*IphaseCreal[c];
					VphaseCimg[c] = Aimg[j][2][0]*VphaseAreal[f] + Aimg[j][2][1]*VphaseBreal[f] + Aimg[j][2][2]*VphaseCreal[f] + Areal[j][2][0]*VphaseAimg[f] + Areal[j][2][1]*VphaseBimg[f] + Areal[j][2][2]*VphaseCimg[f] - Breal[j][2][0]*IphaseAimg[c] - Breal[j][2][1]*IphaseBimg[c] - Breal[j][2][2]*IphaseCimg[c] - Bimg[j][2][0]*IphaseAreal[c] - Bimg[j][2][1]*IphaseBreal[c] - Bimg[j][2][2]*IphaseCreal[c];
								
				}
				else if (edgetype[j]==1) //if the selected link is a transformer
				{
					VphaseAreal[c] = At[j][0][0]*VphaseAreal[f] + At[j][0][1]*VphaseBreal[f] + At[j][0][2]*VphaseCreal[f] - Raa[j]*IphaseAreal[c] + Xaa[j]*IphaseAimg[c];
					VphaseBreal[c] = At[j][1][0]*VphaseAreal[f] + At[j][1][1]*VphaseBreal[f] + At[j][1][2]*VphaseCreal[f] - Rbb[j]*IphaseBreal[c] + Xbb[j]*IphaseBimg[c];
					VphaseCreal[c] = At[j][2][0]*VphaseAreal[f] + At[j][2][1]*VphaseBreal[f] + At[j][2][2]*VphaseCreal[f] - Rcc[j]*IphaseCreal[c] + Xcc[j]*IphaseCimg[c];
					
					VphaseAimg[c] = At[j][0][0]*VphaseAimg[f] + At[j][0][1]*VphaseBimg[f] + At[j][0][2]*VphaseCimg[f] - Xaa[j]*IphaseAreal[c] - Raa[j]*IphaseAimg[c];
					VphaseBimg[c] = At[j][1][0]*VphaseAimg[f] + At[j][1][1]*VphaseBimg[f] + At[j][1][2]*VphaseCimg[f] - Xbb[j]*IphaseBreal[c] - Rbb[j]*IphaseBimg[c];
					VphaseCimg[c] = At[j][2][0]*VphaseAimg[f] + At[j][2][1]*VphaseBimg[f] + At[j][2][2]*VphaseCimg[f] - Xcc[j]*IphaseCreal[c] - Rcc[j]*IphaseCimg[c];
				}
				else if (edgetype[j]==2) //if the selected link is a regulator
				{
					/* tempVregAR = VphaseAreal[f]/PT_reg[c];
					tempVregAI = VphaseAimg[f]/PT_reg[c];
					tempVregBR = VphaseBreal[f]/PT_reg[c];
					tempVregBI = VphaseBimg[f]/PT_reg[c];
					tempVregCR = VphaseCreal[f]/PT_reg[c];
					tempVregCI = VphaseCimg[f]/PT_reg[c];
					tempIcompAR = IphaseAreal[c]/CT_reg[c];
					tempIcompAI = IphaseAimg[c]/CT_reg[c];
					tempIcompBR = IphaseBreal[c]/CT_reg[c];
					tempIcompBI = IphaseBimg[c]/CT_reg[c];
					tempIcompCR = IphaseCreal[c]/CT_reg[c];
					tempIcompCI = IphaseCimg[c]/CT_reg[c];
					VrelyAR = tempVregAR - (Raa[j]*tempIcompAR - Xaa[j]*tempIcompAI);
					VrelyAI = tempVregAI - (Raa[j]*tempIcompAI + Xaa[j]*tempIcompAR);
					VrelyBR = tempVregBR - (Rbb[j]*tempIcompBR - Xbb[j]*tempIcompBI);
					VrelyBI = tempVregBI - (Rbb[j]*tempIcompBI + Xbb[j]*tempIcompBR);
					VrelyCR = tempVregCR - (Rcc[j]*tempIcompCR - Xcc[j]*tempIcompCI);
					VrelyCI = tempVregCI - (Rcc[j]*tempIcompCI + Xcc[j]*tempIcompCR);
					VrelyAmag = sqrt(VrelyAR*VrelyAR + VrelyAI*VrelyAI);
					VrelyBmag = sqrt(VrelyBR*VrelyBR + VrelyBI*VrelyBI);
					VrelyCmag = sqrt(VrelyCR*VrelyCR + VrelyCI*VrelyCI);
					//calcuate tap 
					TapVlow_A=vol_reg_A[c] - 0.5*BW_reg[c];
					TapVhigh_A=vol_reg_A[c] + 0.5*BW_reg[c];
					TapVlow_B=vol_reg_B[c] - 0.5*BW_reg[c];
					TapVhigh_B=vol_reg_B[c] + 0.5*BW_reg[c];
					TapVlow_C=vol_reg_C[c] - 0.5*BW_reg[c];
					TapVhigh_C=vol_reg_C[c] + 0.5*BW_reg[c];
			
					if (TapVlow_A>VrelyAmag) 
					{
						TapA[c] = floor((TapVlow_A - VrelyAmag)/0.75)+1;
						if (TapA[c]>16) 
						{
							TapA[c]=16;
						}
					}
					else if (TapVhigh_A<=VrelyAmag) 
						TapA[c] = floor((TapVhigh_A - VrelyAmag)/0.75);
						if (TapA[c]<-16)
						{
							TapA[c]=-16;
						}
					if (TapVlow_B>VrelyBmag)
					{
						TapB[c] = floor((TapVlow_B - VrelyBmag)/0.75)+1;
						if (TapB[c]>16)
						{
							TapB[c] = 16;
						}
					}
					else if (TapVhigh_B<=VrelyBmag)
					{
						TapB[c] = floor((TapVhigh_B - VrelyBmag)/0.75);
						if (TapB[c]<-16)
						{
							TapB[c]=-16;
						}
					}
					if (TapVlow_C>VrelyCmag)
					{
						TapC[c] = floor((TapVlow_C - VrelyCmag)/0.75)+1;
						if (TapC[c]>16)
						{
							TapC[c] = 16;
						}
					}
					else if (TapVhigh_C<=VrelyCmag)
					{
						TapC[c] = floor((TapVhigh_C - VrelyCmag)/0.75);
						if (TapC[c]<-16)
						{
							TapC[c]=-16;
						}
					}
					//determine the monitoring phase
					if (control_phase_reg[c]==1)
					{
						TapB[c]=TapA[c];
						TapC[c]=TapA[c];
					}
					else if (control_phase_reg[c]==2)
					{
						TapA[c]=TapB[c];
						TapC[c]=TapB[c];
					}
					else if (control_phase_reg[c]==3)
					{
						TapA[c]=TapC[c];
						TapB[c]=TapC[c];
					}
					aRa[c] = 1 + 0.00625*TapA[c];
					aRb[c] = 1 + 0.00625*TapB[c];
					aRc[c] = 1 + 0.00625*TapC[c];
					dRa[c] = 1/aRa[c];
					dRb[c] = 1/aRb[c];
					dRc[c] = 1/aRc[c]; */
					VphaseAreal[c] = VphaseAreal[f]*aRa[c];
					VphaseAimg[c] = VphaseAimg[f]*aRa[c];
					VphaseBreal[c] = VphaseBreal[f]*aRb[c];
					VphaseBimg[c] = VphaseBimg[f]*aRb[c];
					VphaseCreal[c] = VphaseCreal[f]*aRc[c];
					VphaseCimg[c] = VphaseCimg[f]*aRc[c];
				}	
			}
			//update father nodes
			for (i=0; i<in; i++)
			{
				Fatherbuses[i] = Childbuses[i];
			}
			//check if all the node voltages are updated
			endflag = 1;
			for (i=0; i<tn; i++)
			{
				j = Terminalbuses[i] - 1;//get the index of terminal buses
				if (countc[j] == 0)
				{
					endflag = 0;
				}
			//std::cout<<"lv "<<lv << " Terminal "<<Terminalbuses[i]<<" countc :" << countc[j] <<" endflag "<<endflag<< std::endl;

			}		
		lv++;
		}
	    std::cout<<"iter "<<iter << " Maxchange :" << Maxchange <<" accuracy "<<Accuracy<< std::endl;
		// If stopping criteria satisfied, then break out the for loop.
    	if (Maxchange > Accuracy)
		{
			break;
		}  
		
			
   		// clear counter and currents
		for (i=0; i<nv; i++)
		{
			countf[i] = 0;
			countc[i] = 0;
			IphaseAreal[i] = 0;
			IphaseAimg[i] = 0;
			IphaseBreal[i] = 0;
			IphaseBimg[i] = 0;
			IphaseCreal[i] = 0;
			IphaseCimg[i] = 0;
		}		
	}
	
    // Re-calculation voltages and accuracy for output, then store results back to the array	
	for (i=0; i<nv; i++) 
	{	
		VAmag[i] = sqrt(VphaseAreal[i]*VphaseAreal[i] + VphaseAimg[i]*VphaseAimg[i]);		
		VBmag[i] = sqrt(VphaseBreal[i]*VphaseBreal[i] + VphaseBimg[i]*VphaseBimg[i]);		
		VCmag[i] = sqrt(VphaseCreal[i]*VphaseCreal[i] + VphaseCimg[i]*VphaseCimg[i]);
		if (VphaseAreal[i] > 0)
		{
			VAang[i] = atan(VphaseAimg[i]/VphaseAreal[i])/pi_value*180;
		} 
		else {
			VAang[i] = atan(VphaseAimg[i]/VphaseAreal[i])/pi_value*180 + 180;
		} 
		if (VphaseBreal[i] > 0)
		{
			VBang[i] = atan(VphaseBimg[i]/VphaseBreal[i])/pi_value*180;
		} 
		else {
			VBang[i] = atan(VphaseBimg[i]/VphaseBreal[i])/pi_value*180 - 180;
		}
		if (VphaseCreal[i] > 0)
		{
			VCang[i] = atan(VphaseCimg[i]/VphaseCreal[i])/pi_value*180;
		} 
		else {
			VCang[i] = atan(VphaseCimg[i]/VphaseCreal[i])/pi_value*180 + 180;
		}	
		Vertex_D.data_[i].VAang = VAang[i];
		Vertex_D.data_[i].VAmag = VAmag[i]/Vbase[i];
		Vertex_D.data_[i].VBang = VBang[i];
		Vertex_D.data_[i].VBmag = VBmag[i]/Vbase[i];
		Vertex_D.data_[i].VCang = VCang[i];
		Vertex_D.data_[i].VCmag = VCmag[i]/Vbase[i];
	} 
	Final_iter = iter+1;
	Final_accuracy = Accuracy;
    std::cout << "Faccracy: " << Final_accuracy << std::endl;	
	
	
	// free memory
	free(Terminalbuses); free(Links); free(Childbuses); free(Fatherbuses); free(bustype); free(edgetype);
	free(countf); free(branchf); free(ID); free(IDF); free(IDT); free(VphaseAreal); free(VphaseAimg); 
	free(VphaseBreal); free(VphaseBimg); free(VphaseCreal); free(VphaseCimg); free(VAmag); free(VBmag); 
	free(VCmag); free(VABmag); free(VBCmag); free(VCAmag); free(VphaseABreal); free(VphaseABimg); 
	free(VphaseBCreal); free(VphaseBCimg); free(VphaseCAreal); free(VphaseCAimg); free(Vbase); free(CapA); 
	free(CapB); free(CapC); free(CapA_cal); free(CapB_cal); free(CapC_cal); free(PdA); free(QdA); free(PdB); 
	free(QdB); free(PdC); free(QdC); free(PdA_cal); free(QdA_cal); free(PdB_cal); free(QdB_cal); free(PdC_cal); 
	free(QdC_cal); free(PdAB_cal); free(QdAB_cal); free(PdBC_cal); free(QdBC_cal); free(PdCA_cal); free(QdCA_cal); 
	free(aRa); free(aRb); free(aRc); free(dRa); free(dRb); free(dRc); free(IphaseAreal); free(IphaseAimg); free(IphaseBreal); 
	free(IphaseBimg); free(IphaseCreal); free(IphaseCimg); free(IphaseABreal); free(IphaseABimg); free(IphaseBCreal); 
	free(IphaseBCimg); free(IphaseCAreal); free(IphaseCAimg); free(Raa); free(Rab); free(Rac); free(Rba); free(Rbc);
	free(Rca); free(Rcb); free(Rcc); free(Xaa); free(Xab); free(Xac); free(Xba); free(Xbb); free(Xbc); free(Xca); 
	free(Xcb); free(Xcc); free(Nt); 
	
}  

template<typename sort_edge, typename edge_comp, typename sort_vertex, typename vertex_comp>
inline void FBS_pu(HeapAccum<sort_edge, edge_comp>& Edge_D, HeapAccum<sort_vertex, vertex_comp>& Vertex_D, int64_t Maxit, double Maxchange, int64_t SNindex, double SNVm, double SNVs, int64_t &Final_iter, double &Final_accuracy)
{
	// Initialize arrays and variables
	uint__t ne, nv;
	int i, j, k, f, c, n, in, tn, lv, iter, endflag, CRflag;
	int *TapA, *TapB, *TapC;
	double Change, Accuracy, tempVregAR, tempVregAI, tempVregBR, tempVregBI, tempVregCR, tempVregCI, tempIcompAR, tempIcompAI, tempIcompBR, tempIcompBI, tempIcompCR, tempIcompCI, VrelyAR, VrelyAI, VrelyBR, VrelyBI, VrelyCR, VrelyCI, VrelyAmag, VrelyBmag, VrelyCmag, TapVlow_A, TapVhigh_A, TapVlow_B, TapVhigh_B, TapVlow_C, TapVhigh_C;
    double totalP = 0;
	double totalQ = 0;
	const double pi_value = 3.141592653589793;	
	// For radio network ne=nv-1
	ne = Edge_D.data_.size(); 
	nv = Vertex_D.data_.size();	
	uint__t *Terminalbuses, *Links, *Childbuses, *Fatherbuses, *tempfather, *bustype, *edgetype, *countf, *countc, *branchf, *ID, *IDF, *IDT, *LM, *control_phase_reg;
	real__t *VphaseAreal, *VphaseAimg, *VphaseBreal, *VphaseBimg, *VphaseCreal, *VphaseCimg, *VAmag, *VBmag, *VCmag, *VAang, *VBang, *VCang, *VABmag, *VBCmag, *VCAmag, *VphaseABreal, *VphaseABimg, *VphaseBCreal, *VphaseBCimg, *VphaseCAreal, *VphaseCAimg, *Vbase, *CapA, *CapB, *CapC, *CapA_cal, *CapB_cal, *CapC_cal, *PdA, *QdA, *PdB, *QdB, *PdC, *QdC, *PdA_cal, *QdA_cal, *PdB_cal, *QdB_cal, *PdC_cal, *QdC_cal, *PdAB_cal, *QdAB_cal, *PdBC_cal, *QdBC_cal, *PdCA_cal, *QdCA_cal, *aRa, *aRb, *aRc, *dRa, *dRb, *dRc, *IphaseAreal, *IphaseAimg, *IphaseBreal, *IphaseBimg, *IphaseCreal, *IphaseCimg, *IphaseABreal, *IphaseABimg, *IphaseBCreal, *IphaseBCimg, *IphaseCAreal, *IphaseCAimg, *Raa, *Rab, *Rac, *Rba, *Rbb, *Rbc, *Rca, *Rcb, *Rcc, *Xaa, *Xab, *Xac, *Xba, *Xbb, *Xbc, *Xca, *Xcb, *Xcc, *Nt, *PT_reg, *CT_reg, *BW_reg, *vol_reg_A, *vol_reg_B, *vol_reg_C;
	 
	Terminalbuses = (uint__t *)malloc(sizeof(uint__t)*(nv-1)); 
	Childbuses = (uint__t *)malloc(sizeof(uint__t)*(nv-1)); 
	Fatherbuses = (uint__t *)malloc(sizeof(uint__t)*(nv-1)); 
	tempfather = (uint__t *)malloc(sizeof(uint__t)*(nv-1)); 
	countf = (uint__t *)malloc(sizeof(uint__t)*(nv)); 
	countc = (uint__t *)malloc(sizeof(uint__t)*(nv)); 
    branchf = (uint__t *)malloc(sizeof(uint__t)*(nv));
	bustype = (uint__t *)malloc(sizeof(uint__t)*(nv));
	ID = (uint__t *)malloc(sizeof(uint__t)*(nv));
	IDF = (uint__t *)malloc(sizeof(uint__t)*(nv));
	IDT = (uint__t *)malloc(sizeof(uint__t)*(nv));
    VphaseAreal = (real__t *)malloc(sizeof(real__t)*(nv)); 
	VphaseAimg = (real__t *)malloc(sizeof(real__t)*(nv));
	VphaseBreal = (real__t *)malloc(sizeof(real__t)*(nv)); 
	VphaseBimg = (real__t *)malloc(sizeof(real__t)*(nv)); 
	VphaseCreal = (real__t *)malloc(sizeof(real__t)*(nv));
	VphaseCimg = (real__t *)malloc(sizeof(real__t)*(nv));
	VAmag = (real__t *)malloc(sizeof(real__t)*(nv)); 
	VBmag = (real__t *)malloc(sizeof(real__t)*(nv));
	VCmag = (real__t *)malloc(sizeof(real__t)*(nv)); 
	VAang = (real__t *)malloc(sizeof(real__t)*(nv)); 
	VBang = (real__t *)malloc(sizeof(real__t)*(nv));
	VCang = (real__t *)malloc(sizeof(real__t)*(nv)); 
	VABmag = (real__t *)malloc(sizeof(real__t)*(nv)); 
	VBCmag = (real__t *)malloc(sizeof(real__t)*(nv));
	VCAmag = (real__t *)malloc(sizeof(real__t)*(nv));
	VphaseABreal = (real__t *)malloc(sizeof(real__t)*(nv)); 
	VphaseABimg = (real__t *)malloc(sizeof(real__t)*(nv));
	VphaseBCreal = (real__t *)malloc(sizeof(real__t)*(nv)); 
	VphaseBCimg = (real__t *)malloc(sizeof(real__t)*(nv)); 
	VphaseCAreal = (real__t *)malloc(sizeof(real__t)*(nv));
	VphaseCAimg = (real__t *)malloc(sizeof(real__t)*(nv)); 
	Vbase = (real__t *)malloc(sizeof(real__t)*(nv)); 
	CapA = (real__t *)malloc(sizeof(real__t)*(nv)); 
	CapB = (real__t *)malloc(sizeof(real__t)*(nv));
	CapC = (real__t *)malloc(sizeof(real__t)*(nv)); 
	CapA_cal = (real__t *)malloc(sizeof(real__t)*(nv)); 
	CapB_cal = (real__t *)malloc(sizeof(real__t)*(nv));
	CapC_cal = (real__t *)malloc(sizeof(real__t)*(nv));
	PdA = (real__t *)malloc(sizeof(real__t)*(nv)); 
	QdA = (real__t *)malloc(sizeof(real__t)*(nv));
	PdB = (real__t *)malloc(sizeof(real__t)*(nv)); 
	QdB = (real__t *)malloc(sizeof(real__t)*(nv)); 
	PdC = (real__t *)malloc(sizeof(real__t)*(nv));
	QdC = (real__t *)malloc(sizeof(real__t)*(nv));
	PdA_cal = (real__t *)malloc(sizeof(real__t)*(nv)); 
	QdA_cal = (real__t *)malloc(sizeof(real__t)*(nv));
	PdB_cal = (real__t *)malloc(sizeof(real__t)*(nv)); 
	QdB_cal = (real__t *)malloc(sizeof(real__t)*(nv)); 
	PdC_cal = (real__t *)malloc(sizeof(real__t)*(nv));
	QdC_cal = (real__t *)malloc(sizeof(real__t)*(nv));
	PdAB_cal = (real__t *)malloc(sizeof(real__t)*(nv)); 
	QdAB_cal = (real__t *)malloc(sizeof(real__t)*(nv));
	PdBC_cal = (real__t *)malloc(sizeof(real__t)*(nv)); 
	QdBC_cal = (real__t *)malloc(sizeof(real__t)*(nv)); 
	PdCA_cal = (real__t *)malloc(sizeof(real__t)*(nv));
	QdCA_cal = (real__t *)malloc(sizeof(real__t)*(nv));
    TapA = (int *)malloc(sizeof(int)*(nv));
	TapB = (int *)malloc(sizeof(int)*(nv));
	TapC = (int *)malloc(sizeof(int)*(nv));
	LM = (uint__t *)malloc(sizeof(uint__t)*(nv));
    aRa = (real__t *)malloc(sizeof(real__t)*(nv));
	aRb = (real__t *)malloc(sizeof(real__t)*(nv));
	aRc = (real__t *)malloc(sizeof(real__t)*(nv));
	dRa = (real__t *)malloc(sizeof(real__t)*(nv));
	dRb = (real__t *)malloc(sizeof(real__t)*(nv));
	dRc = (real__t *)malloc(sizeof(real__t)*(nv));
	PT_reg = (real__t *)malloc(sizeof(real__t)*(nv));	
	CT_reg = (real__t *)malloc(sizeof(real__t)*(nv));
	BW_reg = (real__t *)malloc(sizeof(real__t)*(nv));
	vol_reg_A = (real__t *)malloc(sizeof(real__t)*(nv));
	vol_reg_B = (real__t *)malloc(sizeof(real__t)*(nv));
	vol_reg_C = (real__t *)malloc(sizeof(real__t)*(nv));
	control_phase_reg = (uint__t *)malloc(sizeof(uint__t)*(nv));
	IphaseAreal = (real__t *)malloc(sizeof(real__t)*(nv));
	IphaseAimg = (real__t *)malloc(sizeof(real__t)*(nv));
	IphaseBreal = (real__t *)malloc(sizeof(real__t)*(nv));
	IphaseBimg = (real__t *)malloc(sizeof(real__t)*(nv));
	IphaseCreal = (real__t *)malloc(sizeof(real__t)*(nv));
	IphaseCimg = (real__t *)malloc(sizeof(real__t)*(nv));
	IphaseABreal = (real__t *)malloc(sizeof(real__t)*(nv));
	IphaseABimg = (real__t *)malloc(sizeof(real__t)*(nv));
	IphaseBCreal = (real__t *)malloc(sizeof(real__t)*(nv));
	IphaseBCimg = (real__t *)malloc(sizeof(real__t)*(nv));
	IphaseCAreal = (real__t *)malloc(sizeof(real__t)*(nv));
	IphaseCAimg = (real__t *)malloc(sizeof(real__t)*(nv));
	edgetype = (uint__t *)malloc(sizeof(uint__t)*(ne));
    Links = (uint__t *)malloc(sizeof(uint__t)*(ne));
	Raa = (real__t *)malloc(sizeof(real__t)*(ne));
	Rab = (real__t *)malloc(sizeof(real__t)*(ne));
	Rac = (real__t *)malloc(sizeof(real__t)*(ne));
	Rba = (real__t *)malloc(sizeof(real__t)*(ne));
	Rbb = (real__t *)malloc(sizeof(real__t)*(ne));
	Rbc = (real__t *)malloc(sizeof(real__t)*(ne));
	Rca = (real__t *)malloc(sizeof(real__t)*(ne));
	Rcb = (real__t *)malloc(sizeof(real__t)*(ne));
	Rcc = (real__t *)malloc(sizeof(real__t)*(ne));
	Xaa = (real__t *)malloc(sizeof(real__t)*(ne));
	Xab = (real__t *)malloc(sizeof(real__t)*(ne));
	Xac = (real__t *)malloc(sizeof(real__t)*(ne));
	Xba = (real__t *)malloc(sizeof(real__t)*(ne));
	Xbb = (real__t *)malloc(sizeof(real__t)*(ne));
	Xbc = (real__t *)malloc(sizeof(real__t)*(ne));
	Xca = (real__t *)malloc(sizeof(real__t)*(ne));
	Xcb = (real__t *)malloc(sizeof(real__t)*(ne));
	Xcc = (real__t *)malloc(sizeof(real__t)*(ne));
	Nt = (real__t *)malloc(sizeof(real__t)*(ne));
	std::vector<std::vector<std::vector<double>>>areal(ne,vector<vector<double>>(3,vector<double>(3,0)));
	std::vector<std::vector<std::vector<double>>>aimg(ne,vector<vector<double>>(3,vector<double>(3,0)));
	std::vector<std::vector<std::vector<double>>>breal(ne,vector<vector<double>>(3,vector<double>(3,0)));
	std::vector<std::vector<std::vector<double>>>bimg(ne,vector<vector<double>>(3,vector<double>(3,0)));
	std::vector<std::vector<std::vector<double>>>creal(ne,vector<vector<double>>(3,vector<double>(3,0)));
	std::vector<std::vector<std::vector<double>>>cimg(ne,vector<vector<double>>(3,vector<double>(3,0)));
	std::vector<std::vector<std::vector<double>>>Areal(ne,vector<vector<double>>(3,vector<double>(3,0)));
	std::vector<std::vector<std::vector<double>>>Aimg(ne,vector<vector<double>>(3,vector<double>(3,0)));
	std::vector<std::vector<std::vector<double>>>Breal(ne,vector<vector<double>>(3,vector<double>(3,0)));
	std::vector<std::vector<std::vector<double>>>Bimg(ne,vector<vector<double>>(3,vector<double>(3,0)));
	std::vector<std::vector<std::vector<double>>>at(ne,vector<vector<double>>(3,vector<double>(3,0)));
    std::vector<std::vector<std::vector<double>>>btreal(ne,vector<vector<double>>(3,vector<double>(3,0)));
    std::vector<std::vector<std::vector<double>>>btimg(ne,vector<vector<double>>(3,vector<double>(3,0)));
	std::vector<std::vector<std::vector<double>>>dt(ne,vector<vector<double>>(3,vector<double>(3,0)));
    std::vector<std::vector<std::vector<double>>>At(ne,vector<vector<double>>(3,vector<double>(3,0)));
	
    // Initialization to zero
	memset(countf, 0, sizeof(uint__t)*(nv));
	memset(countc, 0, sizeof(uint__t)*(nv));
	memset(branchf, 0, sizeof(uint__t)*(nv));
	memset(bustype, 0, sizeof(uint__t)*(nv));
	memset(TapA, 0, sizeof(int)*(nv));
	memset(TapB, 0, sizeof(int)*(nv));
	memset(TapC, 0, sizeof(int)*(nv));
	memset(LM, 0, sizeof(uint__t)*(nv));
	memset(aRa, 1, sizeof(real__t)*(nv));
	memset(aRb, 1, sizeof(real__t)*(nv));
	memset(aRc, 1, sizeof(real__t)*(nv));
	memset(dRa, 1, sizeof(real__t)*(nv));
	memset(dRb, 1, sizeof(real__t)*(nv));
	memset(dRc, 1, sizeof(real__t)*(nv));
	memset(PT_reg, 0, sizeof(real__t)*(nv));
    memset(CT_reg, 0, sizeof(real__t)*(nv));
	memset(BW_reg, 0, sizeof(real__t)*(nv));
	memset(vol_reg_A, 0, sizeof(real__t)*(nv));
	memset(vol_reg_B, 0, sizeof(real__t)*(nv));
	memset(vol_reg_C, 0, sizeof(real__t)*(nv));
	memset(control_phase_reg, 0, sizeof(uint__t)*(nv));
	memset(VphaseAreal, 0, sizeof(real__t)*(nv));
	memset(VphaseAimg, 0, sizeof(real__t)*(nv));
	memset(VphaseBreal, 0, sizeof(real__t)*(nv));
	memset(VphaseBimg, 0, sizeof(real__t)*(nv));
	memset(VphaseCreal, 0, sizeof(real__t)*(nv));
	memset(VphaseCimg, 0, sizeof(real__t)*(nv));
	memset(IphaseAreal, 0, sizeof(real__t)*(nv));
	memset(IphaseAimg, 0, sizeof(real__t)*(nv));
	memset(IphaseBreal, 0, sizeof(real__t)*(nv));
	memset(IphaseBimg, 0, sizeof(real__t)*(nv));
	memset(IphaseCreal, 0, sizeof(real__t)*(nv));
	memset(IphaseCimg, 0, sizeof(real__t)*(nv));
	memset(PdA_cal, 0, sizeof(real__t)*(nv));
	memset(QdA_cal, 0, sizeof(real__t)*(nv));
	memset(PdB_cal, 0, sizeof(real__t)*(nv));
	memset(QdB_cal, 0, sizeof(real__t)*(nv));
	memset(PdC_cal, 0, sizeof(real__t)*(nv));
	memset(QdC_cal, 0, sizeof(real__t)*(nv));
	memset(PdAB_cal, 0, sizeof(real__t)*(nv));
	memset(QdAB_cal, 0, sizeof(real__t)*(nv));
	memset(PdBC_cal, 0, sizeof(real__t)*(nv));
	memset(QdBC_cal, 0, sizeof(real__t)*(nv));
	memset(PdCA_cal, 0, sizeof(real__t)*(nv));
	memset(QdCA_cal, 0, sizeof(real__t)*(nv));
    memset(Terminalbuses, 0, sizeof(uint__t)*(nv-1));
	memset(Childbuses, 0, sizeof(uint__t)*(nv-1));
	memset(Fatherbuses, 0, sizeof(uint__t)*(nv-1));
	memset(tempfather, 0, sizeof(uint__t)*(nv-1));
	memset(Links, 0, sizeof(uint__t)*(ne));
	memset(edgetype, 0, sizeof(uint__t)*(ne));
	
	Edge_D.sort_heap();  
	Vertex_D.sort_heap();
	
	
	for (i=0; i<nv; i++)
	{
		ID[i] = Vertex_D.data_[i].IDNO;
		Vbase[i] = Vertex_D.data_[i].Vbase;
		CapA[i] = Vertex_D.data_[i].CapA;
		CapB[i] = Vertex_D.data_[i].CapB;
		CapC[i] = Vertex_D.data_[i].CapC;
		PdA[i] = Vertex_D.data_[i].PdA;
		QdA[i] = Vertex_D.data_[i].QdA;
		PdB[i] = Vertex_D.data_[i].PdB;
		QdB[i] = Vertex_D.data_[i].QdB;
		PdC[i] = Vertex_D.data_[i].PdC;
		QdC[i] = Vertex_D.data_[i].QdC;
		TapA[i] = Vertex_D.data_[i].TapA;
		TapB[i] = Vertex_D.data_[i].TapB;
		TapC[i]	 = Vertex_D.data_[i].TapC;
		branchf[i] = Vertex_D.data_[i].branchf;
		bustype[i] = Vertex_D.data_[i].bustype;
        /* PT_reg[i] = Vertex_D.data_[i].PT_reg;
		CT_reg[i] = Vertex_D.data_[i].CT_reg;
		BW_reg[i] = Vertex_D.data_[i].BW_reg;
		vol_reg_A[i] = Vertex_D.data_[i].vol_reg_A;
		vol_reg_B[i] = Vertex_D.data_[i].vol_reg_B;
		vol_reg_C[i] = Vertex_D.data_[i].vol_reg_C; 
		if (strcmp(Vertex_D.data_[i].control_phase_reg.c_str(),"A")==0)
		{
			control_phase_reg[i] = 1;
		}
		else if (strcmp(Vertex_D.data_[i].control_phase_reg.c_str(),"B")==0)
		{
			control_phase_reg[i] = 2;
		}
		else if (strcmp(Vertex_D.data_[i].control_phase_reg.c_str(),"C")==0)
		{
			control_phase_reg[i] = 3;
		}*/
		if (strcmp(Vertex_D.data_[i].Load_Model.c_str(),"Y-PQ")==0)
		{
			LM[i] = 1;
		}
		else if (strcmp(Vertex_D.data_[i].Load_Model.c_str(),"Y-Z")==0)
		{
			LM[i] = 2;
		}
		else if (strcmp(Vertex_D.data_[i].Load_Model.c_str(),"Y-I")==0)
		{
			LM[i] = 3;
		}
		else if (strcmp(Vertex_D.data_[i].Load_Model.c_str(),"D-PQ")==0)
		{
			LM[i] = 4;
		}
		else if (strcmp(Vertex_D.data_[i].Load_Model.c_str(),"D-Z")==0)
		{
			LM[i] = 5;
		}else if (strcmp(Vertex_D.data_[i].Load_Model.c_str(),"D-I")==0)
		{
			LM[i] = 6;
		}
		else
		{
			LM[i] = 0;
		}
	}
	
	for (i=0; i<ne; i++)
	{
		IDF[i] = Edge_D.data_[i].IDF;
		IDT[i] = Edge_D.data_[i].IDT;
		Raa[i] = Edge_D.data_[i].Raa;
		Rab[i] = Edge_D.data_[i].Rab;
		Rac[i] = Edge_D.data_[i].Rac;
		Rba[i] = Edge_D.data_[i].Rba;
		Rbb[i] = Edge_D.data_[i].Rbb;
		Rbc[i] = Edge_D.data_[i].Rbc;
		Rca[i] = Edge_D.data_[i].Rca;
		Rcb[i] = Edge_D.data_[i].Rcb;
		Rcc[i] = Edge_D.data_[i].Rcc;
		Xaa[i] = Edge_D.data_[i].Xaa;
		Xab[i] = Edge_D.data_[i].Xab;
		Xac[i] = Edge_D.data_[i].Xac;
		Xba[i] = Edge_D.data_[i].Xba;
		Xbb[i] = Edge_D.data_[i].Xbb;
		Xbc[i] = Edge_D.data_[i].Xbc;
		Xca[i] = Edge_D.data_[i].Xca;
		Xcb[i] = Edge_D.data_[i].Xcb;
		Xcc[i] = Edge_D.data_[i].Xcc;
		Nt[i] = Edge_D.data_[i].Nt;
		edgetype[i] = Edge_D.data_[i].edgetype;
		if (edgetype[i] == 0)
		{
			//ignore the impact of B, get matrix for feeders
			areal[i][0][0] = 1; areal[i][0][1] = 0; areal[i][0][2] = 0;
			areal[i][1][0] = 0; areal[i][1][1] = 1; areal[i][1][2] = 0;
			areal[i][2][0] = 0; areal[i][2][1] = 0; areal[i][2][2] = 1;

			breal[i][0][0] = Raa[i]; breal[i][0][1] = Rab[i]; breal[i][0][2] = Rac[i];
			breal[i][1][0] = Rba[i]; breal[i][1][1] = Rbb[i]; breal[i][1][2] = Rbc[i];
			breal[i][2][0] = Rca[i]; breal[i][2][1] = Rcb[i]; breal[i][2][2] = Rcc[i];
			bimg[i][0][0] = Xaa[i]; bimg[i][0][1] = Xab[i]; bimg[i][0][2] = Xac[i];
			bimg[i][1][0] = Xba[i]; bimg[i][1][1] = Xbb[i]; bimg[i][1][2] = Xbc[i];
			bimg[i][2][0] = Xca[i]; bimg[i][2][1] = Xcb[i]; bimg[i][2][2] = Xcc[i];
			
			Areal[i][0][0] = 1; Areal[i][0][1] = 0; Areal[i][0][2] = 0;
			Areal[i][1][0] = 0; Areal[i][1][1] = 1; Areal[i][1][2] = 0;
			Areal[i][2][0] = 0; Areal[i][2][1] = 0; Areal[i][2][2] = 1;

			Breal[i][0][0] = Raa[i]; Breal[i][0][1] = Rab[i]; Breal[i][0][2] = Rac[i];
			Breal[i][1][0] = Rba[i]; Breal[i][1][1] = Rbb[i]; Breal[i][1][2] = Rbc[i];
			Breal[i][2][0] = Rca[i]; Breal[i][2][1] = Rcb[i]; Breal[i][2][2] = Rcc[i];
			Bimg[i][0][0] = Xaa[i]; Bimg[i][0][1] = Xab[i]; Bimg[i][0][2] = Xac[i];
			Bimg[i][1][0] = Xba[i]; Bimg[i][1][1] = Xbb[i]; Bimg[i][1][2] = Xbc[i];
			Bimg[i][2][0] = Xca[i]; Bimg[i][2][1] = Xcb[i]; Bimg[i][2][2] = Xcc[i];
			
/* 			printf("\n\n================================================================================================\n ");
			for (j=0;j<3;j++)
			{
				for (k=0;k<3;k++)
				{
					std::cout << "Fid: " << IDF[i]<<" Tid "<<IDT[i] <<" areal: "<<areal[i][j][k]<<" aimg "<<aimg[i][j][k]<<" breal: "<<breal[i][j][k]<<" bimg "<<bimg[i][j][k]<<" creal: "<<creal[i][j][k]<<" cimg "<<cimg[i][j][k] << std::endl;

				}
			}
			printf("\n================================================================================================ \n\n"); 
 */		}
		//get matrix for transformers,Delta-Delta 
		else if (edgetype[i] == 1)
		{
			at[i][0][0] = 2*Nt[i]/3; at[i][0][1] = -Nt[i]/3; at[i][0][2] = -Nt[i]/3;
			at[i][1][0] = -Nt[i]/3; at[i][1][1] = 2*Nt[i]/3; at[i][1][2] = -Nt[i]/3;
			at[i][2][0] = -Nt[i]/3; at[i][2][1] = -Nt[i]/3; at[i][2][2] = 2*Nt[i]/3;
			
			btreal[i][0][0] = Nt[i]*Raa[i]; btreal[i][0][1] = 0; btreal[i][0][2] = 0;
			btreal[i][1][0] = 0; btreal[i][1][1] = Nt[i]*Rbb[i]; btreal[i][1][2] = 0;
			btreal[i][2][0] = 0; btreal[i][2][1] = 0; btreal[i][2][2] = Nt[i]*Rcc[i];
			btimg[i][0][0] = Nt[i]*Xaa[i]; btimg[i][0][1] = 0; btimg[i][0][2] = 0;
			btimg[i][1][0] = 0; btimg[i][1][1] = Nt[i]*Xbb[i]; btimg[i][1][2] = 0;
			btimg[i][2][0] = 0; btimg[i][2][1] = 0; btimg[i][2][2] = Nt[i]*Xcc[i];
			
			dt[i][0][0] = 1/Nt[i]; dt[i][0][1] = 0; dt[i][0][2] = 0;
			dt[i][1][0] = 0; dt[i][1][1] = 1/Nt[i]; dt[i][1][2] = 0;
			dt[i][2][0] = 0; dt[i][2][1] = 0; dt[i][2][2] = 1/Nt[i];
			
			At[i][0][0] = 2/(3*Nt[i]); At[i][0][1] = -1/(3*Nt[i]); At[i][0][2] = -1/(3*Nt[i]);
			At[i][1][0] = -1/(3*Nt[i]); At[i][1][1] = 2/(3*Nt[i]); At[i][1][2] = -1/(3*Nt[i]);
			At[i][2][0] = -1/(3*Nt[i]); At[i][2][1] = -1/(3*Nt[i]); At[i][2][2] = 2/(3*Nt[i]);
		}
		
	}
		
//Find Terminal Buses by using the fact that a Terminal has no input bus
	k = 0;//use to store the number of child nodes in each step
	for (i=0; i<nv; i++)
	{
		if (branchf[i] == 0)
		{
			Terminalbuses[k] = ID[i]; //store the id of the terminal nodes.
			k +=1;
		}		
	}
	tn = k;//store the number of terminal nodes
	
	//std::cout << "SNVM : "<<SNVm<<" SNVsA: "<<SNVs<<" SNVsB: "<<SNVs - 2/3*pi_value<<" SNVsC: "<<SNVs + 2/3*pi_value<<std::endl;
	
// forward backward iteration
	for (iter=0; iter<Maxit; ++iter)
	{
		lv=0;
		if (iter == 0)//initialize the voltages at the terminal nodes
		{
			for (i=0; i<tn; i++)
			{
				j = Terminalbuses[i] - 1;//get indexes of terminal buses
				VphaseAreal[j] = SNVm*cos(SNVs);
				VphaseAimg[j] = SNVm*sin(SNVs);
				VphaseBreal[j] = SNVm*cos(SNVs - 2*pi_value/3);
				VphaseBimg[j] = SNVm*sin(SNVs - 2*pi_value/3);
				VphaseCreal[j] = SNVm*cos(SNVs + 2*pi_value/3);
				VphaseCimg[j] = SNVm*sin(SNVs + 2*pi_value/3);
           // std::cout << "VphaseAreal : "<<VphaseAreal[j]<<" VphaseAimg: "<<VphaseAimg[j]<< " VphaseBreal : "<<VphaseBreal[j]<<" VphaseBimg: "<<VphaseBimg[j]<< " VphaseCreal : "<<VphaseCreal[j]<<" VphaseCimg: "<<VphaseCimg[j]<<" pi "<<pi_value<<" PhaseB "<<2*pi_value/3<<" cos "<<cos(SNVs - 2*pi_value/3)<<std::endl;				
			}
		}
	/* printf("\n\n================================================================================================\n ");
 	for (i=0;i<nv;i++)
	{
		std::cout << "ID:" << i+1 <<" VAr: "<<VphaseAreal[i]<<" VAi "<<VphaseAimg[i]<<" VBr: "<<VphaseBreal[i]<<" VBi "<<VphaseBimg[i]<<" VCr: "<<VphaseCreal[i]<<" VCi "<<VphaseCimg[i] << std::endl;
	}
	std::cout << "Maxit : "<<Maxit <<" it "<<iter<< std::endl; 
	printf("\n================================================================================================ \n\n"); */
		// forward sweep, update currents
				
        while (countf[SNindex]!=branchf[SNindex] && lv<100)	
		{	
	        if (lv == 0) //select the terminal buses
		    {
				in = tn; //initial the number of child nodes at first selection
				for (i=0; i<in; i++)
				{
					Childbuses[i] = Terminalbuses[i];
					c = Terminalbuses[i] - 1;
					//calculate the currents at terminal nodes
					//get phase to ground and phase to phase voltage magnitude	
					VAmag[c] = sqrt(VphaseAreal[c]*VphaseAreal[c] + VphaseAimg[c]*VphaseAimg[c]);
					VBmag[c] = sqrt(VphaseBreal[c]*VphaseBreal[c] + VphaseBimg[c]*VphaseBimg[c]);
					VCmag[c] = sqrt(VphaseCreal[c]*VphaseCreal[c] + VphaseCimg[c]*VphaseCimg[c]);
					VphaseABreal[c] = VphaseAreal[c] - VphaseBreal[c];
					VphaseBCreal[c] = VphaseBreal[c] - VphaseCreal[c];
					VphaseCAreal[c] = VphaseCreal[c] - VphaseAreal[c];
					VphaseABimg[c] = VphaseAimg[c] - VphaseBimg[c];
					VphaseBCimg[c] = VphaseBimg[c] - VphaseCimg[c];
					VphaseCAimg[c] = VphaseCimg[c] - VphaseAimg[c];
					VABmag[c] = sqrt(VphaseABreal[c]*VphaseABreal[c] + VphaseABimg[c]*VphaseABimg[c]);
					VBCmag[c] = sqrt(VphaseBCreal[c]*VphaseBCreal[c] + VphaseBCimg[c]*VphaseBCimg[c]);
					VCAmag[c] = sqrt(VphaseCAreal[c]*VphaseCAreal[c] + VphaseCAimg[c]*VphaseCAimg[c]);
					//get actual capacitor reactive power 
					CapA_cal[c] = CapA[c]*VAmag[c]*VAmag[c];
					CapB_cal[c] = CapB[c]*VBmag[c]*VBmag[c];
					CapC_cal[c] = CapC[c]*VCmag[c]*VCmag[c];
					//get actual load based on voltages
					
					if (LM[c] == 1)
					{
						PdA_cal[c] = PdA[c];
						QdA_cal[c] = QdA[c];
						PdB_cal[c] = PdB[c];
						QdB_cal[c] = QdB[c];
						PdC_cal[c] = PdC[c];
						QdC_cal[c] = QdC[c];
						IphaseAreal[c] = (PdA_cal[c]*VphaseAreal[c] + (QdA_cal[c]-CapA_cal[c])*VphaseAimg[c])/(VphaseAreal[c]*VphaseAreal[c] + VphaseAimg[c]*VphaseAimg[c]);
						IphaseBreal[c] = (PdB_cal[c]*VphaseBreal[c] + (QdB_cal[c]-CapB_cal[c])*VphaseBimg[c])/(VphaseBreal[c]*VphaseBreal[c] + VphaseBimg[c]*VphaseBimg[c]);
						IphaseCreal[c] = (PdC_cal[c]*VphaseCreal[c] + (QdC_cal[c]-CapC_cal[c])*VphaseCimg[c])/(VphaseCreal[c]*VphaseCreal[c] + VphaseCimg[c]*VphaseCimg[c]);
						IphaseAimg[c] = (PdA_cal[c]*VphaseAimg[c]-(QdA_cal[c]-CapA_cal[c])*VphaseAreal[c])/(VphaseAreal[c]*VphaseAreal[c] + VphaseAimg[c]*VphaseAimg[c]);
						IphaseBimg[c] = (PdB_cal[c]*VphaseBimg[c]-(QdB_cal[c]-CapB_cal[c])*VphaseBreal[c])/(VphaseBreal[c]*VphaseBreal[c] + VphaseBimg[c]*VphaseBimg[c]);
						IphaseCimg[c] = (PdC_cal[c]*VphaseCimg[c]-(QdC_cal[c]-CapC_cal[c])*VphaseCreal[c])/(VphaseCreal[c]*VphaseCreal[c] + VphaseCimg[c]*VphaseCimg[c]);
						
					}
					else if (LM[c] == 2)
					{
						PdA_cal[c] = PdA[c]*VAmag[c]*VAmag[c];
						QdA_cal[c] = QdA[c]*VAmag[c]*VAmag[c];
						PdB_cal[c] = PdB[c]*VBmag[c]*VBmag[c];
						QdB_cal[c] = QdB[c]*VBmag[c]*VBmag[c];
						PdC_cal[c] = PdC[c]*VCmag[c]*VCmag[c];
						QdC_cal[c] = QdC[c]*VCmag[c]*VCmag[c];
						IphaseAreal[c] = (PdA_cal[c]*VphaseAreal[c] + (QdA_cal[c]-CapA_cal[c])*VphaseAimg[c])/(VphaseAreal[c]*VphaseAreal[c] + VphaseAimg[c]*VphaseAimg[c]);
						IphaseBreal[c] = (PdB_cal[c]*VphaseBreal[c] + (QdB_cal[c]-CapB_cal[c])*VphaseBimg[c])/(VphaseBreal[c]*VphaseBreal[c] + VphaseBimg[c]*VphaseBimg[c]);
						IphaseCreal[c] = (PdC_cal[c]*VphaseCreal[c] + (QdC_cal[c]-CapC_cal[c])*VphaseCimg[c])/(VphaseCreal[c]*VphaseCreal[c] + VphaseCimg[c]*VphaseCimg[c]);
						IphaseAimg[c] = (PdA_cal[c]*VphaseAimg[c]-(QdA_cal[c]-CapA_cal[c])*VphaseAreal[c])/(VphaseAreal[c]*VphaseAreal[c] + VphaseAimg[c]*VphaseAimg[c]);
						IphaseBimg[c] = (PdB_cal[c]*VphaseBimg[c]-(QdB_cal[c]-CapB_cal[c])*VphaseBreal[c])/(VphaseBreal[c]*VphaseBreal[c] + VphaseBimg[c]*VphaseBimg[c]);
						IphaseCimg[c] = (PdC_cal[c]*VphaseCimg[c]-(QdC_cal[c]-CapC_cal[c])*VphaseCreal[c])/(VphaseCreal[c]*VphaseCreal[c] + VphaseCimg[c]*VphaseCimg[c]);
					}
					else if (LM[c] == 3)
					{
						PdA_cal[c] = PdA[c]*VAmag[c];
						QdA_cal[c] = QdA[c]*VAmag[c];
						PdB_cal[c] = PdB[c]*VBmag[c];
						QdB_cal[c] = QdB[c]*VBmag[c];
						PdC_cal[c] = PdC[c]*VCmag[c];
						QdC_cal[c] = QdC[c]*VCmag[c];
						IphaseAreal[c] = (PdA_cal[c]*VphaseAreal[c] + (QdA_cal[c]-CapA_cal[c])*VphaseAimg[c])/(VphaseAreal[c]*VphaseAreal[c] + VphaseAimg[c]*VphaseAimg[c]);
						IphaseBreal[c] = (PdB_cal[c]*VphaseBreal[c] + (QdB_cal[c]-CapB_cal[c])*VphaseBimg[c])/(VphaseBreal[c]*VphaseBreal[c] + VphaseBimg[c]*VphaseBimg[c]);
						IphaseCreal[c] = (PdC_cal[c]*VphaseCreal[c] + (QdC_cal[c]-CapC_cal[c])*VphaseCimg[c])/(VphaseCreal[c]*VphaseCreal[c] + VphaseCimg[c]*VphaseCimg[c]);
						IphaseAimg[c] = (PdA_cal[c]*VphaseAimg[c]-(QdA_cal[c]-CapA_cal[c])*VphaseAreal[c])/(VphaseAreal[c]*VphaseAreal[c] + VphaseAimg[c]*VphaseAimg[c]);
						IphaseBimg[c] = (PdB_cal[c]*VphaseBimg[c]-(QdB_cal[c]-CapB_cal[c])*VphaseBreal[c])/(VphaseBreal[c]*VphaseBreal[c] + VphaseBimg[c]*VphaseBimg[c]);
						IphaseCimg[c] = (PdC_cal[c]*VphaseCimg[c]-(QdC_cal[c]-CapC_cal[c])*VphaseCreal[c])/(VphaseCreal[c]*VphaseCreal[c] + VphaseCimg[c]*VphaseCimg[c]);
					}
					else if (LM[c] == 4)
					{
						PdAB_cal[c] = PdA[c];
						QdAB_cal[c] = QdA[c];
						PdBC_cal[c] = PdB[c];
						QdBC_cal[c] = QdB[c];
						PdCA_cal[c] = PdC[c];
						QdCA_cal[c] = QdC[c];
						IphaseABreal[c] = (PdAB_cal[c]*VphaseABreal[c] + QdAB_cal[c]*VphaseABimg[c])/(VphaseABreal[c]*VphaseABreal[c] + VphaseABimg[c]*VphaseABimg[c]);
						IphaseABimg[c] = (PdAB_cal[c]*VphaseABimg[c] - QdAB_cal[c]*VphaseABreal[c])/(VphaseABreal[c]*VphaseABreal[c] + VphaseABimg[c]*VphaseABimg[c]);
						IphaseBCreal[c] = (PdBC_cal[c]*VphaseBCreal[c] + QdBC_cal[c]*VphaseBCimg[c])/(VphaseBCreal[c]*VphaseBCreal[c] + VphaseBCimg[c]*VphaseBCimg[c]);
						IphaseBCimg[c] = (PdBC_cal[c]*VphaseBCimg[c] - QdBC_cal[c]*VphaseBCreal[c])/(VphaseBCreal[c]*VphaseBCreal[c] + VphaseBCimg[c]*VphaseBCimg[c]);
						IphaseCAreal[c] = (PdCA_cal[c]*VphaseCAreal[c] + QdCA_cal[c]*VphaseCAimg[c])/(VphaseCAreal[c]*VphaseCAreal[c] + VphaseCAimg[c]*VphaseCAimg[c]);
						IphaseCAimg[c] = (PdCA_cal[c]*VphaseCAimg[c] - QdCA_cal[c]*VphaseCAreal[c])/(VphaseCAreal[c]*VphaseCAreal[c] + VphaseCAimg[c]*VphaseCAimg[c]);
						IphaseAreal[c] = IphaseABreal[c]-IphaseCAreal[c]-CapA_cal[c]*VphaseAimg[c]/(VphaseAreal[c]*VphaseAreal[c] + VphaseAimg[c]*VphaseAimg[c]);
						IphaseAimg[c] = IphaseABimg[c]-IphaseCAimg[c]+CapA_cal[c]*VphaseAreal[c]/(VphaseAreal[c]*VphaseAreal[c] + VphaseAimg[c]*VphaseAimg[c]);
						IphaseBreal[c] = IphaseBCreal[c]-IphaseABreal[c]-CapB_cal[c]*VphaseBimg[c]/(VphaseBreal[c]*VphaseBreal[c] + VphaseBimg[c]*VphaseBimg[c]);
						IphaseBimg[c] = IphaseBCimg[c]-IphaseABimg[c]+CapB_cal[c]*VphaseBreal[c]/(VphaseBreal[c]*VphaseBreal[c] + VphaseBimg[c]*VphaseBimg[c]);
						IphaseCreal[c] = IphaseCAreal[c]-IphaseBCreal[c]-CapC_cal[c]*VphaseCimg[c]/(VphaseCreal[c]*VphaseCreal[c] + VphaseCimg[c]*VphaseCimg[c]);
						IphaseCimg[c] = IphaseCAimg[c]-IphaseBCimg[c]+CapC_cal[c]*VphaseCreal[c]/(VphaseCreal[c]*VphaseCreal[c] + VphaseCimg[c]*VphaseCimg[c]);
					}
					else if (LM[c] == 5)
					{
						PdAB_cal[c] = PdA[c]*VABmag[c]*VABmag[c]/3;
						QdAB_cal[c] = QdA[c]*VABmag[c]*VABmag[c]/3;
						PdBC_cal[c] = PdB[c]*VBCmag[c]*VBCmag[c]/3;
						QdBC_cal[c] = QdB[c]*VBCmag[c]*VBCmag[c]/3;
						PdCA_cal[c] = PdC[c]*VCAmag[c]*VCAmag[c]/3;
						QdCA_cal[c] = QdC[c]*VCAmag[c]*VCAmag[c]/3;;
						IphaseABreal[c] = (PdAB_cal[c]*VphaseABreal[c] + QdAB_cal[c]*VphaseABimg[c])/(VphaseABreal[c]*VphaseABreal[c] + VphaseABimg[c]*VphaseABimg[c]);
						IphaseABimg[c] = (PdAB_cal[c]*VphaseABimg[c] - QdAB_cal[c]*VphaseABreal[c])/(VphaseABreal[c]*VphaseABreal[c] + VphaseABimg[c]*VphaseABimg[c]);
						IphaseBCreal[c] = (PdBC_cal[c]*VphaseBCreal[c] + QdBC_cal[c]*VphaseBCimg[c])/(VphaseBCreal[c]*VphaseBCreal[c] + VphaseBCimg[c]*VphaseBCimg[c]);
						IphaseBCimg[c] = (PdBC_cal[c]*VphaseBCimg[c] - QdBC_cal[c]*VphaseBCreal[c])/(VphaseBCreal[c]*VphaseBCreal[c] + VphaseBCimg[c]*VphaseBCimg[c]);
						IphaseCAreal[c] = (PdCA_cal[c]*VphaseCAreal[c] + QdCA_cal[c]*VphaseCAimg[c])/(VphaseCAreal[c]*VphaseCAreal[c] + VphaseCAimg[c]*VphaseCAimg[c]);
						IphaseCAimg[c] = (PdCA_cal[c]*VphaseCAimg[c] - QdCA_cal[c]*VphaseCAreal[c])/(VphaseCAreal[c]*VphaseCAreal[c] + VphaseCAimg[c]*VphaseCAimg[c]);
						IphaseAreal[c] = IphaseABreal[c]-IphaseCAreal[c]-CapA_cal[c]*VphaseAimg[c]/(VphaseAreal[c]*VphaseAreal[c] + VphaseAimg[c]*VphaseAimg[c]);
						IphaseAimg[c] = IphaseABimg[c]-IphaseCAimg[c]+CapA_cal[c]*VphaseAreal[c]/(VphaseAreal[c]*VphaseAreal[c] + VphaseAimg[c]*VphaseAimg[c]);
						IphaseBreal[c] = IphaseBCreal[c]-IphaseABreal[c]-CapB_cal[c]*VphaseBimg[c]/(VphaseBreal[c]*VphaseBreal[c] + VphaseBimg[c]*VphaseBimg[c]);
						IphaseBimg[c] = IphaseBCimg[c]-IphaseABimg[c]+CapB_cal[c]*VphaseBreal[c]/(VphaseBreal[c]*VphaseBreal[c] + VphaseBimg[c]*VphaseBimg[c]);
						IphaseCreal[c] = IphaseCAreal[c]-IphaseBCreal[c]-CapC_cal[c]*VphaseCimg[c]/(VphaseCreal[c]*VphaseCreal[c] + VphaseCimg[c]*VphaseCimg[c]);
						IphaseCimg[c] = IphaseCAimg[c]-IphaseBCimg[c]+CapC_cal[c]*VphaseCreal[c]/(VphaseCreal[c]*VphaseCreal[c] + VphaseCimg[c]*VphaseCimg[c]);
					}
					else if (LM[c] == 6)
					{
						PdAB_cal[c] = PdA[c]*(VABmag[c]/sqrt(3));
						QdAB_cal[c] = QdA[c]*(VABmag[c]/sqrt(3));
						PdBC_cal[c] = PdB[c]*(VBCmag[c]/sqrt(3));
						QdBC_cal[c] = QdB[c]*(VBCmag[c]/sqrt(3));
						PdCA_cal[c] = PdC[c]*(VCAmag[c]/sqrt(3));
						QdCA_cal[c] = QdC[c]*(VCAmag[c]/sqrt(3));
						IphaseABreal[c] = (PdAB_cal[c]*VphaseABreal[c] + QdAB_cal[c]*VphaseABimg[c])/(VphaseABreal[c]*VphaseABreal[c] + VphaseABimg[c]*VphaseABimg[c]);
						IphaseABimg[c] = (PdAB_cal[c]*VphaseABimg[c] - QdAB_cal[c]*VphaseABreal[c])/(VphaseABreal[c]*VphaseABreal[c] + VphaseABimg[c]*VphaseABimg[c]);
						IphaseBCreal[c] = (PdBC_cal[c]*VphaseBCreal[c] + QdBC_cal[c]*VphaseBCimg[c])/(VphaseBCreal[c]*VphaseBCreal[c] + VphaseBCimg[c]*VphaseBCimg[c]);
						IphaseBCimg[c] = (PdBC_cal[c]*VphaseBCimg[c] - QdBC_cal[c]*VphaseBCreal[c])/(VphaseBCreal[c]*VphaseBCreal[c] + VphaseBCimg[c]*VphaseBCimg[c]);
						IphaseCAreal[c] = (PdCA_cal[c]*VphaseCAreal[c] + QdCA_cal[c]*VphaseCAimg[c])/(VphaseCAreal[c]*VphaseCAreal[c] + VphaseCAimg[c]*VphaseCAimg[c]);
						IphaseCAimg[c] = (PdCA_cal[c]*VphaseCAimg[c] - QdCA_cal[c]*VphaseCAreal[c])/(VphaseCAreal[c]*VphaseCAreal[c] + VphaseCAimg[c]*VphaseCAimg[c]);
						IphaseAreal[c] = IphaseABreal[c]-IphaseCAreal[c]-CapA_cal[c]*VphaseAimg[c]/(VphaseAreal[c]*VphaseAreal[c] + VphaseAimg[c]*VphaseAimg[c]);
						IphaseAimg[c] = IphaseABimg[c]-IphaseCAimg[c]+CapA_cal[c]*VphaseAreal[c]/(VphaseAreal[c]*VphaseAreal[c] + VphaseAimg[c]*VphaseAimg[c]);
						IphaseBreal[c] = IphaseBCreal[c]-IphaseABreal[c]-CapB_cal[c]*VphaseBimg[c]/(VphaseBreal[c]*VphaseBreal[c] + VphaseBimg[c]*VphaseBimg[c]);
						IphaseBimg[c] = IphaseBCimg[c]-IphaseABimg[c]+CapB_cal[c]*VphaseBreal[c]/(VphaseBreal[c]*VphaseBreal[c] + VphaseBimg[c]*VphaseBimg[c]);
						IphaseCreal[c] = IphaseCAreal[c]-IphaseBCreal[c]-CapC_cal[c]*VphaseCimg[c]/(VphaseCreal[c]*VphaseCreal[c] + VphaseCimg[c]*VphaseCimg[c]);
						IphaseCimg[c] = IphaseCAimg[c]-IphaseBCimg[c]+CapC_cal[c]*VphaseCreal[c]/(VphaseCreal[c]*VphaseCreal[c] + VphaseCimg[c]*VphaseCimg[c]);
					}
						
					//std::cout << "cid : "<<c+1<<" PdA "<<PdA[c]<<" QdA "<<QdA[c]<<" PdB "<<PdB[c]<<" QdB "<<QdB[c]<<" PdC "<<PdC[c]<<" QdC "<<QdC[c]<<" VAmag "<<VAmag[c]<<" VBmag "<<VBmag[c]<<" VCmag "<<VCmag[c]<<" Vbase "<<Vbase[c]<<" PdA_cal "<<PdA_cal[c]<<" QdA_cal "<<QdA_cal[c]<<" PdB_cal "<<PdB_cal[c]<<" QdB_cal "<<QdB_cal[c]<<" PdC_cal "<<PdC_cal[c]<<" QdC_cal "<<QdC_cal[c]<<" loadmodel "<<LM[c]<<" "<<Vertex_D.data_[c].Load_Model.c_str()<<std::endl; 

				}
			}
			for (i=0; i<in; i++)
	        {
		        for (j=0; j<ne; j++)
		        {   
	                if(IDT[j] == Childbuses[i])
		            {
			            Links[i] = j; //store the link index between the child nodes and father nodes.
						Fatherbuses[i] = IDF[j]; //store the id of father nodes.
						countf[IDF[j]-1] += 1; //Array_id = bus_id -1
		            }
		        }
	        }

			for (i=0; i<in; i++)//calculation at the links between the child nodes and father nodes
			{
				j = Links[i]; //selected link index
				f = IDF[j]-1; //selected father node index
				c = IDT[j]-1; //selected child node index
				
				//std::cout << "fid : "<<f+1<<" cid "<<c+1<<" edgeid "<<j+1<<" edgetype "<<edgetype[j]<<std::endl; 
				
				if (edgetype[j]==0) //if the selected link is a feeder
				{
					// calculate real part of voltage
					VphaseAreal[f] = VphaseAreal[c]*areal[j][0][0] + VphaseBreal[c]*areal[j][0][1] + VphaseCreal[c]*areal[j][0][2] - VphaseAimg[c]*aimg[j][0][0] - VphaseBimg[c]*aimg[j][0][1] - VphaseCimg[c]*aimg[j][0][2] + IphaseAreal[c]*breal[j][0][0] + IphaseBreal[c]*breal[j][0][1] + IphaseCreal[c]*breal[j][0][2] -IphaseAimg[c]*bimg[j][0][0] -IphaseBimg[c]*bimg[j][0][1] - IphaseCimg[c]*bimg[j][0][2];
					VphaseBreal[f] = VphaseAreal[c]*areal[j][1][0] + VphaseBreal[c]*areal[j][1][1] + VphaseCreal[c]*areal[j][1][2] - VphaseAimg[c]*aimg[j][1][0] - VphaseBimg[c]*aimg[j][1][1] - VphaseCimg[c]*aimg[j][1][2] + IphaseAreal[c]*breal[j][1][0] + IphaseBreal[c]*breal[j][1][1] + IphaseCreal[c]*breal[j][1][2] -IphaseAimg[c]*bimg[j][1][0] -IphaseBimg[c]*bimg[j][1][1] - IphaseCimg[c]*bimg[j][1][2];
					VphaseCreal[f] = VphaseAreal[c]*areal[j][2][0] + VphaseBreal[c]*areal[j][2][1] + VphaseCreal[c]*areal[j][2][2] - VphaseAimg[c]*aimg[j][2][0] - VphaseBimg[c]*aimg[j][2][1] - VphaseCimg[c]*aimg[j][2][2]+ IphaseAreal[c]*breal[j][2][0] + IphaseBreal[c]*breal[j][2][1] + IphaseCreal[c]*breal[j][2][2] -IphaseAimg[c]*bimg[j][2][0] -IphaseBimg[c]*bimg[j][2][1] - IphaseCimg[c]*bimg[j][2][2];
				    // calculate imaginary part of voltage
					VphaseAimg[f] = VphaseAreal[c]*aimg[j][0][0] + VphaseBreal[c]*aimg[j][0][1] + VphaseCreal[c]*aimg[j][0][2] + VphaseAimg[c]*areal[j][0][0] + VphaseBimg[c]*areal[j][0][1] + VphaseCimg[c]*areal[j][0][2] + IphaseAreal[c]*bimg[j][0][0] + IphaseBreal[c]*bimg[j][0][1] + IphaseCreal[c]*bimg[j][0][2] + IphaseAimg[c]*breal[j][0][0] + IphaseBimg[c]*breal[j][0][1] + IphaseCimg[c]*breal[j][0][2];
					VphaseBimg[f] = VphaseAreal[c]*aimg[j][1][0] + VphaseBreal[c]*aimg[j][1][1] + VphaseCreal[c]*aimg[j][1][2] + VphaseAimg[c]*areal[j][1][0] + VphaseBimg[c]*areal[j][1][1] + VphaseCimg[c]*areal[j][1][2] + IphaseAreal[c]*bimg[j][1][0] + IphaseBreal[c]*bimg[j][1][1] + IphaseCreal[c]*bimg[j][1][2] + IphaseAimg[c]*breal[j][1][0] + IphaseBimg[c]*breal[j][1][1] + IphaseCimg[c]*breal[j][1][2];
					VphaseCimg[f] = VphaseAreal[c]*aimg[j][2][0] + VphaseBreal[c]*aimg[j][2][1] + VphaseCreal[c]*aimg[j][2][2] + VphaseAimg[c]*areal[j][2][0] + VphaseBimg[c]*areal[j][2][1] + VphaseCimg[c]*areal[j][2][2] + IphaseAreal[c]*bimg[j][2][0] + IphaseBreal[c]*bimg[j][2][1] + IphaseCreal[c]*bimg[j][2][2] + IphaseAimg[c]*breal[j][2][0] + IphaseBimg[c]*breal[j][2][1] + IphaseCimg[c]*breal[j][2][2];
				    // calculate real part of current
					IphaseAreal[f] += VphaseAreal[c]*creal[j][0][0] + VphaseBreal[c]*creal[j][0][1] + VphaseCreal[c]*creal[j][0][2] - VphaseAimg[c]*cimg[j][0][0] - VphaseBimg[c]*cimg[j][0][1] - VphaseCimg[c]*cimg[j][0][2] + IphaseAreal[c]*areal[j][0][0] + IphaseBreal[c]*areal[j][0][1] + IphaseCreal[c]*areal[j][0][2] -IphaseAimg[c]*aimg[j][0][0] -IphaseBimg[c]*aimg[j][0][1] - IphaseCimg[c]*aimg[j][0][2];
					IphaseBreal[f] += VphaseAreal[c]*creal[j][1][0] + VphaseBreal[c]*creal[j][1][1] + VphaseCreal[c]*creal[j][1][2] - VphaseAimg[c]*cimg[j][1][0] - VphaseBimg[c]*cimg[j][1][1] - VphaseCimg[c]*cimg[j][1][2]  + IphaseAreal[c]*areal[j][1][0] + IphaseBreal[c]*areal[j][1][1] + IphaseCreal[c]*areal[j][1][2] -IphaseAimg[c]*aimg[j][1][0] -IphaseBimg[c]*aimg[j][1][1] - IphaseCimg[c]*aimg[j][1][2];
					IphaseCreal[f] += VphaseAreal[c]*creal[j][2][0] + VphaseBreal[c]*creal[j][2][1] + VphaseCreal[c]*creal[j][2][2] - VphaseAimg[c]*cimg[j][2][0] - VphaseBimg[c]*cimg[j][2][1] - VphaseCimg[c]*cimg[j][2][2] + IphaseAreal[c]*areal[j][2][0] + IphaseBreal[c]*areal[j][2][1] + IphaseCreal[c]*areal[j][2][2] -IphaseAimg[c]*aimg[j][2][0] -IphaseBimg[c]*aimg[j][2][1] - IphaseCimg[c]*aimg[j][2][2];
				    // calculate imaginary part of current
					IphaseAimg[f] += VphaseAreal[c]*cimg[j][0][0] + VphaseBreal[c]*cimg[j][0][1] + VphaseCreal[c]*cimg[j][0][2] + VphaseAimg[c]*creal[j][0][0] + VphaseBimg[c]*creal[j][0][1] + VphaseCimg[c]*creal[j][0][2] + IphaseAreal[c]*aimg[j][0][0] + IphaseBreal[c]*aimg[j][0][1] + IphaseCreal[c]*aimg[j][0][2] + IphaseAimg[c]*areal[j][0][0] + IphaseBimg[c]*areal[j][0][1] + IphaseCimg[c]*areal[j][0][2];
					IphaseBimg[f] += VphaseAreal[c]*cimg[j][1][0] + VphaseBreal[c]*cimg[j][1][1] + VphaseCreal[c]*cimg[j][1][2] + VphaseAimg[c]*creal[j][1][0] + VphaseBimg[c]*creal[j][1][1] + VphaseCimg[c]*creal[j][1][2] + IphaseAreal[c]*aimg[j][1][0] + IphaseBreal[c]*aimg[j][1][1] + IphaseCreal[c]*aimg[j][1][2] + IphaseAimg[c]*areal[j][1][0] + IphaseBimg[c]*areal[j][1][1] + IphaseCimg[c]*areal[j][1][2];
					IphaseCimg[f] += VphaseAreal[c]*cimg[j][2][0] + VphaseBreal[c]*cimg[j][2][1] + VphaseCreal[c]*cimg[j][2][2] + VphaseAimg[c]*creal[j][2][0] + VphaseBimg[c]*creal[j][2][1] + VphaseCimg[c]*creal[j][2][2] + IphaseAreal[c]*aimg[j][2][0] + IphaseBreal[c]*aimg[j][2][1] + IphaseCreal[c]*aimg[j][2][2] + IphaseAimg[c]*areal[j][2][0] + IphaseBimg[c]*areal[j][2][1] + IphaseCimg[c]*areal[j][2][2];
					//add load currents at the father nodes
				}
				else if (edgetype[j]==1) //if the selected link is a transformer
				{
					// calculate real part of voltage
					VphaseAreal[f] = VphaseAreal[c]*at[j][0][0] + VphaseBreal[c]*at[j][0][1] + VphaseCreal[c]*at[j][0][2] + IphaseAreal[c]*btreal[j][0][0] + IphaseBreal[c]*btreal[j][0][1] + IphaseCreal[c]*btreal[j][0][2] - IphaseAimg[c]*btimg[j][0][0] - IphaseBimg[c]*btimg[j][0][1] - IphaseCimg[c]*btimg[j][0][2];
					VphaseBreal[f] = VphaseAreal[c]*at[j][1][0] + VphaseBreal[c]*at[j][1][1] + VphaseCreal[c]*at[j][1][2] + IphaseAreal[c]*btreal[j][1][0] + IphaseBreal[c]*btreal[j][1][1] + IphaseCreal[c]*btreal[j][1][2] - IphaseAimg[c]*btimg[j][1][0] - IphaseBimg[c]*btimg[j][1][1] - IphaseCimg[c]*btimg[j][1][2];
					VphaseCreal[f] = VphaseAreal[c]*at[j][2][0] + VphaseBreal[c]*at[j][2][1] + VphaseCreal[c]*at[j][2][2] + IphaseAreal[c]*btreal[j][2][0] + IphaseBreal[c]*btreal[j][2][1] + IphaseCreal[c]*btreal[j][2][2] - IphaseAimg[c]*btimg[j][2][0] - IphaseBimg[c]*btimg[j][2][1] - IphaseCimg[c]*btimg[j][2][2];
				    // calculate imaginary part of voltage
					VphaseAimg[f] = VphaseAimg[c]*at[j][0][0] + VphaseBimg[c]*at[j][0][1] + VphaseCimg[c]*at[j][0][2] + IphaseAreal[c]*btimg[j][0][0] + IphaseBreal[c]*btimg[j][0][1] + IphaseCreal[c]*btimg[j][0][2] + IphaseAimg[c]*btreal[j][0][0] + IphaseBimg[c]*btreal[j][0][1] + IphaseCimg[c]*btreal[j][0][2];
					VphaseBimg[f] = VphaseAimg[c]*at[j][1][0] + VphaseBimg[c]*at[j][1][1] + VphaseCimg[c]*at[j][1][2] + IphaseAreal[c]*btimg[j][1][0] + IphaseBreal[c]*btimg[j][1][1] + IphaseCreal[c]*btimg[j][1][2] + IphaseAimg[c]*btreal[j][1][0] + IphaseBimg[c]*btreal[j][1][1] + IphaseCimg[c]*btreal[j][1][2];
					VphaseCimg[f] = VphaseAimg[c]*at[j][2][0] + VphaseBimg[c]*at[j][2][1] + VphaseCimg[c]*at[j][2][2] + IphaseAreal[c]*btimg[j][2][0] + IphaseBreal[c]*btimg[j][2][1] + IphaseCreal[c]*btimg[j][2][2] + IphaseAimg[c]*btreal[j][2][0] + IphaseBimg[c]*btreal[j][2][1] + IphaseCimg[c]*btreal[j][2][2];
				    // calculate real part of current
					IphaseAreal[f] += IphaseAreal[c]*dt[j][0][0] + IphaseBreal[c]*dt[j][0][1] + IphaseCreal[c]*dt[j][0][2];
					IphaseBreal[f] += IphaseAreal[c]*dt[j][1][0] + IphaseBreal[c]*dt[j][1][1] + IphaseCreal[c]*dt[j][1][2];
					IphaseCreal[f] += IphaseAreal[c]*dt[j][2][0] + IphaseBreal[c]*dt[j][2][1] + IphaseCreal[c]*dt[j][2][2];		
			     	// calculate imaginary part of current
					IphaseAimg[f] += IphaseAimg[c]*dt[j][0][0] + IphaseBimg[c]*dt[j][0][1] + IphaseCimg[c]*dt[j][0][2];
					IphaseBimg[f] += IphaseAimg[c]*dt[j][1][0] + IphaseBimg[c]*dt[j][1][1] + IphaseCimg[c]*dt[j][1][2];
					IphaseCimg[f] += IphaseAimg[c]*dt[j][2][0] + IphaseBimg[c]*dt[j][2][1] + IphaseCimg[c]*dt[j][2][2];
				}
				else if (edgetype[j]==2) //if the selected link is a regulator
				{
					aRa[c] = 1 + 0.00625*TapA[c];
					aRb[c] = 1 + 0.00625*TapB[c];
					aRc[c] = 1 + 0.00625*TapC[c];
					dRa[c] = 1/aRa[c];
					dRb[c] = 1/aRb[c];
					dRc[c] = 1/aRc[c];
				    //calculate the father node voltage
					VphaseAreal[f] = VphaseAreal[c]*dRa[c];
					VphaseAimg[f] = VphaseAimg[c]*dRa[c];
					VphaseBreal[f] = VphaseBreal[c]*dRb[c];
					VphaseBimg[f] = VphaseBimg[c]*dRb[c];
					VphaseCreal[f] = VphaseCreal[c]*dRc[c];
					VphaseCimg[f] = VphaseCimg[c]*dRc[c];
				    //calculate the father node current
					IphaseAreal[f] += IphaseAreal[c]*aRa[c];
					IphaseAimg[f] += IphaseAimg[c]*aRa[c];
					IphaseBreal[f] += IphaseBreal[c]*aRb[c];
					IphaseBimg[f] += IphaseBimg[c]*aRb[c];
					IphaseCreal[f] += IphaseCreal[c]*aRc[c];
					IphaseCimg[f] += IphaseCimg[c]*aRc[c];
				}
			}	
			//update the child nodes, set the father nodes who have finished traversing all their child nodes as new child nodes in the next step 
			n = 0; k = 0;
			for (i=0; i<in; i++)
			{
				j = Fatherbuses[i] - 1;//get the indexes of the father nodes
				if (countf[j] == branchf[j])//all the child nodes of bus_i+1 have been calculated, so it can be selected as child node in the next step 
				{
					tempfather[n] = Fatherbuses[i]; //reduplicated father nodes needs to be reduced 
                    n += 1;					
				}		
			}
			for (i=0; i<n; i++)
			{
				CRflag = 0;
				for (j=i+1; j<n; j++)
				{
					if (tempfather[i] == tempfather[j])
					{
						CRflag = 1;
					}
				}			
				if (CRflag == 0)
				{
					Childbuses[k] = tempfather[i];
					k += 1;
				}
			}
			in = k;				
            //calculations at the father nodes, add the contribution of load current to those father nodes who have finished traversing all their child nodes.
            for (i=0; i<in; i++)
			{
                f = Childbuses[i] - 1; //get index			
				//get phase to ground and phase to phase voltage magnitude	
				VAmag[f] = sqrt(VphaseAreal[f]*VphaseAreal[f] + VphaseAimg[f]*VphaseAimg[f]);
				VBmag[f] = sqrt(VphaseBreal[f]*VphaseBreal[f] + VphaseBimg[f]*VphaseBimg[f]);
				VCmag[f] = sqrt(VphaseCreal[f]*VphaseCreal[f] + VphaseCimg[f]*VphaseCimg[f]);
				VphaseABreal[f] = VphaseAreal[f] - VphaseBreal[f];
				VphaseBCreal[f] = VphaseBreal[f] - VphaseCreal[f];
				VphaseCAreal[f] = VphaseCreal[f] - VphaseAreal[f];
				VphaseABimg[f] = VphaseAimg[f] - VphaseBimg[f];
				VphaseBCimg[f] = VphaseBimg[f] - VphaseCimg[f];
				VphaseCAimg[f] = VphaseCimg[f] - VphaseAimg[f];
				VABmag[f] = sqrt(VphaseABreal[f]*VphaseABreal[f] + VphaseABimg[f]*VphaseABimg[f]);
				VBCmag[f] = sqrt(VphaseBCreal[f]*VphaseBCreal[f] + VphaseBCimg[f]*VphaseBCimg[f]);
				VCAmag[f] = sqrt(VphaseCAreal[f]*VphaseCAreal[f] + VphaseCAimg[f]*VphaseCAimg[f]);
			    //get actual capacitor reactive power 
				CapA_cal[f] = CapA[f]*VAmag[f]*VAmag[f];
				CapB_cal[f] = CapB[f]*VBmag[f]*VBmag[f];
				CapC_cal[f] = CapC[f]*VCmag[f]*VCmag[f];
			    //get actual load and current based on voltages
				if (LM[f] == 1)
				{
					PdA_cal[f] = PdA[f];
					QdA_cal[f] = QdA[f];
					PdB_cal[f] = PdB[f];
					QdB_cal[f] = QdB[f];
					PdC_cal[f] = PdC[f];
					QdC_cal[f] = QdC[f];
					IphaseAreal[f] += (PdA_cal[f]*VphaseAreal[f] + (QdA_cal[f]-CapA_cal[f])*VphaseAimg[f])/(VphaseAreal[f]*VphaseAreal[f] + VphaseAimg[f]*VphaseAimg[f]);
					IphaseBreal[f] += (PdB_cal[f]*VphaseBreal[f] + (QdB_cal[f]-CapB_cal[f])*VphaseBimg[f])/(VphaseBreal[f]*VphaseBreal[f] + VphaseBimg[f]*VphaseBimg[f]);
					IphaseCreal[f] += (PdC_cal[f]*VphaseCreal[f] + (QdC_cal[f]-CapC_cal[f])*VphaseCimg[f])/(VphaseCreal[f]*VphaseCreal[f] + VphaseCimg[f]*VphaseCimg[f]);
					IphaseAimg[f] += (PdA_cal[f]*VphaseAimg[f]-(QdA_cal[f]-CapA_cal[f])*VphaseAreal[f])/(VphaseAreal[f]*VphaseAreal[f] + VphaseAimg[f]*VphaseAimg[f]);
					IphaseBimg[f] += (PdB_cal[f]*VphaseBimg[f]-(QdB_cal[f]-CapB_cal[f])*VphaseBreal[f])/(VphaseBreal[f]*VphaseBreal[f] + VphaseBimg[f]*VphaseBimg[f]);
					IphaseCimg[f] += (PdC_cal[f]*VphaseCimg[f]-(QdC_cal[f]-CapC_cal[f])*VphaseCreal[f])/(VphaseCreal[f]*VphaseCreal[f] + VphaseCimg[f]*VphaseCimg[f]);
					
				}
				else if (LM[f] == 2)
				{
					PdA_cal[f] = PdA[f]*VAmag[f]*VAmag[f];
					QdA_cal[f] = QdA[f]*VAmag[f]*VAmag[f];
					PdB_cal[f] = PdB[f]*VBmag[f]*VBmag[f];
					QdB_cal[f] = QdB[f]*VBmag[f]*VBmag[f];
					PdC_cal[f] = PdC[f]*VCmag[f]*VCmag[f];
					QdC_cal[f] = QdC[f]*VCmag[f]*VCmag[f];
					IphaseAreal[f] += (PdA_cal[f]*VphaseAreal[f] + (QdA_cal[f]-CapA_cal[f])*VphaseAimg[f])/(VphaseAreal[f]*VphaseAreal[f] + VphaseAimg[f]*VphaseAimg[f]);
					IphaseBreal[f] += (PdB_cal[f]*VphaseBreal[f] + (QdB_cal[f]-CapB_cal[f])*VphaseBimg[f])/(VphaseBreal[f]*VphaseBreal[f] + VphaseBimg[f]*VphaseBimg[f]);
					IphaseCreal[f] += (PdC_cal[f]*VphaseCreal[f] + (QdC_cal[f]-CapC_cal[f])*VphaseCimg[f])/(VphaseCreal[f]*VphaseCreal[f] + VphaseCimg[f]*VphaseCimg[f]);
					IphaseAimg[f] += (PdA_cal[f]*VphaseAimg[f]-(QdA_cal[f]-CapA_cal[f])*VphaseAreal[f])/(VphaseAreal[f]*VphaseAreal[f] + VphaseAimg[f]*VphaseAimg[f]);
					IphaseBimg[f] += (PdB_cal[f]*VphaseBimg[f]-(QdB_cal[f]-CapB_cal[f])*VphaseBreal[f])/(VphaseBreal[f]*VphaseBreal[f] + VphaseBimg[f]*VphaseBimg[f]);
					IphaseCimg[f] += (PdC_cal[f]*VphaseCimg[f]-(QdC_cal[f]-CapC_cal[f])*VphaseCreal[f])/(VphaseCreal[f]*VphaseCreal[f] + VphaseCimg[f]*VphaseCimg[f]);
				}
				else if (LM[f] == 3)
				{
					PdA_cal[f] = PdA[f]*VAmag[f];
					QdA_cal[f] = QdA[f]*VAmag[f];
					PdB_cal[f] = PdB[f]*VBmag[f];
					QdB_cal[f] = QdB[f]*VBmag[f];
					PdC_cal[f] = PdC[f]*VCmag[f];
					QdC_cal[f] = QdC[f]*VCmag[f];
					IphaseAreal[f] += (PdA_cal[f]*VphaseAreal[f] + (QdA_cal[f]-CapA_cal[f])*VphaseAimg[f])/(VphaseAreal[f]*VphaseAreal[f] + VphaseAimg[f]*VphaseAimg[f]);
					IphaseBreal[f] += (PdB_cal[f]*VphaseBreal[f] + (QdB_cal[f]-CapB_cal[f])*VphaseBimg[f])/(VphaseBreal[f]*VphaseBreal[f] + VphaseBimg[f]*VphaseBimg[f]);
					IphaseCreal[f] += (PdC_cal[f]*VphaseCreal[f] + (QdC_cal[f]-CapC_cal[f])*VphaseCimg[f])/(VphaseCreal[f]*VphaseCreal[f] + VphaseCimg[f]*VphaseCimg[f]);
					IphaseAimg[f] += (PdA_cal[f]*VphaseAimg[f]-(QdA_cal[f]-CapA_cal[f])*VphaseAreal[f])/(VphaseAreal[f]*VphaseAreal[f] + VphaseAimg[f]*VphaseAimg[f]);
					IphaseBimg[f] += (PdB_cal[f]*VphaseBimg[f]-(QdB_cal[f]-CapB_cal[f])*VphaseBreal[f])/(VphaseBreal[f]*VphaseBreal[f] + VphaseBimg[f]*VphaseBimg[f]);
					IphaseCimg[f] += (PdC_cal[f]*VphaseCimg[f]-(QdC_cal[f]-CapC_cal[f])*VphaseCreal[f])/(VphaseCreal[f]*VphaseCreal[f] + VphaseCimg[f]*VphaseCimg[f]);
				}
				else if (LM[f] == 4)
				{
					PdAB_cal[f] = PdA[f];
					QdAB_cal[f] = QdA[f];
					PdBC_cal[f] = PdB[f];
					QdBC_cal[f] = QdB[f];
					PdCA_cal[f] = PdC[f];
					QdCA_cal[f] = QdC[f];
					IphaseABreal[f] = (PdAB_cal[f]*VphaseABreal[f] + QdAB_cal[f]*VphaseABimg[f])/(VphaseABreal[f]*VphaseABreal[f] + VphaseABimg[f]*VphaseABimg[f]);
					IphaseABimg[f] = (PdAB_cal[f]*VphaseABimg[f] - QdAB_cal[f]*VphaseABreal[f])/(VphaseABreal[f]*VphaseABreal[f] + VphaseABimg[f]*VphaseABimg[f]);
					IphaseBCreal[f] = (PdBC_cal[f]*VphaseBCreal[f] + QdBC_cal[f]*VphaseBCimg[f])/(VphaseBCreal[f]*VphaseBCreal[f] + VphaseBCimg[f]*VphaseBCimg[f]);
					IphaseBCimg[f] = (PdBC_cal[f]*VphaseBCimg[f] - QdBC_cal[f]*VphaseBCreal[f])/(VphaseBCreal[f]*VphaseBCreal[f] + VphaseBCimg[f]*VphaseBCimg[f]);
					IphaseCAreal[f] = (PdCA_cal[f]*VphaseCAreal[f] + QdCA_cal[f]*VphaseCAimg[f])/(VphaseCAreal[f]*VphaseCAreal[f] + VphaseCAimg[f]*VphaseCAimg[f]);
					IphaseCAimg[f] = (PdCA_cal[f]*VphaseCAimg[f] - QdCA_cal[f]*VphaseCAreal[f])/(VphaseCAreal[f]*VphaseCAreal[f] + VphaseCAimg[f]*VphaseCAimg[f]);
					IphaseAreal[f] += IphaseABreal[f]-IphaseCAreal[f]-CapA_cal[f]*VphaseAimg[f]/(VphaseAreal[f]*VphaseAreal[f] + VphaseAimg[f]*VphaseAimg[f]);
					IphaseAimg[f] += IphaseABimg[f]-IphaseCAimg[f]+CapA_cal[f]*VphaseAreal[f]/(VphaseAreal[f]*VphaseAreal[f] + VphaseAimg[f]*VphaseAimg[f]);
					IphaseBreal[f] += IphaseBCreal[f]-IphaseABreal[f]-CapB_cal[f]*VphaseBimg[f]/(VphaseBreal[f]*VphaseBreal[f] + VphaseBimg[f]*VphaseBimg[f]);
					IphaseBimg[f] += IphaseBCimg[f]-IphaseABimg[f]+CapB_cal[f]*VphaseBreal[f]/(VphaseBreal[f]*VphaseBreal[f] + VphaseBimg[f]*VphaseBimg[f]);
					IphaseCreal[f] += IphaseCAreal[f]-IphaseBCreal[f]-CapC_cal[f]*VphaseCimg[f]/(VphaseCreal[f]*VphaseCreal[f] + VphaseCimg[f]*VphaseCimg[f]);
					IphaseCimg[f] += IphaseCAimg[f]-IphaseBCimg[f]+CapC_cal[f]*VphaseCreal[f]/(VphaseCreal[f]*VphaseCreal[f] + VphaseCimg[f]*VphaseCimg[f]);
				}
				else if (LM[f] == 5)
				{
					PdAB_cal[f] = PdA[f]*VABmag[f]*VABmag[f]/3;
					QdAB_cal[f] = QdA[f]*VABmag[f]*VABmag[f]/3;
					PdBC_cal[f] = PdB[f]*VBCmag[f]*VBCmag[f]/3;
					QdBC_cal[f] = QdB[f]*VBCmag[f]*VBCmag[f]/3;
					PdCA_cal[f] = PdC[f]*VCAmag[f]*VCAmag[f]/3;
					QdCA_cal[f] = QdC[f]*VCAmag[f]*VCAmag[f]/3;
					IphaseABreal[f] = (PdAB_cal[f]*VphaseABreal[f] + QdAB_cal[f]*VphaseABimg[f])/(VphaseABreal[f]*VphaseABreal[f] + VphaseABimg[f]*VphaseABimg[f]);
					IphaseABimg[f] = (PdAB_cal[f]*VphaseABimg[f] - QdAB_cal[f]*VphaseABreal[f])/(VphaseABreal[f]*VphaseABreal[f] + VphaseABimg[f]*VphaseABimg[f]);
					IphaseBCreal[f] = (PdBC_cal[f]*VphaseBCreal[f] + QdBC_cal[f]*VphaseBCimg[f])/(VphaseBCreal[f]*VphaseBCreal[f] + VphaseBCimg[f]*VphaseBCimg[f]);
					IphaseBCimg[f] = (PdBC_cal[f]*VphaseBCimg[f] - QdBC_cal[f]*VphaseBCreal[f])/(VphaseBCreal[f]*VphaseBCreal[f] + VphaseBCimg[f]*VphaseBCimg[f]);
					IphaseCAreal[f] = (PdCA_cal[f]*VphaseCAreal[f] + QdCA_cal[f]*VphaseCAimg[f])/(VphaseCAreal[f]*VphaseCAreal[f] + VphaseCAimg[f]*VphaseCAimg[f]);
					IphaseCAimg[f] = (PdCA_cal[f]*VphaseCAimg[f] - QdCA_cal[f]*VphaseCAreal[f])/(VphaseCAreal[f]*VphaseCAreal[f] + VphaseCAimg[f]*VphaseCAimg[f]);
					IphaseAreal[f] += IphaseABreal[f]-IphaseCAreal[f]-CapA_cal[f]*VphaseAimg[f]/(VphaseAreal[f]*VphaseAreal[f] + VphaseAimg[f]*VphaseAimg[f]);
					IphaseAimg[f] += IphaseABimg[f]-IphaseCAimg[f]+CapA_cal[f]*VphaseAreal[f]/(VphaseAreal[f]*VphaseAreal[f] + VphaseAimg[f]*VphaseAimg[f]);
					IphaseBreal[f] += IphaseBCreal[f]-IphaseABreal[f]-CapB_cal[f]*VphaseBimg[f]/(VphaseBreal[f]*VphaseBreal[f] + VphaseBimg[f]*VphaseBimg[f]);
					IphaseBimg[f] += IphaseBCimg[f]-IphaseABimg[f]+CapB_cal[f]*VphaseBreal[f]/(VphaseBreal[f]*VphaseBreal[f] + VphaseBimg[f]*VphaseBimg[f]);
					IphaseCreal[f] += IphaseCAreal[f]-IphaseBCreal[f]-CapC_cal[f]*VphaseCimg[f]/(VphaseCreal[f]*VphaseCreal[f] + VphaseCimg[f]*VphaseCimg[f]);
					IphaseCimg[f] += IphaseCAimg[f]-IphaseBCimg[f]+CapC_cal[f]*VphaseCreal[f]/(VphaseCreal[f]*VphaseCreal[f] + VphaseCimg[f]*VphaseCimg[f]);
				}
				else if (LM[f] == 6)
				{
					PdAB_cal[f] = PdA[f]*(VABmag[f]/sqrt(3));
					QdAB_cal[f] = QdA[f]*(VABmag[f]/sqrt(3));
					PdBC_cal[f] = PdB[f]*(VBCmag[f]/sqrt(3));
					QdBC_cal[f] = QdB[f]*(VBCmag[f]/sqrt(3));
					PdCA_cal[f] = PdC[f]*(VCAmag[f]/sqrt(3));
					QdCA_cal[f] = QdC[f]*(VCAmag[f]/sqrt(3));
					IphaseABreal[f] = (PdAB_cal[f]*VphaseABreal[f] + QdAB_cal[f]*VphaseABimg[f])/(VphaseABreal[f]*VphaseABreal[f] + VphaseABimg[f]*VphaseABimg[f]);
					IphaseABimg[f] = (PdAB_cal[f]*VphaseABimg[f] - QdAB_cal[f]*VphaseABreal[f])/(VphaseABreal[f]*VphaseABreal[f] + VphaseABimg[f]*VphaseABimg[f]);
					IphaseBCreal[f] = (PdBC_cal[f]*VphaseBCreal[f] + QdBC_cal[f]*VphaseBCimg[f])/(VphaseBCreal[f]*VphaseBCreal[f] + VphaseBCimg[f]*VphaseBCimg[f]);
					IphaseBCimg[f] = (PdBC_cal[f]*VphaseBCimg[f] - QdBC_cal[f]*VphaseBCreal[f])/(VphaseBCreal[f]*VphaseBCreal[f] + VphaseBCimg[f]*VphaseBCimg[f]);
					IphaseCAreal[f] = (PdCA_cal[f]*VphaseCAreal[f] + QdCA_cal[f]*VphaseCAimg[f])/(VphaseCAreal[f]*VphaseCAreal[f] + VphaseCAimg[f]*VphaseCAimg[f]);
					IphaseCAimg[f] = (PdCA_cal[f]*VphaseCAimg[f] - QdCA_cal[f]*VphaseCAreal[f])/(VphaseCAreal[f]*VphaseCAreal[f] + VphaseCAimg[f]*VphaseCAimg[f]);
					IphaseAreal[f] += IphaseABreal[f]-IphaseCAreal[f]-CapA_cal[f]*VphaseAimg[f]/(VphaseAreal[f]*VphaseAreal[f] + VphaseAimg[f]*VphaseAimg[f]);
					IphaseAimg[f] += IphaseABimg[f]-IphaseCAimg[f]+CapA_cal[f]*VphaseAreal[f]/(VphaseAreal[f]*VphaseAreal[f] + VphaseAimg[f]*VphaseAimg[f]);
					IphaseBreal[f] += IphaseBCreal[f]-IphaseABreal[f]-CapB_cal[f]*VphaseBimg[f]/(VphaseBreal[f]*VphaseBreal[f] + VphaseBimg[f]*VphaseBimg[f]);
					IphaseBimg[f] += IphaseBCimg[f]-IphaseABimg[f]+CapB_cal[f]*VphaseBreal[f]/(VphaseBreal[f]*VphaseBreal[f] + VphaseBimg[f]*VphaseBimg[f]);
					IphaseCreal[f] += IphaseCAreal[f]-IphaseBCreal[f]-CapC_cal[f]*VphaseCimg[f]/(VphaseCreal[f]*VphaseCreal[f] + VphaseCimg[f]*VphaseCimg[f]);
					IphaseCimg[f] += IphaseCAimg[f]-IphaseBCimg[f]+CapC_cal[f]*VphaseCreal[f]/(VphaseCreal[f]*VphaseCreal[f] + VphaseCimg[f]*VphaseCimg[f]);
				}
				
			    
				// calculation at the start node
				if (f == SNindex)
				{
					Change = (abs(VAmag[f] - SNVm) + abs(VBmag[f] - SNVm) + abs(VCmag[f] - SNVm))/3;
					Accuracy = Change; // accuracy in p.u.
				}
		    }
			
	 //printf("\n\n================================================================================================\n ");
 	//for (i=0;i<nv;i++)
	//{
		//std::cout << "ID:" << i+1 <<" VAr: "<<VphaseAreal[i]<<" VAi "<<VphaseAimg[i]<<" VBr: "<<VphaseBreal[i]<<" VBi "<<VphaseBimg[i]<<" VCr: "<<VphaseCreal[i]<<" VCi "<<VphaseCimg[i] <<" Vbase "<<Vbase[i]<<" IAr: "<<IphaseAreal[i]<<" IAi "<<IphaseAimg[i]<<" IBr: "<<IphaseBreal[i]<<" IBi "<<IphaseBimg[i]<<" ICr: "<<IphaseCreal[i]<<" ICi "<<IphaseCimg[i]<<" loadmodel "<<LM[i] << std::endl;
	//}
	//std::cout << "Maxit : "<<Maxit <<" it "<<iter<<" Maxchange :" << Maxchange <<" Accuracy "<<Accuracy<< std::endl; 
	//std::cout << "SNcount:" << countf[SNindex] <<" SNbranch: "<<branchf[SNindex]<<" lv "<<lv <<" f "<<f<< std::endl;	

	//printf("\n================================================================================================ \n\n"); 
		
		lv++;
				
		}
		lv = 0;	
        endflag = 0;	
		// backward sweep, update voltages 
        while (endflag == 0 && lv<100)		
		{	
	        if (lv == 0) //first step 
			{
				in = 1;
				Fatherbuses[in-1]=SNindex+1;
				VphaseAreal[SNindex] = SNVm*cos(SNVs);
				VphaseAimg[SNindex] = SNVm*sin(SNVs);
				VphaseBreal[SNindex] = SNVm*cos(SNVs - 2*pi_value/3);
				VphaseBimg[SNindex] = SNVm*sin(SNVs - 2*pi_value/3);
				VphaseCreal[SNindex] = SNVm*cos(SNVs + 2*pi_value/3);
				VphaseCimg[SNindex] = SNVm*sin(SNVs + 2*pi_value/3);
			}
	        k = 0;
		    for (i=0; i<in; i++)
			{
				for (j=0; j<ne; j++)
					{   
						if(IDF[j] == Fatherbuses[i])
						{
							Links[k] = j; //store the link index between the child nodes and father nodes.
							Childbuses[k] = IDT[j]; //store the id of child nodes.
							countc[IDT[j]-1] += 1; //Array_id = bus_id -1
							k += 1;//number of child nodes
							
						

						}
					}
			}
			in = k;
			for (i=0; i<in; i++)
			{
				j = Links[i]; //selected link index
				f = IDF[j]-1; //selected father node index
				c = IDT[j]-1; //selected child node index
				//std::cout<<"inid "<<in<<" fid "<<i << " Fatherbuses :" << IDF[j] <<" cid "<< k<<" Childbuses "<<IDT[j]<<" countf "<<countc[c]<< std::endl;

				if (edgetype[j]==0) //if the selected link is a feeder
				{
					VphaseAreal[c] = Areal[j][0][0]*VphaseAreal[f] + Areal[j][0][1]*VphaseBreal[f] + Areal[j][0][2]*VphaseCreal[f] - Aimg[j][0][0]*VphaseAimg[f] - Aimg[j][0][1]*VphaseBimg[f] - Aimg[j][0][2]*VphaseCimg[f] - Breal[j][0][0]*IphaseAreal[c] - Breal[j][0][1]*IphaseBreal[c] - Breal[j][0][2]*IphaseCreal[c] + Bimg[j][0][0]*IphaseAimg[c] + Bimg[j][0][1]*IphaseBimg[c] + Bimg[j][0][2]*IphaseCimg[c];
					VphaseBreal[c] = Areal[j][1][0]*VphaseAreal[f] + Areal[j][1][1]*VphaseBreal[f] + Areal[j][1][2]*VphaseCreal[f] - Aimg[j][1][0]*VphaseAimg[f] - Aimg[j][1][1]*VphaseBimg[f] - Aimg[j][1][2]*VphaseCimg[f] - Breal[j][1][0]*IphaseAreal[c] - Breal[j][1][1]*IphaseBreal[c] - Breal[j][1][2]*IphaseCreal[c] + Bimg[j][1][0]*IphaseAimg[c] + Bimg[j][1][1]*IphaseBimg[c] + Bimg[j][1][2]*IphaseCimg[c];
					VphaseCreal[c] = Areal[j][2][0]*VphaseAreal[f] + Areal[j][2][1]*VphaseBreal[f] + Areal[j][2][2]*VphaseCreal[f] - Aimg[j][2][0]*VphaseAimg[f] - Aimg[j][2][1]*VphaseBimg[f] - Aimg[j][2][2]*VphaseCimg[f] - Breal[j][2][0]*IphaseAreal[c] - Breal[j][2][1]*IphaseBreal[c] - Breal[j][2][2]*IphaseCreal[c] + Bimg[j][2][0]*IphaseAimg[c] + Bimg[j][2][1]*IphaseBimg[c] + Bimg[j][2][2]*IphaseCimg[c];
					
					VphaseAimg[c] = Aimg[j][0][0]*VphaseAreal[f] + Aimg[j][0][1]*VphaseBreal[f] + Aimg[j][0][2]*VphaseCreal[f] + Areal[j][0][0]*VphaseAimg[f] + Areal[j][0][1]*VphaseBimg[f] + Areal[j][0][2]*VphaseCimg[f] - Breal[j][0][0]*IphaseAimg[c] - Breal[j][0][1]*IphaseBimg[c] - Breal[j][0][2]*IphaseCimg[c] - Bimg[j][0][0]*IphaseAreal[c] - Bimg[j][0][1]*IphaseBreal[c] - Bimg[j][0][2]*IphaseCreal[c];
					VphaseBimg[c] = Aimg[j][1][0]*VphaseAreal[f] + Aimg[j][1][1]*VphaseBreal[f] + Aimg[j][1][2]*VphaseCreal[f] + Areal[j][1][0]*VphaseAimg[f] + Areal[j][1][1]*VphaseBimg[f] + Areal[j][1][2]*VphaseCimg[f] - Breal[j][1][0]*IphaseAimg[c] - Breal[j][1][1]*IphaseBimg[c] - Breal[j][1][2]*IphaseCimg[c] - Bimg[j][1][0]*IphaseAreal[c] - Bimg[j][1][1]*IphaseBreal[c] - Bimg[j][1][2]*IphaseCreal[c];
					VphaseCimg[c] = Aimg[j][2][0]*VphaseAreal[f] + Aimg[j][2][1]*VphaseBreal[f] + Aimg[j][2][2]*VphaseCreal[f] + Areal[j][2][0]*VphaseAimg[f] + Areal[j][2][1]*VphaseBimg[f] + Areal[j][2][2]*VphaseCimg[f] - Breal[j][2][0]*IphaseAimg[c] - Breal[j][2][1]*IphaseBimg[c] - Breal[j][2][2]*IphaseCimg[c] - Bimg[j][2][0]*IphaseAreal[c] - Bimg[j][2][1]*IphaseBreal[c] - Bimg[j][2][2]*IphaseCreal[c];
								
				}
				else if (edgetype[j]==1) //if the selected link is a transformer
				{
					VphaseAreal[c] = At[j][0][0]*VphaseAreal[f] + At[j][0][1]*VphaseBreal[f] + At[j][0][2]*VphaseCreal[f] - Raa[j]*IphaseAreal[c] + Xaa[j]*IphaseAimg[c];
					VphaseBreal[c] = At[j][1][0]*VphaseAreal[f] + At[j][1][1]*VphaseBreal[f] + At[j][1][2]*VphaseCreal[f] - Rbb[j]*IphaseBreal[c] + Xbb[j]*IphaseBimg[c];
					VphaseCreal[c] = At[j][2][0]*VphaseAreal[f] + At[j][2][1]*VphaseBreal[f] + At[j][2][2]*VphaseCreal[f] - Rcc[j]*IphaseCreal[c] + Xcc[j]*IphaseCimg[c];
					
					VphaseAimg[c] = At[j][0][0]*VphaseAimg[f] + At[j][0][1]*VphaseBimg[f] + At[j][0][2]*VphaseCimg[f] - Xaa[j]*IphaseAreal[c] - Raa[j]*IphaseAimg[c];
					VphaseBimg[c] = At[j][1][0]*VphaseAimg[f] + At[j][1][1]*VphaseBimg[f] + At[j][1][2]*VphaseCimg[f] - Xbb[j]*IphaseBreal[c] - Rbb[j]*IphaseBimg[c];
					VphaseCimg[c] = At[j][2][0]*VphaseAimg[f] + At[j][2][1]*VphaseBimg[f] + At[j][2][2]*VphaseCimg[f] - Xcc[j]*IphaseCreal[c] - Rcc[j]*IphaseCimg[c];
				}
				else if (edgetype[j]==2) //if the selected link is a regulator
				{
					/* tempVregAR = VphaseAreal[f]/PT_reg[c];
					tempVregAI = VphaseAimg[f]/PT_reg[c];
					tempVregBR = VphaseBreal[f]/PT_reg[c];
					tempVregBI = VphaseBimg[f]/PT_reg[c];
					tempVregCR = VphaseCreal[f]/PT_reg[c];
					tempVregCI = VphaseCimg[f]/PT_reg[c];
					tempIcompAR = IphaseAreal[c]/CT_reg[c];
					tempIcompAI = IphaseAimg[c]/CT_reg[c];
					tempIcompBR = IphaseBreal[c]/CT_reg[c];
					tempIcompBI = IphaseBimg[c]/CT_reg[c];
					tempIcompCR = IphaseCreal[c]/CT_reg[c];
					tempIcompCI = IphaseCimg[c]/CT_reg[c];
					VrelyAR = tempVregAR - (Raa[j]*tempIcompAR - Xaa[j]*tempIcompAI);
					VrelyAI = tempVregAI - (Raa[j]*tempIcompAI + Xaa[j]*tempIcompAR);
					VrelyBR = tempVregBR - (Rbb[j]*tempIcompBR - Xbb[j]*tempIcompBI);
					VrelyBI = tempVregBI - (Rbb[j]*tempIcompBI + Xbb[j]*tempIcompBR);
					VrelyCR = tempVregCR - (Rcc[j]*tempIcompCR - Xcc[j]*tempIcompCI);
					VrelyCI = tempVregCI - (Rcc[j]*tempIcompCI + Xcc[j]*tempIcompCR);
					VrelyAmag = sqrt(VrelyAR*VrelyAR + VrelyAI*VrelyAI);
					VrelyBmag = sqrt(VrelyBR*VrelyBR + VrelyBI*VrelyBI);
					VrelyCmag = sqrt(VrelyCR*VrelyCR + VrelyCI*VrelyCI);
					//calcuate tap 
					TapVlow_A=vol_reg_A[c] - 0.5*BW_reg[c];
					TapVhigh_A=vol_reg_A[c] + 0.5*BW_reg[c];
					TapVlow_B=vol_reg_B[c] - 0.5*BW_reg[c];
					TapVhigh_B=vol_reg_B[c] + 0.5*BW_reg[c];
					TapVlow_C=vol_reg_C[c] - 0.5*BW_reg[c];
					TapVhigh_C=vol_reg_C[c] + 0.5*BW_reg[c];
			
					if (TapVlow_A>VrelyAmag) 
					{
						TapA[c] = floor((TapVlow_A - VrelyAmag)/0.75)+1;
						if (TapA[c]>16) 
						{
							TapA[c]=16;
						}
					}
					else if (TapVhigh_A<=VrelyAmag) 
						TapA[c] = floor((TapVhigh_A - VrelyAmag)/0.75);
						if (TapA[c]<-16)
						{
							TapA[c]=-16;
						}
					if (TapVlow_B>VrelyBmag)
					{
						TapB[c] = floor((TapVlow_B - VrelyBmag)/0.75)+1;
						if (TapB[c]>16)
						{
							TapB[c] = 16;
						}
					}
					else if (TapVhigh_B<=VrelyBmag)
					{
						TapB[c] = floor((TapVhigh_B - VrelyBmag)/0.75);
						if (TapB[c]<-16)
						{
							TapB[c]=-16;
						}
					}
					if (TapVlow_C>VrelyCmag)
					{
						TapC[c] = floor((TapVlow_C - VrelyCmag)/0.75)+1;
						if (TapC[c]>16)
						{
							TapC[c] = 16;
						}
					}
					else if (TapVhigh_C<=VrelyCmag)
					{
						TapC[c] = floor((TapVhigh_C - VrelyCmag)/0.75);
						if (TapC[c]<-16)
						{
							TapC[c]=-16;
						}
					}
					//determine the monitoring phase
					if (control_phase_reg[c]==1)
					{
						TapB[c]=TapA[c];
						TapC[c]=TapA[c];
					}
					else if (control_phase_reg[c]==2)
					{
						TapA[c]=TapB[c];
						TapC[c]=TapB[c];
					}
					else if (control_phase_reg[c]==3)
					{
						TapA[c]=TapC[c];
						TapB[c]=TapC[c];
					}
					aRa[c] = 1 + 0.00625*TapA[c];
					aRb[c] = 1 + 0.00625*TapB[c];
					aRc[c] = 1 + 0.00625*TapC[c];
					dRa[c] = 1/aRa[c];
					dRb[c] = 1/aRb[c];
					dRc[c] = 1/aRc[c]; */
					VphaseAreal[c] = VphaseAreal[f]*aRa[c];
					VphaseAimg[c] = VphaseAimg[f]*aRa[c];
					VphaseBreal[c] = VphaseBreal[f]*aRb[c];
					VphaseBimg[c] = VphaseBimg[f]*aRb[c];
					VphaseCreal[c] = VphaseCreal[f]*aRc[c];
					VphaseCimg[c] = VphaseCimg[f]*aRc[c];
				}	
			}
			//update father nodes
			for (i=0; i<in; i++)
			{
				Fatherbuses[i] = Childbuses[i];
			}
			//check if all the node voltages are updated
			endflag = 1;
			for (i=0; i<tn; i++)
			{
				j = Terminalbuses[i] - 1;//get the index of terminal buses
				if (countc[j] == 0)
				{
					endflag = 0;
				}
			//std::cout<<"lv "<<lv << " Terminal "<<Terminalbuses[i]<<" countc :" << countc[j] <<" endflag "<<endflag<< std::endl;

			}		
		lv++;
		}
	   // std::cout<<"iter "<<iter << " Maxchange :" << Maxchange <<" accuracy "<<Accuracy<< std::endl;
		// If stopping criteria satisfied, then break out the for loop.
    	if (Maxchange > Accuracy)
		{
			break;
		}  
		
			
   		// clear counter and currents
		for (i=0; i<nv; i++)
		{
			countf[i] = 0;
			countc[i] = 0;
			IphaseAreal[i] = 0;
			IphaseAimg[i] = 0;
			IphaseBreal[i] = 0;
			IphaseBimg[i] = 0;
			IphaseCreal[i] = 0;
			IphaseCimg[i] = 0;
		}		
	}
	
    // Re-calculation voltages and accuracy for output, then store results back to the array	
	for (i=0; i<nv; i++)
	{
		totalP += (PdA_cal[i] + PdAB_cal[i] + PdB_cal[i] + PdBC_cal[i] + PdC_cal[i] + PdCA_cal[i])/3;
		totalQ += (QdA_cal[i] + QdAB_cal[i] + QdB_cal[i] + QdBC_cal[i] + QdC_cal[i] + QdCA_cal[i])/3;
	}
	for (i=0; i<nv; i++) 
	{	
		VAmag[i] = sqrt(VphaseAreal[i]*VphaseAreal[i] + VphaseAimg[i]*VphaseAimg[i]);		
		VBmag[i] = sqrt(VphaseBreal[i]*VphaseBreal[i] + VphaseBimg[i]*VphaseBimg[i]);		
		VCmag[i] = sqrt(VphaseCreal[i]*VphaseCreal[i] + VphaseCimg[i]*VphaseCimg[i]);
		if (VphaseAreal[i] > 0)
		{
			VAang[i] = atan(VphaseAimg[i]/VphaseAreal[i])/pi_value*180;
		} 
		else {
			VAang[i] = atan(VphaseAimg[i]/VphaseAreal[i])/pi_value*180 + 180;
		} 
		if (VphaseBreal[i] > 0)
		{
			VBang[i] = atan(VphaseBimg[i]/VphaseBreal[i])/pi_value*180;
		} 
		else {
			VBang[i] = atan(VphaseBimg[i]/VphaseBreal[i])/pi_value*180 - 180;
		}
		if (VphaseCreal[i] > 0)
		{
			VCang[i] = atan(VphaseCimg[i]/VphaseCreal[i])/pi_value*180;
		} 
		else {
			VCang[i] = atan(VphaseCimg[i]/VphaseCreal[i])/pi_value*180 + 180;
		}	
		Vertex_D.data_[i].VAang = VAang[i];
		Vertex_D.data_[i].VAmag = VAmag[i];
		Vertex_D.data_[i].VBang = VBang[i];
		Vertex_D.data_[i].VBmag = VBmag[i];
		Vertex_D.data_[i].VCang = VCang[i];
		Vertex_D.data_[i].VCmag = VCmag[i];
		Vertex_D.data_[i].PdA_cal = PdA_cal[i] + PdAB_cal[i]; 
		Vertex_D.data_[i].QdA_cal = QdA_cal[i] + QdAB_cal[i]; 
		Vertex_D.data_[i].PdB_cal = PdB_cal[i] + PdBC_cal[i]; 
		Vertex_D.data_[i].QdB_cal = QdB_cal[i] + QdBC_cal[i];
		Vertex_D.data_[i].PdC_cal = PdC_cal[i] + PdCA_cal[i]; 
		Vertex_D.data_[i].QdC_cal = QdC_cal[i] + QdCA_cal[i];
		Vertex_D.data_[i].totalP = totalP;
		Vertex_D.data_[i].totalQ = totalQ;
	} 
	Final_iter = iter+1;
	Final_accuracy = Accuracy;
    //std::cout << "Faccracy: " << Final_accuracy << std::endl;	
	
	
	// free memory
	free(Terminalbuses); free(Links); free(Childbuses); free(Fatherbuses); free(bustype); free(edgetype);
	free(countf); free(branchf); free(ID); free(IDF); free(IDT); free(VphaseAreal); free(VphaseAimg); 
	free(VphaseBreal); free(VphaseBimg); free(VphaseCreal); free(VphaseCimg); free(VAmag); free(VBmag); 
	free(VCmag); free(VABmag); free(VBCmag); free(VCAmag); free(VphaseABreal); free(VphaseABimg); 
	free(VphaseBCreal); free(VphaseBCimg); free(VphaseCAreal); free(VphaseCAimg); free(Vbase); free(CapA); 
	free(CapB); free(CapC); free(CapA_cal); free(CapB_cal); free(CapC_cal); free(PdA); free(QdA); free(PdB); 
	free(QdB); free(PdC); free(QdC); free(PdA_cal); free(QdA_cal); free(PdB_cal); free(QdB_cal); free(PdC_cal); 
	free(QdC_cal); free(PdAB_cal); free(QdAB_cal); free(PdBC_cal); free(QdBC_cal); free(PdCA_cal); free(QdCA_cal); 
	free(aRa); free(aRb); free(aRc); free(dRa); free(dRb); free(dRc); free(IphaseAreal); free(IphaseAimg); free(IphaseBreal); 
	free(IphaseBimg); free(IphaseCreal); free(IphaseCimg); free(IphaseABreal); free(IphaseABimg); free(IphaseBCreal); 
	free(IphaseBCimg); free(IphaseCAreal); free(IphaseCAimg); free(Raa); free(Rab); free(Rac); free(Rba); free(Rbc);
	free(Rca); free(Rcb); free(Rcc); free(Xaa); free(Xab); free(Xac); free(Xba); free(Xbb); free(Xbc); free(Xca); 
	free(Xcb); free(Xcc); free(Nt); 
	
}  

//******************END of Functions***************************************************************************

//************************************************************************************************************
//Function for generating GLM file for GridLAB-D simulation
//Developed by Jun Tan, juntan@geirina.net************************
template <typename powerflowtup, typename nodetup, typename loadtup, typename linetup, typename ohlconfigtup, typename uglconfigtup, typename ohlcondconfigtup, typename uglcondconfigtup, typename lsconfigtup, typename trans_configtup, typename transformertup, typename regu_configtup,typename regulatortup, typename switchtup, typename capacitortup>
inline int GLM(ListAccum<powerflowtup>& PowerFlow, ListAccum<nodetup>& Nodes, ListAccum<loadtup>& SpotLoads, ListAccum<linetup>& Lines, ListAccum<ohlconfigtup>& OHLConfig, ListAccum<uglconfigtup>& UGLConfig, ListAccum<ohlcondconfigtup>& OHLCondConfig, ListAccum<uglcondconfigtup>& UGLCondConfig, ListAccum<lsconfigtup>& LSConfig, ListAccum<trans_configtup>& TransConfig, ListAccum<transformertup>& Transformer, ListAccum<regu_configtup>& ReguConfig, ListAccum<regulatortup>& Regulator, ListAccum<switchtup>& Switches, ListAccum<capacitortup>& Capacitors)
{
  

//sort ListAccum 
    struct ListAccum_sort_OHLCondConfig{
            inline bool operator() (const ohlcondconfigtup& tuple1, const ohlcondconfigtup& tuple2)
            {
                return (tuple1.ohlcondconfig_id < tuple2.ohlcondconfig_id);
            }
	};
	struct ListAccum_sort_UGLCondConfig{
            inline bool operator() (const uglcondconfigtup& tuple1, const uglcondconfigtup& tuple2)
            {
                return (tuple1.uglcondconfig_id < tuple2.uglcondconfig_id);
            }
	};
	struct ListAccum_sort_LSConfig{
            inline bool operator() (const lsconfigtup& tuple1, const lsconfigtup& tuple2)
            {
                return (tuple1.linespacing_id < tuple2.linespacing_id);
            }
	};
	struct ListAccum_sort_OHLConfig{
            inline bool operator() (const ohlconfigtup& tuple1, const ohlconfigtup& tuple2)
            {
                return (tuple1.ohlconfig_id < tuple2.ohlconfig_id);
            }
	};
	struct ListAccum_sort_UGLConfig{
            inline bool operator() (const uglconfigtup& tuple1, const uglconfigtup& tuple2)
            {
                return (tuple1.uglconfig_id < tuple2.uglconfig_id);
            }
	};
    struct ListAccum_sort_nodes{
            inline bool operator() (const nodetup& tuple1, const nodetup& tuple2)
            {
                return (tuple1.name < tuple2.name);
            }
	};
	struct ListAccum_sort_loads{
            inline bool operator() (const loadtup& tuple1, const loadtup& tuple2)
            {
                return (tuple1.name < tuple2.name);
            }
	};
	struct ListAccum_sort_lines{
            inline bool operator() (const linetup& tuple1, const linetup& tuple2)
            {
                return (tuple1.name < tuple2.name);
            }
	};
    
	std::sort(OHLCondConfig.begin(), OHLCondConfig.end(), ListAccum_sort_OHLCondConfig());
	std::sort(UGLCondConfig.begin(), UGLCondConfig.end(), ListAccum_sort_UGLCondConfig());
	std::sort(LSConfig.begin(), LSConfig.end(), ListAccum_sort_LSConfig());
	std::sort(OHLConfig.begin(), OHLConfig.end(), ListAccum_sort_OHLConfig());
	std::sort(UGLConfig.begin(), UGLConfig.end(), ListAccum_sort_UGLConfig());
	std::sort(Nodes.begin(), Nodes.end(), ListAccum_sort_nodes());
	std::sort(SpotLoads.begin(), SpotLoads.end(), ListAccum_sort_loads());
	std::sort(Lines.begin(), Lines.end(), ListAccum_sort_lines());
 
//start writing GLM 
string file_name="/tmp/123nodetest.glm";
FILE* fp = fopen(file_name.c_str(), "w");
//setting
fprintf(fp,"\n%s\n%s\n%s\n%s\n%s\n",
        "#set profiler=1;",
        "#set pauseatexit=1;",
        "#set iteration_limit=1000;",
		"//#set server_portnum=6267;",
		"#include \"/tmp/appliance_schedules.glm\";");
		
fprintf(fp,"\n%s%s%s",
        "//realstoptime ",PowerFlow.data_[0].stoptime.c_str(),";");	 
//time
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n%s\n",
       "clock {",
       "starttime '",PowerFlow.data_[0].starttime.c_str(),"';",
       "stoptime '",PowerFlow.data_[0].fakestoptime.c_str(),"';",
       "timezone ","PST+0PDT",";",
       "};");

//simulation step

fprintf(fp,"\nmodule tape;\n");

fprintf(fp,"\n%s\n\t%s%s%s\n\t%s\n%s\n",
       "module powerflow {",
       "solver_method ",PowerFlow.data_[0].solver.c_str(),";",
       "default_maximum_voltage_error 1e-6;",
       "};");


//Phase Conductor for 1 thru 11:
for (int i=0;i<OHLCondConfig.size();i++)
{
   fprintf(fp,"\n%s%d%s\n\t%s%f%s\n\t%s%f%s\n%s\n",
            "object overhead_line_conductor:",OHLCondConfig.data_[i].ohlcondconfig_id," {",
            "geometric_mean_radius ",OHLCondConfig.data_[i].conductor_gmr,";",
            "resistance ",OHLCondConfig.data_[i].conductor_resistance,";",
            "}");  
}
// Phase Conductor for 12: 1/0 AA,TS
for (int i=0;i<UGLCondConfig.size();i++)
{
   fprintf(fp,"\n%s%d%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n%s\n",
            "object underground_line_conductor:",UGLCondConfig.data_[i].uglcondconfig_id," {",
            "outer_diameter ",UGLCondConfig.data_[i].outer_diameter_ugl,";",
            "conductor_gmr ",UGLCondConfig.data_[i].conductor_gmr,";",
			"conductor_diameter ",UGLCondConfig.data_[i].conductor_diameter_ugl,";",
			"conductor_resistance ",UGLCondConfig.data_[i].conductor_resistance,";",
			"neutral_gmr ",UGLCondConfig.data_[i].neutral_gmr_ugl,";",
			"neutral_resistance ",UGLCondConfig.data_[i].neutral_resistance_ugl,";",
			"neutral_diameter ",UGLCondConfig.data_[i].neutral_diameter_ugl,";",
			"neutral_strands ",UGLCondConfig.data_[i].neutral_strands_ugl,";",
			"shield_gmr ",UGLCondConfig.data_[i].shield_gmr_ugl,";",
			"shield_resistance ",UGLCondConfig.data_[i].shield_resistance_ugl,";",
            "}");  
}
// Overhead line configurations
// ABCN
for (int i=0;i<LSConfig.size();i++)
{
   fprintf(fp,"\n%s%d%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n%s\n",
            "object line_spacing:",LSConfig.data_[i].linespacing_id," {",
            "distance_AB ",LSConfig.data_[i].distance_AB,";",
            "distance_AC ",LSConfig.data_[i].distance_AC,";",
			"distance_BC ",LSConfig.data_[i].distance_BC,";",
			"distance_AN ",LSConfig.data_[i].distance_AN,";",
			"distance_BN ",LSConfig.data_[i].distance_BN,";",
			"distance_CN ",LSConfig.data_[i].distance_CN,";",
            "}");  
}

for (int i=0;i<OHLConfig.size();i++)
{
   fprintf(fp,"\n%s%d%s",
            "object line_configuration:",OHLConfig.data_[i].ohlconfig_id," {");
   if (OHLConfig.data_[i].Raa != 0 or OHLConfig.data_[i].Rbb != 0 or OHLConfig.data_[i].Rcc != 0)
   {
	   fprintf(fp,"\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n%s\n",
            "z11 ",OHLConfig.data_[i].Raa,"+",OHLConfig.data_[i].Xaa,"j;",
			"z12 ",OHLConfig.data_[i].Rab,"+",OHLConfig.data_[i].Xab,"j;",
			"z13 ",OHLConfig.data_[i].Rac,"+",OHLConfig.data_[i].Xac,"j;",
			"z21 ",OHLConfig.data_[i].Rba,"+",OHLConfig.data_[i].Xba,"j;",
			"z22 ",OHLConfig.data_[i].Rbb,"+",OHLConfig.data_[i].Xbb,"j;",
			"z23 ",OHLConfig.data_[i].Rbc,"+",OHLConfig.data_[i].Xbc,"j;",
			"z31 ",OHLConfig.data_[i].Rca,"+",OHLConfig.data_[i].Xca,"j;",
			"z32 ",OHLConfig.data_[i].Rcb,"+",OHLConfig.data_[i].Xcb,"j;",
			"z33 ",OHLConfig.data_[i].Rcc,"+",OHLConfig.data_[i].Xcc,"j;",
			"}");
   }
   else
   {
	if (OHLConfig.data_[i].conductor_A != 0)
	{
		fprintf(fp,"\n\t%s%d%s",
            "conductor_A overhead_line_conductor:",OHLConfig.data_[i].conductor_A,";");
	}
	if (OHLConfig.data_[i].conductor_B != 0)
	{
		fprintf(fp,"\n\t%s%d%s",
            "conductor_B overhead_line_conductor:",OHLConfig.data_[i].conductor_B,";");
	}
	if (OHLConfig.data_[i].conductor_C != 0)
	{
		fprintf(fp,"\n\t%s%d%s",
            "conductor_C overhead_line_conductor:",OHLConfig.data_[i].conductor_C,";");
	}
	if (OHLConfig.data_[i].conductor_N != 0)
	{
		fprintf(fp,"\n\t%s%d%s",
            "conductor_N overhead_line_conductor:",OHLConfig.data_[i].conductor_N,";");
	}
			
        fprintf(fp,"\n\t%s%d%s\n%s\n",
			"spacing line_spacing:",OHLConfig.data_[i].line_spacing,";",
            "}"); 
   }			
}

for (int i=0;i<UGLConfig.size();i++)
{
   fprintf(fp,"\n%s%d%s",
            "object line_configuration:",UGLConfig.data_[i].uglconfig_id," {");
	if (UGLConfig.data_[i].Raa != 0 or UGLConfig.data_[i].Rbb != 0 or UGLConfig.data_[i].Rcc != 0)
   {
	   fprintf(fp,"\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n%s\n",
            "z11 ",UGLConfig.data_[i].Raa,"+",UGLConfig.data_[i].Xaa,"j;",
			"z12 ",UGLConfig.data_[i].Rab,"+",UGLConfig.data_[i].Xab,"j;",
			"z13 ",UGLConfig.data_[i].Rac,"+",UGLConfig.data_[i].Xac,"j;",
			"z21 ",UGLConfig.data_[i].Rba,"+",UGLConfig.data_[i].Xba,"j;",
			"z22 ",UGLConfig.data_[i].Rbb,"+",UGLConfig.data_[i].Xbb,"j;",
			"z23 ",UGLConfig.data_[i].Rbc,"+",UGLConfig.data_[i].Xbc,"j;",
			"z31 ",UGLConfig.data_[i].Rca,"+",UGLConfig.data_[i].Xca,"j;",
			"z32 ",UGLConfig.data_[i].Rcb,"+",UGLConfig.data_[i].Xcb,"j;",
			"z33 ",UGLConfig.data_[i].Rcc,"+",UGLConfig.data_[i].Xcc,"j;",
			"}");
   }
   else
   {
	   fprintf(fp,"\n\t%s%d%s\n\t%s%d%s\n\t%s%d%s\n\t%s%d%s\n%s\n",
            "conductor_A underground_line_conductor:",UGLConfig.data_[i].conductor_A,";",
            "conductor_B underground_line_conductor:",UGLConfig.data_[i].conductor_B,";",
			"conductor_C underground_line_conductor:",UGLConfig.data_[i].conductor_C,";",
			"spacing line_spacing:",UGLConfig.data_[i].line_spacing,";",
            "}");
   }			
}
//Define line objects 
for (int i=0;i<Lines.size();i++)
{   
   if (Lines.data_[i].linetype == 0)
   {
	fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s\n\t%s%d%s\n%s\n",
            "object overhead_line: {",
            "phases \"",Lines.data_[i].phases.c_str(),"\";",
            "name overhead_line_",Lines.data_[i].name.c_str(),";",
			"from node_",Lines.data_[i].from_node.c_str(),";",
			"to node_",Lines.data_[i].to_node.c_str(),";",
			"length ",Lines.data_[i].length,";",
			"configuration line_configuration:",Lines.data_[i].config,";",
            "}");  
    }
    else if (Lines.data_[i].linetype == 1)
	{
     fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s\n\t%s%d%s\n%s\n",
            "object underground_line: {",
            "phases \"",Lines.data_[i].phases.c_str(),"\";",
            "name underground_line_",Lines.data_[i].name.c_str(),";",
			"from node_",Lines.data_[i].from_node.c_str(),";",
			"to node_",Lines.data_[i].to_node.c_str(),";",
			"length ",Lines.data_[i].length,";",
			"configuration line_configuration:",Lines.data_[i].config,";",
            "}"); 
	}
}	
//Create node only objects 
for (int i=0;i<Nodes.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s%f%s\n\t%s%f%f%s\n\t%s%f%s%f%s\n\t%s%f%s\n%s\n",
            "object node {",
            "phases \"",Nodes.data_[i].phases.c_str(),"\";",
            "name node_",Nodes.data_[i].name.c_str(),";",
			"voltage_A ",Nodes.data_[i].VA_real,"+",Nodes.data_[i].VA_img,"j;",
			"voltage_B ",Nodes.data_[i].VB_real,Nodes.data_[i].VB_img,"j;",
			"voltage_C ",Nodes.data_[i].VC_real,"+",Nodes.data_[i].VC_img,"j;",
			"nominal_voltage ",Nodes.data_[i].V_nominal,";",
            "}");   
}
//Create load objects
for (int i=0;i<SpotLoads.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s%f%s\n\t%s%f%f%s\n\t%s%f%s%f%s\n\t%s%f%s",
            "object load {",
            "phases \"",SpotLoads.data_[i].phases.c_str(),"\";",
            "name node_",SpotLoads.data_[i].name.c_str(),";",
			"voltage_A ",SpotLoads.data_[i].VA_real,"+",SpotLoads.data_[i].VA_img,"j;",
			"voltage_B ",SpotLoads.data_[i].VB_real,SpotLoads.data_[i].VB_img,"j;",
			"voltage_C ",SpotLoads.data_[i].VC_real,"+",SpotLoads.data_[i].VC_img,"j;",
			"nominal_voltage ",SpotLoads.data_[i].V_nominal,";");           
   if (SpotLoads.data_[i].cons_current_Areal != 0 or SpotLoads.data_[i].cons_current_Aimg != 0)  
   {
	   if (SpotLoads.data_[i].cons_current_Aimg<0)
	   {
	    fprintf(fp,"\n\t%s%f%f%s",
            "constant_current_A ",SpotLoads.data_[i].cons_current_Areal,SpotLoads.data_[i].cons_current_Aimg,"j;");
	   }
	   else
	   {
		fprintf(fp,"\n\t%s%f%s%f%s",
            "constant_current_A ",SpotLoads.data_[i].cons_current_Areal,"+",SpotLoads.data_[i].cons_current_Aimg,"j;");   
	   }
   }
   if (SpotLoads.data_[i].cons_current_Breal != 0 or SpotLoads.data_[i].cons_current_Bimg != 0)  
   {
	   if (SpotLoads.data_[i].cons_current_Bimg<0)
	   {
	    fprintf(fp,"\n\t%s%f%f%s",
            "constant_current_B ",SpotLoads.data_[i].cons_current_Breal,SpotLoads.data_[i].cons_current_Bimg,"j;");
	   }
	   else
	   {
		fprintf(fp,"\n\t%s%f%s%f%s",
            "constant_current_B ",SpotLoads.data_[i].cons_current_Breal,"+",SpotLoads.data_[i].cons_current_Bimg,"j;");   
	   }
   }
   if (SpotLoads.data_[i].cons_current_Creal != 0 or SpotLoads.data_[i].cons_current_Cimg != 0)  
   {
	   if (SpotLoads.data_[i].cons_current_Cimg<0)
	   {
	    fprintf(fp,"\n\t%s%f%f%s",
            "constant_current_C ",SpotLoads.data_[i].cons_current_Creal,SpotLoads.data_[i].cons_current_Cimg,"j;");
	   }
	   else
	   {
		fprintf(fp,"\n\t%s%f%s%f%s",
            "constant_current_C ",SpotLoads.data_[i].cons_current_Creal,"+",SpotLoads.data_[i].cons_current_Cimg,"j;");   
	   }
   }
   if (SpotLoads.data_[i].cons_power_Areal != 0 or SpotLoads.data_[i].cons_power_Aimg != 0)  
   {
	    fprintf(fp,"\n\t%s%f%s\n\t%s%f%s",
            "constant_power_A_real responsive_loads*",SpotLoads.data_[i].cons_power_Areal,";",
			"constant_power_A_reac responsive_loads*",SpotLoads.data_[i].cons_power_Aimg,";");
	  
   }
   if (SpotLoads.data_[i].cons_power_Breal != 0 or SpotLoads.data_[i].cons_power_Bimg != 0)  
   {
	    fprintf(fp,"\n\t%s%f%s\n\t%s%f%s",
            "constant_power_B_real responsive_loads*",SpotLoads.data_[i].cons_power_Breal,";",
			"constant_power_B_reac responsive_loads*",SpotLoads.data_[i].cons_power_Bimg,";");
	  
   }
   if (SpotLoads.data_[i].cons_power_Creal != 0 or SpotLoads.data_[i].cons_power_Cimg != 0)  
   {
	    fprintf(fp,"\n\t%s%f%s\n\t%s%f%s",
            "constant_power_C_real responsive_loads*",SpotLoads.data_[i].cons_power_Creal,";",
			"constant_power_C_reac responsive_loads*",SpotLoads.data_[i].cons_power_Cimg,";");
	  
   }
   if (SpotLoads.data_[i].cons_imped_Areal != 0 or SpotLoads.data_[i].cons_imped_Aimg != 0)  
   {
	   if (SpotLoads.data_[i].cons_imped_Aimg<0)
	   {
	    fprintf(fp,"\n\t%s%f%f%s",
            "constant_impedance_A ",SpotLoads.data_[i].cons_imped_Areal,SpotLoads.data_[i].cons_imped_Aimg,"j;");
	   }
	   else
	   {
		fprintf(fp,"\n\t%s%f%s%f%s",
            "constant_impedance_A ",SpotLoads.data_[i].cons_imped_Areal,"+",SpotLoads.data_[i].cons_imped_Aimg,"j;");   
	   }
   }
   if (SpotLoads.data_[i].cons_imped_Breal != 0 or SpotLoads.data_[i].cons_imped_Bimg != 0)  
   {
	   if (SpotLoads.data_[i].cons_imped_Bimg<0)
	   {
	    fprintf(fp,"\n\t%s%f%f%s",
            "constant_impedance_B ",SpotLoads.data_[i].cons_imped_Breal,SpotLoads.data_[i].cons_imped_Bimg,"j;");
	   }
	   else
	   {
		fprintf(fp,"\n\t%s%f%s%f%s",
            "constant_impedance_B ",SpotLoads.data_[i].cons_imped_Breal,"+",SpotLoads.data_[i].cons_imped_Bimg,"j;");   
	   }
   }
   if (SpotLoads.data_[i].cons_imped_Creal != 0 or SpotLoads.data_[i].cons_imped_Cimg != 0)  
   {
	   if (SpotLoads.data_[i].cons_imped_Cimg<0)
	   {
	    fprintf(fp,"\n\t%s%f%f%s",
            "constant_impedance_C ",SpotLoads.data_[i].cons_imped_Creal,SpotLoads.data_[i].cons_imped_Cimg,"j;");
	   }
	   else
	   {
		fprintf(fp,"\n\t%s%f%s%f%s",
            "constant_impedance_C ",SpotLoads.data_[i].cons_imped_Creal,"+",SpotLoads.data_[i].cons_imped_Cimg,"j;");   
	   }
   }
      fprintf(fp,"\n%s",
               "}"); 
}  
// Switches
for (int i=0;i<Switches.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n%s\n",
            "object switch {",
            "phases \"",Switches.data_[i].phases.c_str(),"\";",
            "name switch_",Switches.data_[i].name.c_str(),";",
			"from node_",Switches.data_[i].from_SW.c_str(),";",
			"to node_",Switches.data_[i].to_SW.c_str(),";",
			"status ",Switches.data_[i].status.c_str(),";",
            "}");   
}
// Capacitors
for (int i=0;i<Capacitors.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s",
            "object capacitor {",
            "phases ",Capacitors.data_[i].phases.c_str(),";",
            "name capacitor_",Capacitors.data_[i].name.c_str(),";",
			"phases_connected ",Capacitors.data_[i].phases.c_str(),";",
			"parent node_",Capacitors.data_[i].name.c_str(),";",
			"control ",Capacitors.data_[i].control.c_str(),";");
	if (Capacitors.data_[i].CapA != 0)  
   {
	    fprintf(fp,"\n\t%s%f%s",
            "capacitor_A ",Capacitors.data_[i].CapA," MVAr;");	   
   }
   if (Capacitors.data_[i].CapB != 0)  
   {
	    fprintf(fp,"\n\t%s%f%s",
            "capacitor_B ",Capacitors.data_[i].CapB," MVAr;");	   
   }
   if (Capacitors.data_[i].CapC != 0)  
   {
	    fprintf(fp,"\n\t%s%f%s",
            "capacitor_C ",Capacitors.data_[i].CapC," MVAr;");	   
   }
		fprintf(fp,"\n\t%s%s%s",
			"control_level ",Capacitors.data_[i].control_level.c_str(),";");
	if (Capacitors.data_[i].CapA != 0)  
   {
	    fprintf(fp,"\n\t%s%s%s",
            "switchA ",Capacitors.data_[i].swA.c_str(),";");	   
   }
   if (Capacitors.data_[i].CapB != 0)  
   {
	    fprintf(fp,"\n\t%s%s%s",
            "switchB ",Capacitors.data_[i].swB.c_str(),";");	   
   }
   if (Capacitors.data_[i].CapC != 0)  
   {
	    fprintf(fp,"\n\t%s%s%s",
            "switchC ",Capacitors.data_[i].swC.c_str(),";");	   
   }		
		fprintf(fp,"\n\t%s%f%s\n\%s\n",	
			"nominal_voltage ",Capacitors.data_[i].Vnominal,";",
            "}");   
}

// Transformer
for (int i=0;i<TransConfig.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%d%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n%s\n",
            "object transformer_configuration {",
			"name transformer_configuration_",TransConfig.data_[i].id.c_str(),";",
            "connect_type ",TransConfig.data_[i].connection.c_str(),";",
            "install_type ","PADMOUNT",";",
			"power_rating ",TransConfig.data_[i].rating,";",
			"primary_voltage ",TransConfig.data_[i].prim_V,";",
			"secondary_voltage ",TransConfig.data_[i].secd_V,";",
			"resistance ",TransConfig.data_[i].R,";",
			"reactance ",TransConfig.data_[i].X,";",
            "}");   
}

for (int i=0;i<Transformer.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n%s\n",
            "object transformer {",
            "phases \"",Transformer.data_[i].phases.c_str(),"\";",
            "name transformer_",Transformer.data_[i].name.c_str(),";",
			"from node_",Transformer.data_[i].from_XF.c_str(),";",
			"to node_",Transformer.data_[i].to_XF.c_str(),";",
			"configuration transformer_configuration_",Transformer.data_[i].config.c_str(),";",
            "}");   
}

// Regulators
for (int i=0;i<ReguConfig.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%d%s\n\t%s%d%s\n\t%s%d%s\n\t%s%d%s",
            "object regulator_configuration {",
			"name regulator_configuration_",ReguConfig.data_[i].id.c_str(),";",
            "connect_type ",ReguConfig.data_[i].connection.c_str(),";",
			"band_center ",ReguConfig.data_[i].bc,";",
			"band_width ",ReguConfig.data_[i].bw,";",
			"time_delay ",ReguConfig.data_[i].td,";",
			"raise_taps ",ReguConfig.data_[i].rt,";",
			"lower_taps ",ReguConfig.data_[i].lt,";",
			"current_transducer_ratio ",ReguConfig.data_[i].ct,";",
			"power_transducer_ratio ",ReguConfig.data_[i].pt,";");
	if (ReguConfig.data_[i].RA != 0 or ReguConfig.data_[i].XA != 0)  
   {
	    fprintf(fp,"\n\t%s%f%s\n\t%s%f%s",
            "compensator_r_setting_A ",ReguConfig.data_[i].RA,";",
			"compensator_x_setting_A ",ReguConfig.data_[i].XA,";");	   
   }
   if (ReguConfig.data_[i].RB != 0 or ReguConfig.data_[i].XB != 0)  
   {
	    fprintf(fp,"\n\t%s%f%s\n\t%s%f%s",
            "compensator_r_setting_B ",ReguConfig.data_[i].RB,";",
			"compensator_x_setting_B ",ReguConfig.data_[i].XB,";");	   
   }
   if (ReguConfig.data_[i].RC != 0 or ReguConfig.data_[i].XC != 0)  
   {
	    fprintf(fp,"\n\t%s%f%s\n\t%s%f%s",
            "compensator_r_setting_C ",ReguConfig.data_[i].RC,";",
			"compensator_x_setting_C ",ReguConfig.data_[i].XC,";");	   
   }	
		fprintf(fp,"\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s\n\t%s%s%s\n\t%s%s%s",
			"CT_phase \"",ReguConfig.data_[i].ct_phase.c_str(),"\";",
			"PT_phase \"",ReguConfig.data_[i].pt_phase.c_str(),"\";",
			"regulation ",ReguConfig.data_[i].reg,";",
			"Control ",ReguConfig.data_[i].control.c_str(),";",
			"Type ",ReguConfig.data_[i].Regtype.c_str(),";");
	if (ReguConfig.data_[i].RA != 0 or ReguConfig.data_[i].XA != 0)  
   {
	    fprintf(fp,"\n\t%s%d%s",
            "tap_pos_A ",ReguConfig.data_[i].tapA,";");	   
   }
   if (ReguConfig.data_[i].RB != 0 or ReguConfig.data_[i].XB != 0)  
   {
	    fprintf(fp,"\n\t%s%d%s",
            "tap_pos_B ",ReguConfig.data_[i].tapB,";");   
   }
   if (ReguConfig.data_[i].RC != 0 or ReguConfig.data_[i].XC != 0)  
   {
	    fprintf(fp,"\n\t%s%d%s",
            "tap_pos_C ",ReguConfig.data_[i].tapC,";");	   
   }		
        fprintf(fp,"\n%s",
            "}");   
}

for (int i=0;i<Regulator.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n%s\n",
            "object regulator {",
			"name regulator_",Regulator.data_[i].name.c_str(),";",
            "phases \"",Regulator.data_[i].phases.c_str(),"\";",
			"from node_",Regulator.data_[i].from_Reg.c_str(),";",
			"to node_",Regulator.data_[i].to_Reg.c_str(),";",
			"configuration regulator_configuration_",Regulator.data_[i].config.c_str(),";",
            "}");   
}





//****************Recorder**************************
 // voltage
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%d%s\n\t%s%s%s\n%s\n",
            "object group_recorder {",
            "name ","load_voltageA",";",
			"group ","\"class=load\"",";",
			"property ","voltage_A",";",
			"file ","/tmp/load_voltageA.csv",";",
			"interval ",900,";",
			"complex_part ","MAG",";",
            "}");
			
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%d%s\n\t%s%s%s\n%s\n",
            "object group_recorder {",
            "name ","load_voltageB",";",
			"group ","\"class=load\"",";",
			"property ","voltage_B",";",
			"file ","/tmp/load_voltageB.csv",";",
			"interval ",900,";",
			"complex_part ","MAG",";",
            "}");
			
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%d%s\n\t%s%s%s\n%s\n",
            "object group_recorder {",
            "name ","load_voltageC",";",
			"group ","\"class=load\"",";",
			"property ","voltage_C",";",
			"file ","/tmp/load_voltageC.csv",";",
			"interval ",900,";",
			"complex_part ","MAG",";",
            "}"); 
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%d%s\n\t%s%s%s\n%s\n",
            "object group_recorder {",
            "name ","node_voltageA",";",
			"group ","\"class=node\"",";",
			"property ","voltage_A",";",
			"file ","/tmp/node_voltageA.csv",";",
			"interval ",900,";",
			"complex_part ","MAG",";",
            "}");
			
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%d%s\n\t%s%s%s\n%s\n",
            "object group_recorder {",
            "name ","node_voltageB",";",
			"group ","\"class=node\"",";",
			"property ","voltage_B",";",
			"file ","/tmp/node_voltageB.csv",";",
			"interval ",900,";",
			"complex_part ","MAG",";",
            "}");
			
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%d%s\n\t%s%s%s\n%s\n",
            "object group_recorder {",
            "name ","node_voltageC",";",
			"group ","\"class=node\"",";",
			"property ","voltage_C",";",
			"file ","/tmp/node_voltageC.csv",";",
			"interval ",900,";",
			"complex_part ","MAG",";",
            "}");
/* //voltage dump
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n%s\n",
            "object voltdump {",
            "filename ","/tmp/output_voltage.csv",";",
			"mode ","polar",";",
            "}"); */

/* // current
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%d%s\n\t%s%s%s\n%s\n",
            "object group_recorder {",
            "name ","load_currentA",";",
			"group ","\"class=load\"",";",
			"property ","current_A",";",
			"file ","load_currentA.csv",";",
			"interval ",900,";",
			"complex_part ","MAG",";",
            "}");
			
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%d%s\n\t%s%s%s\n%s\n",
            "object group_recorder {",
            "name ","load_currentB",";",
			"group ","\"class=load\"",";",
			"property ","current_B",";",
			"file ","load_currentB.csv",";",
			"interval ",900,";",
			"complex_part ","MAG",";",
            "}");
			
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%d%s\n\t%s%s%s\n%s\n",
            "object group_recorder {",
            "name ","load_currentC",";",
			"group ","\"class=load\"",";",
			"property ","current_C",";",
			"file ","load_currentC.csv",";",
			"interval ",900,";",
			"complex_part ","MAG",";",
            "}");
// power
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%d%s\n%s\n",
            "object group_recorder {",
            "name ","load_powerA",";",
			"group ","\"class=load\"",";",
			"property ","constant_power_A_real",";",
			"file ","load_powerA.csv",";",
			"interval ",900,";",
            "}");
			
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%d%s\n%s\n",
            "object group_recorder {",
            "name ","load_powerB",";",
			"group ","\"class=load\"",";",
			"property ","constant_power_B_real",";",
			"file ","load_powerB.csv",";",
			"interval ",900,";",
            "}");
			
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%d%s\n%s\n",
            "object group_recorder {",
            "name ","load_powerC",";",
			"group ","\"class=load\"",";",
			"property ","constant_power_C_real",";",
			"file ","load_powerC.csv",";",
			"interval ",900,";",
            "}"); */







fclose(fp);

/* string file_name1="/tmp/appliance_schedules.glm";
FILE* fp1 = fopen(file_name1.c_str(), "w");
//****************appliance_schedules**************************
// responsive_loads
for (int i=0;i<Schedules.size();i++)
{
    fprintf(fp1,"\n%s\n%s\n%s\n%s\n",
            "schedule responsive_loads {",
		    "{",
		    "*"
        

fclose(fp1);*/
return 0;
} 



//******************END of Functions***************************************************************************

//************************************************************************************************************
//Function for generating GLM file for GridLAB-D simulation
//Developed by Jun Tan, juntan@geirina.net************************
				// template <typename powerflowtup, typename nodetup, typename loadtup, typename linetup, typename ohlconfigtup, typename uglconfigtup, typename ohlcondconfigtup, typename uglcondconfigtup, typename lsconfigtup, typename trans_configtup, typename transformertup, typename regu_configtup,typename regulatortup, typename switchtup, typename capacitortup,typename tripNodeTup,typename tripMeterTup,typename tripLineTup,typename tripLineConfigTup typename houseTup,typename meterTup,typename solarTup,typename windTup,typename batteryTup,typename inverterTup,typename XFTup,typename XFCTup>
				// inline int GLM(ListAccum<powerflowtup>& PowerFlow, ListAccum<nodetup>& Nodes, ListAccum<loadtup>& SpotLoads, ListAccum<linetup>& Lines, ListAccum<ohlconfigtup>& OHLConfig, ListAccum<uglconfigtup>& UGLConfig, ListAccum<ohlcondconfigtup>& OHLCondConfig, ListAccum<uglcondconfigtup>& UGLCondConfig, ListAccum<lsconfigtup>& LSConfig, ListAccum<trans_configtup>& TransConfig, ListAccum<transformertup>& Transformer, ListAccum<regu_configtup>& ReguConfig, ListAccum<regulatortup>& Regulator, ListAccum<switchtup>& Switches, ListAccum<capacitortup>& Capacitors,ListAccum<tripNodeTup>& tripNode,ListAccum<tripMeterTup>& tripMeter, ListAccum<tripLineTup>& tripLine,ListAccum<tripLineConfigTup>$ tripLineConfig,ListAccum<houseTup>& house,ListAccum<meterTup>& meter,ListAccum<solarTup>& solar,ListAccum<windTup>& wind, ListAccum<batteryTup>& battery, ListAccum<inverterTup>& inverter,  ListAccum<XFTup>& XF, ListAccum<XFCTup>& XFC )
template <typename powerflowtup, typename nodetup, typename loadtup, typename linetup, typename ohlconfigtup, typename uglconfigtup, typename ohlcondconfigtup, typename uglcondconfigtup, typename lsconfigtup, typename trans_configtup, typename transformertup, typename regu_configtup,typename regulatortup, typename switchtup, typename capacitortup,typename tripNodeTup,typename tripMeterTup,typename tripLineTup,typename tripLineConfigTup, typename houseTup,typename waterhTup,typename meterTup,typename solarTup,typename windTup,typename batteryTup,typename inverterTup,typename XFTup,typename XFCTup,typename XFresTup,typename XFCresTup>
inline int GLM_Q(ListAccum<powerflowtup>& PowerFlow, ListAccum<nodetup>& Nodes, ListAccum<loadtup>& SpotLoads, ListAccum<linetup>& Lines, ListAccum<ohlconfigtup>& OHLConfig, ListAccum<uglconfigtup>& UGLConfig, ListAccum<ohlcondconfigtup>& OHLCondConfig, ListAccum<uglcondconfigtup>& UGLCondConfig, ListAccum<lsconfigtup>& LSConfig, ListAccum<trans_configtup>& TransConfig, ListAccum<transformertup>& Transformer, ListAccum<regu_configtup>& ReguConfig, ListAccum<regulatortup>& Regulator, ListAccum<switchtup>& Switches, ListAccum<capacitortup>& Capacitors,ListAccum<tripNodeTup>& tripNode,ListAccum<tripMeterTup>& tripMeter, ListAccum<tripLineTup>& tripLine,ListAccum<tripLineConfigTup>& tripLineConfig,ListAccum<houseTup>& house,ListAccum<waterhTup>& waterH,ListAccum<meterTup>& meter,ListAccum<solarTup>& solar,ListAccum<windTup>& wind, ListAccum<batteryTup>& battery, ListAccum<inverterTup>& inverter,  ListAccum<XFTup>& XF, ListAccum<XFCTup>& XFC ,  ListAccum<XFresTup>& XFres, ListAccum<XFCresTup>& XFCres )				

				
{
//sort ListAccum 
    struct ListAccum_sort_OHLCondConfig{
            inline bool operator() (const ohlcondconfigtup& tuple1, const ohlcondconfigtup& tuple2)
            {
                return (tuple1.ohlcondconfig_id < tuple2.ohlcondconfig_id);
            }
	};
	struct ListAccum_sort_UGLCondConfig{
            inline bool operator() (const uglcondconfigtup& tuple1, const uglcondconfigtup& tuple2)
            {
                return (tuple1.uglcondconfig_id < tuple2.uglcondconfig_id);
            }
	};
	struct ListAccum_sort_LSConfig{
            inline bool operator() (const lsconfigtup& tuple1, const lsconfigtup& tuple2)
            {
                return (tuple1.linespacing_id < tuple2.linespacing_id);
            }
	};
	struct ListAccum_sort_OHLConfig{
            inline bool operator() (const ohlconfigtup& tuple1, const ohlconfigtup& tuple2)
            {
                return (tuple1.ohlconfig_id < tuple2.ohlconfig_id);
            }
	};
	struct ListAccum_sort_UGLConfig{
            inline bool operator() (const uglconfigtup& tuple1, const uglconfigtup& tuple2)
            {
                return (tuple1.uglconfig_id < tuple2.uglconfig_id);
            }
	};
    struct ListAccum_sort_nodes{
            inline bool operator() (const nodetup& tuple1, const nodetup& tuple2)
            {
                return (tuple1.name < tuple2.name);
            }
	};
	struct ListAccum_sort_loads{
            inline bool operator() (const loadtup& tuple1, const loadtup& tuple2)
            {
                return (tuple1.name < tuple2.name);
            }
	};
	struct ListAccum_sort_lines{
            inline bool operator() (const linetup& tuple1, const linetup& tuple2)
            {
                return (tuple1.name < tuple2.name);
            }
	};
    
	
	std::sort(OHLCondConfig.begin(), OHLCondConfig.end(), ListAccum_sort_OHLCondConfig());
	std::sort(UGLCondConfig.begin(), UGLCondConfig.end(), ListAccum_sort_UGLCondConfig());
	std::sort(LSConfig.begin(), LSConfig.end(), ListAccum_sort_LSConfig());
	std::sort(OHLConfig.begin(), OHLConfig.end(), ListAccum_sort_OHLConfig());
	std::sort(UGLConfig.begin(), UGLConfig.end(), ListAccum_sort_UGLConfig());
	std::sort(Nodes.begin(), Nodes.end(), ListAccum_sort_nodes());
	std::sort(SpotLoads.begin(), SpotLoads.end(), ListAccum_sort_loads());
	std::sort(Lines.begin(), Lines.end(), ListAccum_sort_lines());
	
//start writing GLM 
string file_name="/tmp/123nodetest.glm";
FILE* fp = fopen(file_name.c_str(), "w");
//setting
fprintf(fp,"\n%s\n%s\n%s\n%s\n%s\n",
        "#set profiler=1;",
        "#set pauseatexit=1;",
        "#set iteration_limit=1000;",
		"//#set server_portnum=6267;",
		"#include \"/tmp/appliance_schedules.glm\";"
		);
		
fprintf(fp,"\n%s%s%s",
        "//realstoptime ",PowerFlow.data_[0].stoptime.c_str(),";");	 
//time
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n%s\n",
       "clock {",
       "starttime '",PowerFlow.data_[0].starttime.c_str(),"';",
       "stoptime '",PowerFlow.data_[0].fakestoptime.c_str(),"';",
       "};");

//simulation step

fprintf(fp,"\nmodule tape;\n");
fprintf(fp,"\nmodule climate;\n");
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n%s\n",
       "object climate{",
       "tmyfile ","./WA-Yakima.tmy2",";",
       "name ","Yakima",";",
       "};");
fprintf(fp,"\nmodule residential{ANSI_voltage_check FALSE;};\n");
fprintf(fp,"\nmodule generators;;\n");
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s\n%s\n",
       "module powerflow {",
       "solver_method ",PowerFlow.data_[0].solver.c_str(),";",
       "default_maximum_voltage_error 1e-6;",
       "};");


//Phase Conductor for 1 thru 11:
for (int i=0;i<OHLCondConfig.size();i++)
{
   fprintf(fp,"\n%s%d%s\n\t%s%f%s\n\t%s%f%s\n%s\n",
            "object overhead_line_conductor:",OHLCondConfig.data_[i].ohlcondconfig_id," {",
            "geometric_mean_radius ",OHLCondConfig.data_[i].conductor_gmr,";",
            "resistance ",OHLCondConfig.data_[i].conductor_resistance,";",
            "}");  
}
// Phase Conductor for 12: 1/0 AA,TS
for (int i=0;i<UGLCondConfig.size();i++)
{
   fprintf(fp,"\n%s%d%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n%s\n",
            "object underground_line_conductor:",UGLCondConfig.data_[i].uglcondconfig_id," {",
            "outer_diameter ",UGLCondConfig.data_[i].outer_diameter_ugl,";",
            "conductor_gmr ",UGLCondConfig.data_[i].conductor_gmr,";",
			"conductor_diameter ",UGLCondConfig.data_[i].conductor_diameter_ugl,";",
			"conductor_resistance ",UGLCondConfig.data_[i].conductor_resistance,";",
			"neutral_gmr ",UGLCondConfig.data_[i].neutral_gmr_ugl,";",
			"neutral_resistance ",UGLCondConfig.data_[i].neutral_resistance_ugl,";",
			"neutral_diameter ",UGLCondConfig.data_[i].neutral_diameter_ugl,";",
			"neutral_strands ",UGLCondConfig.data_[i].neutral_strands_ugl,";",
			"shield_gmr ",UGLCondConfig.data_[i].shield_gmr_ugl,";",
			"shield_resistance ",UGLCondConfig.data_[i].shield_resistance_ugl,";",
            "}");  
}
// Overhead line configurations
// ABCN
for (int i=0;i<LSConfig.size();i++)
{
   fprintf(fp,"\n%s%d%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n%s\n",
            "object line_spacing:",LSConfig.data_[i].linespacing_id," {",
            "distance_AB ",LSConfig.data_[i].distance_AB,";",
            "distance_AC ",LSConfig.data_[i].distance_AC,";",
			"distance_BC ",LSConfig.data_[i].distance_BC,";",
			"distance_AN ",LSConfig.data_[i].distance_AN,";",
			"distance_BN ",LSConfig.data_[i].distance_BN,";",
			"distance_CN ",LSConfig.data_[i].distance_CN,";",
            "}");  
}

for (int i=0;i<OHLConfig.size();i++)
{
   fprintf(fp,"\n%s%d%s",
            "object line_configuration:",OHLConfig.data_[i].ohlconfig_id," {");
   if (OHLConfig.data_[i].Raa != 0 or OHLConfig.data_[i].Rbb != 0 or OHLConfig.data_[i].Rcc != 0)
   {
	   fprintf(fp,"\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n%s\n",
            "z11 ",OHLConfig.data_[i].Raa,"+",OHLConfig.data_[i].Xaa,"j;",
			"z12 ",OHLConfig.data_[i].Rab,"+",OHLConfig.data_[i].Xab,"j;",
			"z13 ",OHLConfig.data_[i].Rac,"+",OHLConfig.data_[i].Xac,"j;",
			"z21 ",OHLConfig.data_[i].Rba,"+",OHLConfig.data_[i].Xba,"j;",
			"z22 ",OHLConfig.data_[i].Rbb,"+",OHLConfig.data_[i].Xbb,"j;",
			"z23 ",OHLConfig.data_[i].Rbc,"+",OHLConfig.data_[i].Xbc,"j;",
			"z31 ",OHLConfig.data_[i].Rca,"+",OHLConfig.data_[i].Xca,"j;",
			"z32 ",OHLConfig.data_[i].Rcb,"+",OHLConfig.data_[i].Xcb,"j;",
			"z33 ",OHLConfig.data_[i].Rcc,"+",OHLConfig.data_[i].Xcc,"j;",
			"}");
   }
   else
   {
	if (OHLConfig.data_[i].conductor_A != 0)
	{
		fprintf(fp,"\n\t%s%d%s",
            "conductor_A overhead_line_conductor:",OHLConfig.data_[i].conductor_A,";");
	}
	if (OHLConfig.data_[i].conductor_B != 0)
	{
		fprintf(fp,"\n\t%s%d%s",
            "conductor_B overhead_line_conductor:",OHLConfig.data_[i].conductor_B,";");
	}
	if (OHLConfig.data_[i].conductor_C != 0)
	{
		fprintf(fp,"\n\t%s%d%s",
            "conductor_C overhead_line_conductor:",OHLConfig.data_[i].conductor_C,";");
	}
	if (OHLConfig.data_[i].conductor_N != 0)
	{
		fprintf(fp,"\n\t%s%d%s",
            "conductor_N overhead_line_conductor:",OHLConfig.data_[i].conductor_N,";");
	}
			
        fprintf(fp,"\n\t%s%d%s\n%s\n",
			"spacing line_spacing:",OHLConfig.data_[i].line_spacing,";",
            "}"); 
   }			
}

for (int i=0;i<UGLConfig.size();i++)
{
   fprintf(fp,"\n%s%d%s",
            "object line_configuration:",UGLConfig.data_[i].uglconfig_id," {");
	if (UGLConfig.data_[i].Raa != 0 or UGLConfig.data_[i].Rbb != 0 or UGLConfig.data_[i].Rcc != 0)
   {
	   fprintf(fp,"\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n\t%s%f%s%f%s\n%s\n",
            "z11 ",UGLConfig.data_[i].Raa,"+",UGLConfig.data_[i].Xaa,"j;",
			"z12 ",UGLConfig.data_[i].Rab,"+",UGLConfig.data_[i].Xab,"j;",
			"z13 ",UGLConfig.data_[i].Rac,"+",UGLConfig.data_[i].Xac,"j;",
			"z21 ",UGLConfig.data_[i].Rba,"+",UGLConfig.data_[i].Xba,"j;",
			"z22 ",UGLConfig.data_[i].Rbb,"+",UGLConfig.data_[i].Xbb,"j;",
			"z23 ",UGLConfig.data_[i].Rbc,"+",UGLConfig.data_[i].Xbc,"j;",
			"z31 ",UGLConfig.data_[i].Rca,"+",UGLConfig.data_[i].Xca,"j;",
			"z32 ",UGLConfig.data_[i].Rcb,"+",UGLConfig.data_[i].Xcb,"j;",
			"z33 ",UGLConfig.data_[i].Rcc,"+",UGLConfig.data_[i].Xcc,"j;",
			"}");
   }
   else
   {
	   fprintf(fp,"\n\t%s%d%s\n\t%s%d%s\n\t%s%d%s\n\t%s%d%s\n%s\n",
            "conductor_A underground_line_conductor:",UGLConfig.data_[i].conductor_A,";",
            "conductor_B underground_line_conductor:",UGLConfig.data_[i].conductor_B,";",
			"conductor_C underground_line_conductor:",UGLConfig.data_[i].conductor_C,";",
			"spacing line_spacing:",UGLConfig.data_[i].line_spacing,";",
            "}");
   }			
}
//Define line objects 
for (int i=0;i<Lines.size();i++)
{   
   if (Lines.data_[i].linetype == 0)
   {
	fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s\n\t%s%d%s\n%s\n",
            "object overhead_line: {",
            "phases \"",Lines.data_[i].phases.c_str(),"\";",
            "name overhead_line_",Lines.data_[i].name.c_str(),";",
			"from node_",Lines.data_[i].from_node.c_str(),";",
			"to node_",Lines.data_[i].to_node.c_str(),";",
			"length ",Lines.data_[i].length,";",
			"configuration line_configuration:",Lines.data_[i].config,";",
            "}");  
    }
    else if (Lines.data_[i].linetype == 1)
	{
     fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s\n\t%s%d%s\n%s\n",
            "object underground_line: {",
            "phases \"",Lines.data_[i].phases.c_str(),"\";",
            "name underground_line_",Lines.data_[i].name.c_str(),";",
			"from node_",Lines.data_[i].from_node.c_str(),";",
			"to node_",Lines.data_[i].to_node.c_str(),";",
			"length ",Lines.data_[i].length,";",
			"configuration line_configuration:",Lines.data_[i].config,";",
            "}"); 
	}
}	
//Create node only objects 
for (int i=0;i<Nodes.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s%f%s\n\t%s%f%f%s\n\t%s%f%s%f%s\n\t%s%f%s\n%s\n",
            "object node {",
            "phases \"",Nodes.data_[i].phases.c_str(),"\";",
            "name node_",Nodes.data_[i].name.c_str(),";",
			"voltage_A ",Nodes.data_[i].VA_real,"+",Nodes.data_[i].VA_img,"j;",
			"voltage_B ",Nodes.data_[i].VB_real,Nodes.data_[i].VB_img,"j;",
			"voltage_C ",Nodes.data_[i].VC_real,"+",Nodes.data_[i].VC_img,"j;",
			"nominal_voltage ",Nodes.data_[i].V_nominal,";",
            "}");   
}
//Create load objects
for (int i=0;i<SpotLoads.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s%f%s\n\t%s%f%f%s\n\t%s%f%s%f%s\n\t%s%f%s",
            "object load {",
            "phases \"",SpotLoads.data_[i].phases.c_str(),"\";",
            "name node_",SpotLoads.data_[i].name.c_str(),";",
			"voltage_A ",SpotLoads.data_[i].VA_real,"+",SpotLoads.data_[i].VA_img,"j;",
			"voltage_B ",SpotLoads.data_[i].VB_real,SpotLoads.data_[i].VB_img,"j;",
			"voltage_C ",SpotLoads.data_[i].VC_real,"+",SpotLoads.data_[i].VC_img,"j;",
			"nominal_voltage ",SpotLoads.data_[i].V_nominal,";");           
   if (SpotLoads.data_[i].cons_current_Areal != 0 or SpotLoads.data_[i].cons_current_Aimg != 0)  
   {
	   if (SpotLoads.data_[i].cons_current_Aimg<0)
	   {
	    fprintf(fp,"\n\t%s%f%f%s",
            "constant_current_A ",SpotLoads.data_[i].cons_current_Areal,SpotLoads.data_[i].cons_current_Aimg,"j;");
	   }
	   else
	   {
		fprintf(fp,"\n\t%s%f%s%f%s",
            "constant_current_A ",SpotLoads.data_[i].cons_current_Areal,"+",SpotLoads.data_[i].cons_current_Aimg,"j;");   
	   }
   }
   if (SpotLoads.data_[i].cons_current_Breal != 0 or SpotLoads.data_[i].cons_current_Bimg != 0)  
   {
	   if (SpotLoads.data_[i].cons_current_Bimg<0)
	   {
	    fprintf(fp,"\n\t%s%f%f%s",
            "constant_current_B ",SpotLoads.data_[i].cons_current_Breal,SpotLoads.data_[i].cons_current_Bimg,"j;");
	   }
	   else
	   {
		fprintf(fp,"\n\t%s%f%s%f%s",
            "constant_current_B ",SpotLoads.data_[i].cons_current_Breal,"+",SpotLoads.data_[i].cons_current_Bimg,"j;");   
	   }
   }
   if (SpotLoads.data_[i].cons_current_Creal != 0 or SpotLoads.data_[i].cons_current_Cimg != 0)  
   {
	   if (SpotLoads.data_[i].cons_current_Cimg<0)
	   {
	    fprintf(fp,"\n\t%s%f%f%s",
            "constant_current_C ",SpotLoads.data_[i].cons_current_Creal,SpotLoads.data_[i].cons_current_Cimg,"j;");
	   }
	   else
	   {
		fprintf(fp,"\n\t%s%f%s%f%s",
            "constant_current_C ",SpotLoads.data_[i].cons_current_Creal,"+",SpotLoads.data_[i].cons_current_Cimg,"j;");   
	   }
   }
   if (SpotLoads.data_[i].cons_power_Areal != 0 or SpotLoads.data_[i].cons_power_Aimg != 0)  
   {
	    fprintf(fp,"\n\t%s%f%s\n\t%s%f%s",
            "constant_power_A_real responsive_loads*",SpotLoads.data_[i].cons_power_Areal,";",
			"constant_power_A_reac responsive_loads*",SpotLoads.data_[i].cons_power_Aimg,";");
	  
   }
   if (SpotLoads.data_[i].cons_power_Breal != 0 or SpotLoads.data_[i].cons_power_Bimg != 0)  
   {
	    fprintf(fp,"\n\t%s%f%s\n\t%s%f%s",
            "constant_power_B_real responsive_loads*",SpotLoads.data_[i].cons_power_Breal,";",
			"constant_power_B_reac responsive_loads*",SpotLoads.data_[i].cons_power_Bimg,";");
	  
   }
   if (SpotLoads.data_[i].cons_power_Creal != 0 or SpotLoads.data_[i].cons_power_Cimg != 0)  
   {
	    fprintf(fp,"\n\t%s%f%s\n\t%s%f%s",
            "constant_power_C_real responsive_loads*",SpotLoads.data_[i].cons_power_Creal,";",
			"constant_power_C_reac responsive_loads*",SpotLoads.data_[i].cons_power_Cimg,";");
	  
   }
   if (SpotLoads.data_[i].cons_imped_Areal != 0 or SpotLoads.data_[i].cons_imped_Aimg != 0)  
   {
	   if (SpotLoads.data_[i].cons_imped_Aimg<0)
	   {
	    fprintf(fp,"\n\t%s%f%f%s",
            "constant_impedance_A ",SpotLoads.data_[i].cons_imped_Areal,SpotLoads.data_[i].cons_imped_Aimg,"j;");
	   }
	   else
	   {
		fprintf(fp,"\n\t%s%f%s%f%s",
            "constant_impedance_A ",SpotLoads.data_[i].cons_imped_Areal,"+",SpotLoads.data_[i].cons_imped_Aimg,"j;");   
	   }
   }
   if (SpotLoads.data_[i].cons_imped_Breal != 0 or SpotLoads.data_[i].cons_imped_Bimg != 0)  
   {
	   if (SpotLoads.data_[i].cons_imped_Bimg<0)
	   {
	    fprintf(fp,"\n\t%s%f%f%s",
            "constant_impedance_B ",SpotLoads.data_[i].cons_imped_Breal,SpotLoads.data_[i].cons_imped_Bimg,"j;");
	   }
	   else
	   {
		fprintf(fp,"\n\t%s%f%s%f%s",
            "constant_impedance_B ",SpotLoads.data_[i].cons_imped_Breal,"+",SpotLoads.data_[i].cons_imped_Bimg,"j;");   
	   }
   }
   if (SpotLoads.data_[i].cons_imped_Creal != 0 or SpotLoads.data_[i].cons_imped_Cimg != 0)  
   {
	   if (SpotLoads.data_[i].cons_imped_Cimg<0)
	   {
	    fprintf(fp,"\n\t%s%f%f%s",
            "constant_impedance_C ",SpotLoads.data_[i].cons_imped_Creal,SpotLoads.data_[i].cons_imped_Cimg,"j;");
	   }
	   else
	   {
		fprintf(fp,"\n\t%s%f%s%f%s",
            "constant_impedance_C ",SpotLoads.data_[i].cons_imped_Creal,"+",SpotLoads.data_[i].cons_imped_Cimg,"j;");   
	   }
   }
      fprintf(fp,"\n%s",
               "}"); 
}  
// Switches
for (int i=0;i<Switches.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n%s\n",
            "object switch {",
            "phases \"",Switches.data_[i].phases.c_str(),"\";",
            "name switch_",Switches.data_[i].name.c_str(),";",
			"from node_",Switches.data_[i].from_SW.c_str(),";",
			"to node_",Switches.data_[i].to_SW.c_str(),";",
			"status ",Switches.data_[i].status.c_str(),";",
            "}");   
}
// Capacitors
for (int i=0;i<Capacitors.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s",
            "object capacitor {",
            "phases ",Capacitors.data_[i].phases.c_str(),";",
            "name capacitor_",Capacitors.data_[i].name.c_str(),";",
			"phases_connected ",Capacitors.data_[i].phases.c_str(),";",
			"parent node_",Capacitors.data_[i].name.c_str(),";",
			"control ",Capacitors.data_[i].control.c_str(),";");
	if (Capacitors.data_[i].CapA != 0)  
   {
	    fprintf(fp,"\n\t%s%f%s",
            "capacitor_A ",Capacitors.data_[i].CapA," MVAr;");	   
   }
   if (Capacitors.data_[i].CapB != 0)  
   {
	    fprintf(fp,"\n\t%s%f%s",
            "capacitor_B ",Capacitors.data_[i].CapB," MVAr;");	   
   }
   if (Capacitors.data_[i].CapC != 0)  
   {
	    fprintf(fp,"\n\t%s%f%s",
            "capacitor_C ",Capacitors.data_[i].CapC," MVAr;");	   
   }
		fprintf(fp,"\n\t%s%s%s",
			"control_level ",Capacitors.data_[i].control_level.c_str(),";");
	if (Capacitors.data_[i].CapA != 0)  
   {
	    fprintf(fp,"\n\t%s%s%s",
            "switchA ",Capacitors.data_[i].swA.c_str(),";");	   
   }
   if (Capacitors.data_[i].CapB != 0)  
   {
	    fprintf(fp,"\n\t%s%s%s",
            "switchB ",Capacitors.data_[i].swB.c_str(),";");	   
   }
   if (Capacitors.data_[i].CapC != 0)  
   {
	    fprintf(fp,"\n\t%s%s%s",
            "switchC ",Capacitors.data_[i].swC.c_str(),";");	   
   }		
		fprintf(fp,"\n\t%s%f%s\n\%s\n",	
			"nominal_voltage ",Capacitors.data_[i].Vnominal,";",
            "}");   
}

// Transformer
for (int i=0;i<TransConfig.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%d%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n%s\n",
            "object transformer_configuration {",
			"name transformer_configuration_",TransConfig.data_[i].id.c_str(),";",
            "connect_type ",TransConfig.data_[i].connection.c_str(),";",
            "install_type ","PADMOUNT",";",
			"power_rating ",TransConfig.data_[i].rating,";",
			"primary_voltage ",TransConfig.data_[i].prim_V,";",
			"secondary_voltage ",TransConfig.data_[i].secd_V,";",
			"resistance ",TransConfig.data_[i].R,";",
			"reactance ",TransConfig.data_[i].X,";",
            "}");   
}

for (int i=0;i<Transformer.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n%s\n",
            "object transformer {",
            "phases \"",Transformer.data_[i].phases.c_str(),"\";",
            "name transformer_",Transformer.data_[i].name.c_str(),";",
			"from node_",Transformer.data_[i].from_XF.c_str(),";",
			"to node_",Transformer.data_[i].to_XF.c_str(),";",
			"configuration transformer_configuration_",Transformer.data_[i].config.c_str(),";",
            "}");   
}

// Regulators
for (int i=0;i<ReguConfig.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%d%s\n\t%s%d%s\n\t%s%d%s\n\t%s%d%s",
            "object regulator_configuration {",
			"name regulator_configuration_",ReguConfig.data_[i].id.c_str(),";",
            "connect_type ",ReguConfig.data_[i].connection.c_str(),";",
			"band_center ",ReguConfig.data_[i].bc,";",
			"band_width ",ReguConfig.data_[i].bw,";",
			"time_delay ",ReguConfig.data_[i].td,";",
			"raise_taps ",ReguConfig.data_[i].rt,";",
			"lower_taps ",ReguConfig.data_[i].lt,";",
			"current_transducer_ratio ",ReguConfig.data_[i].ct,";",
			"power_transducer_ratio ",ReguConfig.data_[i].pt,";");
	if (ReguConfig.data_[i].RA != 0 or ReguConfig.data_[i].XA != 0)  
   {
	    fprintf(fp,"\n\t%s%f%s\n\t%s%f%s",
            "compensator_r_setting_A ",ReguConfig.data_[i].RA,";",
			"compensator_x_setting_A ",ReguConfig.data_[i].XA,";");	   
   }
   if (ReguConfig.data_[i].RB != 0 or ReguConfig.data_[i].XB != 0)  
   {
	    fprintf(fp,"\n\t%s%f%s\n\t%s%f%s",
            "compensator_r_setting_B ",ReguConfig.data_[i].RB,";",
			"compensator_x_setting_B ",ReguConfig.data_[i].XB,";");	   
   }
   if (ReguConfig.data_[i].RC != 0 or ReguConfig.data_[i].XC != 0)  
   {
	    fprintf(fp,"\n\t%s%f%s\n\t%s%f%s",
            "compensator_r_setting_C ",ReguConfig.data_[i].RC,";",
			"compensator_x_setting_C ",ReguConfig.data_[i].XC,";");	   
   }	
		fprintf(fp,"\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s\n\t%s%s%s\n\t%s%s%s",
			"CT_phase \"",ReguConfig.data_[i].ct_phase.c_str(),"\";",
			"PT_phase \"",ReguConfig.data_[i].pt_phase.c_str(),"\";",
			"regulation ",ReguConfig.data_[i].reg,";",
			"Control ",ReguConfig.data_[i].control.c_str(),";",
			"Type ",ReguConfig.data_[i].Regtype.c_str(),";");
	if (ReguConfig.data_[i].RA != 0 or ReguConfig.data_[i].XA != 0)  
   {
	    fprintf(fp,"\n\t%s%d%s",
            "tap_pos_A ",ReguConfig.data_[i].tapA,";");	   
   }
   if (ReguConfig.data_[i].RB != 0 or ReguConfig.data_[i].XB != 0)  
   {
	    fprintf(fp,"\n\t%s%d%s",
            "tap_pos_B ",ReguConfig.data_[i].tapB,";");   
   }
   if (ReguConfig.data_[i].RC != 0 or ReguConfig.data_[i].XC != 0)  
   {
	    fprintf(fp,"\n\t%s%d%s",
            "tap_pos_C ",ReguConfig.data_[i].tapC,";");	   
   }		
        fprintf(fp,"\n%s",
            "}");   
};

for (int i=0;i<Regulator.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n%s\n",
            "object regulator {",
			"name regulator_",Regulator.data_[i].name.c_str(),";",
            "phases \"",Regulator.data_[i].phases.c_str(),"\";",
			"from node_",Regulator.data_[i].from_Reg.c_str(),";",
			"to node_",Regulator.data_[i].to_Reg.c_str(),";",
			"configuration regulator_configuration_",Regulator.data_[i].config.c_str(),";",
            "}");   
};

// triplex_node, triplex_line,triplex_meter, House




   // fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%d%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n%s\n",
            // "object transformer_configuration {",
			// "name transformer_configuration_",TransConfig.data_[i].id.c_str(),";",
            // "connect_type ",TransConfig.data_[i].connection.c_str(),";",
            // "install_type ","PADMOUNT",";",
			// "power_rating ",TransConfig.data_[i].rating,";",
			// "primary_voltage ",TransConfig.data_[i].prim_V,";",
			// "secondary_voltage ",TransConfig.data_[i].secd_V,";",
			// "resistance ",TransConfig.data_[i].R,";",
			// "reactance ",TransConfig.data_[i].X,";",
            // "}"); 

for (int i=0;i<tripNode.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s\n%s\n",
            "object triplex_node {",
            "phases \"",tripNode.data_[i].triplex_node_phase.c_str(),"\";",
            "name tripNode_",house.data_[i].house_name.c_str(),";",
			"nominal_voltage ",tripNode.data_[i].triplex_node_nominal_voltage,";",
            "}");   
};

for (int i=0;i<tripMeter.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s\n%s\n",
            "object triplex_meter {",
            "phases \"",house.data_[i].phase.c_str(),"\";",
            "name tripMeter_",house.data_[i].house_name.c_str(),";",
            "nominal_voltage ",tripNode.data_[i].triplex_node_nominal_voltage,";",
            "}");   
};

for (int i=0;i<tripLine.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s\n\t%s%s%s\n%s\n",
            "object triplex_line {",
            "phases \"",tripLine.data_[i].triplex_line_phase.c_str(),"\";",
            "name tripLine_",house.data_[i].house_name.c_str(),";",
            "from tripNode_",house.data_[i].house_name.c_str(),";",
			"to tripMeter_",house.data_[i].house_name.c_str(),";",
			"length ",tripLine.data_[i].triplex_line_length,";",
			"configuration ",tripLine.data_[i].config.c_str(),";",
            "}");   
};

for (int i=0;i<tripLineConfig.size();i++)
{
	  fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%f%s\n\t%s%f%s\n%s\n",
            "object triplex_line_conductor {",
            "name tripconductor",house.data_[i].house_name.c_str(),";",
			"resistance ",tripLineConfig.data_[i].resistance,";",
			"geometric_mean_radius ",tripLineConfig.data_[i].geometric_mean_radius,";",
            "}");   
}
for (int i=0;i<tripLineConfig.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%f%s\n\t%s%f%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n%s\n",
            "object triplex_line_configuration {",
            "name tripconfig",house.data_[i].house_name.c_str(),";",
            "insulation_thickness ",tripLineConfig.data_[i].insulation_thickness,";",
			"diameter ",tripLineConfig.data_[i].diameter,";",
			"conductor_1 tripconductor",house.data_[i].house_name.c_str(),";",
			"conductor_2 tripconductor",house.data_[i].house_name.c_str(),";",
			"conductor_N tripconductor",house.data_[i].house_name.c_str(),";",
            "}");   
};



for (int i=0;i<house.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%s%s\n%s\n",
            "object house {",
            "name ",house.data_[i].house_name.c_str(),";",
            "floor_area ",house.data_[i].floor_area,";",
			"heating_setpoint ",house.data_[i].heating_setpoint,";",
			"cooling_setpoint ",house.data_[i].cooling_setpoint,";",
			"parent tripMeter_",house.data_[i].house_name.c_str(),";",
            "}");   
};

for (int i=0;i<waterH.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s\n%s\n",
            "object waterheater {",
            "name waterheater_",house.data_[i].house_name.c_str(),";",
			"parent ",house.data_[i].house_name.c_str(),";",
            "tank_volume ",waterH.data_[i].waterH_tank_volume,";",
			"tank_UA ",waterH.data_[i].waterH_tank_UA,";",
			"tank_diameter ",waterH.data_[i].waterH_tank_diameter,";",
			"heating_element_capacity ",waterH.data_[i].waterH_heating_element_capacity,";",
			"heat_mode ",waterH.data_[i].waterH_heat_mode.c_str(),";",
			"location ",waterH.data_[i].waterH_location.c_str(),";",
			"tank_setpoint ",waterH.data_[i].waterH_tank_setpoint,";",
            "}");   
};

//Renewable
for (int i=0;i<meter.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s\n%s\n",
            "object meter {",
            "name \"",meter.data_[i].name.c_str(),"\";",
            "phases ",meter.data_[i].phase.c_str(),";",
            "nominal_voltage ",meter.data_[i].nominal_voltage,";",
            "}");   
};




for (int i=0;i<inverter.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s\n\t%s%s%s\n\t%s%f%s\n\t%s%s%s\n%s\n",
            "object inverter {",
            "parent \"",inverter.data_[i].parent.c_str(),"\";",
            "generator_mode ",inverter.data_[i].generator_mode.c_str(),";",
            "generator_status ",inverter.data_[i].generator_status.c_str(),";",
			"inverter_type ",inverter.data_[i].inverter_type.c_str(),";",
			"power_factor ",inverter.data_[i].power_factor,";",
			"phases  ",inverter.data_[i].phases.c_str(),";",
			"rated_power  ",inverter.data_[i].rated_power,";",
			"name inverter_",solar.data_[i].name.c_str(),";",
            "}");   
};

for (int i=0;i<solar.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s\n\t%s%f%s\n\t%s%s%s\n%s\n",
            "object solar {",
			"parent inverter_",solar.data_[i].name.c_str(),";",
            "generator_mode \"",solar.data_[i].generator_mode.c_str(),"\";",
            "name ",solar.data_[i].name.c_str(),";",
            "generator_status ",solar.data_[i].generator_status.c_str(),";",
			"phases ",solar.data_[i].phase.c_str(),";",
			"panel_type ",solar.data_[i].panel_type.c_str(),";",
			"efficiency  ",solar.data_[i].efficiency,";",
			"area  ",solar.data_[i].area,";",
			"INSTALLATION_TYPE  ",solar.data_[i].INSTALLATION_TYPE.c_str(),";",
            "}");   
};

for (int i=0;i<wind.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n%s\n",
            "object windturb_dg {",
			"parent \"",wind.data_[i].parent.c_str(),"\";",
            "phases \"",wind.data_[i].phase.c_str(),"\";",
            "name ",wind.data_[i].name.c_str(),";",
            "Gen_status ",wind.data_[i].Gen_status.c_str(),";",
			"Gen_type ",wind.data_[i].Gen_type.c_str(),";",
			"Gen_mode ",wind.data_[i].Gen_mode.c_str(),";",
			"Turbine_Model  ",wind.data_[i].Turbine_Model.c_str(),";",
            "}");   
};

for (int i=0;i<battery.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s\n\t%s%f%s\n%s\n%s\n%s\n%s\n%s\n",
            "object battery {",
			"parent \"",battery.data_[i].parent.c_str(),"\";",
            "generator_mode \"",battery.data_[i].generator_mode.c_str(),"\";",
            "name ",battery.data_[i].name.c_str(),";",
            "V_Max ",battery.data_[i].V_Max,";",
			"I_Max ",battery.data_[i].I_Max,";",
			"P_Max ",battery.data_[i].P_Max,";",
			"E_Max  ",battery.data_[i].E_Max,";",
			"base_efficiency ",battery.data_[i].base_efficiency,";",
            "parasitic_power_draw ",battery.data_[i].parasitic_power_draw,";",
			"power_type ",battery.data_[i].power_type.c_str(),";",
			"generator_status ",battery.data_[i].generator_status.c_str(),";",
			"Energy ",battery.data_[i].battery_Energy,";",
			"power_factor ",battery.data_[i].battery_power_factor,";",
			"object player {",
			"property scheduled_power;",
			"file ./BatteryS1.player;",
			"};",
            "}");   
};


//XF


for (int i=0;i<XFC.size();i++)
{
   if(XFC.data_[i].power_rating==-1)
   {
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s%f%s%s\n%s\n",
            "object transformer_configuration {",
			"name ",XFC.data_[i].XFC_name.c_str(),";",
            "connect_type ",XFC.data_[i].connect_type.c_str(),";",
            "install_type ",XFC.data_[i].install_type.c_str(),";",
			"powerA_rating ",XFC.data_[i].powerA_rating,";",
            "primary_voltage ",XFC.data_[i].primary_voltage,";",
			"secondary_voltage ",XFC.data_[i].secondary_voltage,";",
			"impedance ",XFC.data_[i].resistance,"+",XFC.data_[i].reactance,"j",";",
            "}");   
   }
     else if(XFC.data_[i].power_rating==-2)
   {
    fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s%f%s%s\n%s\n",
            "object transformer_configuration {",
			"name \"",XFC.data_[i].XFC_name.c_str(),"\";",
            "connect_type \"",XFC.data_[i].connect_type.c_str(),"\";",
            "install_type ",XFC.data_[i].install_type.c_str(),";",
			"powerB_rating ",XFC.data_[i].powerB_rating,";",
            "primary_voltage ",XFC.data_[i].primary_voltage,";",
			"secondary_voltage ",XFC.data_[i].secondary_voltage,";",
			"impedance ",XFC.data_[i].resistance,"+",XFC.data_[i].reactance,"j",";",
            "}");   
   }
    else  if(XFC.data_[i].power_rating==-3)
   {
     fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s%f%s%s\n%s\n",
            "object transformer_configuration {",
			"name \"",XFC.data_[i].XFC_name.c_str(),"\";",
            "connect_type \"",XFC.data_[i].connect_type.c_str(),"\";",
            "install_type ",XFC.data_[i].install_type.c_str(),";",
			"powerC_rating ",XFC.data_[i].powerC_rating,";",
            "primary_voltage ",XFC.data_[i].primary_voltage,";",
			"secondary_voltage ",XFC.data_[i].secondary_voltage,";",
			"impedance ",XFC.data_[i].resistance,"+",XFC.data_[i].reactance,"j",";",
            "}");   
   }
    else
   {
     fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s%f%s%s\n%s\n",
            "object transformer_configuration {",
			"name \"",XFC.data_[i].XFC_name.c_str(),"\";",
            "connect_type \"",XFC.data_[i].connect_type.c_str(),"\";",
            "install_type ",XFC.data_[i].install_type.c_str(),";",
			"power_rating ",XFC.data_[i].power_rating,";",
            "primary_voltage ",XFC.data_[i].primary_voltage,";",
			"secondary_voltage ",XFC.data_[i].secondary_voltage,";",
			"impedance ",XFC.data_[i].resistance,"+",XFC.data_[i].reactance,"j",";",
            "}");   
   };
 
};

for (int i=0;i<XF.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n%s\n",
            "object transformer {",
			"name \"",XF.data_[i].XF_name.c_str(),"\";",
            "phases \"",XF.data_[i].phase.c_str(),"\";",
            "from ",XF.data_[i].fromNode.c_str(),";",
            "to tripNode_",house.data_[i].house_name.c_str(),";",
			"configuration ",XF.data_[i].config.c_str(),";",
            "}");   
};


// XFres
for (int i=0;i<XFCres.size();i++)
{
   if(XFCres.data_[i].power_rating==-1)
   {
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s%f%s%s\n%s\n",
            "object transformer_configuration {",
			"name ",XFCres.data_[i].XFC_name.c_str(),";",
            "connect_type ",XFCres.data_[i].connect_type.c_str(),";",
            "install_type ",XFCres.data_[i].install_type.c_str(),";",
			"powerA_rating ",XFCres.data_[i].powerA_rating,";",
            "primary_voltage ",XFCres.data_[i].primary_voltage,";",
			"secondary_voltage ",XFCres.data_[i].secondary_voltage,";",
			"impedance ",XFCres.data_[i].resistance,"+",XFCres.data_[i].reactance,"j",";",
            "}");   
   }
     else if(XFCres.data_[i].power_rating==-2)
   {
    fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s%f%s%s\n%s\n",
            "object transformer_configuration {",
			"name \"",XFCres.data_[i].XFC_name.c_str(),"\";",
            "connect_type \"",XFCres.data_[i].connect_type.c_str(),"\";",
            "install_type ",XFCres.data_[i].install_type.c_str(),";",
			"powerB_rating ",XFCres.data_[i].powerB_rating,";",
            "primary_voltage ",XFCres.data_[i].primary_voltage,";",
			"secondary_voltage ",XFCres.data_[i].secondary_voltage,";",
			"impedance ",XFCres.data_[i].resistance,"+",XFCres.data_[i].reactance,"j",";",
            "}");   
   }
    else  if(XFCres.data_[i].power_rating==-3)
   {
     fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s%f%s%s\n%s\n",
            "object transformer_configuration {",
			"name \"",XFCres.data_[i].XFC_name.c_str(),"\";",
            "connect_type \"",XFCres.data_[i].connect_type.c_str(),"\";",
            "install_type ",XFCres.data_[i].install_type.c_str(),";",
			"powerC_rating ",XFCres.data_[i].powerC_rating,";",
            "primary_voltage ",XFCres.data_[i].primary_voltage,";",
			"secondary_voltage ",XFCres.data_[i].secondary_voltage,";",
			"impedance ",XFCres.data_[i].resistance,"+",XFCres.data_[i].reactance,"j",";",
            "}");   
   }
    else
   {
     fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s\n\t%s%f%s%f%s%s\n%s\n",
            "object transformer_configuration {",
			"name \"",XFCres.data_[i].XFC_name.c_str(),"\";",
            "connect_type \"",XFCres.data_[i].connect_type.c_str(),"\";",
            "install_type ",XFCres.data_[i].install_type.c_str(),";",
			"power_rating ",XFCres.data_[i].power_rating,";",
            "primary_voltage ",XFCres.data_[i].primary_voltage,";",
			"secondary_voltage ",XFCres.data_[i].secondary_voltage,";",
			"impedance ",XFCres.data_[i].resistance,"+",XFCres.data_[i].reactance,"j",";",
            "}");   
   };
 
};

for (int i=0;i<XFres.size();i++)
{
   fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n%s\n",
            "object transformer {",
			"name \"",XFres.data_[i].XF_name.c_str(),"\";",
            "phases \"",XFres.data_[i].phase.c_str(),"\";",
            "from ",XFres.data_[i].fromNode.c_str(),";",
            "to ",meter.data_[i].name.c_str(),";",
			"configuration ",XFres.data_[i].config.c_str(),";",
            "}");   
};
//****************Recorder**************************
 // voltage
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%d%s\n\t%s%s%s\n%s\n",
            "object group_recorder {",
            "name ","load_voltageA",";",
			"group ","\"class=load\"",";",
			"property ","voltage_A",";",
			"file ","/tmp/load_voltageA.csv",";",
			"interval ",900,";",
			"complex_part ","MAG",";",
            "}");
			
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%d%s\n\t%s%s%s\n%s\n",
            "object group_recorder {",
            "name ","load_voltageB",";",
			"group ","\"class=load\"",";",
			"property ","voltage_B",";",
			"file ","/tmp/load_voltageB.csv",";",
			"interval ",900,";",
			"complex_part ","MAG",";",
            "}");
			
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%d%s\n\t%s%s%s\n%s\n",
            "object group_recorder {",
            "name ","load_voltageC",";",
			"group ","\"class=load\"",";",
			"property ","voltage_C",";",
			"file ","/tmp/load_voltageC.csv",";",
			"interval ",900,";",
			"complex_part ","MAG",";",
            "}"); 
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%d%s\n\t%s%s%s\n%s\n",
            "object group_recorder {",
            "name ","node_voltageA",";",
			"group ","\"class=node\"",";",
			"property ","voltage_A",";",
			"file ","/tmp/node_voltageA.csv",";",
			"interval ",900,";",
			"complex_part ","MAG",";",
            "}");
			
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%d%s\n\t%s%s%s\n%s\n",
            "object group_recorder {",
            "name ","node_voltageB",";",
			"group ","\"class=node\"",";",
			"property ","voltage_B",";",
			"file ","/tmp/node_voltageB.csv",";",
			"interval ",900,";",
			"complex_part ","MAG",";",
            "}");
			
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%d%s\n\t%s%s%s\n%s\n",
            "object group_recorder {",
            "name ","node_voltageC",";",
			"group ","\"class=node\"",";",
			"property ","voltage_C",";",
			"file ","/tmp/node_voltageC.csv",";",
			"interval ",900,";",
			"complex_part ","MAG",";",
            "}");
			
fprintf(fp,"\n%s\n\%s%s\n\t%s%s%s\n\t%s%d%s\n%s\n",
            "object multi_recorder {",
            "property "," Solar1:P_Out,Solar2:P_Out,Solar3:P_Out,Solar4:VA_Out,Solar5:VA_Out,Solar6:VA_Out,Solar7:P_Out,Solar8:P_Out,Solar9:P_Out,          windturb1:power_A,windturb1:power_B,windturb1:power_C,windturb2:power_A,windturb2:power_B,windturb2:power_C,                           windturb3:power_A,windturb3:power_B,windturb3:power_C,windturb4:power_A,windturb4:power_B,windturb4:power_C,                            windturb5:power_A,windturb5:power_B,windturb5:power_C,windturb6:power_A,windturb6:power_B,windturb6:power_C,                           windturb7:power_A,windturb7:power_B,windturb7:power_C,windturb8:power_A,windturb8:power_B,windturb8:power_C,                            windturb9:power_A,windturb9:power_B,windturb9:power_C,battery1:VA_Out,battery2:VA_Out,battery3:VA_Out,battery4:VA_Out,battery5:VA_Out,battery6:VA_Out,battery7:VA_Out,battery8:VA_Out,battery9:VA_Out;",
			"file ","/tmp/renewable.csv",";",
			"interval ",900,";",
            "}");			
			

/* //voltage dump
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n%s\n",
            "object voltdump {",
            "filename ","/tmp/output_voltage.csv",";",
			"mode ","polar",";",
            "}"); */

/* // current
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%d%s\n\t%s%s%s\n%s\n",
            "object group_recorder {",
            "name ","load_currentA",";",
			"group ","\"class=load\"",";",
			"property ","current_A",";",
			"file ","load_currentA.csv",";",
			"interval ",900,";",
			"complex_part ","MAG",";",
            "}");
			
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%d%s\n\t%s%s%s\n%s\n",
            "object group_recorder {",
            "name ","load_currentB",";",
			"group ","\"class=load\"",";",
			"property ","current_B",";",
			"file ","load_currentB.csv",";",
			"interval ",900,";",
			"complex_part ","MAG",";",
            "}");
			
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%d%s\n\t%s%s%s\n%s\n",
            "object group_recorder {",
            "name ","load_currentC",";",
			"group ","\"class=load\"",";",
			"property ","current_C",";",
			"file ","load_currentC.csv",";",
			"interval ",900,";",
			"complex_part ","MAG",";",
            "}");
// power
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%d%s\n%s\n",
            "object group_recorder {",
            "name ","load_powerA",";",
			"group ","\"class=load\"",";",
			"property ","constant_power_A_real",";",
			"file ","load_powerA.csv",";",
			"interval ",900,";",
            "}");
			
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%d%s\n%s\n",
            "object group_recorder {",
            "name ","load_powerB",";",
			"group ","\"class=load\"",";",
			"property ","constant_power_B_real",";",
			"file ","load_powerB.csv",";",
			"interval ",900,";",
            "}");
			
fprintf(fp,"\n%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%s%s\n\t%s%d%s\n%s\n",
            "object group_recorder {",
            "name ","load_powerC",";",
			"group ","\"class=load\"",";",
			"property ","constant_power_C_real",";",
			"file ","load_powerC.csv",";",
			"interval ",900,";",
            "}"); */







fclose(fp);

/* string file_name1="/tmp/appliance_schedules.glm";
FILE* fp1 = fopen(file_name1.c_str(), "w");
//****************appliance_schedules**************************
// responsive_loads
for (int i=0;i<Schedules.size();i++)
{
    fprintf(fp1,"\n%s\n%s\n%s\n%s\n",
            "schedule responsive_loads {",
		    "{",
		    "*"
        

fclose(fp1);*/
return 0;
} 


//**********************************************************************************************************************************
// Created by: Jun Tan, jun.tan@geirina.net
// Date: 06/05/2018
// This function calculates the N by N complex matrix inverse to get Y matrix for NR power flow
// **********************************************************************************************************************************
template <typename line_tuple, typename line_comp> // define a generic typename T so that any types can be used. 
inline void MatCalY(HeapAccum<line_tuple, line_comp> &gLine)
{
	int i,j,k,n;
	double content = 0;
	bool success = true;
	matrix <double> inputreal2(2,2);  
	matrix<double> inputimg2(2,2);
	matrix <double> outputreal2(2,2); 
	matrix<double> outputimg2(2,2);
	matrix <double> inputreal3(3,3);  
	matrix<double> inputimg3(3,3);
	matrix <double> outputreal3(3,3); 
	matrix<double> outputimg3(3,3);
	matrix <double> m2(4,4);
	matrix <double> m3(6,6);
	real__t *Raa, *Rab, *Rac, *Rba, *Rbb, *Rbc, *Rca, *Rcb, *Rcc, *Xaa, *Xab, *Xac, *Xba, *Xbb, *Xbc, *Xca, *Xcb, *Xcc;
	uint__t *Phase;
	
	n= gLine.data_.size();

	Raa = (real__t *)malloc(sizeof(real__t)*(n));
	Rab = (real__t *)malloc(sizeof(real__t)*(n));
    Rac = (real__t *)malloc(sizeof(real__t)*(n));
	Rba = (real__t *)malloc(sizeof(real__t)*(n)); 
    Rbb = (real__t *)malloc(sizeof(real__t)*(n));
	Rbc = (real__t *)malloc(sizeof(real__t)*(n)); 
    Rca = (real__t *)malloc(sizeof(real__t)*(n));
	Rcb = (real__t *)malloc(sizeof(real__t)*(n)); 
    Rcc = (real__t *)malloc(sizeof(real__t)*(n));
	Xaa = (real__t *)malloc(sizeof(real__t)*(n));
	Xab = (real__t *)malloc(sizeof(real__t)*(n));
    Xac = (real__t *)malloc(sizeof(real__t)*(n));
	Xba = (real__t *)malloc(sizeof(real__t)*(n)); 
    Xbb = (real__t *)malloc(sizeof(real__t)*(n));
	Xbc = (real__t *)malloc(sizeof(real__t)*(n)); 
    Xca = (real__t *)malloc(sizeof(real__t)*(n));
	Xcb = (real__t *)malloc(sizeof(real__t)*(n)); 
    Xcc = (real__t *)malloc(sizeof(real__t)*(n));	
    Phase = (uint__t *)malloc(sizeof(uint__t)*(n));	
	
    //gLine.sort_heap();  
		
	for (k=0;k<n;k++)
	{
	   Raa[k] = gLine.data_[k].Raa;
	   Rab[k] = gLine.data_[k].Rab;
	   Rac[k] = gLine.data_[k].Rac;
	   Rba[k] = gLine.data_[k].Rba;
	   Rbb[k] = gLine.data_[k].Rbb;
	   Rbc[k] = gLine.data_[k].Rbc;
	   Rca[k] = gLine.data_[k].Rca;
	   Rcb[k] = gLine.data_[k].Rcb;
	   Rcc[k] = gLine.data_[k].Rcc;
	   Xaa[k] = gLine.data_[k].Xaa;
	   Xab[k] = gLine.data_[k].Xab;
	   Xac[k] = gLine.data_[k].Xac;
	   Xba[k] = gLine.data_[k].Xba;
	   Xbb[k] = gLine.data_[k].Xbb;
	   Xbc[k] = gLine.data_[k].Xbc;
	   Xca[k] = gLine.data_[k].Xca;
	   Xcb[k] = gLine.data_[k].Xcb;
	   Xcc[k] = gLine.data_[k].Xcc;
	   Phase[k] = gLine.data_[k].phaseFlag;
	   
	   if (Phase[k]==1){  //A
		   gLine.data_[k].Gaa = Raa[k]/(Raa[k]*Raa[k] + Xaa[k]*Xaa[k]);
		   gLine.data_[k].Baa = -Xaa[k]/(Raa[k]*Raa[k] + Xaa[k]*Xaa[k]);
	   }else if (Phase[k]==2){  //B
		   gLine.data_[k].Gbb = Rbb[k]/(Rbb[k]*Rbb[k] + Xbb[k]*Xbb[k]);
		   gLine.data_[k].Bbb = -Xbb[k]/(Rbb[k]*Rbb[k] + Xbb[k]*Xbb[k]);
	   }else if (Phase[k]==3){  //C
		   gLine.data_[k].Gcc = Rcc[k]/(Rcc[k]*Rcc[k] + Xcc[k]*Xcc[k]);
		   gLine.data_[k].Bcc = -Xcc[k]/(Rcc[k]*Rcc[k] + Xcc[k]*Xcc[k]);
	   }else if (Phase[k]==4){  //AB
		   // set inputreal matrix value:		
			inputreal2.setvalue(0,0,Raa[k]);
			inputreal2.setvalue(0,1,Rab[k]);
			inputreal2.setvalue(1,0,Rba[k]);
			inputreal2.setvalue(1,1,Rbb[k]);
			// set inputimg matrix value:
			inputimg2.setvalue(0,0,Xaa[k]);
			inputimg2.setvalue(0,1,Xab[k]);
			inputimg2.setvalue(1,0,Xba[k]);
			inputimg2.setvalue(1,1,Xbb[k]);
			// combine inputreal and inputimg to a big matrix 'm' to be calculated for its inverse.	
			for(i=0;i<2;i++)
			{ 
				for(j=0;j<2;j++)
				{
					inputreal2.getvalue(i,j, content, success);
					m2.setvalue(i,j, content);
					m2.setvalue(i+2,j+2,content);
					inputimg2.getvalue(i,j, content, success);
					m2.setvalue(i,j+2,content);
					m2.setvalue(i+2,j,-content);
				}
			}
			
			m2.invert();  //calculate matrix inversion		
			// split the big 'm' matrix to get the outputreal and outputimg matrix.
			for(i=0;i<2;i++)
			{
				for(j=0;j<2;j++)
				{
					m2.getvalue(i,j,content, success);
					outputreal2.setvalue(i,j,content);
					m2.getvalue(i,j+2,content,success);
					outputimg2.setvalue(i,j,content);
				}
			}		
			// transfer data in outputreal and outputimg to Areal and Aimg as ArrayAccum				
			outputreal2.getvalue(0,0,content,success);
			gLine.data_[k].Gaa = content;
			outputreal2.getvalue(0,1,content,success);
			gLine.data_[k].Gab = content;
			outputreal2.getvalue(1,0,content,success);
			gLine.data_[k].Gba = content;
			outputreal2.getvalue(1,1,content,success);
			gLine.data_[k].Gbb = content;
			outputimg2.getvalue(0,0,content,success);
			gLine.data_[k].Baa = content;
			outputimg2.getvalue(0,1,content,success);
			gLine.data_[k].Bab = content;
			outputimg2.getvalue(1,0,content,success);
			gLine.data_[k].Bba = content;
			outputimg2.getvalue(1,1,content,success);
			gLine.data_[k].Bbb = content;
	   }else if (Phase[k]==5){  //BC
		   // set inputreal matrix value:		
			inputreal2.setvalue(0,0,Rbb[k]);
			inputreal2.setvalue(0,1,Rbc[k]);
			inputreal2.setvalue(1,0,Rcb[k]);
			inputreal2.setvalue(1,1,Rcc[k]);
			// set inputimg matrix value:
			inputimg2.setvalue(0,0,Xbb[k]);
			inputimg2.setvalue(0,1,Xbc[k]);
			inputimg2.setvalue(1,0,Xcb[k]);
			inputimg2.setvalue(1,1,Xcc[k]);
			// combine inputreal and inputimg to a big matrix 'm' to be calculated for its inverse.	
			for(i=0;i<2;i++)
			{ 
				for(j=0;j<2;j++)
				{
					inputreal2.getvalue(i,j, content, success);
					m2.setvalue(i,j, content);
					m2.setvalue(i+2,j+2,content);
					inputimg2.getvalue(i,j, content, success);
					m2.setvalue(i,j+2,content);
					m2.setvalue(i+2,j,-content);
				}
			}
			
			m2.invert();  //calculate matrix inversion		
			// split the big 'm' matrix to get the outputreal and outputimg matrix.
			for(i=0;i<2;i++)
			{
				for(j=0;j<2;j++)
				{
					m2.getvalue(i,j,content, success);
					outputreal2.setvalue(i,j,content);
					m2.getvalue(i,j+2,content,success);
					outputimg2.setvalue(i,j,content);
				}
			}		
			// transfer data in outputreal and outputimg to Areal and Aimg as ArrayAccum				
			outputreal2.getvalue(0,0,content,success);
			gLine.data_[k].Gbb = content;
			outputreal2.getvalue(0,1,content,success);
			gLine.data_[k].Gbc = content;
			outputreal2.getvalue(1,0,content,success);
			gLine.data_[k].Gcb = content;
			outputreal2.getvalue(1,1,content,success);
			gLine.data_[k].Gcc = content;
			outputimg2.getvalue(0,0,content,success);
			gLine.data_[k].Bbb = content;
			outputimg2.getvalue(0,1,content,success);
			gLine.data_[k].Bbc = content;
			outputimg2.getvalue(1,0,content,success);
			gLine.data_[k].Bcb = content;
			outputimg2.getvalue(1,1,content,success);
			gLine.data_[k].Bcc = content;
	   }else if (Phase[k]==6){  //AC
		   // set inputreal matrix value:		
			inputreal2.setvalue(0,0,Raa[k]);
			inputreal2.setvalue(0,1,Rac[k]);
			inputreal2.setvalue(1,0,Rca[k]);
			inputreal2.setvalue(1,1,Rcc[k]);
			// set inputimg matrix value:
			inputimg2.setvalue(0,0,Xaa[k]);
			inputimg2.setvalue(0,1,Xac[k]);
			inputimg2.setvalue(1,0,Xca[k]);
			inputimg2.setvalue(1,1,Xcc[k]);
			// combine inputreal and inputimg to a big matrix 'm' to be calculated for its inverse.	
			for(i=0;i<2;i++)
			{ 
				for(j=0;j<2;j++)
				{
					inputreal2.getvalue(i,j, content, success);
					m2.setvalue(i,j, content);
					m2.setvalue(i+2,j+2,content);
					inputimg2.getvalue(i,j, content, success);
					m2.setvalue(i,j+2,content);
					m2.setvalue(i+2,j,-content);
				}
			}
			
			m2.invert();  //calculate matrix inversion		
			// split the big 'm' matrix to get the outputreal and outputimg matrix.
			for(i=0;i<2;i++)
			{
				for(j=0;j<2;j++)
				{
					m2.getvalue(i,j,content, success);
					outputreal2.setvalue(i,j,content);
					m2.getvalue(i,j+2,content,success);
					outputimg2.setvalue(i,j,content);
				}
			}		
			// transfer data in outputreal and outputimg to Areal and Aimg as ArrayAccum				
			outputreal2.getvalue(0,0,content,success);
			gLine.data_[k].Gaa = content;
			outputreal2.getvalue(0,1,content,success);
			gLine.data_[k].Gac = content;
			outputreal2.getvalue(1,0,content,success);
			gLine.data_[k].Gca = content;
			outputreal2.getvalue(1,1,content,success);
			gLine.data_[k].Gcc = content;
			outputimg2.getvalue(0,0,content,success);
			gLine.data_[k].Baa = content;
			outputimg2.getvalue(0,1,content,success);
			gLine.data_[k].Bac = content;
			outputimg2.getvalue(1,0,content,success);
			gLine.data_[k].Bca = content;
			outputimg2.getvalue(1,1,content,success);
			gLine.data_[k].Bcc = content;
	   }else if (Phase[k]==7){  //ABC
		    // set inputreal matrix value:		
			inputreal3.setvalue(0,0,Raa[k]);
			inputreal3.setvalue(0,1,Rab[k]);
			inputreal3.setvalue(0,2,Rac[k]);
			inputreal3.setvalue(1,0,Rba[k]);
			inputreal3.setvalue(1,1,Rbb[k]);
			inputreal3.setvalue(1,2,Rbc[k]);
			inputreal3.setvalue(2,0,Rca[k]);
			inputreal3.setvalue(2,1,Rcb[k]);
			inputreal3.setvalue(2,2,Rcc[k]);			
			// set inputimg matrix value:
			inputimg3.setvalue(0,0,Xaa[k]);
			inputimg3.setvalue(0,1,Xab[k]);
			inputimg3.setvalue(0,2,Xac[k]);
			inputimg3.setvalue(1,0,Xba[k]);
			inputimg3.setvalue(1,1,Xbb[k]);
			inputimg3.setvalue(1,2,Xbc[k]);
			inputimg3.setvalue(2,0,Xca[k]);
			inputimg3.setvalue(2,1,Xcb[k]);
			inputimg3.setvalue(2,2,Xcc[k]);
			// combine inputreal and inputimg to a big matrix 'm' to be calculated for its inverse.	
			for(i=0;i<3;i++)
			{ 
				for(j=0;j<3;j++)
				{
					inputreal3.getvalue(i,j, content, success);
					m3.setvalue(i,j, content);
					m3.setvalue(i+3,j+3,content);
					inputimg3.getvalue(i,j, content, success);
					m3.setvalue(i,j+3,content);
					m3.setvalue(i+3,j,-content);
				}
			}
			
			m3.invert();  //calculate matrix inversion		
			// split the big 'm' matrix to get the outputreal and outputimg matrix.
			for(i=0;i<3;i++)
			{
				for(j=0;j<3;j++)
				{
					m3.getvalue(i,j,content, success);
					outputreal3.setvalue(i,j,content);
					m3.getvalue(i,j+3,content,success);
					outputimg3.setvalue(i,j,content);
				}
			}		
			// transfer data in outputreal and outputimg to Areal and Aimg as ArrayAccum				
			outputreal3.getvalue(0,0,content,success);
			gLine.data_[k].Gaa = content;
			outputreal3.getvalue(0,1,content,success);
			gLine.data_[k].Gab = content;
			outputreal3.getvalue(0,2,content,success);
			gLine.data_[k].Gac = content;
			outputreal3.getvalue(1,0,content,success);
			gLine.data_[k].Gba = content;
			outputreal3.getvalue(1,1,content,success);
			gLine.data_[k].Gbb = content;
			outputreal3.getvalue(1,2,content,success);
			gLine.data_[k].Gbc = content;
			outputreal3.getvalue(2,0,content,success);
			gLine.data_[k].Gca = content;
			outputreal3.getvalue(2,1,content,success);
			gLine.data_[k].Gcb = content;
			outputreal3.getvalue(2,2,content,success);
			gLine.data_[k].Gcc = content;
			outputimg3.getvalue(0,0,content,success);
			gLine.data_[k].Baa = content;
			outputimg3.getvalue(0,1,content,success);
			gLine.data_[k].Bab = content;
			outputimg3.getvalue(0,2,content,success);
			gLine.data_[k].Bac = content;
			outputimg3.getvalue(1,0,content,success);
			gLine.data_[k].Bba = content;
			outputimg3.getvalue(1,1,content,success);
			gLine.data_[k].Bbb = content;
			outputimg3.getvalue(1,2,content,success);
			gLine.data_[k].Bbc = content;
			outputimg3.getvalue(2,0,content,success);
			gLine.data_[k].Bca = content;
			outputimg3.getvalue(2,1,content,success);
			gLine.data_[k].Bcb = content;
			outputimg3.getvalue(2,2,content,success);
			gLine.data_[k].Bcc = content;								
	   }
	   	 	//std::cout << "gLine: " << k <<": "<< gLine.data_[k]<< std::endl;
	   
	}
	free(Raa);
	free(Rab);
	free(Rac);
	free(Rba);
	free(Rbb);
	free(Rbc);
	free(Rca);
	free(Rcb);
	free(Rcc);
	free(Xaa);
	free(Xab);
	free(Xac);
	free(Xba);
	free(Xbb);
	free(Xbc);
	free(Xca);
	free(Xcb);
	free(Xcc);
	free(Phase);
}	

//*****get Ai[] for each node in NR method for power distribution system power flow ****************************************************************************************************************************
//Created by: Jun Tan, jun.tan@geirina.net
//Date: 05/09/2018
template <typename T_node_tuple, typename node_comp>
inline void GetAi (HeapAccum<T_node_tuple, node_comp> &gNode) {
    int i,j,n; // counter for the array
    uint__t *Ai;
    n= gNode.data_.size();
    Ai = (uint__t *)malloc(sizeof(uint__t)*(n)); 
    memset(Ai, 0, sizeof(uint__t)*(n));

    gNode.sort_heap();  
	// =========================================
	j = 1; // counter for the pointer array
    Ai[0] = 0;
	for (i=0;i<n-1;i++){
		Ai[j] = gNode.data_[i].phaseNO + Ai[j-1];
        j++;
	}
	for (i=0;i<n;i++){
		if (gNode.data_[i].phaseFlag == 1){
			gNode.data_[i].Aia = Ai[i];
		}else if (gNode.data_[i].phaseFlag == 2){
			gNode.data_[i].Aib = Ai[i];
		}else if (gNode.data_[i].phaseFlag == 3){
			gNode.data_[i].Aic = Ai[i];
		}else if (gNode.data_[i].phaseFlag == 4){
			gNode.data_[i].Aia = Ai[i];
			gNode.data_[i].Aib = Ai[i]+1;
		}else if (gNode.data_[i].phaseFlag == 5){
			gNode.data_[i].Aib = Ai[i];
			gNode.data_[i].Aic = Ai[i]+1;
		}else if (gNode.data_[i].phaseFlag == 6){
			gNode.data_[i].Aia = Ai[i];
			gNode.data_[i].Aic = Ai[i]+1;
		}else if (gNode.data_[i].phaseFlag == 7){
			gNode.data_[i].Aia = Ai[i];
			gNode.data_[i].Aib = Ai[i]+1;
			gNode.data_[i].Aic = Ai[i]+2;
		}
	}
	free(Ai);
}
// **********************************************************************************************************************************
// Created by: Jun Tan jun.tan@geirina.net
// Date: 05/10/2018
// This code performs Newton Raphson power flow analysis for power distribution systems
//
template <typename T_vertex_tuple1, typename vertex_comp1, typename T_edge_tuple1, typename edge_comp1>
// template <typename TUP2, typename COMP2>
inline void GRAPHLU_NR_D (HeapAccum<T_vertex_tuple1, vertex_comp1> &gVertex, HeapAccum<T_edge_tuple1, edge_comp1> &gEdge, MaxAccum<int> maxiter, MaxAccum<double> maxchange) {

	// extern size_t g_si, g_sd, g_sp;
	printf("\n\n------------------------------------------------------------------------------------------- ");
	printf("\nStart Running GRAPHLU_NRpf_v4 C function!\n");
	printf("------------------------------------------------------------------------------------------- ");
	// ------------------------------------------------------------------------------------------------
	// 				Initialize variables and arrays
	// ------------------------------------------------------------------------------------------------
	
	// Initialize arrays and variables
	uint__t n, nnz, n_pp, nnz_pp, n_J, nnz_J;
	int ret, i, j, p, iter, counter, rowcnt, clen;
	double VA_R,VA_I,VB_R,VB_I,VC_R,VC_I,VAB_R,VAB_I,VBC_R,VBC_I,VCA_R,VCA_I,VABm,VBCm,VCAm,PdAB_cal,QdAB_cal,PdBC_cal,QdBC_cal,PdCA_cal,QdCA_cal,IAB_R,IAB_I,IBC_R,IBC_I,ICA_R,ICA_I,IA_R,IA_I,IB_R,IB_I,IC_R,IC_I,PdA_cal,QdA_cal,PdB_cal,QdB_cal,PdC_cal,QdC_cal;
	real__t maxDeltaP=0, maxDeltaQ=0, max_change_ex=maxchange;
	SGraphLU *graphlu;

	const double pi_value = 3.141592653589793;
	// real__t *x, err;	
	// uint__t fctnnz; // number of nonzeros after the matrix is factorized

	// // Get the dimension and the nnz of the matrix
	n= gVertex.data_.size(); 	nnz= gEdge.data_.size(); // get the size of the Y bus addmittance
	n_J=2*n;	nnz_J=4*nnz;
	
	//// Convert vectors to GRAPHLU Data Structure (pointers)
	real__t *eG, *eB, *deltaP, *deltaQ, *Vm, *Va, *Pn, *Qn, *Pb, *Qb, *Cb, *H,*N, *J, *L, *Pcal, *Qcal, *Jx, *deltaX, *cosine_array, *sine_array; 
	uint__t *ei, *btype, *Ji; 
	uint__t *ep, *Jp, *LM, *phaseN, *phaseFlag; 

 // values are computed during Jacobian initialization and each iteration 
    cosine_array = (real__t *)malloc(sizeof(real__t)*(nnz));
    sine_array = (real__t *)malloc(sizeof(real__t)*(nnz));
 
	eG = (real__t *)malloc(sizeof(real__t)*(nnz)); // G values in Y 
	eB = (real__t *)malloc(sizeof(real__t)*(nnz)); // B values in Y
	ei = (uint__t *)malloc(sizeof(uint__t)*(nnz)); // column indices of Y
	ep = (uint__t *)malloc(sizeof(uint__t)*(n+1)); // initial row pointers	

	H = (real__t *)malloc(sizeof(real__t)*(nnz)); // G values in Y 
	N = (real__t *)malloc(sizeof(real__t)*(nnz)); // G values in Y 
	J = (real__t *)malloc(sizeof(real__t)*(nnz)); // G values in Y 
	L = (real__t *)malloc(sizeof(real__t)*(nnz)); // G values in Y 

	Jx = (real__t *)malloc(sizeof(real__t)*(nnz_J)); // B values in Y
	Ji = (uint__t *)malloc(sizeof(uint__t)*(nnz_J)); // column indices of Y
	Jp = (uint__t *)malloc(sizeof(uint__t)*(n_J+1)); // initial row pointers	

	deltaP = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b
	deltaQ = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b
	deltaX = (real__t *)malloc(sizeof(real__t)*(n_J)); // deltaP and deltaQ, a 2n x 1 array

	// xP = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b
	// xQ = (real__t *)malloc(sizeof(real__t)*(n_pp)); // b in the Ax=b

	Vm = (real__t *)malloc(sizeof(real__t)*(n)); 
	Va = (real__t *)malloc(sizeof(real__t)*(n)); 

	Pn = (real__t *)malloc(sizeof(real__t)*(n)); 
	Qn = (real__t *)malloc(sizeof(real__t)*(n));
	
	Pb = (real__t *)malloc(sizeof(real__t)*(n)); 
	Qb = (real__t *)malloc(sizeof(real__t)*(n));
	Cb = (real__t *)malloc(sizeof(real__t)*(n));
	LM = (uint__t *)malloc(sizeof(uint__t)*(n)); 
	phaseN = (uint__t *)malloc(sizeof(uint__t)*(n));
	phaseFlag = (uint__t *)malloc(sizeof(uint__t)*(n));

	Pcal = (real__t *)malloc(sizeof(real__t)*(n)); 
	Qcal = (real__t *)malloc(sizeof(real__t)*(n));

	btype = (uint__t *)malloc(sizeof(uint__t)*(n));

	std::vector<double> maxDltP_list; // Max P in each iteration
	std::vector<double> maxDltQ_list; // Max Q in each iteration

	// Initialization to zero
	// [tc] using memset instead of for loop here, maybe can change 
	// to use calloc in the first place?
	memset(Vm, 0, sizeof(real__t)*(n));
	memset(Va, 0, sizeof(real__t)*(n));
	memset(Pn, 0, sizeof(real__t)*(n));
	memset(Qn, 0, sizeof(real__t)*(n));
	memset(deltaP, 0, sizeof(real__t)*(n));
	memset(deltaQ, 0, sizeof(real__t)*(n));

	printf("\n\n------------------------------------------------------------------------------------------- ");
	std::cout << "Number of rows:" << n << ",\tNumber of nonzeros:" << nnz << std::endl; 
	printf("\t -------------------------------------------------------------------------------------------- \n\n");	

	//std::cout << " ======================== Initialization of ararys used to store the factorized matrix and LU ========================"<< std::endl;

	// ------------------------------------------------------------------------------------------------
	// 				Convert Contariner to Arrays Used by NISCLU
	// ------------------------------------------------------------------------------------------------
	// Get the execution time of the code
	struct timeval t2_st, t2_end, t3_st, t3_end; long seconds, useconds;
	gettimeofday(&t2_st, 0);
	// auto t1 = Clock::now(); // a better way to time with higher precisions

 // =================================== Sort all input HeapAccum =================================
    gVertex.sort_heap();  
	gEdge.sort_heap();
  
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n================================================================================================ ");
	std::cout << "Time to sort HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 

	gettimeofday(&t2_st, 0); 
     	
	int ii; // counter for the array

	// ========================================= Convert gVertex data =========================================
	ii = 1; // counter for the pointer array
    ep[0] = 0;
	for (i=0;i<gVertex.size();i++){
   // set up the pointer array for GRAPHLU. Each entry indicates the starting position
   // of the particular row in the Ax/Ai array. Refer to GRAPHLU documentation for detail.  
	   ep[ii] = gVertex.data_[i].ep + ep[ii-1];
	   Vm[i] = gVertex.data_[i].Vm;
	   Va[i] = gVertex.data_[i].Va;
	   Pb[i] = gVertex.data_[i].Pn;
	   Qb[i] = gVertex.data_[i].Qn;
	   Cb[i] = gVertex.data_[i].Cap;
	   if (strcmp(gVertex.data_[i].Load_Model.c_str(),"Y-PQ")==0){
			LM[i] = 1;
		}
		else if (strcmp(gVertex.data_[i].Load_Model.c_str(),"Y-Z")==0){
			LM[i] = 2;
		}
		else if (strcmp(gVertex.data_[i].Load_Model.c_str(),"Y-I")==0){
			LM[i] = 3;
		}
		else if (strcmp(gVertex.data_[i].Load_Model.c_str(),"D-PQ")==0){
			LM[i] = 4;
		}
		else if (strcmp(gVertex.data_[i].Load_Model.c_str(),"D-Z")==0){
			LM[i] = 5;
		}
		else if (strcmp(gVertex.data_[i].Load_Model.c_str(),"D-I")==0){
			LM[i] = 6;
		}
		else {
			LM[i] = 0;
		}
	   phaseN[i] = gVertex.data_[i].phaseN;
	   phaseFlag[i] = gVertex.data_[i].phaseFlag;   
       btype[i] = gVertex.data_[i].bustype;
       ii++;
	}
	// Assign values to the last position of the array
	ep[n] = nnz;

	//update Pn and Qn, for Y connections
	for (i=0;i<gVertex.size();i++){  //Y-PQ
		//update Pn and Qn, for Y connections
		if (LM[i] == 1){  //Y-PQ
			Pn[i] = Pb[i];
			Qn[i] = Qb[i] + Cb[i]*Vm[i]*Vm[i];
		}
		else if (LM[i] == 2){  //Y-Z
			Pn[i] = Pb[i]*Vm[i]*Vm[i];
			Qn[i] = Qb[i]*Vm[i]*Vm[i] + Cb[i]*Vm[i]*Vm[i];
		}
		else if (LM[i] == 3){  //Y-I
			Pn[i] = Pb[i]*Vm[i];
			Qn[i] = Qb[i]*Vm[i] + Cb[i]*Vm[i]*Vm[i];
		}
		//update Pn and Qn, for Delta connections
		//first update the voltages 
		else if (LM[i] == 4){ 
			if (phaseFlag[i] == 4 &&  phaseN[i] == 1){ //phase AB, D-PQ
				VA_R = Vm[i]*cos(Va[i]);
				VA_I = Vm[i]*sin(Va[i]);
				VB_R = Vm[i+1]*cos(Va[i+1]);
				VB_I = Vm[i+1]*sin(Va[i+1]);
				VAB_R = VA_R - VB_R;
				VAB_I = VA_I - VB_I;
				VABm = sqrt(VAB_R*VAB_R + VAB_I*VAB_I);					
				PdAB_cal = Pb[i];
				QdAB_cal = Qb[i];
				IAB_R = (PdAB_cal*VAB_R + QdAB_cal*VAB_I)/(VABm*VABm);
				IAB_I = (PdAB_cal*VAB_I - QdAB_cal*VAB_R)/(VABm*VABm);
				IA_R = IAB_R;
				IA_I = IAB_I;
				IB_R = - IAB_R;
				IB_I = - IAB_I;
				PdA_cal = VA_R*IA_R + VA_I*IA_I;
				QdA_cal = VA_I*IA_R - VA_R*IA_I;
				PdB_cal = VB_R*IB_R + VB_I*IB_I;
				QdB_cal = VB_I*IB_R - VB_R*IB_I;
				Pn[i] = PdA_cal;
				Qn[i] = QdA_cal + Cb[i]*Vm[i]*Vm[i];
				Pn[i+1] = PdB_cal;
				Qn[i+1] = QdB_cal + Cb[i+1]*Vm[i+1]*Vm[i+1];
			}
			else if (phaseFlag[i] == 5 && phaseN[i] == 2){ //phase BC, D-PQ
				VB_R = Vm[i]*cos(Va[i]);
				VB_I = Vm[i]*sin(Va[i]);
				VC_R = Vm[i+1]*cos(Va[i+1]);
				VC_I = Vm[i+1]*sin(Va[i+1]);
				VBC_R = VB_R - VC_R;
				VBC_I = VB_I - VC_I;
				VBCm = sqrt(VBC_R*VBC_R + VBC_I*VBC_I);					
				PdBC_cal = Pb[i];
				QdBC_cal = Qb[i];
				IBC_R = (PdBC_cal*VBC_R + QdBC_cal*VBC_I)/(VBCm*VBCm);
				IBC_I = (PdBC_cal*VBC_I - QdBC_cal*VBC_R)/(VBCm*VBCm);
				IB_R = IBC_R;
				IB_I = IBC_I;
				IC_R = - IBC_R;
				IC_I = - IBC_I;
				PdB_cal = VB_R*IB_R + VB_I*IB_I;
				QdB_cal = VB_I*IB_R - VB_R*IB_I;
				PdC_cal = VC_R*IC_R + VC_I*IC_I;
				QdC_cal = VC_I*IC_R - VC_R*IC_I;
				Pn[i] = PdB_cal;
				Qn[i] = QdB_cal + Cb[i]*Vm[i]*Vm[i];
				Pn[i+1] = PdC_cal;
				Qn[i+1] = QdC_cal + Cb[i+1]*Vm[i+1]*Vm[i+1];
			}
			else if (phaseFlag[i] == 6 && phaseN[i] == 1){ //phase CA, D-PQ
				VA_R = Vm[i]*cos(Va[i]);
				VA_I = Vm[i]*sin(Va[i]);
				VC_R = Vm[i+1]*cos(Va[i+1]);
				VC_I = Vm[i+1]*sin(Va[i+1]);
				VCA_R = VC_R - VA_R;
				VCA_I = VC_I - VA_I;
				VCAm = sqrt(VCA_R*VCA_R + VCA_I*VCA_I);					
				PdCA_cal = Pb[i];
				QdCA_cal = Qb[i];
				ICA_R = (PdCA_cal*VCA_R + QdCA_cal*VCA_I)/(VCAm*VCAm);
				ICA_I = (PdCA_cal*VCA_I - QdCA_cal*VCA_R)/(VCAm*VCAm);
				IC_R = ICA_R;
				IC_I = ICA_I;
				IA_R = - ICA_R;
				IA_I = - ICA_I;
				PdC_cal = VC_R*IC_R + VC_I*IC_I;
				QdC_cal = VC_I*IC_R - VC_R*IC_I;
				PdA_cal = VA_R*IA_R + VA_I*IA_I;
				QdA_cal = VA_I*IA_R - VA_R*IA_I;
				Pn[i] = PdA_cal;
				Qn[i] = QdA_cal + Cb[i]*Vm[i]*Vm[i];
				Pn[i+1] = PdC_cal;
				Qn[i+1] = QdC_cal + Cb[i+1]*Vm[i+1]*Vm[i+1];
			}
			else if (phaseFlag[i] == 7 && phaseN[i] == 1){ //phase ABC, D-PQ
				VA_R = Vm[i]*cos(Va[i]);
				VA_I = Vm[i]*sin(Va[i]);
				VB_R = Vm[i+1]*cos(Va[i+1]);
				VB_I = Vm[i+1]*sin(Va[i+1]);
				VC_R = Vm[i+2]*cos(Va[i+2]);
				VC_I = Vm[i+2]*sin(Va[i+2]);
				VAB_R = VA_R - VB_R;
				VAB_I = VA_I - VB_I;
				VBC_R = VB_R - VC_R;
				VBC_I = VB_I - VC_I;
				VCA_R = VC_R - VA_R;
				VCA_I = VC_I - VA_I;
				VABm = sqrt(VAB_R*VAB_R + VAB_I*VAB_I);
				VBCm = sqrt(VBC_R*VBC_R + VBC_I*VBC_I);	
				VCAm = sqrt(VCA_R*VCA_R + VCA_I*VCA_I);					
				PdAB_cal = Pb[i];
				QdAB_cal = Qb[i];
				PdBC_cal = Pb[i+1];
				QdBC_cal = Qb[i+1];
				PdCA_cal = Pb[i+2];
				QdCA_cal = Qb[i+2];
				IAB_R = (PdAB_cal*VAB_R + QdAB_cal*VAB_I)/(VABm*VABm);
				IAB_I = (PdAB_cal*VAB_I - QdAB_cal*VAB_R)/(VABm*VABm);
				IBC_R = (PdBC_cal*VBC_R + QdBC_cal*VBC_I)/(VBCm*VBCm);
				IBC_I = (PdBC_cal*VBC_I - QdBC_cal*VBC_R)/(VBCm*VBCm);
				ICA_R = (PdCA_cal*VCA_R + QdCA_cal*VCA_I)/(VCAm*VCAm);
				ICA_I = (PdCA_cal*VCA_I - QdCA_cal*VCA_R)/(VCAm*VCAm);
				IA_R = IAB_R - ICA_R;
				IA_I = IAB_I - ICA_I;
				IB_R = IBC_R - IAB_R;
				IB_I = IBC_I - IAB_I;
				IC_R = ICA_R - IBC_R;
				IC_I = ICA_I - IBC_I;
				PdA_cal = VA_R*IA_R + VA_I*IA_I;
				QdA_cal = VA_I*IA_R - VA_R*IA_I;
				PdB_cal = VB_R*IB_R + VB_I*IB_I;
				QdB_cal = VB_I*IB_R - VB_R*IB_I;
				PdC_cal = VC_R*IC_R + VC_I*IC_I;
				QdC_cal = VC_I*IC_R - VC_R*IC_I;
				Pn[i] = PdA_cal;
				Qn[i] = QdA_cal + Cb[i]*Vm[i]*Vm[i];
				Pn[i+1] = PdB_cal;
				Qn[i+1] = QdB_cal + Cb[i+1]*Vm[i+1]*Vm[i+1];
				Pn[i+2] = PdC_cal;
				Qn[i+2] = QdC_cal + Cb[i+2]*Vm[i+1]*Vm[i+1];
			}
		}
		else if (LM[i] == 5){ 
			if (phaseFlag[i] == 4 &&  phaseN[i] == 1){ //phase AB, D-Z
				VA_R = Vm[i]*cos(Va[i]);
				VA_I = Vm[i]*sin(Va[i]);
				VB_R = Vm[i+1]*cos(Va[i+1]);
				VB_I = Vm[i+1]*sin(Va[i+1]);
				VAB_R = VA_R - VB_R;
				VAB_I = VA_I - VB_I;
				VABm = sqrt(VAB_R*VAB_R + VAB_I*VAB_I);					
				PdAB_cal = Pb[i]*VABm*VABm/3;
				QdAB_cal = Qb[i]*VABm*VABm/3;
				IAB_R = (PdAB_cal*VAB_R + QdAB_cal*VAB_I)/(VABm*VABm);
				IAB_I = (PdAB_cal*VAB_I - QdAB_cal*VAB_R)/(VABm*VABm);
				IA_R = IAB_R;
				IA_I = IAB_I;
				IB_R = - IAB_R;
				IB_I = - IAB_I;
				PdA_cal = VA_R*IA_R + VA_I*IA_I;
				QdA_cal = VA_I*IA_R - VA_R*IA_I;
				PdB_cal = VB_R*IB_R + VB_I*IB_I;
				QdB_cal = VB_I*IB_R - VB_R*IB_I;
				Pn[i] = PdA_cal;
				Qn[i] = QdA_cal + Cb[i]*Vm[i]*Vm[i];
				Pn[i+1] = PdB_cal;
				Qn[i+1] = QdB_cal + Cb[i+1]*Vm[i+1]*Vm[i+1];
			}
			else if (phaseFlag[i] == 5 && phaseN[i] == 2){ //phase BC, D-Z
				VB_R = Vm[i]*cos(Va[i]);
				VB_I = Vm[i]*sin(Va[i]);
				VC_R = Vm[i+1]*cos(Va[i+1]);
				VC_I = Vm[i+1]*sin(Va[i+1]);
				VBC_R = VB_R - VC_R;
				VBC_I = VB_I - VC_I;
				VBCm = sqrt(VBC_R*VBC_R + VBC_I*VBC_I);					
				PdBC_cal = Pb[i]*VBCm*VBCm/3;
				QdBC_cal = Qb[i]*VBCm*VBCm/3;
				IBC_R = (PdBC_cal*VBC_R + QdBC_cal*VBC_I)/(VBCm*VBCm);
				IBC_I = (PdBC_cal*VBC_I - QdBC_cal*VBC_R)/(VBCm*VBCm);
				IB_R = IBC_R;
				IB_I = IBC_I;
				IC_R = - IBC_R;
				IC_I = - IBC_I;
				PdB_cal = VB_R*IB_R + VB_I*IB_I;
				QdB_cal = VB_I*IB_R - VB_R*IB_I;
				PdC_cal = VC_R*IC_R + VC_I*IC_I;
				QdC_cal = VC_I*IC_R - VC_R*IC_I;
				Pn[i] = PdB_cal;
				Qn[i] = QdB_cal + Cb[i]*Vm[i]*Vm[i];
				Pn[i+1] = PdC_cal;
				Qn[i+1] = QdC_cal + Cb[i+1]*Vm[i+1]*Vm[i+1];
			}
			else if (phaseFlag[i] == 6 && phaseN[i] == 1){ //phase CA, D-Z
				VA_R = Vm[i]*cos(Va[i]);
				VA_I = Vm[i]*sin(Va[i]);
				VC_R = Vm[i+1]*cos(Va[i+1]);
				VC_I = Vm[i+1]*sin(Va[i+1]);
				VCA_R = VC_R - VA_R;
				VCA_I = VC_I - VA_I;
				VCAm = sqrt(VCA_R*VCA_R + VCA_I*VCA_I);					
				PdCA_cal = Pb[i]*VCAm*VCAm/3;
				QdCA_cal = Qb[i]*VCAm*VCAm/3;
				ICA_R = (PdCA_cal*VCA_R + QdCA_cal*VCA_I)/(VCAm*VCAm);
				ICA_I = (PdCA_cal*VCA_I - QdCA_cal*VCA_R)/(VCAm*VCAm);
				IC_R = ICA_R;
				IC_I = ICA_I;
				IA_R = - ICA_R;
				IA_I = - ICA_I;
				PdC_cal = VC_R*IC_R + VC_I*IC_I;
				QdC_cal = VC_I*IC_R - VC_R*IC_I;
				PdA_cal = VA_R*IA_R + VA_I*IA_I;
				QdA_cal = VA_I*IA_R - VA_R*IA_I;
				Pn[i] = PdA_cal;
				Qn[i] = QdA_cal + Cb[i]*Vm[i]*Vm[i];
				Pn[i+1] = PdC_cal;
				Qn[i+1] = QdC_cal + Cb[i+1]*Vm[i+1]*Vm[i+1];
			}
			else if (phaseFlag[i] == 7 && phaseN[i] == 1){ //phase ABC, D-Z
				VA_R = Vm[i]*cos(Va[i]);
				VA_I = Vm[i]*sin(Va[i]);
				VB_R = Vm[i+1]*cos(Va[i+1]);
				VB_I = Vm[i+1]*sin(Va[i+1]);
				VC_R = Vm[i+2]*cos(Va[i+2]);
				VC_I = Vm[i+2]*sin(Va[i+2]);
				VAB_R = VA_R - VB_R;
				VAB_I = VA_I - VB_I;
				VBC_R = VB_R - VC_R;
				VBC_I = VB_I - VC_I;
				VCA_R = VC_R - VA_R;
				VCA_I = VC_I - VA_I;
				VABm = sqrt(VAB_R*VAB_R + VAB_I*VAB_I);
				VBCm = sqrt(VBC_R*VBC_R + VBC_I*VBC_I);	
				VCAm = sqrt(VCA_R*VCA_R + VCA_I*VCA_I);					
				PdAB_cal = Pb[i]*VABm*VABm/3;
				QdAB_cal = Qb[i]*VABm*VABm/3;
				PdBC_cal = Pb[i+1]*VBCm*VBCm/3;
				QdBC_cal = Qb[i+1]*VBCm*VBCm/3;
				PdCA_cal = Pb[i+2]*VCAm*VCAm/3;
				QdCA_cal = Qb[i+2]*VCAm*VCAm/3;
				IAB_R = (PdAB_cal*VAB_R + QdAB_cal*VAB_I)/(VABm*VABm);
				IAB_I = (PdAB_cal*VAB_I - QdAB_cal*VAB_R)/(VABm*VABm);
				IBC_R = (PdBC_cal*VBC_R + QdBC_cal*VBC_I)/(VBCm*VBCm);
				IBC_I = (PdBC_cal*VBC_I - QdBC_cal*VBC_R)/(VBCm*VBCm);
				ICA_R = (PdCA_cal*VCA_R + QdCA_cal*VCA_I)/(VCAm*VCAm);
				ICA_I = (PdCA_cal*VCA_I - QdCA_cal*VCA_R)/(VCAm*VCAm);
				IA_R = IAB_R - ICA_R;
				IA_I = IAB_I - ICA_I;
				IB_R = IBC_R - IAB_R;
				IB_I = IBC_I - IAB_I;
				IC_R = ICA_R - IBC_R;
				IC_I = ICA_I - IBC_I;
				PdA_cal = VA_R*IA_R + VA_I*IA_I;
				QdA_cal = VA_I*IA_R - VA_R*IA_I;
				PdB_cal = VB_R*IB_R + VB_I*IB_I;
				QdB_cal = VB_I*IB_R - VB_R*IB_I;
				PdC_cal = VC_R*IC_R + VC_I*IC_I;
				QdC_cal = VC_I*IC_R - VC_R*IC_I;
				Pn[i] = PdA_cal;
				Qn[i] = QdA_cal + Cb[i]*Vm[i]*Vm[i];
				Pn[i+1] = PdB_cal;
				Qn[i+1] = QdB_cal + Cb[i+1]*Vm[i+1]*Vm[i+1];
				Pn[i+2] = PdC_cal;
				Qn[i+2] = QdC_cal + Cb[i+2]*Vm[i+2]*Vm[i+2];
			}
		}
		else if (LM[i] == 6){ 
			if (phaseFlag[i] == 4 &&  phaseN[i] == 1){ //phase AB, D-I
				VA_R = Vm[i]*cos(Va[i]);
				VA_I = Vm[i]*sin(Va[i]);
				VB_R = Vm[i+1]*cos(Va[i+1]);
				VB_I = Vm[i+1]*sin(Va[i+1]);
				VAB_R = VA_R - VB_R;
				VAB_I = VA_I - VB_I;
				VABm = sqrt(VAB_R*VAB_R + VAB_I*VAB_I);					
				PdAB_cal = Pb[i]*VABm/sqrt(3);
				QdAB_cal = Qb[i]*VABm/sqrt(3);
				IAB_R = (PdAB_cal*VAB_R + QdAB_cal*VAB_I)/(VABm*VABm);
				IAB_I = (PdAB_cal*VAB_I - QdAB_cal*VAB_R)/(VABm*VABm);
				IA_R = IAB_R;
				IA_I = IAB_I;
				IB_R = - IAB_R;
				IB_I = - IAB_I;
				PdA_cal = VA_R*IA_R + VA_I*IA_I;
				QdA_cal = VA_I*IA_R - VA_R*IA_I;
				PdB_cal = VB_R*IB_R + VB_I*IB_I;
				QdB_cal = VB_I*IB_R - VB_R*IB_I;
				Pn[i] = PdA_cal;
				Qn[i] = QdA_cal + Cb[i]*Vm[i]*Vm[i];
				Pn[i+1] = PdB_cal;
				Qn[i+1] = QdB_cal + Cb[i+1]*Vm[i+1]*Vm[i+1];
			}
			else if (phaseFlag[i] == 5 && phaseN[i] == 2){ //phase BC, D-I
				VB_R = Vm[i]*cos(Va[i]);
				VB_I = Vm[i]*sin(Va[i]);
				VC_R = Vm[i+1]*cos(Va[i+1]);
				VC_I = Vm[i+1]*sin(Va[i+1]);
				VBC_R = VB_R - VC_R;
				VBC_I = VB_I - VC_I;
				VBCm = sqrt(VBC_R*VBC_R + VBC_I*VBC_I);					
				PdBC_cal = Pb[i]*VBCm/sqrt(3);
				QdBC_cal = Qb[i]*VBCm/sqrt(3);
				IBC_R = (PdBC_cal*VBC_R + QdBC_cal*VBC_I)/(VBCm*VBCm);
				IBC_I = (PdBC_cal*VBC_I - QdBC_cal*VBC_R)/(VBCm*VBCm);
				IB_R = IBC_R;
				IB_I = IBC_I;
				IC_R = - IBC_R;
				IC_I = - IBC_I;
				PdB_cal = VB_R*IB_R + VB_I*IB_I;
				QdB_cal = VB_I*IB_R - VB_R*IB_I;
				PdC_cal = VC_R*IC_R + VC_I*IC_I;
				QdC_cal = VC_I*IC_R - VC_R*IC_I;
				Pn[i] = PdB_cal;
				Qn[i] = QdB_cal + Cb[i]*Vm[i]*Vm[i];
				Pn[i+1] = PdC_cal;
				Qn[i+1] = QdC_cal + Cb[i+1]*Vm[i+1]*Vm[i+1];
			}
			else if (phaseFlag[i] == 6 && phaseN[i] == 1){ //phase CA, D-I
				VA_R = Vm[i]*cos(Va[i]);
				VA_I = Vm[i]*sin(Va[i]);
				VC_R = Vm[i+1]*cos(Va[i+1]);
				VC_I = Vm[i+1]*sin(Va[i+1]);
				VCA_R = VC_R - VA_R;
				VCA_I = VC_I - VA_I;
				VCAm = sqrt(VCA_R*VCA_R + VCA_I*VCA_I);					
				PdCA_cal = Pb[i]*VCAm/sqrt(3);
				QdCA_cal = Qb[i]*VCAm/sqrt(3);
				ICA_R = (PdCA_cal*VCA_R + QdCA_cal*VCA_I)/(VCAm*VCAm);
				ICA_I = (PdCA_cal*VCA_I - QdCA_cal*VCA_R)/(VCAm*VCAm);
				IC_R = ICA_R;
				IC_I = ICA_I;
				IA_R = - ICA_R;
				IA_I = - ICA_I;
				PdC_cal = VC_R*IC_R + VC_I*IC_I;
				QdC_cal = VC_I*IC_R - VC_R*IC_I;
				PdA_cal = VA_R*IA_R + VA_I*IA_I;
				QdA_cal = VA_I*IA_R - VA_R*IA_I;
				Pn[i] = PdA_cal;
				Qn[i] = QdA_cal + Cb[i]*Vm[i]*Vm[i];
				Pn[i+1] = PdC_cal;
				Qn[i+1] = QdC_cal + Cb[i+1]*Vm[i+1]*Vm[i+1];
			}
			else if (phaseFlag[i] == 7 && phaseN[i] == 1){ //phase ABC, D-I
				VA_R = Vm[i]*cos(Va[i]);
				VA_I = Vm[i]*sin(Va[i]);
				VB_R = Vm[i+1]*cos(Va[i+1]);
				VB_I = Vm[i+1]*sin(Va[i+1]);
				VC_R = Vm[i+2]*cos(Va[i+2]);
				VC_I = Vm[i+2]*sin(Va[i+2]);
				VAB_R = VA_R - VB_R;
				VAB_I = VA_I - VB_I;
				VBC_R = VB_R - VC_R;
				VBC_I = VB_I - VC_I;
				VCA_R = VC_R - VA_R;
				VCA_I = VC_I - VA_I;
				VABm = sqrt(VAB_R*VAB_R + VAB_I*VAB_I);
				VBCm = sqrt(VBC_R*VBC_R + VBC_I*VBC_I);	
				VCAm = sqrt(VCA_R*VCA_R + VCA_I*VCA_I);					
				PdAB_cal = Pb[i]*VABm/sqrt(3);
				QdAB_cal = Qb[i]*VABm/sqrt(3);
				PdBC_cal = Pb[i+1]*VBCm/sqrt(3);
				QdBC_cal = Qb[i+1]*VBCm/sqrt(3);
				PdCA_cal = Pb[i+2]*VCAm/sqrt(3);
				QdCA_cal = Qb[i+2]*VCAm/sqrt(3);
				IAB_R = (PdAB_cal*VAB_R + QdAB_cal*VAB_I)/(VABm*VABm);
				IAB_I = (PdAB_cal*VAB_I - QdAB_cal*VAB_R)/(VABm*VABm);
				IBC_R = (PdBC_cal*VBC_R + QdBC_cal*VBC_I)/(VBCm*VBCm);
				IBC_I = (PdBC_cal*VBC_I - QdBC_cal*VBC_R)/(VBCm*VBCm);
				ICA_R = (PdCA_cal*VCA_R + QdCA_cal*VCA_I)/(VCAm*VCAm);
				ICA_I = (PdCA_cal*VCA_I - QdCA_cal*VCA_R)/(VCAm*VCAm);
				IA_R = IAB_R - ICA_R;
				IA_I = IAB_I - ICA_I;
				IB_R = IBC_R - IAB_R;
				IB_I = IBC_I - IAB_I;
				IC_R = ICA_R - IBC_R;
				IC_I = ICA_I - IBC_I;
				PdA_cal = VA_R*IA_R + VA_I*IA_I;
				QdA_cal = VA_I*IA_R - VA_R*IA_I;
				PdB_cal = VB_R*IB_R + VB_I*IB_I;
				QdB_cal = VB_I*IB_R - VB_R*IB_I;
				PdC_cal = VC_R*IC_R + VC_I*IC_I;
				QdC_cal = VC_I*IC_R - VC_R*IC_I;
				Pn[i] = PdA_cal;
				Qn[i] = QdA_cal + Cb[i]*Vm[i]*Vm[i];
				Pn[i+1] = PdB_cal;
				Qn[i+1] = QdB_cal + Cb[i+1]*Vm[i+1]*Vm[i+1];
				Pn[i+2] = PdC_cal;
				Qn[i+2] = QdC_cal + Cb[i+2]*Vm[i+2]*Vm[i+2];
			}
		} 
	}
	
	 // // // check for arrays
	 //for (i=0; i<n; i++)
	 	//std::cout << "ep" << i+1 << ":"  << std::setprecision(8) << ep[i+1]<<", "  << gVertex.data_[i]<< std::endl;

	// ========================================= Convert gEdge data  =========================================
	for (i=0; i<gEdge.size(); i++) {
		ei[i] = gEdge.data_[i].ei;
        eG[i] = gEdge.data_[i].eG;
        eB[i] = gEdge.data_[i].eB;
       // std::cout << "ei" << i << ", "  << std::setprecision(8) << ei[i] << ", "  << gEdge.data_[i] << std::endl;
		// ii ++;
	}

	// Get the time to convert the data between containers and arrays
	// auto t1 = Clock::now(); // a better way to time with higher precisions
	// std::clock_t t1_end=std::clock();
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n================================================================================================ ");
	std::cout << "Time to convert data to GRAPHLU time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");

	// ----------------------------------------------------------------------------------------------------
	// 								Initialize Jaccobian Matirx
	// ----------------------------------------------------------------------------------------------------

	//std::cout << "\n ======================== Initialize Jaccobian Matirx ======================== \n"<< std::endl;

	gettimeofday(&t2_st, 0);
	// Get Pcal and Qcal
	for (i=0; i<n; i++){ // clear from previous iteration
		Pcal[i] = 0;
		Qcal[i] = 0;

		// Calculate network injections
		for (j=ep[i]; j<ep[i+1]; j++){
			p=ei[j];
            //calculate cosine and sine term and store in array
            cosine_array[j] = cos(Va[i]-Va[p]);  
            sine_array[j] = sin(Va[i]-Va[p]); 
            Pcal[i] += Vm[i]*Vm[p]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);
			Qcal[i] += Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]);  
		}
	}
    //check Pcal and Qcal
	 //for (i=0; i<n; i++)
	 	//std::cout << "Pcal" << i << ":"  << Pcal[i]<<", Qcal"<< i << ":"  << Qcal[i] <<", LM "<<LM[i]<<", Vm "<<Vm[i]<<", Va "<<Va[i]<< ", Pb: "<<Pb[i]<<", Qb: "<<Qb[i]<<", Pn: "<<Pn[i]<<", Qn: "<<Qn[i]<< std::endl;
 //gettimeofday(&t2_end, 0);
	//seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	//useconds = t2_end.tv_usec  - t2_st.tv_usec;
	//printf("\n\n================================================================================================\n ");
	//std::cout << "Time for Pcal and Qcal Initialization: " << (seconds*1000000 + useconds) << " us" << std::endl;
	//printf("\n================================================================================================ \n\n");

	//std::cout << " ======================== get H,N,J,L matrices ========================"<<std::endl;
	//gettimeofday(&t2_st, 0);
 // Calculate deltaP and deltaQ to update Vm
	counter=0;
	for (i=0; i<n; i++){
   switch (btype[i]){
   case 0:  //PQ bus
   case 1:  //PQ bus
     // Get the values in the Jaccobian matrix
     for (j=ep[i]; j<ep[i+1]; j++){
		    p=ei[j];
		    // H[counter]=0; N[counter]=0; J[counter]=0; L[counter]=0;
		    if(i!=p){             
			  H[j] = Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]);
			  N[j] = Vm[i]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);
		      J[j] = -Vm[i]*Vm[p]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);
		      L[j] = Vm[i]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]);               
		    }
		    else{
		      H[j] = -Qcal[i] - eB[j]*Vm[i]*Vm[i];
		      N[j] = Pcal[i]/Vm[i] + eG[j]*Vm[i];
		      J[j] = Pcal[i] - eG[j]*Vm[i]*Vm[i];
		      L[j] = Qcal[i]/Vm[i] - eB[j]*Vm[i];
		    }
		    // std::cout << "J" << j << ": " << i << ", " << p << ", H, " << H[j] << ", N, " << N[j] << ", J, " << J[j] << ", L, " << L[j] << std::endl;
     }
     break;
   case 2:  //PV bus
     // deltaQ[i]=0; 
     for (j=ep[i]; j<ep[i+1]; j++){
		    p=ei[j];
		    if (p==i){ // For diagnal elements
		      H[j] = -Qcal[i] - eB[j]*Vm[i]*Vm[i];
		      N[j] = Pcal[i]/Vm[i] + eG[j]*Vm[i];
	          J[j]=0;
		      L[j]=1;
		    }
		    else{ // Off-diagnal elements        
              H[j] = Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]);
		      N[j] = Vm[i]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);          
		      J[j] = 0;
		      L[j ]= 0;
		    }
     }
     break;          
   case 3:  //slack bus
     for (j=ep[i]; j<ep[i+1]; j++){
		    p=ei[j];
		    if (p==i){ // For diagnal elements
		      H[j] = 1;
		      N[j] = 0;
		      J[j] = 0;
		      L[j] = 1;
		    }
		    else{ // Off-diagnal elements
		      H[j] = 0;
		      N[j] = 0;
			  J[j] = 0;
			  L[j] = 0;
		    }
     }
     break;
   default: //unknown bus type 
     std::cerr << "Unknown bus type at bus: " << i << " Type: " << btype[i] << std::endl;
     break;
   }      
	}

 //gettimeofday(&t2_end, 0);
	//seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	//useconds = t2_end.tv_usec  - t2_st.tv_usec;
	//printf("\n\n================================================================================================\n ");
	//std::cout << "Time for H, N, J, L Initialization: " << (seconds*1000000 + useconds) << " us" << std::endl;
	//printf("\n================================================================================================ \n\n");

	//std::cout << " ======================== Get the linked list structure for Jaccobian ========================"<<std::endl;
 //gettimeofday(&t2_st, 0); 
	counter=0;
	rowcnt=0;
	for (i=0; i<n; i++){ 
		// clen = ep[i+1] - ep[i] + 1; // number of nonzeros is row i

		// ------- Store H and N -------
		Jp[rowcnt] = counter; // Get the initial row pointer
		for (j=ep[i]; j<ep[i+1]; j++){
			p=ei[j];
			// Store H
			Jx[counter] = H[j]; Ji[counter] = 2*p; 
			counter += 1;
			// Store N
			Jx[counter] = N[j]; Ji[counter] = 2*p+1; 
			counter += 1;
		}
		rowcnt += 1;

		// ------- Store J and L -------
		Jp[rowcnt] = counter; // Get the initial row pointer
		for (j=ep[i]; j<ep[i+1]; j++){
			p=ei[j];
			// Store J
			Jx[counter] = J[j]; Ji[counter] = 2*p; 
			counter += 1;
			// Store L
			Jx[counter] = L[j]; Ji[counter] = 2*p+1; 
			counter += 1;
		}
		rowcnt += 1;
	}
	Jp[rowcnt] = counter; // Last entry in the array

	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	
	printf("\n\n================================================================================================\n ");
	std::cout << "Time for Jaccobian Initialization: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ \n\n");
	// ----------------------------------------------------------------------------------------------------
	// 								Call GRAPHLU and Initialize the GRAPHLU Structure
	// ----------------------------------------------------------------------------------------------------
	gettimeofday(&t2_st, 0);
	//std::cout << "\n ======================== Call GRAPHLU and Initialize the GRAPHLU Structure ======================== \n"<< std::endl;
	// Initialize the structure for GRAPHLU
	graphlu = (SGraphLU *)malloc(sizeof(SGraphLU));
	GraphLU_Initialize(graphlu);

	// Create Matrix
	GraphLU_CreateMatrix(graphlu, n_J, nnz_J, Jx, Ji, Jp);
	// graphlu->cfgf[0] = 1.;

	// // Set control parameters
	// graphlu->cfgi[1] = 0; // 0 is no MC64 scaling
	// 					 // 1 is with MC64 scaling

	// Analyze (column/row ordering, scaling. For details please refer to GRAPHLU user's guide)
	GraphLU_Analyze(graphlu);
	printf("analysis time: %.8g\n", graphlu->stat[0]);
	
 // [Ting][begin] comment out if using sequential factorization, no need to create schedule, create threads and bind threads
	// creates the task scheduler for parallel LU factorization. If you want to run 
 // parallel factorization or parallel re-factorization, it should be called after GraphLU Analyze.
	ret = GraphLU_CreateScheduler(graphlu);
	printf("time of creating scheduler: %.8g\n", graphlu->stat[4]);
    printf("suggestion: %s.\n", ret==0?"parallel":"sequential");
	
	// This function creates threads for parallel computation. The second argument (thread)
	// specifies the number of threads, including the main thread. The last argument (check)
	// specifies whether to check the number of threads or not.
    GraphLU_CreateThreads(graphlu, 2, TRUE);
    printf("total cores: %d, threads created: %d\n", (int)(graphlu->stat[9]), (int)(graphlu->cfgi[5]));
	
	// This function binds threads to cores (unbind = FALSE) or unbinds threads from cores (unbind = TRUE).
	GraphLU_BindThreads(graphlu, FALSE);
 // [Ting][end] comment out if using sequential factorization	
 
	// // Numerical LU factorization with partial pivoting, parallel
	// GraphLU_Factorize_MT(graphlu);
	// printf("factorization time: %.8g\n", graphlu->stat[1]);
 
	// std::cout << " ======================== Number of Total nonzeros after factorization is: "<< graphlu->lu_nnz << "========================"<<std::endl;
	// std::cout << " ======================== Number of nonzeros in L: "<< graphlu->l_nnz << "========================"<<std::endl;
	// std::cout << " ======================== Number of nonzeros in U: "<< graphlu->u_nnz << "========================"<<std::endl;
	// std::cout << " ======================== End of LU Factorization Routines ========================"<<std::endl;
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	
	printf("\n\n================================================================================================\n ");
	std::cout << "Time for GRAPHLU analysis and scheduling " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ \n\n");
 	
	//printf("\n\n================================================================================================ ");
	//std::cout << "\n Start iteratively updating deltaP and deltaQ " << std::endl;
	//printf("\n================================================================================================\n ");
    gettimeofday(&t2_st, 0);
	std::cout << std::fixed; // set fixed precisions
	for (iter=0; iter<maxiter; iter++){
		// update deltaP and deltaQ
		maxDeltaP=0;
		maxDeltaQ=0;
   
        gettimeofday(&t3_st, 0);
		// Get Pcal and Qcal
		for (i=0; i<n; i++){ // clear from previous iteration
			if (iter != 0){   //Pcal and Qcal are computed during initialization, no need to compute it again for iteration 0  
				Pcal[i] = 0;
				Qcal[i] = 0;
				// Calculate network injectionsgad
				for (j=ep[i]; j<ep[i+1]; j++){
					p=ei[j];
					//update consine and sine arrays with new angles          
					cosine_array[j] = cos(Va[i]-Va[p]);  
					sine_array[j] = sin(Va[i]-Va[p]);   
					Pcal[i] += Vm[i]*Vm[p]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);
					Qcal[i] += Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]);
			    }
			    //update Pn and Qn, for Y connections
				if (LM[i] == 1){  //Y-PQ
					Pn[i] = Pb[i];
					Qn[i] = Qb[i] + Cb[i]*Vm[i]*Vm[i];
				}
				else if (LM[i] == 2){  //Y-Z
					Pn[i] = Pb[i]*Vm[i]*Vm[i];
					Qn[i] = Qb[i]*Vm[i]*Vm[i] + Cb[i]*Vm[i]*Vm[i];
				}
				else if (LM[i] == 3){  //Y-I
					Pn[i] = Pb[i]*Vm[i];
					Qn[i] = Qb[i]*Vm[i] + Cb[i]*Vm[i]*Vm[i];
				}
				//update Pn and Qn, for Delta connections
				//first update the voltages 
				else if (LM[i] == 4){ 
					if (phaseFlag[i] == 4 &&  phaseN[i] == 1){ //phase AB, D-PQ
						VA_R = Vm[i]*cos(Va[i]);
						VA_I = Vm[i]*sin(Va[i]);
						VB_R = Vm[i+1]*cos(Va[i+1]);
						VB_I = Vm[i+1]*sin(Va[i+1]);
						VAB_R = VA_R - VB_R;
						VAB_I = VA_I - VB_I;
						VABm = sqrt(VAB_R*VAB_R + VAB_I*VAB_I);					
						PdAB_cal = Pb[i];
						QdAB_cal = Qb[i];
						IAB_R = (PdAB_cal*VAB_R + QdAB_cal*VAB_I)/(VABm*VABm);
						IAB_I = (PdAB_cal*VAB_I - QdAB_cal*VAB_R)/(VABm*VABm);
						IA_R = IAB_R;
						IA_I = IAB_I;
						IB_R = - IAB_R;
						IB_I = - IAB_I;
						PdA_cal = VA_R*IA_R + VA_I*IA_I;
						QdA_cal = VA_I*IA_R - VA_R*IA_I;
						PdB_cal = VB_R*IB_R + VB_I*IB_I;
						QdB_cal = VB_I*IB_R - VB_R*IB_I;
						Pn[i] = PdA_cal;
						Qn[i] = QdA_cal + Cb[i]*Vm[i]*Vm[i];
						Pn[i+1] = PdB_cal;
						Qn[i+1] = QdB_cal + Cb[i+1]*Vm[i+1]*Vm[i+1];
					}
					else if (phaseFlag[i] == 5 && phaseN[i] == 2){ //phase BC, D-PQ
						VB_R = Vm[i]*cos(Va[i]);
						VB_I = Vm[i]*sin(Va[i]);
						VC_R = Vm[i+1]*cos(Va[i+1]);
						VC_I = Vm[i+1]*sin(Va[i+1]);
						VBC_R = VB_R - VC_R;
						VBC_I = VB_I - VC_I;
						VBCm = sqrt(VBC_R*VBC_R + VBC_I*VBC_I);					
						PdBC_cal = Pb[i];
						QdBC_cal = Qb[i];
						IBC_R = (PdBC_cal*VBC_R + QdBC_cal*VBC_I)/(VBCm*VBCm);
						IBC_I = (PdBC_cal*VBC_I - QdBC_cal*VBC_R)/(VBCm*VBCm);
						IB_R = IBC_R;
						IB_I = IBC_I;
						IC_R = - IBC_R;
						IC_I = - IBC_I;
						PdB_cal = VB_R*IB_R + VB_I*IB_I;
						QdB_cal = VB_I*IB_R - VB_R*IB_I;
						PdC_cal = VC_R*IC_R + VC_I*IC_I;
						QdC_cal = VC_I*IC_R - VC_R*IC_I;
						Pn[i] = PdB_cal;
						Qn[i] = QdB_cal + Cb[i]*Vm[i]*Vm[i];
						Pn[i+1] = PdC_cal;
						Qn[i+1] = QdC_cal + Cb[i+1]*Vm[i+1]*Vm[i+1];
					}
					else if (phaseFlag[i] == 6 && phaseN[i] == 1){ //phase CA, D-PQ
						VA_R = Vm[i]*cos(Va[i]);
						VA_I = Vm[i]*sin(Va[i]);
						VC_R = Vm[i+1]*cos(Va[i+1]);
						VC_I = Vm[i+1]*sin(Va[i+1]);
						VCA_R = VC_R - VA_R;
						VCA_I = VC_I - VA_I;
						VCAm = sqrt(VCA_R*VCA_R + VCA_I*VCA_I);					
						PdCA_cal = Pb[i];
						QdCA_cal = Qb[i];
						ICA_R = (PdCA_cal*VCA_R + QdCA_cal*VCA_I)/(VCAm*VCAm);
						ICA_I = (PdCA_cal*VCA_I - QdCA_cal*VCA_R)/(VCAm*VCAm);
						IC_R = ICA_R;
						IC_I = ICA_I;
						IA_R = - ICA_R;
						IA_I = - ICA_I;
						PdC_cal = VC_R*IC_R + VC_I*IC_I;
						QdC_cal = VC_I*IC_R - VC_R*IC_I;
						PdA_cal = VA_R*IA_R + VA_I*IA_I;
						QdA_cal = VA_I*IA_R - VA_R*IA_I;
						Pn[i] = PdA_cal;
						Qn[i] = QdA_cal + Cb[i]*Vm[i]*Vm[i];
						Pn[i+1] = PdC_cal;
						Qn[i+1] = QdC_cal + Cb[i+1]*Vm[i+1]*Vm[i+1];
					}
					else if (phaseFlag[i] == 7 && phaseN[i] == 1){ //phase ABC, D-PQ
						VA_R = Vm[i]*cos(Va[i]);
						VA_I = Vm[i]*sin(Va[i]);
						VB_R = Vm[i+1]*cos(Va[i+1]);
						VB_I = Vm[i+1]*sin(Va[i+1]);
						VC_R = Vm[i+2]*cos(Va[i+2]);
						VC_I = Vm[i+2]*sin(Va[i+2]);
						VAB_R = VA_R - VB_R;
						VAB_I = VA_I - VB_I;
						VBC_R = VB_R - VC_R;
						VBC_I = VB_I - VC_I;
						VCA_R = VC_R - VA_R;
						VCA_I = VC_I - VA_I;
						VABm = sqrt(VAB_R*VAB_R + VAB_I*VAB_I);
						VBCm = sqrt(VBC_R*VBC_R + VBC_I*VBC_I);	
						VCAm = sqrt(VCA_R*VCA_R + VCA_I*VCA_I);					
						PdAB_cal = Pb[i];
						QdAB_cal = Qb[i];
						PdBC_cal = Pb[i+1];
						QdBC_cal = Qb[i+1];
						PdCA_cal = Pb[i+2];
						QdCA_cal = Qb[i+2];
						IAB_R = (PdAB_cal*VAB_R + QdAB_cal*VAB_I)/(VABm*VABm);
						IAB_I = (PdAB_cal*VAB_I - QdAB_cal*VAB_R)/(VABm*VABm);
						IBC_R = (PdBC_cal*VBC_R + QdBC_cal*VBC_I)/(VBCm*VBCm);
						IBC_I = (PdBC_cal*VBC_I - QdBC_cal*VBC_R)/(VBCm*VBCm);
						ICA_R = (PdCA_cal*VCA_R + QdCA_cal*VCA_I)/(VCAm*VCAm);
						ICA_I = (PdCA_cal*VCA_I - QdCA_cal*VCA_R)/(VCAm*VCAm);
						IA_R = IAB_R - ICA_R;
						IA_I = IAB_I - ICA_I;
						IB_R = IBC_R - IAB_R;
						IB_I = IBC_I - IAB_I;
						IC_R = ICA_R - IBC_R;
						IC_I = ICA_I - IBC_I;
						PdA_cal = VA_R*IA_R + VA_I*IA_I;
						QdA_cal = VA_I*IA_R - VA_R*IA_I;
						PdB_cal = VB_R*IB_R + VB_I*IB_I;
						QdB_cal = VB_I*IB_R - VB_R*IB_I;
						PdC_cal = VC_R*IC_R + VC_I*IC_I;
						QdC_cal = VC_I*IC_R - VC_R*IC_I;
						Pn[i] = PdA_cal;
						Qn[i] = QdA_cal + Cb[i]*Vm[i]*Vm[i];
						Pn[i+1] = PdB_cal;
						Qn[i+1] = QdB_cal + Cb[i+1]*Vm[i+1]*Vm[i+1];
						Pn[i+2] = PdC_cal;
						Qn[i+2] = QdC_cal + Cb[i+2]*Vm[i+1]*Vm[i+1];
					}
				}
				else if (LM[i] == 5){ 
					if (phaseFlag[i] == 4 &&  phaseN[i] == 1){ //phase AB, D-Z
						VA_R = Vm[i]*cos(Va[i]);
						VA_I = Vm[i]*sin(Va[i]);
						VB_R = Vm[i+1]*cos(Va[i+1]);
						VB_I = Vm[i+1]*sin(Va[i+1]);
						VAB_R = VA_R - VB_R;
						VAB_I = VA_I - VB_I;
						VABm = sqrt(VAB_R*VAB_R + VAB_I*VAB_I);					
						PdAB_cal = Pb[i]*VABm*VABm/3;
						QdAB_cal = Qb[i]*VABm*VABm/3;
						IAB_R = (PdAB_cal*VAB_R + QdAB_cal*VAB_I)/(VABm*VABm);
						IAB_I = (PdAB_cal*VAB_I - QdAB_cal*VAB_R)/(VABm*VABm);
						IA_R = IAB_R;
						IA_I = IAB_I;
						IB_R = - IAB_R;
						IB_I = - IAB_I;
						PdA_cal = VA_R*IA_R + VA_I*IA_I;
						QdA_cal = VA_I*IA_R - VA_R*IA_I;
						PdB_cal = VB_R*IB_R + VB_I*IB_I;
						QdB_cal = VB_I*IB_R - VB_R*IB_I;
						Pn[i] = PdA_cal;
						Qn[i] = QdA_cal + Cb[i]*Vm[i]*Vm[i];
						Pn[i+1] = PdB_cal;
						Qn[i+1] = QdB_cal + Cb[i+1]*Vm[i+1]*Vm[i+1];
					}
					else if (phaseFlag[i] == 5 && phaseN[i] == 2){ //phase BC, D-Z
						VB_R = Vm[i]*cos(Va[i]);
						VB_I = Vm[i]*sin(Va[i]);
						VC_R = Vm[i+1]*cos(Va[i+1]);
						VC_I = Vm[i+1]*sin(Va[i+1]);
						VBC_R = VB_R - VC_R;
						VBC_I = VB_I - VC_I;
						VBCm = sqrt(VBC_R*VBC_R + VBC_I*VBC_I);					
						PdBC_cal = Pb[i]*VBCm*VBCm/3;
						QdBC_cal = Qb[i]*VBCm*VBCm/3;
						IBC_R = (PdBC_cal*VBC_R + QdBC_cal*VBC_I)/(VBCm*VBCm);
						IBC_I = (PdBC_cal*VBC_I - QdBC_cal*VBC_R)/(VBCm*VBCm);
						IB_R = IBC_R;
						IB_I = IBC_I;
						IC_R = - IBC_R;
						IC_I = - IBC_I;
						PdB_cal = VB_R*IB_R + VB_I*IB_I;
						QdB_cal = VB_I*IB_R - VB_R*IB_I;
						PdC_cal = VC_R*IC_R + VC_I*IC_I;
						QdC_cal = VC_I*IC_R - VC_R*IC_I;
						Pn[i] = PdB_cal;
						Qn[i] = QdB_cal + Cb[i]*Vm[i]*Vm[i];
						Pn[i+1] = PdC_cal;
						Qn[i+1] = QdC_cal + Cb[i+1]*Vm[i+1]*Vm[i+1];
					}
					else if (phaseFlag[i] == 6 && phaseN[i] == 1){ //phase CA, D-Z
						VA_R = Vm[i]*cos(Va[i]);
						VA_I = Vm[i]*sin(Va[i]);
						VC_R = Vm[i+1]*cos(Va[i+1]);
						VC_I = Vm[i+1]*sin(Va[i+1]);
						VCA_R = VC_R - VA_R;
						VCA_I = VC_I - VA_I;
						VCAm = sqrt(VCA_R*VCA_R + VCA_I*VCA_I);					
						PdCA_cal = Pb[i]*VCAm*VCAm/3;
						QdCA_cal = Qb[i]*VCAm*VCAm/3;
						ICA_R = (PdCA_cal*VCA_R + QdCA_cal*VCA_I)/(VCAm*VCAm);
						ICA_I = (PdCA_cal*VCA_I - QdCA_cal*VCA_R)/(VCAm*VCAm);
						IC_R = ICA_R;
						IC_I = ICA_I;
						IA_R = - ICA_R;
						IA_I = - ICA_I;
						PdC_cal = VC_R*IC_R + VC_I*IC_I;
						QdC_cal = VC_I*IC_R - VC_R*IC_I;
						PdA_cal = VA_R*IA_R + VA_I*IA_I;
						QdA_cal = VA_I*IA_R - VA_R*IA_I;
						Pn[i] = PdA_cal;
						Qn[i] = QdA_cal + Cb[i]*Vm[i]*Vm[i];
						Pn[i+1] = PdC_cal;
						Qn[i+1] = QdC_cal + Cb[i+1]*Vm[i+1]*Vm[i+1];
					}
					else if (phaseFlag[i] == 7 && phaseN[i] == 1){ //phase ABC, D-Z
						VA_R = Vm[i]*cos(Va[i]);
						VA_I = Vm[i]*sin(Va[i]);
						VB_R = Vm[i+1]*cos(Va[i+1]);
						VB_I = Vm[i+1]*sin(Va[i+1]);
						VC_R = Vm[i+2]*cos(Va[i+2]);
						VC_I = Vm[i+2]*sin(Va[i+2]);
						VAB_R = VA_R - VB_R;
						VAB_I = VA_I - VB_I;
						VBC_R = VB_R - VC_R;
						VBC_I = VB_I - VC_I;
						VCA_R = VC_R - VA_R;
						VCA_I = VC_I - VA_I;
						VABm = sqrt(VAB_R*VAB_R + VAB_I*VAB_I);
						VBCm = sqrt(VBC_R*VBC_R + VBC_I*VBC_I);	
						VCAm = sqrt(VCA_R*VCA_R + VCA_I*VCA_I);					
						PdAB_cal = Pb[i]*VABm*VABm/3;
						QdAB_cal = Qb[i]*VABm*VABm/3;
						PdBC_cal = Pb[i+1]*VBCm*VBCm/3;
						QdBC_cal = Qb[i+1]*VBCm*VBCm/3;
						PdCA_cal = Pb[i+2]*VCAm*VCAm/3;
						QdCA_cal = Qb[i+2]*VCAm*VCAm/3;
						IAB_R = (PdAB_cal*VAB_R + QdAB_cal*VAB_I)/(VABm*VABm);
						IAB_I = (PdAB_cal*VAB_I - QdAB_cal*VAB_R)/(VABm*VABm);
						IBC_R = (PdBC_cal*VBC_R + QdBC_cal*VBC_I)/(VBCm*VBCm);
						IBC_I = (PdBC_cal*VBC_I - QdBC_cal*VBC_R)/(VBCm*VBCm);
						ICA_R = (PdCA_cal*VCA_R + QdCA_cal*VCA_I)/(VCAm*VCAm);
						ICA_I = (PdCA_cal*VCA_I - QdCA_cal*VCA_R)/(VCAm*VCAm);
						IA_R = IAB_R - ICA_R;
						IA_I = IAB_I - ICA_I;
						IB_R = IBC_R - IAB_R;
						IB_I = IBC_I - IAB_I;
						IC_R = ICA_R - IBC_R;
						IC_I = ICA_I - IBC_I;
						PdA_cal = VA_R*IA_R + VA_I*IA_I;
						QdA_cal = VA_I*IA_R - VA_R*IA_I;
						PdB_cal = VB_R*IB_R + VB_I*IB_I;
						QdB_cal = VB_I*IB_R - VB_R*IB_I;
						PdC_cal = VC_R*IC_R + VC_I*IC_I;
						QdC_cal = VC_I*IC_R - VC_R*IC_I;
						Pn[i] = PdA_cal;
						Qn[i] = QdA_cal + Cb[i]*Vm[i]*Vm[i];
						Pn[i+1] = PdB_cal;
						Qn[i+1] = QdB_cal + Cb[i+1]*Vm[i+1]*Vm[i+1];
						Pn[i+2] = PdC_cal;
						Qn[i+2] = QdC_cal + Cb[i+2]*Vm[i+2]*Vm[i+2];
					}
				}
				else if (LM[i] == 6){ 
					if (phaseFlag[i] == 4 &&  phaseN[i] == 1){ //phase AB, D-I
						VA_R = Vm[i]*cos(Va[i]);
						VA_I = Vm[i]*sin(Va[i]);
						VB_R = Vm[i+1]*cos(Va[i+1]);
						VB_I = Vm[i+1]*sin(Va[i+1]);
						VAB_R = VA_R - VB_R;
						VAB_I = VA_I - VB_I;
						VABm = sqrt(VAB_R*VAB_R + VAB_I*VAB_I);					
						PdAB_cal = Pb[i]*VABm/sqrt(3);
						QdAB_cal = Qb[i]*VABm/sqrt(3);
						IAB_R = (PdAB_cal*VAB_R + QdAB_cal*VAB_I)/(VABm*VABm);
						IAB_I = (PdAB_cal*VAB_I - QdAB_cal*VAB_R)/(VABm*VABm);
						IA_R = IAB_R;
						IA_I = IAB_I;
						IB_R = - IAB_R;
						IB_I = - IAB_I;
						PdA_cal = VA_R*IA_R + VA_I*IA_I;
						QdA_cal = VA_I*IA_R - VA_R*IA_I;
						PdB_cal = VB_R*IB_R + VB_I*IB_I;
						QdB_cal = VB_I*IB_R - VB_R*IB_I;
						Pn[i] = PdA_cal;
						Qn[i] = QdA_cal + Cb[i]*Vm[i]*Vm[i];
						Pn[i+1] = PdB_cal;
						Qn[i+1] = QdB_cal + Cb[i+1]*Vm[i+1]*Vm[i+1];
					}
					else if (phaseFlag[i] == 5 && phaseN[i] == 2){ //phase BC, D-I
						VB_R = Vm[i]*cos(Va[i]);
						VB_I = Vm[i]*sin(Va[i]);
						VC_R = Vm[i+1]*cos(Va[i+1]);
						VC_I = Vm[i+1]*sin(Va[i+1]);
						VBC_R = VB_R - VC_R;
						VBC_I = VB_I - VC_I;
						VBCm = sqrt(VBC_R*VBC_R + VBC_I*VBC_I);					
						PdBC_cal = Pb[i]*VBCm/sqrt(3);
						QdBC_cal = Qb[i]*VBCm/sqrt(3);
						IBC_R = (PdBC_cal*VBC_R + QdBC_cal*VBC_I)/(VBCm*VBCm);
						IBC_I = (PdBC_cal*VBC_I - QdBC_cal*VBC_R)/(VBCm*VBCm);
						IB_R = IBC_R;
						IB_I = IBC_I;
						IC_R = - IBC_R;
						IC_I = - IBC_I;
						PdB_cal = VB_R*IB_R + VB_I*IB_I;
						QdB_cal = VB_I*IB_R - VB_R*IB_I;
						PdC_cal = VC_R*IC_R + VC_I*IC_I;
						QdC_cal = VC_I*IC_R - VC_R*IC_I;
						Pn[i] = PdB_cal;
						Qn[i] = QdB_cal + Cb[i]*Vm[i]*Vm[i];
						Pn[i+1] = PdC_cal;
						Qn[i+1] = QdC_cal + Cb[i+1]*Vm[i+1]*Vm[i+1];
					}
					else if (phaseFlag[i] == 6 && phaseN[i] == 1){ //phase CA, D-I
						VA_R = Vm[i]*cos(Va[i]);
						VA_I = Vm[i]*sin(Va[i]);
						VC_R = Vm[i+1]*cos(Va[i+1]);
						VC_I = Vm[i+1]*sin(Va[i+1]);
						VCA_R = VC_R - VA_R;
						VCA_I = VC_I - VA_I;
						VCAm = sqrt(VCA_R*VCA_R + VCA_I*VCA_I);					
						PdCA_cal = Pb[i]*VCAm/sqrt(3);
						QdCA_cal = Qb[i]*VCAm/sqrt(3);
						ICA_R = (PdCA_cal*VCA_R + QdCA_cal*VCA_I)/(VCAm*VCAm);
						ICA_I = (PdCA_cal*VCA_I - QdCA_cal*VCA_R)/(VCAm*VCAm);
						IC_R = ICA_R;
						IC_I = ICA_I;
						IA_R = - ICA_R;
						IA_I = - ICA_I;
						PdC_cal = VC_R*IC_R + VC_I*IC_I;
						QdC_cal = VC_I*IC_R - VC_R*IC_I;
						PdA_cal = VA_R*IA_R + VA_I*IA_I;
						QdA_cal = VA_I*IA_R - VA_R*IA_I;
						Pn[i] = PdA_cal;
						Qn[i] = QdA_cal + Cb[i]*Vm[i]*Vm[i];
						Pn[i+1] = PdC_cal;
						Qn[i+1] = QdC_cal + Cb[i+1]*Vm[i+1]*Vm[i+1];
					}
					else if (phaseFlag[i] == 7 && phaseN[i] == 1){ //phase ABC, D-I
						VA_R = Vm[i]*cos(Va[i]);
						VA_I = Vm[i]*sin(Va[i]);
						VB_R = Vm[i+1]*cos(Va[i+1]);
						VB_I = Vm[i+1]*sin(Va[i+1]);
						VC_R = Vm[i+2]*cos(Va[i+2]);
						VC_I = Vm[i+2]*sin(Va[i+2]);
						VAB_R = VA_R - VB_R;
						VAB_I = VA_I - VB_I;
						VBC_R = VB_R - VC_R;
						VBC_I = VB_I - VC_I;
						VCA_R = VC_R - VA_R;
						VCA_I = VC_I - VA_I;
						VABm = sqrt(VAB_R*VAB_R + VAB_I*VAB_I);
						VBCm = sqrt(VBC_R*VBC_R + VBC_I*VBC_I);	
						VCAm = sqrt(VCA_R*VCA_R + VCA_I*VCA_I);					
						PdAB_cal = Pb[i]*VABm/sqrt(3);
						QdAB_cal = Qb[i]*VABm/sqrt(3);
						PdBC_cal = Pb[i+1]*VBCm/sqrt(3);
						QdBC_cal = Qb[i+1]*VBCm/sqrt(3);
						PdCA_cal = Pb[i+2]*VCAm/sqrt(3);
						QdCA_cal = Qb[i+2]*VCAm/sqrt(3);
						IAB_R = (PdAB_cal*VAB_R + QdAB_cal*VAB_I)/(VABm*VABm);
						IAB_I = (PdAB_cal*VAB_I - QdAB_cal*VAB_R)/(VABm*VABm);
						IBC_R = (PdBC_cal*VBC_R + QdBC_cal*VBC_I)/(VBCm*VBCm);
						IBC_I = (PdBC_cal*VBC_I - QdBC_cal*VBC_R)/(VBCm*VBCm);
						ICA_R = (PdCA_cal*VCA_R + QdCA_cal*VCA_I)/(VCAm*VCAm);
						ICA_I = (PdCA_cal*VCA_I - QdCA_cal*VCA_R)/(VCAm*VCAm);
						IA_R = IAB_R - ICA_R;
						IA_I = IAB_I - ICA_I;
						IB_R = IBC_R - IAB_R;
						IB_I = IBC_I - IAB_I;
						IC_R = ICA_R - IBC_R;
						IC_I = ICA_I - IBC_I;
						PdA_cal = VA_R*IA_R + VA_I*IA_I;
						QdA_cal = VA_I*IA_R - VA_R*IA_I;
						PdB_cal = VB_R*IB_R + VB_I*IB_I;
						QdB_cal = VB_I*IB_R - VB_R*IB_I;
						PdC_cal = VC_R*IC_R + VC_I*IC_I;
						QdC_cal = VC_I*IC_R - VC_R*IC_I;
						Pn[i] = PdA_cal;
						Qn[i] = QdA_cal + Cb[i]*Vm[i]*Vm[i];
						Pn[i+1] = PdB_cal;
						Qn[i+1] = QdB_cal + Cb[i+1]*Vm[i+1]*Vm[i+1];
						Pn[i+2] = PdC_cal;
						Qn[i+2] = QdC_cal + Cb[i+2]*Vm[i+2]*Vm[i+2];
					}
				} 
			
            }
			// check output
			// std::cout << "Iter" << iter << ", Pcal," << i << ", " << std::setprecision(8) << Pcal[i]<< ", Qcal" << i << ", " << std::setprecision(8) << Qcal[i]<< std::endl;
     
			 //calculate deltaP and deltaQ based on bus type
			 switch (btype[i]){
			 case 0:  //PQ bus
			 case 1:  //PQ bus
			   deltaP[i] = Pn[i] - Pcal[i];
			   deltaQ[i] = Qn[i] - Qcal[i];
			   break;  
			 case 2:  //PV bus
			   deltaP[i] = Pn[i] - Pcal[i];
			   deltaQ[i] = 0;
			   break;            
			 case 3:  //slack bus
			   deltaP[i] = 0;
			   deltaQ[i] = 0;
			   break;
			 default: //unknown bus type  
			   std::cerr << "Unknown bus type at bus: " << i << " Type: " << btype[i] << std::endl;
			   break;
            }          

			// Get max P and max Q
			if (abs(deltaP[i]) > maxDeltaP)
				maxDeltaP = abs(deltaP[i]);
			if (abs(deltaQ[i]) > maxDeltaQ)
				maxDeltaQ = abs(deltaQ[i]);

			// store deltaP and deltaQ into the b vector
			deltaX[2*i]=deltaP[i];
			deltaX[2*i+1]=deltaQ[i];

		}
		gettimeofday(&t3_end, 0);
		seconds=t3_end.tv_sec  - t3_st.tv_sec; 
		useconds = t3_end.tv_usec  - t3_st.tv_usec;
		
		printf("\n\n================================================================================================\n ");
		std::cout << "Time for Pcal and Qcal, max mismatch loop (iteration): " << iter << ": " << (seconds*1000000 + useconds) << " us" << std::endl;
		printf("\n================================================================================================ \n\n");
			// keep track of max deltaP and max deltaQ in each iteration
	    maxDltP_list.push_back(maxDeltaP);
	    maxDltQ_list.push_back(maxDeltaQ);

		// Decide if the program converges	
		if ( maxDeltaP < max_change_ex && maxDeltaQ < max_change_ex){
			break;
		}

		if (iter!=0){ // Since the matrix is already initilized, no need to compute it again in the first NR iteration
			// std::cout << " ======================== Get H,N,J,L matrices ========================"<<std::endl;
            gettimeofday(&t3_st, 0);
			// Calculate deltaP and deltaQ to update Vm
			counter=0;
			for (i=0; i<n; i++){
			   switch (btype[i]){
				   case 0:  //PQ bus
				   case 1:  //PQ bus
					 // Get the values in the Jaccobian matrix
						for (j=ep[i]; j<ep[i+1]; j++){
								  p=ei[j];
								  // H[counter]=0; N[counter]=0; J[counter]=0; L[counter]=0;
								  if(i!=p){             
									H[j] = Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]);
									N[j] = Vm[i]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);
									J[j] = -Vm[i]*Vm[p]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);
									L[j] = Vm[i]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]);               
								  }
								  else{
									H[j] = -Qcal[i] - eB[j]*Vm[i]*Vm[i];
									N[j] = Pcal[i]/Vm[i] + eG[j]*Vm[i];
									J[j] = Pcal[i] - eG[j]*Vm[i]*Vm[i];
									L[j] = Qcal[i]/Vm[i] - eB[j]*Vm[i];
								  }
								  // std::cout << "J" << j << ": " << i << ", " << p << ", H, " << H[j] << ", N, " << N[j] << ", J, " << J[j] << ", L, " << L[j] << std::endl;
						}
					 break;
				   case 2:  //PV bus
					 // deltaQ[i]=0; 
						for (j=ep[i]; j<ep[i+1]; j++){
								  p=ei[j];
								  if (p==i){ // For diagnal elements
									H[j] = -Qcal[i] - eB[j]*Vm[i]*Vm[i];
									N[j] = Pcal[i]/Vm[i] + eG[j]*Vm[i];
									J[j] = 0;
									L[j] = 1;
								  }
								  else{ // Off-diagnal elements        
									H[j] = Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]);
									N[j] = Vm[i]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);          
									J[j] = 0;
									L[j] = 0;
								  }
						}
					 break;          
				   case 3:  //slack bus
							for (j=ep[i]; j<ep[i+1]; j++){
								  p=ei[j];
								  if (p==i){ // For diagnal elements
									H[j] = 1;
									N[j] = 0;
									J[j] = 0;
									L[j] = 1;
								  }
								  else{ // Off-diagnal elements
									H[j] = 0;
									N[j] = 0;
									J[j] = 0;
									L[j] = 0;
								  }
						}
					 break;
				   default:  //unknown bus type
							std::cerr << "Unknown bus type at bus: " << i << " Type: " << btype[i] << std::endl;
					 break;
			   }      
			}
	    gettimeofday(&t3_end, 0);
	    seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	    useconds = t3_end.tv_usec  - t3_st.tv_usec;
	
	    printf("\n\n================================================================================================\n ");
	    std::cout << "Time for getting H,N,J,L matrices (iteration): " << iter << ": " << (seconds*1000000 + useconds) << " us" << std::endl;
	    printf("\n================================================================================================ \n\n");
			
     // std::cout << " ======================== Update the values in Jaccobian ========================"<<std::endl;
			counter=0;
			rowcnt=0;
        gettimeofday(&t3_st, 0);
			for (i=0; i<n; i++){ 
				// clen = ep[i+1] - ep[i] + 1; // number of nonzeros is row i

				// Store H and N
				for (j=ep[i]; j<ep[i+1]; j++){
					p=ei[j];
					// Store H
					Jx[counter] = H[j];
					counter += 1;

					// Store N
					Jx[counter] = N[j];
					counter += 1;
				}

				// Store J and L
				for (j=ep[i]; j<ep[i+1]; j++){
					p=ei[j];
					// Store J
					Jx[counter] = J[j];
					counter += 1;

					// Store L
					Jx[counter] = L[j];
					counter += 1;
				}
			}
     
	    gettimeofday(&t3_end, 0);
	    seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	    useconds = t3_end.tv_usec  - t3_st.tv_usec;
	
	    printf("\n\n================================================================================================\n ");
	    std::cout << "Time for updating the values in Jaccobian (iteration): " << iter << ": " << (seconds*1000000 + useconds) << " us" << std::endl;
	    printf("\n================================================================================================ \n\n");
		}

		// ----------------------------- Refactorize the Jaccobian Matrix -----------------------------
		// // std::cout << " ======================== Refactorize the Jaccobian Matrix ========================"<<std::endl;
		// Code below performs partial pivoting, if no pivoting is needed, use the next block which is commented out
		GraphLU_ResetMatrixValues(graphlu, Jx);
		if (!ret)
          GraphLU_Factorize_MT(graphlu);
        else
          GraphLU_Factorize(graphlu);  
		printf("Factorization time: %.8g\n", graphlu->stat[1]);
   
   // without doing reset matrix values and use refactorize with no pivoting, use code below
   /*
   if (!ret){    // Multi-threaded factorization and refactorization (no pivoting)
		  if (iter==0){
		  	GraphLU_Factorize_MT(graphlu);
		  	printf("Factorization time: %.8g\n", graphlu->stat[1]);
		  }	
		  else{
		  	GraphLU_ReFactorize_MT(graphlu, Jx);
		  	printf("Refactorization time: %.8g\n", graphlu->stat[2]);
		  }
   }   
 	else{    // sequential factorization and refactorization (no pivoting)
		  if (iter==0){
		  	GraphLU_Factorize(graphlu);
		  	printf("Factorization time: %.8g\n", graphlu->stat[1]);
		  }	
		  else{
		  	GraphLU_ReFactorize(graphlu, Jx);
		  	printf("Refactorization time: %.8g\n", graphlu->stat[2]);
		  }
   }
   */    
		// solve for deltaVm and deltaVa
		// std::cout << " ======================== Solve for V angle and V magnitude ========================"<<std::endl;
		GraphLU_Solve(graphlu, deltaX); // This is full forward/backward
   //GraphLU_SolveFast(graphlu, deltaX); // This is fast forward/backward, which uses sparse vector. This is fast when there are many zeros in b
		printf("substitution time: %.8g\n", graphlu->stat[3]); 
		// 

		// Get values of deltaP and deltaQ, update Vm and Va
		printf("\n\n================================================================================================\n ");
		for (i=0; i<n; i++){
			deltaP[i] = deltaX[2*i];
			deltaQ[i] = deltaX[2*i+1];
            
			Va[i] += deltaP[i];
			Vm[i] += deltaQ[i];
			
		}
		printf("\n================================================================================================ \n\n");
	}

	// // show the max mismatch in each iteration
 for( i=0; i<maxDltP_list.size(); i++){
	 	std::cout << "Iter" << i << ", Max MismatchP, " << std::setprecision(10) << maxDltP_list[i] << std::endl;
	 	std::cout << "Iter" << i << ", Max MismatchQ, " << std::setprecision(10) << maxDltQ_list[i] << std::endl;
 }

	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	
	printf("\n\n================================================================================================\n ");
	std::cout << "Time for factorization and power flow iteration: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ \n\n");


	// ----------------------------------------------------------------------------------------------------
	// 								Store the Result back to the array
	// ----------------------------------------------------------------------------------------------------
	for (ii=0; ii<n; ii++) {
     gVertex.data_[ii].Va = Va[ii]/pi_value*180;
     gVertex.data_[ii].Vm = Vm[ii];
     //std::cout << "Final, " << ii+1 << ", Vm, " << Vm[ii]<< ", Va" << ii+1 << ", " << Va[ii]/pi_value*180<< std::endl;
	} 

EXIT:
	// std::cout << "\n free1 " << std::endl;
	GraphLU_Destroy(graphlu);
	// GraphLU_Destroy(graphlu_pp);
	// std::cout << "\n free2 " << std::endl;
	// free(ax);
	// free(ai);
	// free(ap);
	// free(ax_pp);
	// free(ai_pp);
	// free(ap_pp);
	free(graphlu);
	// free(graphlu_pp);
	// std::cout << "\n free3 " << std::endl;

	free(H);
	free(N);
	free(J);
	free(L);

	free(Jx);
	free(Ji);
	free(Jp);

	free(eG);
	free(eB);
	free(ei);
	free(ep);

	free(deltaP);
	free(deltaQ);
	free(deltaX);
	free(Vm);
	free(Va);		
	free(Pn);
	free(Qn);
    free(Pb);
	free(Qb);
	free(Cb);
	free(Pcal);
	free(Qcal);
	free(LM);
	free(phaseN);
	free(phaseFlag);
	free(btype);	
	free(cosine_array);
	free(sine_array);

	printf("\n\n----------------------------------------------------------------------------------------");
	printf("\t\t End of Running GRAPHLU_NRpf_v4 C function!");
	printf("\t\t ----------------------------------------------------------------------------------------\n\n");	
} 
//
// **********************************************************************************************************************************
// Created by: Ting Chan, ting.chan@geirina.net
// Date: 8/31/2016
// This code performs Newton Raphson power flow analysis. It is used as a benchmark to assess the time it takes to run a Newton Raphson
// power flow in C++ with GRAPHLU. This version (v4) takes HeapAccum as inputs instead of MapAccum. The input HeapAcuum is assumed to be
// unsorted, so sorting them is needed before assigning the values to local arrays. Differences between this version (v4) and v2 is 
// the number of input HeapAccum. In this version, only 2 heapAccums are needed (vertex and edge). With fewer HeapAccums, the sorting
// time is reduced. This version does not require calling GRAPHLU_build_Ap2 for the pointer structures (i.e. ep). This version (v4) is
// based on v3 with additional changes made to introduce arrays to store the sine and cosine terms of the angle difference. The calculation
// of these terms are done at Jacobian initialization and at each iteration to reduce computational effort
// History:
// 2016-09-20: [Ting Chan] Use sequential or parallel factorization based on the outcome of GraphLU_CreateScheduler. 
// **********************************************************************************************************************************
// Updated by: Jun Tan, jun.tan@geirina.net
// Date: 4/10/2018
// update NR solver for the application in power distribution system power flow 
// **********************************************************************************************************************************
template <typename T_vertex_tuple1, typename vertex_comp1, typename T_edge_tuple1, typename edge_comp1>
// template <typename TUP2, typename COMP2>
inline void GRAPHLU_NRpf_v4 (HeapAccum<T_vertex_tuple1, vertex_comp1> &gVertex, HeapAccum<T_edge_tuple1, edge_comp1> &gEdge, MaxAccum<int> maxiter, MaxAccum<double> maxchange) {

	// extern size_t g_si, g_sd, g_sp;
	printf("\n\n------------------------------------------------------------------------------------------- ");
	printf("\nStart Running GRAPHLU_NRpf_v4 C function!\n");
	printf("------------------------------------------------------------------------------------------- ");
	// ------------------------------------------------------------------------------------------------
	// 				Initialize variables and arrays
	// ------------------------------------------------------------------------------------------------
	
	// Initialize arrays and variables
	uint__t n, nnz, n_pp, nnz_pp, n_J, nnz_J;
	int ret, i, j, p, iter, counter, rowcnt, clen;
	real__t maxDeltaP=0, maxDeltaQ=0, max_change_ex=maxchange;
	SGraphLU *graphlu;

	const double pi_value = 3.141592653589793;
	// real__t *x, err;	
	// uint__t fctnnz; // number of nonzeros after the matrix is factorized

	// // Get the dimension and the nnz of the matrix
	n= gVertex.data_.size(); 	nnz= gEdge.data_.size(); // get the size of the Y bus addmittance
	n_J=2*n;	nnz_J=4*nnz;
	
	//// Convert vectors to GRAPHLU Data Structure (pointers)
	real__t *eG, *eB, *deltaP, *deltaQ, *b, *Vm, *Va, *Pn, *Qn, *H,*N, *J, *L, *Pcal, *Qcal, *Jx, *deltaX; 
	uint__t *ei, *piv, *btype, *Ji; 
	uint__t *ep, *Jp; 

 // arrays to store the sine and cosine terms of the angle difference
 // values are computed during Jacobian initialization and each iteration 
 real__t *cosine_array, *sine_array;
 cosine_array = (real__t *)malloc(sizeof(real__t)*(nnz));
 sine_array = (real__t *)malloc(sizeof(real__t)*(nnz));
 
	eG = (real__t *)malloc(sizeof(real__t)*(nnz)); // G values in Y 
	eB = (real__t *)malloc(sizeof(real__t)*(nnz)); // B values in Y
	ei = (uint__t *)malloc(sizeof(uint__t)*(nnz)); // column indices of Y
	ep = (uint__t *)malloc(sizeof(uint__t)*(n+1)); // initial row pointers	

	H = (real__t *)malloc(sizeof(real__t)*(nnz)); // G values in Y 
	N = (real__t *)malloc(sizeof(real__t)*(nnz)); // G values in Y 
	J = (real__t *)malloc(sizeof(real__t)*(nnz)); // G values in Y 
	L = (real__t *)malloc(sizeof(real__t)*(nnz)); // G values in Y 

	Jx = (real__t *)malloc(sizeof(real__t)*(nnz_J)); // B values in Y
	Ji = (uint__t *)malloc(sizeof(uint__t)*(nnz_J)); // column indices of Y
	Jp = (uint__t *)malloc(sizeof(uint__t)*(n_J+1)); // initial row pointers	

	deltaP = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b
	deltaQ = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b
	deltaX = (real__t *)malloc(sizeof(real__t)*(n_J)); // deltaP and deltaQ, a 2n x 1 array

	// xP = (real__t *)malloc(sizeof(real__t)*(n)); // b in the Ax=b
	// xQ = (real__t *)malloc(sizeof(real__t)*(n_pp)); // b in the Ax=b

	Vm = (real__t *)malloc(sizeof(real__t)*(n)); 
	Va = (real__t *)malloc(sizeof(real__t)*(n)); 

	Pn = (real__t *)malloc(sizeof(real__t)*(n)); 
	Qn = (real__t *)malloc(sizeof(real__t)*(n));

	Pcal = (real__t *)malloc(sizeof(real__t)*(n)); 
	Qcal = (real__t *)malloc(sizeof(real__t)*(n));

	btype = (uint__t *)malloc(sizeof(uint__t)*(n));

	std::vector<double> maxDltP_list; // Max P in each iteration
	std::vector<double> maxDltQ_list; // Max Q in each iteration

	// Initialization to zero
 // [tc] using memset instead of for loop here, maybe can change 
 // to use calloc in the first place?
 memset(Vm, 0, sizeof(real__t)*(n));
 memset(Va, 0, sizeof(real__t)*(n));
 memset(deltaP, 0, sizeof(real__t)*(n));
 memset(deltaQ, 0, sizeof(real__t)*(n));

	printf("\n\n------------------------------------------------------------------------------------------- ");
	std::cout << "Number of rows:" << n << ",\tNumber of nonzeros:" << nnz << std::endl; 
	printf("\t -------------------------------------------------------------------------------------------- \n\n");	

	//std::cout << " ======================== Initialization of ararys used to store the factorized matrix and LU ========================"<< std::endl;

	// ------------------------------------------------------------------------------------------------
	// 				Convert Contariner to Arrays Used by NISCLU
	// ------------------------------------------------------------------------------------------------
	// Get the execution time of the code
	struct timeval t2_st, t2_end, t3_st, t3_end; long seconds, useconds;
	gettimeofday(&t2_st, 0);
	// auto t1 = Clock::now(); // a better way to time with higher precisions

 // =================================== Sort all input HeapAccum =================================
 gVertex.sort_heap();  
	gEdge.sort_heap();
  
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n================================================================================================ ");
	std::cout << "Time to sort HeapAccum:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ "); 

	gettimeofday(&t2_st, 0); 
     	
	int ii; // counter for the array

	// ========================================= Convert gVertex data =========================================
	ii = 1; // counter for the pointer array
 ep[0] = 0;
	for (i=0;i<gVertex.size();i++){
   // set up the pointer array for GRAPHLU. Each entry indicates the starting position
   // of the particular row in the Ax/Ai array. Refer to GRAPHLU documentation for detail.  
		ep[ii] = gVertex.data_[i].ep + ep[ii-1];
   Vm[i] = gVertex.data_[i].Vm;
   Va[i] = gVertex.data_[i].Va;
   Pn[i] = gVertex.data_[i].Pn;
   Qn[i] = gVertex.data_[i].Qn;
   btype[i] = gVertex.data_[i].bustype;
   ii++;
	}
	// Assign values to the last position of the array
	ep[n] = nnz;

	// // // // check for arrays
	// for (i=0; i<n+1; ++i)
	// 	std::cout << "ep" << i << ":"  << std::setprecision(8) << ep[i]<< std::endl;

	// ========================================= Convert gEdge data  =========================================
	for (i=0; i<gEdge.size(); ++i) {
		ei[i] = gEdge.data_[i].ei;
   eG[i] = gEdge.data_[i].eG;
   eB[i] = gEdge.data_[i].eB;
		// std::cout << "ei" << i << ", "  << std::setprecision(8) << ei[i] << ", "  << gei.data_[i].value << std::endl;
		// ii ++;
	}

	// Get the time to convert the data between containers and arrays
	// auto t1 = Clock::now(); // a better way to time with higher precisions
	// std::clock_t t1_end=std::clock();
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n================================================================================================ ");
	std::cout << "Time to convert data to GRAPHLU time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");

	// ----------------------------------------------------------------------------------------------------
	// 								Initialize Jaccobian Matirx
	// ----------------------------------------------------------------------------------------------------

	//std::cout << "\n ======================== Initialize Jaccobian Matirx ======================== \n"<< std::endl;

	gettimeofday(&t2_st, 0);
	// Get Pcal and Qcal
	for (i=0; i<n; ++i){ // clear from previous iteration
		Pcal[i] = 0;
		Qcal[i] = 0;

		// Calculate network injections
		for (j=ep[i]; j<ep[i+1]; ++j){
			p=ei[j];
     //calculate cosine and sine term and store in array
     cosine_array[j] = cos(Va[i]-Va[p]);  
     sine_array[j] = sin(Va[i]-Va[p]); 
     Pcal[i] += Vm[i]*Vm[p]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);
			Qcal[i] += Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]);  
		}
	}

 //gettimeofday(&t2_end, 0);
	//seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	//useconds = t2_end.tv_usec  - t2_st.tv_usec;
	//printf("\n\n================================================================================================\n ");
	//std::cout << "Time for Pcal and Qcal Initialization: " << (seconds*1000000 + useconds) << " us" << std::endl;
	//printf("\n================================================================================================ \n\n");

	//std::cout << " ======================== get H,N,J,L matrices ========================"<<std::endl;
	//gettimeofday(&t2_st, 0);
 // Calculate deltaP and deltaQ to update Vm
	counter=0;
	for (i=0; i<n; ++i){
   switch (btype[i]){
   case 0:  //PQ bus
   case 1:  //PQ bus
     // Get the values in the Jaccobian matrix
     for (j=ep[i]; j<ep[i+1]; ++j){
		    p=ei[j];
		    // H[counter]=0; N[counter]=0; J[counter]=0; L[counter]=0;
		    if(i!=p){             
         H[j] = Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]);
         N[j] = Vm[i]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);
		      J[j] = -Vm[i]*Vm[p]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);
		      L[j] = Vm[i]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]);               
		    }
		    else{
		      H[j] = -Qcal[i] - eB[j]*Vm[i]*Vm[i];
		      N[j] = Pcal[i]/Vm[i] + eG[j]*Vm[i];
		      J[j] = Pcal[i] - eG[j]*Vm[i]*Vm[i];
		      L[j] = Qcal[i]/Vm[i] - eB[j]*Vm[i];
		    }
		    // std::cout << "J" << j << ": " << i << ", " << p << ", H, " << H[j] << ", N, " << N[j] << ", J, " << J[j] << ", L, " << L[j] << std::endl;
     }
     break;
   case 2:  //PV bus
     // deltaQ[i]=0; 
     for (j=ep[i]; j<ep[i+1]; ++j){
		    p=ei[j];
		    if (p==i){ // For diagnal elements
		      H[j] = -Qcal[i] - eB[j]*Vm[i]*Vm[i];
		      N[j] = Pcal[i]/Vm[i] + eG[j]*Vm[i];
	        J[j]=0;
		      L[j]=1;
		    }
		    else{ // Off-diagnal elements        
         H[j] = Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]);
		      N[j] = Vm[i]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);          
		      J[j] = 0;
		      L[j ]= 0;
		    }
     }
     break;          
   case 3:  //slack bus
     for (j=ep[i]; j<ep[i+1]; ++j){
		    p=ei[j];
		    if (p==i){ // For diagnal elements
		      H[j] = 1;
		      N[j] = 0;
		      J[j] = 0;
		      L[j] = 1;
		    }
		    else{ // Off-diagnal elements
		      H[j] = 0;
		      N[j] = 0;
				  J[j] = 0;
				  L[j] = 0;
		    }
     }
     break;
   default: //unknown bus type 
     std::cerr << "Unknown bus type at bus: " << i << " Type: " << btype[i] << std::endl;
     break;
   }      
	}

 //gettimeofday(&t2_end, 0);
	//seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	//useconds = t2_end.tv_usec  - t2_st.tv_usec;
	//printf("\n\n================================================================================================\n ");
	//std::cout << "Time for H, N, J, L Initialization: " << (seconds*1000000 + useconds) << " us" << std::endl;
	//printf("\n================================================================================================ \n\n");

	//std::cout << " ======================== Get the linked list structure for Jaccobian ========================"<<std::endl;
 //gettimeofday(&t2_st, 0); 
	counter=0;
	rowcnt=0;
	for (i=0; i<n; ++i){ 
		// clen = ep[i+1] - ep[i] + 1; // number of nonzeros is row i

		// ------- Store H and N -------
		Jp[rowcnt] = counter; // Get the initial row pointer
		for (j=ep[i]; j<ep[i+1]; ++j){
			p=ei[j];
			// Store H
			Jx[counter] = H[j]; Ji[counter] = 2*p; 
			counter += 1;
			// Store N
			Jx[counter] = N[j]; Ji[counter] = 2*p+1; 
			counter += 1;
		}
		rowcnt += 1;

		// ------- Store J and L -------
		Jp[rowcnt] = counter; // Get the initial row pointer
		for (j=ep[i]; j<ep[i+1]; ++j){
			p=ei[j];
			// Store J
			Jx[counter] = J[j]; Ji[counter] = 2*p; 
			counter += 1;
			// Store L
			Jx[counter] = L[j]; Ji[counter] = 2*p+1; 
			counter += 1;
		}
		rowcnt += 1;
	}
	Jp[rowcnt] = counter; // Last entry in the array

	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	
	printf("\n\n================================================================================================\n ");
	std::cout << "Time for Jaccobian Initialization: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ \n\n");
	// ----------------------------------------------------------------------------------------------------
	// 								Call GRAPHLU and Initialize the GRAPHLU Structure
	// ----------------------------------------------------------------------------------------------------
	gettimeofday(&t2_st, 0);
	//std::cout << "\n ======================== Call GRAPHLU and Initialize the GRAPHLU Structure ======================== \n"<< std::endl;
	// Initialize the structure for GRAPHLU
	graphlu = (SGraphLU *)malloc(sizeof(SGraphLU));
	GraphLU_Initialize(graphlu);

	// Create Matrix
	GraphLU_CreateMatrix(graphlu, n_J, nnz_J, Jx, Ji, Jp);
	// graphlu->cfgf[0] = 1.;

	// // Set control parameters
	// graphlu->cfgi[1] = 0; // 0 is no MC64 scaling
	// 					 // 1 is with MC64 scaling

	// Analyze (column/row ordering, scaling. For details please refer to GRAPHLU user's guide)
	GraphLU_Analyze(graphlu);
	printf("analysis time: %.8g\n", graphlu->stat[0]);
	
 // [Ting][begin] comment out if using sequential factorization, no need to create schedule, create threads and bind threads
	// creates the task scheduler for parallel LU factorization. If you want to run 
 // parallel factorization or parallel re-factorization, it should be called after GraphLU Analyze.
	ret = GraphLU_CreateScheduler(graphlu);
	printf("time of creating scheduler: %.8g\n", graphlu->stat[4]);
 printf("suggestion: %s.\n", ret==0?"parallel":"sequential");
	
	// This function creates threads for parallel computation. The second argument (thread)
	// specifies the number of threads, including the main thread. The last argument (check)
	// specifies whether to check the number of threads or not.
 GraphLU_CreateThreads(graphlu, 2, TRUE);
 printf("total cores: %d, threads created: %d\n", (int)(graphlu->stat[9]), (int)(graphlu->cfgi[5]));
	
	// This function binds threads to cores (unbind = FALSE) or unbinds threads from cores (unbind = TRUE).
	GraphLU_BindThreads(graphlu, FALSE);
 // [Ting][end] comment out if using sequential factorization	
 
	// // Numerical LU factorization with partial pivoting, parallel
	// GraphLU_Factorize_MT(graphlu);
	// printf("factorization time: %.8g\n", graphlu->stat[1]);
 
	// std::cout << " ======================== Number of Total nonzeros after factorization is: "<< graphlu->lu_nnz << "========================"<<std::endl;
	// std::cout << " ======================== Number of nonzeros in L: "<< graphlu->l_nnz << "========================"<<std::endl;
	// std::cout << " ======================== Number of nonzeros in U: "<< graphlu->u_nnz << "========================"<<std::endl;
	// std::cout << " ======================== End of LU Factorization Routines ========================"<<std::endl;
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	
	printf("\n\n================================================================================================\n ");
	std::cout << "Time for GRAPHLU analysis and scheduling " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ \n\n");
 	
	//printf("\n\n================================================================================================ ");
	//std::cout << "\n Start iteratively updating deltaP and deltaQ " << std::endl;
	//printf("\n================================================================================================\n ");
 gettimeofday(&t2_st, 0);
	std::cout << std::fixed; // set fixed precisions
	for (iter=0; iter<maxiter; ++iter){
		// update deltaP and deltaQ
		maxDeltaP=0;
		maxDeltaQ=0;
   
   gettimeofday(&t3_st, 0);
		// Get Pcal and Qcal
		for (i=0; i<n; ++i){ // clear from previous iteration
     if (iter != 0){   //Pcal and Qcal are computed during initialization, no need to compute it again for iteration 0  
			  Pcal[i] = 0;
			  Qcal[i] = 0;

			  // Calculate network injectionsgad
			  for (j=ep[i]; j<ep[i+1]; ++j){
				  p=ei[j];
         //update consine and sine arrays with new angles          
         cosine_array[j] = cos(Va[i]-Va[p]);  
         sine_array[j] = sin(Va[i]-Va[p]);   
         Pcal[i] += Vm[i]*Vm[p]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);
			    Qcal[i] += Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]);
			  }
     }
			// check output
			// std::cout << "Iter" << iter << ", Pcal," << i << ", " << std::setprecision(8) << Pcal[i]<< ", Qcal" << i << ", " << std::setprecision(8) << Qcal[i]<< std::endl;
     
     //calculate deltaP and deltaQ based on bus type
     switch (btype[i]){
     case 0:  //PQ bus
     case 1:  //PQ bus
       deltaP[i] = Pn[i] - Pcal[i];
       deltaQ[i] = Qn[i] - Qcal[i];
       break;  
     case 2:  //PV bus
       deltaP[i] = Pn[i] - Pcal[i];
       deltaQ[i] = 0;
       break;            
     case 3:  //slack bus
       deltaP[i] = 0;
       deltaQ[i] = 0;
       break;
     default: //unknown bus type  
       std::cerr << "Unknown bus type at bus: " << i << " Type: " << btype[i] << std::endl;
       break;
     }          

			// Get max P and max Q
			if (abs(deltaP[i]) > maxDeltaP)
				maxDeltaP = abs(deltaP[i]);
			if (abs(deltaQ[i]) > maxDeltaQ)
				maxDeltaQ = abs(deltaQ[i]);

			// store deltaP and deltaQ into the b vector
			deltaX[2*i]=deltaP[i];
			deltaX[2*i+1]=deltaQ[i];

		}
	  gettimeofday(&t3_end, 0);
	  seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	  useconds = t3_end.tv_usec  - t3_st.tv_usec;
	
	  printf("\n\n================================================================================================\n ");
	  std::cout << "Time for Pcal and Qcal, max mismatch loop (iteration): " << iter << ": " << (seconds*1000000 + useconds) << " us" << std::endl;
	  printf("\n================================================================================================ \n\n");
		// keep track of max deltaP and max deltaQ in each iteration
   maxDltP_list.push_back(maxDeltaP);
   maxDltQ_list.push_back(maxDeltaQ);

		// Decide if the program converges	
		if ( maxDeltaP < max_change_ex && maxDeltaQ < max_change_ex){
			break;
		}

		if (iter!=0){ // Since the matrix is already initilized, no need to compute it again in the first NR iteration
			// std::cout << " ======================== Get H,N,J,L matrices ========================"<<std::endl;
     gettimeofday(&t3_st, 0);
			// Calculate deltaP and deltaQ to update Vm
			counter=0;
			for (i=0; i<n; ++i){
       switch (btype[i]){
       case 0:  //PQ bus
       case 1:  //PQ bus
         // Get the values in the Jaccobian matrix
	        for (j=ep[i]; j<ep[i+1]; ++j){
					  p=ei[j];
					  // H[counter]=0; N[counter]=0; J[counter]=0; L[counter]=0;
					  if(i!=p){             
             H[j] = Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]);
	            N[j] = Vm[i]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);
					    J[j] = -Vm[i]*Vm[p]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);
					    L[j] = Vm[i]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]);               
					  }
					  else{
					    H[j] = -Qcal[i] - eB[j]*Vm[i]*Vm[i];
					    N[j] = Pcal[i]/Vm[i] + eG[j]*Vm[i];
					    J[j] = Pcal[i] - eG[j]*Vm[i]*Vm[i];
					    L[j] = Qcal[i]/Vm[i] - eB[j]*Vm[i];
					  }
					  // std::cout << "J" << j << ": " << i << ", " << p << ", H, " << H[j] << ", N, " << N[j] << ", J, " << J[j] << ", L, " << L[j] << std::endl;
	        }
         break;
       case 2:  //PV bus
         // deltaQ[i]=0; 
	        for (j=ep[i]; j<ep[i+1]; ++j){
					  p=ei[j];
					  if (p==i){ // For diagnal elements
					    H[j] = -Qcal[i] - eB[j]*Vm[i]*Vm[i];
					    N[j] = Pcal[i]/Vm[i] + eG[j]*Vm[i];
				      J[j] = 0;
					    L[j] = 1;
					  }
					  else{ // Off-diagnal elements        
             H[j] = Vm[i]*Vm[p]*(eG[j]*sine_array[j] - eB[j]*cosine_array[j]);
					    N[j] = Vm[i]*(eG[j]*cosine_array[j] + eB[j]*sine_array[j]);          
					    J[j] = 0;
					    L[j] = 0;
					  }
	        }
         break;          
       case 3:  //slack bus
			    for (j=ep[i]; j<ep[i+1]; ++j){
					  p=ei[j];
					  if (p==i){ // For diagnal elements
					    H[j] = 1;
					    N[j] = 0;
					    J[j] = 0;
					    L[j] = 1;
					  }
					  else{ // Off-diagnal elements
					    H[j] = 0;
					    N[j] = 0;
						  J[j] = 0;
						  L[j] = 0;
					  }
	        }
         break;
       default:  //unknown bus type
			    std::cerr << "Unknown bus type at bus: " << i << " Type: " << btype[i] << std::endl;
         break;
       }      
			}
	    gettimeofday(&t3_end, 0);
	    seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	    useconds = t3_end.tv_usec  - t3_st.tv_usec;
	
	    printf("\n\n================================================================================================\n ");
	    std::cout << "Time for getting H,N,J,L matrices (iteration): " << iter << ": " << (seconds*1000000 + useconds) << " us" << std::endl;
	    printf("\n================================================================================================ \n\n");
			
     // std::cout << " ======================== Update the values in Jaccobian ========================"<<std::endl;
			counter=0;
			rowcnt=0;
     gettimeofday(&t3_st, 0);
			for (i=0; i<n; ++i){ 
				// clen = ep[i+1] - ep[i] + 1; // number of nonzeros is row i

				// Store H and N
				for (j=ep[i]; j<ep[i+1]; ++j){
					p=ei[j];
					// Store H
					Jx[counter] = H[j];
					counter += 1;

					// Store N
					Jx[counter] = N[j];
					counter += 1;
				}

				// Store J and L
				for (j=ep[i]; j<ep[i+1]; ++j){
					p=ei[j];
					// Store J
					Jx[counter] = J[j];
					counter += 1;

					// Store L
					Jx[counter] = L[j];
					counter += 1;
				}
			}
     
	    gettimeofday(&t3_end, 0);
	    seconds=t3_end.tv_sec  - t3_st.tv_sec; 
	    useconds = t3_end.tv_usec  - t3_st.tv_usec;
	
	    printf("\n\n================================================================================================\n ");
	    std::cout << "Time for updating the values in Jaccobian (iteration): " << iter << ": " << (seconds*1000000 + useconds) << " us" << std::endl;
	    printf("\n================================================================================================ \n\n");
		}

		// ----------------------------- Refactorize the Jaccobian Matrix -----------------------------
		// // std::cout << " ======================== Refactorize the Jaccobian Matrix ========================"<<std::endl;
		// Code below performs partial pivoting, if no pivoting is needed, use the next block which is commented out
		GraphLU_ResetMatrixValues(graphlu, Jx);
		if (!ret)
     GraphLU_Factorize_MT(graphlu);
   else
     GraphLU_Factorize(graphlu);  
		printf("Factorization time: %.8g\n", graphlu->stat[1]);
   
   // without doing reset matrix values and use refactorize with no pivoting, use code below
   /*
   if (!ret){    // Multi-threaded factorization and refactorization (no pivoting)
		  if (iter==0){
		  	GraphLU_Factorize_MT(graphlu);
		  	printf("Factorization time: %.8g\n", graphlu->stat[1]);
		  }	
		  else{
		  	GraphLU_ReFactorize_MT(graphlu, Jx);
		  	printf("Refactorization time: %.8g\n", graphlu->stat[2]);
		  }
   }   
 	else{    // sequential factorization and refactorization (no pivoting)
		  if (iter==0){
		  	GraphLU_Factorize(graphlu);
		  	printf("Factorization time: %.8g\n", graphlu->stat[1]);
		  }	
		  else{
		  	GraphLU_ReFactorize(graphlu, Jx);
		  	printf("Refactorization time: %.8g\n", graphlu->stat[2]);
		  }
   }
   */    
		// solve for deltaVm and deltaVa
		// std::cout << " ======================== Solve for V angle and V magnitude ========================"<<std::endl;
		GraphLU_Solve(graphlu, deltaX); // This is full forward/backward
   //GraphLU_SolveFast(graphlu, deltaX); // This is fast forward/backward, which uses sparse vector. This is fast when there are many zeros in b
		printf("substitution time: %.8g\n", graphlu->stat[3]); 
		// 

		// Get values of deltaP and deltaQ, update Vm and Va
		for (i=0; i<n; ++i){
			deltaP[i] = deltaX[2*i];
			deltaQ[i] = deltaX[2*i+1];

			Va[i] += deltaP[i];
			Vm[i] += deltaQ[i];
		}
	}

	// // show the max mismatch in each iteration
 for( i=0; i<maxDltP_list.size(); ++i){
	 	std::cout << "Iter" << i << ", Max MismatchP, " << std::setprecision(10) << maxDltP_list[i] << std::endl;
	 	std::cout << "Iter" << i << ", Max MismatchQ, " << std::setprecision(10) << maxDltQ_list[i] << std::endl;
 }

	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	
	printf("\n\n================================================================================================\n ");
	std::cout << "Time for factorization and power flow iteration: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ \n\n");


	// ----------------------------------------------------------------------------------------------------
	// 								Store the Result back to the array
	// ----------------------------------------------------------------------------------------------------
	for (ii=0; ii<n; ++ii) {
     gVertex.data_[ii].Va = Va[ii]/pi_value*180;
     gVertex.data_[ii].Vm = Vm[ii];
     //std::cout << "Final, " << ii+1 << ", Vm, " << Vm[ii]<< ", Va" << ii+1 << ", " << Va[ii]/pi_value*180<< std::endl;
	} 

EXIT:
	// std::cout << "\n free1 " << std::endl;
	GraphLU_Destroy(graphlu);
	// GraphLU_Destroy(graphlu_pp);
	// std::cout << "\n free2 " << std::endl;
	// free(ax);
	// free(ai);
	// free(ap);
	// free(ax_pp);
	// free(ai_pp);
	// free(ap_pp);
	free(graphlu);
	// free(graphlu_pp);
	// std::cout << "\n free3 " << std::endl;

	free(H);
	free(N);
	free(J);
	free(L);

	free(Jx);
	free(Ji);
	free(Jp);

	free(eG);
	free(eB);
	free(ei);
	free(ep);

	free(deltaP);
	free(deltaQ);
	free(deltaX);
	free(Vm);
	free(Va);		
	free(Pn);
	free(Qn);	

	free(btype);

	printf("\n\n----------------------------------------------------------------------------------------");
	printf("\t\t End of Running GRAPHLU_NRpf_v4 C function!");
	printf("\t\t ----------------------------------------------------------------------------------------\n\n");	
} 
//

// **********************************************************************************************************************************
// Created by: Ting Chan, ting.chan@geirina.net
// Date: 10/26/2016
// This code performs fast-decoupled power flow analysis. It is used as a benchmark to assess the time it takes to run a fast-decoupled
// power flow in C++ with NICSLU. This version (v6) takes HeapAccum as inputs instead of MapAccum. The input HeapAcuum is assumed to be
// unsorted, so sorting them is needed before assigning the values to local arrays. Differences between this version (v6) and v2 is 
// the number of input HeapAccum. In this version, only 4 heapAccums are needed (vertex, Bp, Bpp and e). With fewer HeapAccums, the sorting
// time is reduced. This version does not require calling NICSLU_build_Ap2 for the pointer structures (i.e. Bp_p, Bpp_p and ep).
// This version (v7) is based on v6 but there are only two input HeapAccums - one for vertex, another one for Ybus, B' and B". 
// History: 
// 10/26/2016: [Ting Chan] Added return type as a string to indicate whether power flow is converged.
// **********************************************************************************************************************************
template <typename T_vertex_tuple, typename vertex_comp, typename T_edge_all_tuple, typename edge_all_comp>
inline string NICSLU_fdpf_v7 (int64_t& gBp, int64_t& gBpp, HeapAccum<T_vertex_tuple, vertex_comp>& gVertex, 
              HeapAccum<T_edge_all_tuple, edge_all_comp>& gEdge_all, MaxAccum<int> maxiter, MaxAccum<double> maxchange) {

	// extern size_t g_si, g_sd, g_sp;
	printf("\n\n------------------------------------------------------------------------------------------- ");
	printf("\nStart Running NICSLU_fdpf_v7 C function!\n");
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
  
	
	//// Convert vectors to NICSLU Data Structure (pointers)
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
  // Done converting all input HeapAccums to arrays
  
	// Get the time to convert the data between containers and arrays
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	printf("\n\n============================================================================================== ");
	std::cout << "Time to convert data to NICSLU time:: " << (seconds*1000000 + useconds) << " us" << std::endl;
	printf("\n================================================================================================ ");		
	
	// ----------------------------------------------------------------------------------------------------
	// 								Call NICSLU and Factorize B' Matirx
	// ----------------------------------------------------------------------------------------------------
  int error_p = 0, error_pp = 0;
	gettimeofday(&t2_st, 0);
	std::cout << "\n ======================== Start factorizing B' Matirx ======================== \n"<< std::endl;
	// Initialize the structure for NICSLU
	graphlu = (SGraphLU *)malloc(sizeof(SGraphLU));
	GraphLU_Initialize(graphlu);

	// Create Matrix
	GraphLU_CreateMatrix(graphlu, n, nnz, ax, ai, ap);
	// nicslu->cfgf[0] = 1.;

	// // Set control parameters
	// nicslu->cfgi[1] = 0; // 0 is no MC64 scaling
	// 					 // 1 is with MC64 scaling

	// Analyze (column/row ordering, scaling. For details please refer to NICSLU user's guide)
	GraphLU_Analyze(graphlu);
	printf("analysis time: %.8g\n", graphlu->stat[0]);
	
  // [Ting][begin] comment out if to try sequential factorization 
	// creates the task scheduler for parallel LU factorization. If you want to run 
	// parallel factorization or parallel re-factorization, it should be called after NicsLU Analyze.
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
      std::cout << "NicsLU_Factorize error code:" << error_p <<std::endl;  
  }   
  // [Ting][end] comment out if to try sequential factorization 
  else{
    // Sequential factorization
	  error_p = GraphLU_Factorize(graphlu);
	  printf("Factorization time: %.8g\n", graphlu->stat[1]);
    if (error_p < 0) //there is an error, print the code 
      std::cout << "NicsLU_Factorize error code:" << error_p <<std::endl; 
	}
 
	//std::cout << " ======================== Number of Total nonzeros after factorization is: "<< nicslu->lu_nnz << "========================"<<std::endl;
	//std::cout << " ======================== Number of nonzeros in L: "<< nicslu->l_nnz << "========================"<<std::endl;
	//std::cout << " ======================== Number of nonzeros in U: "<< nicslu->u_nnz << "========================"<<std::endl;
	//std::cout << " ======================== End of LU Factorization Routines ========================"<<std::endl;
	
	// ----------------------------------------------------------------------------------------------------
	// 								Call NICSLU and Factorize B" Matirx
	// ----------------------------------------------------------------------------------------------------
	std::cout << "\n ======================== Start factorizing B'' Matirx ======================== \n"<< std::endl;
	// Initialize the structure for NICSLU
	graphlu_pp = (SGraphLU *)malloc(sizeof(SGraphLU));
	GraphLU_Initialize(graphlu_pp);

	// Create Matrix
	GraphLU_CreateMatrix(graphlu_pp, n_pp, nnz_pp, ax_pp, ai_pp, ap_pp);
	// nicslu_pp->cfgf[0] = 1.;

	// // Set control parameters
	// nicslu->cfgi[1] = 0; // 0 is no MC64 scaling
	// 					 // 1 is with MC64 scaling
  
	// Analyze (column/row ordering, scaling. For details please refer to NICSLU user's guide)
	GraphLU_Analyze(graphlu_pp);
	printf("analysis time: %.8g\n", graphlu_pp->stat[0]);
  
	// [Ting][begin] comment out if to try sequential factorization 
	// creates the task scheduler for parallel LU factorization. If you want to run 
	// parallel factorization or parallel re-factorization, it should be called after NicsLU Analyze.
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
      std::cout << "NicsLU_Factorize error code:" << error_pp <<std::endl;      
  }
  // [Ting][end] comment out if to try sequential factorization 
  else{ 
    //Sequential factorization 
	  error_pp = GraphLU_Factorize(graphlu_pp);
    printf("Factorization time: %.8g\n", graphlu_pp->stat[1]);
    if (error_pp < 0) //there is an error, print the code 
      std::cout << "NicsLU_Factorize error code:" << error_pp <<std::endl; 
  }
	// // Get the number of nonzeros in the factorized matrix
	// fctnnz = nicslu_pp->lu_nnz;
	// l_nnz = nicslu_pp->l_nnz;
	// u_nnz = nicslu_pp->u_nnz;

	// std::cout << " ======================== Number of Total nonzeros after factorization is: "<< nicslu_pp->lu_nnz << "========================"<<std::endl;
	// std::cout << " ======================== Number of nonzeros in L: "<< nicslu_pp->l_nnz << "========================"<<std::endl;
	// std::cout << " ======================== Number of nonzeros in U: "<< nicslu_pp->u_nnz << "========================"<<std::endl;
	// std::cout << " ======================== End of LU Factorization Routines ========================"<<std::endl;
	
  // Get the time for factorizing B' and B"
	gettimeofday(&t2_end, 0);
	seconds=t2_end.tv_sec  - t2_st.tv_sec; 
	useconds = t2_end.tv_usec  - t2_st.tv_usec;
	
	printf("\n\n============================================================================================== ");
	std::cout << "Time for NICSLU to factorize B' and B'':: " << (seconds*1000000 + useconds) << " us" << std::endl;
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
  
  			// deltaP[i] = deltaP[i]/Vm[i];
  			// deltaQ[i] = deltaQ[i]/Vm[i];
  
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
  		GraphLU_Solve(graphlu, deltaP);
  
  		// Update V angle (Va)
  		for (i=0; i<n; ++i)
      {
  			Va[i] -= deltaP[i];
        //std::cout << "Iter" << iter << ", Va" << i << ", " << Va[i]<< ", deltaVa" << i << ", " << deltaP[i]<< std::endl;
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
  			//std::cout << "Iter" << iter << ", deltaP" << i << ", " << deltaP[i]<< ", deltaQ" << i << ", " << deltaQ[i]<< std::endl;
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
        //std::cout << "Iter" << iter << ", Vm" << i << ", " << Vm[i]<< ", deltaVm" << i << ", " << deltaQ[i]<< std::endl;
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
        //gVertex.data_[ii].Va = Va[ii]/pi_value*180;
		gVertex.data_[ii].Va = Va[ii];
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
	printf("\t\t End of Running NICSLU_fdpf_v7 C function!");
	printf("\t\t ----------------------------------------------------------------------------------------\n\n");	
 
  return result;
}


// **********************************************************************************************************************************
// Created by: Ting Chan, ting.chan@geirina.net
// Date: 9/21/2016
// This function takes an index and a HeapAccum as inputs and returns Vm (voltage magnitude) as output. Specified for gVertex
// History:
// **********************************************************************************************************************************
template <typename T_vertex_tuple, typename vertex_comp>
inline double GRAPHLU_getVm (uint64_t& index, const HeapAccum<T_vertex_tuple, vertex_comp> &gVertex) {
 return gVertex.data_[index-1].Vm;
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

//**********************************************************************************************************************************
// Created by: Jun Tan, juntan@geirina
// Date: 4/18/2018
// History:
// **********************************************************************************************************************************
template<typename sort_vertex_out, typename vertex_comp_out>
inline double FBS_getVAm (uint64_t& index, const HeapAccum<sort_vertex_out, vertex_comp_out>& Vertex_D) {
   return Vertex_D.data_[index-1].VAmag;
} 
//
template<typename sort_vertex_out, typename vertex_comp_out>
inline double FBS_getVAa (uint64_t& index, const HeapAccum<sort_vertex_out, vertex_comp_out>& Vertex_D) {
   return Vertex_D.data_[index-1].VAang;
} 
// 
template<typename sort_vertex_out, typename vertex_comp_out>
inline double FBS_getVBm (uint64_t& index, const HeapAccum<sort_vertex_out, vertex_comp_out>& Vertex_D) {
   return Vertex_D.data_[index-1].VBmag;
} 
// 
template<typename sort_vertex_out, typename vertex_comp_out>
inline double FBS_getVBa (uint64_t& index, const HeapAccum<sort_vertex_out, vertex_comp_out>& Vertex_D) {
   return Vertex_D.data_[index-1].VBang;
} 
// 
template<typename sort_vertex_out, typename vertex_comp_out>
inline double FBS_getVCm (uint64_t& index, const HeapAccum<sort_vertex_out, vertex_comp_out>& Vertex_D) {
   return Vertex_D.data_[index-1].VCmag;
} 
// 
template<typename sort_vertex_out, typename vertex_comp_out>
inline double FBS_getVCa (uint64_t& index, const HeapAccum<sort_vertex_out, vertex_comp_out>& Vertex_D) {
   return Vertex_D.data_[index-1].VCang;
} 
// 
template<typename sort_vertex_out, typename vertex_comp_out>
inline double FBS_getPdA (uint64_t& index, const HeapAccum<sort_vertex_out, vertex_comp_out>& Vertex_D) {
   return Vertex_D.data_[index-1].PdA_cal;
} 
//
template<typename sort_vertex_out, typename vertex_comp_out>
inline double FBS_getQdA (uint64_t& index, const HeapAccum<sort_vertex_out, vertex_comp_out>& Vertex_D) {
   return Vertex_D.data_[index-1].QdA_cal;
} 
// 
template<typename sort_vertex_out, typename vertex_comp_out>
inline double FBS_getPdB (uint64_t& index, const HeapAccum<sort_vertex_out, vertex_comp_out>& Vertex_D) {
   return Vertex_D.data_[index-1].PdB_cal;
} 
// 
template<typename sort_vertex_out, typename vertex_comp_out>
inline double FBS_getQdB (uint64_t& index, const HeapAccum<sort_vertex_out, vertex_comp_out>& Vertex_D) {
   return Vertex_D.data_[index-1].QdB_cal;
} 
// 
template<typename sort_vertex_out, typename vertex_comp_out>
inline double FBS_getPdC (uint64_t& index, const HeapAccum<sort_vertex_out, vertex_comp_out>& Vertex_D) {
   return Vertex_D.data_[index-1].PdC_cal;
} 
// 
template<typename sort_vertex_out, typename vertex_comp_out>
inline double FBS_getQdC (uint64_t& index, const HeapAccum<sort_vertex_out, vertex_comp_out>& Vertex_D) {
   return Vertex_D.data_[index-1].QdC_cal;
} 
// 
template<typename sort_vertex_out, typename vertex_comp_out>
inline double FBS_getP (const HeapAccum<sort_vertex_out, vertex_comp_out>& Vertex_D) {
   return Vertex_D.data_[0].totalP;
} 
// 
template<typename sort_vertex_out, typename vertex_comp_out>
inline double FBS_getQ (const HeapAccum<sort_vertex_out, vertex_comp_out>& Vertex_D) {
   return Vertex_D.data_[0].totalQ;
} 
// 
template<typename sort_vertex_out1, typename vertex_comp_out1>
inline double NR_getAia (uint64_t& index, const HeapAccum<sort_vertex_out1, vertex_comp_out1>& Node_D) {
   return Node_D.data_[index-1].Aia;
} 
// 
template<typename sort_vertex_out1, typename vertex_comp_out1>
inline double NR_getAib (uint64_t& index, const HeapAccum<sort_vertex_out1, vertex_comp_out1>& Node_D) {
   return Node_D.data_[index-1].Aib;
} 
// 
template<typename sort_vertex_out1, typename vertex_comp_out1>
inline double NR_getAic (uint64_t& index, const HeapAccum<sort_vertex_out1, vertex_comp_out1>& Node_D) {
   return Node_D.data_[index-1].Aic;
} 
// 
template<typename sort_vertex_out1, typename vertex_comp_out1>
inline double NR_getVm (int64_t& index, const HeapAccum<sort_vertex_out1, vertex_comp_out1>& Node_D) {
   return Node_D.data_[index].Vm;
} 
// 
template<typename sort_vertex_out1, typename vertex_comp_out1>
inline double NR_getVa (int64_t& index, const HeapAccum<sort_vertex_out1, vertex_comp_out1>& Node_D) {
   return Node_D.data_[index].Va;
} 
// 
template<typename sort_line_out, typename line_comp_out>
inline double NR_getGaa (uint64_t lineID, const HeapAccum<sort_line_out, line_comp_out>& Line_D) {
   for (int i=0;i<Line_D.size();i++)
   {
	   if (lineID == Line_D.data_[i].lineID)
	   {
		    return Line_D.data_[i].Gaa;
	   }
   }
} 
// 
template<typename sort_line_out, typename line_comp_out>
inline double NR_getGab (uint64_t lineID, const HeapAccum<sort_line_out, line_comp_out>& Line_D) {
   for (int i=0;i<Line_D.size();i++)
   {
	   if (lineID == Line_D.data_[i].lineID)
	   {
		    return Line_D.data_[i].Gab;
	   }
   }
} 
// 
template<typename sort_line_out, typename line_comp_out>
inline double NR_getGac (uint64_t lineID, const HeapAccum<sort_line_out, line_comp_out>& Line_D) {
   for (int i=0;i<Line_D.size();i++)
   {
	   if (lineID == Line_D.data_[i].lineID)
	   {
		    return Line_D.data_[i].Gac;
	   }
   }
} 
// 
template<typename sort_line_out, typename line_comp_out>
inline double NR_getGba (uint64_t lineID, const HeapAccum<sort_line_out, line_comp_out>& Line_D) {
   for (int i=0;i<Line_D.size();i++)
   {
	   if (lineID == Line_D.data_[i].lineID)
	   {
		    return Line_D.data_[i].Gba;
	   }
   }
} 
// 
template<typename sort_line_out, typename line_comp_out>
inline double NR_getGbb (uint64_t lineID, const HeapAccum<sort_line_out, line_comp_out>& Line_D) {
   for (int i=0;i<Line_D.size();i++)
   {
	   if (lineID == Line_D.data_[i].lineID)
	   {
		    return Line_D.data_[i].Gbb;
	   }
   }
} 
// 
template<typename sort_line_out, typename line_comp_out>
inline double NR_getGbc (uint64_t lineID, const HeapAccum<sort_line_out, line_comp_out>& Line_D) {
   for (int i=0;i<Line_D.size();i++)
   {
	   if (lineID == Line_D.data_[i].lineID)
	   {
		    return Line_D.data_[i].Gbc;
	   }
   }
} 
// 
template<typename sort_line_out, typename line_comp_out>
inline double NR_getGca (uint64_t lineID, const HeapAccum<sort_line_out, line_comp_out>& Line_D) {
   for (int i=0;i<Line_D.size();i++)
   {
	   if (lineID == Line_D.data_[i].lineID)
	   {
		    return Line_D.data_[i].Gca;
	   }
   }
} 
// 
template<typename sort_line_out, typename line_comp_out>
inline double NR_getGcb (uint64_t lineID, const HeapAccum<sort_line_out, line_comp_out>& Line_D) {
   for (int i=0;i<Line_D.size();i++)
   {
	   if (lineID == Line_D.data_[i].lineID)
	   {
		    return Line_D.data_[i].Gcb;
	   }
   }
} 
// 
template<typename sort_line_out, typename line_comp_out>
inline double NR_getGcc (uint64_t lineID, const HeapAccum<sort_line_out, line_comp_out>& Line_D) {
   for (int i=0;i<Line_D.size();i++)
   {
	   if (lineID == Line_D.data_[i].lineID)
	   {
		    return Line_D.data_[i].Gcc;
	   }
   }
} 
// 
template<typename sort_line_out, typename line_comp_out>
inline double NR_getBaa (uint64_t lineID, const HeapAccum<sort_line_out, line_comp_out>& Line_D) {
   for (int i=0;i<Line_D.size();i++)
   {
	   if (lineID == Line_D.data_[i].lineID)
	   {
		    return Line_D.data_[i].Baa;
	   }
   }
} 
// 
template<typename sort_line_out, typename line_comp_out>
inline double NR_getBab (uint64_t lineID, const HeapAccum<sort_line_out, line_comp_out>& Line_D) {
   for (int i=0;i<Line_D.size();i++)
   {
	   if (lineID == Line_D.data_[i].lineID)
	   {
		    return Line_D.data_[i].Bab;
	   }
   }
} 
// 
template<typename sort_line_out, typename line_comp_out>
inline double NR_getBac (uint64_t lineID, const HeapAccum<sort_line_out, line_comp_out>& Line_D) {
   for (int i=0;i<Line_D.size();i++)
   {
	   if (lineID == Line_D.data_[i].lineID)
	   {
		    return Line_D.data_[i].Bac;
	   }
   }
} 
// 
template<typename sort_line_out, typename line_comp_out>
inline double NR_getBba (uint64_t lineID, const HeapAccum<sort_line_out, line_comp_out>& Line_D) {
   for (int i=0;i<Line_D.size();i++)
   {
	   if (lineID == Line_D.data_[i].lineID)
	   {
		    return Line_D.data_[i].Bba;
	   }
   }
} 
// 
template<typename sort_line_out, typename line_comp_out>
inline double NR_getBbb (uint64_t lineID, const HeapAccum<sort_line_out, line_comp_out>& Line_D) {
   for (int i=0;i<Line_D.size();i++)
   {
	   if (lineID == Line_D.data_[i].lineID)
	   {
		    return Line_D.data_[i].Bbb;
	   }
   }
} 
// 
template<typename sort_line_out, typename line_comp_out>
inline double NR_getBbc (uint64_t lineID, const HeapAccum<sort_line_out, line_comp_out>& Line_D) {
   for (int i=0;i<Line_D.size();i++)
   {
	   if (lineID == Line_D.data_[i].lineID)
	   {
		    return Line_D.data_[i].Bbc;
	   }
   }
} 
// 
template<typename sort_line_out, typename line_comp_out>
inline double NR_getBca (uint64_t lineID, const HeapAccum<sort_line_out, line_comp_out>& Line_D) {
   for (int i=0;i<Line_D.size();i++)
   {
	   if (lineID == Line_D.data_[i].lineID)
	   {
		    return Line_D.data_[i].Bca;
	   }
   }
} 
// 
template<typename sort_line_out, typename line_comp_out>
inline double NR_getBcb (uint64_t lineID, const HeapAccum<sort_line_out, line_comp_out>& Line_D) {
   for (int i=0;i<Line_D.size();i++)
   {
	   if (lineID == Line_D.data_[i].lineID)
	   {
		    return Line_D.data_[i].Bcb;
	   }
   }
} 
// 
template<typename sort_line_out, typename line_comp_out>
inline double NR_getBcc (uint64_t lineID, const HeapAccum<sort_line_out, line_comp_out>& Line_D) {
   for (int i=0;i<Line_D.size();i++)
   {
	   if (lineID == Line_D.data_[i].lineID)
	   {
		    return Line_D.data_[i].Bcc;
	   }
   }
}

}
/****************************************/

#endif /* EXPRFUNCTIONS_HPP_ */
