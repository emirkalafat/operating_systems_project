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
            // Ana program kapandığında pipe kapanır, biz de çıkalım
            break;
        }

        // "exit" komutu geldiyse çık
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

        double result = a + b;

        // Sonucu kaydet
        char res_str[64];
        snprintf(res_str, sizeof(res_str), "%.2f", result);

        // saver çağır
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

        // Sonucu stdout'a yaz (ana programa)
        printf("%.2f\n", result);
        fflush(stdout);
    }

    return 0;
}