// Lab_1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <fstream>

#define MAX_ITER 10000

using namespace std;
typedef double type;

// область 1 - основание
// область 2 - шапка

type x_0, x_n, y_0, y_m; // Параметры сетки
type x_0_border, x_n_border; // Границы области Т по x
type y_del; // Граница области T по y
int N_x, M_y; // Количество биений по x и y ( +1 - количество узлов)
int k_1_x, k_2_x; // ширина (по узлам) в каждой области x
int k_1_y, k_2_y; // ширина (по узлам) в каждой области y
type step_x, step_y;  // шаг по x и y
int sum_nodes; // Количество узлов
type* res_u;


type resh_U(type x, type y) // считает истинное решение U
{
   return x + y;
}

type m_function(type x, type y) // правая функция
{
   return x + y;
}

type lambda(type x, type y) 
{
   return 1;
}

type gamma(type x, type y)
{
   return 1;
}

type first_boundary_condition(type x, type y) // первое краевое условие
{
   return x + y;
}


// Тут вроде правильно
int calc_grid()
{
   step_x = (x_n - x_0) / N_x;
   k_2_x = N_x + 1; // ширина в узлах
   k_1_x = type(x_n_border - x_0_border) / step_x + 1; // ширина в узлах
   
   step_y = (y_m - y_0) / M_y;

   k_1_y = (y_del - y_0) / step_y; // здесь не учитываем границу
   k_2_y = M_y - k_1_y + 1;

   sum_nodes = k_1_y * k_1_x + k_2_x * k_2_y;
   return 1;
}

int read_grid()
{
   ifstream fout_x;
   ifstream fout_y;
   fout_x.open("gridx.txt");
   fout_y.open("gridy.txt");

   if (fout_x) // начало инт, конец инт, начало инт Т, конец инт Т, количествой биений
      fout_x >> x_0 >> x_n >> x_0_border >> x_n_border >> N_x;
   else return 0;
   
   if (fout_y) // начало инт, конец инт, уровень деления, количествой биений
      fout_y >> y_0 >> y_m >> y_del >> M_y;
   else return 0;
   
   fout_x.close();
   fout_y.close();
   
   if (!calc_grid()) return 0;

   return 1;
}
//


// Где-то здесь трабл
int calc_ox_oy(type* X, type* Y) // Все окей
{
   for (int i = 0; i < N_x + 1; i++)
      *(X + i) = x_0 + step_x * i;
   for (int i = 0; i < M_y + 1; i++)
      *(Y + i) = y_0 + step_y * i;
   return 1;
}

int calc_func(const type* X, const type* Y, type* F) // Пока все окей
{
   int i, k,
      supp_x = (x_0_border - x_0) / step_x;

   for (i = 0; i < k_1_y; i++)
      for (int j = 0; j < k_1_x; j++)
      {
         if (j == 0 || i == 0 || j == k_1_x - 1)
            *(F + i * k_1_x + j) = first_boundary_condition(*(X + j + supp_x), *(Y + i));
         else
            *(F + i * k_1_x + j) = m_function(*(X + j + supp_x), *(Y + i));
         *(res_u + i * k_1_x + j) = resh_U(*(X + j + supp_x), *(Y + i));
      }

   k = i;
   for (int j = 0; j < k_2_x; j++)
   {
      if (j <= supp_x || j >= k_2_x - 1 - supp_x) // (Надо проверить)
         *(F + k * k_1_x + j) = first_boundary_condition(*(X + j), *(Y + i));
      else
         *(F + k * k_1_x + j) = m_function(*(X + j), *(Y + i));
      *(res_u + k * k_1_x + j) = resh_U(*(X + j), *(Y + i));
   }

   i++;
   for (i; i < M_y + 1; i++)
      for (int j = 0; j < k_2_x; j++)
      {
         if (j == 0 || j == k_2_x - 1 || i == M_y)
            *(F + k * k_1_x + (i - k) * k_2_x + j) = first_boundary_condition(*(X + j), *(Y + i)); 
         else
            *(F + k * k_1_x + (i - k) * k_2_x + j) = m_function(*(X + j), *(Y + i));
         *(res_u + k * k_1_x + (i - k) * k_2_x + j) = resh_U(*(X + j), *(Y + i));
      }

   return 1;
}

