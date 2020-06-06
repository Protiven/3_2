#define _CRT_SECURE_NO_WARNINGS
#include "vector2f.h"

using namespace std;

#define ESCAPE '\033'
#define DELETE '\010'
#define PI 3.141592653589793238463
int sizeMenu, mainMenu, colorMenu, shiftMenu, rotateMenu, changeMenu;
double a, b, c, d, f_d, f_d2;
double betta, gamma;

// шрифт по умолчанию
void* font = GLUT_BITMAP_9_BY_15;

GLint Width = 1024, Height = 640;

Vector2f Right(Width * 2 + Width / 2, Height / 2), Left(-Width * 2 + Width / 2, Height / 2), Up(Width / 2, Height * 2 + Height / 2), Down(Width / 2, -Height * 2 + Height / 2);
int nx = (Width * 2) / 64;
int ny = (Height * 2) / 64;
Vector2f center{ (double)Width / 2,(double)Height / 2 };
double AxisCoef = 1;

bool myfunction(Vector2f i, Vector2f j) { return (i.x < j.x); }

vector<Vector2f>Points;//точки по порядку добавлению
vector<Vector2f>NewPoints;//отсортированные точки
vector<Vector2f>SplitedPoints;//промежуточные точки на одном интервале
vector<GLuint>Color = { 255,0,255 };

void renderBitmapString(double x, double y, char* string) {
	char* c;
	glRasterPos2f(x, y);
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}

void DrawAxis() {
	glLineWidth(1);
	glColor3ub(255, 255, 255);
	glBegin(GL_LINES);
	// Ось Х
	glVertex2f(Left.x, Left.y);
	glVertex2f(Right.x, Right.y);

	// Ось Y
	glVertex2f(Down.x, Down.y);
	glVertex2f(Up.x, Up.y);
	glEnd();

	char array[256];
	ostringstream strs;
	memset(array, 0, sizeof(array));
	array[0] = '0';
	renderBitmapString(Up.x + 10, Right.y - 20, array);

	//засечки по х
	for (int xi = 0, i = 0; i <= nx; xi += 64, i++) {
		glBegin(GL_LINES);
		glVertex2f(Up.x + xi, Right.y + 5);
		glVertex2f(Up.x + xi, Right.y - 5);
		glEnd();

		glBegin(GL_LINES);
		glVertex2f(Up.x - xi, Right.y + 5);
		glVertex2f(Up.x - xi, Right.y - 5);
		glEnd();


		if (i != 0) {
			double ii = i * AxisCoef;

			memset(array, 0, sizeof(array));

			strs.str("");
			strs << ii;
			string str = strs.str();
			for (int j = 0; j < str.length(); j++)
				array[j] = str[j];

			renderBitmapString(Up.x + xi - 5, Right.y - 20, array);

			memset(array, 0, sizeof(array));
			strs.str("");
			strs << -ii;
			str = strs.str();
			for (int j = 0; j < str.length(); j++)
				array[j] = str[j];
			renderBitmapString(Up.x - xi - 10, Right.y - 20, array);
		}
	}

	//засечки по у
	for (int yi = 0, i = 0; i <= ny; yi += 64, i++) {
		glBegin(GL_LINES);
		glVertex2f(Up.x + 5, Right.y + yi);
		glVertex2f(Up.x - 5, Right.y + yi);
		glEnd();

		glBegin(GL_LINES);
		glVertex2f(Up.x + 5, Right.y - yi);
		glVertex2f(Up.x - 5, Right.y - yi);
		glEnd();

		if (i != 0) {
			double ii = i * AxisCoef;
			memset(array, 0, sizeof(array));
			strs.str("");
			strs << ii;
			string str = strs.str();
			for (int j = 0; j < str.length(); j++)
				array[j] = str[j];

			renderBitmapString(Up.x + 10, Right.y + yi - 4, array);

			memset(array, 0, sizeof(array));
			strs.str("");
			strs << -ii;
			str = strs.str();
			for (int j = 0; j < str.length(); j++)
				array[j] = str[j];
			renderBitmapString(Up.x + 6, Right.y - yi - 6, array);
		}
	}

}

