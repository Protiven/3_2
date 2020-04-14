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
   int i, z = 0;
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
               *(A + sum_nodes + j + i * N_x) = - 2 * lambda(x_mas[j], y_mas[i]) / (step(x_mas[j - 1], x_mas[j]) * (step(x_mas[j - 1], x_mas[j]) + step(x_mas[j], x_mas[j + 1])));///
               *(A + 2 * sum_nodes + j + i * N_x) = +2 * lambda(x_mas[j], y_mas[i]) * (1. / (step(x_mas[j - 1], x_mas[j]) * step(x_mas[j], x_mas[j + 1])) + 1. / (step(y_mas[i - 1], y_mas[i]) * step(y_mas[i], y_mas[i + 1]))) + gamma(x_mas[j], y_mas[i]); // диагональ
               *(A + 3 * sum_nodes + j + i * N_x) = -2 * lambda(x_mas[j], y_mas[i]) / (step(x_mas[j], x_mas[j + 1]) * (step(x_mas[j - 1], x_mas[j]) + step(x_mas[j], x_mas[j + 1])));///
               *(A + 4 * sum_nodes + j + i * N_x) = -2 * lambda(x_mas[j], y_mas[i]) / (step(y_mas[i], y_mas[i + 1]) * (step(y_mas[i - 1], y_mas[i]) + step(y_mas[i], y_mas[i + 1])));//pow(step_y, 2); ///;////
               F[j + i * N_x] = m_function(x_mas[j], y_mas[i]);
            }
            res_u[z] = resh_U(x_mas[j], y_mas[i]);
            z++;
         }
         else
         {
            F[j + i * N_x] = 0;
         }
      }
   for(i; i < M_y; i++)
      for (int j = 0; j < N_x; j++)
      {
         if ((y_mas[i] == y_del && (x_mas[j] <= x_0_border || x_mas[j] >= x_n_border)) || i == M_y - 1 || j == 0 || j == N_x - 1)
         {
            *(A + 2 * sum_nodes + j + i * N_x) = 1;
            F[j + i * N_x] = resh_U(x_mas[j], y_mas[i]);
         }
         else
         {
            *(A + j + i * N_x) = -2 * lambda(x_mas[j], y_mas[i]) / (step(y_mas[i - 1], y_mas[i]) * (step(y_mas[i - 1], y_mas[i]) + step(y_mas[i], y_mas[i + 1])));//pow(step_y, 2); ///
            *(A + sum_nodes + j + i * N_x) = -2 * lambda(x_mas[j], y_mas[i]) / (step(x_mas[j - 1], x_mas[j]) * (step(x_mas[j - 1], x_mas[j]) + step(x_mas[j], x_mas[j + 1])));///
            *(A + 2 * sum_nodes + j + i * N_x) = +2 * lambda(x_mas[j], y_mas[i]) * (1. / (step(x_mas[j - 1], x_mas[j]) * step(x_mas[j], x_mas[j + 1])) + 1. / (step(y_mas[i - 1], y_mas[i]) * step(y_mas[i], y_mas[i + 1]))) + gamma(x_mas[j], y_mas[i]); // диагональ
            *(A + 3 * sum_nodes + j + i * N_x) = -2 * lambda(x_mas[j], y_mas[i]) / (step(x_mas[j], x_mas[j + 1]) * (step(x_mas[j - 1], x_mas[j]) + step(x_mas[j], x_mas[j + 1])));///
            *(A + 4 * sum_nodes + j + i * N_x) = -2 * lambda(x_mas[j], y_mas[i]) / (step(y_mas[i], y_mas[i + 1]) * (step(y_mas[i - 1], y_mas[i]) + step(y_mas[i], y_mas[i + 1])));//pow(step_y, 2); ///;////
            F[j + i * N_x] = m_function(x_mas[j], y_mas[i]);
         }
         res_u[z] = resh_U(x_mas[j], y_mas[i]);
         z++;
      }
   return 1;
}

