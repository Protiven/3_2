#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

struct data
{
	int pid;
	double value;
} process;

double Cosinus(int n, double x)
{
	int i;
	double t = 1, z = -1, cos = 1;
	for (i = 1; i < n; i++)
	{
		t *= (x * x) / ((2 * i - 1) * (2 * i));
		cos += z * t;
		z *= -1;
	}
	return cos;
}

double Pi(int n, double x)
{
	double pi = 0, z = 4.0;
	int i;
	for (i = 0; i < n; i++)
	{
		pi += z / (2 * i + 1);
		z *= -1.0;
	}
	return pi;
}

int main()
{
	int f_write, f_read;
	int cos_pid, pi_pid;
	int n;
	double	pi, cos, f, x;
	int	status;
	
	printf("¬ведите  x:");
	scanf("%lf", &x);
	printf("¬ведите  n:");
	scanf("%d", &n);

	creat("test1.txt", 0666);
	f_write = open("test1.txt", O_WRONLY, 0666); // временной файл
	cos_pid = fork();

	if (cos_pid < 0)
	{
		printf("Cannot create child process 1. Exit!!!");
		exit(1); //завершаем программу
	}
	else if(cos_pid == 0)                  // потомок 1
	{
		process.value = Cosinus(n, x);
		process.pid = getpid();
		write(f_write, &process, sizeof(process));
		pi_pid = fork();
		if (pi_pid < 0)
		{
			printf("Cannot create child process 2. Exit!!!");
			exit(1); //завершаем программу
		}
		else if(pi_pid == 0)   	// потомок 2
		{
			process.value = Pi(n, x);
			process.pid = getpid();
			write(f_write, &process, sizeof(process));
			close(f_write);
			exit(0);
		}
		else
		{
			wait(&status);
			f_read = open("test1.txt", O_RDONLY, 0666);
			read(f_read, &process, sizeof(process));
			if (process.pid == pi_pid)
			{
				pi = process.value;
			}
			else
			{
				cos = process.value;
			}
			read(f_read, &process, sizeof(process));
			if (process.pid == cos_pid)
			{
				cos = process.value;
			}
			else
			{
				pi = process.value;
			}
			close(f_read);
			
			f = (1 - cos) / (pi * x * x);
			
			printf("\n");
			printf("cos = %f\n", cos);
			printf("pi  = %f\n", pi);
			printf("f(x)= %f\n", f);

			remove("test1.txt");
			printf("cos_pid %d\n", cos_pid);
			printf("pi_pid %d\n", pi_pid);
			printf("The program is finished.\n");
		}
	}
}

