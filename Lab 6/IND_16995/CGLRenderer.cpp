#include "pch.h"
#include "CGLRenderer.h"

CGLRenderer::CGLRenderer()
{
	this->viewR = 50;

	this->viewAngleXY = 0;
	this->viewAngleXZ = 0;

	this->lookingAt[0] = 0;
	this->lookingAt[1] = 0;
	this->lookingAt[2] = 0;

	this->upVector[0] = 0;
	this->upVector[1] = 1;
	this->upVector[2] = 0;

	this->showAxes = true;

	this->CalculatePosition();

	this->truck = new CGLMaterial(),
		this->wheels = new CGLMaterial(),
		this->platform = new CGLMaterial();

	this->texAtlas = new CGLTexture(),
		texGrass = new CGLTexture();
}

CGLRenderer::~CGLRenderer()
{
	if (this->truck)
	{
		delete this->truck;
		this->truck = nullptr;
	}

	if (this->wheels)
	{
		delete this->wheels;
		this->wheels = nullptr;
	}

	if (this->platform)
	{
		delete this->platform;
		this->platform = nullptr;
	}

	if (this->texAtlas)
	{
		delete this->texAtlas;
		this->texAtlas = nullptr;
	}

	if (this->texGrass)
	{
		delete this->texGrass;
		this->texGrass = nullptr;
	}
}

bool CGLRenderer::CreateGLContext(CDC* pDC)
{
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int nPixelFormat = ChoosePixelFormat(pDC->m_hDC, &pfd);

	if (nPixelFormat == 0) return false;

	BOOL bResult = SetPixelFormat(pDC->m_hDC, nPixelFormat, &pfd);

	if (!bResult) return false;

	m_hrc = wglCreateContext(pDC->m_hDC);

	if (!m_hrc) return false;

	return true;
}