void ComputeCoef2(int i) {
	int hx = NewPoints[i + 1].x - NewPoints[i].x;
	if (i == 0) {
		f_d = (NewPoints[i + 1].y - NewPoints[i].y) / hx;
		a = 1. / (hx * hx) * (NewPoints[i + 1].y - NewPoints[i].y) - 1. / hx * f_d;
		b = f_d - 2. * a * NewPoints[i].x;
		c = NewPoints[i].y - f_d * NewPoints[i].x + a * NewPoints[i].x * NewPoints[i].x;
	}
	else {
		betta = 2. * a * NewPoints[i].x + b;
		a = 1. / (hx * hx) * (NewPoints[i + 1].y - NewPoints[i].y) - 1. / hx * betta;
		b = betta - 2. * a * NewPoints[i].x;
		c = NewPoints[i].y - betta * NewPoints[i].x + a * NewPoints[i].x * NewPoints[i].x;
	}
}
double ComputeSpline2(int x)
{
	return a * x * x + b * x + c;
}

void SplitPoints(int i) {
	double h = NewPoints[i + 1].x - NewPoints[i].x;
	h /= 20;

	for (int j = 0; j < 20; j++) {
		Vector2f vec(NewPoints[i].x + j * h, ComputeSpline2(NewPoints[i].x + j * h));
		SplitedPoints.push_back(vec);
	}
	Vector2f vec(NewPoints[i + 1].x, NewPoints[i + 1].y);
	SplitedPoints.push_back(vec);
}

