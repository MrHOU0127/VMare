#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<assert.h>
#include<stdbool.h>

#define BUFFER_SIZE 10

typedef struct Node{
    int data;
    struct Node* next;
}Node;

typedef struct{
    Node* front;
    Node* rear;//
    int size;
    int capacity;
}Queue;

void InitQueue(Queue* q){
    q->front=NULL;
    q->rear=NULL;
    q->size=0;
    q->capacity = BUFFER_SIZE;
}

bool QueueIsEmpty(Queue* q){
    return q->size == 0;
}

bool QueueIsFull(Queue* q){
    return q->size == q->capacity;
}

void QueuePush(Queue* q,int val){
    Node* temp = (Node*)malloc(sizeof(Node));
    if(temp==NULL){
        perror("enqueue malloc failed\n");
        return;
    }
    temp->data = val;
    temp->next = NULL;
    if(QueueIsEmpty(q)){
        q->front = temp;
        q->rear = temp;
    }else{
        q->rear->next = temp;
        q->rear = temp;
    }
    
    q->size++;
    return;
}

int QueuePop(Queue* q){
    assert(q != NULL && !QueueIsEmpty(q));
    int tmpdata = q->front->data;
    Node* tmp = q->front;
    q->front = q->front->next;
    free(tmp);
    q->size--;
    if(QueueIsEmpty(q)){
        q->rear = NULL;
    }
    return tmpdata;
}

int QueueFront(Queue* q) {
	assert(q != NULL && !QueueIsEmpty(q));
	return q->front->data;
}


int QueueBack(Queue* q) {
	assert(q != NULL && !QueueIsEmpty(q));
	return q->rear->data;
}


// 清空队列（释放所有节点）
void QueueClear(Queue* q) {
    if(q == NULL) return;
    while (!QueueIsEmpty(q)) {
        QueuePop(q);  // 复用Pop的释放逻辑
    }
}

// 销毁队列（仅清空节点，不释放q本身，因为q是栈上变量）
void QueueDestroy(Queue* q) {
    if(q == NULL) return;
    QueueClear(q);  // 释放所有节点
    // 无需free(q)：q是栈上变量，程序退出时自动释放
}


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

//int buffer[BUFFER_SIZE];
Queue buffer;
int tickets_id = 0;

void* produce(void* arg){
    while(1){
        pthread_mutex_lock(&mutex);

        //判断队列是否满，如果满了，需要等待；
        while(QueueIsFull(&buffer)){
            pthread_cond_wait(&not_full,&mutex);
        }

        tickets_id++;
        QueuePush(&buffer,tickets_id);
        printf("游乐场生产一张门票 id :%d，还剩%d 门票\n",tickets_id,buffer.size);
        
        pthread_cond_broadcast(&not_empty);//唤醒操作
        pthread_mutex_unlock(&mutex);

        usleep(1000000);
    }
}

void* consumer(void* args){
    
    while(1){
        pthread_mutex_lock(&mutex);
        while(QueueIsEmpty(&buffer)){
            pthread_cond_wait(&not_empty,&mutex);
        }
        int get_ticket = QueuePop(&buffer);

        printf("%s,消费了一张门票 id:%d\n",(char*)args,get_ticket);
        pthread_cond_broadcast(&not_full);
        pthread_mutex_unlock(&mutex);
        usleep(8000000);
    }
}

int main(){
    InitQueue(&buffer);
    pthread_t p;
    pthread_create(&p,NULL,produce,NULL);
    
    pthread_t st1,st2;
    pthread_create(&st1,NULL,consumer,"章三");
    pthread_create(&st2,NULL,consumer,"利斯");

    pthread_detach(p);  // 生产者线程分离，无需主线程join
    pthread_join(st1,NULL);
    pthread_join(st2,NULL);
    QueueDestroy(&buffer);
    return 0;
}
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<assert.h>
#include<stdbool.h>

#define BUFFER_SIZE 10

typedef struct Node{
    int data;
    struct Node* next;
}Node;

typedef struct{
    Node* front;
    Node* rear;//
    int size;
    int capacity;
}Queue;

void InitQueue(Queue* q){
    q->front=NULL;
    q->rear=NULL;
    q->size=0;
    q->capacity = BUFFER_SIZE;
}

bool QueueIsEmpty(Queue* q){
    return q->size == 0;
}

bool QueueIsFull(Queue* q){
    return q->size == q->capacity;
}

void QueuePush(Queue* q,int val){
    Node* temp = (Node*)malloc(sizeof(Node));
    if(temp==NULL){
        perror("enqueue malloc failed\n");
        return;
    }
    temp->data = val;
    temp->next = NULL;
    if(QueueIsEmpty(q)){
        q->front = temp;
        q->rear = temp;
    }else{
        q->rear->next = temp;
        q->rear = temp;
    }
    
    q->size++;
    return;
}

int QueuePop(Queue* q){
    assert(q != NULL && !QueueIsEmpty(q));
    int tmpdata = q->front->data;
    Node* tmp = q->front;
    q->front = q->front->next;
    free(tmp);
    q->size--;
    if(QueueIsEmpty(q)){
        q->rear = NULL;
    }
    return tmpdata;
}

int QueueFront(Queue* q) {
	assert(q != NULL && !QueueIsEmpty(q));
	return q->front->data;
}


int QueueBack(Queue* q) {
	assert(q != NULL && !QueueIsEmpty(q));
	return q->rear->data;
}


// 清空队列（释放所有节点）
void QueueClear(Queue* q) {
    if(q == NULL) return;
    while (!QueueIsEmpty(q)) {
        QueuePop(q);  // 复用Pop的释放逻辑
    }
}

// 销毁队列（仅清空节点，不释放q本身，因为q是栈上变量）
void QueueDestroy(Queue* q) {
    if(q == NULL) return;
    QueueClear(q);  // 释放所有节点
    // 无需free(q)：q是栈上变量，程序退出时自动释放
}


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

//int buffer[BUFFER_SIZE];
Queue buffer;
int tickets_id = 0;

void* produce(void* arg){
    while(1){
        pthread_mutex_lock(&mutex);

        //判断队列是否满，如果满了，需要等待；
        while(QueueIsFull(&buffer)){
            pthread_cond_wait(&not_full,&mutex);
        }

        tickets_id++;
        QueuePush(&buffer,tickets_id);
        printf("游乐场生产一张门票 id :%d，还剩%d 门票\n",tickets_id,buffer.size);
        
        pthread_cond_broadcast(&not_empty);//唤醒操作
        pthread_mutex_unlock(&mutex);

        usleep(1000000);
    }
}

void* consumer(void* args){
    
    while(1){
        pthread_mutex_lock(&mutex);
        while(QueueIsEmpty(&buffer)){
            pthread_cond_wait(&not_empty,&mutex);
        }
        int get_ticket = QueuePop(&buffer);

        printf("%s,消费了一张门票 id:%d\n",(char*)args,get_ticket);
        pthread_cond_broadcast(&not_full);
        pthread_mutex_unlock(&mutex);
        usleep(8000000);
    }
}

int main(){
    InitQueue(&buffer);
    pthread_t p;
    pthread_create(&p,NULL,produce,NULL);
    
    pthread_t st1,st2;
    pthread_create(&st1,NULL,consumer,"章三");
    pthread_create(&st2,NULL,consumer,"利斯");

    pthread_detach(p);  // 生产者线程分离，无需主线程join
    pthread_join(st1,NULL);
    pthread_join(st2,NULL);
    QueueDestroy(&buffer);
    return 0;
}

