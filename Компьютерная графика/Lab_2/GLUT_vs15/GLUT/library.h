#include <iostream>
#include <vector>

#define eps 1e-7
using namespace std;
typedef float type_coordinates;

typedef struct sk {
	type_coordinates shift_x, shift_y, shift_z; // Сдивиги
	type_coordinates value_scale; // Масштабирование
	type_coordinates alpha, betha; // Вращение

	sk() {
		this->shift_x = 0;
		this->shift_y = 0;
		this->shift_z = 0;
		this->alpha = 0;
		this->betha = 0;

		this->value_scale = 1;
	}


} system_coord;

system_coord SystemCoordinates;


typedef struct Point2D
{
	type_coordinates x, y;

	Point2D(type_coordinates x_, type_coordinates y_) {
		this->x = x_;
		this->y = y_;
	}
} point_2d;


typedef struct Point3D			//трехмерные точки
{
	type_coordinates x, y, z;

	Point3D() {
		this->x = 0;
		this->y = 0;
		this->z = 0;

	}

	Point3D(type_coordinates x_, type_coordinates y_, type_coordinates z_) {
		this->x = x_;
		this->y = y_;
		this->z = z_;
	}

	template <typename T>
	Point3D operator/(T size) {
		this->x /= size;
		this->y /= size;
		this->z /= size;
		return *this;
	}

	Point3D operator-(Point3D a) {
		Point3D k(this->x - a.x, this->y - a.y, this->z - a.z);
		return k;
	}

	Point3D operator+(Point3D z)
	{
		Point3D k;
		k.x = this->x + z.x;
		k.y = this->y + z.y;
		k.z = this->z + z.z;
		return k;
	}

	bool operator==(Point3D z) {
		if (this->x == z.x && this->y == z.y)
			return true;
		return false;
	}
} point_3d;

typedef struct carcass_obj {
	Point3D a, b;
	bool is_intersection;

	carcass_obj(Point3D z, Point3D f) {
		this->a = z;
		this->b = f;
		this->is_intersection = 0;
	}

	carcass_obj() {
		this->is_intersection = 0;
	}

	carcass_obj operator=(carcass_obj f) {
		this->a = f.a;
		this->b = f.b;
		this->is_intersection = f.is_intersection;
		return *this;
	}

	bool operator==(carcass_obj k) {
		if (this->a == k.a && this->b == k.b)
			return true;

		if (this->b == k.a && this->a == k.b)
			return true;

		return false;
	}
} carcass_obj;

typedef struct section {
	vector<point_3d> points;
	vector<carcass_obj> ribs;
	point_3d vec_normal;
	vector<int> sequence;
} section;

typedef struct object
{
	vector<point_3d> mass_point;
	vector<section> platan;
	point_3d center_of_mass;
}  Object;

typedef struct vector_ {
	type_coordinates x, y, z;

	vector_(point_3d b, point_3d a) {
		this->x = a.x - b.x;
		this->y = a.y - b.y;
		this->z = a.z - b.z;
	}
} vector_;

carcass_obj create_line(Point3D a, Point3D b)
{
	carcass_obj k(a, b);
	return k;
}

