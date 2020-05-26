#define _CRT_SECURE_NO_WARNINGS
#include<stdlib.h>
#include <vector>
#include <fstream>
#include <math.h>
#include "glut.h"

#define M_PI 3.14159265358979323846

using namespace std;

struct TPoint2D			//двумерные точки
{
	double X, Y;
	TPoint2D operator*(TPoint2D k)
	{
		X *= k.X;
		Y *= k.Y;
		return *this;
	}
};

struct TPoint3D			//трехмерные точки
{
	double X, Y, Z;
	vector <TPoint3D> norm;
	TPoint3D()
	{
		X = 0;
		Y = 0;
		Z = 0;
	}
	TPoint3D(double _X, double _Y, double _Z)
	{
		X = _X;
		Y = _Y;
		Z = _Z;
	}
	
	TPoint3D operator+(TPoint3D k)
	{
		TPoint3D n;
		n.X = X + k.X;
		n.Y = Y + k.Y;
		n.Z = Z + k.Z;
		return n;
	}
	
	TPoint3D operator-(TPoint3D k)
	{
		TPoint3D n;
		n.X = X - k.X;
		n.Y = Y - k.Y;
		n.Z = Z - k.Z;
		return n;
	}
	
	bool operator==(TPoint3D k)
	{
		TPoint3D n;
		if (X == k.X && Y == k.Y && Z == k.Z)
			return true;
		return false;
	}
};

vector<TPoint2D> rectangle(4);			//прямоугольник
vector<TPoint3D> trajectory;			//траектория
vector<TPoint2D> coefs;					//коэффициенты для преобразования масштаба

vector<vector<TPoint3D>> Figures;
TPoint3D camPos;
GLint Width = 500, Height = 500;
int cam_y = 30, cam_x = 0;
double zoom = 3;
int texture_num = 4;
bool karkas = false;
bool rot = true;
int material_num = 1;
bool two_light = true;
int light_num = 3;  // light = 0...4, two_light = 0...3
bool normals = true;
bool texture_act = true;
bool smooth_normal = false;
bool grid = true;

int proekcia = 1;	//1 = Перспективная проекция; 0 = Ортографическая проекция

float angle = 60.;	//Угол перспективной проекции
float mn = 1.0;		//Множитель ортографической проекции
unsigned char* Pixels;
vector<TPoint3D> Normals;

void add_norm(TPoint3D a, TPoint3D n)
{
	bool flag = false;
	for (auto& b : Normals)
	{
		if (b == a)
		{
			b.norm.push_back(n);
			flag = true;
			break;
		}
	}
	if (!flag)
	{
		Normals.push_back(a);
		Normals[Normals.size() - 1].norm.push_back(n);
	}
}

vector<TPoint2D> scale(TPoint2D coef, vector<TPoint2D> rect)
{
	vector<TPoint2D> temp;
	for (int i = 0; i < rect.size(); i++)
		temp.push_back(rect[i] * coef);
	
	return temp;
}

//Считываем данные
int input()
{
	ifstream in2D("2D.txt");
	ifstream in3D("3D.txt");
	ifstream coef("coef.txt");

	if (!in2D.is_open() || !in3D.is_open() || !coef.is_open())
		return 0;

	TPoint3D temp3D(0, 0, 0);
	TPoint2D temp2D;
	for (int i = 0; i < 4; i++)
	{
		in2D >> rectangle[i].X;
		in2D >> rectangle[i].Y;
	}
	while (!in3D.eof())
	{
		in3D >> temp3D.X;
		in3D >> temp3D.Y;
		in3D >> temp3D.Z;
		trajectory.push_back(temp3D);
	}
	while (!coef.eof())
	{
		coef >> temp2D.X;
		coef >> temp2D.Y;
		coefs.push_back(temp2D);
	}
	vector<TPoint3D> tempV3D;
	vector<TPoint2D> tempV2D;
	for (int j = 0; j < coefs.size(); j++)
	{
		for (int i = 0; i < 4; i++)
		{
			tempV2D = scale(coefs[j], rectangle);
			tempV3D.push_back(TPoint3D(tempV2D[i].X + trajectory[j].X, tempV2D[i].Y + trajectory[j].Y, trajectory[j].Z));
		}
		Figures.push_back(tempV3D);
		tempV3D.clear();
	}

	return 1;
}

