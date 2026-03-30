#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef void (*taskFnc_t)(int);

#define MAIN_LOOP_TASKS_COUNT 20
#define THREAD_COUNT 8
// theoretically tasks is ring buffer, but I don't want to implement it fully
#define TASKS_CAPACITY (MAIN_LOOP_TASKS_COUNT + THREAD_COUNT)

pthread_cond_t cv_work_available = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mut_tasks_read_access = PTHREAD_MUTEX_INITIALIZER;

void task_1(int task_id) {
  usleep((500 + rand() % 1000) * 1000);
  printf("Uloha %i typu 1 hotova!\n", task_id);
}

void task_2(int task_id) {
  usleep((800 + rand() % 800) * 1000);
  printf("Uloha %i typu 2 hotova!\n", task_id);
}

void task_3(int task_id) {
  usleep((1000 + rand() % 600) * 1000);
  printf("Uloha %i typu 3 hotova!\n", task_id);
}

// UKOL: vytvorte 8 vlaken, ktere budou zpracovavat ulohy typu 1, 2 a 3
//       - ulohy mohou prijit kdykoliv, tzn. neni dobre vytvaret pokazde nove
//       specializovane vlakno
//           - hlavni vlakno bude vytvaret ulohy a predavat je vlaknum funkci
//           submit_task
//       - udrzujte si pole 8 vlaken, ktere kazde bude cekat na prichod ulohy
//       - vlakno bude z fronty prebirat ulohy formou ukazatele na funkci
//       (taskFnc_t) a jeji poradove cislo

// meta information about task
typedef enum { T_EMPTY, T_WAITING, T_IN_PROGRESS, T_TERMINATE } task_meta;
// task encapsulation
typedef struct {
  taskFnc_t fnc;
  int id;
  task_meta meta;
} task;

// global buffer for tasks
task tasks[TASKS_CAPACITY];
int tasks_head_read = 0;
int tasks_head_write = 0;

void *thread_func(void *arg) {
  // because cond_var need this
  pthread_mutex_lock(&mut_tasks_read_access);

  while (1) {
    // needs locked mutex
    pthread_cond_wait(&cv_work_available, &mut_tasks_read_access);
    // while waiting it's unlocked, but after it it is locked again

    task *current = &tasks[tasks_head_read++];
    if (current->meta == T_EMPTY) { // is this spurious wakeup?
      tasks_head_read--;            // if so, don't read it
      continue;
    }
    if (current->meta == T_TERMINATE) {
      // must unlock, there is no future read access from this thread
      pthread_mutex_unlock(&mut_tasks_read_access);

      // notify others - they might want to terminate too
      pthread_cond_signal(&cv_work_available);
      break;
    }

    current->meta = T_IN_PROGRESS;

    { // THE WORK (+ allow others to read other work while this is running)
      // let others see work while this thread is working
      pthread_mutex_unlock(&mut_tasks_read_access);

      // do the work
      current->fnc(current->id);

      // make it empty for future ring buffer access (not needed here)
      current->meta = T_EMPTY;

      // there is another work to be done
      if (tasks_head_read != tasks_head_write) {
        pthread_cond_broadcast(&cv_work_available);
      }

      // this is for wait() to work correctly
      pthread_mutex_lock(&mut_tasks_read_access);
    }
  }

  return (void *)0;
}

void submit_task_full(int task_id, taskFnc_t func, task_meta meta) {
  // TODO: ulozit ulohu do fronty a signalizovat vlaknum, ze je tam nova uloha
  tasks[tasks_head_write++] = (task){func, task_id, meta};
  pthread_cond_signal(&cv_work_available);
}

// default meta = T_WAITING
void submit_task(int task_id, taskFnc_t func) {
  // TODO: ulozit ulohu do fronty a signalizovat vlaknum, ze je tam nova uloha
  submit_task_full(task_id, func, T_WAITING);
}

int main(int argc, char **argv) {
  srand(time(NULL));

  // TODO: inicializace, vytvoreni a spusteni 8 vlaken
  pthread_t thread_pool[THREAD_COUNT];
  for (int i = 0; i < THREAD_COUNT; ++i) {
    pthread_create(&thread_pool[i], NULL, thread_func, NULL);
  }
  for (int i = 0; i < TASKS_CAPACITY; ++i) {
    tasks[i].meta = T_EMPTY;
  }

  // priklad uloh ke zpracovani
  for (int i = 0; i < MAIN_LOOP_TASKS_COUNT; i++) {
    int task_type = rand() % 3;

    switch (task_type) {
    case 0:
      submit_task(i, &task_1);
      break;
    case 1:
      submit_task(i, &task_2);
      break;
    case 2:
      submit_task(i, &task_3);
      break;
    }
  }

  // TODO: signalizace vlaknum, ze uz nebudou zadne ulohy, korektni ukonceni
  // vlaken a programu

  for (int i = 0; i < THREAD_COUNT; ++i) {
    submit_task_full(0, 0, T_TERMINATE);
  }

  for (int i = 0; i < THREAD_COUNT; ++i) {
    pthread_join(thread_pool[i], NULL);
  }

  pthread_cond_destroy(&cv_work_available);
  pthread_mutex_destroy(&mut_tasks_read_access);

  printf("Program dokoncil praci!\n");

  return 0;
}
