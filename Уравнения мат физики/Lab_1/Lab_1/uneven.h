#pragma once
type* x_mas, * y_mas;

int read_massive_ox_oy()
{
   x_mas = new type[N_x];
   y_mas = new type[M_y];

   if (fout_x)
      for (int i = 0; i < N_x; i++)
         fout_x >> x_mas[i];

   if (fout_y)
      for (int i = 0; i < M_y; i++)
         fout_y >> y_mas[i];
   sum_nodes = N_x * M_y;
   return 1;
}

type step(type x_, type x_1)
{
   return x_1 - x_;
}

int matrix_filling_uneven(type* A, type* F, type* res_u) // проблема в степах
{
   int i;
   for(i = 0; y_mas[i] < y_del; i++)
      for (int j = 0; j < N_x; j++)
      {
         if (x_mas[j] >= x_0_border && x_mas[j] <= x_n_border)
         {
            if (i == 0 || x_mas[j] == x_0_border || x_mas[j] == x_n_border)
            {
               *(A + 2 * sum_nodes + j + i * N_x) = 1;
               F[j + i * N_x] = resh_U(x_mas[j], y_mas[i]);
            }
            else
            {
               *(A + j + i * N_x) = - 2 * lambda(x_mas[j], y_mas[i]) / (step(y_mas[i - 1], y_mas[i])*(step(y_mas[i - 1], y_mas[i]) + step(y_mas[i], y_mas[i + 1])));//pow(step_y, 2); ///
               *(A + sum_nodes + j + i * N_x) = - 2 * lambda(x_mas[j], y_mas[i]) / (step(x_mas[i - 1], x_mas[i]) * (step(x_mas[i - 1], x_mas[i]) + step(x_mas[i], x_mas[i + 1])));///
               *(A + 2 * sum_nodes + j + i * N_x) = +2 * lambda(x_mas[j], y_mas[i]) * (1. / (step(x_mas[i - 1], x_mas[i]) * step(x_mas[i], x_mas[i + 1])) + 1. / (step(y_mas[i - 1], y_mas[i]) * step(y_mas[i], y_mas[i + 1]))) + gamma(x_mas[j], y_mas[i]); // диагональ
               *(A + 3 * sum_nodes + j + i * N_x) = -2 * lambda(x_mas[j], y_mas[i]) / (step(x_mas[i], x_mas[i + 1]) * (step(x_mas[i - 1], x_mas[i]) + step(x_mas[i], x_mas[i + 1])));///
               *(A + 4 * sum_nodes + j + i * N_x) = -2 * lambda(x_mas[j], y_mas[i]) / (step(y_mas[i], y_mas[i + 1]) * (step(y_mas[i - 1], y_mas[i]) + step(y_mas[i], y_mas[i + 1])));//pow(step_y, 2); ///;////
               F[j + i * N_x] = m_function(x_mas[j], y_mas[i]);
            }
            res_u[i * N_x + j] = resh_U(x_mas[j], y_mas[i]);
         }
      }
   for(i; i < M_y; i++)
      for (int j = 0; j < N_x; j++)
      {
         if ((y_mas[i] == y_del && (x_mas[j] < x_0_border || x_mas[j] > x_n_border)) || i == M_y - 1 || j == 0 || j == N_x - 1)
         {
            *(A + 2 * sum_nodes + j + i * N_x) = 1;
            F[j + i * N_x] = resh_U(x_mas[j], y_mas[i]);
         }
         else
         {
            *(A + j + i * N_x) = -2 * lambda(x_mas[j], y_mas[i]) / (step(y_mas[i - 1], y_mas[i]) * (step(y_mas[i - 1], y_mas[i]) + step(y_mas[i], y_mas[i + 1])));//pow(step_y, 2); ///
            *(A + sum_nodes + j + i * N_x) = -2 * lambda(x_mas[j], y_mas[i]) / (step(x_mas[i - 1], x_mas[i]) * (step(x_mas[i - 1], x_mas[i]) + step(x_mas[i], x_mas[i + 1])));///
            *(A + 2 * sum_nodes + j + i * N_x) = +2 * lambda(x_mas[j], y_mas[i]) * (1. / (step(x_mas[i - 1], x_mas[i]) * step(x_mas[i], x_mas[i + 1])) + 1. / (step(y_mas[i - 1], y_mas[i]) * step(y_mas[i], y_mas[i + 1]))) + gamma(x_mas[j], y_mas[i]); // диагональ
            *(A + 3 * sum_nodes + j + i * N_x) = -2 * lambda(x_mas[j], y_mas[i]) / (step(x_mas[i], x_mas[i + 1]) * (step(x_mas[i - 1], x_mas[i]) + step(x_mas[i], x_mas[i + 1])));///
            *(A + 4 * sum_nodes + j + i * N_x) = -2 * lambda(x_mas[j], y_mas[i]) / (step(y_mas[i], y_mas[i + 1]) * (step(y_mas[i - 1], y_mas[i]) + step(y_mas[i], y_mas[i + 1])));//pow(step_y, 2); ///;////
            F[j + i * N_x] = m_function(x_mas[j], y_mas[i]);
         }
         res_u[i * N_x + j] = resh_U(x_mas[j], y_mas[j]);
      }
   return 1;
}
/*
type* multiply_matr_vect_1(type* A, type* vec) // 20%
{
   type* v = new type[sum_nodes]{ 0 };
   
   int i;
   for (i = 0; i < k_1_y; i++)
      for (int j = 0; j < N_x; j++)
      {
         if (x_mas[j] >= x_0_border && x_mas[j] <= x_n_border)
         {
            if (i == 0 || x_mas[j] == x_0_border || x_mas[j] == x_n_border)
            {
               v[j + i * N_x] += *(A + 2 * sum_nodes + j + i * N_x) * vec[j + i * N_x];
            }
            else
            {
               v[j + i * N_x] += *(A + 4 * sum_nodes + j + i * N_x) * vec[j + i * N_x - N_x];///
               v[j + i * N_x] += *(A + sum_nodes + j + i * N_x) * vec[j + i * N_x - 1];///
               v[j + i * N_x] += *(A + 2 * sum_nodes + j + i * N_x) * vec[j + i * N_x]; // диагональ
               v[j + i * N_x] += *(A + 3 * sum_nodes + j + i * N_x) * vec[j + i * N_x + 1];///
               v[j + i * N_x] += *(A + 4 * sum_nodes + j + i * N_x) * vec[j + i * N_x + N_x];///
            }
         }
      }
   for (i; i < M_y; i++)
      for (int j = 0; j < N_x; j++)
      {
         if ((i == k_1_y && x_mas[j] < x_0_border || x_mas[j] > x_n_border) || i == M_y - 1 || j == 0 || j == N_x - 1)
         {
            v[j + i * N_x] += *(A + 2 * sum_nodes + j + i * N_x) * vec[j + i * N_x];
         }
         else
         {
            v[j + i * N_x] += *(A + 4 * sum_nodes + j + i * N_x) * vec[j + i * N_x - N_x];///
            v[j + i * N_x] += *(A + sum_nodes + j + i * N_x) * vec[j + i * N_x - 1];///
            v[j + i * N_x] += *(A + 2 * sum_nodes + j + i * N_x) * vec[j + i * N_x]; // диагональ
            v[j + i * N_x] += *(A + 3 * sum_nodes + j + i * N_x) * vec[j + i * N_x + 1];///
            v[j + i * N_x] += *(A + 4 * sum_nodes + j + i * N_x) * vec[j + i * N_x + N_x];///
         }
      }
   return v;
}
*/
int calc_SLAE_2(type* X_0, type* F, type* A) // 
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
         X_0[i] -= sup[i] / A[2 * sum_nodes + i]; 
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
   for (i = 0; y_mas[i] != y_del; i++)
      for (int j = 0; j < N_x; j++)
         if (x_mas[j] >= x_0_border && x_mas[j] <= x_n_border)
         {
            k++;
            fout << "Узел " << k << '\t' << *(U_ + i * N_x + j) << '\t' << *(res_u + i * k_1_x + j) << endl;
         }
   for (; i < M_y + 1; i++)
      for (int j = 0; j < N_x; j++)
      {
         k++;
         fout << "Узел " << k << '\t' << *(U_ + i * N_x + j) << '\t' << *(res_u + i * k_1_x + j) << endl;
      }
}

/*int calc_func(const type* X, const type* Y, type* F) // 
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

*/