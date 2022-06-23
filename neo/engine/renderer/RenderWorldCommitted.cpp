// RenderWorldCommitted.cpp
//

#include "RenderSystem_local.h"

/*
===================
R_PointInFrustum

Assumes positive sides face outward
===================
*/
static bool R_PointInFrustum(idVec3& p, idPlane* planes, int numPlanes) {
	for (int i = 0; i < numPlanes; i++) {
		float d = planes[i].Distance(p);
		if (d > 0) {
			return false;
		}
	}
	return true;
}


/*
=========================
idRenderWorldCommitted::CommitRenderLight
=========================
*/
idRenderLightCommitted* idRenderWorldCommitted::CommitRenderLight(idRenderLightLocal* light) {
	idRenderLightCommitted* vLight;

	if (light->viewCount == tr.viewCount) {
		return light->viewLight;
	}
	light->viewCount = tr.viewCount;

	// add to the view light chain
	vLight = (idRenderLightCommitted*)R_ClearedFrameAlloc(sizeof(*vLight));
	vLight->lightDef = light;

	// the scissorRect will be expanded as the light bounds is accepted into visible portal chains
	vLight->scissorRect.Clear();

	// calculate the shadow cap optimization states
	vLight->viewInsideLight = vLight->TestPointInViewLight(tr.viewDef->renderView.vieworg, light);
	if (!vLight->viewInsideLight) {
		vLight->viewSeesShadowPlaneBits = 0;
		for (int i = 0; i < light->numShadowFrustums; i++) {
			float d = light->shadowFrustums[i].planes[5].Distance(tr.viewDef->renderView.vieworg);
			if (d < INSIDE_LIGHT_FRUSTUM_SLOP) {
				vLight->viewSeesShadowPlaneBits |= 1 << i;
			}
		}
	}
	else {
		// this should not be referenced in this case
		vLight->viewSeesShadowPlaneBits = 63;
	}

	// see if the light center is in view, which will allow us to cull invisible shadows
	vLight->viewSeesGlobalLightOrigin = R_PointInFrustum(light->globalLightOrigin, tr.viewDef->frustum, 4);

	// copy data used by backend
	vLight->globalLightOrigin = light->globalLightOrigin;
	vLight->lightProject[0] = light->lightProject[0];
	vLight->lightProject[1] = light->lightProject[1];
	vLight->lightProject[2] = light->lightProject[2];
	vLight->lightProject[3] = light->lightProject[3];
	vLight->fogPlane = light->frustum[5];
	vLight->frustumTris = light->frustumTris;
	vLight->falloffImage = light->falloffImage;
	vLight->lightShader = light->lightShader;
	vLight->shaderRegisters = NULL;		// allocated and evaluated in R_AddLightSurfaces

	// link the view light
	vLight->next = viewLights;
	viewLights = vLight;

	light->viewLight = vLight;

	return vLight;
}


/*
=============
idRenderWorldCommitted::CommitRenderModel

If the entityDef isn't already on the viewEntity list, create
a viewEntity and add it to the list with an empty scissor rect.

This does not instantiate dynamic models for the entity yet.
=============
*/
idRenderModelCommitted* idRenderWorldCommitted::CommitRenderModel(idRenderEntityLocal* def) {
	idRenderModelCommitted* vModel;

	if (def->viewCount == tr.viewCount) {
		return def->viewEntity;
	}
	def->viewCount = tr.viewCount;

	// set the model and modelview matricies
	vModel = (idRenderModelCommitted*)R_ClearedFrameAlloc(sizeof(*vModel));
	vModel->entityDef = def;

	// the scissorRect will be expanded as the model bounds is accepted into visible portal chains
	vModel->scissorRect.Clear();

	// copy the model and weapon depth hack for back-end use
	vModel->modelDepthHack = def->parms.modelDepthHack;
	vModel->weaponDepthHack = def->parms.weaponDepthHack;

	R_AxisToModelMatrix(def->parms.axis, def->parms.origin, vModel->modelMatrix);

	// we may not have a viewDef if we are just creating shadows at entity creation time
	if (tr.viewDef) {
		myGlMultMatrix(vModel->modelMatrix, tr.viewDef->worldSpace.modelViewMatrix, vModel->modelViewMatrix);

		vModel->next = tr.viewDef->viewEntitys;
		tr.viewDef->viewEntitys = vModel;
	}

	def->viewEntity = vModel;

	return vModel;
}