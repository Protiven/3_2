#include <math.h>
#include <stdio.h>
#include <fstream>
#include <vector>

using namespace std;
const double x1 = 0, x2 = 10;

double f(double x) {
   return (x - 15.0) * (x - 15.0) + 5.0;
}

void dichotomiya(double a, double b, double eps, double betta, ofstream& out) {
   double f1, f2, x1, x2;
   double length, PREVlength;
   double div;
   length = abs(b - a);
   int iter = 0;//номер текущей итерации
   out << "n|\t" << "a\t\t " << "|\t" << "b\t\t " << "|" << "b(i)-a(i) \t|" <<
      "погрешность " << "|\t" << "x1\t\t |\t" << "x2\t\t |\t" << "f1\t\t|\t\t" << "f2" << endl;
   while (length > eps)
   {
      iter++;
      x1 = (a + b - betta) / 2.0;
      x2 = (a + b + betta) / 2.0;
      f1 = f(x1);
      f2 = f(x2);
      if (f1 < f2)
         b = x2;
      else
         a = x1;
      PREVlength = length;
      length = abs(b - a);
      div = PREVlength / length;
      out.setf(ios::scientific);
      out << iter << "|" << a << "|" << b << "|" << length << "|" << div << "|"
         << x1 << "|" << x2 << "|" << f1 << "|" << f2 << endl;
   }
}

void golden_ratio(double a, double b, double eps, ofstream& out) {
   double f1, f2, x1, x2;
   double length, PREVlength;
   double div;
   length = fabs(b - a);
   int iter = 0;//номер текущей итерации
   x1 = a + 0.381966011 * (b - a);
   x2 = a + 0.618003399 * (b - a);
   f1 = f(x1);
   f2 = f(x2);
   out << "n|\t" << "a\t\t " << "|\t" << "b\t\t " << "|" << "b(i)-a(i) \t|" <<
      "погрешность " << "|\t" << "x1\t\t |\t" << "x2\t\t |\t" << "f1\t\t|\t\t" << "f2" << endl;
   while (length > eps)
   {
      iter++;
      if (f1 <= f2)
      {
         b = x2;
         x2 = x1;
         f2 = f1;
         x1 = a + 0.381966011 * (b - a);
         f1 = f(x1);
      }
      else
      {
         a = x1;
         x1 = x2;
         f1 = f2;
         x2 = a + 0.618003399 * (b - a);
         f2 = f(x2);
      }
      PREVlength = length;
      length = abs(b - a);
      div = PREVlength / length;
      out.setf(ios::scientific);
      out << iter << "|" << a << "|" << b << "|" << length << "|" << div << "|" << x1 <<
         "|" << x2 << "|" << f1 << "|" << f2 << endl;
   }
}


double F(int n) {
   return (pow(1.0 + sqrt(5.0), n) / pow(2.0, n) - pow(1.0 - sqrt(5.0), n) / pow(2.0, n)) /
      sqrt(5.0);
}

void Fibonacci(double a, double b, double eps, ofstream& out) {
   double a0, b0;
   double f1, f2, x1, x2;
   double length, PREVlength;
   double div;
   int n = 2, k;
   a0 = a;
   b0 = b;
   length = b - a;
   while ((b - a) / eps >= F(n + 2)) n++;
   n = n - 1;
   x1 = a + (b - a) * F(n) / F(n + 2);
   x2 = a + (b - a) * F(n + 1) / F(n + 2);
   f1 = f(x1);
   f2 = f(x2);
   out << "n|\t" << "a\t\t " << "|\t" << "b\t\t " << "|" << "b(i)-a(i) \t|" <<
      "погрешность " << "|\t" << "x1\t\t |\t" << "x2\t\t |\t" << "f1\t\t|\t\t" << "f2" << endl;
   for (k = 1; k < n; k++)
   {
      if (f1 < f2)
      {
         b = x2;
         x2 = x1;
         x1 = a + (b - a) * F(n - k + 1) / F(n - k + 3);
         f1 = f(x1);
      }
      else
      {
         a = x1;
         x1 = x2;
         x1 = a + (b - a) * F(n - k + 2) / F(n - k + 3);
         f2 = f(x2);
      }
      PREVlength = length;
      length = b - a;
      div = PREVlength / length;
      out.setf(ios::scientific);
      out << k << "|" << a << "|" << b << "|" << length << "|" << div << "|" << x1 << "|"
         << x2 << "|" << f1 << "|" << f2 << endl;
   }
   out << endl;
}

void Interval(double a, double b, double eps, ofstream& out) {
   double x0 = 15.1;
   double x1 = 0, f0, f1;
   double length;
   int iter = 1;
   // double betta = eps/3;
   double h = 0.5;
   out << "n |\t" << "x_k\t\t " << "|\t " << "f(x_k)\t " << "|" << "интервал содержащий минимум\t\t | " << "длина интервала " << endl;
      f0 = f(x0);
   f1 = f(x0 + h);
   if (f0 > f1)
   {
      x1 = x0 + h;
      length = fabs(h * 2);
      out.setf(ios::scientific);
      out << iter << " | " << x1 << " | " << f1 << " | " << " [ " << x1 - h << " , " <<
         x1 + h << " ] " << " | " << length << endl;
   }
   else
   {
      x1 = x0 - h;
      h = -h;
      length = fabs(h * 2);
      out.setf(ios::scientific);
      out << iter << " | " << x1 << " | " << f1 << " | " << " [ " << x1 - h << " , " <<
         x1 + h << " ] " << " | " << length << endl;
   }
   h *= 2;
   x1 = x0 + h;

   f0 = f(x0);
   f1 = f(x1);
   x0 = x1;

   while (f0 > f1)
   {
      iter++;
      h *= 2;
      x1 = x0 + h;

      f0 = f(x0);
      f1 = f(x1);
      x0 = x1;
      length = fabs(h * 2);
      out.setf(ios::scientific);
      out << iter << " | " << x1 << " | " << f1 << " | " << " [ " << x1 - h << " , " << x1 + h << " ] " << " | "<<length<< endl;
   }
   out << endl;
}

int main() {
   double a = 2, b = 200;
   double eps;
   double betta;
   ofstream fout;
   fout.open("dichtomiya.txt");
   for (eps = 1.E-1; eps > 1.E-7; eps *= 1.E-1)
   {
      fout << "eps=" << eps << endl;
      betta = eps / 2.0;
      dichotomiya(a, b, eps, betta, fout);
   }

   fout.close();
   fout.open("golden.txt");
   for (eps = 1.E-2; eps > 1.E-7; eps *= 1.E-2)
   {
      fout << "eps=" << eps << endl;
      golden_ratio(a, b, eps, fout);
   }
   fout.close();
   fout.open("fibonacci.txt");
   for (eps = 1.E-1; eps > 1.E-7; eps *= 1.E-1)
   {
      fout << "eps=" << eps << endl;
      Fibonacci(a, b, eps, fout);
   }
   fout.close();
   fout.open("interval.txt");
   for (eps = 1.E-1; eps > 1.E-7; eps *= 1.E-1)
   {
      fout << "eps=" << eps << endl;
      Interval(a, b, eps, fout);
   }
   fout.close();
}
