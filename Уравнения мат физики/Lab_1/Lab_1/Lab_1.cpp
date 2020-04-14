// Lab_1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <fstream>

#define MAX_ITER 70000
double epsilon = 1e-15;

using namespace std;
typedef double type;

// область 1 - основание
// область 2 - шапка

// 
ofstream fout("output.txt");
ifstream fout_x;
ifstream fout_y;
type x_0, x_n, y_0, y_m; // Параметры сетки
type x_0_border, x_n_border; // Границы области Т по x
type y_del; // Граница области T по y
int N_x, M_y; // Количество биений по x и y ( +1 - количество узлов)
int k_1_x, k_2_x; // ширина (по узлам) в каждой области x
int k_1_y, k_2_y; // ширина (по узлам) в каждой области y
type step_x, step_y;  // шаг по x и y
int sum_nodes; // Количество узлов
type* res_u;
int choice;

type resh_U(type x, type y) // считает истинное решение U
{
   return pow(x, 1) + pow(y, 1);
}

type m_function(type x, type y) // правая функция
{
   return 2*(pow(x, 1) + pow(y, 1));
}

type lambda(type x, type y)
{
   return 1;
}

type gamma(type x, type y)
{
   return 2;
}

type first_boundary_condition(type x, type y) // первое краевое условие
{
   return resh_U(x, y);
}

#include "even.h"
#include "uneven.h"

int read_grid()
{

   ifstream info("info.txt");

   choice = 1;
   info >> choice; // Если 1, то равномерная сетка, иначе неравномерная
   
   if (choice == 1)
   {
      fout_x.open("gridx.txt");
      fout_y.open("gridy.txt");

      if (fout_x) // начало инт, конец инт, начало инт Т, конец инт Т, количествой биений
         fout_x >> x_0 >> x_n >> x_0_border >> x_n_border >> N_x;
      else return 0;

      if (fout_y) // начало инт, конец инт, уровень деления, количествой биений
         fout_y >> y_0 >> y_m >> y_del >> M_y;
      else return 0;

      if (!calc_grid()) return 0;
   }
   else
   {
      info >> x_0_border >> x_n_border >> y_del >> N_x >> M_y;
      fout_x.open("gridx_1.txt");
      fout_y.open("gridy_1.txt");
      if (!read_massive_ox_oy()) return 0;
      ///////////!!!!!!!!!!!!!!
   }

   info.close();
   fout_x.close();
   fout_y.close();
   return 1;
}

int main()
{
   // Начало этапа "сетка"
   type* a,* f, * u_1;
   setlocale(LC_ALL, "rus");
   if (!read_grid()) return -1;


   f = new type[sum_nodes]{ 0 }; // Значение правой части в узлах
   u_1 = new type[sum_nodes]{ 0 }; // значения U(x, y) в узлах
   a = new type[sum_nodes * 5]{ 0 }; // матрица A
   res_u = new type[sum_nodes]{ 0 }; // Истинное решение u

   if (choice == 1)
   {
      if (choice == 1 && !matrix_filling(a, f)) return -2;
   }
   else
      matrix_filling_uneven(a, f, res_u); /////!!!!!!!!!!
   
   ///////////////////////// Для проверки
   cout << "Для матрицы " << "\n";
   for (int i = 0; i < sum_nodes; i++)
   {
      cout << "Узел номер " << i + 1 << "\t \t" <<
         *(a + i) << '\t' << *(a + i + sum_nodes) << '\t' << *(a + i + 2 * sum_nodes) << '\t'
         << *(a + i + 3 * sum_nodes) << '\t' << *(a + i + 4 * sum_nodes) << '\n';
   }
   cout << "\nДля правой части " << "\n";
   for (int i = 0; i < sum_nodes; i++)
   {
      cout << "Узел номер " << i + 1 << "\t \t" << *(f + i) << endl;
   }
   /////////////////////////
   if (choice == 1)
   {
      if (!calc_SLAE_1(u_1, f, a)) return -2;
   }
   else
      if(!calc_SLAE_2(u_1, f, a)) return -2; ///!!!!!!!!!!
  
   delete(f);
   delete(a);

   
  
   if (choice == 1)
   {
      fout << "\tU =\tU* =\t|U - U*|\n";
      for (int i = 0; i < sum_nodes; i++)
         fout  << *(u_1 + i) << '\t' << *(res_u + i)<< '\t' << abs(*(res_u + i) - *(u_1 + i)) << endl;
   }
   else 
      out_f(u_1);
   delete(res_u);
   fout.close();
   return 1;
   system("exit");
}