TPoint3D mult(TPoint3D v1, TPoint3D v2)
{
	return TPoint3D(v1.Y * v2.Z - v1.Z * v2.Y, v1.Z * v2.X - v1.X * v2.Z, v1.X * v2.Y - v1.Y * v2.X);
}

double Norm(TPoint3D vec)
{
	double res = 0;
	res += vec.X * vec.X;
	res += vec.Y * vec.Y;
	res += vec.Z * vec.Z;
	return sqrt(res);
}

void Draw3DSGrid()
{
	glColor3ub(0, 0, 255);
	for (float i = -10; i <= 10; i += 0.5)
	{
		glBegin(GL_LINES);
		// Ось Х
		glVertex3f(i, -100, 0);
		glVertex3f(i, 100, 0);
		// Ось Y
		glVertex3f(-100, i, 0);
		glVertex3f(100, i, 0);
		glEnd();
	}
}

TPoint3D DrawPoint(TPoint3D* p) //Вычисление нормы фигуры
{
	TPoint3D norm;
	norm = mult(p[3] - p[0], p[1] - p[0]);
	norm.X /= Norm(norm);
	norm.Y /= Norm(norm);
	norm.Z /= Norm(norm);
	glEnd();
	return norm;
}

void Light()
{
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT2);
	glDisable(GL_LIGHT3);
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);		//Задаем модель освещени
	GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };		//Белый свет
	GLfloat ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };		//Цвет фонового излучения
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };		//Спектральное отражение от объекта
	GLfloat position[] = { 1.0f, 1.0f, -1.0f, 0.0f };	//Позиция лампы для фонового освещения
	GLfloat position1[] = { 1.0f, 3.0f, -10.0f, 1.0f };	//Положение для точечного света
	GLfloat position2[] = { 1.0f, 3.0f, 10.0f, 1.0f };	//Положение для прожектора
	GLfloat spot_direction[] = { .0f, .0f, 0.0f };			//Направление на центр плоскости

	if (light_num == 0) //Выключение освещения
		glDisable(GL_LIGHTING);

	if (light_num == 1) //Точечный источник света
	{
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
		glLightfv(GL_LIGHT0, GL_POSITION, position);
	}

	if (light_num == 2) //Точечный источник света
	{
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
		glLightfv(GL_LIGHT0, GL_POSITION, position1);
	}

	if (light_num == 3) //Точечный источник света с убыванием интенсивности
	{
		glEnable(GL_LIGHT1);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
		glLightfv(GL_LIGHT1, GL_POSITION, position1);
		glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0);
		glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.001);
		glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.00001);
	}

	if (light_num == 4) //прожектор
	{
		glEnable(GL_LIGHT2);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT2, GL_SPECULAR, specular);
		glLightfv(GL_LIGHT2, GL_POSITION, position2);
		glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 100);					//Угловая ширина луча
		glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_direction);	//Направление света
	}
}

