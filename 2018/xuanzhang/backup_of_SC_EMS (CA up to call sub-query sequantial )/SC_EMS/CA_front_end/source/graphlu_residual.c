/*calculate the residual error, i.e. error = |Ax-b|*/
/*last modified: june 7, 2013*/
/*author: Chen, Xiaoming*/

#include "graphlu.h"
#include "graphlu_internal.h"
#include "math.h"

int GraphLU_Residual(uint__t n, real__t *ax, uint__t *ai, uint__t *ap, real__t *x, real__t *b, \
					real__t *err, int norm, int mode)
{
	uint__t i, j, end;
	real__t sum, t, n1, n2, ni;

	if (0 == n || NULL == ax || NULL == ai || NULL == ap || NULL == x || NULL == b || NULL == err)
	{
		return GRAPHLU_ARGUMENT_ERROR;
	}

	*err = -1.;
	n1 = 0.;
	n2 = 0.;
	ni = 0.;

	if (mode == 0)
	{
		for (i=0; i<n; ++i)
		{
			sum = 0.;
			end = ap[i+1];
			for (j=ap[i]; j<end; ++j)
			{
				sum += ax[j] * x[ai[j]];
			}
			t = sum - b[i];
			if (t < 0.) t = -t;
			n1 += t;
			n2 += t*t;
			if (t > ni) ni = t;
		}
	}
	else
	{
		real__t *r = (real__t *)malloc(sizeof(real__t)*n);
		if (r == NULL) return GRAPHLU_MEMORY_OVERFLOW;
		memset(r, 0, sizeof(real__t)*n);

		for (i=0; i<n; ++i)
		{
			t = x[i];
			end = ap[i+1];
			for (j=ap[i]; j<end; ++j)
			{
				r[ai[j]] += ax[j] * t;
			}
		}

		for (i=0; i<n; ++i)
		{
			t = r[i] - b[i];
			if (t < 0.) t = -t;
			n1 += t;
			n2 += t*t;
			if (t > ni) ni = t;
		}

		free(r);
	}

	if (norm == 1)
	{
		*err = n1;
	}
	else if (norm == 2)
	{
		*err = sqrt(n2);
	}
	else
	{
		*err = ni;
	}

	return GRAPH_OK;
}

/*Ax-b, vector*/
void _I_GraphLU_Residual(SGraphLU *graphlu, real__t *sol, real__t *b, real__t *rerror)
{
	uint__t i, j;
	uint__t *cp, *cpi;
	uint__t *ai, *ap;
	real__t *ax;
	uint__t n, end;
	real__t sum;
	real__t *rs, *cs;
	uint__t col;
	real__t t;
	uint__t mc64_scale;
	real__t rrs;
	real__t xi;

	mc64_scale = graphlu->cfgi[1];
	n = graphlu->n;

	memset(rerror, 0, sizeof(real__t)*n);

	if (graphlu->cfgi[0] == 0)
	{
		cp = graphlu->col_perm;
		ax = graphlu->ax;
		ai = graphlu->ai;
		ap = graphlu->ap;	

		if (mc64_scale)
		{
			rs = graphlu->row_scale;
			cs = graphlu->col_scale_perm;

			for (i=0; i<n; ++i)
			{
				sum = 0.;
				end = ap[i+1];
				for (j=ap[i]; j<end; ++j)
				{
					col = ai[j];
					sum += ax[j] * sol[cp[col]] / cs[col];
				}
				t = sum/rs[i] - b[i];
				rerror[i] = t;
			}
		}
		else
		{
			for (i=0; i<n; ++i)
			{
				sum = 0.;
				end = ap[i+1];
				for (j=ap[i]; j<end; ++j)
				{
					col = ai[j];
					sum += ax[j] * sol[cp[col]];
				}
				t = sum - b[i];
				rerror[i] = t;
			}
		}
	}
	else/*CSC*/
	{
		cp = graphlu->col_perm;
		cpi = graphlu->col_perm_inv;
		ax = graphlu->ax;
		ai = graphlu->ai;
		ap = graphlu->ap;

		memset(rerror, 0, sizeof(real__t)*n);

		if (mc64_scale)
		{
			rs = graphlu->row_scale;
			cs = graphlu->col_scale_perm;

			for (i=0; i<n; ++i)
			{
				rrs = rs[i];
				xi = sol[i];
				xi /= rrs;
				end = ap[i+1];
				for (j=ap[i]; j<end; ++j)
				{
					col = ai[j];
					rerror[cp[col]] += ax[j] * xi;
				}
			}

			for (i=0; i<n; ++i)
			{
				t = rerror[i]/cs[cpi[i]] - b[i];
				rerror[i] = t;
			}
		}
		else
		{
			for (i=0; i<n; ++i)
			{
				xi = sol[i];
				end = ap[i+1];
				for (j=ap[i]; j<end; ++j)
				{
					col = ai[j];
					rerror[cp[col]] += ax[j] * xi;
				}
			}

			for (i=0; i<n; ++i)
			{
				t = rerror[i] - b[i];
				rerror[i] = t;
			}
		}
	}
}
