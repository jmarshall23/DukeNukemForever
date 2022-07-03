// DnFullscreenRenderTarget.cpp
//

#include "../../game/Game_local.h"

/*
=========================
DnFullscreenRenderTarget::DnFullscreenRenderTarget
=========================
*/
DnFullscreenRenderTarget::DnFullscreenRenderTarget(const char* name, bool hasAlbedo, bool hasDepth, bool hasMSAA)
{
	if (!hasAlbedo && !hasDepth)
	{
		common->FatalError("DnFullscreenRenderTarget: Invalid setup\n");
	}

	numMultiSamples = 0;

	if (hasMSAA)
	{
		numMultiSamples = renderSystem->GetNumMSAASamples();
	}

	if (hasAlbedo)
	{
		idImageOpts opts;
		opts.format = FMT_RGBA8;
		opts.colorFormat = CFM_DEFAULT;
		opts.numLevels = 1;
		opts.textureType = TT_2D;
		opts.isPersistant = true;
		opts.width = renderSystem->GetScreenWidth();
		opts.height = renderSystem->GetScreenHeight();
		opts.numMSAASamples = numMultiSamples;

		albedoImage = renderSystem->CreateImage(va("_%sAlbedo", name), &opts, TF_LINEAR);
	}
	else
	{
		albedoImage = nullptr;
	}

	if (hasDepth)
	{
		idImageOpts opts;
		opts.format = FMT_RGBA8;
		opts.colorFormat = CFM_DEFAULT;
		opts.numLevels = 1;
		opts.textureType = TT_2D;
		opts.isPersistant = true;
		opts.width = renderSystem->GetScreenWidth();
		opts.height = renderSystem->GetScreenHeight();
		opts.numMSAASamples = numMultiSamples;
		opts.format = FMT_DEPTH;

		depthImage = renderSystem->CreateImage(va("_%sDepth", name), &opts, TF_LINEAR);
	}
	else
	{
		depthImage = nullptr;
	}

	renderTexture = renderSystem->AllocRenderTexture(name, albedoImage, depthImage);
}

/*
=========================
DnFullscreenRenderTarget::Bind
=========================
*/
void DnFullscreenRenderTarget::Bind(void)
{
	renderSystem->BindRenderTexture(renderTexture);
}

/*
=========================
DnFullscreenRenderTarget::BindNull
=========================
*/
void DnFullscreenRenderTarget::BindNull(void)
{
	renderSystem->BindRenderTexture(nullptr);
}

/*
=========================
DnFullscreenRenderTarget::ResolveMSAA
=========================
*/
void DnFullscreenRenderTarget::ResolveMSAA(DnFullscreenRenderTarget* destTarget)
{
	if (numMultiSamples == 0)
	{
		common->FatalError("Failed to resolve Fullscreen render target, its not a multisampled!");
		return;
	}

	if (destTarget->numMultiSamples != 0)
	{
		common->FatalError("Can't resolve into a multisampled rendertarget!");
		return;
	}

	renderSystem->ResolveMSAA(renderTexture, destTarget->renderTexture);
}

/*
=========================
DnFullscreenRenderTarget::Clear
=========================
*/
void DnFullscreenRenderTarget::Clear(void)
{
	bool clearColor = albedoImage != nullptr;
	bool clearDepth = depthImage != nullptr;

	renderSystem->ClearRenderTarget(clearColor, clearDepth, 1.0f, 1.0f, 0.0f, 0.0f);
}

/*
=========================
DnFullscreenRenderTarget::Resize
=========================
*/
void DnFullscreenRenderTarget::Resize(int width, int height)
{
	int targetWidth, targetHeight;

	if (albedoImage)
	{
		renderSystem->GetImageSize(albedoImage, targetWidth, targetHeight);
	}
	else if (depthImage)
	{
		renderSystem->GetImageSize(depthImage, targetWidth, targetHeight);
	}

	if (targetWidth != width || targetHeight != height)
	{
		renderSystem->ResizeRenderTexture(renderTexture, targetWidth, targetHeight);
	}
}