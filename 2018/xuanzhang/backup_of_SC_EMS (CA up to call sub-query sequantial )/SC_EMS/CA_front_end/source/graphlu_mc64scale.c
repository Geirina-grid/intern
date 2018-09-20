/*scale the matrix by the mc64 algorithm*/
/*last modified: june 7, 2013*/
/*author: Chen, Xiaoming*/

#include "graphlu.h"
#include "graphlu_internal.h"

void _I_GraphLU_MC64Scale(SGraphLU *graphlu)
{
	uint__t end;
	uint__t i, j;
	uint__t n;
	real__t *rs, *csp, *ax;
	uint__t *ai, *ap;
	real__t s;
#ifdef GRAPHLU_DEBUG
	FILE *fp;
#endif

	if (!graphlu->cfgi[1]) return;

	n = graphlu->n;
	ax = graphlu->ax;
	ai = graphlu->ai;/*changed in preprocess*/
	ap = graphlu->ap;
	rs = graphlu->row_scale;
	csp = graphlu->col_scale_perm;

#ifdef GRAPHLU_DEBUG
	fp = fopen("smat0.txt", "w");
	for (i=0; i<graphlu->nnz; ++i)
	{
		fprintf(fp, "%.16g\n", ax[i]);
	}
	fclose(fp);
#endif

	for (i=0; i<n; ++i)
	{
		s = rs[i];/*not permuted*/
		end = ap[i+1];/*not permuted*/
		for (j=ap[i]; j<end; ++j)
		{
			ax[j] *= s*csp[ai[j]];
		}
	}

#ifdef GRAPHLU_DEBUG
	fp = fopen("smat1.txt", "w");
	for (i=0; i<graphlu->nnz; ++i)
	{
		fprintf(fp, "%.16g\n", ax[i]);
	}
	fclose(fp);
#endif
}

void _I_GraphLU_MC64ScaleForRefact(SGraphLU *graphlu, real__t *ax0)
{
	uint__t end;
	uint__t i, j;
	uint__t n;
	real__t *rs, *csp, *ax;
	uint__t *ai, *ap;
	real__t s;

	if (!graphlu->cfgi[1]) return;

	n = graphlu->n;
	ax = graphlu->ax;
	ai = graphlu->ai;/*changed in preprocess*/
	ap = graphlu->ap;
	rs = graphlu->row_scale;
	csp = graphlu->col_scale_perm;

	for (i=0; i<n; ++i)
	{
		s = rs[i];/*not permuted*/
		end = ap[i+1];/*not permuted*/
		for (j=ap[i]; j<end; ++j)
		{
			ax[j] = ax0[j] * s * csp[ai[j]];
		}
	}
}
