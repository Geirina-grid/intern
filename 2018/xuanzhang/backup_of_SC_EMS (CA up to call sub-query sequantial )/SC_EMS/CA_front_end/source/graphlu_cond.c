/*calculate the condition number*/
/*this function is modified from the corresponding function in KLU package*/
/* http://www.cise.ufl.edu/research/sparse/klu/ */

#include "graphlu.h"
#include "graphlu_internal.h"
#include "math.h"

int GraphLU_ConditionNumber(SGraphLU *graphlu, real__t *cond)
{
	uint__t n, nnz, i, j;
	real__t *ax;
	uint__t *ai;
	real__t *ldiag;
	real__t *csum;
	real__t anorm, aivnorm, est_old, est_new;
	uint__t jmax, jnew;
	real__t *x, *s, t, xmax;
	bool__t unch;

	if (NULL == graphlu)
	{
		return GRAPHLU_ARGUMENT_ERROR;
	}

	graphlu->stat[6] = -1.;
	if (cond != NULL) *cond = -1.;

	if (!graphlu->flag[2])
	{
		return GRAPHLU_MATRIX_NOT_FACTORIZED;
	}

	n = graphlu->n;
	nnz = graphlu->nnz;
	ldiag = graphlu->ldiag;
	ax = graphlu->ax;
	ai = graphlu->ai;

	if (n == 1)
	{
		graphlu->stat[6] = 0.;
		if (cond != NULL) *cond = 0.;
		return GRAPH_OK;
	}

	/*check the diag*/
	for (i=0; i<n; ++i)
	{
		if (ldiag[i] == 0.)
		{
			graphlu->stat[6] = DBL_MAX;
			if (cond != NULL) *cond = DBL_MAX;
			return GRAPH_OK;
		}
	}

	/*compute the 1-norm of A, maximum column sum*/
	csum = (real__t *)malloc(sizeof(real__t)*n);
	if (csum == NULL)
	{
		return GRAPHLU_MEMORY_OVERFLOW;
	}
	memset(csum, 0, sizeof(real__t)*n);

	for (i=0; i<nnz; ++i)
	{
		t = ax[i];
		if (t < 0.) t = -t;
		csum[ai[i]] += t;
	}
	anorm = -1.;
	for (i=0; i<n; ++i)
	{
		if (csum[i] > anorm) anorm = csum[i];
	}

	/*1-norm of A^(-1)*/
	x = (real__t *)graphlu->workspace;
	s = csum;
	memset(s, 0, sizeof(real__t)*n);
	t = 1. / n;
	for (i=0; i<n; ++i)
	{
		x[i] = t;
	}

	aivnorm = 0.;
	jmax = 0;

	for (i=0; i<5; ++i)
	{
		if (i > 0)
		{
			memset(x, 0, sizeof(real__t)*n);
			x[jmax] = 1.;
		}

		GraphLU_Solve(graphlu, x);
		est_old = aivnorm;
		aivnorm = 0.;

		for (j=0; j<n; ++j)
		{
			t = x[j];
			if (t < 0.) t = -t;
			aivnorm += t;
		}

		unch = TRUE;

		for (j=0; j<n; ++j)
		{
			t = (x[j] >= 0.) ? 1 : -1;
			if (t != (int__t)s[j])
			{
				s[j] = t;
				unch = FALSE;
			}
		}

		if (i > 0 && (aivnorm <= est_old || unch))
		{
			break;
		}

		memcpy(x, s, sizeof(real__t)*n);
		graphlu->cfgi[0] = !graphlu->cfgi[0];
		GraphLU_Solve(graphlu, x);
		graphlu->cfgi[0] = !graphlu->cfgi[0];

		jnew = 0;
		xmax = 0.;
		for (j=0; j<n; ++j)
		{
			t = x[j];
			if (t < 0.) t = -t;
			if (t > xmax)
			{
				xmax = t;
				jnew = j;
			}
		}
		if (i > 0 && jnew == jmax)
		{
			break;
		}
		jmax = jnew;
	}

	/*another 1-norm of A^(-1)*/

	for (j=0; j<n; ++j)
	{
		if (j % 2)
		{
			x[j] = 1. + j/(n-1.);
		}
		else
		{
			x[j] = -1. - j/(n-1.);
		}
	}

	GraphLU_Solve(graphlu, x);

	est_new = 0.;
	for (j=0; j<n; ++j)
	{
		t = x[j];
		if (t < 0.) t = -t;
		est_new += t;
	}
	est_new = 2.*est_new/3./n;
	aivnorm = MAX(est_new, aivnorm);

	graphlu->stat[6] = aivnorm*anorm;
	if (cond != NULL) *cond = graphlu->stat[6];

	free(csum);
	return GRAPH_OK;
}
