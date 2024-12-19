#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{

    int in_fd = atoi(argv[1]);
    int out_fd = atoi(argv[2]);

    char buffer[128];
    for (;;)
    {
        ssize_t read_count = read(in_fd, buffer, sizeof(buffer) - 1);
        if (read_count <= 0)
            break;

        buffer[read_count] = '\0';

        if (strncmp(buffer, "exit", 4) == 0)
            break;

        double a, b;
        if (sscanf(buffer, "%lf %lf", &a, &b) != 2)
        {
            const char *err_msg = "Invalid input\n";
            write(out_fd, err_msg, strlen(err_msg));
            continue;
        }

        double result = a * b;

        char res_str[64];
        snprintf(res_str, sizeof(res_str), "%.2f", result);

        pid_t pid = fork();
        if (pid == 0)
        {
            execl("./saver", "./saver", res_str, (char *)NULL);
            perror("execl saver");
            exit(EXIT_FAILURE);
        }
        else if (pid > 0)
        {
            wait(NULL);
        }

        snprintf(res_str, sizeof(res_str), "%.2f\n", result);
        write(out_fd, res_str, strlen(res_str));
    }

    return 0;
}