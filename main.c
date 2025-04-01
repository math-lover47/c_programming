#include <stdio.h>
#include <omp.h>

#define num_steps 1000000000 // Larger for better accuracy

int main()
{
  double start_time = omp_get_wtime();

  double step = 1.0 / (double)num_steps;
  double pi = 0.0;

#pragma omp parallel reduction(+ : pi)
  {
    double sum = 0.0;

#pragma omp for
    for (int i = 0; i < num_steps; i++)
    {
      double x = (i + 0.5) * step;
      sum += 4.0 / (1.0 + x * x);
    }

    pi += sum * step;
  }

  printf("Pi = %.15f\n", pi);

  double end_time = omp_get_wtime();
  printf("Time: %.4f seconds\n", end_time - start_time);
  return 0;
}
