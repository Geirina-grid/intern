/*create and destroy the matrix*/
/*last modified: june 16, 2013*/
/*author: Chen, Xiaoming*/

#include "graphlu.h"
#include "graphlu_internal.h"
#include "system.h"
#include "graphlu_default.h"

int GraphLU_CreateMatrix(SGraphLU *graphlu, uint__t n, uint__t nnz, real__t *ax, uint__t *ai, uint__t *ap)
{
	int err;
	void *ptr;
	size_t size;

	if (NULL == graphlu)
	{
		return GRAPHLU_ARGUMENT_ERROR;
	}

	/*destroy*/
	_I_GraphLU_DestroyMatrix(graphlu);

	if (NULL == ax || NULL == ai || NULL == ap)
	{
		return GRAPHLU_ARGUMENT_ERROR;
	}

	err = _I_GraphLU_CheckMatrix(n, nnz, ai, ap);
	if (FAIL(err))
	{
		return err;
	}

	err = _I_GraphLU_AllocMatrixMemory(n, nnz, graphlu);
	if (FAIL(err))
	{
		return err;
	}

	graphlu->n = n;
	graphlu->nnz = nnz;

	memcpy(graphlu->ax, ax, sizeof(real__t)*nnz);
	memcpy(graphlu->ai, ai, sizeof(int__t)*nnz);
	memcpy(graphlu->ap, ap, sizeof(int__t)*(1+n));

	/*perm*/
	size = sizeof(int__t)*n;
	ptr = malloc(size*7);
	if (NULL == ptr)
	{
		return GRAPHLU_MEMORY_OVERFLOW;
	}
	memset(ptr, 0, size);
	graphlu->row_perm = (uint__t *)ptr;
	ptr = ((uint__t *)ptr) + n;
	graphlu->row_perm_inv = (uint__t *)ptr;
	ptr = ((uint__t *)ptr) + n;
	graphlu->col_perm = (uint__t *)ptr;
	ptr = ((uint__t *)ptr) + n;
	graphlu->col_perm_inv = (uint__t *)ptr;
	ptr = ((uint__t *)ptr) + n;
	graphlu->pivot = (int__t *)ptr;
	ptr = ((uint__t *)ptr) + n;
	graphlu->pivot_inv = (int__t *)ptr;
	ptr = ((int__t *)ptr) + n;
	memset(ptr, 0, size);
	graphlu->singular = (int__t *)ptr;

	/*mc64_scale & scale*/
	size = sizeof(real__t)*n*3;
	ptr = malloc(size);
	if (NULL == ptr)
	{
		return GRAPHLU_MEMORY_OVERFLOW;
	}
	graphlu->col_scale_perm = (real__t *)ptr;
	ptr = ((real__t *)ptr) + n;
	graphlu->row_scale = (real__t *)ptr;
	ptr = ((real__t *)ptr) + n;
	graphlu->cscale = (real__t *)ptr;

	/*lu matrix*/
	size = sizeof(real__t)*n;
	ptr = malloc(size);
	if (NULL == ptr)
	{
		return GRAPHLU_MEMORY_OVERFLOW;
	}
	graphlu->ldiag = (real__t *)ptr;

	size = sizeof(size_t)*n;
	ptr = malloc(size);
	if (NULL == ptr)
	{
		return GRAPHLU_MEMORY_OVERFLOW;
	}
	graphlu->up = (size_t *)ptr;

	size = sizeof(uint__t)*(n+n);
	ptr = malloc(size);
	if (NULL == ptr)
	{
		return GRAPHLU_MEMORY_OVERFLOW;
	}
	graphlu->llen = (uint__t *)ptr;
	ptr = ((uint__t *)ptr) + n;
	graphlu->ulen = (uint__t *)ptr;

	/*work space*/
	size = (sizeof(int__t)*4+sizeof(real__t)) * n;
	ptr = malloc(size);
	if (NULL == ptr)
	{
		return GRAPHLU_MEMORY_OVERFLOW;
	}
	graphlu->workspace = ptr;

	/*finish*/
	graphlu->flag[0] = TRUE;
	int i;

/*
	for (i = 0; i < nnz; i++)
	{
		printf("Debug: ai[%d]: %d", i, ai[i]);
	}
*/
	return GRAPH_OK;
}

