/*interface of GRAPHLU*/
/*last modified: june 7, 2013*/
/*author: Chen, Xiaoming*/

#ifndef __GRAPHLU__
#define __GRAPHLU__

#include "graph_config.h"

/*return code*/
#define GRAPHLU_GENERAL_FAIL							(-1)
#define GRAPHLU_ARGUMENT_ERROR						(-2)
#define GRAPHLU_MEMORY_OVERFLOW						(-3)
#define GRAPHLU_FILE_CANNOT_OPEN						(-4)
#define GRAPHLU_MATRIX_STRUCTURAL_SINGULAR			(-5)
#define GRAPHLU_MATRIX_NUMERIC_SINGULAR				(-6)
#define GRAPHLU_MATRIX_INVALID						(-7)
#define GRAPHLU_MATRIX_ENTRY_DUPLICATED				(-8)
#define GRAPHLU_THREADS_NOT_INITIALIZED				(-9)
#define GRAPHLU_MATRIX_NOT_INITIALIZED				(-10)
#define GRAPHLU_SCHEDULER_NOT_INITIALIZED			(-11)
#define GRAPHLU_SINGLE_THREAD						(-12)
#define GRAPHLU_THREADS_INIT_FAIL					(-13)
#define GRAPHLU_MATRIX_NOT_ANALYZED					(-14)
#define GRAPHLU_MATRIX_NOT_FACTORIZED				(-15)
#define GRAPHLU_NUMERIC_OVERFLOW						(-16)
#define GRAPHLU_USE_SEQUENTIAL_FACTORIZATION			(+1)
#define GRAPHLU_BIND_THREADS_FAIL					(+2)

/*******************************************************************************/
/*definition of the main structure*/
typedef struct tagSGraphLU
{
	/*flags*/
	bool__t *flag;

	/*statistics*/
	real__t *stat;

	/*configurations*/
	uint__t *cfgi;
	real__t *cfgf;

	/*matrix data, 6 items*/
	uint__t n;				/*dimension*/
	uint__t nnz;			/*nonzeros of A*/
	real__t *ax;			/*value*/
	uint__t *ai;			/*column/row index*/
	uint__t *ap;			/*row/column header*/
	real__t *rhs;			/*for solve and tsolve*/

	/*other matrix data, 9 items*/
	uint__t *row_perm;		/*row_perm[i]=j-->row i in the permuted matrix is row j in the original matrix*/
	uint__t *row_perm_inv;	/*row_perm_inv[i]=j-->row i in the original matrix is row j in the permuted matrix*/
	uint__t *col_perm;
	uint__t *col_perm_inv;
	real__t *col_scale_perm;/*permuted*/
	real__t *row_scale;		/*not permuted*/
	real__t *cscale;
	int__t *pivot;			/*pivot[i]=j-->column j is the ith pivot column*/
	int__t *pivot_inv;		/*pivot_inv[i]=j-->column i is the jth pivot column*/
	int__t *singular;               /*singular flag*/

	/*lu matrix, 13 items*/
	size_t lu_nnz_est;		/*estimated total nnz by AMD*/
	size_t lu_nnz;			/*nonzeros of factorized matrix L+U-I*/
	size_t l_nnz;			/*inclu diag*/
	size_t u_nnz;			/*inclu diag*/
	real__t *ldiag;			/*udiag=1.0*/
	void *lu_array;			/*lu index and data*/
	size_t *up;				/*u row header, the header of each row*/
	uint__t *llen;			/*exclu diag*/
	uint__t *ulen;			/*exclu diag*/
	size_t *len_est;		/*estimated len, for parallelism, in bytes*/
	size_t *wkld_est;		/*estimated workload, for parallelism*/
	byte__t *row_state;		/*row state, finished or un-finished*/
	void **lu_array2;		/*for parallelism*/

	/*work space, 3 items*/
	void *workspace;
	void **workspace_mt1;
	void **workspace_mt2;

	/*for parallelism, 10 items*/
	volatile int thread_work;
	void *thread_id;		/*thread id, internal structure*/
	void *thread_arg;		/*thread argument, internal structure*/
	bool__t *thread_active;
	bool__t *thread_finish;
	uint__t *cluster_start;
	uint__t *cluster_end;
	uint__t pipeline_start;
	uint__t pipeline_end;
	uint__t last_busy;

	/*aegraph, 6 items*/
	uint__t aeg_level;
	uint__t *aeg_data;
	uint__t *aeg_header;
	uint__t aeg_refact_level;
	uint__t *aeg_refact_data;
	uint__t *aeg_refact_header;

	/*timer*/
	void *timer;

	/*Jin: 06/23/2017 store etree to gsql */
	int__t *parents;			//The structure of ETree. For AEGraph + GSQL
	uint__t *levels_of_nodes;	//The levels of each node in the AEGraph.



} SGraphLU;


