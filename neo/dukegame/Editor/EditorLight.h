// EditorLight.h
//

//
// dnEditorLight
//
class dnEditorLight : public dnEditorEntity {
public:
	dnEditorLight(idRenderWorld* editorRenderWorld);
	~dnEditorLight();

	// Renders the light.
	virtual void				Render(idDict& spawnArgs, bool isSelected) override;

private:
	qhandle_t	renderLightHandle;
	renderLight_t renderLightParams;
};