int _I_GraphLU_DestroyMatrix(SGraphLU *graphlu)
{
	if (NULL == graphlu)
	{
		return GRAPHLU_ARGUMENT_ERROR;
	}

	GraphLU_DestroyThreads(graphlu);
	_I_GraphLU_DestroyScheduler(graphlu);

	if (graphlu->flag != NULL)
	{
		memset(graphlu->flag, 0, sizeof(bool__t)*32);
	}
	if (graphlu->stat != NULL)
	{
		memset(graphlu->stat, 0, sizeof(real__t)*32);
		graphlu->stat[9] = GetProcessorNumber();
	}
	if (graphlu->cfgi != NULL)
	{
		memset(graphlu->cfgi, 0, sizeof(uint__t)*32);
		graphlu->cfgi[0] = 0;/*row/column*/
		graphlu->cfgi[1] = 1;/*mc64*/
		graphlu->cfgi[2] = 0;/*scale*/
		graphlu->cfgi[3] = GRAPHLU_PIPELINE_THRESHOLD;
		graphlu->cfgi[4] = GRAPHLU_STATIC_RNNZ_UB;
		graphlu->cfgi[5] = 1;/*threads created*/
		graphlu->cfgi[6] = GRAPHLU_AMD_FLAG1;
		graphlu->cfgi[7] = 1;/*threads used*/
		graphlu->cfgi[8] = GetProcessorNumber();
	}
	if (graphlu->cfgf != NULL)
	{
		memset(graphlu->cfgf, 0, sizeof(real__t)*32);
		graphlu->cfgf[0] = GRAPHLU_PIVOT_TOLERANCE;
		graphlu->cfgf[1] = GRAPHLU_STATIC_MEMORY_MULT;
		graphlu->cfgf[2] = GRAPHLU_AMD_FLAG2;
		graphlu->cfgf[3] = GRAPHLU_SYNC_CYCLES;
		graphlu->cfgf[4] = GRAPHLU_LOAD_BALANCE;
		graphlu->cfgf[5] = GRAPHLU_MEMORY_GROW;
	}

	if (graphlu->ax != NULL)
	{
		free(graphlu->ax);
		graphlu->ax = NULL;
	}
	if (graphlu->ai != NULL)
	{
		free(graphlu->ai);
		graphlu->ai = NULL;
	}
	if (graphlu->ap != NULL)
	{
		free(graphlu->ap);
		graphlu->ap = NULL;
	}
	if (graphlu->rhs != NULL)
	{
		free(graphlu->rhs);
		graphlu->rhs = NULL;
	}
	graphlu->n = 0;
	graphlu->nnz = 0;

	if (graphlu->row_perm != NULL)
	{
		free(graphlu->row_perm);
	}
	graphlu->row_perm = NULL;
	graphlu->row_perm_inv = NULL;
	graphlu->col_perm = NULL;
	graphlu->col_perm_inv = NULL;
	graphlu->pivot = NULL;
	graphlu->pivot_inv = NULL;
	graphlu->singular = NULL;

	if (graphlu->col_scale_perm != NULL)
	{
		free(graphlu->col_scale_perm);
		graphlu->col_scale_perm = NULL;
	}
	graphlu->row_scale = NULL;
	graphlu->cscale = NULL;

	graphlu->lu_nnz_est = 0;
	graphlu->lu_nnz = 0;
	graphlu->l_nnz = 0;
	graphlu->u_nnz = 0;

	if (graphlu->ldiag != NULL)
	{
		free(graphlu->ldiag);
		graphlu->ldiag = NULL;
	}
	if (graphlu->lu_array != NULL)
	{
		free(graphlu->lu_array);
		graphlu->lu_array = NULL;
	}
	if (graphlu->up != NULL)
	{
		free(graphlu->up);
		graphlu->up = NULL;
	}
	if (graphlu->llen != NULL)
	{
		free(graphlu->llen);
		graphlu->llen = NULL;
	}
	graphlu->ulen = NULL;

	if (graphlu->workspace != NULL)
	{
		free(graphlu->workspace);
		graphlu->workspace = NULL;
	}

	return GRAPH_OK;
}

int GraphLU_ResetMatrixValues(SGraphLU *graphlu, real__t *ax)
{
	int err;

	if (NULL == graphlu || NULL == ax)
	{
		return GRAPHLU_ARGUMENT_ERROR;
	}
	if (!graphlu->flag[0])
	{
		return GRAPHLU_MATRIX_NOT_INITIALIZED;
	}
	graphlu->flag[2] = FALSE;
	graphlu->flag[5] = FALSE;

	memcpy(graphlu->ax, ax, sizeof(real__t)*(graphlu->nnz));

	if (graphlu->flag[1])
	{
		_I_GraphLU_MC64Scale(graphlu);
		err = _I_GraphLU_Scale(graphlu);
		if (FAIL(err)) return err;
	}

	return GRAPH_OK;
}
