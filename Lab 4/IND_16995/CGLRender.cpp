#include "pch.h"
#include "CGLRender.h"


CGLRender::CGLRender()
{
	this->m_hrc = NULL;

	this->viewR = 20;
	this->viewAngleXY = 30;
	this->viewAngleXZ = 0;

	this->viewPosition = new GLdouble[3]
	{
		this->viewR * cos(M_PI * viewAngleXY / 180) * cos(M_PI * viewAngleXZ / 180),
		this->viewR * sin(M_PI * viewAngleXY / 180),
		this->viewR * cos(M_PI * viewAngleXY / 180) * sin(M_PI * viewAngleXZ / 180)
	};
	this->upVector = new GLdouble[3]{ 0, 1, 0 };
	this->lookingAt = new GLdouble[3]{ 0, 2.5, 0 };

	this->showGrid = this->showAxes = true;

	this->pieceAngle = 45;
	this->sphereR = 0.25;
	this->cylinderH = 1;
	this->cylinderR = 0.35;
	this->coneH = 1;
	this->coneR = 0.35;
	this->nSeg = 90;
}

CGLRender::~CGLRender()
{
	if (this->lookingAt)
	{
		delete[] this->lookingAt;
		this->lookingAt = nullptr;
	}

	if (this->viewPosition)
	{
		delete[] this->viewPosition;
		this->viewPosition = nullptr;
	}

	if (this->upVector)
	{
		delete[] this->upVector;
		this->upVector = nullptr;
	}
}

bool CGLRender::CreateGLContext(CDC* pDC)
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

	if (nPixelFormat == 0)
		return false;

	BOOL bResult = SetPixelFormat(pDC->m_hDC, nPixelFormat, &pfd);

	if (bResult == FALSE)
		return false;

	this->m_hrc = wglCreateContext(pDC->m_hDC);

	if (this->m_hrc == NULL)
		return false;

	return true;
}

