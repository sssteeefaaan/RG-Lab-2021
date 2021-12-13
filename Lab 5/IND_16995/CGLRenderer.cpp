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
		this->showNormals = true;

	this->CalculatePosition();

	this->room = new CGLMaterial(),
		this->pedestal = new CGLMaterial(),
		this->vaseDark = new CGLMaterial(),
		this->vaseLight = new CGLMaterial(),
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

	if (this->vaseDark)
	{
		delete this->vaseDark;
		this->vaseDark = nullptr;
	}
	if (this->vaseLight)
	{
		delete this->vaseLight;
		this->vaseLight = nullptr;
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

	glClearColor(0, 0, 0, 0);
	glEnable(GL_DEPTH_TEST);

	this->room->SetAmbient(.4, .4, .4, 1);
	this->room->SetDiffuse(.6, .6, .6, 1);

	this->pedestal->SetAmbient(.45, .45, .45, 1);
	this->pedestal->SetDiffuse(.7, .7, .7, 1);

	this->vaseLight->SetAmbient(.3, .3, .5, 1);
	this->vaseLight->SetDiffuse(.4, .4, .9, 1);
	this->vaseLight->SetSpecular(.4, .4, .9, 1);
	this->vaseLight->SetShininess(45);

	this->vaseDark->SetAmbient(.2, .2, .4, 1);
	this->vaseDark->SetDiffuse(.3, .3, .8, 1);
	this->vaseDark->SetSpecular(.3, .3, .8, 1);
	this->vaseDark->SetShininess(45);

	this->light->SetAmbient(.5, .5, .5, 1);
	//this->light->SetDiffuse(1, 1, 1, 1);
	this->light->SetSpecular(1, 1, 1, 1);
	this->light->SetShininess(50);

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
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	glLoadIdentity();

	gluLookAt(this->viewPosition[0], this->viewPosition[1], this->viewPosition[2],
		this->lookingAt[0], this->lookingAt[1], this->lookingAt[2],
		this->upVector[0], this->upVector[1], this->upVector[2]);

	SetLighting();

	DrawRoom(100, 100, 100, 50);
	DrawPedestal(10, 8);
	DrawVase(22.5, 28 / 14, 12.175, 15, 16);

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


	//--------------------------------
	wglMakeCurrent(NULL, NULL);

	if (m_hrc) {
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
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
		this->viewAngleXY = min(this->viewAngleXY + step, 89.9);
		this->CalculatePosition();
		break;
	case('A'):
		this->viewAngleXZ += step;
		this->CalculatePosition();
		break;
	case('S'):
		this->viewAngleXY = max(this->viewAngleXY - step, -89.9);
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

void CGLRenderer::CalculatePosition()
{
	double dWXY = this->viewAngleXY * M_PI / 180,
		dWXZ = this->viewAngleXZ * M_PI / 180;

	this->viewPosition[0] = this->viewR * cos(dWXY) * cos(dWXZ);
	this->viewPosition[1] = 50+ this->viewR * sin(dWXY);
	this->viewPosition[2] = this->viewR * cos(dWXY) * sin(dWXZ);
}

void CGLRenderer::DrawRoom(double l, double w, double h, int nStep)
{
	this->room->Select(GL_FRONT);
	glPushMatrix();
	{
		glTranslatef(0, h/2, 0);
		DrawCuboid(l, w, h, nStep, true);
	}
	glPopMatrix();
}
void CGLRenderer::DrawPedestal(double base, int nStep)
{
	this->pedestal->Select(GL_FRONT);
	glPushMatrix();
	{
		DrawSphere(base, nStep * 3, nStep * 3);

		glTranslatef(0, base, 0);
		DrawCylinder(2 * base, base, base, nStep, false);

		glTranslatef(0, base * (5 / 4), 0);
		DrawCuboid(3 * base, 3 * base, base / 2, nStep, false);
	}
	glPopMatrix();
}
void CGLRenderer::DrawVase(double from, double h, double rTop, double rBottom, int nStep)
{
	double dR = rBottom - rTop;

	glPushMatrix();
	{
		glTranslatef(0, from, 0);

		this->vaseLight->Select(GL_FRONT);
		rTop = rBottom - dR;
		glTranslatef(0, h/2, 0);
		DrawCylinder(h, rTop, rBottom, nStep, false, this->showNormals);

		this->vaseDark->Select(GL_FRONT);
		rBottom = rTop;
		rTop = rBottom - dR;
		glTranslatef(0, h, 0);
		DrawCylinder(h, rTop, rBottom, nStep, false, this->showNormals);

		this->vaseLight->Select(GL_FRONT);
		rBottom = rTop;
		glTranslatef(0, h, 0);
		DrawCylinder(h, rTop, rBottom, nStep, false, this->showNormals);

		this->vaseDark->Select(GL_FRONT);
		glTranslatef(0, h, 0);
		DrawCylinder(h, rTop, rBottom, nStep, false, this->showNormals);

		this->vaseLight->Select(GL_FRONT);
		rTop = rBottom + dR;
		glTranslatef(0, h, 0);
		DrawCylinder(h, rTop, rBottom, nStep, false, this->showNormals);

		this->vaseDark->Select(GL_FRONT);
		rBottom = rTop;
		rTop = rBottom + dR;
		glTranslatef(0, h, 0);
		DrawCylinder(h, rTop, rBottom, nStep, false, this->showNormals);


		this->vaseLight->Select(GL_FRONT);
		rBottom = rTop;
		rTop = rBottom + dR;
		glTranslatef(0, h, 0);
		DrawCylinder(h, rTop, rBottom, nStep, false, this->showNormals);

		this->vaseDark->Select(GL_FRONT);
		rBottom = rTop;
		rTop = rBottom - dR;
		glTranslatef(0, h, 0);
		DrawCylinder(h, rTop, rBottom, nStep, false, this->showNormals);

		this->vaseLight->Select(GL_FRONT);
		rBottom = rTop;
		rTop = rBottom + dR;
		glTranslatef(0, h, 0);
		DrawCylinder(h, rTop, rBottom, nStep, false, this->showNormals);

		this->vaseDark->Select(GL_FRONT);
		rBottom = rTop;
		rTop = rBottom - dR;
		glTranslatef(0, h, 0);
		DrawCylinder(h, rTop, rBottom, nStep, false, this->showNormals);

		this->vaseLight->Select(GL_FRONT);
		rBottom = rTop;
		rTop = rBottom + dR;
		glTranslatef(0, h, 0);
		DrawCylinder(h, rTop, rBottom, nStep, false, this->showNormals);

		this->vaseDark->Select(GL_FRONT);
		rBottom = rTop;
		rTop = rBottom - dR;
		glTranslatef(0, h, 0);
		DrawCylinder(h, rTop, rBottom, nStep, false, this->showNormals);



		this->vaseLight->Select(GL_FRONT);
		rBottom = rTop;
		rTop = rBottom - dR;
		glTranslatef(0, h, 0);
		DrawCylinder(h, rTop, rBottom, nStep, false, this->showNormals);

		this->vaseDark->Select(GL_FRONT);
		rBottom = rTop;
		rTop = rBottom - dR;
		glTranslatef(0, h, 0);
		DrawCylinder(h, rTop, rBottom, nStep, false, this->showNormals);
	}
	glPopMatrix();
}
void CGLRenderer::SetLighting()
{
	// Globalno ambientno svetlo
	GLfloat l_model_ambient[] = { .5, .5, .5, 1 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, l_model_ambient);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

	glEnable(GL_LIGHTING);

	// Direkciono svetlo
	float light_ambient0[] = { .3, .3, .3, 1 };
	float light_diffuse0[] = { 1, 1, 1, 1 };
	float light_specular0[] = { 1, 1, 1, 1 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);

	float light_position[] = { .5, 1, .75, 0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glEnable(GL_LIGHT0);

	if (this->redLight)
	{
		float light_ambient1[] = { .3, 0, 0, 1 };
		float light_diffuse1[] = { 1, 0, 0, 1 };
		float light_specular1[] = { 1, 0, 0, 1 };

		glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient1);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
		glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);

		glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 20);
		glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 0);

		glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1);
		glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0);
		glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0);

		float light_spot_direction1[] = { 0, 0, -1, 1 };
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light_spot_direction1);

		float light_position_red[] = { 0, 50, 49, 1 };
		glLightfv(GL_LIGHT1, GL_POSITION, light_position_red);

		this->light->SetEmission(1, 0, 0, 0);
		this->light->Select(GL_FRONT_AND_BACK);
		glBegin(GL_TRIANGLE_FAN);
		{
			glVertex3f(0, 50, 48);
			for (double i = 0; i < (2 * M_PI + .25); i += .25)
				glVertex3f(2 * sin(i), 50 + 2 * cos(i), 48);
		}
		glEnd();

		glEnable(GL_LIGHT1);
	}
	else
		glDisable(GL_LIGHT1);

	if (this->greenLight)
	{
		float light_ambient2[] = { 0, .3, 0, 1 };
		float light_diffuse2[] = { 0, 1, 0, 1 };
		float light_specular2[] = { 0, 1, 0, 1 };

		glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient2);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse2);
		glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular2);

		glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 20);
		glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 0);

		glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1);
		glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0);
		glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0);

		float light_spot_direction2[] = { 0, 0, 1, 1 };
		glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, light_spot_direction2);

		float light_position_green[] = { 0, 50, -49, 1 };
		glLightfv(GL_LIGHT2, GL_POSITION, light_position_green);

		this->light->SetEmission(0, 1, 0, 0);
		this->light->Select(GL_FRONT_AND_BACK);
		glBegin(GL_TRIANGLE_FAN);
		{
			glVertex3f(0, 50, -48);
			for (double i = 0; i < (2 * M_PI + .25); i += .25)
				glVertex3f(2 * sin(i), 50 + 2 * cos(i), -48);
		}
		glEnd();

		glEnable(GL_LIGHT2);
	}
	else
		glDisable(GL_LIGHT2);

	if (this->blueLight)
	{
		float light_ambient3[] = { 0, 0, .2, 1 };
		float light_diffuse3[] = { 0, 0, 1, 1 };
		float light_specular3[] = { 0, 0, 1, 1 };

		glLightfv(GL_LIGHT3, GL_AMBIENT, light_ambient3);
		glLightfv(GL_LIGHT3, GL_DIFFUSE, light_diffuse3);
		glLightfv(GL_LIGHT3, GL_SPECULAR, light_specular3);

		glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 20);
		glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 0);

		glLightf(GL_LIGHT3, GL_CONSTANT_ATTENUATION, 1);
		glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION, 0);
		glLightf(GL_LIGHT3, GL_QUADRATIC_ATTENUATION, 0);

		float light_spot_direction3[] = { 0, -1, 0, 1 };
		glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, light_spot_direction3);

		float light_position_blue[] = { 0, 98, 0, 1 };
		glLightfv(GL_LIGHT3, GL_POSITION, light_position_blue);

		this->light->SetEmission(0, 0, 1, 0);
		this->light->Select(GL_FRONT_AND_BACK);
		glBegin(GL_TRIANGLE_FAN);
		{
			glVertex3f(0, 98, 0);
			for(double i = 0; i < (2*M_PI + .25); i+=.25)
				glVertex3f(2 * sin(i), 99, 2 * cos(i));
		}
		glEnd();

		glEnable(GL_LIGHT3);
	}
	else
		glDisable(GL_LIGHT3);
}

