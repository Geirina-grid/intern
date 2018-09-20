/*create and destroy the scheduler*/
/*last modified: june 7, 2013*/
/*author: Chen, Xiaoming*/

#include "graphlu.h"
#include "graphlu_internal.h"
#include "timer_c.h"

int GraphLU_CreateScheduler(SGraphLU *graphlu)
{
	uint__t n;
	size_t size;
	void *ptr;
/*	bool__t a, b;
	real__t nnz;*/
	int err;
	
	if (NULL == graphlu)
	{
		return GRAPHLU_ARGUMENT_ERROR;
	}
	if (!graphlu->flag[1])
	{
		return GRAPHLU_MATRIX_NOT_ANALYZED;
	}

	TimerStart((STimer *)(graphlu->timer));
	_I_GraphLU_DestroyScheduler(graphlu);

	n = graphlu->n;

	/*wkld_est & len_est*/
	size = sizeof(size_t)*(n+n);
	ptr = malloc(size);
	if (NULL == ptr)
	{
		return GRAPHLU_MEMORY_OVERFLOW;
	}
	graphlu->wkld_est = (size_t *)ptr;
	ptr = ((size_t *)ptr) + n;
	graphlu->len_est = (size_t *)ptr;

	/*row state*/
	size = sizeof(byte__t)*n;
	ptr = malloc(size);
	if (NULL == ptr)
	{
		return GRAPHLU_MEMORY_OVERFLOW;
	}
	graphlu->row_state = (byte__t *)ptr;

	/*scheduler*/
	size = sizeof(uint__t) * (4*n+2);
	ptr = malloc(size);
	if (NULL == ptr)
	{
		return GRAPHLU_MEMORY_OVERFLOW;
	}
	graphlu->aeg_data = (uint__t *)ptr;
	ptr = ((uint__t *)ptr) + n;
	graphlu->aeg_header = (uint__t *)ptr;
	ptr = ((uint__t *)ptr) + n + 1;
	graphlu->aeg_refact_data = (uint__t *)ptr;
	ptr = ((uint__t *)ptr) + n;
	graphlu->aeg_refact_header = (uint__t *)ptr;

	/*lu_array2*/
	size = sizeof(void *) * n;
	ptr = malloc(size);
	if (NULL == ptr)
	{
		return GRAPHLU_MEMORY_OVERFLOW;
	}
	memset(ptr, 0, size);
	graphlu->lu_array2 = (void **)ptr;

	/*calculate the scheduler*/
	 err = _I_GraphLU_CreateETree(graphlu);
	if (FAIL(err)) return err;

	err = _I_GraphLU_StaticSymbolicFactorize(graphlu);
	if (FAIL(err)) return err;

	/*finish*/
	graphlu->flag[4] = TRUE;
	
/*	nnz = graphlu->stat[11];
	a = ((nnz/(real__t)(graphlu->nnz)) >= 1.99999999);
	b = ((graphlu->stat[10]/nnz) >= 50.);*/

	TimerStop((STimer *)(graphlu->timer));
	graphlu->stat[4] = TimerGetRuntime((STimer *)(graphlu->timer));

/*	err = (!a && !b);*/
	err = ((graphlu->stat[10]/graphlu->stat[11] < 50.) ? GRAPHLU_USE_SEQUENTIAL_FACTORIZATION : GRAPH_OK);
	graphlu->stat[13] = err;
	return err;
}

int _I_GraphLU_DestroyScheduler(SGraphLU *graphlu)
{
	uint__t i, n;
	void **ppv;

	if (NULL == graphlu)
	{
		return GRAPHLU_ARGUMENT_ERROR;
	}
	n = graphlu->n;

	if (graphlu->wkld_est != NULL)
	{
		free(graphlu->wkld_est);
		graphlu->wkld_est = NULL;
	}
	graphlu->len_est = NULL;
	if (graphlu->row_state != NULL)
	{
		free(graphlu->row_state);
		graphlu->row_state = NULL;
	}
	if (graphlu->aeg_data != NULL)
	{
		free(graphlu->aeg_data);
		graphlu->aeg_data = NULL;
	}
	graphlu->aeg_header = NULL;
	graphlu->aeg_refact_data = NULL;
	graphlu->aeg_refact_header = NULL;

	if (graphlu->lu_array2 != NULL)
	{
		ppv = graphlu->lu_array2;
		for (i=0; i<n; ++i)
		{
			if (ppv[i] != NULL)
			{
				free(ppv[i]);
			}
		}
		free(ppv);
		graphlu->lu_array2 = NULL;
	}

	graphlu->flag[4] = FALSE;
	return GRAPH_OK;
}
