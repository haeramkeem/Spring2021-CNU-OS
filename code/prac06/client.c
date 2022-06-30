#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>

#define FIFO_FILE	"./fifo_temp"
#define BUF_SIZE	100

int main(void)
{
	int cnt = 0;
	int fd;
	int score = 100;
	char buf[BUF_SIZE];
	char pongstr[] = "pong\n";
	sem_t* sem;

	if((sem = sem_open("smphr", O_CREAT, 0644, 1)) == SEM_FAILED) {
		exit(1);
	}

	fd = open(FIFO_FILE, O_RDWR);

	for (cnt=0; cnt<5; cnt++)
	{
		sem_wait(sem);
		memset(buf, 0x00, BUF_SIZE);

		//recv
		read(fd, buf, BUF_SIZE);
		printf("[opponent] %s", buf);
		
		//send
		printf("Your turn!\n");
		fgets(buf, BUF_SIZE, stdin);
		write(fd, buf, strlen(buf));

		//cmpr
		if (strcmp(buf, pongstr))
		{
			printf("wrong! -20\n");
			score -= 20;
		}
		sem_post(sem);
		sleep(1);
	}

	printf("Done! Your score : %d\n", score);

	close (fd);
	sem_close("smphr");

	return 0;
}
