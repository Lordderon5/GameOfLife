#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 25
#define M 80
#define H 200

int** initDynamicMemory();
void cleanDynamicMemory(int** a);
void output(int** a);
void input(int** a, int** b, char** argv);
int update(int** a, int** b);
int countCells(int** b, int i, int j);
int getCell(int** b, int i, int j);
void updateOldGeneration(int** a, int** b);
int hashCreate(int** a);
int hash(int i, int j, int n);
void add_to_table(int* hash_table, int hash_res);
void hashMove(int* table);
int hashSearch(int* table, int hash_res);
void fill_table(int* table);
void sleep_(int n);
void play(char** argv);
void printna();

int main(int argc, char* argv[]) {
  initscr();
  curs_set(0);
  noecho();
  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);
  if (argc == 2) {
    play(argv);
  } else {
    printna();
  }
  return 0;
}

void printna() { printf("n/a"); }

void play(char** argv) {
  int** generation;
  int** generationOld;
  int hash_table[H];
  int hash_res;
  int flag = 0;
  int n = 10;

  generation = initDynamicMemory();
  generationOld = initDynamicMemory();
  fill_table(hash_table);
  input(generation, generationOld, argv);
  output(generation);

  while (1) {
    int key = getch();
    if (key == KEY_DOWN) n++;
    if (key == KEY_UP) {
      if (n > 0) n--;
    }
    if (key == 'q') break;

    hash_res = update(generation, generationOld);
    if (hash_res == 0) {
      output(generation);
      flag = 1;
    } else {
      flag = hashSearch(hash_table, hash_res);
      output(generation);
    }
    if (flag == 1) {
      break;
      endwin();
    }
    sleep_(n);
  }

  cleanDynamicMemory(generation);
  cleanDynamicMemory(generationOld);
}

int** initDynamicMemory() {
  int** p;
  p = (int**)malloc(N * sizeof(int*));
  for (int i = 0; i < N; i++) {
    p[i] = (int*)malloc(M * sizeof(int));
  }
  return p;
}

void cleanDynamicMemory(int** a) {
  for (int i = 0; i < N; i++) {
    free(a[i]);
  }
  free(a);
}

void output(int** a) {
  char key;
  if ((key = getch()) == ERR) {
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < M; j++) {
        if (a[i][j] == 0) mvaddch(i, j, '.');
        if (a[i][j] == 1) mvaddch(i, j, 'o');
        refresh();
      }
    }
  }
}

void input(int** a, int** b, char** argv) {
  FILE* fp = fopen(argv[1], "r");
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < M; j++) {
      fscanf(fp, "%d", &a[i][j]);
      b[i][j] = a[i][j];
    }
  }
  fclose(fp);
}

int update(int** a, int** b) {
  updateOldGeneration(a, b);
  int cellsCount = 0;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < M; j++) {
      cellsCount = countCells(b, i, j);
      if ((b[i][j] == 0) && (cellsCount == 3)) {
        a[i][j] = 1;
      } else {
        if ((cellsCount < 2) || (cellsCount > 3)) {
          a[i][j] = 0;
        }
      }
    }
  }
  return hashCreate(a);
}

void updateOldGeneration(int** a, int** b) {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < M; j++) {
      b[i][j] = a[i][j];
    }
  }
}

