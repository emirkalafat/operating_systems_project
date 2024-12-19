#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

// Alt süreç başlatma fonksiyonu
// Bu fonksiyon, verilen exec yoluna göre bir alt süreç başlatır,
// stdin ve stdout'u ilgili pipe'lara yönlendirir.
void start_process(const char *prog, int in_pipe[2], int out_pipe[2])
{
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        // Child process
        // stdin için: in_pipe'dan okunacak, dolayısıyla in_pipe[0] -> STDIN_FILENO
        dup2(in_pipe[0], STDIN_FILENO);
        close(in_pipe[1]);
        close(in_pipe[0]);

        // stdout için: out_pipe'a yazılacak, dolayısıyla out_pipe[1] -> STDOUT_FILENO
        dup2(out_pipe[1], STDOUT_FILENO);
        close(out_pipe[0]);
        close(out_pipe[1]);

        // Diğer tüm pipe uçlarını kapat
        // (Ana program bunları kapatacak, bu süreçte gerek yok.)

        execl(prog, prog, (char *)NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }
    else
    {
        // Parent process
        // Bu pipe uçları parent'ta farklı amaçlarla kullanılacak
        // Parent okumak için out_pipe[0]'ı kullanacak, yazmak için in_pipe[1]'i kullanacak
        close(in_pipe[0]);  // parent içeriye yazacak, okuyucu uç kapansın
        close(out_pipe[1]); // parent çıkışı okuyacak, yazıcı uç kapansın
    }
}

int main()
{
    int add_pipes_in[2];  // main -> addition
    int add_pipes_out[2]; // addition -> main

    int sub_pipes_in[2];  // main -> subtraction
    int sub_pipes_out[2]; // subtraction -> main

    int mul_pipes_in[2];  // main -> multiplication
    int mul_pipes_out[2]; // multiplication -> main

    int div_pipes_in[2];  // main -> division
    int div_pipes_out[2]; // division -> main

    if (pipe(add_pipes_in) == -1 || pipe(add_pipes_out) == -1 ||
        pipe(sub_pipes_in) == -1 || pipe(sub_pipes_out) == -1 ||
        pipe(mul_pipes_in) == -1 || pipe(mul_pipes_out) == -1 ||
        pipe(div_pipes_in) == -1 || pipe(div_pipes_out) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // 4 alt süreci başlat
    start_process("./addition", add_pipes_in, add_pipes_out);
    start_process("./subtraction", sub_pipes_in, sub_pipes_out);
    start_process("./multiplication", mul_pipes_in, mul_pipes_out);
    start_process("./division", div_pipes_in, div_pipes_out);

    // Artık menü döngüsüne girelim
    while (1)
    {
        printf("Calculator:\n");
        printf("1 - addition\n");
        printf("2 - subtraction\n");
        printf("3 - multiplication\n");
        printf("4 - division\n");
        printf("5 - exit\n");
        printf("Select operation: ");

        int choice;
        scanf("%d", &choice);

        if (choice == 5)
        {
            // exit seçimi
            // Tüm alt süreçlere "exit" mesajı gönderelim
            char exit_msg[] = "exit\n";
            write(add_pipes_in[1], exit_msg, strlen(exit_msg));
            write(sub_pipes_in[1], exit_msg, strlen(exit_msg));
            write(mul_pipes_in[1], exit_msg, strlen(exit_msg));
            write(div_pipes_in[1], exit_msg, strlen(exit_msg));

            // Alt süreçlerin bitmesini bekle
            // 4 alt süreç var
            for (int i = 0; i < 4; i++)
            {
                wait(NULL);
            }

            printf("Exiting.\n");
            break;
        }
        else
        {
            double a, b;
            printf("Enter first operand: ");
            scanf("%lf", &a);
            printf("Enter second operand: ");
            scanf("%lf", &b);

            char buffer[128];
            snprintf(buffer, sizeof(buffer), "%.2f %.2f\n", a, b);

            int in_fd, out_fd;
            switch (choice)
            {
            case 1: // addition
                in_fd = add_pipes_in[1];
                out_fd = add_pipes_out[0];
                break;
            case 2: // subtraction
                in_fd = sub_pipes_in[1];
                out_fd = sub_pipes_out[0];
                break;
            case 3: // multiplication
                in_fd = mul_pipes_in[1];
                out_fd = mul_pipes_out[0];
                break;
            case 4: // division
                in_fd = div_pipes_in[1];
                out_fd = div_pipes_out[0];
                break;
            default:
                printf("Invalid choice!\n");
                continue;
            }

            // İlgili alt sürece verileri yolla
            write(in_fd, buffer, strlen(buffer));

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
    }

    return 0;
}
