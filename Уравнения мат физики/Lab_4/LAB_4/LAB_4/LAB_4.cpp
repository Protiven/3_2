#include "GMRES.h"

void GMRES::init(int* s_ig, int* s_jg, double* s_gu, double* s_gl, double* s_di, int s_n)
{
   ig = s_ig;
   jg = s_jg;
   gu = s_gu;
   gl = s_gl;
   di = s_di;
   n = s_n;
   precond();
}

void GMRES::setF(double* s_rp) { 
   rp = s_rp; 
}

void GMRES::precond()
{
   double sum_l, sum_u, sum_d;
   int copy_end = ig[n];
   Ll = new double[copy_end];
   Uu = new double[copy_end];
   Ld = new double[n];
   for (int i = 0; i < copy_end; i++)
   {
      Ll[i] = gl[i];
      Uu[i] = gu[i];
   }
   for (int i = 0; i < n; i++) Ld[i] = di[i];
   for (int k = 1, k1 = 0; k <= n; k++, k1++)
   {
      sum_d = 0;
      int i_s = ig[k1], i_e = ig[k];
      for (int m = i_s; m < i_e; m++)
      {
         sum_l = 0;
         sum_u = 0;
         int j_s = ig[jg[m]], j_e = ig[jg[m] + 1];
         for (int i = i_s; i < m; i++)
         {
            for (int j = j_s; j < j_e; j++)
            {
               if (jg[i] == jg[j])
               {
                  sum_l += Ll[i] * Uu[j];
                  sum_u += Ll[j] * Uu[i];
                  j_s++;
               }
            }
         }
         Ll[m] = Ll[m] - sum_l;
         Uu[m] = (Uu[m] - sum_u) / Ld[jg[m]];
         sum_d += Ll[m] * Uu[m];
      }
      Ld[k1] = Ld[k1] - sum_d;
   }
}

double GMRES::dot_prod(double* a, double* b)
{
   double dp = 0;
   for (int i = 0; i < n; i++)
      dp += a[i] * b[i];
   return dp;
}

void GMRES::mull_A(double* f, double*& x)
{
   for (int i = 0; i < n; i++)
   {
      double v_el = f[i];
      x[i] = di[i] * v_el;
      for (int k = ig[i], k1 = ig[i + 1]; k < k1; k++)
      {
         int j = jg[k];
         x[i] += gl[k] * f[j];
         x[j] += gu[k] * v_el;
      }
   }
}

void GMRES::SolutionSLAE_L(double* f, double*& x)
{
   for (int k = 1, k1 = 0; k <= n; k++, k1++)
   {
      double sum = 0;
      for (int i = ig[k1]; i < ig[k]; i++)
         sum += Ll[i] * x[jg[i]];
      x[k1] = (f[k1] - sum) / Ld[k1];
   }
}

void GMRES::SolutionSLAE_U(double* f, double*& x)
{
   double* f1 = new double[n];
   for (int i = 0; i < n; i++)
      f1[i] = f[i];
   for (int k = n, k1 = n - 1; k > 0; k--, k1--)
   {
      x[k1] = f1[k1] / Ld[k1];
      double v_el = x[k1];
      for (int i = ig[k1]; i < ig[k]; i++)
         f1[jg[i]] -= Uu[i] * v_el;
   }
   delete[] f1;
}

