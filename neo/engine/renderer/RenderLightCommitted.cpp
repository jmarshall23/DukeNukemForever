// RenderLightCommitted.cpp
//

#include "RenderSystem_local.h"


/*
====================
R_TestPointInViewLight
====================
*/
// this needs to be greater than the dist from origin to corner of near clip plane
bool idRenderLightCommitted::TestPointInViewLight(const idVec3& org, const idRenderLightLocal* light) {
	int		i;
	idVec3	local;

	for (i = 0; i < 6; i++) {
		float d = light->frustum[i].Distance(org);
		if (d > INSIDE_LIGHT_FRUSTUM_SLOP) {
			return false;
		}
	}

	return true;
}