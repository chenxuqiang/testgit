#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <error.h>
#include <pthread.h>
#include <math.h>

#define QUEUE_LEN 10

struct request_queue
{
	int request_blocks;
	struct request_queue *prev;
	struct request_queue *next;
};

struct request_queue * init_rq()
{
	struct request_queue *prq;
	prq = (struct request_queue *)malloc(sizeof(struct request_queue));
	if(prq == NULL)
	{
		perror("malloc error");
		exit(1);
	}

	prq->request_blocks = 100;
	prq->next = NULL;
	prq->prev = NULL;

	return prq;
}

void destory_queue(struct request_queue *queue_head)
{
	if(queue_head)
	{
		free(queue_head);
		queue_head = NULL;
	}
}

int add_request(struct request_queue *queue_head, int rq_bl)
{
	struct request_queue *prq;
	struct request_queue *ptemp;  
	prq = (struct request_queue *)malloc(sizeof(struct request_queue));
	if(prq == NULL)
	{
		perror("malloc error");
		exit(1);
	}

	prq->request_blocks = rq_bl;

	for(ptemp = queue_head; ptemp->next != NULL; ptemp = ptemp->next);

	ptemp->next = prq;
	prq->prev = ptemp;
	prq->next = NULL;

	return rq_bl;
}

int delete_request(struct request_queue *queue_head, int rq_bl)
{
	struct request_queue *ptemp;
	for(ptemp = queue_head; ptemp->next != NULL; ptemp = ptemp->next)
	{
		if(ptemp->request_blocks == rq_bl)
		{
			ptemp->prev->next = ptemp->next;
			ptemp->next->prev = ptemp->prev;
			free(ptemp);
			return rq_bl;
		}
	}

	return 0;
}

void delete_request_current(struct request_queue *current)
{
	if(current->next != NULL)
	{
		current->next->prev = current->prev;
		current->prev->next = current->next;
	}
	else
	{
		current->prev->next = NULL;
	}
	if(current)
	{
		free(current);
		current = NULL;
	}
}

void delete_request_from_head(struct request_queue *queue_head)
{
	struct request_queue *ptemp = queue_head->next;

	if(ptemp->next != NULL)
	{
		queue_head->next = ptemp->next;
		ptemp->next->prev = queue_head;
	}
	else
	{
		queue_head->next = NULL;
	}
	if(ptemp)
	{
		free(ptemp);
		ptemp = NULL;
	}
}

void delete_request_from_tail(struct request_queue *queue_tail)
{
	struct request_queue *ptemp = queue_tail;
	ptemp->prev->next = NULL;
	if(ptemp)
	{
		free(ptemp);
		ptemp = NULL;
	}
}

int create_request()
{
	//srand((unsigned)time(NULL));
	int request_blocks = rand()%100 + 1;
	return request_blocks;
}

void traverse_queue(struct request_queue *queue_head)
{
	struct request_queue *ptemp = queue_head->next;
	for(; ptemp != NULL; ptemp = ptemp->next)
	{
		printf("%d  ", ptemp->request_blocks);
	}

	printf("\n");
}

struct request_queue *sort_request(struct request_queue *queue_head)
{
	struct request_queue *ptemp1, *ptemp2;
	int temp_blocks = 0;
	for(ptemp1 = queue_head->next; ptemp1 != NULL; ptemp1 = ptemp1->next)
	{
		for(ptemp2 = ptemp1->next; ptemp2 != NULL; ptemp2 = ptemp2->next)
		{
			if(ptemp1->request_blocks > ptemp2->request_blocks)
			{
				temp_blocks = ptemp1->request_blocks;
				ptemp1->request_blocks = ptemp2->request_blocks;
				ptemp2->request_blocks = temp_blocks;
			}
		}
	}

	return queue_head;
}

/**
**FIFO 
**/
int FCFS(struct request_queue *queue_head, int position, int distance, FILE *file_fd)
{
	while(queue_head->next != NULL)
	{
		fprintf(file_fd, "%d->", queue_head->next->request_blocks);
		printf("%d->", queue_head->next->request_blocks);
		distance = distance + abs(position - queue_head->next->request_blocks);
		position = queue_head->next->request_blocks;
		delete_request_from_head(queue_head);
	}

	return distance;
}