int matrix_filling(type* A, type* F)  // Здесь траблов вроде нет (вроде действительно нет)
{
   type* x = new type[N_x + 1]{ 0 };
   type* y = new type[M_y + 1]{ 0 };

   if (!calc_ox_oy(x, y)) return -1;
   if (!calc_func(x, y, F)) return -1;

   int i, k, supp_x = (x_0_border - x_0) / step_x;
  
   // первая область 
   for (i = 0; i < k_1_y; i++)
      for (int j = 0; j < k_1_x; j++)
         if (j == 0 || i == 0 || j == k_1_x - 1)
            *(A + 2 * sum_nodes + j + i * k_1_x) = 1;
         else
         {
            *(A + j + i * k_1_x) = - lambda(x[j + supp_x], y[i]) / pow(step_y, 2);
            *(A + sum_nodes + j + i * k_1_x) = -lambda(x[j + supp_x], y[i]) / pow(step_x, 2);
            *(A + 2 * sum_nodes + j + i * k_1_x) = 2 * lambda(x[j + supp_x], y[i]) * (1. / pow(step_x, 2) + 1. / pow(step_y, 2)) + gamma(x[j + supp_x], y[i]); // диагональ
            *(A + 3 * sum_nodes + j + i * k_1_x) = *(A + sum_nodes + j + i * k_1_x);
            *(A + 4 * sum_nodes + j + i * k_1_x) = *(A + j + i * k_1_x);
         }
   
   k = i;
   // граница первой и второй области
   for (int j = 0; j < k_2_x; j++)
      if (j <= supp_x || j >= k_2_x - 1 - supp_x) // (Надо проверить)
         *(A + 2 * sum_nodes + k * k_1_x + j) = 1;
      else
      {
         *(A + j + i * k_1_x) = -lambda(x[j + supp_x], y[i]) / pow(step_y, 2);
         *(A + sum_nodes + k * k_1_x + j) = -lambda(x[j + supp_x], y[i]) / pow(step_x, 2);
         *(A + 2 * sum_nodes + k * k_1_x + j) = 2 * lambda(x[j + supp_x], y[i]) / pow(step_x, 2) + 2 * lambda(x[j + supp_x], y[i]) / pow(step_y, 2) + gamma(x[j + supp_x], y[i]); // диагональ
         *(A + 3 * sum_nodes + k * k_1_x + j) = *(A + sum_nodes + k * k_1_x + j);
         *(A + 4 * sum_nodes + k * k_1_x + j) = *(A + k * k_1_x + j);
      }

   // граница второй области
   i++;
   for (i; i < M_y + 1; i++)
      for (int j = 0; j < k_2_x; j++)
         if (j == 0 || j == k_2_x - 1 || i == M_y)
            *(A + 2 * sum_nodes + k * k_1_x + (i - k) * k_2_x + j) = 1;
         else
         {
            *(A + k * k_1_x + (i - k) * k_2_x + j) = -lambda(x[j + supp_x], y[i]) / pow(step_y, 2);
            *(A + sum_nodes + k * k_1_x + (i - k) * k_2_x + j) = -lambda(x[j + supp_x], y[i]) / pow(step_x, 2);
            *(A + 2 * sum_nodes + k * k_1_x + (i - k) * k_2_x + j) = 2 * lambda(x[j + supp_x], y[i]) / pow(step_x, 2) + 2 * lambda(x[j + supp_x], y[i]) / pow(step_y, 2) + gamma(x[j + supp_x], y[i]); // диагональ
            *(A + 3 * sum_nodes + k * k_1_x + (i - k) * k_2_x + j) = *(A + sum_nodes + k * k_1_x + (i - k) * k_2_x + j);
            *(A + 4 * sum_nodes + k * k_1_x + (i - k) * k_2_x + j) = *(A + k * k_1_x + (i - k) * k_2_x + j);
         }
   //         *(A);
   //         *(A + sum_nodes);
   //         *(A + 2 * sum_nodes); // центральная диагональ 
   //         *(A + 3 * sum_nodes); 
   //         *(A + 4 * sum_nodes);

   // Освобождение памяти
   delete(x);
   delete(y);
   return 1;
}
//

// Тут все правильно
type vector_norm(type* vec) // 100%
{
   type val = 0;
   for (int i = 0; i < sum_nodes; i++)
      val += pow(*(vec + i), 2);
   return sqrt(val);
}

