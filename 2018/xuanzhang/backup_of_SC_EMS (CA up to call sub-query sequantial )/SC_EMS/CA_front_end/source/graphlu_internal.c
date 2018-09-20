/*internal functions*/
/*last modified: june 11, 2013*/
/*author: Chen, Xiaoming*/

#include "graphlu.h"
#include "graphlu_internal.h"

extern size_t g_sd, g_si, g_sp, g_sis1, g_sps1;

int _I_GraphLU_AllocMatrixMemory(uint__t n, uint__t nnz, SGraphLU *graphlu)
{
	real__t *ax;
	uint__t *ai;
	uint__t *ap;
	real__t *rhs;

	ax = (real__t *)malloc(sizeof(real__t)*nnz);
	ai = (uint__t *)malloc(sizeof(int__t)*nnz);
	ap = (uint__t *)malloc(sizeof(int__t)*(1+n));
	rhs = (real__t *)malloc(sizeof(real__t)*n);

	if (NULL == ax || NULL == ai || NULL == ap || NULL == rhs)
	{
		if (ax != NULL) free(ax);
		if (ai != NULL) free(ai);
		if (ap != NULL) free(ap);
		if (rhs != NULL) free(rhs);
		return GRAPHLU_MEMORY_OVERFLOW;
	}

	graphlu->ax = ax;
	graphlu->ai = ai;
	graphlu->ap = ap;
	graphlu->rhs = rhs;

	return GRAPH_OK;
}

int _I_GraphLU_ConstructCSR(SGraphLU *graphlu, int__t *match, uint__t *ai, uint__t *ap)
{
	uint__t row;
	uint__t end;
	uint__t i, j;
	uint__t ct;
	uint__t n;
	uint__t *ai0, *ap0;

	n = graphlu->n;
	ai0 = graphlu->ai;
	ap0 = graphlu->ap;
	ct = 0;
	ap[0] = 0;
	
	for (i=0; i<n; ++i)
	{
		row = match[i];
		end = ap0[1+row];
		for (j=ap0[row]; j<end; ++j)
		{
			ai[ct++] = ai0[j];
		}
		ap[i+1] = ct;
	}

	return GRAPH_OK;
}

int _I_GraphLU_Permute(SGraphLU *graphlu, int__t *match, int__t *p, int__t *pinv)
{
	uint__t row, i, n, nnz;
	uint__t *ai;
	uint__t *rp, *rpi;

	n = graphlu->n;
	nnz = graphlu->nnz;
	ai = graphlu->ai;
	rp = graphlu->row_perm;
	rpi = graphlu->row_perm_inv;

	for (i=0; i<n; ++i)
	{
		row = match[p[i]];

		rpi[row] = i;
		rp[i] = row;
	}

	for (i=0; i<nnz; ++i)
	{
		ai[i] = pinv[ai[i]];
	}

	memcpy(graphlu->col_perm, p, sizeof(int__t)*n);
	memcpy(graphlu->col_perm_inv, pinv, sizeof(int__t)*n);

	return GRAPH_OK;
}

void _I_GraphLU_CopyData(SGraphLU *graphlu, unsigned int id)
{
	uint__t i, ul, ll;
	size_t tl;
	uint__t *ulen, *llen;
	size_t *up;
	void *lu, **lu2;
	uint__t start, end;

	ulen = graphlu->ulen;
	llen = graphlu->llen;
	up = graphlu->up;
	lu = graphlu->lu_array;
	lu2 = graphlu->lu_array2;

	start = graphlu->cluster_start[id];
	end = graphlu->cluster_end[id];

	for (i=start; i<end; ++i)
	{
		ul = ulen[i];
		ll = llen[i];
		
		tl = (ul+ll) * g_sp;
		memcpy(((byte__t *)lu)+up[i], ((uint__t *)lu2[i])+ul, tl);
	}

	graphlu->thread_finish[id] = TRUE;
}