int reverse_FCFS(struct request_queue *queue_head, struct request_queue *queue_tail, int position, int distance, FILE *file_fd)
{
	struct request_queue *ptail = queue_tail;
	for(; ptail != queue_head; ptail = ptail->prev)
	{
		fprintf(file_fd, "%d->", ptail->request_blocks);
		printf("%d->", ptail->request_blocks);
		distance = distance + abs(position - ptail->request_blocks);
		position = ptail->request_blocks;
		delete_request_from_tail(ptail);
	}

	return distance;
}

/**
**SSTF
**/
int SSTF(struct request_queue *queue_head, int position, int distance, FILE *file_fd)
{
	struct request_queue *ptemp = sort_request(queue_head);
	struct request_queue *pnext = NULL;

	while(ptemp->next != NULL)
	{
		if(position > ptemp->next->request_blocks)
		{
			ptemp = ptemp->next;
		}
		else
		{
			break;
		}
	}

	while(queue_head->next != NULL)
	{
		if(ptemp == queue_head)
		{
			distance = FCFS(queue_head, position, distance, file_fd);
		}
		else if(ptemp->next == NULL)
		{
			distance = reverse_FCFS(queue_head, ptemp, position, distance, file_fd);
		}
		else
		{
			pnext = ptemp->next;
			if(abs(ptemp->request_blocks - position) <= abs(pnext->request_blocks - position))
			{
				struct request_queue * pptemp = NULL;
				fprintf(file_fd, "%d->", ptemp->request_blocks);
				printf("%d->", ptemp->request_blocks);
				distance = distance + abs(ptemp->request_blocks - position);
				position = ptemp->request_blocks;
				pptemp = ptemp;
				ptemp = ptemp->prev;
				delete_request_current(pptemp);
			}
			else
			{
				struct request_queue *pptemp = NULL;
				fprintf(file_fd, "%d->", pnext->request_blocks);
				printf("%d->", pnext->request_blocks);
				distance = distance + abs(pnext->request_blocks - position);
				position = pnext->request_blocks;
				pptemp = pnext;
				delete_request_current(pptemp);
			}
		}
	}
	return distance;
}

int SCAN(struct request_queue *queue_head, int position, int distance, FILE *file_fd)
{
	struct request_queue *ptemp = sort_request(queue_head);
	//traverse_queue(queue_head);
	struct request_queue *pnext = NULL;

	while(ptemp->next != NULL)
	{
		if(position > ptemp->next->request_blocks)
		{
			ptemp = ptemp->next;
		}
		else
		{
			break;
		}
	}

	while(queue_head->next != NULL)
	{
		if(ptemp == queue_head)
		{
			distance = FCFS(queue_head, position, distance, file_fd);
		}
		else if(ptemp->next == NULL)
		{
			distance = reverse_FCFS(queue_head, ptemp, position, distance, file_fd);
		}
		else
		{
			pnext = ptemp->next;
			if(abs(ptemp->request_blocks - position) <= abs(pnext->request_blocks - position))
			{
				struct request_queue *pptemp = NULL;
				while(ptemp != queue_head)
				{
					fprintf(file_fd, "%d->", ptemp->request_blocks);
					printf("%d->", ptemp->request_blocks);
					distance += abs(ptemp->request_blocks - position);
					position = ptemp->request_blocks;
					pptemp = ptemp;
					ptemp = ptemp->prev;
					pptemp->next->prev = pptemp->prev;
					pptemp->prev->next = pptemp->next;

					if(pptemp)
					{
						free(pptemp);
						pptemp = NULL;
					}
				}
			}
			else
			{
				struct request_queue *pptemp = NULL;
				while(pnext != queue_head)
				{
					fprintf(file_fd, "%d->", pnext->request_blocks);
					printf("%d->", pnext->request_blocks);
					distance += abs(pnext->request_blocks - position);
					pptemp = pnext;
					pnext = pnext->prev;
					if(pptemp->next != NULL)
					{
						pptemp->next->prev = pptemp->prev;
						pptemp->prev->next = pptemp->next;
					}
					if(pptemp)
					{
						free(pptemp);
						pptemp = NULL;
					}
				}

				ptemp = pnext;
			}
		}
	}

	return distance;
}

