#pragma once

// “ут вроде правильно
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

int calc_ox_oy(type* X, type* Y) // ¬се окей
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
         if (j == 0 || i == 0 || j == k_1_x - 1)
            *(F + i * k_1_x + j) = first_boundary_condition(*(X + j + supp_x), *(Y + i));
         else
            *(F + i * k_1_x + j) = m_function(*(X + j + supp_x), *(Y + i));
         *(res_u + i * k_1_x + j) = resh_U(*(X + j + supp_x), *(Y + i));
      }

   k = i;
   for (int j = 0; j < k_2_x; j++)
   {
      if (j <= supp_x || j >= k_2_x - 1 - supp_x) 
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

int matrix_filling(type* A, type* F)  // ћатрица заполн€етс€ правильно, что на счет правой части??
{
   type* x = new type[N_x + 1]{ 0 };
   type* y = new type[M_y + 1]{ 0 };

   if (!calc_ox_oy(x, y)) return -1;
   if (!calc_func(x, y, F)) return -1;

   int i, k, supp_x = (x_0_border - x_0) / step_x;

   // перва€ область 
   for (i = 0; i < k_1_y; i++)
      for (int j = 0; j < k_1_x; j++)
         if (j == 0 || i == 0 || j == k_1_x - 1)
            *(A + 2 * sum_nodes + j + i * k_1_x) = 1;
         else
         {
            *(A + j + i * k_1_x) = -lambda(x[j + supp_x], y[i]) / pow(step_y, 2);
            *(A + sum_nodes + j + i * k_1_x) = -lambda(x[j + supp_x], y[i]) / pow(step_x, 2);
            *(A + 2 * sum_nodes + j + i * k_1_x) = +2 * lambda(x[j + supp_x], y[i]) * (1. / pow(step_x, 2) + 1. / pow(step_y, 2)) + gamma(x[j + supp_x], y[i]); // диагональ
            *(A + 3 * sum_nodes + j + i * k_1_x) = *(A + sum_nodes + j + i * k_1_x);
            *(A + 4 * sum_nodes + j + i * k_1_x) = *(A + j + i * k_1_x);
         }

   k = i;
   // граница первой и второй области
   for (int j = 0; j < k_2_x; j++)
      if (j <= supp_x || j >= k_2_x - 1 - supp_x) // (Ќадо проверить)
         *(A + 2 * sum_nodes + k * k_1_x + j) = 1;
      else
      {
         *(A + j + i * k_1_x) = -lambda(x[j + supp_x], y[i]) / pow(step_y, 2);
         *(A + sum_nodes + k * k_1_x + j) = -lambda(x[j + supp_x], y[i]) / pow(step_x, 2);
         *(A + 2 * sum_nodes + k * k_1_x + j) = +2 * lambda(x[j + supp_x], y[i]) / pow(step_x, 2) + 2 * lambda(x[j + supp_x], y[i]) / pow(step_y, 2) + gamma(x[j + supp_x], y[i]); // диагональ
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
   //         *(A + 2 * sum_nodes); // центральна€ диагональ 
   //         *(A + 3 * sum_nodes); 
   //         *(A + 4 * sum_nodes);

   // ќсвобождение пам€ти
   delete(x);
   delete(y);
   return 1;
}

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

      if (i + k_1_x < sum_area_1)
         *(v + i) += *(A + 4 * sum_nodes + i) * *(vec + i + k_1_x);
      else
         *(v + i) += *(A + 4 * sum_nodes + i) * *(vec + i + (k_1_x + k_2_x) / 2);
   }

   // граница между 1 и 2
   for (int i = sum_area_1; i < sum_area_1 + k_2_x; i++)
   {
      if (i - k_1_x >= 0)
         *(v + i) += *(A + i) * *(vec + i - (k_1_x + k_2_x) / 2); // Ёто под вопросом
      if (i - 1 >= 0)
         *(v + i) += *(A + 1 * sum_nodes + i) * *(vec + i - 1);

      *(v + i) += *(A + 2 * sum_nodes + i) * *(vec + i);
      *(v + i) += *(A + 3 * sum_nodes + i) * *(vec + i + 1);
      *(v + i) += *(A + 4 * sum_nodes + i) * *(vec + i + k_2_x);
   }

   // ќбласть 2 
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

int calc_SLAE_1(type* X_0, type* F, type* A) // 
{
   // –еализаци€ якоби
   type* residual = new type[sum_nodes]{ 0 };
   type* sup;

   int z = 0;
   while (z < MAX_ITER)
   {
      sup = multiply_matr_vect(A, X_0);
      for (int i = 0; i < sum_nodes; i++) {
         X_0[i] += F[i] / A[2 * sum_nodes + i];
         X_0[i] -= sup[i] / A[2 * sum_nodes + i]; // Ёто надо исправить!!!!
      }

      // ¬ыход по нев€зке
      sup = multiply_matr_vect(A, X_0);
      for (int i = 0; i < sum_nodes; i++)
         residual[i] = F[i] - sup[i];

      if (vector_norm(residual) / vector_norm(F) < epsilon)
         break; // ѕо относительной нев€зке

      z++;
   }
   return 1;
}