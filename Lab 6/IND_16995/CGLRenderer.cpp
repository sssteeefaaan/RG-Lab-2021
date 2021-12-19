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

	this->truck->SetAmbient(1, 1, 1, 1);
	this->truck->SetDiffuse(1, 1, 1, 1);

	CGLTexture::PrepareTexturing(true);
	this->texAtlas->LoadFromFile((CString)"lab-6-atlas.jpg");
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
		glTranslatef(0, -1.5, 0);
		DrawPlatform(50, 50);
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

		glTexCoord2f(repS, repT);
		glVertex3f(width / 2, 0, length / 2);

		glTexCoord2f(repS, 0);
		glVertex3f(width / 2, 0, -length / 2);

		glTexCoord2f(0, 0);
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
		glTranslatef(0, 0, 2);
		DrawTruckBody(1, 1.0 / 16);

		glTranslatef(0, 0, -4);
		DrawTruckBody(1, 1.0 / 16);
	}
	glPopMatrix();

	DrawWheels();
	DrawCargo();

	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawWheels()
{
	glPushMatrix();
	{
		glTranslatef(0, 1.5, 0);
		glRotatef(90, 1, 0, 0);

		glPushMatrix();
		{
			glTranslatef(1, 1, 0);
			DrawCylinder(1, 1.5, 1.5, 8, 10.0 / 16, 1.5 / 16, 1.5 / 16);
		}
		glPopMatrix();

		glPushMatrix();
		{
			glTranslatef(-3, 1, 0);
			DrawCylinder(1, 1.5, 1.5, 8, 10.0 / 16, 1.5 / 16, 1.5 / 16);
		}
		glPopMatrix();

		glRotatef(-180, 1, 0, 0);

		glPushMatrix();
		{
			glTranslatef(1, 1, 0);
			DrawCylinder(1, 1.5, 1.5, 8, 10.0 / 16, 1.5 / 16, 1.5 / 16);
		}
		glPopMatrix();

		glPushMatrix();
		{
			glTranslatef(-3, 1, 0);
			DrawCylinder(1, 1.5, 1.5, 8, 10.0 / 16, 1.5 / 16, 1.5 / 16);
		}
		glPopMatrix();
	}
	glPopMatrix();
}

void CGLRenderer::DrawCargo()
{
	glPushMatrix();
	{
		glTranslatef(1, 5.5, 0);
		glScalef(2, 1.5, 2);
		DrawSphere(1, 16, 16, 1, .5);
	}
	glPopMatrix();
}

void CGLRenderer::DrawTruckBody(double a, double texA)
{
	glPushMatrix();
	{
		glTranslatef(-5.5, 1.5, 0);

		glNormal3f(0, 0, 1);

		glBegin(GL_POLYGON);
		{
			glTexCoord2f(4 * texA, 3 * texA);
			glVertex3f(4 * a, 3 * a, 0);

			glTexCoord2f(4 * texA, 0);
			glVertex3f(4 * a, 6 * a, 0);

			glTexCoord2f(2 * texA, 0);
			glVertex3f(2 * a, 6 * a, 0);

			glTexCoord2f(0, 2 * texA);
			glVertex3f(0, 4 * a, 0);

			glTexCoord2f(0, 3 * texA);
			glVertex3f(0, 3 * a, 0);
		}
		glEnd();

		glBegin(GL_QUADS);
		{
			glTexCoord2f(4 * texA, 4 * texA);
			glVertex3f(4 * a, 2 * a, 0);

			glTexCoord2f(4 * texA, 3 * texA);
			glVertex3f(4 * a, 3 * a, 0);

			glTexCoord2f(0, 3 * texA);
			glVertex3f(0, 3 * a, 0);

			glTexCoord2f(0, 4 * texA);
			glVertex3f(0, 2 * a, 0);


			glTexCoord2f(texA, 6 * texA);
			glVertex3f(a, 0, 0);

			glTexCoord2f(texA, 4 * texA);
			glVertex3f(a, 2*a, 0);

			glTexCoord2f(0, 4 * texA);
			glVertex3f(0, 2 * a, 0);

			glTexCoord2f(0, 6 * texA);
			glVertex3f(0, 0, 0);
		}
		glEnd();

		glBegin(GL_TRIANGLES);
		{
			glTexCoord2f(2 * texA, 4 * texA);
			glVertex3f(2 * a, 2 * a, 0);

			glTexCoord2f(texA, 4 * texA);
			glVertex3f(a, 2 * a, 0);

			glTexCoord2f(texA, 5 * texA);
			glVertex3f(a, a, 0);


			glTexCoord2f(4 * texA, 5 * texA);
			glVertex3f(4 * a, a, 0);

			glTexCoord2f(4 * texA, 4 * texA);
			glVertex3f(4 * a, 2 * a, 0);

			glTexCoord2f(3 * texA, 4 * texA);
			glVertex3f(3 * a, 2 * a, 0);
		}
		glEnd();


		glBegin(GL_QUADS);
		{
			glTexCoord2f(8 * texA, 4 * texA);
			glVertex3f(8 * a, 2 * a, 0);

			glTexCoord2f(8 * texA, 3 * texA);
			glVertex3f(8 * a, 3 * a, 0);

			glTexCoord2f(4 * texA, 3 * texA);
			glVertex3f(4 * a, 3 * a, 0);

			glTexCoord2f(4 * texA, 4 * texA);
			glVertex3f(4 * a, 2 * a, 0);


			glTexCoord2f(5 * texA, 6 * texA);
			glVertex3f(5 * a, 0, 0);

			glTexCoord2f(5 * texA, 4 * texA);
			glVertex3f(5 * a, 2 * a, 0);

			glTexCoord2f(4 * texA, 4 * texA);
			glVertex3f(4 * a, 2 * a, 0);

			glTexCoord2f(4 * texA, 6 * texA);
			glVertex3f(4 * a, 0, 0);
		}
		glEnd();

		glBegin(GL_TRIANGLES);
		{
			glTexCoord2f(6 * texA, 4 * texA);
			glVertex3f(6 * a, 2 * a, 0);

			glTexCoord2f(5 * texA, 4 * texA);
			glVertex3f(5 * a, 2 * a, 0);

			glTexCoord2f(5 * texA, 5 * texA);
			glVertex3f(5 * a, a, 0);


			glTexCoord2f(8 * texA, 5 * texA);
			glVertex3f(8 * a, a, 0);

			glTexCoord2f(8 * texA, 4 * texA);
			glVertex3f(8 * a, 2 * a, 0);

			glTexCoord2f(7 * texA, 4 * texA);
			glVertex3f(7 * a, 2 * a, 0);
		}
		glEnd();

		glBegin(GL_QUADS);
		{
			glTexCoord2f(8 * texA, 6 * texA);
			glVertex3f(8 * a, 0, 0);

			glTexCoord2f(8 * texA, 3 * texA);
			glVertex3f(8 * a, 3 * a, 0);

			glTexCoord2f(11 * texA, 3 * texA);
			glVertex3f(11 * a, 3 * a, 0);

			glTexCoord2f(11 * texA, 6 * texA);
			glVertex3f(11 * a, 0, 0);
		}
		glEnd();
	}
	glPopMatrix();
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
		ti = 1;

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
				glTexCoord2f(sj, ti - dT);
				glVertex3f(r * x1, r * y1, r * z1);

				glNormal3f(x0, y0, z0);
				glTexCoord2f(sj, ti);
				glVertex3f(r * x0, r * y0, r * z0);

				sj += dS;
			}
		}
		glEnd();

		ti -= dT;
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