void CGLRender::PrepareScene(CDC* pDC)
{
	wglMakeCurrent(pDC->m_hDC, this->m_hrc);
	// -------------------------------------------

	glClearColor(0, .8, .0, .0);

	// -------------------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRender::DestroyScene(CDC* pDC)
{
	wglMakeCurrent(pDC->m_hDC, this->m_hrc);
	// -------------------------------------------
	wglMakeCurrent(NULL, NULL);

	if (this->m_hrc != NULL)
	{
		wglDeleteContext(this->m_hrc);
		this->m_hrc = NULL;
	}
}

void CGLRender::Reshape(CDC* pDC, int w, int h)
{
	wglMakeCurrent(pDC->m_hDC, this->m_hrc);
	// -------------------------------------------

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30, (double)w / (double)h, 1, 100);
	glMatrixMode(GL_MODELVIEW);

	// -------------------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRender::DrawScene(CDC* pDC)
{
	wglMakeCurrent(pDC->m_hDC, this->m_hrc);
	// -------------------------------------------
	glClearColor(0, .8, .0, .0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	glLoadIdentity();

	gluLookAt(this->viewPosition[0], this->viewPosition[1], this->viewPosition[2],
		this->lookingAt[0], this->lookingAt[1], this->lookingAt[2],
		this->upVector[0], this->upVector[1], this->upVector[2]);

	if(this->showAxes)
		this->DrawAxes();

	if(this->showGrid)
		this->DrawGrid();

	this->DrawFigure(this->pieceAngle);

	// -------------------------------------------
	SwapBuffers(pDC->m_hDC);
	wglMakeCurrent(NULL, NULL);
}

void CGLRender::DrawFigure(double angle)
{
	double heightCylinderSphereDif = this->cylinderH / 2 + this->sphereR,
		heightConeSphereDif = this->coneH / 2 + this->sphereR;
	glTranslatef(0, 0.5, 0);
	DrawCylinder(1, 1.2, 1, 8, { .5, .75, 1 });

	glTranslatef(0, 0.75, 0);
	DrawCylinder(0.5, 1.4, 1.4, 8, { .5, .75, 1 });

	glTranslatef(0, .75, 0);
	DrawCone(this->coneH, this->coneR, this->nSeg);

	glTranslatef(0, heightCylinderSphereDif, 0);
	DrawSphere(this->sphereR, this->nSeg, this->nSeg);

	// Desna grana
	glPushMatrix();
	{
		glRotatef(-45, 1, 0, 0);

		glTranslatef(0, heightCylinderSphereDif, 0);
		DrawCylinder(this->cylinderH, this->cylinderR, this->cylinderR, this->nSeg);

		glTranslatef(0, heightCylinderSphereDif, 0);
		DrawSphere(this->sphereR, this->nSeg, this->nSeg);
	}
	glPopMatrix();

	// Srednja grana
	glPushMatrix();
	{
		glTranslatef(0, heightConeSphereDif, 0);
		DrawCone(this->coneH, this->coneR, this->nSeg);

		glTranslatef(0, heightConeSphereDif, 0);
		DrawSphere(this->sphereR, this->nSeg, this->nSeg);
	}
	glPopMatrix();

	// Leva grana
	glPushMatrix();
	{
		glRotatef(45, 1, 0, 0);

		glTranslatef(0, heightConeSphereDif, 0);
		DrawCylinder(this->cylinderH, this->cylinderR, this->cylinderR, this->nSeg);

		glTranslatef(0, heightConeSphereDif, 0);
		DrawSphere(this->sphereR, this->nSeg, this->nSeg);

		// Na gore
		glPushMatrix();
		{
			glRotatef(-45, 1, 0, 0);

			glTranslatef(0, heightCylinderSphereDif, 0);
			DrawCylinder(this->cylinderH, this->cylinderR, this->cylinderR, this->nSeg);

			glTranslatef(0, heightCylinderSphereDif, 0);
			DrawSphere(this->sphereR, this->nSeg, this->nSeg);

			// Na desno
			glPushMatrix();
			{
				glRotatef(-45, 1, 0, 0);

				glTranslatef(0, heightConeSphereDif, 0);
				DrawCylinder(this->cylinderH, this->cylinderR, this->cylinderR, this->nSeg);

				glTranslatef(0, heightConeSphereDif, 0);
				DrawSphere(this->sphereR, this->nSeg, this->nSeg);
			}
			glPopMatrix();

			// Na levo
			glPushMatrix();
			{
				glRotatef(45, 1, 0, 0);

				glTranslatef(0, heightConeSphereDif, 0);
				DrawCone(this->coneH, this->coneR, this->nSeg);

				glTranslatef(0, heightConeSphereDif, 0);
				DrawSphere(this->sphereR, this->nSeg, this->nSeg);
			}
			glPopMatrix();
		}
		glPopMatrix();

		// Na levo
		glPushMatrix();
		{
			glRotatef(angle, 1, 0, 0);

			glTranslatef(0, heightCylinderSphereDif, 0);
			DrawCylinder(this->cylinderH, this->cylinderR, this->cylinderR, this->nSeg, { 1, 1, 0 });

			glTranslatef(0, heightCylinderSphereDif, 0);
			DrawSphere(this->sphereR, this->nSeg, this->nSeg);

			// Na desno
			glPushMatrix();
			{
				glRotatef(-45, 1, 0, 0);

				glTranslatef(0, heightCylinderSphereDif, 0);
				DrawCylinder(this->cylinderH, this->cylinderR, this->cylinderR, this->nSeg);

				glTranslatef(0, heightCylinderSphereDif, 0);
				DrawSphere(this->sphereR, this->nSeg, this->nSeg);
			}
			glPopMatrix();

			// Pravo
			glPushMatrix();
			{

				glTranslatef(0, heightCylinderSphereDif, 0);
				DrawCylinder(this->cylinderH, this->cylinderR, this->cylinderR, this->nSeg);

				glTranslatef(0, heightCylinderSphereDif, 0);
				DrawSphere(this->sphereR, this->nSeg, this->nSeg);
			}
			glPopMatrix();

			glRotatef(45, 1, 0, 0);

			glTranslatef(0, heightConeSphereDif, 0);
			DrawCone(this->coneH, this->coneR, this->nSeg);

			glTranslatef(0, heightConeSphereDif, 0);
			DrawSphere(this->sphereR, this->nSeg, this->nSeg);
		}
		glPopMatrix();
	}
	glPopMatrix();
}

void CGLRender::DrawCylinder(double h, double rTop, double rBottom, int nSeg, COLOR3F color)
{
	double step = 360.0 / nSeg,
		hHalf = h / 2.0,
		factor = M_PI / 180;

	// Top base center + mantle strips + bottom base center
	long size = sizeof(float) * (1 + (nSeg + 1) * 2 + 1) * 3;
	float* vertexes = (float*)malloc(size);
	memset(vertexes, 0, size);

	int i = 6, indices = 2;
	vertexes[1] = hHalf,
	vertexes[4] = -hHalf;
	for (double j = 0; j <= 360; j += step)
	{
		double cosine = cos(factor * j),
			sine = sin(factor * j);

		vertexes[i++] = rTop * cosine,
		vertexes[i++] = hHalf,
		vertexes[i++] = rTop* sine;

		vertexes[i++] = rBottom * cosine,
		vertexes[i++] = -hHalf,
		vertexes[i++] = rBottom * sine;

		indices += 2;
	}

	glColor3f(color.R, color.G, color.B);
	glEnableClientState(GL_VERTEX_ARRAY);

	// Top base
	glVertexPointer(3, GL_FLOAT, sizeof(float) * 6, vertexes);
	glDrawArrays(GL_TRIANGLE_FAN, 0, indices / 2);

	// Mantle
	glVertexPointer(3, GL_FLOAT, 0, &vertexes[6]);
	glDrawArrays(GL_QUAD_STRIP, 0, indices - 2);

	// Bottom base
	glVertexPointer(3, GL_FLOAT, sizeof(float) * 6, &vertexes[3]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, indices / 2);

	glDisableClientState(GL_VERTEX_ARRAY);

	free(vertexes);
}

void CGLRender::DrawCone(double h, double r, int nSeg, COLOR3F color)
{
	double step = 360.0 / nSeg,
		hHalf = h / 2.0,
		factor = M_PI / 180;

	long size = sizeof(float) * (nSeg + 1 + 1) * 3;
	float* mantleVertexes = (float*)malloc(size);
	memset(mantleVertexes, 0, size);

	int i = 1, indices = 0;
	mantleVertexes[i] = hHalf;
	i += 2;
	indices++;

	for (double iM = 0; iM <= 360; iM += step)
	{
		mantleVertexes[i++] = r * cos(factor * iM);
		mantleVertexes[i++] = -hHalf;
		mantleVertexes[i++] = r * sin(factor * iM);

		indices++;
	}

	glColor3f(color.R, color.G, color.B);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, mantleVertexes);

	// Mantle
	glDrawArrays(GL_TRIANGLE_FAN, 0, indices);

	// Base
	mantleVertexes[1] = -hHalf;
	glDrawArrays(GL_TRIANGLE_FAN, 0, indices);

	glDisableClientState(GL_VERTEX_ARRAY);

	free(mantleVertexes);
}

void CGLRender::DrawSphere(double r, int nSegAlpha, int nSegBeta, COLOR3F color)
{
	double factorAlpha = M_PI / nSegAlpha,
		factorBeta = M_PI / nSegBeta,
		doublePI = 2* M_PI,
		sineFactorAlpha = sin(factorAlpha),
		cosineFactorAlpha = cos(factorAlpha),
		prevSineAlpha = .0,
		prevCosineAlpha = 1.0,
		sineFactorBeta = sin(factorBeta),
		cosineFactorBeta = cos(factorBeta),
		prevSineBeta = .0,
		prevCosineBeta = 1.0;

	long size = sizeof(float) * 3 * (nSegAlpha * (nSegBeta + 1) * 4);
	float* vertexes = (float*) malloc(size);
	memset(vertexes, 0, size);

	int i = 0, indices = 0;
	for (double dAlpha = 0.0; dAlpha < M_PI; dAlpha += factorAlpha)
	{
		double sineAlpha = prevSineAlpha * cosineFactorAlpha + prevCosineAlpha * sineFactorAlpha,
			cosineAlpha = prevCosineAlpha * cosineFactorAlpha - prevSineAlpha * sineFactorAlpha;

		for (double dBeta = 0.0; dBeta <= doublePI; dBeta += factorBeta)
		{
			double sineBeta = prevSineBeta * cosineFactorBeta + prevCosineBeta * sineFactorBeta,
				cosineBeta = prevCosineBeta * cosineFactorBeta - prevSineBeta * sineFactorBeta;

			vertexes[i++] = r * prevSineAlpha * cosineBeta,
				vertexes[i++] = r * prevSineAlpha * sineBeta,
				vertexes[i++] = r * prevCosineAlpha,
			vertexes[i++] = r * sineAlpha * cosineBeta,
				vertexes[i++] = r * sineAlpha * sineBeta,
				vertexes[i++] = r * cosineAlpha;

			prevSineBeta = sineBeta,
				prevCosineBeta = cosineBeta;

			indices += 2;
		}

		prevSineAlpha = sineAlpha,
			prevCosineAlpha = cosineAlpha;
	}

	glColor3f(color.R, color.G, color.B);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertexes);
	glDrawArrays(GL_QUAD_STRIP, 0, indices);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	free(vertexes);
}

