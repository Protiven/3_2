/*
Отключено консольное (главное) окно:
	Linker ->  Advanced -> Entery Point := "mainCRTStartup"
	Linker ->  System -> SubSystem := "Windows (/SUBSYSTEM:WINDOWS)"
*/
#include <math.h>
#include <stdlib.h>
#include "glut.h"
#include <vector>
using namespace std;

GLint Width = 512, Height = 512;
/*
GLubyte ColorR = 0, ColorG = 0, ColorB = 0;
GLubyte PointSize = 5;
*/
enum keys { Empty, KeyR, KeyG, KeyB, KeyW, KeyA, KeyS, KeyD, KeyU, KeyI };

int i_groups = 0;

/* задание контейнера вершин */
struct type_point
{
	GLint x, y, x0, y0;
	type_point(GLint _x, GLint _y) { x = _x; y = _y; x0 = _x; y0 = _y; }
};

struct group_of_objects
{
	group_of_objects(GLdouble rt, GLubyte R, GLubyte G, GLubyte B, GLubyte Size)
	{
		this->ColorB = B;
		this->ColorG = G;
		this->ColorR = R;
		this->PointSize = Size;
		this->rotation = rt;
	}
	vector <type_point> Points;
	GLdouble rotation;
	GLubyte ColorR, ColorG, ColorB, PointSize;
	unsigned int type_of_gr = 0;
};

vector <group_of_objects> groups;


/* Функция вывода на экран */
void Display(void)
{
	glClearColor(0.5, 0.5, 0.5, 1); glClear(GL_COLOR_BUFFER_BIT);

	int size_group = groups.size();
	for (int k = 0; k < size_group; k++)
	{
		glColor3ub(groups[k].ColorR, groups[k].ColorG, groups[k].ColorB);
		glPointSize(groups[k].PointSize);
		//glMatrixMode();
		//glLoadIdentity();
		glBegin(groups[k].type_of_gr);
		for (int i = 0; i < groups[k].Points.size(); i++)
			glVertex2i(groups[k].Points[i].x, groups[k].Points[i].y);
		glEnd();
	}
	glFinish();
}

