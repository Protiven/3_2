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

#define Pi 3.1415926

using namespace std;

object figure;
//point_3d spectator(0, 0, 0.1); // x - расстояние от объекта

bool buff_depth = 1;
bool double_buff = 1;
bool light = 0;
int type_source_light = 0; 

bool carcass = 0;
bool show_normals = 0;

unsigned char* tex_2d = 0; // stat_load_texture ... Сама картинка
int width_text;
int height_text;
bool texture = 0;
bool stat_load_texture = 0;
FILE* file = fopen("texture.bmp", "rb");

bool ortho_or_perspective = 0; // 0 - ортографич, 1 - перспективная
float perspective_angle = 30;
float perspective_ratio = 1;

bool smooth_normal = 0; 
#define Max_size 50
int matrix_smezhnost[Max_size][Max_size];

typedef struct system_sphere {
	type_coordinates Radius, Alpha, Theta;

	system_sphere(type_coordinates rad, type_coordinates alph, type_coordinates beth) {
		this->Radius = rad;
		this->Alpha = alph;
		this->Theta = beth;
	}
}Sphere_syst;
Sphere_syst SystemCoordinate(0, 0, 1);

void clear_matrix(){
	for (int i = 0; i < Max_size; i++)
		for (int j = 0; j < Max_size; j++)
			matrix_smezhnost[i][j] = 0;
}

void create_neib_matrix(section sk) {
	int in1 = 0, in2 = 0;
	for (int i = 0; i < sk.ribs.size(); i++)
	{
		for (int j = 0; j < sk.points.size(); j++)
			if (sk.ribs[i].a == sk.points[j])
				in1 = j;

		for (int j = 0; j < sk.points.size(); j++)
			if (sk.ribs[i].b == sk.points[j])
				in2 = j;

		matrix_smezhnost[in1][in2] = 1;
		matrix_smezhnost[in2][in1] = 1;
	}
}

void step(vector<int>& seq, section sk, int current_index, int prev) {
	for (int i = 0; i < sk.points.size(); i++)
	{
		if (matrix_smezhnost[current_index][i] == 1 && i == 0 && prev != 0)
			break;

		if (matrix_smezhnost[current_index][i] == 1 && i != prev && i != current_index)
		{
			seq.push_back(i);
			step(seq, sk, i, current_index);
				break;
		}
	}

}

void show_normals_func(object fig) {
	for (int i = 0; i < fig.platan.size(); i++)
	{
		point_3d center_of_platan, a, b;
		for (int j = 0; j < fig.platan[i].points.size(); j++)
			center_of_platan = center_of_platan + fig.platan[i].points[j];
		center_of_platan = center_of_platan / fig.platan[i].points.size(); // Нашли середину поверхности
		a = center_of_platan + fig.platan[i].vec_normal / 5;
		b = center_of_platan - fig.platan[i].vec_normal / 5;

		glBegin(GL_LINES); // Просто линия
		glLineWidth(5000);
		glColor3f(0.0f, 0.0f, 0.0f); // Черный

		glVertex3d(a.x - figure.center_of_mass.x
			, a.y - figure.center_of_mass.y
			, a.z - figure.center_of_mass.z);
		glVertex3d(b.x - figure.center_of_mass.x
			, b.y - figure.center_of_mass.y
			, b.z - figure.center_of_mass.z);
		glEnd();
	}

}