void Two_light()
{
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT2);
	glDisable(GL_LIGHT3);
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);		//Задаем модель освещени
	GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };			//Белый свет
	GLfloat ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };			//Цвет фонового излучения
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };		//Спектральное отражение от объекта
	GLfloat position[] = { 1.0f, 1.0f, -1.0f, 0.0f };		//Позиция лампы для фонового освещения
	GLfloat position1[] = { 1.0f, 3.0f, -10.0f, 1.0f };		//Положение для точечного света
	GLfloat position2[] = { 1.0f, 3.0f, 10.0f, 1.0f };		//Положение для прожектора
	GLfloat spot_direction[] = { .0f, .0f, 0.0f };			//Направление на центр плоскости
	if (light_num == 0) //Точечный и прожектор
	{
		glEnable(GL_LIGHT1);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
		glLightfv(GL_LIGHT1, GL_POSITION, position1);
		glEnable(GL_LIGHT2);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT2, GL_SPECULAR, specular);
		glLightfv(GL_LIGHT2, GL_POSITION, position2);
		glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 20.0); //Угловая ширина луча
		glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_direction); //Направление света
	}
	if (light_num == 1) //Направленный и прожектор
	{
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		glEnable(GL_LIGHT2);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT2, GL_SPECULAR, specular);
		glLightfv(GL_LIGHT2, GL_POSITION, position2);
		glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 20.0); //Угловая ширина луча
		glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_direction); //Направление света
	}
	if (light_num == 2) //Направленный и точечный
	{
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		glEnable(GL_LIGHT1);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
		glLightfv(GL_LIGHT1, GL_POSITION, position1);
	}
	if (light_num == 3) //Направленный, точечный и прожектор
	{
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		glEnable(GL_LIGHT1);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
		glLightfv(GL_LIGHT1, GL_POSITION, position1);
		glEnable(GL_LIGHT2);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT2, GL_SPECULAR, specular);
		glLightfv(GL_LIGHT2, GL_POSITION, position2);
		glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 20.0);					//Угловая ширина луча
		glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_direction);	//Направление света
	}
}

void Material()
{
	GLfloat mat[] = { 0.0, 255.0, 255.0, 1.0 };
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	if (material_num == 0)	//Диффузное отражение
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat);

	if (material_num == 1)	//Зеркальное отражение
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat);

	if (material_num == 2)	//Поверхностное отражение
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat);
}

bool LoadTextureFromFile(char* FileName, int& Width, int& Height, GLenum& Type)
{
	unsigned char data54[54]; // служебный заголовок 54 байта

	FILE* file = fopen(FileName, "rb");
	if (!file)
		return(false);
	int err = fread(data54, 1, 54, file);
	if (err != 54)
		return(false);
	int size = *(data54 + 10);
	if (size != 54)
		return(false);
	Width = *(data54 + 18);
	Height = *(data54 + 22);
	Pixels = new unsigned char[Width * Height * 3];
	err = fread(Pixels, Width * Height * 3, 1, file);
	if (err != 1)
		return (false);
	fclose(file);
	Type = GL_BGR_EXT;
	return (true);
}

