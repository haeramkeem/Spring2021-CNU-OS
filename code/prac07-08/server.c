#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>

#define FILE_S2C	"./file_s2c"
#define FILE_C2S	"./file_c2s"
#define MAPPED_SIZE 64
#define PHYS_MEM 0

const char* controller();
void print_msg();
void send();
const char* recv();
void mkmem();
void mksem();
void del_all();

int state = 0;
char *mem_s2c = NULL, *mem_c2s = NULL;
char w_pid[10];
char r_num[5] = "";
sem_t *sem_s2c, *sem_c2s, *sem_ready;

int main() {
	int sem_status;
	int i;
	int rand_num;
	int ready_num = 2, ready_num_temp = 2;
	char inbuf[MAPPED_SIZE] = "";
	char outbuf[MAPPED_SIZE] = "";

	//make memory map
	mkmem();
	//make semaphores
	mksem();

	//create random number
	srand(time(0));
	while((rand_num = rand() % 1000) < 100) {}
	printf("rand num : %d\n", rand_num);
	sprintf(r_num, "%d", rand_num);

	//waiting for clients
	do {
		sem_getvalue(sem_ready, &(ready_num_temp));
		for(i = 0; i < ready_num - ready_num_temp; i++) {
			printf("client hi\n");
		}
		ready_num = ready_num_temp;
	} while(ready_num > 0);

	//game start
	printf("========================\n");
	printf("       game start!!     \n");
	printf("========================\n");

	/*sequence here*/
	while(state == 0) {
		sem_wait(sem_c2s);
		strcpy(inbuf, recv(mem_c2s));
		strcpy(outbuf, controller(inbuf));
		send(outbuf);
		sem_post(sem_s2c);
		sem_post(sem_s2c);
	}
	printf("%s win!\n", w_pid);

	//delete memories & semaphores
	del_all();
}

const char* controller(char *msg) {
	int i, j;
	int s = 0, b = 0, splt = 0;
	int msg_len = (int)strlen(msg);
	char c_pid[10] = "";
	char c_num[5] = "";
	char c_strike[3] = "";
	char c_ball[3] = "";
	static char buf[MAPPED_SIZE] = "";

	//find '='
	for(i = 0; i < msg_len; i++) {
		splt = msg[i] == '=' ? i : splt;
	}

	//get pid
	for(i = 0; i < splt; i++) {
		c_pid[i] = msg[i];
	}

	//get num
	for(i = splt + 1; i < msg_len; i++) {
		c_num[i - splt - 1] = msg[i];
	}

	//strike or ball
	for(i = 0; i < 3; i++) {
		for(j = 0; j < 3; j++) {
			if(c_num[i] == r_num[j]) {
				if(i == j) {
					s++;
				} else {
					b++;
				}
			}
		}
	}
	sprintf(c_strike, "%d", s);
	sprintf(c_ball, "%d", b);

	//print recv'd
	print_msg(c_pid, c_num, c_strike, c_ball);

	//create msg
	strcpy(buf, c_pid);
	strcat(buf, "=");
	strcat(buf, c_num);
	strcat(buf, ":");
	strcat(buf, c_strike);
	strcat(buf, "&");
	strcat(buf, c_ball);

	//end check
	if(atoi(c_strike) == 3) {
		state = 1;
		strcpy(w_pid, c_pid);
	}

	return buf;
}

void print_msg(char *pid, char *num, char *strike, char *ball) {
	printf("[%s] %s : %s strike, %s ball\n", pid, num, strike, ball);
}

void send(char *msg) {
	strcpy(mem_s2c, msg);
	msync(mem_s2c, MAPPED_SIZE, MS_SYNC);
}

const char* recv() {
	static char recv_msg[MAPPED_SIZE] = "";
	msync(mem_c2s, MAPPED_SIZE, MS_SYNC);
	strcpy(recv_msg, mem_c2s);
	return recv_msg;
}

void mkmem() {
	//make memory map
	int f_s2c, f_c2s;
	
	//memory for server to client
	if((f_s2c = open(FILE_S2C, O_RDWR | O_SYNC | O_CREAT | O_TRUNC, 0666)) < 0) {
		perror("open s2c fail");
		exit(1);
	}
	if((ftruncate(f_s2c, (off_t)MAPPED_SIZE)) == -1) {
		perror("resize s2c fail");
		exit(1);
	}
	if((mem_s2c = (char *)(mmap(0, MAPPED_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, f_s2c, PHYS_MEM))) == MAP_FAILED) {
		perror("map fail");
		exit(1);
	}
	close(f_s2c);

	//memory for client to serverfcatecdl 
	if((f_c2s = open(FILE_C2S, O_RDWR | O_SYNC | O_CREAT | O_TRUNC, 0666)) < 0) {
		perror("open c2s fail");
		exit(1);
	}
	if((ftruncate(f_c2s, (off_t)MAPPED_SIZE)) == -1) {
		perror("resize c2s fail");
		exit(1);
	}
	if((mem_c2s = (char *)(mmap(0, MAPPED_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, f_c2s, PHYS_MEM))) == MAP_FAILED) {
		perror("map fail");
		exit(1);
	}
	close(f_c2s);
}

void mksem() {
	int i, sem_status;

	//semaphore - server to client
	sem_unlink("sem_s2c");
	if((sem_s2c = sem_open("sem_s2c", O_CREAT, 0777, 0)) == SEM_FAILED) {
		perror("open sem_s2c fail");
		exit(1);
	}
	sem_getvalue(sem_s2c, &(sem_status));
	for(i = 0; i < sem_status; i++) {
		sem_wait(sem_s2c);
	}

	//semaphore - client to server
	sem_unlink("sem_c2s");
	if((sem_c2s = sem_open("sem_c2s", O_CREAT, 0777, 0)) == SEM_FAILED) {
		perror("open sem_s2c fail");
		exit(1);
	}
	sem_getvalue(sem_c2s, &(sem_status));
	for(i = 0; i < sem_status; i++) {
		sem_wait(sem_c2s);
	}

	//semaphore - client ready
	sem_unlink("sem_ready");
	if((sem_ready = sem_open("sem_ready", O_CREAT | O_EXCL, 0777, 2)) == SEM_FAILED) {
		perror("open sem_ready fail");
		exit(1);
	}
	sem_getvalue(sem_ready, &(sem_status));
	if(sem_status > 2) {
		for(i = 0; i < sem_status - 2; i++) {
			sem_wait(sem_ready);
		}
	} else if(sem_status < 2) {
		for(i = 0; i < 2 - sem_status; i++) {
			sem_post(sem_ready);
		}
	}
}

void del_all() {
	//del memories
	munmap(mem_s2c, MAPPED_SIZE);
	munmap(mem_c2s, MAPPED_SIZE);

	//del semaphores
	sem_unlink("sem_s2c");
	sem_unlink("sem_c2s");
	sem_unlink("sem_ready");
}