void CGLRenderer::PrepareScene(CDC* pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//--------------------------------
	
	glClearColor(0.8, 0.8, 0.8, 1);
	glEnable(GL_DEPTH_TEST);

	this->platform->SetAmbient(.35, .35, .35, 1);
	this->platform->SetDiffuse(.65, .65, .65, 1);

	this->truck->SetAmbient(.45, .45, .45, 1);
	this->truck->SetDiffuse(.75, .75, .75, 1);

	this->wheels->SetAmbient(1, 1, 1, 1);
	this->wheels->SetDiffuse(1, 1, 1, 1);

	CGLTexture::PrepareTexturing(true);
	this->texAtlas->LoadFromFile((CString)"lab-6-atlas2.jpg");
	this->texGrass->LoadFromFile((CString)"lab-6-grass.jpg", GL_REPEAT, GL_REPEAT);

	// Globalno ambientno svetlo
	GLfloat l_model_ambient[] = { .4, .4, .4, 1 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, l_model_ambient);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

	// Direkciono svetlo 
	float light_ambient_directional[] = { .3, .3, .3, 1 };
	float light_diffuse_directional[] = { 1, 1, 1, 1 };
	float light_specular_directional[] = { .2, .2, .2, 1 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient_directional);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse_directional);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular_directional);

	//--------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::Reshape(CDC* pDC, int w, int h)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//--------------------------------

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (double)w / (double)h, 1, 200);

	//--------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC* pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//--------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	glLoadIdentity();

	gluLookAt(this->viewPosition[0], this->viewPosition[1], this->viewPosition[2],
		this->lookingAt[0], this->lookingAt[1], this->lookingAt[2],
		this->upVector[0], this->upVector[1], this->upVector[2]);

	SetLighting();

	glPushMatrix();
	{
		glTranslatef(0, -3, 0);
		DrawPlatform();
		DrawTruck();
	}
	glPopMatrix();

	if (this->showAxes)
		DrawAxes(50);

	//--------------------------------
	glFlush();
	SwapBuffers(pDC->m_hDC);
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC* pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//--------------------------------

	this->texAtlas->Release();
	this->texGrass->Release();

	//--------------------------------
	wglMakeCurrent(NULL, NULL);

	if (m_hrc) {
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::DrawPlatform(double width, double length, int repS, int repT)
{
	glEnable(GL_TEXTURE_2D);
	this->platform->Select();
	this->texGrass->Select();

	glBegin(GL_QUADS);
	{
		glNormal3f(0, 1, 0);

		glTexCoord2f(0, 0);
		glVertex3f(width / 2, 0, length / 2);

		glTexCoord2f(repS, 0);
		glVertex3f(width / 2, 0, -length / 2);

		glTexCoord2f(repS, repT);
		glVertex3f(-width / 2, 0, -length / 2);

		glTexCoord2f(0, repT);
		glVertex3f(-width / 2, 0, length / 2);
	}
	glEnd();


	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawTruck()
{
	glEnable(GL_TEXTURE_2D);

	this->truck->Select();
	this->texAtlas->Select();

	glPushMatrix();
	{
		glTranslatef(0, 3, 0);
		glRotatef(90, 1, 0, 0);

		this->wheels->Select();

		glPushMatrix();
		{
			glTranslatef(2, 3, 0);
			DrawCylinder(1.5, 3, 3, 8, 10.0 / 16, 14.5 / 16, 1.5 / 16);
		}
		glPopMatrix();

		glPushMatrix();
		{
			glTranslatef(-6, 3, 0);
			DrawCylinder(1.5, 3, 3, 8, 10.0 / 16, 14.5 / 16, 1.5 / 16);
		}
		glPopMatrix();

		glRotatef(-180, 1, 0, 0);

		glPushMatrix();
		{
			glTranslatef(2, 3, 0);
			DrawCylinder(1.5, 3, 3, 8, 10.0 / 16, 14.5 / 16, 1.5 / 16);
		}
		glPopMatrix();

		glPushMatrix();
		{
			glTranslatef(-6, 3, 0);
			DrawCylinder(1.5, 3, 3, 8, 10.0 / 16, 14.5 / 16, 1.5 / 16);
		}
		glPopMatrix();

		this->truck->Select();
	}
	glPopMatrix();

	glPushMatrix();
	{
		glTranslatef(-11.2, 2.5, 3);
		DrawTruckBody(2, 1.0 / 16, 0, 10.0/16);
	}
	glPopMatrix();

	glPushMatrix();
	{
		glTranslatef(0, 15, 0);
		glScalef(1.5, 1, 1);
		DrawSphere(5, 16, 16, 1, .5);
	}
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawTruckBody(double a, double texA, double dS, double dT)
{
	glBegin(GL_POLYGON);
	{
		glNormal3f(0, 0, 1);

		glTexCoord2f(dS, dT + 3 * texA);
		glVertex3f(0, 3.0 * a, 0);

		glTexCoord2f(dS, dT);
		glVertex3f(0, 0, 0);

		glTexCoord2f(dS + texA, dT);
		glVertex3f(a, 0, 0);

		glTexCoord2f(dS + texA, dT + texA);
		glVertex3f(a, a, 0);

		glTexCoord2f(dS + 2.0 * texA, dT + 2.0 * texA);
		glVertex3f(2.0 * a, 2.0 * a, 0);

		glTexCoord2f(dS + 3.0 * texA, dT + 2 * texA);
		glVertex3f(3.0 * a, 2.0 * a, 0);

		glTexCoord2f(dS + 4.0 * texA, dT + texA);
		glVertex3f(4.0 * a, a, 0);

		glTexCoord2f(dS + 4.0 * texA, dT);
		glVertex3f(4.0 * a, 0, 0);

		glTexCoord2f(dS + 5 * texA, dT);
		glVertex3f(5.0 * a, 0, 0);

		glTexCoord2f(dS + 5 * texA, dT + 3 * texA);
		glVertex3f(5.0 * a, 3.0 * a, 0);
	}
	glEnd();
}

void CGLRenderer::DrawCylinder(double h, double rTop, double rBottom, int nStep, double s, double t, double rTex)
{
	double dW = M_PI / (double)nStep,
		hHalf = h / 2,
		r = rBottom - rTop,
		L = sqrt(h * h + r * r),
		ny = r / L,
		nr = h / L;

	glBegin(GL_TRIANGLE_FAN);
	{
		glNormal3f(0, 1, 0);
		glTexCoord2f(s, t);
		glVertex3f(0, 0, 0);
		for (double i = 0; i < (2 * M_PI + dW); i += dW)
		{
			glTexCoord2f(rTex * sin(i) + s, rTex * cos(i) + t);
			glVertex3f(rTop * sin(i), hHalf, rTop * cos(i));
		}
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	{
		for (double i = 0; i < (2 * M_PI + dW); i += dW)
		{
			double x = sin(i),
				y = 1,
				z = cos(i);

			glNormal3f(nr * x, ny, nr * z);
			glVertex3f(rTop * x, hHalf, rTop * z);
			glVertex3f(rBottom * x, -hHalf, rBottom * z);
		}
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	{
		glNormal3f(0, -1, 0);
		glTexCoord2f(s, t);
		glVertex3f(0, 0, 0);
		for (double i = 0; i < (2 * M_PI + dW); i += dW)
		{
			glTexCoord2f(s + rTex * sin(i), t + rTex * cos(i));
			glVertex3f(rBottom * sin(i), -hHalf, rBottom * cos(i));
		}
	}
	glEnd();
}

void CGLRenderer::DrawSphere(double r, int nStepAlpha, int nStepBeta, double w, double h)
{
	double dAlpha = M_PI / nStepAlpha,
		dBeta = 2 * M_PI / nStepBeta,
		dS = w / nStepBeta,
		dT = h / nStepAlpha,
		sj = 0,
		ti = 0;

	for (double i = -M_PI_2; i < M_PI_2; i += dAlpha)
	{
		glBegin(GL_QUAD_STRIP);
		{
			sj = 0;
			for (double j = 0; j > -(2 * M_PI + dBeta); j -= dBeta)
			{
				double x1 = cos(i + dAlpha) * cos(j),
					y1 = sin(i + dAlpha),
					z1 = cos(i + dAlpha) * sin(j),
					x0 = cos(i) * cos(j),
					y0 = sin(i),
					z0 = cos(i) * sin(j);

				glNormal3f(x1, y1, z1);
				glTexCoord2f(sj, ti + dT);
				glVertex3f(r * x1, r * y1, r * z1);

				glNormal3f(x0, y0, z0);
				glTexCoord2f(sj, ti);
				glVertex3f(r * x0, r * y0, r * z0);

				sj += dS;
			}
		}
		glEnd();

		ti += dT;
	}
}

void CGLRenderer::DrawAxes(double len)
{
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	{
		glColor3f(1, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(len, 0, 0);

		glColor3f(0, 1, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, len, 0);

		glColor3f(0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, len);
	}
	glEnd();
	glEnable(GL_LIGHTING);
}

void CGLRenderer::CalculatePosition()
{
	double dWXY = this->viewAngleXY * M_PI / 180,
		dWXZ = this->viewAngleXZ * M_PI / 180;

	this->viewPosition[0] = this->viewR * cos(dWXY) * cos(dWXZ);
	this->viewPosition[1] = 0 + this->viewR * sin(dWXY);
	this->viewPosition[2] = this->viewR * cos(dWXY) * sin(dWXZ);

	this->upVector[1] = signbit(cos(dWXY)) ? -1 : 1;
}

void CGLRenderer::SetLighting()
{
	glEnable(GL_LIGHTING);

	// Direkciono svetlo 0
	float light_position_0[] = { 0, 1, 0, 0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position_0);

	glEnable(GL_LIGHT0);
}

void CGLRenderer::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	this->viewR += zDelta < 0 ? 1 : -1;
	this->CalculatePosition();
}

void CGLRenderer::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	double step = 5;
	switch (nChar)
	{
	case('W'):
		this->viewAngleXY += step;
		this->CalculatePosition();
		break;
	case('A'):
		this->viewAngleXZ += step;
		this->CalculatePosition();
		break;
	case('S'):
		this->viewAngleXY -= step;
		this->CalculatePosition();
		break;
	case('D'):
		this->viewAngleXZ -= step;
		this->CalculatePosition();
		break;
	case('C'):
		this->showAxes = !this->showAxes;
		break;
	default:
		break;
	}
}