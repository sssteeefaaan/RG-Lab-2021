#pragma once

#include <gl/GL.h>
#include <gl/GLU.h>
#include "DImage.h"

class CGLTexture
{

private:
	unsigned int m_ID;

public:
	CGLTexture();
	~CGLTexture();

	void LoadFromFile(CString filePath, GLint sWrap = GL_CLAMP, GLint tWrap = GL_CLAMP, GLint magFilter = GL_LINEAR, GLint minFilter=GL_LINEAR_MIPMAP_LINEAR);
	void Select();
	void Release();

	static void PrepareTexturing(bool withLighting = true);
};

