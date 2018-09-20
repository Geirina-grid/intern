/*scale the matrix*/
/*last modified: june 7, 2013*/
/*author: Chen, Xiaoming*/

#include "graphlu.h"
#include "graphlu_internal.h"

int _I_GraphLU_Scale(SGraphLU *graphlu)
{
	uint__t n, i, j, col;
	uint__t scale;
	real__t *cscale;
	real__t val;
	real__t *ax;
	uint__t *ai, *ap;
	uint__t cend, oldrow;
	uint__t *rowperm;

	scale = graphlu->cfgi[2];
	if (scale == 0 || scale > 2) return GRAPH_OK;

	n = graphlu->n;
	cscale = graphlu->cscale;
	
	ax = graphlu->ax;
	ai = graphlu->ai;
	ap = graphlu->ap;
	rowperm = graphlu->row_perm;

	memset(cscale, 0, sizeof(real__t)*n);

	if (scale == 1)/*maximum*/
	{
		for (i=0; i<n; ++i)
		{
			oldrow = rowperm[i];
			cend = ap[oldrow+1];
			for (j=ap[oldrow]; j<cend; ++j)
			{
				col = ai[j];
				val = ax[j];
				if (val < 0.) val = -val;
				if (val > cscale[col])
				{
					cscale[col] = val;
				}
			}
		}
	}
	else if (scale == 2)/*sum*/
	{
		for (i=0; i<n; ++i)
		{
			oldrow = rowperm[i];
			cend = ap[oldrow+1];
			for (j=ap[oldrow]; j<cend; ++j)
			{
				col = ai[j];
				val = ax[j];
				if (val < 0.) val = -val;
				cscale[col] += val;
			}
		}
	}

	for (i=0; i<n; ++i)
	{
		if (cscale[i] == 0.) return GRAPHLU_MATRIX_NUMERIC_SINGULAR;
	}

	return GRAPH_OK;
}