#define IN__
#define OUT__
#define INOUT__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/*initialize the main structure. must be called at first*/
/*call it ONLY ONCE. don't repeatedly call this routine*/
int \
	GraphLU_Initialize( \
	INOUT__ SGraphLU *graphlu);

/*destroy the main structure and free all memories. must be called in the last*/
int \
	GraphLU_Destroy( \
	INOUT__ SGraphLU *graphlu);

/*initialize the matrix*/
/*all configurations should be set AFTER this routine*/
/*if it is called repeatedly, the previous allocated memory will be freed*/
int \
	GraphLU_CreateMatrix( \
	INOUT__ SGraphLU *graphlu, \
	IN__ uint__t n, \
	IN__ uint__t nnz, \
	IN__ real__t *ax, \
	IN__ uint__t *ai, \
	IN__ uint__t *ap);

/*create the scheduler for parallel factorization and re-factorization*/
/*return 0: suggest using parallel version*/
/*return >0: suggest using sequential version*/
/*the suggestion is only for GraphLU_Factorize_MT*/
/*GraphLU_ReFactorize_MT can always get effective speedups*/
/*if it is called repeatedly, the previous allocated memory will be freed*/
int \
	GraphLU_CreateScheduler( \
	INOUT__ SGraphLU *graphlu);

/*create threads for parallel factoriztion and re-factorization*/
/*it first calls GraphLU_DestroyThreads to destroy the previously created threads*/
int \
	GraphLU_CreateThreads( \
	INOUT__ SGraphLU *graphlu, \
	IN__ unsigned int threads, \
	IN__ bool__t check);/*whether to check the validity of #threads*/

/*bind threads to cores or unbind*/
int \
	GraphLU_BindThreads( \
	IN__ SGraphLU *graphlu, \
	IN__ bool__t unbind);

/*destroy threads. this routine is included in GraphLU_Destroy*/
int \
	GraphLU_DestroyThreads( \
	INOUT__ SGraphLU *graphlu);

/*pre-processing, including ordering and static pivoting*/
//int \
        GraphLU_Analyze( \ INOUT__ SGraphLU *graphlu);

int \
	GraphLU_Analyze_Opt( \
	INOUT__ SGraphLU *graphlu,
	IN__ int option);

int \
	GraphLU_Analyze( \
	INOUT__ SGraphLU *graphlu);

int \
	GraphLU_Analyze_NoAMD( \
	INOUT__ SGraphLU *graphlu,
        IN__ int__t *p,
        IN__ int__t *pinv);

int \
 	GraphLU_GetAnalyze( \ 
	INOUT__ SGraphLU *graphlu, 
	IN__ uint__t *row_perm, 
	IN__ uint__t *row_perm_inv, 
	IN__ uint__t *col_perm, 
        IN__ uint__t *col_perm_inv, 
	IN__ real__t *row_scale,
	IN__ real__t *col_scale_perm);

/*LU factorization, with partial pivoting*/
/*before called, GraphLU_Analyze must be called*/
int \
	GraphLU_Factorize( \
	INOUT__ SGraphLU *graphlu);

/*LU factorization, without partial pivoting*/
/*before called, GraphLU_Factorize or GraphLU_Factorize_MT must be called at least once*/
int \
	GraphLU_ReFactorize( \
	INOUT__ SGraphLU *graphlu, \
	IN__ real__t *ax);

/*multi-threaded version of GraphLU_Factorize*/
int \
	GraphLU_Factorize_MT( \
	INOUT__ SGraphLU *graphlu);

/*multi-threaded version of GraphLU_ReFactorize*/
/*before called, GraphLU_Factorize or GraphLU_Factorize_MT must be called at least once*/
int \
	GraphLU_ReFactorize_MT( \
	INOUT__ SGraphLU *graphlu, \
	IN__ real__t *ax);

/*solve the linear system Ax=b after LU factorization*/
int \
	GraphLU_Solve( \
	INOUT__ SGraphLU *graphlu, \
	INOUT__ real__t *rhs);/*for inputs, it's b, for outputs, it's overwritten by x*/

