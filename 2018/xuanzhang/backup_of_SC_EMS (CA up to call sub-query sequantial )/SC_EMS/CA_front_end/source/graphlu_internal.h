#ifndef __GRAPHLU_INTERNAL__
#define __GRAPHLU_INTERNAL__

#include "graphlu.h"
#include "thread.h"


#define OK(code)						((code) >= GRAPH_OK)
#define FAIL(code)						((code) < GRAPH_OK)
#define WARNING(code)					((code) > GRAPH_OK)

/*warning code*/
#define GRAPHLU_MATRIX_NOT_SORTED		(1)

typedef struct tagSGraphLUThreadArg
{
	SGraphLU *graphlu;
	unsigned int id;
	size_t lnnz;
	size_t unnz;
	uint__t offdiag;
	int err;
} SGraphLUThreadArg;

#define GRAPHLU_WORK_EXIT				(-1)
#define GRAPHLU_WORK_NONE				(0)
#define GRAPHLU_WORK_FACT_CLUSTER		(1)
#define GRAPHLU_WORK_FACT_PIPELINE		(2)
#define GRAPHLU_WORK_REFACT_CLUSTER		(3)
#define GRAPHLU_WORK_REFACT_PIPELINE		(4)
#define GRAPHLU_WORK_COPY_DATA			(5)