int countCells(int** b, int i, int j) {
  int res;
  if ((i != 0) && (i != N - 1) && (j != 0) && (j != M - 1)) {
    res = getCell(b, i - 1, j) + getCell(b, i - 1, j - 1) +
          getCell(b, i, j - 1) + getCell(b, i + 1, j - 1) +
          getCell(b, i + 1, j) + getCell(b, i + 1, j + 1) +
          getCell(b, i, j + 1) + getCell(b, i - 1, j + 1);
  }
  if ((i == 0) && (j != 0) && (j != M - 1)) {
    res = getCell(b, N - 1, j) + getCell(b, N - 1, j - 1) +
          getCell(b, i, j - 1) + getCell(b, i + 1, j - 1) +
          getCell(b, i + 1, j) + getCell(b, i + 1, j + 1) +
          getCell(b, i, j + 1) + getCell(b, N - 1, j + 1);
  }
  if ((i == 0) && (j == 0)) {
    res = getCell(b, N - 1, j) + getCell(b, H - 1, M - 1) +
          getCell(b, i, M - 1) + getCell(b, i + 1, M - 1) +
          getCell(b, i + 1, j) + getCell(b, i + 1, j + 1) +
          getCell(b, i, j + 1) + getCell(b, H - 1, j + 1);
  }
  if ((i != 0) && (i != N - 1) && (j == 0)) {
    res = getCell(b, i - 1, j) + getCell(b, i - 1, M - 1) +
          getCell(b, i, M - 1) + getCell(b, i + 1, M - 1) +
          getCell(b, i + 1, j) + getCell(b, i + 1, j + 1) +
          getCell(b, i, j + 1) + getCell(b, i - 1, j + 1);
  }
  if ((i == N - 1) && (j != 0) && (j != M - 1)) {
    res = getCell(b, i - 1, j) + getCell(b, i - 1, j - 1) +
          getCell(b, i, j - 1) + getCell(b, 0, j - 1) + getCell(b, 0, j) +
          getCell(b, 0, j + 1) + getCell(b, i, j + 1) +
          getCell(b, i - 1, j + 1);
  }
  if ((i == N - 1) && (j == M - 1)) {
    res = getCell(b, i - 1, j) + getCell(b, i - 1, j - 1) +
          getCell(b, i, j - 1) + getCell(b, 0, j - 1) + getCell(b, 0, j) +
          getCell(b, 0, 0) + getCell(b, i, 0) + getCell(b, i - 1, 0);
  }
  if ((i != 0) && (i != N - 1) && (j == M - 1)) {
    res = getCell(b, i - 1, j) + getCell(b, i - 1, j - 1) +
          getCell(b, i, j - 1) + getCell(b, i + 1, j - 1) +
          getCell(b, i + 1, j) + getCell(b, i + 1, 0) + getCell(b, i, 0) +
          getCell(b, i - 1, 0);
  }
  return (res);
}

int getCell(int** b, int i, int j) {
  int res;
  if ((i < 0) || (i > N - 1) || (j < 0) || (j > M - 1)) {
    res = 0;
  } else {
    res = b[i][j];
  }
  return res;
}

int hashCreate(int** a) {
  int counti = 0, countj = 0, countn = 0, n = 1;
  int result;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < M; j++) {
      if (a[i][j] == 1) {
        counti += i;
        countj += j;
        countn += n;
      }
    }
  }
  result = hash(counti, countj, countn);
  return result;
}

int hash(int i, int j, int n) {
  int res;
  int key_i = 674;
  int key_j = 19123;
  int key_n = 17441;
  res = (i * key_i + j * key_j + n * key_n);
  return res;
}

void add_to_table(int* hash_table, int hash_res) {
  for (int i = 0; i < H; i++) {
    if (hash_table[i] == 0) {
      hash_table[i] = hash_res;
      break;
    } else {
      hashMove(hash_table);
      hash_table[H - 1] = hash_res;
      break;
    }
  }
}

void hashMove(int* table) {
  for (int i = 0; i < H - 1; i++) {
    table[i] = table[i + 1];
  }
}

int hashSearch(int* table, int hash_res) {
  int flag = 0;
  for (int i = 0; i < H; i++) {
    if (table[i] == hash_res) {
      flag = 1;
      break;
    }
  }
  if (flag == 0) {
    add_to_table(table, hash_res);
  }
  return flag;
}

void fill_table(int* table) {
  for (int i = 0; i < H; i++) {
    table[i] = -1;
  }
}

void sleep_(int n) { usleep(50000 * n); }
