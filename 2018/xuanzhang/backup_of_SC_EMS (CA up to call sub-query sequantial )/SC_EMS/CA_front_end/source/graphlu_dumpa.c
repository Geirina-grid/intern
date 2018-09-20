#include "graphlu.h"
#include "graphlu_internal.h"

int GraphLU_DumpA(SGraphLU *graphlu, real__t **ax, uint__t **ai, uint__t **ap)
{
	uint__t n, nnz;
	real__t *ax0;
	uint__t *ai0, *ap0, *rowperm, *pinv, oldrow, start, end;
	uint__t i, j, p;

	if (NULL == graphlu || NULL == ax || NULL == ai || NULL == ap)
	{
		return GRAPHLU_ARGUMENT_ERROR;
	}
	if (!graphlu->flag[2])
	{
		return GRAPHLU_MATRIX_NOT_FACTORIZED;
	}

	if (*ax != NULL)
	{
		free(*ax);
		*ax = NULL;
	}
	if (*ai != NULL)
	{
		free(*ai);
		*ai = NULL;
	}
	if (*ap != NULL)
	{
		free(*ap);
		*ap = NULL;
	}

	n = graphlu->n;
	nnz = graphlu->nnz;
	ax0 = graphlu->ax;
	ai0 = graphlu->ai;
	ap0 = graphlu->ap;
	rowperm = graphlu->row_perm;/*row_perm[i]=j-->row i in the permuted matrix is row j in the original matrix*/
	pinv = (uint__t *)graphlu->pivot_inv;/*pivot_inv[i]=j-->column i is the jth pivot column*/

	*ax = (real__t *)malloc(sizeof(real__t)*nnz);
	*ai = (uint__t *)malloc(sizeof(uint__t)*nnz);
	*ap = (uint__t *)malloc(sizeof(uint__t)*(n+1));
	if (NULL == *ax || NULL == *ai || NULL == *ap)
	{
		goto FAIL;
	}
	(*ap)[0] = 0;

	p = 0;
	for (i=0; i<n; ++i)
	{
		oldrow = rowperm[i];
		start = ap0[oldrow];
		end = ap0[oldrow+1];
		(*ap)[i+1] = (*ap)[i] + end - start;

		for (j=start; j<end; ++j)
		{
			(*ax)[p] = ax0[j];
			(*ai)[p++] = pinv[ai0[j]];
		}
	}

	return GRAPH_OK;

FAIL:
	if (*ax != NULL)
	{
		free(*ax);
		*ax = NULL;
	}
	if (*ai != NULL)
	{
		free(*ai);
		*ai = NULL;
	}
	if (*ap != NULL)
	{
		free(*ap);
		*ap = NULL;
	}
	return GRAPHLU_MEMORY_OVERFLOW;
}