void return_text_coord(section &sk, section z) // Пересчитываем текстурные координаты (проблема с переводом из 3-х мерного пространства)
{
	type_coordinates x_min, x_max, y_min, y_max;
	x_min = z.points[0].x;
	x_max = z.points[0].x;
	y_min = z.points[0].y;
	y_max = z.points[0].y;

	sk = z;
	for (int i = 0; i < sk.points.size(); i++)
	{
		if (sk.points[i].x < x_min)
			x_min = sk.points[i].x;
		else if (sk.points[i].x > x_max)
			x_max = sk.points[i].x;

		if (sk.points[i].y < y_min)
			y_min = sk.points[i].y;
		else if (sk.points[i].y > y_max)
			y_max = sk.points[i].y;
	}

	if (x_min < 0)
	{
		for (int i = 0; i < sk.points.size(); i++)
			sk.points[i].x = sk.points[i].x + abs(x_min);
	}
	else if (x_min > 0)
	{
		for (int i = 0; i < sk.points.size(); i++)
			sk.points[i].x = sk.points[i].x - x_min;
	}

	if (y_min < 0)
	{
		for (int i = 0; i < sk.points.size(); i++)
			sk.points[i].y = sk.points[i].y + abs(y_min);
	}
	else if (y_min > 0)
	{
		for (int i = 0; i < sk.points.size(); i++)
			sk.points[i].y = sk.points[i].y - y_min;
	}

	type_coordinates f = y_max - y_min,
		g = x_max - x_min;


	for (int i = 0; i < sk.points.size(); i++)
	{
		sk.points[i].x = sk.points[i].x / g;
		sk.points[i].y = sk.points[i].y / f;
	}
	return;
}

point_3d Sphere_to_Dekart(Sphere_syst SK)
{
	point_3d pos;
	pos.z = SK.Radius * cos(SK.Theta * Pi / 180);
	pos.x = SK.Radius * sin(SK.Theta * Pi / 180) * cos(SK.Alpha * Pi / 180);
	pos.y = SK.Radius * sin(SK.Theta * Pi / 180) * sin(SK.Alpha * Pi / 180);
	return pos;
}

void Draw_smooth_normals() {
	Point3D k(0, 0, 0);
	for (int i = 0; i < figure.mass_point.size(); i++)
	{
		int count_normals_for_point = 0;
		for (int j = 0; j < figure.platan.size(); j++)
			if (have_platan_the_point(figure.platan[j], figure.mass_point[i]))
			{
				k = k + figure.platan[j].vec_normal;
				count_normals_for_point++;
			}

		k = k / count_normals_for_point;

		Point3D a(k.x + figure.mass_point[i].x,
			k.y + figure.mass_point[i].y,
			k.z + figure.mass_point[i].z);

		Point3D b(-k.x + figure.mass_point[i].x,
			-k.y + figure.mass_point[i].y,
			-k.z + figure.mass_point[i].z);

		glBegin(GL_LINES); // Просто линия
		glLineWidth(5000);
		glColor3f(0.0f, 0.0f, 0.0f); // Черный
		glVertex3d(a.x - figure.center_of_mass.x
			, a.y - figure.center_of_mass.y
			, a.z - figure.center_of_mass.z);
		glVertex3d(b.x - figure.center_of_mass.x
			, b.y - figure.center_of_mass.y
			, b.z - figure.center_of_mass.z);
		glEnd();

	}
}

