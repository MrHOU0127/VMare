#define _POSIX_C_SOURCE 200809L  // 特性宏必须放在最前面（启用POSIX标准接口）
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

/**
 * @brief SIGALRM 信号处理函数：超时提示并退出程序
 * @param sig 接收的信号编号（此处固定为 SIGALRM）
 */
void timeout_handler(int sig) {
    // 信号处理函数中尽量使用可重入函数（printf 原则上非可重入，但简单场景可用）
    // 输出换行符保证提示信息完整（避免和输入缓冲区内容混在一起）
    printf("\nTime out! The answer is 2\n");
    _exit(EXIT_FAILURE);  // 使用标准退出码，更规范
}

int main() {
    printf("1+1=? 请在5秒内输入答案（仅需输入数字）：\n");

    // 1. 初始化信号动作结构体
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));  // 清空结构体，避免脏数据
    sa.sa_handler = timeout_handler;  // 设置信号处理函数
    sa.sa_flags = SA_RESTART;  // 重启被信号中断的系统调用（如 fgets）
    sigemptyset(&sa.sa_mask);  // 清空信号掩码（处理信号时不屏蔽其他信号）

    // 2. 注册 SIGALRM 信号处理函数
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction failed");  // 错误信息更具体
        exit(EXIT_FAILURE);
    }

    // 3. 启动5秒定时器（设置后开始倒计时）
    alarm(5);

    // 4. 读取用户输入（安全读取，处理换行符和缓冲区问题）
    char arr[1024] = {0};  // 初始化缓冲区，避免脏数据
    if (fgets(arr, sizeof(arr), stdin) == NULL) {
        // fgets 返回 NULL 可能是：信号中断、读取错误、EOF
        if (errno == EINTR) {
            // 被信号中断（超时），无需额外处理（信号处理函数已退出）
            return EXIT_FAILURE;
        } else {
            perror("fgets failed");
            exit(EXIT_FAILURE);
        }
    }

    // 5. 取消定时器（用户按时输入，无需超时）
    alarm(0);

    // 6. 处理输入内容（去除 fgets 读取的换行符，避免比较失败）
    size_t input_len = strlen(arr);
    if (input_len > 0 && arr[input_len - 1] == '\n') {
        arr[input_len - 1] = '\0';  // 替换换行符为字符串结束符
    }

    // 7. 验证答案（支持输入 "2" 或 " 2 " 等带空格的情况，更友好）
    // 用 atoi 转换为数字比较，避免因输入空格导致判断错误
    int answer = atoi(arr);
    if (answer == 2) {
        printf("Beautiful! Correct answer: %d\n", answer);
    } else {
        printf("Wrong answer! The correct answer is 2\n");
    }

    return EXIT_SUCCESS;
}
