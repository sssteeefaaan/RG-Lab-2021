#include "pch.h"
#include "CGLRenderer.h"

CGLRenderer::CGLRenderer()
{
	this->viewR = 50;

	this->viewAngleXY = 0;
	this->viewAngleXZ = 0;

	this->lookingAt[0] = 0;
	this->lookingAt[1] = 2;
	this->lookingAt[2] = 0;

	this->upVector[0] = 0;
	this->upVector[1] = 1;
	this->upVector[2] = 0;

	this->showAxes = true;

	this->CalculatePosition();

	this->truckMat = new CGLMaterial(),
		this->cargoMat = new CGLMaterial(),
		this->platformMat = new CGLMaterial();

	this->texAtlas = new CGLTexture(),
		texGrass = new CGLTexture();
}

CGLRenderer::~CGLRenderer()
{
	if (this->platformMat)
	{
		delete this->platformMat;
		this->platformMat = nullptr;
	}

	if (this->truckMat)
	{
		delete this->truckMat;
		this->truckMat = nullptr;
	}

	if (this->cargoMat)
	{
		delete this->cargoMat;
		this->cargoMat = nullptr;
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

	this->platformMat->SetAmbient(.55, .55, .55, 1);
	this->platformMat->SetDiffuse(.85, .85, .85, 1);

	this->truckMat->SetAmbient(.65, .65, .45, 1);
	this->truckMat->SetDiffuse(.65, .65, .45, 1);

	this->cargoMat->SetAmbient(.85, .85, .85, 1);
	this->cargoMat->SetDiffuse(1, 1, 1, 1);

	CGLTexture::PrepareTexturing(true);
	this->texAtlas->LoadFromFile((CString)"lab-6-atlas.jpg");
	this->texGrass->LoadFromFile((CString)"lab-6-grass.jpg", GL_REPEAT, GL_REPEAT);

	// Globalno ambientno svetlo
	GLfloat l_model_ambient[] = { .2, .2, .2, 1 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, l_model_ambient);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

	// Direkciono svetlo 
	float light_ambient_directional[] = { .35, .35, .35, 1 };
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

		//glEnable(GL_CULL_FACE);
		DrawGround(50, 50, 2, 2);
		//glDisable(GL_CULL_FACE);

		DrawTruck();
	}
	glPopMatrix();

	if (this->showAxes)
		DrawAxes(25);

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

void CGLRenderer::DrawGround(double width, double length, int repS, int repT)
{
	glEnable(GL_TEXTURE_2D);
	this->platformMat->Select();
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
	glEnable(GL_NORMALIZE);

	this->truckMat->Select();
	this->texAtlas->Select();

	glPushMatrix();
	{
		glTranslatef(0, 0, 3);
		DrawTruckBody(1, 1.0 / 16, 3);

		glTranslatef(0, 0, -6);
		glScalef(1, 1, -1);
		DrawTruckBody(1, 1.0 / 16, 3);
	}
	glPopMatrix();

	this->cargoMat->Select();

	glPushMatrix();
	{
		glTranslatef(2 * 1, 2 * 3.25, 0);
		DrawCargo(1, 3.5, 2, 3);
	}
	glPopMatrix();

	glDisable(GL_LIGHTING);
	glPushMatrix();
	{
		glRotatef(90, 1, 0, 0);
		DrawWheels(1, 1.5, -90);

		glScalef(1, -1, 1);
		DrawWheels(1, 1.5, 90);
	}
	glPopMatrix();
	glEnable(GL_LIGHTING);

	glDisable(GL_NORMALIZE);
	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawWheels(double h, double r, double angle)
{
	glPushMatrix();
	{
		glTranslatef(1.35 * r, 2 * h, -r);
		glRotatef(angle, 0, 1, 0);
		DrawCylinder(h, r, 8, 7.0 / 16, 1.5 / 16, 1.5 / 16);
	}
	glPopMatrix();

	glPushMatrix();
	{
		glTranslatef(-2 * r, 2 * h, -r);
		glRotatef(angle, 0, 1, 0);
		DrawCylinder(h, r, 8, 7.0 / 16, 1.5 / 16, 1.5 / 16);
	}
	glPopMatrix();
}

void CGLRenderer::DrawCargo(double r, double sx, double sy, double sz)
{
	glPushMatrix();
	{
		glScalef(sx, sy, sz);
		DrawSphere(r, 16, 16, 1, .5);
	}
	glPopMatrix();
}

void CGLRenderer::DrawTruckBody(double a, double texA, double width)
{
	glPushMatrix();
	{
		glTranslatef(-5.5 * a, 1.5 * a, 0);

		glNormal3f(0, 0, 1);

		glBegin(GL_POLYGON);
		{
			glTexCoord2f(4 * texA, 4 * texA);
			glVertex3f(4 * a, 3 * a, 0);

			glTexCoord2f(4 * texA, 0);
			glVertex3f(4 * a, 7 * a, 0);

			glTexCoord2f(2 * texA, 0);
			glVertex3f(2 * a, 7 * a, 0);

			glTexCoord2f(0, 2 * texA);
			glVertex3f(0, 5 * a, 0);

			glTexCoord2f(0, 4 * texA);
			glVertex3f(0, 3 * a, 0);
		}
		glEnd();

		glBegin(GL_QUADS);
		{
			glTexCoord2f(5 * texA, 5 * texA);
			glVertex3f(5 * a, 2 * a, 0);

			glTexCoord2f(5 * texA, 4 * texA);
			glVertex3f(5 * a, 3 * a, 0);

			glTexCoord2f(0, 4 * texA);
			glVertex3f(0, 3 * a, 0);

			glTexCoord2f(0, 5 * texA);
			glVertex3f(0, 2 * a, 0);


			glTexCoord2f(texA, 7 * texA);
			glVertex3f(a, 0, 0);

			glTexCoord2f(texA, 5 * texA);
			glVertex3f(a, 2 * a, 0);

			glTexCoord2f(0, 5 * texA);
			glVertex3f(0, 2 * a, 0);

			glTexCoord2f(0, 7 * texA);
			glVertex3f(0, 0, 0);


			glTexCoord2f(5 * texA, 7 * texA);
			glVertex3f(5 * a, 0, 0);

			glTexCoord2f( 5 * texA, 5 * texA);
			glVertex3f(5 * a, 2 * a, 0);

			glTexCoord2f(4 * texA, 5 * texA);
			glVertex3f(4 * a, 2 * a, 0);

			glTexCoord2f(4 * texA, 7 * texA);
			glVertex3f(4 * a, 0, 0);


			// Drugi deo
			glTexCoord2f(11 * texA, 5 * texA);
			glVertex3f(11 * a, 2 * a, 0);

			glTexCoord2f(11 * texA, 4 * texA);
			glVertex3f(11 * a, 3 * a, 0);

			glTexCoord2f(5 * texA, 4 * texA);
			glVertex3f(5 * a, 3 * a, 0);

			glTexCoord2f(5 * texA, 5 * texA);
			glVertex3f(5 * a, 2 * a, 0);


			glTexCoord2f(6 * texA, 7 * texA);
			glVertex3f(6 * a, 0, 0);

			glTexCoord2f(6 * texA, 5 * texA);
			glVertex3f(6 * a, 2 * a, 0);

			glTexCoord2f(5 * texA, 5 * texA);
			glVertex3f(5 * a, 2 * a, 0);

			glTexCoord2f(5 * texA, 7 * texA);
			glVertex3f(5 * a, 0, 0);


			glTexCoord2f(11 * texA, 7 * texA);
			glVertex3f(11 * a, 0, 0);

			glTexCoord2f(11 * texA, 5 * texA);
			glVertex3f(11 * a, 2 * a, 0);

			glTexCoord2f(9 * texA, 5 * texA);
			glVertex3f(9 * a, 2 * a, 0);

			glTexCoord2f(9 * texA, 7 * texA);
			glVertex3f(9 * a, 0, 0);
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

			// Drugi deo
			glTexCoord2f(7 * texA, 4 * texA);
			glVertex3f(7 * a, 2 * a, 0);

			glTexCoord2f(6 * texA, 4 * texA);
			glVertex3f(6 * a, 2 * a, 0);

			glTexCoord2f(6 * texA, 5 * texA);
			glVertex3f(6 * a, a, 0);


			glTexCoord2f(9 * texA, 5 * texA);
			glVertex3f(9 * a, a, 0);

			glTexCoord2f(9 * texA, 4 * texA);
			glVertex3f(9 * a, 2 * a, 0);

			glTexCoord2f(8 * texA, 4 * texA);
			glVertex3f(8 * a, 2 * a, 0);
		}
		glEnd();


		// Bez teksture
		glDisable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		{
			// Front of the car
			glNormal3f(-1, 0, 0);

			glVertex3f(0, 0, -width);
			glVertex3f(0, 0, 0);

			glVertex3f(0, 5 * a, 0);
			glVertex3f(0, 5 * a, -width);


			// Wind shield
			glNormal3f(-M_SQRT1_2, M_SQRT1_2, 0);

			glVertex3f(0, 5 * a, -width);
			glVertex3f(0, 5 * a, 0);

			glVertex3f(2 * a, 7 * a, 0);
			glVertex3f(2 * a, 7 * a, -width);

			// Roof
			glNormal3f(0, 1, 0);

			glVertex3f(2 * a, 7 * a, -width);
			glVertex3f(2 * a, 7 * a, 0);

			glVertex3f(4 * a, 7 * a, 0);
			glVertex3f(4 * a, 7 * a, -width);


			// Back shield
			glNormal3f(1, 0, 0);

			glVertex3f(4 * a, 7 * a, -width);
			glVertex3f(4 * a, 7 * a, 0);

			glVertex3f(4 * a, 3 * a, 0);
			glVertex3f(4 * a, 3 * a, -width);

			// Propshaft
			glNormal3f(0, 1, 0);

			glVertex3f(4 * a, 3 * a, -width);
			glVertex3f(4 * a, 3 * a, 0);

			glVertex3f(11 * a, 3 * a, 0);
			glVertex3f(11 * a, 3 * a, -width);

			// Back
			glNormal3f(1, 0, 0);

			glVertex3f(11 * a, 3 * a, -width);
			glVertex3f(11 * a, 3 * a, 0);

			glVertex3f(11 * a, 0, 0);
			glVertex3f(11 * a, 0, -width);

			// Bottom - back
			glNormal3f(0, -1, 0);

			glVertex3f(11 * a, 0, -width);
			glVertex3f(11 * a, 0, 0);
			glVertex3f(9 * a, 0, 0);
			glVertex3f(9 * a, 0, -width);

			glVertex3f(8 * a, 2 * a, -width);
			glVertex3f(8 * a, 2 * a, 0);
			glVertex3f(7 * a, 2 * a, 0);
			glVertex3f(7 * a, 2 * a, -width);

			glVertex3f(6 * a, 0, -width);
			glVertex3f(6 * a, 0, 0);
			glVertex3f(4 * a, 0, 0);
			glVertex3f(4 * a, 0, -width);

			glVertex3f(3 * a, 2 * a, -width);
			glVertex3f(3 * a, 2 * a, 0);
			glVertex3f(2 * a, 2 * a, 0);
			glVertex3f(2 * a, 2 * a, -width);

			glVertex3f(a, 0, -width);
			glVertex3f(a, 0, 0);
			glVertex3f(0, 0, 0);
			glVertex3f(0, 0, -width);

			glNormal3f(-M_SQRT1_2, -M_SQRT1_2, 0);

			glVertex3f(9 * a, a, -width);
			glVertex3f(9 * a, a, 0);
			glVertex3f(8 * a, 2 * a, 0);
			glVertex3f(8 * a, 2 * a, -width);

			glVertex3f(4 * a, a, -width);
			glVertex3f(4 * a, a, 0);
			glVertex3f(3 * a, 2 * a, 0);
			glVertex3f(3 * a, 2 * a, -width);


			glNormal3f(M_SQRT1_2, -M_SQRT1_2, 0);

			glVertex3f(7 * a, 2 * a, -width);
			glVertex3f(7 * a, 2 * a, 0);
			glVertex3f(6 * a, a, 0);
			glVertex3f(6 * a, a, -width);

			glVertex3f(2 * a, 2 * a, -width);
			glVertex3f(2 * a, 2 * a, 0);
			glVertex3f(a, a, 0);
			glVertex3f(a, a, -width);

			glNormal3f(-1, 0, 0);

			glVertex3f(9 * a, 0, -width);
			glVertex3f(9 * a, 0, 0);
			glVertex3f(9 * a, a, 0);
			glVertex3f(9 * a, a, -width);

			glVertex3f(4 * a, 0, -width);
			glVertex3f(4 * a, 0, 0);
			glVertex3f(4 * a, a, 0);
			glVertex3f(4 * a, a, -width);

			glNormal3f(1, 0, 0);

			glVertex3f(6 * a, 0, -width);
			glVertex3f(6 * a, 0, 0);
			glVertex3f(6 * a, a, 0);
			glVertex3f(6 * a, a, -width);

			glVertex3f(a, 0, -width);
			glVertex3f(a, 0, 0);
			glVertex3f(a, a, 0);
			glVertex3f(a, a, -width);
		}
		glEnd();
		glEnable(GL_TEXTURE_2D);
	}
	glPopMatrix();
}

void CGLRenderer::DrawCylinder(double h, double r, int nStep, double s, double t, double rTex)
{
	double dW = M_PI / (double)nStep,
		hHalf = h / 2;

	glColor3f(1, 1, 1);
	glBegin(GL_TRIANGLE_FAN);
	{
		glNormal3f(0, 1, 0);
		glTexCoord2f(s, t);
		glVertex3f(0, 0, 0);
		for (double i = 0; i < (2 * M_PI + dW); i += dW)
		{
			glTexCoord2f(rTex * sin(i) + s, rTex * cos(i) + t);
			glVertex3f(r * sin(i), hHalf, r * cos(i));
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
			glVertex3f(r * sin(i), -hHalf, r * cos(i));
		}
	}
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glColor3f(.75, .75, .55);
	glBegin(GL_QUAD_STRIP);
	{
		for (double i = 0; i < (2 * M_PI + dW); i += dW)
		{
			double x = sin(i),
				y = 1,
				z = cos(i);

			glNormal3f(x, 0, z);
			glVertex3f(r * x, hHalf, r * z);
			glVertex3f(r * x, -hHalf, r * z);
		}
	}
	glEnd();
	glEnable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawSphere(double r, int nStepAlpha, int nStepBeta, double w, double h, double offset)
{
	double dAlpha = M_PI / nStepAlpha,
		dBeta = 2 * M_PI / nStepBeta,
		dS = w / nStepBeta,
		dT = h / nStepAlpha,
		sj = 0,
		ti = 1,
		fi = offset * M_PI / 180;

	for (double i = -M_PI_2; i < M_PI_2; i += dAlpha)
	{
		glBegin(GL_QUAD_STRIP);
		{
			sj = 0;
			for (double j = fi; j > -(fi + M_PI + dBeta); j -= dBeta)
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

	// Direkciono svetlo
	float light_position[] = { .5, 1, 1, 0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

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