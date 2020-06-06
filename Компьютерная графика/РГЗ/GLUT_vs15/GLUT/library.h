#include "glut.h"

#define count_delim 10
int n_power_interpolation;

using namespace std;

typedef float type_p;

struct Point {
   type_p x, y;

   Point(type_p x_, type_p y_) {
      this->x = x_;
      this->y = y_;
   }
};

void sort_points(vector<Point> &k) {
   Point val(0, 0);

   for (int i = 0; i < k.size(); i++) {          
      for (int j = k.size() - 1; j > i; j--) 
         if (k[i].x > k[j].x) {
            val = k[j - 1]; 
            k[j - 1] = k[j]; 
            k[j] = val;
         }
      }
}

void draw_axis(int width, int height, type_p scale) {
   Point OX_beg(0, height / 2),
      OX_fin(width, height / 2),
      OY_beg(width / 2, 0),
      OY_fin(width / 2, height);

   type_p x_split = abs(OX_fin.x - OX_beg.x) / count_delim,
      y_split = abs(OY_fin.y - OY_beg.y) / count_delim;

   Point center(width / 2, height / 2);

   glLineWidth(0.1);
   glBegin(GL_LINES);
   glColor3d(0, 0, 0);

   glVertex2f(OX_beg.x, OX_beg.y);
   glVertex2f(OX_fin.x, OX_fin.y);



   for (int i = -count_delim / 2; i <= count_delim / 2; i++) {
      glVertex2f(center.x + i * x_split * scale, center.y + 5);
      glVertex2f(center.x + i * x_split * scale, center.y - 5);
   }

   glVertex2f(OY_beg.x, OY_beg.y);
   glVertex2f(OY_fin.x, OY_fin.y);


   for (int i = -count_delim / 2; i <= count_delim / 2; i++) {
      glVertex2f(center.x - 5, center.y + i * y_split * scale);
      glVertex2f(center.x + 5, center.y + i * y_split * scale);
   }
   
   glEnd();
}

type_p L_i_func(vector<Point> arr_points, int i, type_p x) {
   type_p val = 1;

   for (int j = 0; j < arr_points.size(); j++) {
      if (i == j)
         continue;
      else
         val = val * (x - arr_points[j].x) / (arr_points[i].x - arr_points[j].x);
   }
   return val;
}

void func_init_true_points(vector<Point> arr_points, vector<Point> &true_array_point, int width, int height) {
   for (int i = 0; i < arr_points.size(); i++)
      true_array_point.push_back(Point(arr_points[i].x - width / 2, arr_points[i].y - height / 2));
}

void interpolation_func(vector<Point> arr_points, vector<Point> true_array_point, int width, int height, type_p scale) {
   if (arr_points.size() < n_power_interpolation + 1) {
      char kp[250];
      sprintf(kp, "There aren't enough points for this degree. You need %d points. Now %d", n_power_interpolation + 1, arr_points.size());
      glutSetWindowTitle(kp);
      return;
   }
   else
   {
      char kp[250];
      sprintf(kp, "Very well! Now %d points and power %d.",  arr_points.size(), n_power_interpolation);
      glutSetWindowTitle(kp);
   }

   vector<Point> Lagranzh;
   // Вычисление функции Лагранжа
   for (int x = 0; x < width; x++) {
      type_p y = 0;
      for (int i = 0; i < n_power_interpolation + 1; i++)
         y += true_array_point[i].y * L_i_func(true_array_point, i, x - width / 2);
      Lagranzh.push_back(Point(x - width / 2, y));
   }   
   //

   // Отрисовка сплайна
   glColor3d(1, 0, 0);
   glBegin(GL_LINE_STRIP);
   for (int i = 0; i < Lagranzh.size(); i++)
      glVertex2d(Lagranzh[i].x * scale + width / 2, Lagranzh[i].y * scale + height / 2); // Переход обратно к координатам окна
   glEnd();
   //
   Lagranzh.clear();
}

type_p distance(Point a, Point b) {
   return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

bool get_info() {
   fstream fcin("info.txt");

   if (!fcin.is_open())
      return false;
   fcin >> n_power_interpolation;
   return true;
}