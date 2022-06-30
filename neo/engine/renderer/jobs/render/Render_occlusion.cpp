// draw_occlusion.cpp
//

#include "../../RenderSystem_local.h"

enum rvnmOcclusionQueryState
{
	OCCLUSION_QUERY_STATE_HIDDEN,
	OCCLUSION_QUERY_STATE_VISIBLE,
	OCCLUSION_QUERY_STATE_WAITING
};

class rvmOcclusionQuery {
public:
	rvmOcclusionQuery()
	{
		glGenQueries(1, &id);
		queryState = OCCLUSION_QUERY_STATE_HIDDEN;
		queryStartTime = -1;
	}

	~rvmOcclusionQuery() {
		queryStartTime = -1;
		glDeleteQueries(1, &id);
	}

	void RunQuery(idRenderLightLocal* light)
	{
		this->light = light;

		if (queryState != OCCLUSION_QUERY_STATE_WAITING)
		{
			queryState = OCCLUSION_QUERY_STATE_WAITING;

			queryStartTime = Sys_Milliseconds();
			queryTimeOutTime = queryStartTime + SEC2MS(2);

			glBeginQuery(GL_ANY_SAMPLES_PASSED, id);
			RB_DrawElementsImmediate(light->frustumTris);
			glEndQuery(GL_ANY_SAMPLES_PASSED);
		}
	}

	idRenderLightLocal* GetViewLight() { return light; }

	bool IsVisible() {
		GLuint passed = INT_MAX;

		passed = 0;
		glGetQueryObjectuiv(id, GL_QUERY_RESULT, &passed);
		if (passed) {
			return true;
		}

		return false;
	}

	int GetQueryStartTime(void) {
		return queryStartTime;
	}

	bool IsQueryStale(void) {
		int currentTime = Sys_Milliseconds();
		return queryTimeOutTime < currentTime;
	}

private:
	int queryStartTime;
	int queryTimeOutTime;
	GLuint id;
	idRenderLightLocal* light;
	rvnmOcclusionQueryState queryState;
};

bool RB_Reap_Occlusion(idRenderLightCommitted* vLight) {
	bool isVisible = false;

	if (vLight->lightDef->currentOcclusionQuery) {
		// Only reap the occlusion query if it isn't stale.
		if (!vLight->lightDef->currentOcclusionQuery->IsQueryStale())
		{
			if (vLight->lightDef->currentOcclusionQuery->IsVisible()) {
				vLight->lightDef->visibleFrame = tr.frameCount;
				isVisible = true;
			}
			else
			{
				// If we are inside of the light volume then assume we are visible.
				idBounds bounds = vLight->lightDef->globalLightBounds;
				if (bounds.ContainsPoint(backEnd.viewDef->renderView.vieworg)) {
					vLight->lightDef->visibleFrame = tr.frameCount;
				}
			}
		}

		delete vLight->lightDef->currentOcclusionQuery;
		vLight->lightDef->currentOcclusionQuery = NULL;
	}

	return isVisible;
}

void idRender::DrawTestOcclusion(void) {

	idRenderLightCommitted* vLight;

	int numLights = 0;
	int numVisibleLights = 0;

	if (backEnd.viewDef->viewLights == NULL)
		return;

	tr.occluderProgram->Bind();	

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);

	for (vLight = backEnd.viewDef->viewLights; vLight; vLight = vLight->next) {
		numLights++;

		if (RB_Reap_Occlusion(vLight))
		{
			numVisibleLights++;
		}

		rvmOcclusionQuery* query = new rvmOcclusionQuery();
		query->RunQuery(vLight->lightDef);
		vLight->lightDef->currentOcclusionQuery = query;
	}

	glDepthMask(GL_TRUE);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	tr.occluderProgram->BindNull();
}