/**/
type* multiply_matr_vect_1(type* A, type* vec) // 20%
{
   sum_nodes = M_y * N_x;
   type* v = new type[sum_nodes]{ 0 };
   int i, sum_nodes_1, k;

   for (i = 0; y_mas[i] < y_del; i++)
   {
      for (int j = 0; j < N_x; j++)
      {
         if (i * N_x + j - N_x >= 0)
            *(v + i * N_x + j) += *(A + i * N_x + j) * *(vec + i * N_x + j - N_x); // Это под вопросом
         if (i - 1 >= 0)
            *(v + i * N_x + j) += *(A + 1 * sum_nodes + i * N_x + j) * *(vec + i * N_x + j - 1);

         *(v + i * N_x + j) += *(A + 2 * sum_nodes + i * N_x + j) * *(vec + i * N_x + j);
         *(v + i * N_x + j) += *(A + 3 * sum_nodes + i * N_x + j) * *(vec + i * N_x + j + 1);
         *(v + i * N_x + j) += *(A + 4 * sum_nodes + i * N_x + j) * *(vec + i * N_x + j + N_x);
      }
   }
   i = i * N_x;
   k = i;
   // граница между 1 и 2
   for (i; i < k + N_x; i++)
   {
      if (i - N_x >= 0)
         *(v + i) += *(A + i) * *(vec + i - N_x); // Это под вопросом
      if (i - 1 >= 0)
         *(v + i) += *(A + 1 * sum_nodes + i) * *(vec + i - 1);

      *(v + i) += *(A + 2 * sum_nodes + i) * *(vec + i);
      *(v + i) += *(A + 3 * sum_nodes + i) * *(vec + i + 1);
      *(v + i) += *(A + 4 * sum_nodes + i) * *(vec + i + N_x);
   }

   // Область 2 
   for (; i < sum_nodes; i++)
   {
      *(v + i) += *(A + i) * *(vec + i - N_x);
      *(v + i) += *(A + 1 * sum_nodes + i) * *(vec + i - 1);
      *(v + i) += *(A + 2 * sum_nodes + i) * *(vec + i);
      if (i + 1 <= sum_nodes)
         *(v + i) += *(A + 3 * sum_nodes + i) * *(vec + i + 1);
      if (i + N_x <= sum_nodes)
         *(v + i) += *(A + 4 * sum_nodes + i) * *(vec + i + N_x);
   }
   return v;
}

int calc_SLAE_2(type* X_0, type* F, type* A) // 
{
   // Реализация Якоби
   type* residual = new type[sum_nodes]{ 0 };
   type* sup;

   int z = 0;
   while (z < MAX_ITER)
   {
      sup = multiply_matr_vect_1(A, X_0);
      for (int i = 0; i < M_y; i++) {
         for (int j = 0; j < N_x; j++) {
            if (!(y_mas[i] < y_del && (x_mas[j] < x_0_border || x_mas[j] > x_n_border))) {
               X_0[i * N_x + j] += F[i * N_x + j] / A[2 * sum_nodes + i * N_x + j];
               X_0[i * N_x + j] -= sup[i * N_x + j] / A[2 * sum_nodes + i * N_x + j];
            }
         }
      }

      // Выход по невязке
      sup = multiply_matr_vect_1(A, X_0);
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
   fout << "X =\t\t Y = \t\tU = \t\tU* =\t|U - U*|\n";
   int k = 0;
   int i;
   for (i = 0; y_mas[i] != y_del; i++)
      for (int j = 0; j < N_x; j++)
         if (x_mas[j] >= x_0_border && x_mas[j] <= x_n_border)
         {
            k++;
            fout << x_mas[j] << '\t' << y_mas[i] << '\t'  << *(U_ + i * N_x + j) << '\t' << *(res_u + k - 1) << '\t' << abs(*(U_ + i * N_x + j) - *(res_u + k - 1)) <<endl;
         }
   for (; i < M_y; i++)
      for (int j = 0; j < N_x; j++)
      {
         k++;
         fout << x_mas[j] << '\t' << y_mas[i] << '\t' << *(U_ + i * N_x + j) << '\t' << *(res_u + k - 1) << '\t' << abs(*(U_ + i * N_x + j) - *(res_u + k - 1)) << endl;
      }
}

