#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <result>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *result = argv[1];

    FILE *f = fopen("results.txt", "a");
    if (!f)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    fprintf(f, "%s\n", result);
    fclose(f);

    return 0;
}