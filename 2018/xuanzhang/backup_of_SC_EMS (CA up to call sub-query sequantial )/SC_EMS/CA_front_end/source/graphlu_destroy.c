/*destroy the GraphLU structure*/
/*last modified: june 7, 2013*/
/*author: Chen, Xiaoming*/

#include "graphlu.h"
#include "graphlu_internal.h"

int GraphLU_Destroy(SGraphLU *graphlu)
{
	if (NULL == graphlu)
	{
		return GRAPHLU_ARGUMENT_ERROR;
	}

	_I_GraphLU_DestroyMatrix(graphlu);

	free(graphlu->timer);
	graphlu->timer = NULL;

	free(graphlu->flag);
	graphlu->flag = NULL;

	free(graphlu->stat);
	graphlu->stat = NULL;

	free(graphlu->cfgi);
	graphlu->cfgi = NULL;

	free(graphlu->cfgf);
	graphlu->cfgf = NULL;

	return GRAPH_OK;
}
