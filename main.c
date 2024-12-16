#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

int main()
{
    char operation[16];
    char arg1[32], arg2[32];

    printf("Birinci sayiyi giriniz: ");
    scanf("%s", arg1);

    printf("Ikinci sayiyi giriniz: ");
    scanf("%s", arg2);

    printf("Islemi giriniz (add, sub, mul, div): ");
    scanf("%s", operation);

    // Kullanıcıdan gelen işleme göre çalıştırılacak program adı
    char *prog_name = NULL;
    if (strcmp(operation, "add") == 0)
    {
        prog_name = "./add";
    }
    else if (strcmp(operation, "sub") == 0)
    {
        prog_name = "./sub";
    }
    else if (strcmp(operation, "mul") == 0)
    {
        prog_name = "./mul";
    }
    else if (strcmp(operation, "div") == 0)
    {
        prog_name = "./div";
    }
    else
    {
        fprintf(stderr, "Gecersiz islem!\n");
        exit(EXIT_FAILURE);
    }

    // Pipe oluştur
    int fd[2];
    if (pipe(fd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        // Child process
        // Çıktıyı pipe'ın yazma ucuna yönlendir
        close(fd[0]); // Okuma ucunu kapat
        if (dup2(fd[1], STDOUT_FILENO) == -1)
        {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        close(fd[1]);

        // execve için argüman dizisi
        // Argümanlar: prog_name, arg1, arg2, NULL şeklinde
        char *args[] = {prog_name, arg1, arg2, NULL};

        // execve çağrısı
        // execve için mutlak yol gereklidir, burada ./ ile kullanıldı.
        // Ortam değişkenlerini aktarmak istemiyorsanız env NULL olabilir.
        extern char **environ;
        if (execve(prog_name, args, environ) == -1)
        {
            perror("execve");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        // Parent process
        close(fd[1]); // Yazma ucunu kapat
        // Çocuk sürecin tamamlanmasını bekle
        if (wait(NULL) == -1)
        {
            perror("wait");
            exit(EXIT_FAILURE);
        }

        // Pipe üzerinden sonucu oku
        char buffer[128];
        ssize_t n = read(fd[0], buffer, sizeof(buffer) - 1);
        if (n == -1)
        {
            perror("read");
            exit(EXIT_FAILURE);
        }
        buffer[n] = '\0'; // Null terminate
        close(fd[0]);

        printf("Sonuc: %s\n", buffer);
    }

    return 0;
}