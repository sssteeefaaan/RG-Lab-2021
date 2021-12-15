#include "pch.h"
#include "CGLRenderer.h"

CGLRenderer::CGLRenderer()
{
	this->viewR = 50;

	this->viewAngleXY = 0;
	this->viewAngleXZ = 0;

	this->lookingAt[0] = 0;
	this->lookingAt[1] = 50;
	this->lookingAt[2] = 0;

	this->upVector[0] = 0;
	this->upVector[1] = 1;
	this->upVector[2] = 0;

	this->showAxes =
		this->showNormals =
		this->redLight =
		this->greenLight =
		this->blueLight = true;

	this->CalculatePosition();

	this->room = new CGLMaterial(),
		this->pedestal = new CGLMaterial(),
		this->vase = new CGLMaterial(),
		this->light = new CGLMaterial();
}

CGLRenderer::~CGLRenderer()
{
	if (this->room)
	{
		delete this->room;
		this->room = nullptr;
	}

	if (this->pedestal)
	{
		delete this->pedestal;
		this->pedestal = nullptr;
	}

	if (this->vase)
	{
		delete this->vase;
		this->vase = nullptr;
	}

	if (this->light)
	{
		delete this->light;
		this->light = nullptr;
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

	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);

	this->room->SetAmbient(.65, .65, .65, 1);
	this->room->SetDiffuse(.65, .65, .65, 1);

	this->pedestal->SetAmbient(.65, .65, .65, 1);
	this->pedestal->SetDiffuse(.7, .7, .7, 1);

	this->vase->SetShininess(100);

	this->light->SetAmbient(0, 0, 0, 1);
	this->light->SetDiffuse(0, 0, 0, 1);
	this->light->SetSpecular(0, 0, 0, 1);
	this->light->SetShininess(112);

	// Globalno ambientno svetlo
	GLfloat l_model_ambient[] = { .3, .3, .3, 1 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, l_model_ambient);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	// Direkciono svetlo
	float light_ambient0[] = { .3, .3, .3, 1 };
	float light_diffuse0[] = { 1, 1, 1, 1 };
	float light_specular0[] = { .2, .2, .2, 1 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);

	// Crveno poziciono svetlo
	float light_ambient1[] = { .25, 0, 0, 1 };
	float light_diffuse1[] = { 1, 0, 0, 1 };
	float light_specular1[] = { .2, 0, 0, 1 };

	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);

	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 20);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, .1);

	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0);

	// Zeleno poziciono svetlo
	float light_ambient2[] = { 0, .25, 0, 1 };
	float light_diffuse2[] = { 0, 1, 0, 1 };
	float light_specular2[] = { 0, .2, 0, 1 };

	glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse2);
	glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular2);

	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 25);
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 2);

	glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1);
	glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0);
	glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0);

	// Plavo poziciono svetlo
	float light_ambient3[] = { 0, 0, .25, 1 };
	float light_diffuse3[] = { 0, 0, 1, 1 };
	float light_specular3[] = { 0, 0, .2, 1 };

	glLightfv(GL_LIGHT3, GL_AMBIENT, light_ambient3);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, light_diffuse3);
	glLightfv(GL_LIGHT3, GL_SPECULAR, light_specular3);

	glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 25);
	glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 1);

	glLightf(GL_LIGHT3, GL_CONSTANT_ATTENUATION, 1);
	glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION, 0);
	glLightf(GL_LIGHT3, GL_QUADRATIC_ATTENUATION, 0);

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
	gluPerspective(100, (double)w / (double)h, 1, 200);

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


	glEnable(GL_CULL_FACE);
	DrawRoom(100, 100, 100, 100);
	glDisable(GL_CULL_FACE);

	DrawPedestal(10, 8);
	DrawVase(20, 28.5 / 14, 12.175, 15, 16, this->showNormals);

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
	// 
	// 
	// 
	//--------------------------------
	wglMakeCurrent(NULL, NULL);

	if (m_hrc) {
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::DrawRoom(double l, double w, double h, int nStep)
{
	this->room->Select(GL_FRONT_AND_BACK);
	glPushMatrix();
	{
		glTranslatef(0, h / 2, 0);
		DrawCuboid(l, w, h, nStep, false, true);
	}
	glPopMatrix();
}

void CGLRenderer::DrawPedestal(double base, int nStep)
{
	this->pedestal->Select(GL_FRONT_AND_BACK);
	glPushMatrix();
	{
		DrawSphere(base / 1.1, nStep * 2, nStep * 4);

		glTranslatef(0, base, 0);
		DrawCylinder(base, base, base, nStep, false);

		glTranslatef(0, 3.0 / 4.0 * base, 0);
		DrawCuboid(3 * base, 3 * base, base / 2, nStep);
	}
	glPopMatrix();
}

void CGLRenderer::DrawVase(double from, double h, double rTop, double rBottom, int nStep, bool showNorms)
{
	double dR = rBottom - rTop;

	glPushMatrix();
	{
		glTranslatef(0, from, 0);

		glPushMatrix();
		{
			this->vase->SetAmbient(.3, .3, .475, 1);
			this->vase->SetDiffuse(.6, .6, .95, 1);
			this->vase->SetSpecular(.6, .6, .95, 1);
			this->vase->Select(GL_FRONT_AND_BACK);

			rTop = rBottom - dR;
			glTranslatef(0, h / 2, 0);
			DrawCylinder(h, rTop, rBottom, nStep, false, showNorms);

			rBottom = rTop - dR;
			rTop = rBottom;
			glTranslatef(0, 2 * h, 0);
			DrawCylinder(h, rTop, rBottom, nStep, false, showNorms);

			rTop = rBottom + dR;
			glTranslatef(0, 2 * h, 0);
			DrawCylinder(h, rTop, rBottom, nStep, false, showNorms);

			rBottom = rTop + dR;
			rTop = rBottom + dR;
			glTranslatef(0, 2 * h, 0);
			DrawCylinder(h, rTop, rBottom, nStep, false, showNorms);

			glTranslatef(0, 2 * h, 0);
			DrawCylinder(h, rTop, rBottom, nStep, false, showNorms);

			glTranslatef(0, 2 * h, 0);
			DrawCylinder(h, rTop, rBottom, nStep, false, showNorms);

			rTop = rBottom - dR;
			glTranslatef(0, 2 * h, 0);
			DrawCylinder(h, rTop, rBottom, nStep, false, showNorms);
		}
		glPopMatrix();


		glPushMatrix();
		{
			this->vase->SetAmbient(.225, .225, .475, 1);
			this->vase->SetDiffuse(.45, .45, .95, 1);
			this->vase->SetSpecular(.45, .45, .95, 1);
			this->vase->Select(GL_FRONT_AND_BACK);

			rBottom = rTop;
			rTop = rBottom - dR;
			glTranslatef(0, 3.0 / 2.0 * h, 0);
			DrawCylinder(h, rTop, rBottom, nStep, false, showNorms);

			rBottom = rTop;
			glTranslatef(0, 2 * h, 0);
			DrawCylinder(h, rTop, rBottom, nStep, false, showNorms);

			rBottom = rTop + dR;
			rTop = rBottom + dR;
			glTranslatef(0, 2 * h, 0);
			DrawCylinder(h, rTop, rBottom, nStep, false, showNorms);

			rBottom += 2 * dR;
			glTranslatef(0, 2 * h, 0);
			DrawCylinder(h, rTop, rBottom, nStep, false, showNorms);

			glTranslatef(0, 2 * h, 0);
			DrawCylinder(h, rTop, rBottom, nStep, false, showNorms);

			glTranslatef(0, 2 * h, 0);
			DrawCylinder(h, rTop, rBottom, nStep, false, showNorms);

			rBottom = rTop - dR;
			rTop = rBottom - dR;
			glTranslatef(0, 2 * h, 0);
			DrawCylinder(h, rTop, rBottom, nStep, false, showNorms);
		}
		glPopMatrix();
	}
	glPopMatrix();
}

void CGLRenderer::DrawLight(double x, double y, double z, double r, double nx, double ny, double nz, int nSize)
{
	int ind;
	float* vertexes = new float[(nSize + 2) * 3],
		step = 2 * M_PI / nSize;

	vertexes[0] = x;
	vertexes[1] = y;
	vertexes[2] = z;
	ind = 3;

	for (double i = 0; i < (2 * M_PI + step); i += step)
	{
		vertexes[ind++] = x + (nx != 0 ? 0 : r * sin(i));
		vertexes[ind++] = y + (ny != 0 ? 0 : r * cos(i));
		vertexes[ind++] = z + (nz != 0 ? 0 : r * cos(i));
	}

	glNormal3f(nx, ny, nz);
	glEnableClientState(GL_VERTEX_ARRAY);
	{
		glVertexPointer(3, GL_FLOAT, 0, vertexes);
		glDrawArrays(GL_TRIANGLE_FAN, 0, ind / 3);
	}
	glDisableClientState(GL_VERTEX_ARRAY);

	if (vertexes)
	{
		delete[] vertexes;
		vertexes = nullptr;
	}
}

void CGLRenderer::DrawCuboid(double l, double w, double h, int nStep, bool drawRoof, bool inverted)
{
	double lHalf = l / 2,
		wHalf = w / 2,
		hHalf = h / 2,
		lStep = l / (double)nStep,
		wStep = w / (double)nStep,
		hStep = h / (double)nStep;

	// Desna strana
	if (inverted)
	{
		glPushMatrix();
		glScalef(-1, 1, 1);
	}

	glNormal3f(inverted ? -1 : 1, 0, 0);
	for (double i = hHalf; i > -hHalf; i -= hStep)
	{
		glBegin(GL_QUAD_STRIP);
		{
			for (double j = wHalf; j > -(wHalf + wStep); j -= wStep)
			{
				glVertex3f(lHalf, i, j);
				glVertex3f(lHalf, i - hStep, j);
			}
		}
		glEnd();
	}

	// Leva strana
	glNormal3f(inverted ? 1 : -1, 0, 0);
	for (double i = hHalf; i > -hHalf; i -= hStep)
	{
		glBegin(GL_QUAD_STRIP);
		{
			for (double j = -wHalf; j < (wHalf + wStep); j += wStep)
			{
				glVertex3f(-lHalf, i, j);
				glVertex3f(-lHalf, i - hStep, j);
			}
		}
		glEnd();
	}

	if (inverted)
		glScalef(-1, -1, 1);

	if (drawRoof ^ inverted)
	{
		// Gornja strana
		glNormal3f(0, 0, 1);
		for (double i = -wHalf; i < wHalf; i += wStep)
		{
			glBegin(GL_QUAD_STRIP);
			{
				for (double j = -lHalf; j < (lHalf + lStep); j += lStep)
				{
					glVertex3f(j, hHalf, i);
					glVertex3f(j, hHalf, i + wStep);
				}
			}
			glEnd();
		}
	}

	if (!inverted || drawRoof)
	{
		// Donja strana
		glNormal3f(0, 0, -1);
		for (double i = wHalf; i > -wHalf; i -= wStep)
		{
			glBegin(GL_QUAD_STRIP);
			{
				for (double j = -lHalf; j < (lHalf + lStep); j += lStep)
				{
					glVertex3f(j, -hHalf, i);
					glVertex3f(j, -hHalf, i - wStep);
				}
			}
			glEnd();
		}
	}

	// Prednja strana
	if (inverted)
		glScalef(1, -1, -1);

	glNormal3f(0, 0, inverted ? -1 : 1);
	for (double i = hHalf; i > -hHalf; i -= hStep)
	{
		glBegin(GL_QUAD_STRIP);
		{
			for (double j = -lHalf; j < (lHalf + lStep); j += lStep)
			{
				glVertex3f(j, i, wHalf);
				glVertex3f(j, i - hStep, wHalf);
			}
		}
		glEnd();
	}

	// Zadnja strana
	glNormal3f(0, 0, inverted ? 1 : -1);
	for (double i = hHalf; i > -hHalf; i -= hStep)
	{
		glBegin(GL_QUAD_STRIP);
		{
			for (double j = lHalf; j > -(lHalf + lStep); j -= lStep)
			{
				glVertex3f(j, i, -wHalf);
				glVertex3f(j, i - hStep, -wHalf);
			}
		}
		glEnd();
	}

	if (inverted)
		glPopMatrix();
}

void CGLRenderer::DrawCylinder(double h, double rTop, double rBottom, int nStep, bool withBase, bool showNormals)
{
	double dW = M_PI / (180.0 / (double)nStep),
		hHalf = h / 2,
		rTopHalf = rTop / 2,
		rBottomHalf = rBottom / 2,
		r = rBottom - rTop,
		L = sqrt(h * h + r * r),
		ny = r / L,
		nr = h / L;

	long size = ((nStep + 1) * 3) << 2 + 12;
	float* vertNorm = new float[size];
	int counter = 0;

	// Normala vrha
	vertNorm[counter++] = 0;
	vertNorm[counter++] = 1;
	vertNorm[counter++] = 0;

	// Centar gornje osnove
	vertNorm[counter++] = 0;
	vertNorm[counter++] = hHalf;
	vertNorm[counter++] = 0;

	// Normala dna
	vertNorm[counter++] = 0;
	vertNorm[counter++] = -1;
	vertNorm[counter++] = 0;

	// Centar donje osnove
	vertNorm[counter++] = 0;
	vertNorm[counter++] = -hHalf;
	vertNorm[counter++] = 0;

	for (double i = 0; i < (2 * M_PI + dW); i += dW)
	{
		// Osnova normale
		vertNorm[counter++] = sin(i);
		vertNorm[counter++] = 1;
		vertNorm[counter++] = cos(i);

		// Gornje teme
		vertNorm[counter++] = rTopHalf * vertNorm[counter - 3];
		vertNorm[counter++] = hHalf;
		vertNorm[counter++] = rTopHalf * vertNorm[counter - 3];

		// Normala za donje teme
		vertNorm[counter++] = nr * vertNorm[counter - 6];
		vertNorm[counter++] = ny;
		vertNorm[counter++] = nr * vertNorm[counter - 6];

		// Donje teme
		vertNorm[counter++] = rBottomHalf * vertNorm[counter - 9];
		vertNorm[counter++] = -hHalf;
		vertNorm[counter++] = rBottomHalf * vertNorm[counter - 9];

		// Normala za gornje teme == normala za donje teme
		vertNorm[counter - 12] = vertNorm[counter - 6];
		vertNorm[counter - 11] = vertNorm[counter - 5];
		vertNorm[counter - 10] = vertNorm[counter - 4];
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(float) * 6, &vertNorm[15]);
	glNormalPointer(GL_FLOAT, sizeof(float) * 6, &vertNorm[12]);

	glDrawArrays(GL_QUAD_STRIP, 0, (counter - 2) / 6);

	if (withBase)
	{
		glVertexPointer(3, GL_FLOAT, sizeof(float) * 12, &vertNorm[3]);
		glNormalPointer(GL_FLOAT, sizeof(float) * 12, &vertNorm[0]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, (counter - 1) / 6);

		glVertexPointer(3, GL_FLOAT, sizeof(float) * 12, &vertNorm[9]);
		glNormalPointer(GL_FLOAT, sizeof(float) * 12, &vertNorm[6]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, (counter - 1) / 6);
	}

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	if (showNormals)
	{
		glDisable(GL_LIGHTING);
		glLineWidth(1.25);
		glColor3f(0, 1, 0);

		glEnableClientState(GL_VERTEX_ARRAY);

		for (int i = 12; i < counter; i += 6)
		{
			vertNorm[i] = vertNorm[i] * h + vertNorm[i + 3];
			vertNorm[i + 1] = vertNorm[i + 1] * h + vertNorm[i + 4];
			vertNorm[i + 2] = vertNorm[i + 2] * h + vertNorm[i + 5];
		}
		glVertexPointer(3, GL_FLOAT, 0, &vertNorm[12]);

		glDrawArrays(GL_LINES, 0, (counter - 2) / 3);

		glDisableClientState(GL_VERTEX_ARRAY);
		glEnable(GL_LIGHTING);
	}

	if (vertNorm)
	{
		delete[] vertNorm;
		vertNorm = nullptr;
	}
}

void CGLRenderer::DrawSphere(double r, int nStep1, int nStep2, int alphaMax, int betaMax)
{
	float aMax = alphaMax * M_PI / 180,
		bMax = betaMax * M_PI / 180,
		dAlpha = aMax / nStep1,
		dBeta = bMax / nStep2;

	long size = (nStep1 * nStep2 * 3) << 4;
	float* vertNorm = new float[size];
	int counter = 0;

	for (float i = 0; i < aMax; i += dAlpha)
	{
		for (float j = 0; j < bMax; j += dBeta)
		{
			vertNorm[counter++] = r * cos(i + dAlpha) * cos(j + dBeta);
			vertNorm[counter++] = r * sin(i + dAlpha);
			vertNorm[counter++] = r * cos(i + dAlpha) * sin(j + dBeta);

			vertNorm[counter++] = cos(i) * cos(j + dBeta);
			vertNorm[counter++] = sin(i);
			vertNorm[counter++] = cos(i) * sin(j + dBeta);

			vertNorm[counter++] = r * vertNorm[counter - 3];
			vertNorm[counter++] = r * vertNorm[counter - 3];
			vertNorm[counter++] = r * vertNorm[counter - 3];

			vertNorm[counter++] = vertNorm[counter - 6];
			vertNorm[counter++] = vertNorm[counter - 6];
			vertNorm[counter++] = vertNorm[counter - 6];

			vertNorm[counter++] = r * cos(i) * cos(j);
			vertNorm[counter++] = r * sin(i);
			vertNorm[counter++] = r * cos(i) * sin(j);

			vertNorm[counter++] = cos(i + dAlpha) * cos(j);
			vertNorm[counter++] = sin(i + dAlpha);
			vertNorm[counter++] = cos(i + dAlpha) * sin(j);

			vertNorm[counter++] = r * vertNorm[counter - 3];
			vertNorm[counter++] = r * vertNorm[counter - 3];
			vertNorm[counter++] = r * vertNorm[counter - 3];

			vertNorm[counter++] = vertNorm[counter - 6];
			vertNorm[counter++] = vertNorm[counter - 6];
			vertNorm[counter++] = vertNorm[counter - 6];
		}
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(float) * 6, &vertNorm[0]);
	glNormalPointer(GL_FLOAT, sizeof(float) * 6, &vertNorm[3]);
	glDrawArrays(GL_QUADS, 0, counter / 6);

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	if (vertNorm)
	{
		delete[] vertNorm;
		vertNorm = nullptr;
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
	this->viewPosition[1] = 50 + this->viewR * sin(dWXY);
	this->viewPosition[2] = this->viewR * cos(dWXY) * sin(dWXZ);

	this->upVector[1] = signbit(cos(dWXY)) ? -1 : 1;
}

void CGLRenderer::SetLighting()
{
	glEnable(GL_LIGHTING);

	// Direkciono svetlo
	float light_position[] = { .5, 1, .75, 0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glEnable(GL_LIGHT0);

	if (this->redLight)
	{
		// Crveno poziciono svetlo

		float light_spot_direction1[] = { 0, 0, -1, 1 };
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light_spot_direction1);

		float light_position_red[] = { 0, 50, 49, 1 };
		glLightfv(GL_LIGHT1, GL_POSITION, light_position_red);

		this->light->SetEmission(.8, 0, 0, 0);
		this->light->SetDiffuse(.8, 0, 0, 0);
		this->light->Select(GL_FRONT_AND_BACK);

		glEnable(GL_LIGHT1);

		glPushMatrix();
		glTranslatef(0, 50, 49);
		glRotatef(-90, 1, 0, 0);
		DrawSphere(2, 16, 16);
		glPopMatrix();
	}
	else
		glDisable(GL_LIGHT1);

	if (this->greenLight)
	{
		// Zeleno poziciono svetlo

		float light_spot_direction2[] = { 0, 0, 1, 1 };
		glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, light_spot_direction2);

		float light_position_green[] = { 0, 50, -49, 1 };
		glLightfv(GL_LIGHT2, GL_POSITION, light_position_green);

		this->light->SetEmission(0, .8, 0, 0);
		this->light->SetDiffuse(0, .8, 0, 0);
		this->light->Select(GL_FRONT_AND_BACK);

		glEnable(GL_LIGHT2);

		glPushMatrix();
		glTranslatef(0, 50, -49);
		glScalef(1, 1, -1);
		glRotatef(-90, 1, 0, 0);
		DrawSphere(2, 16, 16);
		glPopMatrix();
	}
	else
		glDisable(GL_LIGHT2);

	if (this->blueLight)
	{
		// Plavo poziciono svetlo

		float light_spot_direction3[] = { 0, -1, 0, 1 };
		glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, light_spot_direction3);

		float light_position_blue[] = { 0, 99, 0, 1 };
		glLightfv(GL_LIGHT3, GL_POSITION, light_position_blue);

		this->light->SetEmission(0, 0, .8, 0);
		this->light->SetDiffuse(0, 0, .8, 0);
		this->light->Select(GL_FRONT_AND_BACK);
		glEnable(GL_LIGHT3);

		glPushMatrix();
		glTranslatef(0, 99, 0);
		glScalef(1, -1, 1);
		DrawSphere(2, 16, 16);
		glPopMatrix();
	}
	else
		glDisable(GL_LIGHT3);
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
	case('N'):
		this->showNormals = !this->showNormals;
		break;
	case('R'):
		this->redLight = !this->redLight;
		break;
	case('G'):
		this->greenLight = !this->greenLight;
		break;
	case('B'):
		this->blueLight = !this->blueLight;
		break;
	default:
		break;
	}
}