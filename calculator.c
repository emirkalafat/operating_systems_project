#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

/*
   Bu kodda struct kullanılmadan her alt süreç için ayrı ayrı
   in ve out pipe'ları tanımlanmıştır.

   addition için: add_in, add_out
   subtraction için: sub_in, sub_out
   multiplication için: mul_in, mul_out
   division için: div_in, div_out
*/

/* Belirtilen program için bir alt süreç başlatır.
   in_pipe[0] child tarafından okunacak, out_pipe[1] child tarafından yazılacak.
   Child'a in_fd ve out_fd argüman olarak verilir. */
static void create_subprocess(const char *prog, int in_pipe[2], int out_pipe[2])
{
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0)
    {
        // Child process
        close(in_pipe[1]);  // child okumak için in_pipe[0]'ı kullanacak, bu yüzden in_pipe[1] kapatılır
        close(out_pipe[0]); // child yazmak için out_pipe[1]'i kullanır, out_pipe[0] kapatılır

        // in_fd ve out_fd değerleri string'e çevrilip exec'e argüman olarak verilir.
        char in_fd_str[16], out_fd_str[16];
        snprintf(in_fd_str, sizeof(in_fd_str), "%d", in_pipe[0]);
        snprintf(out_fd_str, sizeof(out_fd_str), "%d", out_pipe[1]);

        execl(prog, prog, in_fd_str, out_fd_str, (char *)NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }
    else
    {
        // Parent process
        // Parent yazmak için in_pipe[1]'i, okumak için out_pipe[0]'ı kullanacak
        // Bu nedenle parent için in_pipe[0] ve out_pipe[1] gereksiz
        close(in_pipe[0]);
        close(out_pipe[1]);
    }
}

/* Kullanıcıdan menü seçimi al */
static int get_user_choice()
{
    printf("Calculator:\n");
    printf("1 - addition\n");
    printf("2 - subtraction\n");
    printf("3 - multiplication\n");
    printf("4 - division\n");
    printf("5 - exit\n");
    printf("Select operation: ");

    int choice;
    if (scanf("%d", &choice) != 1)
    {
        while (getchar() != '\n')
            ; // tampon temizliği
        return -1;
    }
    return choice;
}

/* Kullanıcıdan iki operand al */
static int get_operands(double *a, double *b)
{
    printf("Enter first operand: ");
    if (scanf("%lf", a) != 1)
    {
        while (getchar() != '\n')
            ;
        return 0;
    }
    printf("Enter second operand: ");
    if (scanf("%lf", b) != 1)
    {
        while (getchar() != '\n')
            ;
        return 0;
    }
    return 1;
}

int main(void)
{
    int add_in[2], add_out[2];
    int sub_in[2], sub_out[2];
    int mul_in[2], mul_out[2];
    int div_in[2], div_out[2];

    if (pipe(add_in) == -1 || pipe(add_out) == -1 ||
        pipe(sub_in) == -1 || pipe(sub_out) == -1 ||
        pipe(mul_in) == -1 || pipe(mul_out) == -1 ||
        pipe(div_in) == -1 || pipe(div_out) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    create_subprocess("./addition", add_in, add_out);
    create_subprocess("./subtraction", sub_in, sub_out);
    create_subprocess("./multiplication", mul_in, mul_out);
    create_subprocess("./division", div_in, div_out);

    while (1)
    {
        int choice = get_user_choice();
        if (choice < 1 || choice > 5)
        {
            printf("Invalid choice!\n");
            continue;
        }

        if (choice == 5)
        {
            // exit komutu
            const char *exit_msg = "exit\n";
            write(add_in[1], exit_msg, strlen(exit_msg));
            write(sub_in[1], exit_msg, strlen(exit_msg));
            write(mul_in[1], exit_msg, strlen(exit_msg));
            write(div_in[1], exit_msg, strlen(exit_msg));

            // Alt süreçlerin bitmesini bekle
            for (int i = 0; i < 4; i++)
                wait(NULL);

            printf("Exiting.\n");
            break;
        }

        double a, b;
        if (!get_operands(&a, &b))
        {
            printf("Invalid input for operands!\n");
            continue;
        }

        char request[128];
        snprintf(request, sizeof(request), "%.2f %.2f\n", a, b);

        int in_fd, out_fd;
        switch (choice)
        {
        case 1:
            in_fd = add_in[1];
            out_fd = add_out[0];
            break;
        case 2:
            in_fd = sub_in[1];
            out_fd = sub_out[0];
            break;
        case 3:
            in_fd = mul_in[1];
            out_fd = mul_out[0];
            break;
        case 4:
            in_fd = div_in[1];
            out_fd = div_out[0];
            break;
        default:
            continue; // burada default'a düşmez ama yine de
        }

        // Seçilen alt sürece operandları gönder
        write(in_fd, request, strlen(request));

        // Alt süreçten sonucu oku
        char result_buf[128];
        ssize_t n = read(out_fd, result_buf, sizeof(result_buf) - 1);
        if (n > 0)
        {
            result_buf[n] = '\0';
            printf("Result: %s\n", result_buf);
        }
        else
        {
            printf("No response from subprocess.\n");
        }
    }

    return 0;
}