void GMRES::SolutionSLAE(double*& solution, int& its) {
   // Параметры решателя
   int maxiter = 1000;
   double eps = 1E-14;
   m = 3;

   // Норма правой части, для выхода
   double rp_norm = sqrt(dot_prod(rp, rp));

   // Начинаем решение
   solution = new double[n];
   for (int i = 0; i < n; i++)
      solution[i] = 0.0;
   double* tmp = new double[n];
   double* hlp = new double[n];
   w = new double[n];
   y = new double[m];
   v = new double[m * n];
   h = new double[m * (m + 1)];
   h2 = new double[m * m];
   int j, newm = m, flag = 0, k = 0;
   double norma_r, h_last;
   int i;
   mull_A(solution, tmp);
   for (i = 0; i < n; i++) w[i] = rp[i] - tmp[i];
   SolutionSLAE_L(w, hlp);
   norma_r = sqrt(dot_prod(hlp, hlp));
   while (norma_r / rp_norm > eps && k < maxiter)
   {
      k++;
      flag = 0;
      for (i = 0; i < n; i++) hlp[i] /= norma_r;
      insert_col(0, hlp);

      for (j = 0; j < newm && !flag; j++)
      {
         SolutionSLAE_U(hlp, tmp);
         mull_A(tmp, hlp);
         SolutionSLAE_L(hlp, w);
         for (i = 0; i <= j; i++) calc_h_w(i, j);
         h[(j + 1) * m + j] = sqrt(dot_prod(w, w));
         h_last = h[(j + 1) * m + j];
         if (fabs(h_last) > eps * eps && j != newm - 1)
         {
            for (i = 0; i < n; i++) hlp[i] = w[i] / h_last;
            insert_col(j + 1, hlp);
         }
         if (fabs(h_last) < eps * eps)
         {
            newm = j + 1;
            flag = 1;
         }
      }
      if (flag)
      {
         y[0] = norma_r;
         for (i = 1; i < newm; i++) y[i] = 0;
         gauss(h, y, newm);
      }
      else
      {
         for (i = 0; i < newm; i++) y[i] = h[i] * norma_r;
         calc_HtH(newm);
         gauss(h2, y, newm);
      }
      mult_V_y(newm, tmp);

      SolutionSLAE_U(tmp, hlp);
      for (i = 0; i < n; i++) solution[i] += hlp[i];
      mull_A(solution, tmp);
      for (i = 0; i < n; i++) w[i] = rp[i] - tmp[i];
      SolutionSLAE_L(w, hlp);
      norma_r = sqrt(dot_prod(hlp, hlp));
   }
   its = k;

   // Очистка памяти
   delete[] hlp;
   delete[] tmp;
   delete[] w;
   delete[] y;
   delete[] v;
   delete[] h;
   delete[] h2;
}

void GMRES::mult_V_y(int mr, double* res) {
   long int k;
   int p;
   for (k = 0; k < n; k++)
   {
      res[k] = 0;
      for (p = 0; p < mr; p++)
         res[k] += v[k * m + p] * y[p];
   }
}

void GMRES::insert_col(int j, double* x)
{
   for (long int i = 0; i < n; i++) v[i * m + j] = x[i];
}

void GMRES::calc_h_w(int i, int j)
{
   int ind = i * m + j;
   long int k;
   h[ind] = 0;
   for (k = 0; k < n; k++) h[ind] += w[k] * v[k * m + i];
   for (k = 0; k < n; k++) w[k] -= h[ind] * v[k * m + i];
}

void GMRES::calc_HtH(int mr)
{
   int i, j, k, idx;
   memset(h2, 0., m * m * sizeof(double));
   for (i = 0; i < mr; i++)
      for (j = 0; j < mr; j++)
      {
         idx = i * m + j;
         for (k = 0; k <= mr; k++)
            h2[idx] += h[k * m + i] * h[k * m + j];
      }
}

void GMRES::gauss(double* M, double* vector, int mr)
{
   for (int i = 0; i < mr; i++)
   {
      int ind_di = i * (m + 1);
      int ind_maxstr = i;
      double* lead, temp, max;
      max = fabs(M[ind_di]);
      for (int j = 1; j < mr - i; j++)
         if (fabs(M[ind_di + j * m]) > max)
         {
            max = fabs(M[ind_di + j * m]);
            ind_maxstr = i + j;
         }
      for (int j = i; j < mr; j++)
      {
         temp = M[i * m + j];
         M[i * m + j] = M[ind_maxstr * m + j];
         M[ind_maxstr * m + j] = temp;
      }
      temp = vector[i];
      vector[i] = vector[ind_maxstr];
      vector[ind_maxstr] = temp;

      lead = &M[ind_di];
      for (int j = 1; j < mr - i; j++)
         *(lead + j) /= (*lead);
      vector[i] /= (*lead);
      *lead = 1;
      for (int j = i + 1; j < mr; j++)
      {
         for (int k = i + 1; k < mr; k++)
            M[j * m + k] = M[j * m + k] - M[i * m + k] * M[j * m + i];
         vector[j] = vector[j] - M[j * m + i] * vector[i];
         M[j * m + i] = 0.0;
      }
   }
   for (int i = mr - 1; i >= 0; i--)
      for (int j = mr - 1; j > i; j--)
         vector[i] = vector[i] - M[i * m + j] * vector[j];
}