/*when there are many zeros in b, this routine is faster than GraphLU_Solve*/
int \
	GraphLU_SolveFast( \
	INOUT__ SGraphLU *graphlu, \
	INOUT__ real__t *rhs);/*for inputs, it's b, for outputs, it's overwritten by x*/

/*solve the linear system Ax=b after LU factorization*/
int \
	GraphLU_Solve_Singular( \
	INOUT__ SGraphLU *graphlu, \
	INOUT__ real__t *rhs,
	IN__ real__t *ref);/*for inputs, it's b, for outputs, it's overwritten by x*/

/*when values of A are changed but the nonzero pattern is not changed, this routine resets the values*/
/*then a new LU factorization can be performed*/
/*if the nonzero pattern is also changed, call GraphLU_CreateMatrix and re-preform the whole process*/
int \
	GraphLU_ResetMatrixValues( \
	INOUT__ SGraphLU *graphlu, \
	IN__ real__t *ax);

/*refine the results. this routine is not always successful*/
int \
	GraphLU_Refine( \
	INOUT__ SGraphLU *graphlu, \
	INOUT__ real__t *x, \
	IN__ real__t *b, \
	IN__ real__t eps, \
	IN__ uint__t maxiter);/*if set to 0, then no constraint for iteration count*/

/*total memory access in LU factorization, including read and write, in bytes*/
/*call it after factorization*/
int \
	GraphLU_Throughput( \
	IN__ SGraphLU *graphlu, \
	OUT__ real__t *thr);

/*number of floating-point operations in LU factorization*/
/*call it after factorization*/
int \
	GraphLU_Flops( \
	INOUT__ SGraphLU *graphlu, \
	OUT__ real__t *flops);

/*flops of each thread*/
/*call it after factorization*/
int \
	GraphLU_ThreadLoad( \
	IN__ SGraphLU *graphlu, \
	IN__ unsigned int threads, \
	OUT__ real__t **thread_flops);

/*extract A after pre-processing*/
int \
	GraphLU_DumpA( \
	IN__ SGraphLU *graphlu, \
	OUT__ real__t **ax, \
	OUT__ uint__t **ai, \
	OUT__ uint__t **ap);

/*extract LU factors after LU factorization*/
int \
	GraphLU_DumpLU( \
	IN__ SGraphLU *graphlu, \
	OUT__ real__t **lx, \
	OUT__ uint__t **li, \
	OUT__ size_t **lp, \
	OUT__ real__t **ux, \
	OUT__ uint__t **ui, \
	OUT__ size_t **up);

/*condition number estimation*/
/*call it after factorization*/
int \
	GraphLU_ConditionNumber( \
	INOUT__ SGraphLU *graphlu, \
	OUT__ real__t *cond);

/*memory used by GRAPHLU, in bytes*/
/*return an approximate value*/
int \
	GraphLU_MemoryUsage( \
	IN__ SGraphLU *graphlu, \
	OUT__ real__t *memuse);


/*******************************************************************************/
/*the following routines are without the SGraphLU structure*/

/*residual error = |Ax-b|*/
/*1-norm(norm=1), 2-norm(norm=2), ¡Þ-norm(norm=other)*/
/*mode=0:row mode, mode=1:column mode*/
int \
	GraphLU_Residual( \
	IN__ uint__t n, \
	IN__ real__t *ax, \
	IN__ uint__t *ai, \
	IN__ uint__t *ap, \
	IN__ real__t *x, \
	IN__ real__t *b, \
	OUT__ real__t *error, \
	IN__ int norm, \
	IN__ int mode);

/*transpose the matrix stored in CSR or CSC format*/
int \
	GraphLU_Transpose( \
	IN__ uint__t n, \
	IN__ uint__t nnz, \
	INOUT__ real__t *ax, \
	INOUT__ uint__t *ai, \
	INOUT__ uint__t *ap);

/*sort the CSR or CSC storage*/
/*using a radix-sort like method*/
int \
	GraphLU_Sort( \
	IN__ uint__t n, \
	IN__ uint__t nnz, \
	INOUT__ real__t *ax, \
	INOUT__ uint__t *ai, \
	INOUT__ uint__t *ap);

/*merge duplicated entries in CSR/CSC*/
/*this routine also sorts the matrix*/
int \
	GraphLU_MergeDuplicateEntries( \
	IN__ uint__t n, \
	INOUT__ uint__t *nnz, \
	INOUT__ real__t **ax, \
	INOUT__ uint__t **ai, \
	INOUT__ uint__t **ap);

int \
	permute_and_scale(SGraphLU *graphlu);

#ifdef __cplusplus
}
#endif

#endif
