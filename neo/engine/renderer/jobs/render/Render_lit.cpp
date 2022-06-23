/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company. 

This file is part of the Doom 3 GPL Source Code (?Doom 3 Source Code?).  

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#include "../../RenderSystem_local.h"

/*
=========================================================================================

GENERAL INTERACTION RENDERING

=========================================================================================
*/

/*
====================
GL_SelectTextureNoClient
====================
*/
void GL_SelectTextureNoClient( int unit ) {
	backEnd.glState.currenttmu = unit;
	glActiveTextureARB( GL_TEXTURE0_ARB + unit );
	RB_LogComment( "glActiveTextureARB( %i )\n", unit );
}

/*
==================
RB_ARB2_DrawInteraction
==================
*/
void	RB_ARB2_DrawInteraction( const drawInteraction_t *din ) {
// jmarshall
	// load all the vertex program parameters
	tr.lightOriginParam->SetVectorValue(din->localLightOrigin);
	tr.viewOriginParam->SetVectorValue(din->localViewOrigin);
	tr.lightProjectionSParam->SetVectorValue(din->lightProjection[0]);
	tr.lightProjectionTParam->SetVectorValue(din->lightProjection[1]);
	tr.lightProjectionQParam->SetVectorValue(din->lightProjection[2]);
	tr.lightfalloffSParam->SetVectorValue(din->lightProjection[3]);
	tr.bumpmatrixSParam->SetVectorValue(din->bumpMatrix[0]);
	tr.bumpmatrixTParam->SetVectorValue(din->bumpMatrix[1]);
	tr.diffuseMatrixSParam->SetVectorValue(din->diffuseMatrix[0]);
	tr.diffuseMatrixTParam->SetVectorValue(din->diffuseMatrix[1]);
	tr.specularMatrixSParam->SetVectorValue(din->specularMatrix[0]);
	tr.specularMatrixTParam->SetVectorValue(din->specularMatrix[1]);

	// load all the vertex program parameters
	//glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_LIGHT_ORIGIN, din->localLightOrigin.ToFloatPtr() );
	//glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_VIEW_ORIGIN, din->localViewOrigin.ToFloatPtr() );
	//glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_LIGHT_PROJECT_S, din->lightProjection[0].ToFloatPtr() );
	//glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_LIGHT_PROJECT_T, din->lightProjection[1].ToFloatPtr() );
	//glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_LIGHT_PROJECT_Q, din->lightProjection[2].ToFloatPtr() );
	//glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_LIGHT_FALLOFF_S, din->lightProjection[3].ToFloatPtr() );
	//glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_BUMP_MATRIX_S, din->bumpMatrix[0].ToFloatPtr() );
	//glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_BUMP_MATRIX_T, din->bumpMatrix[1].ToFloatPtr() );
	//glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_DIFFUSE_MATRIX_S, din->diffuseMatrix[0].ToFloatPtr() );
	//glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_DIFFUSE_MATRIX_T, din->diffuseMatrix[1].ToFloatPtr() );
	//glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_SPECULAR_MATRIX_S, din->specularMatrix[0].ToFloatPtr() );
	//glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_SPECULAR_MATRIX_T, din->specularMatrix[1].ToFloatPtr() );
// jmarshall end


	// testing fragment based normal mapping
	//if ( r_testARBProgram.GetBool() ) {
	//	glProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, 2, din->localLightOrigin.ToFloatPtr() );
	//	glProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, 3, din->localViewOrigin.ToFloatPtr() );
	//}

	static idVec4 zero( 0, 0, 0, 0 );
	static idVec4 one(1, 1, 1, 1 );
	static idVec4 negOne(-1, -1, -1, -1 );

	switch ( din->vertexColor ) {
	case SVC_IGNORE:		
		tr.vertexScaleModulateParam->SetVectorValue(zero);
		tr.vertexScaleAddParam->SetVectorValue(one);
		break;
	case SVC_MODULATE:
		tr.vertexScaleModulateParam->SetVectorValue(one);
		tr.vertexScaleAddParam->SetVectorValue(zero);
		break;
	case SVC_INVERSE_MODULATE:
		tr.vertexScaleModulateParam->SetVectorValue(negOne);
		tr.vertexScaleAddParam->SetVectorValue(one);		
		break;
	}

	// set the constant colors
	//glProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, 0, din->diffuseColor.ToFloatPtr() );
	//glProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, 1, din->specularColor.ToFloatPtr() );
	tr.lightColorParam->SetVectorValue(din->diffuseColor);

	// set the textures

	tr.bumpmapTextureParam->SetImage(din->bumpImage);
	tr.lightfalloffTextureParam->SetImage(din->lightFalloffImage);
	tr.lightProgTextureParam->SetImage(din->lightImage);
	tr.albedoTextureParam->SetImage(din->diffuseImage);
	tr.specularTextureParam->SetImage(din->specularImage);

	// draw it
	tr.interactionProgram->Bind();
	RB_DrawElementsWithCounters( din->surf->geo );
}


