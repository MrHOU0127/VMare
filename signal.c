nclude <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <errno.h>

/**
 * @brief SIGINT 信号处理函数（捕获 Ctrl+C）
 * @param sig 接收的信号编号（此处固定为 SIGINT=2）
 * 说明：信号处理函数应尽量使用可重入函数，避免复杂操作
 */
void sigint_handler(int sig) {
    // 输出换行符：避免提示信息与当前行的 printf 内容混排（如 "hello,are you there?^C" 之后换行）
    printf("\nReceive SIGINT signal (number: %d), stopping program...\n", sig);
    exit(EXIT_SUCCESS);  // 使用标准退出码，区分正常退出与错误退出
}

int main(int argc, char const *argv[]) {
    // 1. 注册 SIGINT 信号处理函数（替换 signal 为 sigaction，更稳定、功能更强）
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));          // 清空结构体，避免脏数据
    sa.sa_handler = sigint_handler;      // 设置信号处理函数
    sa.sa_flags = 0;                     // 无特殊标志（如需重启系统调用可加 SA_RESTART）
    sigemptyset(&sa.sa_mask);            // 处理信号时不屏蔽其他信号

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction register SIGINT failed");  // 错误信息更具体
        return EXIT_FAILURE;
    }

    printf("Program started: Send Ctrl+C (SIGINT) to stop.\n");
    printf("------------------------------------------------\n");

    // 2. 循环输出提示信息（处理 sleep 被信号中断的情况）
    while (1) {
        // sleep(1) 可能被信号中断（返回剩余睡眠时间），需循环确保实际休眠1秒
        int sleep_ret = sleep(1);
        while (sleep_ret > 0) {
            sleep_ret = sleep(sleep_ret);  // 补全剩余睡眠时间
        }

        // 输出提示信息（加换行符保证格式整洁）
        printf("hello, are you there?\n");
    }

    return EXIT_SUCCESS;  // 理论上不会执行到此处（循环无退出条件）
}
