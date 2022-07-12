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


#pragma hdrstop

#include "RenderSystem_local.h"

#include "../external/bink/bink.h"

#define CIN_system	1
#define CIN_loop	2
#define	CIN_hold	4
#define CIN_silent	8
#define CIN_shader	16

class idCinematicLocal : public idCinematic {
public:
	idCinematicLocal();
	~idCinematicLocal();


	virtual bool			InitFromFile( const char *qpath, bool looping );
	virtual cinData_t		ImageForTime( int milliseconds );
	virtual void			Close();
	virtual void			ResetTime(int time);
	virtual bool			IsDone() { return isDone; }

	virtual idImage*		GetRenderImage() { return renderImage; }
private:
	HBINK Bink = 0;
	HBINKBUFFER Bink_buffer = 0;
	cinData_t currentFrameData;
	bool isLooping = false;
	bool isDone = false;

	idImage* renderImage;
};

//===========================================

/*
====================
idCinematic::InitCinematic
====================
*/
void idCinematic::InitCinematic(void) {

}

/*
====================
idCinematic::ShutdownCinematic
====================
*/
void idCinematic::ShutdownCinematic(void) {

}

/*
====================
idCinematicLocal::idCinematicLocal
====================
*/
idCinematicLocal::idCinematicLocal() {
	renderImage = nullptr;
	Bink = nullptr;
}

/*
====================
idCinematicLocal::~idCinematicLocal
====================
*/
idCinematicLocal::~idCinematicLocal() {
	if (renderImage)
	{
		renderImage->PurgeImage();
		renderImage = nullptr;
	}

	if (Bink)
	{
		BinkClose(Bink);
		Bink = nullptr;
	}
}

/*
====================
idCinematic::Alloc
====================
*/
idCinematic* idCinematic::Alloc() {
	return new idCinematicLocal();
}

/*
====================
idCinematic::InitFromFile
====================
*/
bool idCinematicLocal::InitFromFile(const char* qpath, bool looping) {
	idStr osPath = fileSystem->RelativePathToOSPath(qpath);

	// Open the movie file
	Bink = BinkOpen(osPath.c_str(), 0);
	if (!Bink)
	{
		Bink = nullptr;
		common->Warning("Failed to open %s", qpath);
		return false;
	}

	currentFrameData.imageWidth = Bink->Width;
	currentFrameData.imageHeight = Bink->Height;
	currentFrameData.image = new byte[Bink->Width * Bink->Height * 4];
	currentFrameData.status = FMV_PLAY;

	{
		idImageOpts opts;
		static int numRenderImages = 0;

		opts.format = FMT_RGBA8;
		opts.colorFormat = CFM_DEFAULT;
		opts.numLevels = 1;
		opts.textureType = TT_2D;
		opts.isPersistant = true;
		opts.width = Bink->Width;
		opts.height = Bink->Height;
		opts.numMSAASamples = 0;

		renderImage = renderSystem->CreateImage(va("_binkMovie%d", numRenderImages++), &opts, TF_NEAREST);
	}

	

	isDone = false;
	isLooping = looping;

	return true;
}

/*
====================
idCinematic::ImageForTime
====================
*/
cinData_t idCinematicLocal::ImageForTime(int milliseconds) {
	if (!BinkWait(Bink))
	{
		if (Bink->FrameNum == Bink->Frames)
		{
			if (!isLooping)
			{
				currentFrameData.status = FMV_EOF;
				isDone = true;
				return currentFrameData;
			}

			Bink->FrameNum = 0;
		}

		BinkDoFrame(Bink);

		BinkCopyToBufferRect(Bink,
			(void *)currentFrameData.image,
			Bink->Width * 4,
			Bink->Height,
			0, 0,
			0, 0, Bink->Width, Bink->Height,
			BINKSURFACE32 |
			BINKCOPYALL);

		// RGB to BGR
		byte* image_data = (byte*)currentFrameData.image;
		for (int i = 0; i < Bink->Width * Bink->Height * 4; i += 4)
		{
			byte r = image_data[i + 0];
			image_data[i + 0] = image_data[i + 2];
			image_data[i + 2] = r;
		}

		BinkNextFrame(Bink);
	}

	return currentFrameData;
}


/*
==============
idSndWindow::Close
==============
*/
void idCinematicLocal::Close() {
	if (Bink == nullptr)
		return;

	delete currentFrameData.image;
	
	BinkClose(Bink);
}

/*
==============
idSndWindow::ResetTime
==============
*/
void idCinematicLocal::ResetTime(int time) {
	Bink->FrameNum = 0;
	isDone = false;
}
