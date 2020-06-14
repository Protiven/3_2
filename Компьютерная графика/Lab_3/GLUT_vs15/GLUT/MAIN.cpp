#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <stack>
#include <math.h>
#include "freeglut.h"
#include <soil.h> // Через Nuget Устанавливаем библиотеки для загрузки текстур


int width = 800, height = 800;

#include "library.h"
#include "spectator.h"

#define ESC 27 // Код клавиши escape


// Основной функционал
struct main_functional {
	int tracer_mode;
	int rotation_mode;
	type scale;

	main_functional() {
		this->rotation_mode = 0;
		this->tracer_mode = 0;
		this->scale = 1;
	}
} System_;



using namespace std;

void Reshape(GLint w, GLint h)
{
	//изменить размеры окна
	width = w;
	height = h;

	if (height == 0)
		height = 1;

	spect.ratio = 1. * width / height;

	if (!System_.tracer_mode)	{
		//установить матрицу проекции/определяет объем сцены
		glMatrixMode(GL_PROJECTION);

		//загрузить единичную матрицу
		glLoadIdentity();

		//определить окно просмотра
		glViewport(0, 0, width, height);

		//использовать перспективную проекциию
		gluPerspective(60, spect.ratio, 0.1f, 100.0f);

		//возврат к матрице модели
		glMatrixMode(GL_MODELVIEW);
	}
	else {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(0, 0, width, height);
		gluOrtho2D(0.0, width, 0.0, height);
	}
}

void Draw3DSGrid() {
	glColor3ub(0, 0.5, 0);
	for (float i = -50; i <= 50; i++) {
		glBegin(GL_LINES);
		// Ось Х
		glVertex3f(-50, 0, i);
		glVertex3f(50, 0, i);

		// Ось Z
		glVertex3f(i, 0, -50);
		glVertex3f(i, 0, 50);
		glEnd();
	}
}

void Display(void)
{
	//очистка буфера цвета и глубины
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//включить буфер глубины
	glEnable(GL_DEPTH_TEST);

	//обнуление трансформации
	glLoadIdentity();

	//установить вид камеры
	gluLookAt(spect.LookFrom.x, spect.LookFrom.y, spect.LookFrom.z,
		spect.LookAt.x, spect.LookAt.y, spect.LookAt.z,
		0, 1, 0);

	//отрисовать сетку
	Draw3DSGrid();

	
	main_scene.draw_scene();

	//есди включен режим трассировки
	if (System_.tracer_mode)
	{
		//установить параметры трассировщика
		//raytracer1 = Raytracer(camera1.Position, scene1, camera1, ratio, width, height, 1);

		//запустить трассировку
		//raytracer1.Raytrace();
	}

	glutPostRedisplay();

	//сменить буфер
	glutSwapBuffers();
}

enum keys { Empty, KeyF, KeyT, KeyC, Key1, Key2, Key3, Key4, Key5, Key6, KeyW, KeyS, KeyA, KeyD, KeyE };



#include "callback_funcs.h"

int main(int argc, char* argv[]) { 
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(width, height);
	glutCreateWindow("Lab 3");								

	// Обработка клавиш клавиатуры и мыши
	glutKeyboardFunc(Keys_);		
	glutSpecialFunc(Callback_special_keys);		
	glutMouseFunc(Mouse);
	glutMouseWheelFunc(mouseWheel);
	
	// Обработка движения мыши при любом изменении
	glutPassiveMotionFunc(Mouse_move);


	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	Menu(Empty);
	glutMainLoop();
}