int CSCAN(struct request_queue *queue_head, int position, int distance, FILE *file_fd)
{
	struct request_queue *ptemp = sort_request(queue_head);
	struct request_queue *pnext = NULL;

	while(ptemp->next != NULL)
	{
		if(position > ptemp->next->request_blocks)
		{
			ptemp = ptemp->next;
		}
		else
		{
			break;
		}
	}

	while(queue_head->next != NULL)
	{
		if(ptemp == queue_head)
		{
			while(ptemp->next != NULL)
			{
				ptemp = ptemp->next;
			}

			distance = reverse_FCFS(queue_head, ptemp, position, distance, file_fd);
		}
		else if(ptemp->next == NULL)
		{
			distance = reverse_FCFS(queue_head, ptemp, position, distance, file_fd);
		}
		else
		{
			pnext = ptemp->next;
			if(abs(ptemp->request_blocks - position) <= abs(pnext->request_blocks - position))
			{
				struct request_queue *pptemp = NULL;
				while(ptemp != queue_head)
				{
					fprintf(file_fd, "%d->", ptemp->request_blocks);
					printf("%d->", ptemp->request_blocks);
					distance += abs(ptemp->request_blocks - position);
					position = ptemp->request_blocks;
					pptemp = ptemp;
					ptemp = ptemp->prev;

					pptemp->next->prev = pptemp->prev;
					pptemp->prev->next = pptemp->next;

					if(pptemp)
					{
						free(pptemp);
						pptemp = NULL;
					}
				}
				
			}
			else
			{
				struct request_queue *pptemp = NULL;
				while(pnext != queue_head)
				{
					fprintf(file_fd, "%d->", pnext->request_blocks);
					printf("%d->", pnext->request_blocks);
					distance += abs(pnext->request_blocks - position);
					position = pnext->request_blocks;
					pptemp = pnext;
					pnext = pnext->prev;

					if(pptemp->next != NULL)
					{
						pptemp->next->prev = pptemp->prev;
						pptemp->prev->next = pptemp->next;
					}
		
					if(pptemp)
					{
						free(pptemp);
						pptemp = NULL;
					}
				}

				ptemp = pnext;
			}
		}
	}

	return distance;
}

void write_log(FILE *file_fd, int choose, int distance)
{
	fprintf(file_fd, "\ndistance is %d\n", distance);
	printf("\ndistance is %d\n", distance);
}

int choose(char *algorithms[], struct request_queue *queue_head, int distance, int position, FILE *file_fd)
{
	int choose = 0;
	printf("-------------disk scheduler-------------\n");
	printf("1. FCFS\n2. SSTF\n3. SCAN\n4. CSCAN\n5. EXIT\n");
	while(1)
	{
		printf("input your choose[1-5]: ");
		scanf("%d", &choose);
		switch(choose)
		{
			case 1:
				fprintf(file_fd, ">>start schdeuler [%s]\n", algorithms[1]);
				distance = FCFS(queue_head, position, distance, file_fd);
				write_log(file_fd, choose, distance);
				break;
			case 2:
				fprintf(file_fd, ">>start schdeuler [%s]\n", algorithms[2]);
				distance = SSTF(queue_head, position, distance, file_fd);
				write_log(file_fd, choose, distance);
				break;
			case 3:
				fprintf(file_fd, ">>start schdeuler [%s]\n", algorithms[3]);
				distance = SCAN(queue_head, position, distance, file_fd);
				write_log(file_fd, choose, distance);
				break;
			case 4:
				fprintf(file_fd, ">>start schdeuler [%s]\n", algorithms[4]);
				distance = CSCAN(queue_head, position, distance, file_fd);
				write_log(file_fd, choose, distance);
				break;
			case 5:
				return 0;
			default:
				fprintf(file_fd, "input error, input again\n");
				printf("input error, input again\n");
				//getchar();
		}
	}
	
}

int main(void)
{
	char *logfile = "/home/chenxuqiang/Documents/learngit/schedulers/logfile";
	char *algorithms[4] = {"FCFS", "SSTF", "SCAN", "CSCAN"};

	struct request_queue *queue_head = init_rq();
	int i = 0;
	int position = 0;
	int distance = 0;
	FILE *file_fd;

	file_fd = fopen(logfile, "w+");
	if(file_fd == NULL)
	{
		perror("fopen error");
		exit(1);
	}
	fprintf(file_fd, ">>fill request_queue\n");
	srand((unsigned)time(NULL));
	for(i = 0; i < QUEUE_LEN; i++)
	{
		add_request(queue_head, create_request());
	}
	traverse_queue(queue_head);
	position = rand()%100 + 1;

	choose(algorithms, queue_head, distance, position, file_fd);

	destory_queue(queue_head);
	fclose(file_fd);
	return 0;
}