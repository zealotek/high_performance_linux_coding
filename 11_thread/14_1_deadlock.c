#include<stdio.h>
#include<unistd.h>
#include<pthread.h>

pthread_mutex_t a;
pthread_mutex_t b;

void func(void *arg)
{
	pthread_mutex_lock(&b);
	printf("2: hold b wait a\n");
	sleep(5);
	pthread_mutex_lock(&a);
	printf("2: hold b hold a\n");
}

int main()
{
	pthread_t id;

	pthread_mutex_init(&a, NULL);
	pthread_mutex_init(&b, NULL);

	pthread_create(&id, NULL, func, NULL);
	pthread_mutex_lock(&a);
	printf("1: hold a wait b\n");
	sleep(5);
	pthread_mutex_lock(&b);
	printf("1: hold a hold b\n");
}
