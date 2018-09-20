/*create and destroy threads*/
/*last modified: june 7, 2013*/
/*author: Chen, Xiaoming*/

#include "graphlu.h"
#include "graphlu_internal.h"
#include "thread.h"
#include "system.h"

int GraphLU_CreateThreads(SGraphLU *graphlu, unsigned int threads, bool__t check)
{
	uint__t n;
	void *ptr;
	size_t size;
	SGraphLUThreadArg *tharg;
	unsigned int i, cores;

	if (NULL == graphlu)
	{
		return GRAPHLU_ARGUMENT_ERROR;
	}
	if (!graphlu->flag[0])
	{
		return GRAPHLU_MATRIX_NOT_INITIALIZED;
	}

	n = graphlu->n;

	/*check threads*/
	if (check)
	{
		cores = (unsigned int)(graphlu->cfgi[8]);
		if (threads > cores)
		{
			threads = cores;
		}
	}
	if (threads == 0)
	{
		return GRAPHLU_ARGUMENT_ERROR;
	}
	if (threads == 1)
	{
		return GRAPHLU_SINGLE_THREAD;
	}

	/*exit existing threads first*/
	GraphLU_DestroyThreads(graphlu);
	graphlu->cfgi[5] = threads;
	graphlu->cfgi[7] = threads;

	/*thread_id*/
	size = sizeof(thread_id__t) * threads;
	ptr = malloc(size);
	if (NULL == ptr)
	{
		return GRAPHLU_MEMORY_OVERFLOW;
	}
	memset(ptr, 0, size);
	graphlu->thread_id = ptr;

	/*thread arg*/
	size = sizeof(SGraphLUThreadArg) * threads;
	ptr = malloc(size);
	if (NULL == ptr)
	{
		return GRAPHLU_MEMORY_OVERFLOW;
	}
	graphlu->thread_arg = ptr;
	for (i=0; i<threads; ++i)
	{
		tharg = &(((SGraphLUThreadArg *)ptr)[i]);
		tharg->graphlu = graphlu;
		tharg->id = i;
	}

	/*thread work*/
	graphlu->thread_work = GRAPHLU_WORK_NONE;

	/*thread active & thread finish*/
	size = sizeof(bool__t) * (threads+threads);
	ptr = malloc(size);
	if (NULL == ptr)
	{
		return GRAPHLU_MEMORY_OVERFLOW;
	}
	memset(ptr, 0, size);
	graphlu->thread_finish = (bool__t *)ptr;
	ptr = ((bool__t *)ptr) + threads;
	graphlu->thread_active = (bool__t *)ptr;

	/*cluster start and end*/
	size = sizeof(uint__t) * (threads+threads);
	ptr = malloc(size);
	if (NULL == ptr)
	{
		return GRAPHLU_MEMORY_OVERFLOW;
	}
	graphlu->cluster_start = (uint__t *)ptr;
	ptr = ((uint__t *)ptr) + threads;
	graphlu->cluster_end = (uint__t *)ptr;

	/*work space*/
	size = sizeof(void *) * threads;
	ptr = malloc(size);
	if (NULL == ptr)
	{
		return GRAPHLU_MEMORY_OVERFLOW;
	}
	memset(ptr, 0, size);
	graphlu->workspace_mt1 = (void **)ptr;

	ptr = malloc(size);
	if (NULL == ptr)
	{
		return GRAPHLU_MEMORY_OVERFLOW;
	}
	memset(ptr, 0, size);
	graphlu->workspace_mt2 = (void **)ptr;

	size = (sizeof(int__t)*4+sizeof(real__t)) * n;
	ptr = malloc(size*threads);
	if (NULL == ptr)
	{
		return GRAPHLU_MEMORY_OVERFLOW;
	}
	for (i=0; i<threads; ++i)
	{
		graphlu->workspace_mt1[i] = ptr;
		ptr = ((byte__t *)ptr) + size;
	}

	size = (sizeof(int__t)*3) * n;
	ptr = malloc(size*threads);
	if (NULL == ptr)
	{
		return GRAPHLU_MEMORY_OVERFLOW;
	}
	for (i=0; i<threads; ++i)
	{
		graphlu->workspace_mt2[i] = ptr;
		ptr = ((byte__t *)ptr) + size;
	}

	/*create threads*/
	for (i=1; i<threads; ++i)
	{
		if (_CreateThread( \
			_I_GraphLU_ThreadProc, \
			&(((SGraphLUThreadArg *)graphlu->thread_arg)[i]), \
			&(((thread_id__t *)graphlu->thread_id)[i]) \
			) != 0)
		{
			return GRAPHLU_THREADS_INIT_FAIL;
		}
	}
	((thread_id__t *)graphlu->thread_id)[0] = _GetCurrentThread();

	/*finish*/
	graphlu->flag[3] = TRUE;

	return GRAPH_OK;
}