void DrawFigure()		//Рисовать рисунок
{
	int width = 128, height = 128; // разрешение текстуры (ширина и высота)
	unsigned int tex;

	GLenum type;
	char texture[25];

	if (texture_num == 1)
		strcpy(texture, "texture1.bmp");
	else
		if (texture_num == 2)
			strcpy(texture, "texture2.bmp");
		else
			if (texture_num == 3)
				strcpy(texture, "texture3.bmp");
			else
				if (texture_num == 4)
					strcpy(texture, "texture4.bmp");
				else
					if (texture_num == 5)
						strcpy(texture, "texture5.bmp");
	if (!LoadTextureFromFile(texture, width, height, type))
		return;

	if (texture_act == true)
	{
		glEnable(GL_TEXTURE_2D);			//Разрешение отображения текстур
		glGenTextures(1, &tex);				//Генерация массива номеров текстур
		glBindTexture(GL_TEXTURE_2D, tex);	//Выбор текущей текстуры по номеру
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, Pixels);	//Задание режима выравнивания
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);	//Задание режима учета параметров материала
	}
	else
		glDisable(GL_TEXTURE_2D);



	TPoint3D Figure[4];
	TPoint3D norm;
	//Если вершина, ее цвет и нормаль хранятся подряд 
	//(в соседних позициях общего массива), тогда в качестве
	//сдвига нужно задавать не 0, а суммарную размерность 
	//в байтах элементов цвета и нормали.
	glEnableClientState(GL_VERTEX_ARRAY);	//с параметром GL_VERTEX_ARRAY для вершин
	glEnableClientState(GL_COLOR_ARRAY);	//для цвета
	glEnableClientState(GL_NORMAL_ARRAY);	//для нормалей
	glEnable(GL_NORMALIZE);
	if (karkas)								//отобразить только грани без заливки?
	{
		glLineWidth(3);						//толщину линий
		glColor3ub(127, 255, 150);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	//Другой вариант изменения 
		glDisable(GL_TEXTURE_2D);
		//режима отрисовки качестве параметров которой передается тип граней 
		//многоугольника и режим отображения
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glLineWidth(1);
	}
	vector<TPoint2D> texCoord(4);
	texCoord[0].X = 0; texCoord[0].Y = 0;
	texCoord[1].X = 0; texCoord[1].Y = 1;
	texCoord[2].X = 1; texCoord[2].Y = 1;
	texCoord[3].X = 1; texCoord[3].Y = 0;
	for (int i = 0; i < 4; i++)
		Figure[i] = Figures[0][i];
	norm = DrawPoint(Figure);

	glEnable(GL_LIGHTING); //Включение освещения

	glBegin(GL_QUADS);	//Каждая отдельная четверка  
						//вершин определяет четырехугольник
	for (int i = 0; i < 4; i++)
	{
		glNormal3f(norm.X, norm.Y, norm.Z);					//Для задания нормали
		glTexCoord2f(texCoord[i].X, texCoord[i].Y);			//привязка текстур к примитивам
		glVertex3f(Figure[i].X, Figure[i].Y, Figure[i].Z);	//задание точек
	}
	glEnd();

	for (int j = 0; j < Figures.size() - 1; j++)
	{
		for (int k = 0; k < 3; k++)
		{
			Figure[0] = Figures[j][k];
			Figure[1] = Figures[j][k + 1];
			Figure[2] = Figures[j + 1][k + 1];
			Figure[3] = Figures[j + 1][k];
			norm = DrawPoint(Figure);
			glBegin(GL_QUADS);	//Каждая отдельная четверка  
								//вершин определяет четырехугольник
			for (int i = 0; i < 4; i++)
			{
				glNormal3f(norm.X, norm.Y, norm.Z);				//Для задания нормали
				glTexCoord2f(texCoord[i].X, texCoord[i].Y);
				glVertex3f(Figure[i].X, Figure[i].Y, Figure[i].Z);
			}
			glEnd();
		}
		Figure[0] = Figures[j][3];
		Figure[1] = Figures[j][0];
		Figure[2] = Figures[j + 1][0];
		Figure[3] = Figures[j + 1][3];
		norm = DrawPoint(Figure);
		glBegin(GL_QUADS);	//Каждая отдельная четверка  
							//вершин определяет четырехугольник


		for (int i = 0; i < 4; i++)
		{
			glNormal3f(norm.X, norm.Y, norm.Z);	//Для задания нормали
			//«Привязывание» текстуры к примитиву
			glTexCoord2f(texCoord[i].X, texCoord[i].Y);
			//Задание трехмерных объектов
			glVertex3f(Figure[i].X, Figure[i].Y, Figure[i].Z);
		}
		glEnd();
	}
	int j = Figures.size() - 1;
	for (int i = 0; i < 4; i++)
		Figure[i] = Figures[j][i];
	norm = DrawPoint(Figure);
	glBegin(GL_QUADS);	//Каждая отдельная четверка  
						//вершин определяет четырехугольник

	for (int i = 0; i < 4; i++)
	{
		glNormal3f(norm.X, norm.Y, norm.Z);	//Для задания нормали
		//«Привязывание» текстуры к примитиву
		glTexCoord2f(texCoord[i].X, texCoord[i].Y);
		//Задание трехмерных объектов
		glVertex3f(Figure[i].X, Figure[i].Y, Figure[i].Z);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
}

