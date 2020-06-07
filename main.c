#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Струтура, описывающая граф*/
struct vertexs
{
	int number;
	int degree;
	int *nears;
};

/*Прототипы функций*/
int input_data();
int connectivity(struct vertexs *, int );
void dfs(struct vertexs *, int , int);
void mark_clear();
int dfs_up(struct vertexs *, int , int);
void parent_clear();
void create_copy();
/*Глобальные переменные*/
struct vertexs *point_vert = NULL;	/*Указатель на массив вершин*/
struct vertexs *point_vert_copy = NULL;
int n;		/*Число вершин*/
int *mark = NULL;		/*Массив меток*/
int *result = NULL; /*Результат*/
int cycle_flag;
int *parent = NULL;
/*Примем соглашение касающееся массива меток. Если в вершине с номером совпа-
дающим с номером метки ещё прохода еще не было, то метка будет иметь значение
равное 0. В случае если вершина уже была пройдена, то метка будет иметь значение
равное 1. Я все сказал!*/


int main()
{
	int components;
	int numb_res;
	int delete_point;
	int i, j, k, l;
	int counter = 0;
	numb_res = 0;
	if(input_data() == 0)
	{
		return 0;
	}
	result = (int *)malloc(n*sizeof(int));
	result[0] = -1;
	/*Проверяем граф на связность */
	components = connectivity(point_vert, -1);
	if(components != 1)
	{
		puts("Граф не связанный.");
	}
	else
	{
		mark_clear();
		cycle_flag = -1;
		parent_clear();
		for (i = 0; i < n; i++)
		{
			if (dfs_up (point_vert, i, -1) == 1)
				break;
		}
		if (cycle_flag == -1)
		{
			puts ("Граф уже дерево.");
		}
		else
		{
			puts("Исходный граф не дерево.");
			/*Создаем копию списка смежности*/
			point_vert_copy = (struct vertexs *)malloc(n*sizeof(struct vertexs));
			for(i = 0; i < n; i++)
			{
				point_vert_copy[i].number = point_vert[i].number;
				point_vert_copy[i].degree = point_vert[i].degree;
				point_vert_copy[i].nears = (int *)malloc(point_vert_copy[i].degree*sizeof(int));
			}
			mark_clear();
			parent_clear();
			/*Используем метод "грубой силы"*/
			for(i = 0; i < n; i++)
			{
				create_copy();
				for(j = 0; j < n; j++)
				{
					for(k = 0; k < point_vert_copy[j].degree; k++)
					{
						if(point_vert_copy[j].nears[k] == i)
						{
							point_vert_copy[j].nears[k] = -1;
							break;
						}

					}
				}
				mark_clear();
				parent_clear();
				components = connectivity(point_vert_copy, i);
				if(components == 1)
				{
					mark_clear();
					cycle_flag = -1;
					parent_clear();
					for (l = 0; l < n; l++)
					{
						if (dfs_up (point_vert_copy, l, -1) == 1)
							break;
					}
					if (cycle_flag == -1)
					{
						result[counter] = i;
						counter++;
					}
				}

			}
			if(result[0] != -1)
			{
				puts("При удалении одной из следующих точек, возможнополучение дерева: ");
				for(i = 0; i < counter; i++)
				{
					printf("%d ", result[i]);
				}
				puts("\n");
			}
			else
			{
				puts("Нет таких вершин, при удалении которых получилось бы дерево!");
			}




			/*Зачищаем память от копии*/
			for(i = 0; i < n; i++)
			{
				free(point_vert_copy[i].nears);
			}
			free(point_vert_copy);
		}
	}
	/*Освобождаем оставшуюся память*/
	free(parent);
	free(mark);
	free(result);
	for(i = 0; i < n; i++)
	{
		free(point_vert[i].nears);
	}
	free(point_vert);
	return 0;
}

/*Тела функций*/

/*Функция получения данных из файла*/
int input_data()
{
	int i;
	int j;
	char filename[30];
	FILE *file;
	printf("Введите имя файла: \n");
	scanf("%s",filename);
	if ((file = fopen(filename, "r")) == NULL)
	{
		printf("Файл не найден!\n");
		return 0;
	}
	/*Проверка графа на "простоту"*/
	fscanf(file, "%d", &n); /*считываем кол-во вершин*/
	if(n==0)
	{
		printf("Пустой граф!\n");
		return 0;
	}
	if(n==1)
	{
		printf("Тривиальный граф.\n");
		return 0;
	}
	/*Выделяем память под список смежности и массив марок*/
	point_vert = (struct vertexs *)malloc(n*sizeof(struct vertexs));
	mark = (int *)malloc(n*sizeof(int));
	parent = (int *)malloc(n*sizeof(int));
	for(i = 0; i < n; i++)
	{
		/*Считываем номер вершины и её степень*/
		fscanf(file, "%d", &point_vert[i].number);
		fscanf(file, "%d", &point_vert[i].degree);
		/*Если степень вершины не нулевая, то считываем смежные вершины*/
		if(point_vert[i].degree != 0)
		{
			point_vert[i].nears = (int *)malloc(point_vert[i].degree*sizeof(int));
			for(j = 0; j < point_vert[i].degree; j++)
			{
				fscanf(file, "%d", &point_vert[i].nears[j]);
			}
		}

	}
	/*Закрываем за собой файл, т.к. он уже не нужен*/
	fclose(file);
	return 1;
}
/*Рассматриваем компаненты связности. Если есть одна компнента,то граф связанный,
иначе не связанный.*/
int connectivity(struct vertexs *points, int not_point)
{
	int i;
	int components;
	mark_clear();
	components = 0;
	/*Начинаем обход вершин*/
	for(i = 0; i < n; i++)
	{
		if(i == not_point)
			continue;
		if((mark[i] != 1))
		{
			components += 1;
			dfs(points, i, -1);
		}
	}
	mark_clear();
	return components;
}
/*Функция поиска в глубину*/
void dfs(struct vertexs *points, int j, int not_point)
{
	int i;
	int time_var;
	mark[j] = 1;
	if(points[j].degree != 0)
	{
		for(i = 0; i < points[j].degree; i++)
		{
			time_var = points[j].nears[i];
			if (time_var == not_point)
				continue;
			if((mark[time_var] != 1) && (time_var != not_point))
			{
				dfs(points, time_var, not_point);
			}
		}
	}
}
/*Функция очистки массива меток*/
void mark_clear()
{
	int i;
	for (i = 0; i < n; i++)
	{
		mark[i] = 0;
	}
}
void parent_clear()
{
	int i;
	for (i = 0; i < n; i++)
	{
		parent[i] = -1;
	}
}
/*Доработанная функция dfs для поиска на циклы*/
int dfs_up(struct vertexs *points, int j, int not_point)
{
	int i;
	int time_var;
	mark[j] = 1;
	for(i = 0; i < points[j].degree; i++)
	{
		time_var = points[j].nears[i];
		if (time_var == not_point)
			continue;
		if(mark[time_var] == 0)
		{
			parent[time_var] = j;
			if(dfs_up(points, time_var, not_point) == 1)
				return 1;
		}
		else if((mark[time_var] == 1) && (time_var != parent[j]))
		{
			cycle_flag = 0;
			return 1;
		}
	}
	mark[j] = 2;
	return 0;
}
void create_copy()
{
	int i, j;
	for(i = 0; i < n; i++)
	{
		for(j = 0; j < point_vert_copy[i].degree; j++)
		{
			point_vert_copy[i].nears[j] = point_vert[i].nears[j];
		}
	}
}
