#define _CRT_SECURE_NO_WARNINGS
#include<stdlib.h>
#include <vector>
#include <fstream>
#include <stack>
#include <math.h>
#include "library.h"
#include "glut.h"
#include <soil.h> // Через Nuget Устанавливаем библиотеки для загрузки текстур

int Width = 800, Height = 800;

using namespace std;

vector<Point> points;
vector<Point> true_array_point;

void Display(void) {
	glClearColor(0.9, 0.9, 0.9, 1); 
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();


	// Рисование осей координат
	draw_axis(Width, Height);
	//

	// Печать точек, к которым будет производиться интерполяция
	true_array_point.clear();
	sort_points(true_array_point); // сортировка точек вдоль оси X медом пузырька
	func_init_true_points(points, true_array_point, Width, Height); // Инициализирует точки относительно центра окна
	int size;
	if ((size = points.size()) != 0) {
		glPointSize(6);
		glBegin(GL_POINTS);
		for (int i = 0; i < size; i++) { 
			glColor3ub(0, 0, 0);
			glVertex2i(true_array_point[i].x + Width / 2, true_array_point[i].y + Height / 2);
		}
		glEnd();
	}
	//


	// Интерполяция
	interpolation_func(points, true_array_point, Width, Height);
	//

	glFinish();
}

void Reshape(GLint w, GLint h)	//Функция изменения размеров окна
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, 0, h, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

enum keys
{
	KeyW, KeyA, KeyS, KeyD,
	KeyZ, KeyX, KeyC, KeyV, KeyB, KeyN,
	KeyO, KeyP, Empty
};

void Keyboard(unsigned char key, int xx, int yy)
{
	char message[50];
	
	if (key == '-') {
		if (n_power_interpolation > 1)
			n_power_interpolation--;
		else {
			sprintf(message, "Power interpolation can't be 0 !!!");
			glutSetWindowTitle(message);
			return;
		}
	}

	if (key == '=') {
			n_power_interpolation++;
	}


	sprintf(message, "Power interpolation %d", n_power_interpolation);
	glutSetWindowTitle(message);
	glutPostRedisplay();
}

void Menu(int pos)
{
	int key = (keys)pos;

	switch (key)
	{

	case KeyW: Keyboard('w', 0, 0); break;
	case KeyS: Keyboard('s', 0, 0); break;
	case KeyA: Keyboard('a', 0, 0); break;
	case KeyD: Keyboard('d', 0, 0); break;

	case KeyO: Keyboard('o', 0, 0); break;
	case KeyP: Keyboard('p', 0, 0); break;

	case KeyZ: Keyboard('z', 0, 0); break;
	case KeyX: Keyboard('x', 0, 0); break;
	case KeyC: Keyboard('c', 0, 0); break;
	case KeyV: Keyboard('v', 0, 0); break;
	case KeyB: Keyboard('b', 0, 0); break;

	default:
		int menu_projection = glutCreateMenu(Menu);
		glutAddMenuEntry("Перспективная проекция (P)", KeyP);
		glutAddMenuEntry("Ортографическая проекция (O)", KeyO);

		int menu_show = glutCreateMenu(Menu);
		glutAddMenuEntry("Каркасный режим (Z)", KeyZ);
		glutAddMenuEntry("Наложение текстуры (X)", KeyX);
		glutAddMenuEntry("Показ нормалей (С)", KeyC);
		glutAddMenuEntry("Показ сглаженных нормалей (V)", KeyV);


		glutAttachMenu(GLUT_RIGHT_BUTTON);
		Keyboard(Empty, 0, 0);
	}
}

void Mouse(int button, int state, int x, int y)  {
	if (state != GLUT_DOWN) return;

	if (button == GLUT_LEFT_BUTTON)	{
		points.push_back(Point(x, Height - y));
	}

	// Удаляем точку ближайшую к положению
	if (button == GLUT_MIDDLE_BUTTON) {
		if (points.size() != 0) {
			int k = 0;
			Point val(1000, 1000);
			for (int i = 0; i < points.size(); i++) 
				if (distance(Point(x, Height - y), val) > distance(points[i], Point(x, Height - y))) {
					val = points[i];
					k = i;
				}

			points.erase(points.begin() + k);
		}
	}

	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	if (!get_info()) return -1;

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

	return 1;
}
