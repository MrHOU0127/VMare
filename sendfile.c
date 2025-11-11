#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/sendfile.h>

int main() {
    // 直接设置源文件和目标文件的固定名称（硬编码）
    const char* src_file = "test.txt";  // 源文件变量
    const char* dest_file = "test_backup.txt";  // 目标文件变量

    // 打开源文件（用硬编码的变量）
    int src_fd = open(src_file, O_RDONLY);
    if (src_fd == -1) {
        perror("打开源文件失败");
        exit(EXIT_FAILURE);
    }

    // 打开/创建目标文件（用硬编码的变量）
    int dest_fd = open(dest_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd == -1) {
        perror("打开/创建目标文件失败");
        close(src_fd);
        exit(EXIT_FAILURE);
    }

    // 后续获取文件大小、sendfile拷贝、关闭文件的逻辑完全不变...
    struct stat stat_buf;
    if (fstat(src_fd, &stat_buf) == -1) {
        perror("获取文件状态失败");
        close(src_fd);
        close(dest_fd);
        exit(EXIT_FAILURE);
    }
    off_t file_size = stat_buf.st_size;

    ssize_t bytes_copied = sendfile(dest_fd, src_fd, NULL, file_size);
    if (bytes_copied == -1) {
        perror("sendfile拷贝失败");
    }
    else if (bytes_copied != file_size) {
        fprintf(stderr, "拷贝不完整，预期拷贝 %ld 字节，实际拷贝 %ld 字节\n", file_size, bytes_copied);
    }
    else {
        printf("文件拷贝成功，共拷贝 %ld 字节\n", bytes_copied);
    }

    close(src_fd);
    close(dest_fd);
    return 0;
}
