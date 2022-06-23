#include "../../tr_local.h"

/*
================
RB_SetMVP
================
*/
void RB_SetMVP(const idRenderMatrix& mvp) {
	tr.mvpMatrixX->SetVectorValue(mvp[0]);
	tr.mvpMatrixY->SetVectorValue(mvp[1]);
	tr.mvpMatrixZ->SetVectorValue(mvp[2]);
	tr.mvpMatrixW->SetVectorValue(mvp[3]);
}

/*
================
RB_SetModelMatrix
================
*/
void RB_SetModelMatrix(const idRenderMatrix& modelMatrix) {
	tr.modelMatrixX->SetVectorValue(modelMatrix[0]);
	tr.modelMatrixY->SetVectorValue(modelMatrix[1]);
	tr.modelMatrixZ->SetVectorValue(modelMatrix[2]);
	tr.modelMatrixW->SetVectorValue(modelMatrix[3]);
}

/*
================
RB_SetModelMatrix
================
*/
void RB_SetModelMatrix(const float* modelMatrix) {
	idRenderMatrix m;
	idRenderMatrix transposed;
	memcpy(m.GetFloatPtr(), modelMatrix, sizeof(float) * 16);

	idRenderMatrix::Transpose(m, transposed);
	RB_SetModelMatrix(transposed);
}