// DeclRenderProg.cpp
//


#pragma hdrstop

#include "RenderSystem_local.h"

void GL_SelectTextureNoClient(int unit);

/*
===================
rvmDeclRenderProg::Size
===================
*/
size_t rvmDeclRenderProg::Size(void) const {
	return sizeof(rvmDeclRenderProg);
}

/*
===================
rvmDeclRenderProg::SetDefaultText
===================
*/
bool rvmDeclRenderProg::SetDefaultText(void) {
	return false;
}

/*
===================
rvmDeclRenderProg::DefaultDefinition
===================
*/
const char* rvmDeclRenderProg::DefaultDefinition(void) const {
	return "";
}

/*
===================
rvmDeclRenderProg::ParseRenderParms
===================
*/
idStr rvmDeclRenderProg::ParseRenderParms(idStr& bracketText) {
	idStr uniforms = "#version 130\n";

	idLexer src;
	idToken	token, token2;

	src.LoadMemory(bracketText.c_str(), bracketText.Length(), GetFileName(), GetLineNum());
	src.SetFlags(DECL_LEXER_FLAGS);
	src.SkipUntilString("{");

	while (1) {
		if (!src.ReadToken(&token)) {
			break;
		}

		if (!token.Icmp("}")) {
			break;
		}

		if (token == "$")
		{
			idToken tokenCase;
			src.ReadToken(&tokenCase);

			token = tokenCase;
			token.ToLower();
			rvmDeclRenderParam* parm = declManager->FindRenderParam(tokenCase.c_str());
			if (!parm)
			{
				src.Error("Failed to find render parm %s", token.c_str());
				return "";
			}

			// Make all params lower case.
			bracketText.Replace(tokenCase, token);

			idStr name = token;
			char* buffer = (char *)name.c_str(); // still not the worst thing I've ever done, muaahahaha!
			for (int i = 0; i < name.Length(); i++)
			{
				if (buffer[i] == '.')
				{
					buffer[i] = 0;
					break;
				}
			}

			if (parm->GetArraySize() == 1)
			{
				switch (parm->GetType())
				{
				case RENDERPARM_TYPE_IMAGE:
					uniforms += va("uniform sampler2D %s;\n", name.c_str());
					break;
				case RENDERPARM_TYPE_VEC4:
					uniforms += va("uniform vec4 %s;\n", name.c_str());
					break;
				case RENDERPARM_TYPE_FLOAT:
					uniforms += va("uniform float %s;\n", name.c_str());
					break;
				case RENDERPARM_TYPE_INT:
					uniforms += va("uniform int %s;\n", name.c_str());
					break;
				}
			}
			else
			{
				switch (parm->GetType())
				{
				case RENDERPARM_TYPE_IMAGE:
					uniforms += va("uniform sampler2D %s[%d];\n", name.c_str(), parm->GetArraySize());
					break;
				case RENDERPARM_TYPE_VEC4:
					uniforms += va("uniform vec4 %s[%d];\n", name.c_str(), parm->GetArraySize());
					break;
				case RENDERPARM_TYPE_FLOAT:
					uniforms += va("uniform float %s[%d];\n", name.c_str(), parm->GetArraySize());
					break;
				case RENDERPARM_TYPE_INT:
					uniforms += va("uniform int %s[%d];\n", name.c_str(), parm->GetArraySize());
					break;
				}
			}
			

			renderParams.AddUnique(parm);
		}
	}

	bracketText.Replace("$", "");

	uniforms += "\n";
	uniforms += tr.globalRenderInclude;
	uniforms += "\n";

	return uniforms;
}

/*
===================
rvmDeclRenderProg::CreateVertexShader
===================
*/
void rvmDeclRenderProg::CreateVertexShader(idStr& bracketText) {
	vertexShader = ParseRenderParms(bracketText);

	vertexShader += "attribute vec4		attr_TexCoord0;\n";
	vertexShader += "attribute vec3		attr_Tangent;\n";
	vertexShader += "attribute vec3		attr_Bitangent;\n";
	vertexShader += "attribute vec3      attr_Normal;\n";

	vertexShader += "void main(void)\n";
	vertexShader += "{\n";
	vertexShader += bracketText;
	vertexShader += "}\n";
}

