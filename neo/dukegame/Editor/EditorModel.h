// EditorModel.h
//

//
// dnEditorModel
//
class dnEditorModel : public dnEditorEntity {
public:
	dnEditorModel(idRenderWorld* editorRenderWorld);
	~dnEditorModel();

	virtual void				Render(idDict& spawnArgs, bool isSelected) override;

private:
	qhandle_t	renderEntityHandle;
	renderEntity_t renderEntityParams;
};