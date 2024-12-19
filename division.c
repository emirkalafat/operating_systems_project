#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
    char line[128];

    while (1)
    {
        if (fgets(line, sizeof(line), stdin) == NULL)
        {
            break;
        }

        if (strncmp(line, "exit", 4) == 0)
        {
            break;
        }

        double a, b;
        if (sscanf(line, "%lf %lf", &a, &b) != 2)
        {
            fprintf(stderr, "Invalid input\n");
            continue;
        }

        if (b == 0.0)
        {
            fprintf(stderr, "Division by zero error\n");
            printf("Error\n");
            fflush(stdout);
            continue;
        }

        double result = a / b;

        char res_str[64];
        snprintf(res_str, sizeof(res_str), "%.2f", result);

        if (fork() == 0)
        {
            execl("./saver", "./saver", res_str, (char *)NULL);
            perror("execl saver");
            exit(EXIT_FAILURE);
        }
        else
        {
            wait(NULL);
        }

        printf("%.2f\n", result);
        fflush(stdout);
    }

    return 0;
}