#include "graphlu.h"
#include "graphlu_internal.h"
#include "timer_c.h"
#include "math.h"

//! @brief Permute and scale the matrix for factorization using
//!        existing permutation and scaling arrays
//! @param[in] graphlu The work space
//! @return err (a non-zero value) if the matrix is singular,
//!         or 0 upon successful completion
//!
int permute_and_scale(SGraphLU *graphlu)
{
	int err;
	int i;
	uint__t *pinv;
	uint__t *ai;
	uint__t nnz;
	pinv = graphlu->col_perm_inv;
	ai = graphlu->ai;
	nnz = graphlu->nnz;

	for (i=0; i<nnz; ++i) {
		ai[i] = pinv[ai[i]];
	}

	_I_GraphLU_MC64Scale(graphlu);

	// Compute the column scale factor
	err = _I_GraphLU_Scale(graphlu);
	if (FAIL(err)) {
		return err;
	}

	return GRAPH_OK;
}