void DrawNormals()
{
	TPoint3D Figure[4];
	TPoint3D norm;
	int i, j;
	for (i = 0; i < 4; i++)
		Figure[i] = Figures[0][i];
	norm = DrawPoint(Figure);
	glBegin(GL_LINES);
	for (int i = 0; i < 4; i++)
	{
		glVertex3f(Figure[i].X, Figure[i].Y, Figure[i].Z);
		glVertex3f(Figure[i].X + norm.X / 5, Figure[i].Y + norm.Y / 5, Figure[i].Z - norm.Z / 5);
	}
	glEnd();

	for (j = 0; j < Figures.size() - 1; j++)
	{
		for (int k = 0; k < 3; k++)
		{
			Figure[0] = Figures[j][k];
			Figure[1] = Figures[j][k + 1];
			Figure[2] = Figures[j + 1][k + 1];
			Figure[3] = Figures[j + 1][k];
			norm = DrawPoint(Figure);
			glBegin(GL_LINES);
			for (int i = 0; i < 4; i++)
			{
				glVertex3f(Figure[i].X, Figure[i].Y, Figure[i].Z);
				glVertex3f(Figure[i].X + norm.X / 5, Figure[i].Y + norm.Y / 5, Figure[i].Z + norm.Z / 5);
			}
			glEnd();
		}
		Figure[0] = Figures[j][3];
		Figure[1] = Figures[j][0];
		Figure[2] = Figures[j + 1][0];
		Figure[3] = Figures[j + 1][3];
		norm = DrawPoint(Figure);
		glBegin(GL_LINES);
		for (int i = 0; i < 4; i++)
		{
			glVertex3f(Figure[i].X, Figure[i].Y, Figure[i].Z);
			glVertex3f(Figure[i].X + norm.X / 5, Figure[i].Y + norm.Y / 5, Figure[i].Z + norm.Z / 5);
		}
		glEnd();
	}

	for (int i = 0; i < 4; i++)
		Figure[i] = Figures[j][i];
	norm = DrawPoint(Figure);
	glBegin(GL_LINES);
	for (int i = 0; i < 4; i++)
	{
		glVertex3f(Figure[i].X, Figure[i].Y, Figure[i].Z);
		glVertex3f(Figure[i].X + norm.X / 5, Figure[i].Y + norm.Y / 5, Figure[i].Z + norm.Z / 5);
	}
	glEnd();
}

void DrawSmoothNormals()
{
	TPoint3D Figure[4];
	TPoint3D norm;
	int i, j;
	for (i = 0; i < 4; i++)
		Figure[i] = Figures[0][i];
	norm = DrawPoint(Figure);
	norm.Z = -norm.Z;
	for (i = 0; i < 4; i++)
		add_norm(Figure[i], norm);

	for (j = 0; j < Figures.size() - 1; j++)
	{
		for (int k = 0; k < 3; k++)
		{
			Figure[0] = Figures[j][k];
			Figure[1] = Figures[j][k + 1];
			Figure[2] = Figures[j + 1][k + 1];
			Figure[3] = Figures[j + 1][k];
			norm = DrawPoint(Figure);
			for (int i = 0; i < 4; i++)
				add_norm(Figure[i], norm);
		}
		Figure[0] = Figures[j][3];
		Figure[1] = Figures[j][0];
		Figure[2] = Figures[j + 1][0];
		Figure[3] = Figures[j + 1][3];
		norm = DrawPoint(Figure);
		for (i = 0; i < 4; i++)
			add_norm(Figure[i], norm);
	}

	for (i = 0; i < 4; i++)
		Figure[i] = Figures[j][i];
	norm = DrawPoint(Figure);
	for (i = 0; i < 4; i++)
	{
		add_norm(Figure[i], norm);
	}

	glBegin(GL_LINES);
	i = 0;
	for (auto& a : Normals)
	{
		if (i < 4 || i >= Normals.size() - 4)
		{
			norm = a.norm[0] + a.norm[1] + a.norm[2];
			glVertex3f(a.X, a.Y, a.Z);
			glVertex3f(a.X + norm.X / 5, a.Y + norm.Y / 5, a.Z + norm.Z / 5);
			i++;
		}
		else
		{
			norm = a.norm[0] + a.norm[1] + a.norm[2] + a.norm[3];
			glVertex3f(a.X, a.Y, a.Z);
			glVertex3f(a.X + norm.X / 5, a.Y + norm.Y / 5, a.Z + norm.Z / 5);
			i++;
		}
	}
	glEnd();

	glLineWidth(1);
}

