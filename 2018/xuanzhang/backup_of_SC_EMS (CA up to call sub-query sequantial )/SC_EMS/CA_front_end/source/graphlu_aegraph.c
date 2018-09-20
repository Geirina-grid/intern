/*init the aegraph*/
/*last modified: june 7, 2013*/
/*author: Chen, Xiaoming*/

#include "graphlu.h"
#include "graphlu_internal.h"
#include "math.h"

extern size_t g_sp, g_sd, g_si;

/*only for parallel refact*/
void _I_GraphLU_CreateAEGraphForRefact(SGraphLU *graphlu)
{
	uint__t n, i, j;
	uint__t *llen, *ulen;
	void *lu;
	uint__t *lip;
	size_t *up;
	uint__t ul, ll, col;
	uint__t *data, *header;
	uint__t *inlevel;
	uint__t *lv_len;
	uint__t *tlen;
	int__t max, lv;
	int__t level;

	n = graphlu->n;
	llen = graphlu->llen;
	ulen = graphlu->ulen;
	lu = graphlu->lu_array;
	up = graphlu->up;
	data = graphlu->aeg_refact_data;
	header = graphlu->aeg_refact_header;


	/*|----------|----------|----------|*/
	/*inlevel    lv_len     tlen*/
	inlevel = (uint__t *)graphlu->workspace;
	lv_len = inlevel + n;
	memset(inlevel, 0, sizeof(uint__t)*n*2);
	tlen = (uint__t *)(lv_len + n);

	/*begin*/
	level = 0;
	for (i=0; i<n; ++i)
	{
		ll = llen[i];
		ul = ulen[i];
		lip = (uint__t *)(((byte__t *)lu) + up[i] + g_sp*ul);

		max = -1;
		for (j=0; j<ll; ++j)
		{
			col = lip[j];
			ul = ulen[col];
			if (ul > 0)/*col--->i*/
			{
				lv = inlevel[col];
				if (lv > max)
				{
					max = lv;
				}
			}
		}
		lv = max + 1;
		inlevel[i] = lv;
		++lv_len[lv];
		if (lv > level)
		{
			level = lv;
		}
	}
	++level;

	header[0] = 0;
	for (i=0; i<(uint__t)level; ++i)
	{
		header[i+1] = header[i] + lv_len[i];
	}
	memcpy(tlen, header, sizeof(uint__t)*level);

	for (i=0; i<n; ++i)
	{
		data[tlen[inlevel[i]]++] = i;
	}

	graphlu->aeg_refact_level = level;

#ifdef GRAPHLU_DEBUG
	{
		FILE *fp = fopen("aegraph.txt", "w");
		for (i=0; i<level; ++i)
		{
			fprintf(fp, "%d\t%d\n", i, header[i+1]-header[i]);
		}
		fclose(fp);
	}
#endif

	graphlu->flag[5] = TRUE;
}
