// DnRender.h
//

//
// DnFullscreenRenderTarget
//
class DnFullscreenRenderTarget {
public:
	DnFullscreenRenderTarget(const char *name, bool hasAlbedo, bool hasDepth, bool hasMSAA);

	void Bind(void);
	void ResolveMSAA(DnFullscreenRenderTarget* destTarget);
	void Resize(int width, int height);
	void Clear(void);

	static void BindNull(void);
private:
	idImage* albedoImage;
	idImage* depthImage;
	idRenderTexture* renderTexture;
	int numMultiSamples;
};
