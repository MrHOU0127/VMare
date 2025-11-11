#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

#define TICKET_TOTAL 500  // 门票总数（常量定义，便于维护）
int ticket = TICKET_TOTAL;  // 共享门票变量
pthread_mutex_t ticket_mutex;  // 互斥锁：保护共享变量的原子操作

/**
 * @brief 售票线程函数（统一逻辑，避免重复代码）
 * @param args 线程编号（用于区分不同线程）
 * @return 无
 */
void* sell_ticket(void *args) {
    int thread_num = *(int*)args;  // 取出线程编号
    free(args);  // 释放主线程分配的内存（避免内存泄漏）

    while (1) {
        // 1. 加锁：确保"读ticket→判断→打印→自减"是原子操作
        pthread_mutex_lock(&ticket_mutex);

        // 2. 检查门票是否售罄（加锁后再判断，避免竞态条件）
        if (ticket <= 0) {
            pthread_mutex_unlock(&ticket_mutex);  // 解锁后再退出，避免死锁
            break;
        }

        // 3. 售票操作（加锁后执行，确保线程安全）
        printf("线程%d（ID：%ld）售出1张门票，当前售出第%d张\n",
               thread_num, pthread_self(), ticket);
        ticket--;  // 共享变量自减（原子操作，需锁保护）

        // 4. 解锁：释放锁，让其他线程可以操作
        pthread_mutex_unlock(&ticket_mutex);

        // （可选）模拟售票耗时，让线程切换更明显，便于观察效果
        usleep(1000);  // 微秒级休眠，不影响主线程等待
    }

    // 线程结束提示
    printf("线程%d售票结束，剩余门票：%d\n", thread_num, ticket);
    return NULL;
}

int main() {
    pthread_t tid1, tid2;
    int ret;

    // 1. 初始化互斥锁（必须在创建线程前初始化）
    ret = pthread_mutex_init(&ticket_mutex, NULL);
    if (ret != 0) {
        fprintf(stderr, "互斥锁初始化失败：%s\n", strerror(ret));
        exit(EXIT_FAILURE);
    }

    // 2. 为线程传递编号（动态分配，避免栈内存竞争）
    int *p1 = malloc(sizeof(int));
    int *p2 = malloc(sizeof(int));
    *p1 = 1;  // 线程1编号
    *p2 = 2;  // 线程2编号

    // 3. 创建两个售票线程（共用同一个售票函数，减少代码冗余）
    ret = pthread_create(&tid1, NULL, sell_ticket, (void*)p1);
    if (ret != 0) {
        fprintf(stderr, "创建线程1失败：%s\n", strerror(ret));
        free(p1); free(p2);  // 失败时释放已分配内存
        pthread_mutex_destroy(&ticket_mutex);  // 销毁锁
        exit(EXIT_FAILURE);
    }

    ret = pthread_create(&tid2, NULL, sell_ticket, (void*)p2);
    if (ret != 0) {
        fprintf(stderr, "创建线程2失败：%s\n", strerror(ret));
        free(p1); free(p2);
        pthread_cancel(tid1);  // 取消已创建的线程1
        pthread_mutex_destroy(&ticket_mutex);
        exit(EXIT_FAILURE);
    }

    // 4. 等待两个线程结束（避免主线程提前退出）
    ret = pthread_join(tid1, NULL);
    if (ret != 0) {
        fprintf(stderr, "等待线程1失败：%s\n", strerror(ret));
    }
    ret = pthread_join(tid2, NULL);
    if (ret != 0) {
        fprintf(stderr, "等待线程2失败：%s\n", strerror(ret));
    }

    // 5. 销毁互斥锁（不再使用时释放资源）
    pthread_mutex_destroy(&ticket_mutex);

    // 最终门票核对
    printf("主线程：所有线程结束，最终剩余门票：%d（应剩余0）\n", ticket);
    return EXIT_SUCCESS;
}
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

#define TICKET_TOTAL 500  // 门票总数（常量定义，便于维护）
int ticket = TICKET_TOTAL;  // 共享门票变量
pthread_mutex_t ticket_mutex;  // 互斥锁：保护共享变量的原子操作

/**
 * @brief 售票线程函数（统一逻辑，避免重复代码）
 * @param args 线程编号（用于区分不同线程）
 * @return 无
 */
void* sell_ticket(void *args) {
    int thread_num = *(int*)args;  // 取出线程编号
    free(args);  // 释放主线程分配的内存（避免内存泄漏）

    while (1) {
        // 1. 加锁：确保"读ticket→判断→打印→自减"是原子操作
        pthread_mutex_lock(&ticket_mutex);

        // 2. 检查门票是否售罄（加锁后再判断，避免竞态条件）
        if (ticket <= 0) {
            pthread_mutex_unlock(&ticket_mutex);  // 解锁后再退出，避免死锁
            break;
        }

        // 3. 售票操作（加锁后执行，确保线程安全）
        printf("线程%d（ID：%ld）售出1张门票，当前售出第%d张\n",
               thread_num, pthread_self(), ticket);
        ticket--;  // 共享变量自减（原子操作，需锁保护）

        // 4. 解锁：释放锁，让其他线程可以操作
        pthread_mutex_unlock(&ticket_mutex);

        // （可选）模拟售票耗时，让线程切换更明显，便于观察效果
        usleep(1000);  // 微秒级休眠，不影响主线程等待
    }

    // 线程结束提示
    printf("线程%d售票结束，剩余门票：%d\n", thread_num, ticket);
    return NULL;
}

int main() {
    pthread_t tid1, tid2;
    int ret;

    // 1. 初始化互斥锁（必须在创建线程前初始化）
    ret = pthread_mutex_init(&ticket_mutex, NULL);
    if (ret != 0) {
        fprintf(stderr, "互斥锁初始化失败：%s\n", strerror(ret));
        exit(EXIT_FAILURE);
    }

    // 2. 为线程传递编号（动态分配，避免栈内存竞争）
    int *p1 = malloc(sizeof(int));
    int *p2 = malloc(sizeof(int));
    *p1 = 1;  // 线程1编号
    *p2 = 2;  // 线程2编号

    // 3. 创建两个售票线程（共用同一个售票函数，减少代码冗余）
    ret = pthread_create(&tid1, NULL, sell_ticket, (void*)p1);
    if (ret != 0) {
        fprintf(stderr, "创建线程1失败：%s\n", strerror(ret));
        free(p1); free(p2);  // 失败时释放已分配内存
        pthread_mutex_destroy(&ticket_mutex);  // 销毁锁
        exit(EXIT_FAILURE);
    }

    ret = pthread_create(&tid2, NULL, sell_ticket, (void*)p2);
    if (ret != 0) {
        fprintf(stderr, "创建线程2失败：%s\n", strerror(ret));
        free(p1); free(p2);
        pthread_cancel(tid1);  // 取消已创建的线程1
        pthread_mutex_destroy(&ticket_mutex);
        exit(EXIT_FAILURE);
    }

    // 4. 等待两个线程结束（避免主线程提前退出）
    ret = pthread_join(tid1, NULL);
    if (ret != 0) {
        fprintf(stderr, "等待线程1失败：%s\n", strerror(ret));
    }
    ret = pthread_join(tid2, NULL);
    if (ret != 0) {
        fprintf(stderr, "等待线程2失败：%s\n", strerror(ret));
    }

    // 5. 销毁互斥锁（不再使用时释放资源）
    pthread_mutex_destroy(&ticket_mutex);

    // 最终门票核对
    printf("主线程：所有线程结束，最终剩余门票：%d（应剩余0）\n", ticket);
    return EXIT_SUCCESS;
}

