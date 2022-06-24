// FrameRateGui.cpp
//

rvmToolShowFrameRate displayFrameRateTool;

/*
=======================
rvmToolShowFrameRate::Name
=======================
*/
const char* rvmToolShowFrameRate::Name(void) {
	return "Framerate";
}

/*
=======================
rvmToolShowFrameRate::Render
=======================
*/
#define	FPS_FRAMES	4
void rvmToolShowFrameRate::Render( void ) {
	char* s;
	int			w;
	static int	previousTimes[FPS_FRAMES];
	static int	index;
	int		i, total;
	int		fps = 0;
	static	int	previous;
	int		t, frameTime;
	//rvmPerformanceMetrics_t* metrics;

	// don't use serverTime, because that will be drifting to
	// correct for internet lag changes, timescales, timedemos, etc
	t = Sys_Milliseconds();
	frameTime = t - previous;
	previous = t;

	previousTimes[index % FPS_FRAMES] = frameTime;
	index++;
	if (index > FPS_FRAMES) {
		// average multiple frames together to smooth changes out a bit
		total = 0;
		for (i = 0; i < FPS_FRAMES; i++) {
			total += previousTimes[i];
		}
		if (!total) {
			total = 1;
		}
		fps = 10000 * FPS_FRAMES / total;
		fps = (fps + 5) / 10;
	}

	bool toolActive = false;

	//metrics = session->GetGameRenderMetrics();

	ImGuiStyle& style = ImGui::GetStyle();
	style.FramePadding = ImVec2(0, 0);
	ImGui::SetNextWindowSize( ImVec2( 350, 75 ) );
    ImGui::SetNextWindowPos( ImVec2( renderSystem->GetScreenWidth() - 400, 20 ) );
	ImGui::Begin("Framerate", &toolActive, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
#ifdef _DEBUG
	ImGui::Text("Frame Metrics(Debug Build)");
#else
	ImGui::Text("Frame Metrics(Release Build)");
#endif
		ImGui::LabelText(va("%d fps", fps), "FPS");
		ImGui::LabelText(va("%d ms", time_gameFrame), "Game");
		
	ImGui::End();
}