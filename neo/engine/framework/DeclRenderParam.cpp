// DeclRenderParm.cpp
//



/*
===================
rvmDeclRenderParam::Size
===================
*/
size_t rvmDeclRenderParam::Size(void) const {
	return sizeof(rvmDeclRenderParam);
}

/*
===================
rvmDeclRenderParam::SetDefaultText
===================
*/
bool rvmDeclRenderParam::SetDefaultText(void) {
	return false;
}

/*
===================
rvmDeclRenderParam::DefaultDefinition
===================
*/
const char* rvmDeclRenderParam::DefaultDefinition(void) const {
	return "";
}

/*
===================
rvmDeclRenderParam::Parse
===================
*/
bool rvmDeclRenderParam::Parse(const char* text, const int textLength) {
	idLexer src;
	idToken	token, token2;

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

		if (token == "texture")
		{
			type = RENDERPARM_TYPE_IMAGE;
		}
		else if (token == "float4")
		{
			type = RENDERPARM_TYPE_VEC4;
		}
		else if (token == "float")
		{
			type = RENDERPARM_TYPE_FLOAT;
		}
		else
		{
			src.Error("Unknown render parm type!\n");
		}
	}

	return true;
}

/*
===================
rvmDeclRenderParam::FreeData
===================
*/
void rvmDeclRenderParam::FreeData(void) {

}