/* Функция изменения размеров окна */
void Reshape(GLint w, GLint h)
{
	Width = w;    Height = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, 0, h, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/* Функция обработки сообщений от клавиатуры */
void Keyboard(unsigned char key, int x, int y)
{
	bool rot_flag = false;
	int i, n = groups[i_groups].Points.size();

	/* Изменение примитива */
	if (key == '8')
	{
		if (groups[i_groups].type_of_gr == 9)
			groups[i_groups].type_of_gr = 0;
		else
			groups[i_groups].type_of_gr++;
	}
	if (key == '7')
	{
		if (groups[i_groups].type_of_gr == 0)
			groups[i_groups].type_of_gr = 9;
		else
			groups[i_groups].type_of_gr--;
	}

	/* Изменение RGB-компонент цвета точек */
	if (key == 'r') groups[i_groups].ColorR += 5;
	if (key == 'g') groups[i_groups].ColorG += 5;
	if (key == 'b') groups[i_groups].ColorB += 5;

	/* Изменение XY-кординат точек */
	if (key == 'w') for (i = 0; i < n; i++) groups[i_groups].Points[i].y += 5;
	if (key == 's') for (i = 0; i < n; i++) groups[i_groups].Points[i].y -= 5;
	if (key == 'a') for (i = 0; i < n; i++) groups[i_groups].Points[i].x -= 5;
	if (key == 'd') for (i = 0; i < n; i++) groups[i_groups].Points[i].x += 5;

	/* Угол*/
	if (key == 'q')
	{
		groups[i_groups].rotation += 0.0436332306;
		for (i = 0; i < n; i++)
		{
			groups[i_groups].Points[i].x = groups[i_groups].Points[i].x0 * cos(groups[i_groups].rotation) - groups[i_groups].Points[i].y0 * sin(groups[i_groups].rotation);
			groups[i_groups].Points[i].y = groups[i_groups].Points[i].x0 * sin(groups[i_groups].rotation) + groups[i_groups].Points[i].y0 * cos(groups[i_groups].rotation);
			rot_flag = true;
		}
	}

	if (key == 'e')
	{
		groups[i_groups].rotation -= 0.0436332306;
		for (i = 0; i < n; i++)
		{
			groups[i_groups].Points[i].x = groups[i_groups].Points[i].x0 * cos(groups[i_groups].rotation) - groups[i_groups].Points[i].y0 * sin(groups[i_groups].rotation);
			groups[i_groups].Points[i].y = groups[i_groups].Points[i].x0 * sin(groups[i_groups].rotation) + groups[i_groups].Points[i].y0 * cos(groups[i_groups].rotation);
			rot_flag = true;
		}
	}

	/* Изменение размера точек */
	if (key == 'u') 
		groups[i_groups].PointSize++;
	if (key == 'i') 
		groups[i_groups].PointSize--;

	if (key == '0')
	{
		if (i_groups < groups.size() - 1)
			i_groups++;
		else
		{
			groups.push_back(group_of_objects(0, 0, 0, 0, 15));
			i_groups++;
		}
	}
	if (key == '9')
	{
		if (i_groups - 1 < 0)
			i_groups = groups.size() - 1;
		else
			i_groups--;
	}

	glutPostRedisplay();

	char v[50]; 
	//sprintf(v, "Текущий цвет всех точек: R=%.3d G=%.3d B=%.3d", groups[i_groups].ColorR, groups[i_groups].ColorG, groups[i_groups].ColorB);
	glutSetWindowTitle(v);
	
}

/* Функция обработки сообщения от мыши */
void Mouse(int button, int state, int x, int y)
{
	/* клавиша была нажата, но не отпущена */
	if (state != GLUT_DOWN) return;

	/* новая точка по левому клику */
	if (button == GLUT_LEFT_BUTTON)
	{
		type_point p(x, Height - y);
		groups[i_groups].Points.push_back(p);
	}
	/* удаление последней точки по центральному клику */
	if (button == GLUT_MIDDLE_BUTTON)
	{
		groups[i_groups].Points.pop_back();
	}

	glutPostRedisplay();
}

void Menu(int pos)
{
	int key = (keys)pos;

	switch (key)
	{
	case KeyR: Keyboard('r', 0, 0); break;
	case KeyG: Keyboard('g', 0, 0); break;
	case KeyB: Keyboard('b', 0, 0); break;
	case KeyW: Keyboard('w', 0, 0); break;
	case KeyS: Keyboard('s', 0, 0); break;
	case KeyA: Keyboard('a', 0, 0); break;
	case KeyD: Keyboard('d', 0, 0); break;
	case KeyU: Keyboard('u', 0, 0); break;
	case KeyI: Keyboard('i', 0, 0); break;

	default:
		int menu_color = glutCreateMenu(Menu);
		glutAddMenuEntry("Компонента R", KeyR);
		glutAddMenuEntry("Компонента G", KeyG);
		glutAddMenuEntry("Компонента B", KeyB);

		int menu_move = glutCreateMenu(Menu);
		glutAddMenuEntry("Вверх", KeyW);
		glutAddMenuEntry("Вниз", KeyS);
		glutAddMenuEntry("Bлево", KeyA);
		glutAddMenuEntry("Вправо", KeyD);

		int menu_size = glutCreateMenu(Menu);
		glutAddMenuEntry("Увеличить", KeyU);
		glutAddMenuEntry("Уменьшить", KeyI);

		int menu = glutCreateMenu(Menu);
		glutAddSubMenu("Смена цвета", menu_color);
		glutAddSubMenu("Перемещение", menu_move);
		glutAddSubMenu("Изменение размера точки", menu_size);

		glutAttachMenu(GLUT_RIGHT_BUTTON);
		Keyboard(Empty, 0, 0);
	}
}


/* Головная программа */
void main(int argc, char* argv[])
{
	groups.push_back(group_of_objects(0, 0, 0, 0, 15));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(Width, Height);
	glutCreateWindow("Текущий цвет всех точек:");
	Menu(Empty);
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);

	glutMainLoop();
}