void Display(void)
{
	glClearColor(0.2, 0.2, 0.2, 1); glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	glEnable(GL_NORMALIZE);

	// Включение буффера
	if (buff_depth == 1) {
		glEnable(GL_DEPTH_TEST); // включили Тест глубины
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	else glDisable(GL_DEPTH_TEST);

	// Включение двойного буфера 
	if(double_buff == 1)
		glutInitDisplayMode(GLUT_DOUBLE);
	else
		glutInitDisplayMode(GLUT_SINGLE);



	// Текстурирование
	if (texture == 1)
	{
		unsigned char data54[54]; // служебный заголовок 54 байта

		if (file != NULL) { // Подгрузка текстуры, если файл открыт (после поток вывода из файла зануляется P.S. Все равно, что текстура уже загружена)
			fread(data54, 1, 54, file);
			int size = *(data54 + 10);

			int width = *(data54 + 18);
			int height = *(data54 + 22);
			unsigned char* Pixels = new unsigned char[Width * Height * 3];
			
			fread(Pixels, Width * Height * 3, 1, file);
			fclose(file);
			file = NULL;
			int Type = GL_BGR_EXT;
			GLuint tex;
			glEnable(GL_TEXTURE_2D);				//Разрешение отображения текстур
			glGenTextures(1, &tex);					//Генерация массива номеров текстур
			glBindTexture(GL_TEXTURE_2D, tex);	//Выбор текущей текстуры по номеру
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, Pixels);	//Задание режима выравнивания
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);	//Задание режима учета параметров материала
		}

		for (int i = 0; i < figure.platan.size(); i++) {

			if (figure.platan[i].points.size() > 2)
			{
				glBegin(GL_POLYGON);
				clear_matrix();
				create_neib_matrix(figure.platan[i]);

				if (figure.platan[i].sequence.size() == 0)// Если не закэширована "правильная" последовательность точек, то кэшируем
				{
					figure.platan[i].sequence.push_back(0);
					step(figure.platan[i].sequence, figure.platan[i], 0, 0);
				}

				int k;

				section help;
				return_text_coord(help, figure.platan[i]); // Считаем текстурные координаты

				for (int z = 0; z < figure.platan[i].sequence.size(); z++)
				{
					//glColor3f(1 - float(i) / figure.platan.size(), 1 - float(i) / figure.platan.size(), 1); // Черный
					glColor3f(0.7, 0.7, 0.7);
					// Надо придумать перевод в двумерную точку
					glTexCoord2f(help.points[z].x, help.points[z].y);
					glVertex3d(figure.platan[i].points[figure.platan[i].sequence[z]].x - figure.center_of_mass.x,
						figure.platan[i].points[figure.platan[i].sequence[z]].y - figure.center_of_mass.y,
						figure.platan[i].points[figure.platan[i].sequence[z]].z - figure.center_of_mass.z);
				}

				glEnd();
			}
		}

	}
	else
	{
		// Просто вывод полигонов
		for (int i = 0; i < figure.platan.size(); i++) {

			if (figure.platan[i].points.size() > 2)
			{
				glBegin(GL_POLYGON);
				clear_matrix();
				create_neib_matrix(figure.platan[i]);

				if (figure.platan[i].sequence.size() == 0)// Если не закэширована "правильная" последовательность точек, то кэшируем
				{
					figure.platan[i].sequence.push_back(0);
					step(figure.platan[i].sequence, figure.platan[i], 0, 0);
				}

				int k;

				for (int z = 0; z < figure.platan[i].sequence.size(); z++)
				{
					///glColor3f(1 - float(i) / figure.platan.size(), 1 - float(i) / figure.platan.size(), 1); // Черный
					glVertex3d(figure.platan[i].points[figure.platan[i].sequence[z]].x - figure.center_of_mass.x,
						figure.platan[i].points[figure.platan[i].sequence[z]].y - figure.center_of_mass.y,
						figure.platan[i].points[figure.platan[i].sequence[z]].z - figure.center_of_mass.z);
				}

				glEnd();
			}
		}
		tex_2d = 0;
	}
	
	// Включение света
	if (light == 1)
	{
		glEnable(GL_NORMALIZE);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0); // !!!! В зависимости от типа света нужно менять аргумент
		GLfloat light0_position[] = { 0.0, 0.0, 0.7, 1.0 };
		glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	}
	else
	{
		glDisable(GL_LIGHT0); // !!!! В зависимости от типа света нужно менять аргумент
		glDisable(GL_LIGHTING);
	}


	if (show_normals == 1)
		show_normals_func(figure);


	if (carcass == 1) {

		glDisable(GL_DEPTH_TEST);
		// Рисуем каркасс
		for (int i = 0; i < figure.platan.size(); i++) {
			for (int k = 0; k < figure.platan[i].ribs.size(); k++) {
				glBegin(GL_LINES); // Просто линия
				glLineWidth(3000);
				glColor3f(0.0f, 0.0f, 0.0f); // Черный
				glVertex3d(figure.platan[i].ribs[k].a.x - figure.center_of_mass.x
					, figure.platan[i].ribs[k].a.y - figure.center_of_mass.y
					, figure.platan[i].ribs[k].a.z - figure.center_of_mass.z);
				glVertex3d(figure.platan[i].ribs[k].b.x - figure.center_of_mass.x
					, figure.platan[i].ribs[k].b.y - figure.center_of_mass.y
					, figure.platan[i].ribs[k].b.z - figure.center_of_mass.z);
				glEnd();
			}
		}
	}
	//

	if (smooth_normal)
		Draw_smooth_normals();

	glFinish();
	
	glutSwapBuffers(); // Для двойного буффера
}