void CGLRender::DrawGrid(double width, double height, int nSegW, int nSegH, COLOR3F color)
{
	double stepW = width / nSegW,
		stepH = height / nSegH,
		halfW = width / 2.0,
		halfH = height / 2.0;

	long size = sizeof(float) * (nSegH + 1.0 + nSegW + 1.0) * 6.0;
	float* gridVertexes = (float*)malloc(size);
	memset(gridVertexes, 0, size);

	int i = 0, indices = 0;
	for (double iW = -halfW; iW <= halfW; iW += stepW)
	{
		gridVertexes[i++] = iW; gridVertexes[i++] = 0; gridVertexes[i++] = halfH;
		gridVertexes[i++] = iW; gridVertexes[i++] = 0; gridVertexes[i++] = -halfH;
		indices += 2;
	}

	for (double iH = -halfH; iH <= halfH; iH += stepH)
	{
		gridVertexes[i++] = halfW; gridVertexes[i++] = 0; gridVertexes[i++] = iH;
		gridVertexes[i++] = -halfW; gridVertexes[i++] = 0; gridVertexes[i++] = iH;
		indices += 2;
	}

	glColor3f(color.R, color.G, color.B);
	glEnableClientState(GL_VERTEX_ARRAY);

	// 4 granicna temena su po dva puta alocirana (4 temena vise), ali je jednostavnije ovako
	glVertexPointer(3, GL_FLOAT, 0, gridVertexes);
	glDrawArrays(GL_LINES, 0, indices);

	glDisableClientState(GL_VERTEX_ARRAY);

	free(gridVertexes);
}