/*internal functions*/
#ifdef __cplusplus
extern "C" {
#endif

int			_I_GraphLU_CheckMatrix(uint__t n, uint__t nnz, uint__t *ai, uint__t *ap);
int			_I_GraphLU_Check(IN__ SGraphLU *graphlu);
int			_I_GraphLU_AllocMatrixMemory(IN__ uint__t n, IN__ uint__t nnz, INOUT__ SGraphLU *graphlu);
int			_I_GraphLU_ConstructCSR(SGraphLU *graphlu, int__t *match, uint__t *ai, uint__t *ap);
int			_I_GraphLU_Permute(SGraphLU *graphlu, int__t *match, int__t *p, int__t *pinv);
void		_I_GraphLU_Residual(SGraphLU *graphlu, real__t *sol, real__t *b, real__t *err);

/*amd*/
void		_I_GraphLU_AMDSort(IN__ uint__t n, IN__ uint__t *ai, IN__ uint__t *ap, IN__ int__t *w, OUT__ uint__t *ci, OUT__ uint__t *cp);
uint__t		_I_GraphLU_AAT(IN__ uint__t n, IN__ uint__t *ai, IN__ uint__t *ap, IN__ int__t *len, IN__ int__t *tp);
void		_I_GraphLU_AAT2(IN__ uint__t n, IN__ uint__t *ai, IN__ uint__t *ap, IN__ int__t *len, OUT__ int__t *pe, IN__ int__t *sp, OUT__ int__t *iw, IN__ int__t *tp);
void		_I_GraphLU_AMD(IN__ int__t n, IN__ int__t nnz, IN__ int__t iwlen, IN__ int__t *pe, IN__ int__t *iw, IN__ int__t *len, IN__ int__t *work, \
								OUT__ int__t *p, OUT__ int__t *pinv, real__t alpha, int__t aggr, size_t *lunnz);
void		_I_GraphLU_PostOrder(int__t n, int__t *parent, int__t *nv, int__t *fsize, int__t *order, int__t *child, int__t *sibling, int__t *stack);
int__t		_I_GraphLU_PostTree(int__t root, int__t k, int__t *child, int__t *sibling, int__t *order, int__t *stack);

/*mc64*/
int__t		_I_GraphLU_MC64ad(IN__ uint__t n, IN__ uint__t nnz, IN__ uint__t *ai, IN__ uint__t *ap, IN__ real__t *ax, \
								OUT__ int__t *match, OUT__ int__t *match2, IN__ uint__t liw, INOUT__ int__t *iw, IN__ uint__t ldw, INOUT__ real__t *dw);
int__t		_I_GraphLU_MC64wd(IN__ uint__t n, IN__ uint__t nnz, IN__ uint__t *ai, IN__ uint__t *ap, IN__ real__t *ax, OUT__ int__t *iperm, OUT__ int__t *jperm, \
								OUT__ int__t *out, IN__ int__t *pr, IN__ int__t *q, IN__ int__t *l, OUT__ real__t *u, OUT__ real__t *d__);
void		_I_GraphLU_MC64dd(uint__t i, uint__t n, int__t *q, real__t *d__, int__t *l, uint__t iway);
void		_I_GraphLU_MC64ed(int__t *qlen, uint__t n, int__t *q, real__t *d__, int__t *l, uint__t iway);
void		_I_GraphLU_MC64fd(int__t pos0, int__t *qlen, uint__t n, int__t *q, real__t *d__, int__t *l, uint__t iway);

/*mc64_scale*/
void		_I_GraphLU_MC64Scale(SGraphLU *graphlu);
void		_I_GraphLU_MC64ScaleForRefact(SGraphLU *graphlu, real__t *ax0);

/*scale*/
int			_I_GraphLU_Scale(SGraphLU *graphlu);

/*thread proc*/
THREAD_DECL	_I_GraphLU_ThreadProc(void *arg);

/*scheduler for refact*/
void		_I_GraphLU_CreateAEGraphForRefact(SGraphLU *graphlu);

/*static symbolic*/
int			_I_GraphLU_StaticSymbolicFactorize(SGraphLU *graphlu);

/*etree*/
int			_I_GraphLU_CreateETree(SGraphLU *graphlu);/*with building aegraph for fact*/

/*kernel functions*/
int__t		_I_GraphLU_Symbolic(uint__t n, uint__t k, \
								int__t *pinv, int__t *stack, int__t *flag, int__t *pend, int__t *appos, \
								uint__t *uindex, uint__t *ulen, void *lu, size_t *up, uint__t *aidx, uint__t arownnz);
int__t		_I_GraphLU_Symbolic_Cluster(void **lu, uint__t n, uint__t k, \
								int__t *pinv, int__t *stack, int__t *flag, int__t *pend, int__t *appos, \
								uint__t *uindex, uint__t *ulen, uint__t *aidx, uint__t arownnz);
int__t		_I_GraphLU_Symbolic_Pipeline(void **lu, uint__t n, uint__t k, \
								int__t *pinv, int__t *stack, int__t *flag, int__t *pend, int__t *appos, \
								uint__t *uindex, uint__t *ulen, uint__t *aidx, uint__t arownnz, int__t *pruned);
int			_I_GraphLU_Pivot(int__t diagcol, uint__t *ulen, size_t up, \
								real__t tol, real__t *x, uint__t *p_pivcol, real__t *p_pivot, void *lu);
int			_I_GraphLU_Pivot_Parallel(int__t diagcol, uint__t *ulen, uint__t *uip, \
								real__t tol, real__t *x, uint__t *p_pivcol, real__t *p_pivot, void *lu);
void		_I_GraphLU_Prune(int__t *pend, uint__t llen, uint__t *ulen, int__t *pinv, \
								int__t pivcol, uint__t *lip, size_t *ui, void *lu);
void		_I_GraphLU_Prune_Parallel(int__t *pend, uint__t llen, uint__t *ulen, int__t *pinv, \
								int__t pivcol, uint__t *lip, void **lu);

/*parallelism*/
void		_I_GraphLU_Factorize_Cluster(SGraphLU *graphlu, SGraphLUThreadArg *tharg, unsigned int no);
void		_I_GraphLU_Factorize_Pipeline(SGraphLU *graphlu, SGraphLUThreadArg *tharg, unsigned int no);
void		_I_GraphLU_ReFactorize_Cluster(SGraphLU *graphlu, SGraphLUThreadArg *tharg, unsigned int no);
void		_I_GraphLU_ReFactorize_Pipeline(SGraphLU *graphlu, SGraphLUThreadArg *tharg, unsigned int no);
void		_I_GraphLU_CopyData(SGraphLU *graphlu, unsigned int no);

int			_I_GraphLU_DestroyMatrix(INOUT__ SGraphLU *graphlu);
int			_I_GraphLU_DestroyScheduler(INOUT__ SGraphLU *graphlu);

int			_I_GraphLU_Sort(uint__t n, real__t *ax, uint__t *ai, uint__t *ap, \
						   real__t *bx, uint__t *bi, uint__t *bp);

#ifdef __cplusplus
}
#endif

/*#define GRAPHLU_DEBUG*/

#endif