int GraphLU_BindThreads(SGraphLU *graphlu, bool__t reset)
{
	int err;
	thread_id__t *id;
	unsigned int i, threads;

	if (NULL == graphlu)
	{
		return GRAPHLU_ARGUMENT_ERROR;
	}
	if (!graphlu->flag[3])
	{
		return GRAPHLU_THREADS_NOT_INITIALIZED;
	}

	err = GRAPH_OK;
	id = (thread_id__t *)(graphlu->thread_id);
	threads = (unsigned int)(graphlu->cfgi[5]);

	if (!reset)/*set*/
	{
		unsigned int core;
		for (i=0; i<threads; ++i)
		{
			core = i;
			err = _BindThreadToCores(id[i], &core, 1);
		}

		return (err==0 ? GRAPH_OK : GRAPHLU_BIND_THREADS_FAIL);
	}
	else/*reset*/
	{
		for (i=0; i<threads; ++i)
		{
			err = _UnbindThreadFromCores(id[i]);
		}

		return (err==0 ? GRAPH_OK : GRAPHLU_BIND_THREADS_FAIL);
	}
}

int GraphLU_DestroyThreads(SGraphLU *graphlu)
{
	bool__t mt;
	thread_id__t *thid;
	bool__t *thac;
	unsigned int i, th;

	if (NULL == graphlu)
	{
		return GRAPHLU_ARGUMENT_ERROR;
	}

	th = (unsigned int)(graphlu->cfgi[5]);
	mt = graphlu->flag[3];
	thid = (thread_id__t *)(graphlu->thread_id);
	thac = graphlu->thread_active;

	if (mt && th > 1)
	{
		/*set all threads active and wait to exit*/
		graphlu->thread_work = GRAPHLU_WORK_EXIT;
		for (i=1; i<th; ++i)
		{
			thac[i] = TRUE;
		}
		for (i=1; i<th; ++i)
		{
			_WaitThreadExit(thid[i]);
		}
	}
	graphlu->flag[3] = FALSE;
	graphlu->cfgi[5] = 1;
	graphlu->cfgi[7] = 1;
	graphlu->thread_work = GRAPHLU_WORK_NONE;

	if (graphlu->thread_id != NULL)
	{
		free(graphlu->thread_id);
		graphlu->thread_id = NULL;
	}
	if (graphlu->thread_arg != NULL)
	{
		free(graphlu->thread_arg);
		graphlu->thread_arg = NULL;
	}
	if (graphlu->thread_finish != NULL)
	{
		free(graphlu->thread_finish);
		graphlu->thread_finish = NULL;
	}
	if (graphlu->cluster_start != NULL)
	{
		free(graphlu->cluster_start);
		graphlu->cluster_start = NULL;
	}
	graphlu->cluster_end = NULL;
	if (graphlu->workspace_mt1 != NULL)
	{
		if (graphlu->workspace_mt1[0] != NULL)
		{
			free(graphlu->workspace_mt1[0]);
		}
		free(graphlu->workspace_mt1);
		graphlu->workspace_mt1 = NULL;
	}
	if (graphlu->workspace_mt2 != NULL)
	{
		if (graphlu->workspace_mt2[0] != NULL)
		{
			free(graphlu->workspace_mt2[0]);
		}
		free(graphlu->workspace_mt2);
		graphlu->workspace_mt2 = NULL;
	}
	graphlu->thread_active = NULL;

	return GRAPH_OK;
}
