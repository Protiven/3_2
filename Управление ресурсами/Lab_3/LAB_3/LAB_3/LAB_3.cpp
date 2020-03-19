#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SIZE_FNAME 30
#define SIZE_BUFF 512
#define MAX_COUNT_PROCESS 10


struct table
{
	char file_name[SIZE_FNAME];
	int inode;
	int local_point;
} f_tables[MAX_COUNT_PROCESS];

struct stat st;
int f_descr[MAX_COUNT_PROCESS];
char f_name[SIZE_FNAME];


void write_into_table(int i, char file[SIZE_FNAME], int point)
{
	strcpy(f_tables[i].file_name, file);
	f_tables[i].inode = st.st_ino;
	f_tables[i].local_point = point;
}

void print_table(int i)
{
	printf("\nfile name \t\t number inode \t\t local point\n");
	printf("%s \t\t %d \t %d\n\n", f_tables[i].file_name, f_tables[i].inode, f_tables[i].local_point);
}

void change_lpoint(int i, int point)
{
	f_tables[i].local_point += point;
}

int func_actions(int i, int choice)
{
	char buff[SIZE_BUFF];
	int val, counts;

	switch (choice)
	{
	case 1:
		printf("How many characters do you want to read to buffer? ");
		if (!scanf("%d", &counts))
			return -1;

		printf("The process read %d characters!\n", counts);
		val = read(f_descr[i], buff, counts * sizeof(char));
		break;
	case 2:
		printf("How many characters do you want to write from buffer? ");
		if (!scanf("%d", &counts))
			return -1;

		if (counts == 0) {
			printf("Input the text :");
			getc(stdin);
			strcpy(buff, gets(stdin));
			counts = strlen(buff);
		}

		printf("The process write %d characters!\n", counts);
		val = write(f_descr[i], buff, counts * sizeof(char));
		break;
	case 3:
		printf("How many characters do you want to add from buffer? ");
		if (!scanf("%d", &counts))
			return -1;

		if (counts == 0) {
			printf("Input the text :");
			getc(stdin);
			strcpy(buff, gets(stdin));
			counts = strlen(buff);
		}

		printf("The process adds %d characters!\n", counts);
		val = write(f_descr[i], buff, counts * sizeof(char));
		break;
	default:
		break;
	}

	change_lpoint(i, val);
	return 1;
}


int main(int argc, char* argv[])
{
	if (argc == 1) {
		printf("Enter the name file: ");
		if (!scanf("%s", f_name)) {
			perror("Failed to read file name!\n");
			return -1;
		}
	}
	else {
		strcpy(f_name, argv[1]);
		if (f_name[0] == ' ') {
			perror("Failed to read file name!\n");
			return -1;
		}
	}


	if (stat(f_name, &st) != -1)
	{
		int choice = 4, choice_2;
		int flag = 1;

		for (int i = 0; i < MAX_COUNT_PROCESS; i++)
		{
			printf("Open for:\n1 - read only,\n2 - write only,\n3 - append,\n4 - Exit\n");

			flag = 1;

			if (!scanf("%d", &choice)) {
				perror("Input Error\n");
				return -2;
			}

			switch (choice)
			{
			case 1:
				f_descr[i] = open(f_name, O_RDONLY);
				break;
			case 2:
				f_descr[i] = open(f_name, O_WRONLY);
				break;
			case 3:
				f_descr[i] = open(f_name, O_WRONLY | O_APPEND);
				break;
			case 4:
				return 0;
				break;
			default:
				printf("Try again!\n");
				flag = 0;
			}

			if (flag != 1) break;

			if (fstat(f_descr[i], &st) != -1)
			{
				printf("The process opens %d the file.\n", i);
				write_into_table(i, f_name, 0);

				for (int j = 0; j <= i; j++)
					print_table(j);

				printf("Do you want to make some action with this process?\n1 - Yes, 2 - No\n");
				if (!scanf("%d", &choice_2)) {
					perror("Input Error\n");
					return -2;
				}

				if (choice_2 == 1)
				{
					if (func_actions(i, choice) == -1)
						return -1;

					for (int j = 0; j <= i; j++)
						print_table(j);
				}
			}
			else
				perror("Failed to return information about the open file!\n");
		}
	}
	else
	{
		perror("Failed to read file information!\n");
		return -3;
	}
}
