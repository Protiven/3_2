// Lab_1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <fstream>

#define MAX_ITER 40000
double epsilon = 1e-15;

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
ofstream fout("output.txt");
type* x;
type* y;


type real_U(type x, type y) // считает истинное решение U
{
   return x + y;
}

type func_(type x, type y) // правая функция
{
   return 2 * (x + y);
}

type lambda(type x, type y)
{
   return 3;
}

type gamma(type x, type y)
{
   return 2;
}


int calc_grid()
{
   step_x = (x_n - x_0) / N_x;
   k_2_x = N_x + 1; // ширина в узлах
   k_1_x = k_2_x; // ширина в узлах

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

   if (!calc_grid()) return 0;

   fout_x.close();
   fout_y.close();
   return 1;
}

int calc_ox_oy(type* X, type* Y) // Все окей
{
   for (int i = 0; i < N_x + 1; i++)
      *(X + i) = x_0 + step_x * i;
   for (int i = 0; i < M_y + 1; i++)
      *(Y + i) = y_0 + step_y * i;
   return 1;
}

int calc_func(const type* X, const type* Y, type* F) // 
{
   int i, k,
      supp_x = (x_0_border - x_0) / step_x;

   for (i = 0; i < k_1_y; i++)
      for (int j = 0; j < k_1_x; j++)
      {
         if (X[j] <= x_0_border || X[j] >= x_n_border)
         {
            if (i == 0 || j == 0 || X[j] == x_0_border || X[j] == x_n_border || j == k_1_x - 1)
               F[i * k_1_x + j] = real_U(X[j], Y[i]);
            else
               F[i * k_1_x + j] = func_(X[j], Y[i]);
            res_u[i * k_1_x + j] = real_U(X[j], Y[i]);
         }
      }

   k = i;
   for (int j = 0; j < k_2_x; j++)
   {
         if (j == 0 || (X[j] >= x_0_border && X[j] <= x_n_border) || j == k_1_x - 1)
            F[i * k_1_x + j] = real_U(X[j], Y[i]);
         else
            F[i * k_1_x + j] = func_(X[j], Y[i]);
         res_u[i * k_1_x + j] = real_U(X[j], Y[i]);
   }

   i++;
   for (i; i < M_y + 1; i++)
      for (int j = 0; j < k_2_x; j++)
      {
         if (j == 0 || j == k_2_x - 1 || i == M_y)
            *(F + k * k_1_x + (i - k) * k_2_x + j) = real_U(*(X + j), *(Y + i));
         else
            *(F + k * k_1_x + (i - k) * k_2_x + j) = func_(*(X + j), *(Y + i));
         *(res_u + k * k_1_x + (i - k) * k_2_x + j) = real_U(*(X + j), *(Y + i));
      }
   return 1;
}

int matrix_filling(type* A, type* F)  // Матрица заполняется правильно
{
   x = new type[N_x + 1]{ 0 };
   y = new type[M_y + 1]{ 0 };

   if (!calc_ox_oy(x, y)) return -1;
   if (!calc_func(x, y, F)) return -1;

   int i, k, supp_x = (x_0_border - x_0) / step_x;

   // первая область 
   for (i = 0; i < k_1_y; i++)
      for (int j = 0; j < k_1_x; j++)
            if (j == 0 || i == 0 || j == k_1_x - 1 || x[j] == x_0_border || x[j] == x_n_border)
               *(A + 2 * sum_nodes + j + i * k_1_x) = 1;
            else
            {
               *(A + j + i * k_1_x) = -lambda(x[j], y[i]) / pow(step_y, 2);
               *(A + sum_nodes + j + i * k_1_x) = -lambda(x[j], y[i]) / pow(step_x, 2);
               *(A + 2 * sum_nodes + j + i * k_1_x) = +2 * lambda(x[j], y[i]) * (1. / pow(step_x, 2) + 1. / pow(step_y, 2)) + gamma(x[j], y[i]); // диагональ
               *(A + 3 * sum_nodes + j + i * k_1_x) = *(A + sum_nodes + j + i * k_1_x);
               *(A + 4 * sum_nodes + j + i * k_1_x) = *(A + j + i * k_1_x);
            }
   k = i;


   // граница первой и второй области
   for (int j = 0; j < k_2_x; j++)
      if (j == 0 || (x[j] >= x_0_border && x[j] <= x_n_border) || j == k_1_x - 1) // (Надо проверить)
         *(A + 2 * sum_nodes + k * k_1_x + j) = 1;
      else
      {
         *(A + j + i * k_1_x) = -lambda(x[j], y[i]) / pow(step_y, 2);
         *(A + sum_nodes + k * k_1_x + j) = -lambda(x[j], y[i]) / pow(step_x, 2);
         *(A + 2 * sum_nodes + k * k_1_x + j) = +2 * lambda(x[j], y[i]) / pow(step_x, 2) + 2 * lambda(x[j], y[i]) / pow(step_y, 2) + gamma(x[j ], y[i]); // диагональ
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
            *(A + k * k_1_x + (i - k) * k_2_x + j) = -lambda(x[j], y[i]) / pow(step_y, 2);
            *(A + sum_nodes + k * k_1_x + (i - k) * k_2_x + j) = -lambda(x[j], y[i]) / pow(step_x, 2);
            *(A + 2 * sum_nodes + k * k_1_x + (i - k) * k_2_x + j) = 2 * lambda(x[j], y[i]) / pow(step_x, 2) + 2 * lambda(x[j ], y[i]) / pow(step_y, 2) + gamma(x[j ], y[i]); // диагональ
            *(A + 3 * sum_nodes + k * k_1_x + (i - k) * k_2_x + j) = *(A + sum_nodes + k * k_1_x + (i - k) * k_2_x + j);
            *(A + 4 * sum_nodes + k * k_1_x + (i - k) * k_2_x + j) = *(A + k * k_1_x + (i - k) * k_2_x + j);
         }


   //         *(A);
   //         *(A + sum_nodes);
   //         *(A + 2 * sum_nodes); // центральная диагональ 
   //         *(A + 3 * sum_nodes); 
   //         *(A + 4 * sum_nodes);
   return 1;
}

