#include "config.h"

#define swap(a, b)                                                             \
  {                                                                            \
    __local data_t *tmp = a;                                                   \
    a = b;                                                                     \
    b = tmp;                                                                   \
  }

// Данная функция принимает на вход массив input размера n
// Разбивает его на отрезки размером BLOCK_SIZE
// Для каждого отдельного отрезка считает inclusive scan
// Сохраняет результат в input
// Дополнительно сохраняет в массив chunks последний элемент из каждого отрезка
// (со сдвигом вправо на 1 позицию)

// Пример (пусть BLOCK_SIZE = 4)
// input:  [1, 2, 3, 1, 0, 2, 2, 4, 1, 2, 3, 3, 1, 2, 4, 0]
// split:  |1, 2, 3, 1| 0, 2, 2, 4| 1, 2, 3, 3| 1, 2, 4, 0|
// scan:   |1, 3, 6, 7| 0, 2, 4, 8| 1, 3, 6, 9| 1, 3, 7, 7|
//                    \_________  \_________  \_________
//                              \           \           \
// chunks: [         0,          7,          8,          9]
// input:  [1, 3, 6, 7, 0, 2, 4, 8, 1, 3, 6, 9, 1, 3, 7, 7]
__kernel void scan(__global data_t *input, int n, __global data_t *chunks) {
  // https://man.opencl.org/get_global_id.html
  const int global_id = get_global_id(0);
  // https://man.opencl.org/get_local_id.html
  const int local_id = get_local_id(0);
  // https://man.opencl.org/get_group_id.html
  const int group_id = get_group_id(0);

  const int needed_groups = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;

  if (group_id >= needed_groups)
    return;

  // это аналог __shared__ памяти в CUDA
  // нам нужно два массива для эффективной реализации scan в блоке
  __local data_t buf1[BLOCK_SIZE];
  __local data_t buf2[BLOCK_SIZE];

  // копируем в кеш
  if (global_id < n) {
    buf1[local_id] = input[global_id];
    buf2[local_id] = input[global_id];
  } else {
    buf1[local_id] = 0;
    buf2[local_id] = 0;
  }

  // это аналог __syncthreads(), синхронизация всех потоков внутри блока
  barrier(CLK_LOCAL_MEM_FENCE);


  // https://en.wikipedia.org/wiki/Prefix_sum#Algorithm_1:_Shorter_span,_more_parallel
  // надо реализовать схему из ссылки
  __local data_t *a = buf1;
  __local data_t *b = buf2;
  // swap(a, b) -- позволит быстро переставить местами указатели

  // Solution #1 with `2 * log(BLOCK_SIZE)` barriers
  for (int p = 1; p < BLOCK_SIZE; p <<= 1) {
    if (local_id - p >= 0) b[local_id] += a[local_id - p];
    barrier(CLK_LOCAL_MEM_FENCE);
    a[local_id] = b[local_id];
    barrier(CLK_LOCAL_MEM_FENCE);
  }

  // Solution #2 with `log(BLOCK_SIZE)` barriers that seg. faulted if written as loop
  // for (int p = 1; p < BLOCK_SIZE; p <<= 1) {
  //   if (local_id - p >= 0) b[local_id] = a[local_id - p] + a[local_id];
  //   else b[local_id] = a[local_id];
  //   swap(a, b)
  //   barrier(CLK_LOCAL_MEM_FENCE);
  // }

  // Working solution #2 that doesn't use loops. WHY??? Maybe it's side effect of compilier optimization
  // if (local_id - 1 >= 0) b[local_id] = a[local_id - 1] + a[local_id];
  // else b[local_id] = a[local_id];
  // swap(a, b)
  // barrier(CLK_LOCAL_MEM_FENCE);
  // if (local_id - 2 >= 0) b[local_id] = a[local_id - 2] + a[local_id];
  // else b[local_id] = a[local_id];
  // swap(a, b)
  // barrier(CLK_LOCAL_MEM_FENCE);
  // if (local_id - 4 >= 0) b[local_id] = a[local_id - 4] + a[local_id];
  // else b[local_id] = a[local_id];
  // swap(a, b)
  // barrier(CLK_LOCAL_MEM_FENCE);
  // if (local_id - 8 >= 0) b[local_id] = a[local_id - 8] + a[local_id];
  // else b[local_id] = a[local_id];
  // swap(a, b)
  // barrier(CLK_LOCAL_MEM_FENCE);
  // if (local_id - 16 >= 0) b[local_id] = a[local_id - 16] + a[local_id];
  // else b[local_id] = a[local_id];
  // swap(a, b)
  // barrier(CLK_LOCAL_MEM_FENCE);
  // if (local_id - 32 >= 0) b[local_id] = a[local_id - 32] + a[local_id];
  // else b[local_id] = a[local_id];
  // swap(a, b)
  // barrier(CLK_LOCAL_MEM_FENCE);
  // if (local_id - 64 >= 0) b[local_id] = a[local_id - 64] + a[local_id];
  // else b[local_id] = a[local_id];
  // swap(a, b)
  // barrier(CLK_LOCAL_MEM_FENCE);
  // if (local_id - 128 >= 0) b[local_id] = a[local_id - 128] + a[local_id];
  // else b[local_id] = a[local_id];
  // swap(a, b)

  // save last item of each chunk into chunks
  if (local_id == BLOCK_SIZE - 1 && chunks != NULL) {
    if (group_id == needed_groups - 1) {
      chunks[0] = 0;
    } else {
      chunks[group_id + 1] = a[local_id];
    }
  }

  // save input
  if (global_id < n) {
    input[global_id] = a[local_id];
  }
}

__kernel void add_chunk_sum(__global data_t *input, int n,
                            __global data_t *chunk_sum) {
  const int global_id = get_global_id(0);
  const int group_id = get_group_id(0);

  if (global_id >= n)
    return;

  input[global_id] += chunk_sum[group_id];
}
