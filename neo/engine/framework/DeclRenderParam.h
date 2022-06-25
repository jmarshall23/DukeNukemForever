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
	RENDERPARM_TYPE_FLOAT,
	RENDERPARM_TYPE_INT
};

#define MAX_RENDERPARM_ARRAY_SIZE		128

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

	float*					GetVectorValuePtr(void) { return &vectorValue[0].x; }
	idVec4					GetVectorValue(int idx = 0) { return vectorValue[idx]; }
	void					SetVectorValue(idVec4 value, int idx = 0) { vectorValue[idx] = value; }
	void					SetVectorValue(const float *value, int idx = 0) { vectorValue[idx] = idVec4(value[0], value[1], value[2], value[3]); }

	float					GetFloatValue(void) { return floatValue; }
	void					SetFloatValue(float value) { floatValue = value; }

	int						GetArraySize(void) { return array_size; }

	int						GetIntValue(void) { return intValue; }
	void					SetIntValue(int value) { intValue = value; }
private:
	rvmDeclRenderParmType_t type;

	idImage*				imageValue;
	idVec4					vectorValue[MAX_RENDERPARM_ARRAY_SIZE];
	float					floatValue;
	int						array_size;
	int						intValue;
};