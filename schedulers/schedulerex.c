#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <error.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#define QUEUE_LEN 10
#define FILENAME "./logfiles"

struct request_queue
{
	int request_blocks;
	struct request_queue *prev;
	struct request_queue *next;
};

FILE *file_fd;
int position = 0;
int distance = 0;
int flag = 0;

void destory_queue(struct request_queue *queue_head)
{
	if(queue_head)
	{
		free(queue_head);
		queue_head= = NULL;
	}

}

struct request_queue *init_rq()
{
	struct request_queue *queue_head = NULL;
	if(!(queue_head = (struct request_queue *)malloc(sizeof(struct request_queue))))
	{
		perror("error malloc");
		exit(1);
	}

	queue_head->next = NULL;
	queue_head->prev = NULL;
	queue_head->request_blocks = 101;

	return queue_head;
}

void *add_request_thread(void * queue_head)
{
	struct request_queue *ptemp = (struct request_queue *)queue_head;
	int track = 0;
	struct request_queue *prq = NULL;
	while(1)
	{
		prq = NULL;
		printf("input disk track[1-100]:");
		scanf("%d", &track);

		if(track > 100)
		{
			flag = 1;
			return (void *)NULL;
		}

		if((prq = (struct request_queue *)malloc(sizeof(struct request_queue))) == NULL)
		{
			perror("error malloc");
			exit(1);
		}

		prq->request_blocks = track;
		prq->next = NULL;
		prq->prev = NULL;

		while(ptemp->next != NULL)
		{
			ptemp = ptemp->next;
		}

		ptemp->next = prq;
		prq->prev = ptemp;

		printf("add sucess");

	}
}

void *deal_request_thread(void *queue_head)
{
	struct request_queue *phead = (struct request_queue *)queue_head;
	struct request_queue *ptemp = phead;

	while(1)
	{
		if(phead->next != NULL)
		{
			ptemp = phead->next;
			fprintf(file_fd, "%d->", ptemp->request_blocks);
			distance += abs(ptemp->request_blocks - position);
			position = ptemp->request_blocks;

			if(ptemp->next != NULL)
			{
				ptemp->prev->next = ptemp->next;
				ptemp->next->prev = ptemp->prev;
			}
				ptemp->prev->next = ptemp->next;

			if(ptemp)
			{
				free(ptemp);
				ptemp = NULL;
			}
			printf("delete sucess");
			sleep(2);
		}
		else
		{
			if(flag == 1)
			{
				return (void *)NULL;
			}
		}
	}
}

int main(void)
{
	pthread_t add_thread, deal_thread;
	struct request_queue *queue_head = NULL;
	srand((unsigned)time(NULL));

	if(!(file_fd = fopen(FILENAME, "w")))
	{
		perror("error fopen");
		exit(1);
	}

	position = rand()%100 + 1;
	queue_head = init_rq();

	if(pthread_create(&add_thread, NULL, (void *)add_request_thread, (void *)queue_head))
	{
		perror("error pthread_create");
		exit(1);
	}

	if(pthread_create(&deal_thread, NULL, (void *)deal_request_thread, (void *)queue_head))
	{
		perror("error pthread_create");
		exit(1);
	}

	pthread_join(add_thread, NULL);
	pthread_join(deal_thread, NULL);

	fclose(file_fd);
	destory_queue(queue_head);
	return 0;

}