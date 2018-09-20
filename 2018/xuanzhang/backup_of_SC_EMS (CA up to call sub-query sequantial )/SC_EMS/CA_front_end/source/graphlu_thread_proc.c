/*thread process function*/
/*last modified: june 7, 2013*/
/*author: Chen, Xiaoming*/

#include "graphlu.h"
#include "graphlu_internal.h"
#include "thread.h"

THREAD_DECL _I_GraphLU_ThreadProc(void *tharg)
{
	SGraphLUThreadArg *arg;
	SGraphLU *graphlu;
	unsigned int id;
	bool__t *ac;

	arg = (SGraphLUThreadArg *)tharg;
	graphlu = arg->graphlu;
	id = arg->id;
	ac = &(graphlu->thread_active[id]);

	while (TRUE)
	{
		_SpinWaitChar((volatile char *)ac);
		*ac = FALSE;

		switch (graphlu->thread_work)
		{
		case GRAPHLU_WORK_EXIT:
			goto RETURN;
			break;

		case GRAPHLU_WORK_FACT_CLUSTER:
			_I_GraphLU_Factorize_Cluster(graphlu, arg, id);
			break;

		case GRAPHLU_WORK_FACT_PIPELINE:
			_I_GraphLU_Factorize_Pipeline(graphlu, arg, id);
			break;

		case GRAPHLU_WORK_REFACT_CLUSTER:
			_I_GraphLU_ReFactorize_Cluster(graphlu, arg, id);
			break;

		case GRAPHLU_WORK_REFACT_PIPELINE:
			_I_GraphLU_ReFactorize_Pipeline(graphlu, arg, id);
			break;

	/*	case GRAPHLU_WORK_COPY_DATA:
			_I_GraphLU_CopyData(graphlu, id);
			break;*/

		case GRAPHLU_WORK_NONE:
		default:
			break;
		}
	}

RETURN:
	return THREAD_RETURN;
}
