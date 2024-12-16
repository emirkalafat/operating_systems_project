#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Eksik argüman!\n");
        exit(EXIT_FAILURE);
    }
    double a = atof(argv[1]);
    double b = atof(argv[2]);

    if (b == 0.0)
    {
        fprintf(stderr, "Sifira bolme hatasi!\n");
        exit(EXIT_FAILURE);
    }

    double result = a / b;
    printf("%.2f\n", result);
    return 0;
}