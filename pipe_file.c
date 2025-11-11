#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>  // 包含 wait() 函数声明

int main() {
    // 管道1：父进程 → 子进程（父写子读）
    int pipe_parent_to_child[2];
    // 管道2：子进程 → 父进程（子写父读）
    int pipe_child_to_parent[2];
    char buff[1024] = {0};  // 数据缓冲区，初始化避免脏数据
    int ret = 0;

    // 创建第一个管道（父→子）
    if (pipe(pipe_parent_to_child) == -1) {
        perror("pipe (parent→child) create failed");
        exit(EXIT_FAILURE);
    }

    // 创建第二个管道（子→父）
    if (pipe(pipe_child_to_parent) == -1) {
        perror("pipe (child→parent) create failed");
        // 失败时关闭已创建的第一个管道，避免资源泄漏
        close(pipe_parent_to_child[0]);
        close(pipe_parent_to_child[1]);
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {  // fork失败
        perror("fork failed");
        // 关闭所有已创建的管道文件描述符
        close(pipe_parent_to_child[0]);
        close(pipe_parent_to_child[1]);
        close(pipe_child_to_parent[0]);
        close(pipe_child_to_parent[1]);
        exit(EXIT_FAILURE);
    } else if (pid > 0) {  // 父进程：写pipe1 → 读pipe2
        // 父进程不需要读pipe1、写pipe2，提前关闭（关键：避免管道阻塞）
        close(pipe_parent_to_child[0]);  // 关闭pipe1的读端
        close(pipe_child_to_parent[1]);  // 关闭pipe2的写端

        // 向子进程写数据（pipe1的写端）
        const char* parent_msg = "hello son";
        ssize_t write_len = write(pipe_parent_to_child[1], parent_msg, strlen(parent_msg));
        if (write_len == -1) {
            perror("parent write to child failed");
            ret = EXIT_FAILURE;
            goto PARENT_CLEANUP;  // 出错跳转到资源清理
        }
        printf("父进程：已发送数据 → %s（字节数：%zd）\n", parent_msg, write_len);

        // 关闭pipe1写端（告知子进程：父进程已写完，避免子进程读阻塞）
        close(pipe_parent_to_child[1]);

        // 从子进程读数据（pipe2的读端）
        ssize_t read_len = read(pipe_child_to_parent[0], buff, sizeof(buff) - 1);
        if (read_len == -1) {
            perror("parent read from child failed");
            ret = EXIT_FAILURE;
            goto PARENT_CLEANUP;
        } else if (read_len == 0) {
            printf("父进程：子进程管道写端已关闭（无数据可读）\n");
        }

        // 手动添加字符串结束符，避免乱码
        buff[read_len] = '\0';
        printf("父进程 接收：%s\n", buff);

PARENT_CLEANUP:
        // 清理父进程剩余的管道文件描述符
        close(pipe_child_to_parent[0]);
        // 等待子进程结束，避免产生僵尸进程
        wait(NULL);
        printf("父进程：子进程已退出，通信完成\n");

    } else {  // 子进程：读pipe1 → 写pipe2
        // 子进程不需要写pipe1、读pipe2，提前关闭（关键：避免管道阻塞）
        close(pipe_parent_to_child[1]);  // 关闭pipe1的写端
        close(pipe_child_to_parent[0]);  // 关闭pipe2的读端

        // 从父进程读数据（pipe1的读端）
        ssize_t read_len = read(pipe_parent_to_child[0], buff, sizeof(buff) - 1);
        if (read_len == -1) {
            perror("child read from parent failed");
            ret = EXIT_FAILURE;
            goto CHILD_CLEANUP;
        } else if (read_len == 0) {
            printf("子进程：父进程管道写端已关闭（无数据可读）\n");
        }

        buff[read_len] = '\0';
        printf("子进程 接收：%s\n", buff);

        // 向父进程写数据（pipe2的写端）
        const char* child_msg = "hello parent";
        ssize_t write_len = write(pipe_child_to_parent[1], child_msg, strlen(child_msg));
        if (write_len == -1) {
            perror("child write to parent failed");
            ret = EXIT_FAILURE;
            goto CHILD_CLEANUP;
        }
        printf("子进程：已发送数据 → %s（字节数：%zd）\n", child_msg, write_len);

        // 关闭pipe2写端（告知父进程：子进程已写完）
        close(pipe_child_to_parent[1]);

CHILD_CLEANUP:
        // 清理子进程剩余的管道文件描述符
        close(pipe_parent_to_child[0]);
        exit(ret);  // 子进程退出，返回执行结果
    }

    return ret;
}#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>  // 包含 wait() 函数声明

int main() {
    // 管道1：父进程 → 子进程（父写子读）
    int pipe_parent_to_child[2];
    // 管道2：子进程 → 父进程（子写父读）
    int pipe_child_to_parent[2];
    char buff[1024] = {0};  // 数据缓冲区，初始化避免脏数据
    int ret = 0;

    // 创建第一个管道（父→子）
    if (pipe(pipe_parent_to_child) == -1) {
        perror("pipe (parent→child) create failed");
        exit(EXIT_FAILURE);
    }

    // 创建第二个管道（子→父）
    if (pipe(pipe_child_to_parent) == -1) {
        perror("pipe (child→parent) create failed");
        // 失败时关闭已创建的第一个管道，避免资源泄漏
        close(pipe_parent_to_child[0]);
        close(pipe_parent_to_child[1]);
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {  // fork失败
        perror("fork failed");
        // 关闭所有已创建的管道文件描述符
        close(pipe_parent_to_child[0]);
        close(pipe_parent_to_child[1]);
        close(pipe_child_to_parent[0]);
        close(pipe_child_to_parent[1]);
        exit(EXIT_FAILURE);
    } else if (pid > 0) {  // 父进程：写pipe1 → 读pipe2
        // 父进程不需要读pipe1、写pipe2，提前关闭（关键：避免管道阻塞）
        close(pipe_parent_to_child[0]);  // 关闭pipe1的读端
        close(pipe_child_to_parent[1]);  // 关闭pipe2的写端

        // 向子进程写数据（pipe1的写端）
        const char* parent_msg = "hello son";
        ssize_t write_len = write(pipe_parent_to_child[1], parent_msg, strlen(parent_msg));
        if (write_len == -1) {
            perror("parent write to child failed");
            ret = EXIT_FAILURE;
            goto PARENT_CLEANUP;  // 出错跳转到资源清理
        }
        printf("父进程：已发送数据 → %s（字节数：%zd）\n", parent_msg, write_len);

        // 关闭pipe1写端（告知子进程：父进程已写完，避免子进程读阻塞）
        close(pipe_parent_to_child[1]);

        // 从子进程读数据（pipe2的读端）
        ssize_t read_len = read(pipe_child_to_parent[0], buff, sizeof(buff) - 1);
        if (read_len == -1) {
            perror("parent read from child failed");
            ret = EXIT_FAILURE;
            goto PARENT_CLEANUP;
        } else if (read_len == 0) {
            printf("父进程：子进程管道写端已关闭（无数据可读）\n");
        }

        // 手动添加字符串结束符，避免乱码
        buff[read_len] = '\0';
        printf("父进程 接收：%s\n", buff);

PARENT_CLEANUP:
        // 清理父进程剩余的管道文件描述符
        close(pipe_child_to_parent[0]);
        // 等待子进程结束，避免产生僵尸进程
        wait(NULL);
        printf("父进程：子进程已退出，通信完成\n");

    } else {  // 子进程：读pipe1 → 写pipe2
        // 子进程不需要写pipe1、读pipe2，提前关闭（关键：避免管道阻塞）
        close(pipe_parent_to_child[1]);  // 关闭pipe1的写端
        close(pipe_child_to_parent[0]);  // 关闭pipe2的读端

        // 从父进程读数据（pipe1的读端）
        ssize_t read_len = read(pipe_parent_to_child[0], buff, sizeof(buff) - 1);
        if (read_len == -1) {
            perror("child read from parent failed");
            ret = EXIT_FAILURE;
            goto CHILD_CLEANUP;
        } else if (read_len == 0) {
            printf("子进程：父进程管道写端已关闭（无数据可读）\n");
        }

        buff[read_len] = '\0';
        printf("子进程 接收：%s\n", buff);

        // 向父进程写数据（pipe2的写端）
        const char* child_msg = "hello parent";
        ssize_t write_len = write(pipe_child_to_parent[1], child_msg, strlen(child_msg));
        if (write_len == -1) {
            perror("child write to parent failed");
            ret = EXIT_FAILURE;
            goto CHILD_CLEANUP;
        }
        printf("子进程：已发送数据 → %s（字节数：%zd）\n", child_msg, write_len);

        // 关闭pipe2写端（告知父进程：子进程已写完）
        close(pipe_child_to_parent[1]);

CHILD_CLEANUP:
        // 清理子进程剩余的管道文件描述符
        close(pipe_parent_to_child[0]);
        exit(ret);  // 子进程退出，返回执行结果
    }

    return ret;
}
