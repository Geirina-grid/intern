/*refine the solution, used for ill-conditioned matrix*/
/*last modified: june 7, 2013*/
/*author: Chen, Xiaoming*/

#include "graphlu.h"
#include "graphlu_internal.h"
#include "math.h"
#include "timer_c.h"

int GraphLU_Refine(SGraphLU *graphlu, real__t *x, real__t *b, real__t error, uint__t maxiter)
{
	uint__t n, i, iter;
	real__t rnorm, laste;
	real__t *rr;

	if (NULL == graphlu || NULL == x || NULL == b || error <= 0.)
	{
		return GRAPHLU_ARGUMENT_ERROR;
	}
	if (!graphlu->flag[2])
	{
		return GRAPHLU_MATRIX_NOT_FACTORIZED;
	}

	TimerStart((STimer *)(graphlu->timer));

	iter = 0;
	n = graphlu->n;
	rr = (real__t *)graphlu->workspace;

	if (maxiter > 0)
	{
		while (iter < maxiter)
		{
			++iter;
			/*r = Ax-b*/
			_I_GraphLU_Residual(graphlu, x, b, rr);
			rnorm = 0.;
			for (i=0; i<n; ++i)
			{
				rnorm += ABS(rr[i]);
			}
			/*Adx=r*/
			GraphLU_Solve(graphlu, rr);
			if (rnorm > error)
			{
				/*x = x-dx*/
				for (i=0; i<n; ++i)
				{
					x[i] -= rr[i];
				}
			}
			else break;
		}
	}
	else
	{
		laste = DBL_MAX;
		while (TRUE)
		{
			++iter;
			/*r = Ax-b*/
			_I_GraphLU_Residual(graphlu, x, b, rr);
			rnorm = 0.;
			for (i=0; i<n; ++i)
			{
				rnorm += ABS(rr[i]);
			}
			/*Adx=r*/
			GraphLU_Solve(graphlu, rr);
			if (rnorm > error)
			{
				if (rnorm >= laste) break;
				laste = rnorm;
				/*x = x+dx*/
				for (i=0; i<n; ++i)
				{
					x[i] -= rr[i];
				}
			}
			else break;
		}
	}

	TimerStop((STimer *)(graphlu->timer));
	graphlu->stat[15] = TimerGetRuntime((STimer *)(graphlu->timer));
	graphlu->stat[16] = (real__t)iter;

	return GRAPH_OK;
}