//Функция отрисовки
void Display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//отчистка
	glClearColor(0.3, 0.3, 0.3, 1.0);

	//glColor4b(255, 140, 0, 1);
	DrawFigure();				//Рисование самой фигуры	
	glLineWidth(1);
	if (grid)
		Draw3DSGrid();			//Построение сетки плоскости
	glLineWidth(0.5);
	glColor3ub(0, 255, 0);
	if (normals)
		DrawNormals();
	glColor4b(120, 10, 30, 1);
	if (smooth_normal)
		DrawSmoothNormals();
	glFinish();					//Используется для переключения буферов
	glutSwapBuffers();
}

TPoint3D Polar2Normal(float x, float y, float r)
{
	TPoint3D pos;
	pos.Y = r * (double)cos(x) * (double)cos(y);
	pos.Z = r * (double)sin(x);
	pos.X = r * (double)cos(x) * (double)sin(y);
	return pos;
}

void Reshape(GLint w, GLint h)	//Функция изменения размеров окна
{
	Width = w;
	Height = h;

	//Bычисляем соотношение между шириной и высотой, предотвратив деление на 0
	if (Height == 0) Height = 1;
	float ratio = 1. * Width / Height;

	glViewport(0, 0, w, h);		//Определяем окно просмотра
	glMatrixMode(GL_PROJECTION);//Устанавливаем матрицу проекции
	glLoadIdentity();			//Загружаем единичную матрицу

	//Перспективная проекция
	if (proekcia)
		gluPerspective(angle, ratio, 0.1f, 100.0f);

	//Ортографическая проекция
	else
	{
		if (Width >= Height)
			glOrtho(-mn * ratio, mn * ratio, -mn, mn, -100.0f, 100.0f);
		else
			glOrtho(-mn, mn, -mn / ratio, mn / ratio, -100.0f, 100.0f);
	}
	camPos = Polar2Normal(cam_y * (float)M_PI / 180, cam_x * (float)M_PI / 180, zoom);
	gluLookAt(camPos.X, camPos.Y, camPos.Z, 0, 0, 0, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW); //Возврат к матрице модели
	glLoadIdentity();
}

enum keys
{
	KeyW, KeyA, KeyS, KeyD,
	KeyZ, KeyX, KeyC, KeyV, KeyB, KeyN,
	KeyO, KeyP, KeySpace,
	Key1, Key2, Key3, Key4, Key5,
	Key_Num10, Key_Num11, Empty
};

void Keyboard(unsigned char key, int xx, int yy)
{
	if (key == '1')
		texture_num = 1;

	if (key == '2')
		texture_num = 2;

	if (key == '3')
		texture_num = 3;

	if (key == '4')
		texture_num = 4;

	if (key == '5')
		texture_num = 5;

	if (key == 'z') //Каркасный режим
	{
		karkas = !karkas;
		texture_act = false;
	}

	if (key == 'x') //Наложение текстуры
	{
		texture_act = !texture_act;
	}

	if (key == 'c') //Показ нормалей
		normals = !normals;

	if (key == 'v') //Сглаженные нормали
		smooth_normal = !smooth_normal;

	if (key == 'b') //Сглаженные нормали
		grid = !grid;

	if (key == 'p') //Перспективная проекция
	{
		proekcia = 1;
		glutReshapeWindow(Width + 1, Height);
	}

	if (key == 'o') //Ортографическая проекция
	{
		proekcia = 0;
		glutReshapeWindow(Width - 1, Height);
	}

	if (key == 'w')
	{
		if (cam_y + 15 < 90)
		{
			if (cam_y == -89 || cam_y + 15 == 90)
				cam_y += 14;
			else
				cam_y += 15;
			Reshape(Width, Height);
		}
	}
	if (key == 's')
	{
		if (cam_y - 15 > -90)
		{
			if (cam_y == 89 || cam_y - 15 == -90)
				cam_y -= 14;
			else
				cam_y -= 15;
			Reshape(Width, Height);
		}
	}
	if (key == 'a')
	{
		cam_x += 15;
		Reshape(Width, Height);

	}
	if (key == 'd')
	{
		cam_x -= 15;
		Reshape(Width, Height);
	}
	if (key == '-')
	{
		zoom += 0.5;
		Reshape(Width, Height);
	}
	if (key == '+')
	{
		zoom -= 0.5;
		Reshape(Width, Height);
	}
}