void Reshape(GLint w, GLint h)	//Функция изменения размеров окна
{
	Display();
	point_3d spect; 
	Width = w;    Height = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, 0, h, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	Width = w;
	Height = h;

	//Bычисляем соотношение между шириной и высотой, предотвратив деление на 0
	if (Height == 0) Height = 1;
	float ratio = 1. * Width / Height;

	glViewport(0, 0, w, h);		//Определяем окно просмотра
	glMatrixMode(GL_PROJECTION);//Устанавливаем матрицу проекции
	glLoadIdentity();			//Загружаем единичную матрицу

	//Перспективная проекция 
	if (ortho_or_perspective == 1)
		gluPerspective(perspective_angle, perspective_ratio, 0.1f, 100.0f);
	//Ортографическая проекция  
	else
	{
		double aspect = Width / double(Height);
		if (Width >= Height) {
			gluOrtho2D(-1. * aspect, 1. * aspect, -1., 1.);
		}
		else {
			gluOrtho2D(-1., 1., -1. / aspect, 1. / aspect);
		}
	}

	spect = Sphere_to_Dekart(SystemCoordinate);
	gluLookAt(spect.x, spect.y, spect.z, 
		0, 0, 0, 0, 1, 0); 
	glMatrixMode(GL_MODELVIEW); //Возврат к матрице модели
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
	// Свет
	if (key == 'l') {
		light = !light;
	}
	
	//Каркасный режим
	if (key == 'z') {
		carcass = !carcass;
	}

	//Наложение текстуры
	if (key == 'x') {
		texture = !texture;
	}

	if (key == 'c') //Показ нормалей
		show_normals = !show_normals;

	if (key == 'v') //Сглаженные нормали
		smooth_normal = !smooth_normal;

	if (key == 'i') //Глубинный тест
		buff_depth = !buff_depth;


	if (key == 'p') //Перспективная проекция
	{
		ortho_or_perspective = 1;
	}

	if (key == 'o') //Ортографическая проекция
	{
		ortho_or_perspective = 0;
	}

	if (key == 'w') {
		if (SystemCoordinate.Theta + 5 <= 180)
			SystemCoordinate.Theta += 5;
	}

	if (key == 's') {
		if(SystemCoordinate.Theta - 5 > 0)
			SystemCoordinate.Theta -= 5;
	}

	if (key == 'a') {
		if (SystemCoordinate.Alpha + 5 < 360)
			SystemCoordinate.Alpha += 5;
		else
			SystemCoordinate.Alpha = 0;
	}

	if (key == 'd') {
		if (SystemCoordinate.Alpha - 5 >= 0)
			SystemCoordinate.Alpha -= 5;
		else
			SystemCoordinate.Alpha = 360;
	}

	if (key == '-') {
		SystemCoordinate.Radius -= 0.01;
	}
	
	if (key == '=') {
		SystemCoordinate.Radius += 0.01;
	}
	
	
	Reshape(Width, Height);
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

int main(int argc, char* argv[])
{
	int choose_task;
	if (read_func(figure.mass_point, choose_task) != 1) {
		return -1;
	}
	init_figure(figure); // Надо улучшить вектор нормали?


	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(Width, Height);
	glutCreateWindow("Текущий цвет всех точек:");
	Menu(Empty);
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMainLoop();
	return 1;
}