point_3d vec_mult(point_3d a, point_3d b)
{
	point_3d k(a.y * b.z - b.y * a.z, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
	return k;
}

point_3d vec_mult(vector_ a, vector_ b)
{
	point_3d k(a.y * b.z - b.y * a.z, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
	return k;
}

point_3d get_vec_normal(section& sc)
{
	point_3d sup, vec_1, vec_2;

	// Инициализация двух векторов плоскости
	if (sc.points.size() > 2)
	{
		vec_1 = sc.points[1] - sc.points[0];
		vec_2 = sc.points[2] - sc.points[0];
	}

	sup = vec_mult(vec_1, vec_2); // здесь надо продумать

	type_coordinates z = sqrt(pow(sup.x, 2) + pow(sup.y, 2) + pow(sup.z, 2));
	sup = sup / z;

	return sup;
}

void init_ribs(section& sk) {
	point_3d point_1, point_2;

	for (int j = 0; j < sk.points.size(); j++)
	{
		point_1 = sk.points[j];
		for (int k = j + 1; k < sk.points.size(); k++)
		{
			point_2 = sk.points[k];
			sk.ribs.push_back(create_line(point_1, point_2));
		}
	}
}

bool have_common_line(object fig, int i, int j, int& index_edge_i, int& index_edge_j)
{
	int count_i = fig.platan[i].ribs.size(), count_j = fig.platan[j].ribs.size();
	for (index_edge_i = 0; index_edge_i < count_i; index_edge_i++)
		for (index_edge_j = 0; index_edge_j < count_j; index_edge_j++)
			if (fig.platan[i].ribs[index_edge_i] == fig.platan[j].ribs[index_edge_j])
				return 1;

	return 0;
}

bool have_platan_the_point(section s, Point3D p) {
	for (int i = 0; i < s.points.size(); i++)
		if (p == s.points[i])
			return true;
	return false;
}

type_coordinates norma_vec(Point3D K) {
	return sqrt(pow(K.x, 2) + pow(K.y, 2) + pow(K.z, 2));
}

bool are_they_parallel(object fig, int i, int j) {
	if (norma_vec(fig.platan[i].vec_normal - fig.platan[j].vec_normal) < eps)
		return true;

	if (norma_vec(fig.platan[i].vec_normal + fig.platan[j].vec_normal) < eps)
		return true;

	return false;
}

bool same_sign(type_coordinates a, type_coordinates b)
{
	if (a > 0 && b > 0 || a < 0 && b < 0 || a == 0 && b == 0)
		return true;
	return false;
}

bool check_intersection(carcass_obj a, carcass_obj b) {
	if (a.a == b.a || a.a == b.b
		|| a.b == b.a || a.b == b.b)
		return false; // либо исходят из одной точки, либо вообще совпадают




	point_3d A(a.a), B(a.b), C(b.a), D(b.b), f, g;
	vector_ AB(A, B), AC(A, C), AD(A, D);

	f = vec_mult(AB, AD);
	g = vec_mult(AC, AB);

	if (same_sign(f.x, g.x) && same_sign(f.y, g.y) && same_sign(f.z, g.z))
		return true;

	return false;
}

int init_figure(object& fig)
{
	// Ищем центр масс
	point_3d sup(0, 0, 0);
	int count_points = fig.mass_point.size();
	for (int i = 0; i < count_points; i++)
		sup = sup + fig.mass_point[i];
	sup = sup / count_points;
	fig.center_of_mass = sup;
	//

	// Считаем всевозможные сечения 
	section plat;
	point_3d point_1, point_2, point_3;
	for (int i = 0; i < count_points; i++)
		for (int j = i + 1; j < count_points; j++)
			for (int k = j + 1; k < count_points; k++)
			{
				point_1 = fig.mass_point[i];
				point_2 = fig.mass_point[j];
				point_3 = fig.mass_point[k];

				// Инициализация Сечения
				plat.points.push_back(point_1);
				plat.points.push_back(point_2);
				plat.points.push_back(point_3);
				plat.vec_normal = get_vec_normal(plat);
				// 

				fig.platan.push_back(plat);
				plat.points.clear();
			}
	//

	// Инициализируем ребра для плоскостей
	for (int i = 0; i < fig.platan.size(); i++) // выбираем грань
		init_ribs(fig.platan[i]);
	//

	// Ищем плоскости, которые имеют общую грань и параллельны

	int indx_1, indx_2; // Индексы для определения общих граней



	bool flag = false; // Если никаких изменений не произшло, тогда заканчиваем внешний цикл, так как ещё раз перебирать бесполезно
	point_3d point_not_general;
	for (int z = 0; z < fig.platan.size(); z++) {
		for (int i = 0; i < fig.platan.size(); i++) // Ходим по плоскостям N^3 раз (Чтобы наверняка)
			for (int j = i + 1; j < fig.platan.size(); j++) // Ходим по плоскостям
				if (have_common_line(fig, i, j, indx_1, indx_2) && are_they_parallel(fig, i, j))
				{


					// добавляение точек в i сечение из j
					for (int i_1 = 0; i_1 < fig.platan[j].points.size(); i_1++)
						fig.platan[i].points.push_back(fig.platan[j].points[i_1]);

					// Удаляем дубликаты точек в i сечении 
					for (int i_1 = 0; i_1 < fig.platan[i].points.size(); i_1++)
						for (int j_1 = i_1 + 1; j_1 < fig.platan[i].points.size(); j_1++)
							if (fig.platan[i].points[i_1] == fig.platan[i].points[j_1])
								fig.platan[i].points.erase(fig.platan[i].points.begin() + j_1);

					fig.platan[i].ribs.clear();
					init_ribs(fig.platan[i]);
					// Мы обязаны проверять, какая точка находится на прямой и удалить её ???

					// Удаляем ненужную плоскости
					fig.platan.erase(fig.platan.begin() + j);
				}
	}
	//


	stack<carcass_obj> Stack_;

	// Помечаем для каждой плоскости пересечения граней
	for (int i = 0; i < fig.platan.size(); i++)
		for (int j = 0; j < fig.platan[i].ribs.size(); j++)
			for (int k = j + 1; k < fig.platan[i].ribs.size(); k++)
				if (check_intersection(fig.platan[i].ribs[j], fig.platan[i].ribs[k]))
				{
					Stack_.push(fig.platan[i].ribs[j]);
					Stack_.push(fig.platan[i].ribs[k]);
				}


	carcass_obj Z;
	while (Stack_.size() != 0) {
		Z = Stack_.top();
		Stack_.pop();

		for (int i = 0; i < fig.platan.size(); i++)
			for (int j = 0; j < fig.platan[i].ribs.size(); j++)
				if (fig.platan[i].ribs[j] == Z)
					fig.platan[i].ribs.erase(fig.platan[i].ribs.begin() + j);
	}
	//	


	// Теперь удаляем лишние плоскости
	for (int i = 0; i < fig.platan.size(); i++)
		if (fig.platan[i].points.size() != fig.platan[i].ribs.size())
		{
			fig.platan.erase(fig.platan.begin() + i);
			i--;
		}
	//
	return 1;
}

int read_func(vector<point_3d>& mas_3d, int& choose)
{
	type_coordinates x_, y_, z_, shift;
	int count;
	ifstream in_pointer("info.txt");

	if (in_pointer.is_open() == false)
		return -1;

	in_pointer >> choose;
	in_pointer >> count;
	ifstream fin;

	switch (choose)
	{
	case 0: // 2D-координаты вершин сечения (считающегося выпуклым)


		fin.open("coordinates_2d.txt");

		if (fin.is_open() == true) {
			fin >> shift;

			for (int i = 0; i < count; i++) {

				fin >> x_ >> y_;
				point_3d k(x_, y_, 0);
				mas_3d.push_back(k);
				point_3d z(x_, y_, shift);
				mas_3d.push_back(z);
			}
		}
		else
			return -1;

		break;
	case 1: // 3D-координаты траектории тиражирования
		fin.open("coordinates_3d.txt");
		if (fin.is_open()) {
			for (int i = 0; i < count; i++) {
				fin >> x_ >> y_ >> z_;
				point_3d k(x_, y_, z_);
				mas_3d.push_back(k);
			}
		}
		else
			return -1;

		break;
	default:
		return -1;
	}

	return 1;
}

