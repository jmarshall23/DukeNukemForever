// DeclRenderParm.h
//

#pragma once

class idImage;

//
// rvmDeclRenderParmType_t
//
enum rvmDeclRenderParmType_t {
	RENDERPARM_TYPE_INVALID = 0,
	RENDERPARM_TYPE_IMAGE,
	RENDERPARM_TYPE_VEC4,
	RENDERPARM_TYPE_FLOAT
};

//
// rvmDeclRenderProg
//
class rvmDeclRenderParam : public idDecl {
public:
	virtual size_t			Size(void) const;
	virtual bool			SetDefaultText(void);
	virtual const char* DefaultDefinition(void) const;
	virtual bool			Parse(const char* text, const int textLength);
	virtual void			FreeData(void);

	rvmDeclRenderParmType_t GetType() { return type; }

	idImage*				GetImage(void) { return imageValue; }
	void					SetImage(idImage* image) { imageValue = image; }

	idVec4					GetVectorValue(void) { return vectorValue; }
	void					SetVectorValue(idVec4 value) { vectorValue = value; }

	float					GetFloatValue(void) { return floatValue; }
	void					SetFloatValue(float value) { floatValue = value; }
private:
	rvmDeclRenderParmType_t type;

	idImage*				imageValue;
	idVec4					vectorValue;
	float					floatValue;
};