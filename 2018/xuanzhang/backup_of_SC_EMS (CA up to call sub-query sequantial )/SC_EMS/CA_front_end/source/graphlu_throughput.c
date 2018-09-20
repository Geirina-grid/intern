/*calculate the memory throughput*/
/*last modified: june 7, 2013*/
/*author: Chen, Xiaoming*/

#include "graphlu.h"
#include "graphlu_internal.h"

extern size_t g_si, g_sd, g_sp;

int GraphLU_Throughput(SGraphLU *graphlu, real__t *thr)
{
	uint__t n, i, j;
	real__t total;
	uint__t *ulen, *llen, ul, ll, *rp, oldrow, *ap;
	size_t *up;
	uint__t *lip;
	void *lu;

	if (NULL == graphlu)
	{
		return GRAPHLU_ARGUMENT_ERROR;
	}
	if (thr != NULL) *thr = 0.;

	if (!graphlu->flag[2])
	{
		return GRAPHLU_MATRIX_NOT_FACTORIZED;
	}

	n = graphlu->n;
	total = 0.;
	ulen = graphlu->ulen;
	llen = graphlu->llen;
	up = graphlu->up;
	lu = graphlu->lu_array;
	rp = graphlu->row_perm;
	ap = graphlu->ap;

	for (i=0; i<n; ++i)
	{
		oldrow = rp[i];
		total += (ap[oldrow+1]-ap[oldrow]) * (g_si+2*g_sd);

		ll = llen[i];
		ul = ulen[i];

		lip = (uint__t *)(((byte__t *)lu) + up[i] + g_sp*ul);

		for (j=0; j<ll; ++j)
		{
			total += (g_si+3*g_sd) * ulen[lip[j]];
		}

		total += (ul+ll) * (g_si+3*g_sd);
	}

	graphlu->stat[12] = total;
	if (thr != NULL) *thr = total;

	return GRAPH_OK;
}
