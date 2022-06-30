#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
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
	char pingstr[] = "ping\n";
	sem_t* sem;

	mkfifo(FIFO_FILE, 0666);
	fd = open(FIFO_FILE, O_RDWR);

	sem_unlink("smphr");
	if((sem = sem_open("smphr", O_CREAT, 0644, 1)) == SEM_FAILED) {
		exit(1);
	}

	for (cnt=0; cnt<5; cnt++)
	{
		sem_wait(sem);
		memset(buf, 0x00, BUF_SIZE);

		//recv
		if(cnt > 0) {
			read(fd, buf, BUF_SIZE);
			printf("[opponent] %s", buf);	
		}
		//send
		printf("Your turn!\n");
		fgets(buf, BUF_SIZE, stdin);
		write(fd, buf, strlen(buf));

		//cmpr
		if (strcmp(buf, pingstr))
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
