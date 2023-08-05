#include <limits>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <iostream>
#include <vector>

#include <GL/gl.h>
#include <GL/glut.h>

using namespace std;


///////////////////////////////////////////////////////////////////////////////
/////////////////////////// CLASES - STRUCTS /////////////////////////////////

struct Point
{
	float x, y, z;
	Point(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	Point()
	{
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}
};

struct Line
{
	Point A, B;

};


///////////////////////////////////////////////////////////////////////////////
///////////////////////// VARIABLES GLOBALES /////////////////////////////////

std::vector<Point> puntos;

float rotX= 0, rotY = 0, rotZ = 0;

int WWidth = 1200, WHeight = 720;

float aspect = WWidth/WHeight;
float fovy = 45;
float dist = 5;

float frustumHeight = 2.0 * dist * tan(fovy * 0.5 * (M_PI/180));
float scale = frustumHeight;

int pCount = 0;
int pIndex = 0;

float pMovSpeed = 0.1;

float t = 0; //Debe ir de 0 a 1

///////////////////////////////////////////////////////////////////////////////
//////////////////////////// HELPER FUNCS ////////////////////////////////////

Point mix(Point a, Point b, float u)
{
	Point c;
	c.x = (1-u)*a.x + u*b.x;
	c.y = (1-u)*a.y + u*b.y;
	c.z = (1-u)*a.z + u*b.z;
	return c;
}

Point bezier(std::vector<Point> ps, float t)
{
	if(ps.size() < 2)
	{
		return Point(ps[0]);
	}
	std::vector<Point> nP;
	for( int i = 0; i < ps.size()-1; i++)
	{
		Point A = ps[i];
		Point B = ps[i+1];
		// std::cout<<"A: "  << A.x << ", " << A.y << ", " << A.z <<"\n";
		// std::cout<<"B: "  << B.x << ", " << B.y << ", " << B.z <<"\n\n";
		nP.push_back(mix(A, B, t));
	}
	return bezier(nP, t);

}



///////////////////////////////////////////////////////////////////////////////
//////////////////////////// ACTORS //////////////////////////////////////////

void drawCurvePolygon(std::vector<Point> ps)
{
	if(ps.size() < 2)
	{
		glColor3f(0,0,1);
		glBegin(GL_POINTS);
		glVertex3f(ps[0].x, ps[0].y, ps[0].z);
		glEnd();
		return;
	}
	
	glColor3f(1,1,1);
	glLineWidth(1);
	glBegin(GL_LINES);
	std::vector<Point> nP;
	for( int i = 0; i < ps.size()-1; i++)
	{
		Point A = ps[i];
		Point B = ps[i+1];
		// std::cout<<"A: "  << A.x << ", " << A.y << ", " << A.z <<"\n";
		// std::cout<<"B: "  << B.x << ", " << B.y << ", " << B.z <<"\n\n";
		nP.push_back(mix(A, B, t));
		glVertex3f(A.x, A.y, A.z);
		glVertex3f(B.x, B.y, B.z);
	}
	glEnd();
	drawCurvePolygon(nP);

}

void drawLine(Point a, Point b)
{
	glColor3f(1,1,0);
	glBegin(GL_LINES);
	glVertex3f(a.x,a.y,a.z);
	glVertex3f(b.x,b.y,b.z);
	glEnd();
}

void drawCurve()
{
	Point po = puntos[0];
	for (double x = 0.0; x<=1.0;x+=0.01)
	{
		Point P = bezier(puntos, x);
		drawLine(po, P);
		po = P;
	}
}


void drawPoints()
{
	for (int i = 0; i < puntos.size(); i++)
	{
		glColor3f(1, 0, 0.5);
		if(i == pIndex)
			glColor3f(0,1,0);
		glPointSize(10);
		glBegin(GL_POINTS);
		glVertex3f(puntos[i].x, puntos[i].y, puntos[i].z);
		glEnd();
		glFlush();
	}
}

void drawAxis() {
	glBegin (GL_LINES);
	glColor3f (1, 0, 0);
	glVertex3f (0, 0, 0);
	glVertex3f (1, 0, 0);
	glColor3f (0, 1, 0);
	glVertex3f (0, 0, 0);
	glVertex3f (0, 1, 0);
	glColor3f (0, 0, 1);
	glVertex3f (0, 0, 0);
	glVertex3f (0, 0, 1);
	glEnd ();
}


void DrawAllActors()
{
	glRotatef(rotX, 1, 0, 0);
	glRotatef(rotY, 0, 1, 0);
	glRotatef(rotZ, 0, 0, 1);

	drawPoints();
	drawAxis();
	if(pCount >= 3)
	{
		drawCurvePolygon(puntos);
		drawCurve();
	}
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////// GLUT EVENT FUNCS /////////////////////////////////////


static void Draw(void)
{
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	//gluOrtho2D( 0.0, 1200.0, 0.0, 800.0);
	gluPerspective(fovy, aspect, 0.1, 50);
	gluLookAt(0, 0, dist, 0, 0, 0, 0, 1, 0);
	DrawAllActors();
	glFlush();
}

static void KeyPress(unsigned char Key, int x, int y)
{
	switch (Key)
	{
	//Mover el punto seleccionado en Z
	case '+':
		if(pIndex >= 0 && pIndex < pCount)
			puntos[pIndex].z += 1*pMovSpeed;
		break;
	case '-':
		if(pIndex >= 0 && pIndex < pCount)
			puntos[pIndex].z -= 1*pMovSpeed;
		break;

	//Mover el punto seleccionado en X
	case 'd':
		if(pIndex >= 0 && pIndex < pCount)
			puntos[pIndex].x += 1*pMovSpeed;
		break;
	case 'a':
		if(pIndex >= 0 && pIndex < pCount)
			puntos[pIndex].x -= 1*pMovSpeed;
		break;

	//Mover el punto seleccionado en Y
	case 'w':
		if(pIndex >= 0 && pIndex < pCount)
			puntos[pIndex].y += 1*pMovSpeed;
		break;
	case 's':
		if(pIndex >= 0 && pIndex < pCount)
			puntos[pIndex].y -= 1*pMovSpeed;
		break;

	//Reset la rotacion de la camara
	case 'r':
		rotX = 0;
		rotY = 0;
		rotZ = 0;
		break;
	//Cambiar al siguiente punto en la lista
	case 'c':
		pIndex++;
		if(pIndex >= pCount)
			pIndex = pCount -1;
		break;
	//Cambiar al anterior punto en la lista
	case 'x':
		pIndex--;
		if(pIndex < 0)
			pIndex = 0;
		break;

	//Cambiar el t
	case 't':
		t += 0.01;
		if(t>1)
			t = 1;
		break;
	case 'y':
		t -= 0.01;
		if(t<0)
			t = 0;
		break;
	case 27:
		exit(1);
	}
	
	glutPostRedisplay();
}

static void SpecialKey(int Key, int x, int y)
{
	switch (Key)
	{
	case GLUT_KEY_UP:
		rotX += 1;
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		rotX -= 1;
		glutPostRedisplay();
		break;
	case GLUT_KEY_LEFT:
		rotY += 1;
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		rotY -= 1;
		glutPostRedisplay();
		break;
	case GLUT_KEY_F11:
		rotZ -= 1;
		glutPostRedisplay();
		break;
	case GLUT_KEY_F12:
		rotZ += 1;
		glutPostRedisplay();
		break;
	}
}


void mouse(int button, int state, int mousex, int mousey)
{
	
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
		float worldX = (scale*((float)mousex- WWidth/2)/(float)WWidth) ;
		float worldY = (scale*(float)(WHeight-mousey - WHeight/2)/(float)WHeight);
		std::cout<<"X: "  << worldX << " Y: " << worldY << "\n";
		puntos.push_back(Point(worldX, worldY, 0));

		pCount++;
		pIndex = pCount-1;
    	glutPostRedisplay();
    }
}



///////////////////////////////////////////////////////////////////////////////
////////////////////////////// MAIN //////////////////////////////////////////

int main(int argc, char **argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(WWidth, WHeight);
	glutCreateWindow("Bezier Curves ");

	glClearColor(1, 1, 1, 0); 
    glClear(GL_COLOR_BUFFER_BIT); 

	glutDisplayFunc(Draw);
	glutMouseFunc(mouse);
	glutKeyboardFunc(KeyPress);
	glutSpecialFunc(SpecialKey);
	glutMainLoop();
	return (0);
}
// eof