type* multiply_matr_vect(type* A, type* vec) // 100%
{
   int sum_area_1 = k_1_x * k_1_y;
   type* v = new type[sum_nodes]{ 0 };

   // область 1
   for (int i = 0; i < sum_area_1; i++)
   {
      if (i - k_1_x >= 0)
         *(v + i) += *(A + i) * *(vec + i - k_1_x);
      if (i - 1 >= 0)
         *(v + i) += *(A + 1 * sum_nodes + i) * *(vec + i - 1);

      *(v + i) += *(A + 2 * sum_nodes + i) * *(vec + i);
      *(v + i) += *(A + 3 * sum_nodes + i) * *(vec + i + 1);
      *(v + i) += *(A + 4 * sum_nodes + i) * *(vec + i + k_1_x);
   }

   // граница между 1 и 2
   for (int i = sum_area_1; i < sum_area_1 + k_2_x; i++)
   {
      if (i - k_1_x >= 0)
         *(v + i) += *(A + i) * *(vec + i - k_1_x);
      if (i - 1 >= 0)
         *(v + i) += *(A + 1 * sum_nodes + i) * *(vec + i - 1);

      *(v + i) += *(A + 2 * sum_nodes + i) * *(vec + i);
      *(v + i) += *(A + 3 * sum_nodes + i) * *(vec + i + 1);
      *(v + i) += *(A + 4 * sum_nodes + i) * *(vec + i + k_2_x);
   }

   // Область 2 
   for (int i = sum_area_1 + k_2_x; i < sum_nodes; i++)
   {
      *(v + i) += *(A + i) * *(vec + i - k_2_x);
      *(v + i) += *(A + 1 * sum_nodes + i) * *(vec + i - 1);
      *(v + i) += *(A + 2 * sum_nodes + i) * *(vec + i);
      if (i + 1 <= sum_nodes)
         *(v + i) += *(A + 3 * sum_nodes + i) * *(vec + i + 1);
      if (i + k_2_x <= sum_nodes)
         *(v + i) += *(A + 4 * sum_nodes + i) * *(vec + i + k_2_x);
   }

   return v;
}

type multiply_vect_vect(type* a, type* b) // 100%
{
   type scalar = 0;
   for (int i = 0; i < sum_nodes; i++)
      scalar += a[i] * b[i];
   return scalar;
}
//

int calc_SLAE(type* X_0, type* F, type* A) // Решает правильно 100%
{
   // Шаг 1.1
   type supp = 0, * sup_v, alpha, beta;
   int z = 0;
   int sum_area_1 = k_1_x * k_1_y;
   double epsilon = 1e-14; 

   type* residual = new type[sum_nodes]{ 0 };
   type* p = new type[sum_nodes]{ 0 };
   type* q;
   
   // Шаг 1.2 - 1.3
   sup_v = multiply_matr_vect(A, X_0);
   for (int i = 0; i < sum_nodes; i++)
   {
      *(residual + i) = *(F + i) - *(sup_v + i);
      *(p + i) = *(residual + i);
   }
  

   while (z < MAX_ITER)
   {
      // Шаг 2.1
      q = multiply_matr_vect(A, p); 

      // Шаг 2.2
      alpha = multiply_vect_vect(residual, p) / multiply_vect_vect(q, p); 
         
      // Шаг 2.3 
      for (int i = 0; i < sum_nodes; i++)
         X_0[i] += alpha * p[i];

      // Шаг 2.4
      for (int i = 0; i < sum_nodes; i++)
         residual[i] -= alpha * q[i];

      // Шаг 2.5
      if (vector_norm(residual) < epsilon) break;

      // Шаг 3.1
      beta = multiply_vect_vect(residual, q) / multiply_vect_vect(p, q);

      // Шаг 3.2
      for (int i = 0; i < sum_nodes; i++)
         p[i] = residual[i] - beta * p[i];

      // Шаг 3.3
      z++;

     
   }
   delete(sup_v);
   return 1;
}
//

int main()
{
   // Начало этапа "сетка"
   type* a,* u,* f;
   setlocale(LC_ALL, "rus");
   if(!read_grid()) return -1;


   f = new type[sum_nodes]{ 0 }; // Значение правой части в узлах
   u = new type[sum_nodes]{ 0 }; // значения U(x, y) в узлах
   a = new type[sum_nodes * 5]{ 0 }; // матрица A
   res_u = new type[sum_nodes]{ 0 }; // Истинное решение u

   if (!matrix_filling(a, f)) return -2;
   
   ///////////////////////// Для проверки
   cout << "Для матрицы " << "\n";
   for (int i = 0; i < sum_nodes; i++)
   {
      cout << "Узел номер " << i + 1 << "\t \t" <<
         *(a + i) << '\t' <<* (a + i + sum_nodes) << '\t' <<* (a + i + 2*sum_nodes) << '\t'
         <<* (a + i + 3*sum_nodes) << '\t'<< * (a + i + 4*sum_nodes) << '\n';
   }

   cout << "\nДля правой части " << "\n";

   for (int i = 0; i < sum_nodes; i++)
   {
      cout << "Узел номер " << i + 1 << "\t \t" << *(f + i) << endl;
   }
   /////////////////////////

   // Конец этапа "Сетка" (вроде все окей!!!!). Начало решения СЛАУ.
   if (!calc_SLAE(u, f, a)) return -2;
   delete(f);
   delete(a);

   ofstream fout("output.txt");
   for (int i = 0; i < sum_nodes; i++)
      fout << "Узел номер " << i + 1 << '\t' << *(u + i) << '\t' << *(res_u + i) << endl;

   delete(res_u);
   delete(u);


   fout.close();
   return 1;
   system("exit");
}

