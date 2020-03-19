// Lab_1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <fstream>
using namespace std;
typedef double type;

type x_0, x_n, y_0, y_m; // Параметры сетки
type x_0_border, x_n_border; // Границы Т по x
type y_del; // Граница T по y
int N_x, M_y; // Количество узлов по x и y
int k_1_x, k_2_x; 
int k_1_y, k_2_y;
type step_x, step_y; 
int sum_nodes; // Количество узлов


int calc_grid()
{
   step_x = (x_n - x_0) / N_x;
   k_2_x = N_x + 1;
   k_1_x = type(x_n_border - x_0_border) / step_x + 1;
   
   step_y = (y_m - y_0) / M_y;
   k_1_y = (y_del - y_0) / step_y; // здесь не учитываем границу
   k_2_y = M_y - k_1_y + 1;

   sum_nodes = k_1_y * k_1_x + k_2_x * k_2_y;
   return 1;
}

int read_grid(int size)
{
   ifstream fout_x;
   ifstream fout_y;
   fout_x.open("gridx.txt");
   fout_y.open("gridy.txt");

   if (fout_x)
      fout_x >> x_0 >> x_n >> x_0_border >> x_n_border >> N_x;
   else return 0;
   
   if (fout_y)
      fout_y >> y_0 >> y_m >> y_del >> M_y;
   else return 0;
   
   fout_x.close();
   fout_y.close();
   
   if (!calc_grid()) return 0;

   return 1;
}

type m_function(type x, type y)
{
   return x + y;
}

int main()
{
   int size_mas = 0;
   type* diag,* u;

   if(!read_grid(size_mas)) return -1;

   u = new type[size_mas];
   for (int i = 0; i < 5; i++)
      diag = new type[size_mas * 5];


}

