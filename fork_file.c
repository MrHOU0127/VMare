#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const int BUF_SIZE = 100;  // 分段读取缓冲区大小（可根据需求调整）
    char arr[BUF_SIZE] = {0};  // 分段读取缓冲区，初始化避免脏数据
    const char* filename = "io.txt";  // 文件名常量
    FILE* fp = NULL;

    // 打开文件：a+模式（读+追加写，不存在则创建）
    fp = fopen(filename, "a+");
    if (fp == NULL) {
        perror("fopen failed");
        return EXIT_FAILURE;
    }

    // a+模式默认指针在文件末尾，移至开头才能读取全部内容
    fseek(fp, 0, SEEK_SET);
    printf("File content (char-separated):\n");

    // 核心：while循环分段读取，直到文件结束（feof返回非0）
    while (!feof(fp)) {
        // 每次读取 BUF_SIZE-1 个字符（预留1字节存临时结束符）
        size_t read_count = fread(arr, sizeof(char), BUF_SIZE - 1, fp);

        // 检查读取是否出错（区分「出错」和「文件结束」）
        if (ferror(fp)) {
            perror("fread failed");
            fclose(fp);
            return EXIT_FAILURE;
        }

        // 若本次读取到数据，添加字符串结束符后遍历输出
        if (read_count > 0) {
            arr[read_count] = '\0';  // 确保当前分段是合法字符串

            // 遍历当前分段的字符，保持原有的「字符-」格式
            char* p = arr;
            while (*p != '\0') {
                printf("%c-", *p);
                p++;
            }
        }

        // 清空缓冲区，避免下一轮读取残留上一轮数据
        memset(arr, 0, BUF_SIZE);
    }

    printf("\n");  // 结尾换行，格式整洁
    puts("File read completed!");

    // 关闭文件，检查关闭是否失败
    if (fclose(fp) == EOF) {
        perror("fclose failed");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const int BUF_SIZE = 100;  // 分段读取缓冲区大小（可根据需求调整）
    char arr[BUF_SIZE] = {0};  // 分段读取缓冲区，初始化避免脏数据
    const char* filename = "io.txt";  // 文件名常量
    FILE* fp = NULL;

    // 打开文件：a+模式（读+追加写，不存在则创建）
    fp = fopen(filename, "a+");
    if (fp == NULL) {
        perror("fopen failed");
        return EXIT_FAILURE;
    }

    // a+模式默认指针在文件末尾，移至开头才能读取全部内容
    fseek(fp, 0, SEEK_SET);
    printf("File content (char-separated):\n");

    // 核心：while循环分段读取，直到文件结束（feof返回非0）
    while (!feof(fp)) {
        // 每次读取 BUF_SIZE-1 个字符（预留1字节存临时结束符）
        size_t read_count = fread(arr, sizeof(char), BUF_SIZE - 1, fp);

        // 检查读取是否出错（区分「出错」和「文件结束」）
        if (ferror(fp)) {
            perror("fread failed");
            fclose(fp);
            return EXIT_FAILURE;
        }

        // 若本次读取到数据，添加字符串结束符后遍历输出
        if (read_count > 0) {
            arr[read_count] = '\0';  // 确保当前分段是合法字符串

            // 遍历当前分段的字符，保持原有的「字符-」格式
            char* p = arr;
            while (*p != '\0') {
                printf("%c-", *p);
                p++;
            }
        }

        // 清空缓冲区，避免下一轮读取残留上一轮数据
        memset(arr, 0, BUF_SIZE);
    }

    printf("\n");  // 结尾换行，格式整洁
    puts("File read completed!");

    // 关闭文件，检查关闭是否失败
    if (fclose(fp) == EOF) {
        perror("fclose failed");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
