#include "pch.h"
#include "CGLTexture.h"

CGLTexture::CGLTexture()
{
	m_ID = 0;
}

CGLTexture::~CGLTexture()
{
	Release();
}

void CGLTexture::LoadFromFile(CString filePath, GLint sWrap, GLint tWrap, GLint magFilter, GLint minFilter)
{
	if (m_ID != 0) Release();

	// Alokacija ID-a i kreiranje teksture
	glGenTextures(1, &m_ID);
	glBindTexture(GL_TEXTURE_2D, m_ID);

	// Ucitavanje slike
	DImage img;
	img.Load(filePath);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);

	// Kopiranje sadrzaja slike u teksturu
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_RGBA, GL_UNSIGNED_BYTE, img.GetDIBBits());

	// Bind na default teksturu
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CGLTexture::Select()
{
	if(m_ID)
		glBindTexture(GL_TEXTURE_2D, m_ID);
}

void CGLTexture::Release()
{
	if (m_ID)
	{
		glDeleteTextures(1, &m_ID);
		m_ID = 0;
	}
}

void CGLTexture::PrepareTexturing(bool withLighting)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	if (withLighting)
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	else
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}