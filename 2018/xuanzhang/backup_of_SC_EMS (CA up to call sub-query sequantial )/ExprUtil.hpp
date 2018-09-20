/******************************************************************************
 * Copyright (c) 2016, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL Query Language
 *
 * - This library is for defining struct and helper functions that will be used
 *   in the user-defined functions in "ExprFunctions.hpp". Note that functions
 *   defined in this file cannot be directly called from GraphSQL Query scripts.
 *   Please put such functions into "ExprFunctions.hpp" under the same directory
 *   where this file is located.
 *
 * - Please don't remove necessary codes in this file
 *
 * - A backup of this file can be retrieved at
 *     <graphsql_root_path>/dev_<backup_time>/gdk/gsql/src/QueryUdf/ExprUtil.hpp
 *   after upgrading the system.
 *
 ******************************************************************************/

#ifndef EXPRUTIL_HPP_
#define EXPRUTIL_HPP_

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <gle/engine/cpplib/headers.hpp>

#include <cassert>
#include <mutex>
#include <vector>
#include "graphlu.h"     
#include "graphlu_util.h"

typedef std::string string; //XXX DON'T REMOVE

using namespace std;

class CSRMatrix {
private:
  uint__t _n;
  uint__t _nnz;
  vector<uint__t> _bp;
  //vector<uint__t> _bpp;
  vector<size_t> _p;
  vector<uint__t> _i;
  vector<double> _x;
  vector<uint__t> _rp;  
  vector<uint__t> _cpi;  
  vector<double> _rows;
  vector<double> _cols;  
  vector<double> _Pn;  
  vector<double> _Qn;
  vector<double> _Va;
  vector<double> _Vm;
  vector<uint> _btype;

public:
  CSRMatrix() : _n(0),_nnz(0){}

  void loadMatrix(uint__t n, uint__t nnz, size_t *p, uint__t *i, double *x, uint__t *rp, uint__t* cpi, double *rows, double *cols) {
    loadMatrix(n, nnz, p, i, x);
    loadAuxiliaryData(n, rp, cpi, rows, cols);
  }

  void loadMatrix(uint__t n, uint__t nnz, size_t *p, uint__t *i, double *x) { 
    assert(n >= 1);
    _n = n;
	_nnz = nnz;
    _p.resize(_n + 1);
    _i.resize(p[_n]);
    _x.resize(p[_n]);
    
    std::copy(p, p + n + 1, _p.begin());
    std::copy(i, i + p[_n], _i.begin());
    std::copy(x, x + p[_n], _x.begin());
  }

  void loadAuxiliaryData(uint__t n, uint__t *rp, uint__t* cpi, double *rows, double *cols) {
    assert(n >= 1);
    _n = n;
    _rp.resize(_n);
    _cpi.resize(_n);  
    _rows.resize(_n);
    _cols.resize(_n); 
    
    std::copy(rp, rp + n, _rp.begin());  
    std::copy(cpi, cpi + n, _cpi.begin());
    std::copy(rows, rows + n, _rows.begin());   
    std::copy(cols, cols + n, _cols.begin());
  }
  
    void loadMatrix(uint__t n, uint__t nnz, uint__t *p, uint__t *i, double *x, uint__t *btype, double *Va,double *Vm,double *Pn) {
    loadMatrix(n, nnz, p, i, x);
    loadBasecaseStatus(n, btype, Va, Vm, Pn);
  }
  
    void loadMatrix(uint__t n, uint__t nnz, uint__t *p, uint__t *i, double *x) { 
    assert(n >= 1);
    _n = n;
	_nnz = nnz;
    _bp.resize(_n + 1);
    _i.resize(p[_n]);
    _x.resize(p[_n]);
    
    std::copy(p, p + n + 1, _bp.begin());
    std::copy(i, i + p[_n], _i.begin());
    std::copy(x, x + p[_n], _x.begin());
  }
  
