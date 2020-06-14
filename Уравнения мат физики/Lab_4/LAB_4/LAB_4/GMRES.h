#ifndef GMRES_HMFE_H_
#define GMRES_HMFE_H_
#include <math.h>
#include <iostream>
#include <string.h>
using namespace std;


class GMRES
{
public:
   // Инициализация
   void init(int* s_ig, int* s_jg, double* s_gu, double* s_gl, double* s_di, int s_n);
   // Установка правой части
   void setF(double* s_rp);
   // Получение решения и количества итераций
   void SolutionSLAE(double*& solution, int& its);

private:
   // Размерность СЛАУ
   int n;
   // Основные массивы
   int* ig, * jg;
   double* gu, * gl, * di;
   double* rp;
   // Массивы для преобславливателя
   double* Uu, * Ll, * Ld;
   // Вычисление матриц L и U
   void precond();
   // Скалярное произведение
   double dot_prod(double* a, double* b);
   // x = Af
   void mull_A(double* f, double*& x);
   // Lx = f, прямой ход
   void SolutionSLAE_L(double* f, double*& x);
   // Ux = f, обратный ход
   void SolutionSLAE_U(double* f, double*& x);
   void mult_V_y(int mr, double* res);
   void insert_col(int j, double* x);
   // Элемент матрицы H
   void calc_h_w(int i, int j);
   void calc_HtH(int mr);
   void gauss(double* M, double* vector, int mr);
   double* h, * h2, * v, * w, * y;
   int m;
};
#endif