/*
===================
rvmDeclRenderProg::CreatePixelShader
===================
*/
void rvmDeclRenderProg::CreatePixelShader(idStr& bracketText) {
	pixelShader = ParseRenderParms(bracketText);
	pixelShader += "void main(void)\n";
	pixelShader += bracketText;
}
/*
===================
rvmDeclRenderProg::LoadGLSLShader
===================
*/
int rvmDeclRenderProg::LoadGLSLShader(GLenum target, idStr& programGLSL) {
	const GLuint shader = glCreateShader(target);
	if (shader) {
		const char* source[1] = { programGLSL.c_str() };

		glShaderSource(shader, 1, source, NULL);
		glCompileShader(shader);

		int infologLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);
		if (infologLength > 1) {
			idTempArray<char> infoLog(infologLength);
			int charsWritten = 0;
			glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog.Ptr());

			// catch the strings the ATI and Intel drivers output on success
			if (strstr(infoLog.Ptr(), "successfully compiled to run on hardware") != NULL ||
				strstr(infoLog.Ptr(), "No errors.") != NULL) {
				//common->Printf( "%s program %s from %s compiled to run on hardware\n", typeName, GetName(), GetFileName() );
			}
			else {
				common->Printf("While compiling %s program %s\n", (target == GL_FRAGMENT_SHADER) ? "fragment" : "vertex", GetName());

				const char separator = '\n';
				idList<idStr> lines;
				lines.Clear();
				idStr source(programGLSL);
				lines.Append(source);
				for (int index = 0, ofs = lines[index].Find(separator); ofs != -1; index++, ofs = lines[index].Find(separator)) {
					lines.Append(lines[index].c_str() + ofs + 1);
					lines[index].CapLength(ofs);
				}

				common->Printf("-----------------\n");
				for (int i = 0; i < lines.Num(); i++) {
					common->Printf("%3d: %s\n", i + 1, lines[i].c_str());
				}
				common->Printf("-----------------\n");

				common->Printf("%s\n", infoLog.Ptr());
			}
		}

		GLint compiled = GL_FALSE;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (compiled == GL_FALSE) {
			glDeleteShader(shader);
			return -1;
		}
	}

	return shader;
}

/*
================================================================================================
idRenderProgManager::LoadGLSLProgram
================================================================================================
*/
void rvmDeclRenderProg::LoadGLSLProgram(void) {
	GLuint vertexProgID = vertexShaderHandle;
	GLuint fragmentProgID = pixelShaderHandle;

	program = glCreateProgram();
	if (program) {
		glAttachShader(program, vertexProgID);
		glAttachShader(program, fragmentProgID);

		glBindAttribLocation(program, PC_ATTRIB_INDEX_ST, "attr_TexCoord0");
		glBindAttribLocation(program, PC_ATTRIB_INDEX_TANGENT, "attr_Tangent");
		glBindAttribLocation(program, PC_ATTRIB_INDEX_BINORMAL, "attr_Bitangent");
		glBindAttribLocation(program, PC_ATTRIB_INDEX_NORMAL, "attr_Normal");

		glLinkProgram(program);

		int infologLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLength);
		if (infologLength > 1) {
			char* infoLog = (char*)malloc(infologLength);
			int charsWritten = 0;
			glGetProgramInfoLog(program, infologLength, &charsWritten, infoLog);

			// catch the strings the ATI and Intel drivers output on success
			if (strstr(infoLog, "Vertex shader(s) linked, fragment shader(s) linked.") != NULL || strstr(infoLog, "No errors.") != NULL) {
				//common->Printf( "render prog %s from %s linked\n", GetName(), GetFileName() );
			}
			else {
				if(strstr(infoLog, "error") || strstr(infoLog, "Error"))
					common->FatalError("WHILE LINKING %s\n", infoLog);
				else
					common->Warning("WHILE LINKING %s\n", infoLog);
			}

			free(infoLog);
		}
	}

	int linked = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (linked == GL_FALSE) {
		glDeleteProgram(program);
		idLib::Error("While linking GLSL program %s there was a internal error\n", GetName());
		return;
	}

	int textureUnit = 0;
	glUseProgram(program);

	// store the uniform locations after we have linked the GLSL program
	uniformLocations.Clear();
	for (int i = 0; i < renderParams.Num(); i++) {
		const char* parmName = renderParams[i]->GetName();
		GLint loc = glGetUniformLocation(program, parmName);
		if (loc != -1) {
			glslUniformLocation_t uniformLocation;
			uniformLocation.parmIndex = i;
			uniformLocation.uniformIndex = loc;

			if (renderParams[i]->GetType() == RENDERPARM_TYPE_IMAGE)
			{
				glUniform1i(loc, textureUnit);
				uniformLocation.textureUnit = textureUnit;
				textureUnit++;
			}

			uniformLocations.Append(uniformLocation);
		}
	}

	glUseProgram(0);
}