void Menu(int pos)
{
	int key = (keys)pos;

	switch (key)
	{
	case Key1: Keyboard('1', 0, 0); break;
	case Key2: Keyboard('2', 0, 0); break;
	case Key3: Keyboard('3', 0, 0); break;
	case Key4: Keyboard('4', 0, 0); break;

	case KeyW: Keyboard('w', 0, 0); break;
	case KeyS: Keyboard('s', 0, 0); break;
	case KeyA: Keyboard('a', 0, 0); break;
	case KeyD: Keyboard('d', 0, 0); break;
	case Key_Num10: Keyboard('+', 0, 0); break;
	case Key_Num11: Keyboard('-', 0, 0); break;

	case KeyO: Keyboard('o', 0, 0); break;
	case KeyP: Keyboard('p', 0, 0); break;

	case KeyZ: Keyboard('z', 0, 0); break;
	case KeyX: Keyboard('x', 0, 0); break;
	case KeyC: Keyboard('c', 0, 0); break;
	case KeyV: Keyboard('v', 0, 0); break;
	case KeyB: Keyboard('b', 0, 0); break;

	default:

		int menu_texture = glutCreateMenu(Menu);
		glutAddMenuEntry("Текстура №1 (1)", Key1);
		glutAddMenuEntry("Текстура №2 (2)", Key2);
		glutAddMenuEntry("Текстура №3 (3)", Key3);
		glutAddMenuEntry("Текстура №4 (4)", Key4);

		int menu_move = glutCreateMenu(Menu);
		glutAddMenuEntry("Вверх (W)", KeyW);
		glutAddMenuEntry("Вниз (S)", KeyS);
		glutAddMenuEntry("Bлево (A)", KeyA);
		glutAddMenuEntry("Вправо (D)", KeyD);
		glutAddMenuEntry("Приблизить камеру (Num '-')", Key_Num11);
		glutAddMenuEntry("Отдалить камеру (Num '+')", Key_Num10);

		int menu_projection = glutCreateMenu(Menu);
		glutAddMenuEntry("Перспективная проекция (P)", KeyP);
		glutAddMenuEntry("Ортографическая проекция (O)", KeyO);

		int menu_show = glutCreateMenu(Menu);
		glutAddMenuEntry("Каркасный режим (Z)", KeyZ);
		glutAddMenuEntry("Наложение текстуры (X)", KeyX);
		glutAddMenuEntry("Показ нормалей (С)", KeyC);
		glutAddMenuEntry("Показ сглаженных нормалей (V)", KeyV);

		//Интерфейс меню
		int menu = glutCreateMenu(Menu);
		glutAddSubMenu("Отображение", menu_show);
		glutAddSubMenu("Проекции", menu_projection);
		glutAddSubMenu("Перемещение по экрану", menu_move);
		glutAddSubMenu("Смена текстуры", menu_texture);

		glutAttachMenu(GLUT_RIGHT_BUTTON);
		Keyboard(Empty, 0, 0);
	}
}

//Головная программа 
int main(int argc, char* argv[])
{
	if (!input())
		return -1;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);		//Включение двойной буфферизации
	glutInitWindowSize(Width, Height);
	glutCreateWindow("Lab_work_2");
	glutReshapeFunc(Reshape);

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);				//Включение буфера глубины

	glutDisplayFunc(Display);
	if (two_light)
		Two_light();
	else
		Light();
	Material();

	glutIdleFunc(Display);

	glutKeyboardFunc(Keyboard);
	Menu(Empty);
	//glutMouseFunc(Mouse);



	glutMainLoop();

	return 1;
}