/*
=============
RB_ARB2_CreateDrawInteractions

=============
*/
void RB_ARB2_CreateDrawInteractions( const drawSurf_t *surf ) {
	if ( !surf ) {
		return;
	}

	// perform setup here that will be constant for all interactions
	GL_State( GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE | GLS_DEPTHMASK | backEnd.depthFunc );

	// bind the vertex program
	//if ( r_testARBProgram.GetBool() ) {
	//	glBindProgramARB( GL_VERTEX_PROGRAM_ARB, VPROG_TEST );
	//	glBindProgramARB( GL_FRAGMENT_PROGRAM_ARB, FPROG_TEST );
	//} else {
	//	glBindProgramARB( GL_VERTEX_PROGRAM_ARB, VPROG_INTERACTION );
	//	glBindProgramARB( GL_FRAGMENT_PROGRAM_ARB, FPROG_INTERACTION );
	//}

	//glEnable(GL_VERTEX_PROGRAM_ARB);
	//glEnable(GL_FRAGMENT_PROGRAM_ARB);

	// enable the vertex arrays
	glEnableVertexAttribArrayARB( 8 );
	glEnableVertexAttribArrayARB( 9 );
	glEnableVertexAttribArrayARB( 10 );
	glEnableVertexAttribArrayARB( 11 );
	glEnableClientState( GL_COLOR_ARRAY );

	for ( ; surf ; surf=surf->nextOnLight ) {
		// perform setup here that will not change over multiple interaction passes

		// set the vertex pointers
		idDrawVert	*ac = (idDrawVert *)vertexCache.Position( surf->geo->ambientCache );
		glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( idDrawVert ), ac->color );
		glVertexAttribPointerARB( 11, 3, GL_FLOAT, false, sizeof( idDrawVert ), ac->normal.ToFloatPtr() );
		glVertexAttribPointerARB( 10, 3, GL_FLOAT, false, sizeof( idDrawVert ), ac->tangents[1].ToFloatPtr() );
		glVertexAttribPointerARB( 9, 3, GL_FLOAT, false, sizeof( idDrawVert ), ac->tangents[0].ToFloatPtr() );
		glVertexAttribPointerARB( 8, 2, GL_FLOAT, false, sizeof( idDrawVert ), ac->st.ToFloatPtr() );
		glVertexPointer( 3, GL_FLOAT, sizeof( idDrawVert ), ac->xyz.ToFloatPtr() );

		// this may cause RB_ARB2_DrawInteraction to be exacuted multiple
		// times with different colors and images if the surface or light have multiple layers
		RB_CreateSingleDrawInteractions( surf, RB_ARB2_DrawInteraction );
	}

	glDisableVertexAttribArrayARB( 8 );
	glDisableVertexAttribArrayARB( 9 );
	glDisableVertexAttribArrayARB( 10 );
	glDisableVertexAttribArrayARB( 11 );
	glDisableClientState( GL_COLOR_ARRAY );

	// disable features
	tr.interactionProgram->BindNull();

	backEnd.glState.currenttmu = -1;
	GL_SelectTexture( 0 );

	//glDisable(GL_VERTEX_PROGRAM_ARB);
	//glDisable(GL_FRAGMENT_PROGRAM_ARB);
}


/*
==================
idRender::DrawForwardLit
==================
*/
void idRender::DrawForwardLit( void ) {
	idRenderLightCommitted		*vLight;
	const idMaterial	*lightShader;

	GL_SelectTexture( 0 );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	//
	// for each light, perform adding and shadowing
	//
	for ( vLight = backEnd.viewDef->viewLights ; vLight ; vLight = vLight->next ) {
		backEnd.vLight = vLight;

		// do fogging later
		if ( vLight->lightShader->IsFogLight() ) {
			continue;
		}
		if ( vLight->lightShader->IsBlendLight() ) {
			continue;
		}

		// set the shadow map info.
		idVec4 shadowMapInfo(backEnd.vLight->shadowMapSlice, renderShadowSystem.GetAtlasSampleScale(), renderShadowSystem.GetShadowMapAtlasSize(), 0);
		tr.shadowMapInfoParm->SetVectorValue(shadowMapInfo);

		lightShader = vLight->lightShader;

		idVec4 lightOrigin(vLight->lightDef->parms.origin.x, vLight->lightDef->parms.origin.y, vLight->lightDef->parms.origin.z, 1.0);
		tr.globalLightOriginParam->SetVectorValue(lightOrigin);

		for (int i = 0; i < vLight->litRenderEntities.Num(); i++)
		{
			idRenderModel* renderModel = vLight->litRenderEntities[i]->viewEntity->renderModel;


			for (int s = 0; s < renderModel->NumSurfaces(); s++)
			{
				drawSurf_t fakeDrawSurf = { };
				const modelSurface_t* surface = renderModel->Surface(s);

				idScreenRect	shadowScissor;
				idScreenRect	lightScissor;

				lightScissor = vLight->scissorRect;
				lightScissor.Intersect(vLight->litRenderEntities[i]->viewEntity->scissorRect);

				if (lightScissor.IsEmpty())
					continue;

				fakeDrawSurf.geo = surface->geometry;
				fakeDrawSurf.material = surface->shader;
				fakeDrawSurf.space = vLight->litRenderEntities[i]->viewEntity;
				fakeDrawSurf.scissorRect = vLight->scissorRect;

				if(fakeDrawSurf.geo->numVerts == 0)
					continue;

				RB_SetModelMatrix(fakeDrawSurf.space->modelMatrix);

				const float* constRegs = surface->shader->ConstantRegisters();
				if (constRegs) {
					// this shader has only constants for parameters
					fakeDrawSurf.shaderRegisters = constRegs;
				}
				else
				{
					float* regs = (float*)R_FrameAlloc(fakeDrawSurf.material->GetNumRegisters() * sizeof(float));
					fakeDrawSurf.shaderRegisters = regs;
					fakeDrawSurf.material->EvaluateRegisters(regs, fakeDrawSurf.shaderRegisters, backEnd.viewDef, nullptr);
				}
				

				RB_ARB2_CreateDrawInteractions(&fakeDrawSurf);
			}
		}		
	}

	// disable stencil shadow test
	glStencilFunc( GL_ALWAYS, 128, 255 );

	GL_SelectTexture( 0 );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
}