type vector_norm(type* vec) // 100%
{
   type val = 0;
   for (int i = 0; i < sum_nodes; i++)
      val += pow(*(vec + i), 2);
   return sqrt(val);
}

type* multiply_matr_vect(type* A, type* vec) // Переделать
{
   int sum_area_1 = k_1_x * k_1_y;
   type* v = new type[sum_nodes]{ 0 };
   // область 1
   for (int i = 0; i < sum_area_1; i++)
   {
         if (i - k_1_x >= 0)
            *(v + i) += *(A + i) * *(vec + i - k_1_x); // Это под вопросом
         if (i - 1 >= 0)
            *(v + i) += *(A + 1 * sum_nodes + i) * *(vec + i - 1);

         *(v + i) += *(A + 2 * sum_nodes + i) * *(vec + i);
         *(v + i) += *(A + 3 * sum_nodes + i) * *(vec + i + 1);
         *(v + i) += *(A + 4 * sum_nodes + i) * *(vec + i + k_2_x);
   }

   // граница между 1 и 2
   for (int i = sum_area_1; i < sum_area_1 + k_2_x; i++)
   {
      if (i - k_1_x >= 0)
         *(v + i) += *(A + i) * *(vec + i - k_1_x); // Это под вопросом
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

int calc_SLAE(type* X_0, type* F, type* A) // 
{
   // Реализация Якоби
   type* residual = new type[sum_nodes]{ 0 };
   type* sup;

   int z = 0;
   while (z < MAX_ITER)
   {
      sup = multiply_matr_vect(A, X_0);
      for (int i = 0; i < sum_nodes; i++) {
         X_0[i] += F[i] / A[2 * sum_nodes + i];
         X_0[i] -= sup[i] / A[2 * sum_nodes + i]; // Это надо исправить!!!!
      }


      // Выход по невязке
      sup = multiply_matr_vect(A, X_0);
      for (int i = 0; i < sum_nodes; i++)
         residual[i] = F[i] - sup[i];

      if (vector_norm(residual) / vector_norm(F) < epsilon)
         break; // По относительной невязке

      z++;
   }
   return 1;
}

void out_f(type* U_)
{
   fout << "\t\tЧисленное решение\t\tИстинное решение\n";
   int k = 0;
   int i;
   for (i = 0; i < k_1_y; i++)
      for (int j = 0; j < k_1_x; j++)
         if (x[j] <= x_0_border || x[j] >= x_n_border)
         {
            k++;
            fout << "Узел " << k << '\t' << *(U_ + i * k_1_x + j) << '\t' << *(res_u + i * k_1_x + j) << endl;
         }
   for (; i < M_y + 1; i++)
      for (int j = 0; j < k_1_x; j++)
      {
         k++;
         fout << "Узел " << k << '\t' << *(U_ + i * k_1_x + j) << '\t' << *(res_u + i * k_1_x + j) << endl;
      }
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

   if (!matrix_filling(a, f)) return -2;

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

   // Конец этапа "Сетка" (вроде все окей!!!!). Начало решения СЛАУ.
   if (!calc_SLAE(u_1, f, a)) return -2;

   delete(f);
   delete(a);

   out_f(u_1);
   /*for (int i = 0; i < sum_nodes; i++)
      fout << "Узел " << i + 1 <<  '\t' << *(u_1 + i) << '\t' << *(res_u + i) << endl;*/

   delete(res_u);
   fout.close();
   return 1;
   system("exit");
}