/*
===================
rvmDeclRenderProg::Bind
===================
*/
void rvmDeclRenderProg::Bind(void) {
	tmu = 0;

	glUseProgram(program);

	for (int i = 0; i < uniformLocations.Num(); i++) {
		const glslUniformLocation_t& uniformLocation = uniformLocations[i];
		rvmDeclRenderParam* parm = renderParams[uniformLocations[i].parmIndex];

		switch (parm->GetType())
		{
			case RENDERPARM_TYPE_IMAGE:
				if (uniformLocation.textureUnit != -1)
				{
					GL_SelectTextureNoClient(uniformLocation.textureUnit);
					parm->GetImage()->Bind();
					tmu++;
				}
				break;

			case RENDERPARM_TYPE_VEC4:
				glUniform4fv(uniformLocation.uniformIndex, parm->GetArraySize(), parm->GetVectorValuePtr());
				break;

			case RENDERPARM_TYPE_FLOAT:
				glUniform1f(uniformLocation.uniformIndex, parm->GetFloatValue());
				break;

			case RENDERPARM_TYPE_INT:
				glUniform1i(uniformLocation.uniformIndex, parm->GetIntValue());
				break;
		}
		
	}
}
/*
===================
rvmDeclRenderProg::BindNull
===================
*/
void rvmDeclRenderProg::BindNull(void) {
	glUseProgram(0);
	if (tmu > 1) {
		while (tmu > 1)
		{
			GL_SelectTextureNoClient(tmu);
			globalImages->BindNull();
			tmu--;
		}
	}
}

/*
===================
rvmDeclRenderProg::Parse
===================
*/
bool rvmDeclRenderProg::Parse(const char* text, const int textLength) {
	idLexer src;
	idToken	token, token2;

	tmu = 0;

	src.LoadMemory(text, textLength, GetFileName(), GetLineNum());
	src.SetFlags(DECL_LEXER_FLAGS);
	src.SkipUntilString("{");

	while (1) {
		if (!src.ReadToken(&token)) {
			break;
		}

		if (!token.Icmp("}")) {
			break;
		}

		if (token == "vertex")
		{
			idStr bracketSection;
			src.ParseBracedSection(bracketSection);
			CreateVertexShader(bracketSection);

			vertexShaderHandle = LoadGLSLShader(GL_VERTEX_SHADER, vertexShader);
		}
		else if (token == "pixel")
		{
			idStr bracketSection;
			src.ParseBracedSection(bracketSection);
			CreatePixelShader(bracketSection);

			pixelShaderHandle = LoadGLSLShader(GL_FRAGMENT_SHADER, pixelShader);
		}
		else
		{
			src.Error("Unknown or unexpected token %s\n", token.c_str());
		}
	}

	LoadGLSLProgram();
	return true;
}

/*
===================
rvmDeclRenderProg::FreeData
===================
*/
void rvmDeclRenderProg::FreeData(void) {

}