    void loadBasecaseStatus(uint__t n, uint__t *btype, double *Va, double *Vm, double *Pn) {
    assert(n >= 1);
    _n = n;
	_btype.resize(_n);
    _Va.resize(_n); 
	_Vm.resize(_n); 
    _Pn.resize(_n); 

	std::copy(btype, btype + n, _btype.begin());
    std::copy(Va, Va + n, _Va.begin());
	std::copy(Vm, Vm + n, _Vm.begin());
    std::copy(Pn, Pn + n, _Pn.begin());
  }

  bool isValid() const { return (_n >= 1) && (_p.size() == _n + 1) && (_p[_n] >= _n) && (_i.size() == _p[_n]) && (_x.size() == _p[_n]); }

  bool hasAuxiliaryData() const { return !_rp.empty(); }
  
  const uint__t *getBtype() const { return &_btype[0]; }
  const double *getVa() const { return &_Va[0]; }
  const double *getVm() const { return &_Vm[0]; }
  const double *getPn() const { return &_Pn[0]; }
  const uint__t *getBp() const { return &_bp[0]; }

  const size_t *getP() const { return &_p[0]; }
  const uint__t *getI() const { return &_i[0]; }
  const double *getX() const { return &_x[0]; }
  const uint__t *getRp() const { return &_rp[0]; }
  const uint__t *getCpi() const { return &_cpi[0]; }
  const double *getRows() const { return &_rows[0]; }
  const double *getCols() const { return &_cols[0]; }
  const uint__t getn() const {return _n; }
  const uint__t getnnz() const {return _nnz; }
};

class SingletonMatrixInterface {
private:

static CSRMatrix *getOrDeleteMatrixeG(bool isDelete) {
    static CSRMatrix *eG = NULL;
    static std::mutex mtx;
    mtx.lock();
    if (isDelete) {
      if (eG != NULL) {
        delete eG;
        eG = NULL;
      }
      assert(!eG);
    } else {
      if (eG == NULL) {
        eG = new CSRMatrix();
      }
      assert(eG);
    }
    mtx.unlock();
    return eG;
  }
  
  static CSRMatrix *getOrDeleteMatrixeB(bool isDelete) {
    static CSRMatrix *eB = NULL;
    static std::mutex mtx;
    mtx.lock();
    if (isDelete) {
      if (eB != NULL) {
        delete eB;
        eB = NULL;
      }
      assert(!eB);
    } else {
      if (eB == NULL) {
        eB = new CSRMatrix();
      }
      assert(eB);
    }
    mtx.unlock();
    return eB;
  }
  