void CGLRender::RotateView(double dXY, double dXZ)
{
	this->viewAngleXY += dXY;
	this->viewAngleXZ += dXZ;

	double dwXY = M_PI * this->viewAngleXY / 180,
		dwXZ = M_PI * this->viewAngleXZ / 180;

	if(dXY)
		this->upVector[1] = signbit(this->viewR * sin(dwXY) - this->viewPosition[1]) ? (signbit(dXY) ? 1 : -1) : (signbit(dXY) ? -1 : 1);

	this->viewPosition[0] = this->viewR * cos(dwXY) * cos(dwXZ);
	this->viewPosition[1] = this->viewR * sin(dwXY);
	this->viewPosition[2] = this->viewR * cos(dwXY) * sin(dwXZ);
}

void CGLRender::Zoom(bool out)
{
	this->viewR += out ? 1 : -1;

	double dwXY = M_PI * this->viewAngleXY / 180,
		dwXZ = M_PI * this->viewAngleXZ / 180;

	this->viewPosition[0] = this->viewR * cos(dwXY) * cos(dwXZ);
	this->viewPosition[1] = this->viewR * sin(dwXY);
	this->viewPosition[2] = this->viewR * cos(dwXY) * sin(dwXZ);
}

void CGLRender::DrawAxes(double lenght, bool useNegative)
{
	glLineWidth(2);

	long size = sizeof(float) * 36;
	float *vertexes = (float*)malloc(size);
	memset(vertexes, 0, size);

	for (int i = 0; i < 36; i+=12)
	{
		int offset = i / 12;
		vertexes[i + offset] = vertexes[i + 6 + offset] = 1;
		vertexes[i + 3 + offset] = lenght;
		vertexes[i + 9 + offset] = useNegative ? -vertexes[i + 3 + offset] : 0;
	}
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(float) * 6, &vertexes[3]);
	glColorPointer(3, GL_FLOAT, sizeof(float) * 6, &vertexes[0]);

	glDrawArrays(GL_LINES, 0, 6);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	free(vertexes);
}