/* Функция вывода на экран */
void Display(void) {
	glClearColor(0.5, 0.5, 0.5, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	glLineWidth(3);
	glEnable(GL_LINE_WIDTH);
	glPointSize(3);

	DrawAxis();

	glBegin(GL_POINTS);

	for (int i = 0; i < NewPoints.size(); i++) {
		glVertex2f(NewPoints[i].x, NewPoints[i].y);
	}
	glEnd();
	glLineWidth(3);
	//серые линии
	glColor3ub(0, 0, 0);
	if (Points.size() != 0) {
		glBegin(GL_LINES);
		for (int i = 0; i < NewPoints.size() - 1; i++) {
			glVertex2f(NewPoints[i].x, NewPoints[i].y);
			glVertex2f(NewPoints[i + 1].x, NewPoints[i + 1].y);
		}
		glEnd();
	}

	int i;
	SplitedPoints.resize(0);
	if (NewPoints.size() != 0) {
		for (i = 0; i < NewPoints.size() - 1; i++) {

			ComputeCoef2(i);
			SplitPoints(i);
			//20 ВНУТРЕННИХ ТОЧЕК
		}
		glColor3ub(Color[0], Color[1], Color[2]);
		glBegin(GL_LINE_STRIP);
		for (i = 0; i < SplitedPoints.size(); i++) {
			glVertex2f(SplitedPoints[i].x, SplitedPoints[i].y);

		}
		glEnd();
	}
	glFinish();
}

void AddPoint(Vector2f P)
{
	int q = 0;
	for (int i = 0; i < Points.size(); i++)
		if (Points[i].x == P.x) q++;

	if (q == 0) {

		Points.push_back(P);
		NewPoints.push_back(P);

		sort(NewPoints.begin(), NewPoints.end(), myfunction);
	}
}
void DeletePoint()
{
	Vector2f P = Points[Points.size() - 1];
	auto res = find_if(NewPoints.begin(), NewPoints.end(), [&](const Vector2f& s)-> bool {return (s.x == P.x) && (s.y == P.y); });
	NewPoints.erase(res);
	Points.pop_back();
}

/* Функция изменения размеров окна */
void Reshape(GLint w, GLint h) {
	Width = w;
	Height = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/* Функция обработки сообщений от клавиатуры */
void Keyboard(unsigned char key, int x, int y) {
	if ((NewPoints.size()) >= 0) {
		int i, n = NewPoints.size();
		// Изменение RGB-компонент цвета точек
		if (key == 'r') Color[0] -= 5;
		if (key == 'g') Color[1] -= 5;
		if (key == 'b') Color[2] -= 5;

		// Изменение XY-кординат точек
		if (key == 'w') {
			for (i = 0; i < n; i++) {
				Points[i].y += 4;
				NewPoints[i].y += 4;
			}
			Left.y += 4;
			Right.y += 4;
			Up.y += 4;
			Down.y += 4;

		}
		if (key == 's') {
			for (i = 0; i < n; i++) {
				Points[i].y -= 4;
				NewPoints[i].y -= 4;
			}
			Left.y -= 4;
			Right.y -= 4;
			Up.y -= 4;
			Down.y -= 4;

		}
		if (key == 'a') {
			for (i = 0; i < n; i++) {
				Points[i].x -= 4;
				NewPoints[i].x -= 4;
			}
			Left.x -= 4;
			Right.x -= 4;
			Up.x -= 4;
			Down.x -= 4;

		}
		if (key == 'd') {
			for (i = 0; i < n; i++) {
				Points[i].x += 4;
				NewPoints[i].x += 4;
			}
			Left.x += 4;
			Right.x += 4;
			Up.x += 4;
			Down.x += 4;

		}

		if (key == ESCAPE) {
			exit(0);
		}

		if (key == 'm') {
			double CoefStretch = 2;
			for (int i = 0; i < NewPoints.size(); i++) {
				NewPoints[i].x = center.x - CoefStretch * (center.x - NewPoints[i].x);
				NewPoints[i].y = center.y - CoefStretch * (center.y - NewPoints[i].y);
				Points[i].x = center.x - CoefStretch * (center.x - Points[i].x);
				Points[i].y = center.y - CoefStretch * (center.y - Points[i].y);
			}
			AxisCoef /= 2;
		}
		if (key == 'n') {
			double CoefStretch = 0.5;

			for (int i = 0; i < NewPoints.size(); i++) {
				NewPoints[i].x = center.x - CoefStretch * (center.x - NewPoints[i].x);
				NewPoints[i].y = center.y - CoefStretch * (center.y - NewPoints[i].y);

				Points[i].x = center.x - CoefStretch * (center.x - Points[i].x);
				Points[i].y = center.y - CoefStretch * (center.y - Points[i].y);
			}
			AxisCoef *= 2;
		}
		glutPostRedisplay();
	}
}

/* Функция обработки сообщения от мыши */
void Mouse(int button, int state, int x, int y) {
	/* клавиша была нажата, но не отпущена */
	if (state != GLUT_DOWN) return;
	/* новая точка по левому клику */
	if (button == GLUT_LEFT_BUTTON) {
		Vector2f p;
		p.x = x;
		p.y = Height - y;
		AddPoint(p);
	}

	if (button == GLUT_RIGHT_BUTTON && Points.size() != 0) {
		if (Points.size() != 0)
			DeletePoint();
	}

	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
	{
		if (Points.size() != 0) {
			int j = 0;
			int index = 0;
			if (Points.size() > 0) {
				int minind = 0;
				double mindist = sqrt((Points[0].x - x) * (Points[0].x - x) + (Points[0].y - (Height - y)) * (Points[0].y - (Height - y)));
				for (int k = 0; k < Points.size(); k++) {
					double d = sqrt((Points[k].x - x) * (Points[k].x - x) + (Points[k].y - (Height - y)) * (Points[k].y - (Height - y)));
					if (d < mindist) {
						mindist = d;
						index = k;
					}
				}
				j = minind;
			}

			Vector2f P = Points[index];
			auto res = find_if(NewPoints.begin(), NewPoints.end(), [&](const Vector2f& s)-> bool {return (s.x == P.x) && (s.y == P.y); });
			NewPoints.erase(res);
			Points.erase(Points.begin() + index);
		}
	}

	glutPostRedisplay();
}

/* Головная программа */
void main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(Width, Height);
	glutCreateWindow("Spline");
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMainLoop();
}