void CGLRenderer::DrawCuboid(double l, double w, double h, int nStep, bool inverted)
{
	double lHalf = l / 2,
		wHalf = w / 2,
		hHalf = h / 2,
		lStep = l / (double)nStep,
		wStep = w / (double)nStep,
		hStep = h / (double)nStep;

	glBegin(GL_QUAD_STRIP);
	{
		glNormal3f(inverted ? -1 : 1, 0, 0);
		for (double i = hHalf; i > -hHalf; i -= hStep)
		{
			for (double j = wHalf; j > -(wHalf + wStep); j -= wStep)
			{
				glVertex3f(lHalf, i, j);
				glVertex3f(lHalf, i - hStep, j);
			}
		}
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	{
		glNormal3f(inverted ? 1 : -1, 0, 0);
		for (double i = hHalf; i > -hHalf; i -= hStep)
		{
			for (double j = -wHalf; j < (wHalf + wStep); j += wStep)
			{
				glVertex3f(-lHalf, i, j);
				glVertex3f(-lHalf, i - hStep, j);
			}
		}
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	{
		glNormal3f(0, inverted ? -1 : 1, 0);
		for (double i = -wHalf; i < wHalf; i += wStep)
		{
			for (double j = -lHalf; j < (lHalf + lStep); j += lStep)
			{
				glVertex3f(j, hHalf, i);
				glVertex3f(j, hHalf, i + wStep);
			}
		}
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	{
		glNormal3f(0, inverted ? 1 : -1, 0);
		for (double i = wHalf; i > -wHalf; i -= wStep)
		{
			for (double j = -lHalf; j < (lHalf + lStep); j += lStep)
			{
				glVertex3f(j, -hHalf, i);
				glVertex3f(j, -hHalf, i - wStep);
			}
		}
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	{
		glNormal3f(0, 0, inverted ? -1 : 1);
		for (double i = -lHalf; i < lHalf; i += lStep)
		{
			for (double j = hHalf; j > -(hHalf + hStep); j -= hStep)
			{
				glVertex3f(i, j, wHalf);
				glVertex3f(i + lStep, j, wHalf);
			}
		}
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	{
		glNormal3f(0, 0, inverted ? 1 : -1);
		for (double i = lHalf; i > -lHalf; i -= lStep)
		{
			for (double j = hHalf; j > -(hHalf + hStep); j -= hStep)
			{
				glVertex3f(i - lStep, j, -wHalf);
				glVertex3f(i, j, -wHalf);
			}
		}
	}
	glEnd();
}
void CGLRenderer::DrawCone(double h, double r, int nStep)
{

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

	glBegin(GL_QUAD_STRIP);
	{
		for (double i = 0; i < (2 * M_PI + dW); i += dW)
		{
			double x = sin(i),
				z = cos(i);

			glNormal3f(nr*x, ny, nr*z);
			glVertex3f(rTopHalf * x, hHalf, rTopHalf * z);
			glVertex3f(rBottomHalf * x, -hHalf, rBottomHalf * z);
		}
	}
	glEnd();

	if (withBase)
	{
		glBegin(GL_TRIANGLE_FAN);
		{
			glNormal3f(0, 1, 0);
			glVertex3f(0, 0, 0);

			for (double i = 0; i < (2 * M_PI + dW); i += dW)
				glVertex3f(rTopHalf * sin(i), hHalf, rTopHalf * cos(i));
		}
		glEnd();

		glBegin(GL_TRIANGLE_FAN);
		{
			glNormal3f(0, -1, 0);
			glVertex3f(0, 0, 0);

			for (double i = 0; i < (2 * M_PI + dW); i += dW)
				glVertex3f(rBottomHalf * sin(i), -hHalf, rBottomHalf * cos(i));
		}
		glEnd();
	}

	if (showNormals)
	{
		glDisable(GL_LIGHTING);
		glColor3f(0, 1, 0);
		glBegin(GL_LINES);
		{
			for (double i = 0; i < (2 * M_PI + dW); i += dW)
			{
				double x = sin(i),
					z = cos(i);

				glVertex3f(rTopHalf * x, hHalf, rTopHalf * z);
				glVertex3f((rTopHalf + nr) * x, hHalf + ny, (rTopHalf + nr) * z);

				glVertex3f(rBottomHalf * x, -hHalf, rBottomHalf * z);
				glVertex3f((rBottomHalf + nr) * x, -hHalf + ny, (rBottomHalf + nr) * z);
			}
		}
		glEnd();
		glEnable(GL_LIGHTING);
	}
}
void CGLRenderer::DrawSphere(double r, int nStep1, int nStep2, int alphaMax, int betaMax)
{
	double aMax = alphaMax * M_PI / 180,
		bMax = betaMax * M_PI / 180,
		dAlpha = aMax / nStep1,
		dBeta = bMax / nStep2;

	glBegin(GL_QUAD_STRIP);
	{
		for (double i = 0; i < aMax; i += dAlpha)
		{
			for (double j = 0; j <= bMax; j += dBeta)
			{
				double x = cos(i) * cos(j),
					y = sin(i),
					z = cos(i) * sin(j);

				glNormal3f(x, y, z);
				glVertex3f(r * cos(i + dAlpha) * cos(j), r * sin(i + dAlpha), r * cos(i + dAlpha) * sin(j));
				glVertex3f(r * x, r * y, r * z);
			}
		}
	}
	glEnd();
}