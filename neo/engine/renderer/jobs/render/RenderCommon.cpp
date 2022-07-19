#include "../../RenderSystem_local.h"

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
RB_SetProjectionMatrix
================
*/
void RB_SetProjectionMatrix(const idRenderMatrix& modelMatrix) {
	tr.projectionMatrixX->SetVectorValue(modelMatrix[0]);
	tr.projectionMatrixY->SetVectorValue(modelMatrix[1]);
	tr.projectionMatrixZ->SetVectorValue(modelMatrix[2]);
	tr.projectionMatrixW->SetVectorValue(modelMatrix[3]);
}

/*
================
RB_SetViewMatrix
================
*/
void RB_SetViewMatrix(const idRenderMatrix& modelMatrix) {
	tr.viewMatrixX->SetVectorValue(modelMatrix[0]);
	tr.viewMatrixY->SetVectorValue(modelMatrix[1]);
	tr.viewMatrixZ->SetVectorValue(modelMatrix[2]);
	tr.viewMatrixW->SetVectorValue(modelMatrix[3]);
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

/*
================
RB_SetProjectionMatrix
================
*/
void RB_SetProjectionMatrix(const float* projectionMatrix) {
	idRenderMatrix m;
	idRenderMatrix transposed;
	memcpy(m.GetFloatPtr(), projectionMatrix, sizeof(float) * 16);

	idRenderMatrix::Transpose(m, transposed);
	RB_SetProjectionMatrix(transposed);
}

/*
================
RB_SetViewMatrix
================
*/
void RB_SetViewMatrix(const float* projectionMatrix) {
	idRenderMatrix m;
	idRenderMatrix transposed;
	memcpy(m.GetFloatPtr(), projectionMatrix, sizeof(float) * 16);

	idRenderMatrix::Transpose(m, transposed);
	RB_SetViewMatrix(transposed);
}


/*
======================
RB_BindJointBuffer
======================
*/
void RB_BindJointBuffer(idJointBuffer* jointBuffer, float* inverseJointPose, int numJoints, void* colorOffset, void* color2Offset) {
	glEnableVertexAttribArrayARB(PC_ATTRIB_INDEX_COLOR);
	glEnableVertexAttribArrayARB(PC_ATTRIB_INDEX_COLOR2);

	glVertexAttribPointerARB(PC_ATTRIB_INDEX_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(idDrawVert), colorOffset);
	glVertexAttribPointerARB(PC_ATTRIB_INDEX_COLOR2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(idDrawVert), color2Offset);

	jointBuffer->Update(inverseJointPose, numJoints);

	const GLuint ubo = reinterpret_cast<GLuint>(jointBuffer->GetAPIObject());
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, jointBuffer->GetOffset(), jointBuffer->GetNumJoints() * sizeof(idJointMat));
}

/*
======================
RB_UnBindJointBuffer
======================
*/
void RB_UnBindJointBuffer(void) {
	glDisableVertexAttribArrayARB(PC_ATTRIB_INDEX_COLOR);
	glDisableVertexAttribArrayARB(PC_ATTRIB_INDEX_COLOR2);
}