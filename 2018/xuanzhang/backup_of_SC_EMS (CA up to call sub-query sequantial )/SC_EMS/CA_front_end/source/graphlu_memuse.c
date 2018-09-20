/*last modified: june 7, 2013*/

#include "graphlu.h"
#include "graphlu_internal.h"
#include "timer_c.h"

extern size_t g_sp;

int GraphLU_MemoryUsage(SGraphLU *graphlu, real__t *mem)
{
	size__t total;
	uint__t n, nnz, i;
	unsigned int cores;
	size_t *len_est;

	total = 0;
	if (mem != NULL) *mem = 0.;

	if (NULL == graphlu)
	{
		return GRAPHLU_ARGUMENT_ERROR;
	}
	n = graphlu->n;
	nnz = graphlu->nnz;
	cores = (unsigned int)(graphlu->cfgi[5]);
	len_est = graphlu->len_est;

	/*flag*/
	total += sizeof(bool__t) * 32;
	/*stat*/
	total += sizeof(real__t) * 32;
	/*cfgi*/
	total += sizeof(uint__t) * 32;
	/*cfgf*/
	total += sizeof(real__t) * 32;
	/*timer*/
	total += sizeof(STimer);

	/*matrix*/
	if (graphlu->flag[0])
	{
		/*ax*/
		total += sizeof(real__t) * nnz;
		/*ai*/
		total += sizeof(uint__t) * nnz;
		/*ap*/
		total += sizeof(uint__t) * (1+n);
		/*rhs*/
		total += sizeof(real__t) * n;
		/*perm & p & pinv*/
		total += sizeof(uint__t) * n * 6;
		/*scale*/
		total += sizeof(real__t) * n * 3;
		/*ldiag*/
		total += sizeof(real__t) * n;
		/*up*/
		total += sizeof(size_t) * n;
		/*llen & ulen*/
		total += sizeof(uint__t) * (n+n);
		/*workspace*/
		total += (sizeof(int__t)*4+sizeof(real__t)) * n;
	}
	/*threads*/
	if (graphlu->flag[3])
	{
		/*thread id*/
		total += sizeof(thread_id__t) * cores;
		/*thread arg*/
		total += sizeof(SGraphLUThreadArg) * cores;
		/*thread active & thread finish*/
		total += sizeof(bool__t) * (cores+cores);
		/*cluster start & cluster end*/
		total += sizeof(uint__t) * (cores+cores);
		/*work space*/
		total += sizeof(void *) * (cores+cores);
		total += (sizeof(uint__t)*4+sizeof(real__t)) * n * cores;
		total += (sizeof(uint__t)*3) * n * cores;
	}
	/*scheduler*/
	if (graphlu->flag[4])
	{
		/*wkld & estlen*/
		total += sizeof(size_t) * (n+n);
		/*row state*/
		total += sizeof(byte__t) * n;
		/*scheduler*/
		total += sizeof(uint__t) * (4*n+2);
		/*lu array2*/
		total += sizeof(void *) * n;
	}
	/*lu data*/
	total += g_sp * graphlu->lu_nnz_est;
	if (graphlu->flag[4])
	{
		for (i=0; i<n; ++i)
		{
			total += len_est[i];
		}
	}

	graphlu->stat[21] = (real__t)total;
	if (mem != NULL) *mem = (real__t)total;

	return GRAPH_OK;
}
