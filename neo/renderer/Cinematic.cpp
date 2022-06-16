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

#include "precompiled.h"
#pragma hdrstop

#include "tr_local.h"

#include "../external/bink/bink.h"

#define CIN_system	1
#define CIN_loop	2
#define	CIN_hold	4
#define CIN_silent	8
#define CIN_shader	16

class idCinematicLocal : public idCinematic {
public:
	virtual bool			InitFromFile( const char *qpath, bool looping );
	virtual cinData_t		ImageForTime( int milliseconds );
	virtual void			Close();
	virtual void			ResetTime(int time);
	virtual bool			IsDone() { return isDone; }
private:
	HBINK Bink = 0;
	HBINKBUFFER Bink_buffer = 0;
	cinData_t currentFrameData;
	bool isLooping = false;
	bool isDone = false;
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
	// Open the movie file
	Bink = BinkOpen(qpath, 0);
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
