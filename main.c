#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <unistd.h>
#include <time.h>
#include <mqueue.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#define PRODUCER_Q_NAME "/producer_mq_"
#define CONFIG_FILE_NAME "configuration"
#define PRODUCER "PRODUCER"
#define CO_EDITOR "Co-Editor"
#define S_Queue "/S_mq"
#define N_Queue "/N_mq"
#define W_Queue "/W_mq"
#define MQ_MAX_MSG_SIZE    	 		 100
enum type
{
   SPORTS = 0,
   NEWS ,
   WEATHER 
};

char* typeStr[] = {"SPORTS", "NEWS", "WEATHER"};

struct ProducerData
{
	char *id;
	int prodAmount;
	int queueSize;
} PD; 
void* producer(void* a);

int main()
{
	srand(time(0));// for random
	FILE* config;
	config = fopen (CONFIG_FILE_NAME, "r");
	char who[50];
	char producer_id[10];
	char numOfProduct[10];
	char qSize[10];
	char emptyLine[20];
	//int sizeArray = 2, capacity = 0;
	//struct ProducerData** producers = malloc(sizeof(struct ProducerData*) * 2); //nned to change to dynamic array
	struct ProducerData* producers[100];
	int ret, i=0;
	
	while (fgetc(config) != EOF) 
	{
		fseek(config, -1, SEEK_CUR);
		fscanf(config, "%s", who);
		if(!strcmp(who,PRODUCER))
		{
			struct ProducerData *prodData = malloc(sizeof(struct ProducerData));
			fscanf(config, "%s", producer_id);
			prodData->id = malloc(strlen(producer_id)+1);
			strcpy(prodData->id,producer_id);
			fscanf(config, "%s", numOfProduct);
			prodData->prodAmount = atoi(numOfProduct);
			fscanf(config, "%s", emptyLine);
			fscanf(config, "%s", emptyLine);
			fscanf(config, "%s", emptyLine);
			fscanf(config, "%s", qSize);
			prodData->queueSize = atoi(qSize);
			producers[i] = prodData;
			i++;
			//create thread

		}
		else if (!strcmp(who,CO_EDITOR))
		{
			fscanf(config, "%s", emptyLine);
			fscanf(config, "%s", emptyLine);
			fscanf(config, "%s", emptyLine);
			fscanf(config, "%s", qSize);
		}

	}
	fclose(config);
	
	pthread_t *initializer_threads = (pthread_t *)malloc((sizeof(pthread_t) * (i + 1)));
	int j = 0;
	for(j = 0; j < i; j++)
	{
		pthread_create( &initializer_threads[j], NULL, producer, producers[j]);
	}
	sleep(100);

	return (0);
}

/*
	Producer thread
*/
void* producer(void* a){
	struct ProducerData* data = (struct ProducerData*)a;
	/* initialize the queue attributes */
	struct mq_attr attr = {0};
	mqd_t mq;
    attr.mq_maxmsg = data->queueSize;
    attr.mq_msgsize = MQ_MAX_MSG_SIZE;
	char* queueName = malloc(strlen(PRODUCER_Q_NAME) + strlen(data->id) + 1);
	strcpy(queueName, PRODUCER_Q_NAME);
    strcat(queueName, data->id);
	queueName[strlen(queueName)-1] -= '\0';

    mq_unlink(queueName); // delete first if already exists, this requires sudo privilege

    mq = mq_open(queueName, O_RDONLY | O_NONBLOCK | O_CREAT, S_IRWXG | S_IRWXU, &attr);
	int randomType = rand() % 3;
	printf("the thread start: %s\n", data->id);
	printf("the selected type is: %s\n", typeStr[randomType]);
	printf("mq is: %s\n", queueName);

}

	/*void addToArray(int *size, int *capacity, struct ProducerData* node, struct ProducerData **producers)
	{
		capacity++;
		if(capacity > size)
		{
			*size *= 2;
		}
		producers

	}*/
