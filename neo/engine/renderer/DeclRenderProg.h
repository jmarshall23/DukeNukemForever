// DeclRenderProg.h
//

#pragma once

static const int PC_ATTRIB_INDEX_VERTEX = 0;
static const int PC_ATTRIB_INDEX_ST = 8;
static const int PC_ATTRIB_INDEX_TANGENT = 9;
static const int PC_ATTRIB_INDEX_BINORMAL = 10;
static const int PC_ATTRIB_INDEX_NORMAL = 11;

struct glslUniformLocation_t {
	int		parmIndex;
	GLint	uniformIndex;
	GLint   textureUnit;
};

//
// rvmDeclRenderProg
//
class rvmDeclRenderProg : public idDecl {
public:
	virtual size_t			Size(void) const;
	virtual bool			SetDefaultText(void);
	virtual const char* DefaultDefinition(void) const;
	virtual bool			Parse(const char* text, const int textLength);
	virtual void			FreeData(void);

	void					Bind(void);

	void					BindNull(void);
private:
	void					CreateVertexShader(idStr &bracketText);
	void					CreatePixelShader(idStr& bracketText);

	idStr					ParseRenderParms(idStr& bracketText, const char *programMacro);
private:
	int						LoadGLSLShader(GLenum target, idStr& programGLSL);
	void					LoadGLSLProgram(void);
private:
	idStr					vertexShader;
	int						vertexShaderHandle;

	idStr					pixelShader;
	int						pixelShaderHandle;

	GLuint					program;

	int						tmu;

	idList<rvmDeclRenderParam*> renderParams;
	idList<glslUniformLocation_t> uniformLocations;
};