  static CSRMatrix *getOrDeleteMatrixLineQ1(bool isDelete) {
    static CSRMatrix *LineQ1 = NULL;
    static std::mutex mtx;
    mtx.lock();
    if (isDelete) {
      if (LineQ1 != NULL) {
        delete LineQ1;
        LineQ1 = NULL;
      }
      assert(!LineQ1);
    } else {
      if (LineQ1 == NULL) {
        LineQ1 = new CSRMatrix();
      }
      assert(LineQ1);
    }
    mtx.unlock();
    return LineQ1;
  }
  
  
static CSRMatrix *getOrDeleteMatrixBp(bool isDelete) {
    static CSRMatrix *Bp = NULL;
    static std::mutex mtx;
    mtx.lock();
    if (isDelete) {
      if (Bp != NULL) {
        delete Bp;
        Bp = NULL;
      }
      assert(!Bp);
    } else {
      if (Bp == NULL) {
        Bp = new CSRMatrix();
      }
      assert(Bp);
    }
    mtx.unlock();
    return Bp;
  }

static CSRMatrix *getOrDeleteMatrixBpp(bool isDelete) {
    static CSRMatrix *Bpp = NULL;
    static std::mutex mtx;
    mtx.lock();
    if (isDelete) {
      if (Bpp != NULL) {
        delete Bpp;
        Bpp = NULL;
      }
      assert(!Bpp);
    } else {
      if (Bpp == NULL) {
        Bpp = new CSRMatrix();
      }
      assert(Bpp);
    }
    mtx.unlock();
    return Bpp;
  }

static CSRMatrix *getOrDeleteMatrixLp(bool isDelete) {
    static CSRMatrix *Lp = NULL;
    static std::mutex mtx;
    mtx.lock();
    if (isDelete) {
      if (Lp != NULL) {
        delete Lp;
        Lp = NULL;
      }
      assert(!Lp);
    } else {
      if (Lp == NULL) {
        Lp = new CSRMatrix();
      }
      assert(Lp);
    }
    mtx.unlock();
    return Lp;
  }

static CSRMatrix *getOrDeleteMatrixUp(bool isDelete) {
    static CSRMatrix *Up = NULL;
    static std::mutex mtx;
    mtx.lock();
    if (isDelete) {
      if (Up != NULL) {
        delete Up;
        Up = NULL;
      }
      assert(!Up);
    } else {
      if (Up == NULL) {
        Up = new CSRMatrix();
      }
      assert(Up);
    }
    mtx.unlock();
    return Up;
  }

static CSRMatrix *getOrDeleteMatrixLpp(bool isDelete) {
    static CSRMatrix *Lpp = NULL;
    static std::mutex mtx;
    mtx.lock();
    if (isDelete) {
      if (Lpp != NULL) {
        delete Lpp;
        Lpp = NULL;
      }
      assert(!Lpp);
    } else {
      if (Lpp == NULL) {
        Lpp = new CSRMatrix();
      }
      assert(Lpp);
    }
    mtx.unlock();
    return Lpp;
  }

static CSRMatrix *getOrDeleteMatrixUpp(bool isDelete) {
    static CSRMatrix *Upp = NULL;
    static std::mutex mtx;
    mtx.lock();
    if (isDelete) {
      if (Upp != NULL) {
        delete Upp;
        Upp = NULL;
      }
      assert(!Upp);
    } else {
      if (Upp == NULL) {
        Upp = new CSRMatrix();
      }
      assert(Upp);
    }
    mtx.unlock();
    return Upp;
  }
  
  
  // static CSRMatrix *getOrDeleteMatrixGainP(bool isDelete) {
    // static CSRMatrix *GainP = NULL;
    // static std::mutex mtx;
    // mtx.lock();
    // if (isDelete) {
      // if (GainP != NULL) {
        // delete GainP;
        // GainP = NULL;
      // }
      // assert(!GainP);
    // } else {
      // if (GainP == NULL) {
        // GainP = new CSRMatrix();
      // }
      // assert(GainP);
    // }
    // mtx.unlock();
    // return GainP;
  // }

// static CSRMatrix *getOrDeleteMatrixGainQ(bool isDelete) {
    // static CSRMatrix *GainQ = NULL;
    // static std::mutex mtx;
    // mtx.lock();
    // if (isDelete) {
      // if (GainQ != NULL) {
        // delete GainQ;
        // GainQ = NULL;
      // }
      // assert(!GainQ);
    // } else {
      // if (GainQ == NULL) {
        // GainQ = new CSRMatrix();
      // }
      // assert(GainQ);
    // }
    // mtx.unlock();
    // return GainQ;
  // }

static CSRMatrix *getOrDeleteMatrixGainP_L(bool isDelete) {
    static CSRMatrix *GainP_L = NULL;
    static std::mutex mtx;
    mtx.lock();
    if (isDelete) {
      if (GainP_L != NULL) {
        delete GainP_L;
        GainP_L = NULL;
      }
      assert(!GainP_L);
    } else {
      if (GainP_L == NULL) {
        GainP_L = new CSRMatrix();
      }
      assert(GainP_L);
    }
    mtx.unlock();
    return GainP_L;
  }

static CSRMatrix *getOrDeleteMatrixGainP_U(bool isDelete) {
    static CSRMatrix *GainP_U = NULL;
    static std::mutex mtx;
    mtx.lock();
    if (isDelete) {
      if (GainP_U != NULL) {
        delete GainP_U;
        GainP_U = NULL;
      }
      assert(!GainP_U);
    } else {
      if (GainP_U == NULL) {
        GainP_U = new CSRMatrix();
      }
      assert(GainP_U);
    }
    mtx.unlock();
    return GainP_U;
  }

static CSRMatrix *getOrDeleteMatrixGainQ_L(bool isDelete) {
    static CSRMatrix *GainQ_L = NULL;
    static std::mutex mtx;
    mtx.lock();
    if (isDelete) {
      if (GainQ_L != NULL) {
        delete GainQ_L;
        GainQ_L = NULL;
      }
      assert(!GainQ_L);
    } else {
      if (GainQ_L == NULL) {
        GainQ_L = new CSRMatrix();
      }
      assert(GainQ_L);
    }
    mtx.unlock();
    return GainQ_L;
  }

static CSRMatrix *getOrDeleteMatrixGainQ_U(bool isDelete) {
    static CSRMatrix *GainQ_U = NULL;
    static std::mutex mtx;
    mtx.lock();
    if (isDelete) {
      if (GainQ_U != NULL) {
        delete GainQ_U;
        GainQ_U = NULL;
      }
      assert(!GainQ_U);
    } else {
      if (GainQ_U == NULL) {
        GainQ_U = new CSRMatrix();
      }
      assert(GainQ_U);
    }
    mtx.unlock();
    return GainQ_U;
  }
  


public:

  static CSRMatrix *getMatrixeG() {return getOrDeleteMatrixeG(false); }
  static void deleteMatrixeG() { getOrDeleteMatrixeG(true); }

  static CSRMatrix *getMatrixeB() {return  getOrDeleteMatrixeB(false); }
  static void deleteMatrixeB() { getOrDeleteMatrixeB(true); }
  
  static CSRMatrix *getMatrixLineQ1() {return  getOrDeleteMatrixLineQ1(false); }
  static void deleteMatrixLineQ1() { getOrDeleteMatrixLineQ1(true); }
  
  
  static CSRMatrix *getMatrixBp() {return getOrDeleteMatrixBp(false); }
  static void deleteMatrixBp() { getOrDeleteMatrixBp(true); }

  static CSRMatrix *getMatrixBpp() {return  getOrDeleteMatrixBpp(false); }
  static void deleteMatrixBpp() { getOrDeleteMatrixBpp(true); }

  static CSRMatrix *getMatrixLp() {return getOrDeleteMatrixLp(false); }
  static void deleteMatrixLp() { getOrDeleteMatrixLp(true); }

  static CSRMatrix *getMatrixUp() {return getOrDeleteMatrixUp(false); }
  static void deleteMatrixUp() { getOrDeleteMatrixUp(true); }

  static CSRMatrix *getMatrixLpp() {return getOrDeleteMatrixLpp(false); }
  static void deleteMatrixLpp() { getOrDeleteMatrixLpp(true); }

  static CSRMatrix *getMatrixUpp() {return getOrDeleteMatrixUpp(false); }
  static void deleteMatrixUpp() { getOrDeleteMatrixUpp(true); }
  
  
  // static CSRMatrix *getMatrixGainP() {return getOrDeleteMatrixGainP(false); }
  // static void deleteMatrixGainP() { getOrDeleteMatrixGainP(true); }

  // static CSRMatrix *getMatrixGainQ() {return  getOrDeleteMatrixGainQ(false); }
  // static void deleteMatrixGainQ() { getOrDeleteMatrixGainQ(true); }

  static CSRMatrix *getMatrixGainP_L() {return getOrDeleteMatrixGainP_L(false); }
  static void deleteMatrixGainP_L() { getOrDeleteMatrixGainP_L(true); }

  static CSRMatrix *getMatrixGainP_U() {return getOrDeleteMatrixGainP_U(false); }
  static void deleteMatrixGainP_U() { getOrDeleteMatrixGainP_U(true); }

  static CSRMatrix *getMatrixGainQ_L() {return getOrDeleteMatrixGainQ_L(false); }
  static void deleteMatrixGainQ_L() { getOrDeleteMatrixGainQ_L(true); }

  static CSRMatrix *getMatrixGainQ_U() {return getOrDeleteMatrixGainQ_U(false); }
  static void deleteMatrixGainQ_U() { getOrDeleteMatrixGainQ_U(true); }

};

/*
 * Define structs that used in the functions in "ExprFunctions.hpp"
 * below. For example,
 *
 *   struct Person {
 *     string name;
 *     int age;
 *     double height;
 *     double weight;
 *   }
 *
 */



#endif /* EXPRUTIL_HPP_ */
