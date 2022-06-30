#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>

#define FILE_S2C	"./file_s2c"
#define FILE_C2S	"./file_c2s"
#define MAPPED_SIZE 64
#define PHYS_MEM 0

const char* send_controller();
void recv_controller();
void print_msg();
void send();
const char* recv();
void opmem();
void opsem();
void del_all();

int end_state = 0;
char *mem_s2c = NULL, *mem_c2s = NULL;
char pid[10] = "";
sem_t *sem_s2c, *sem_c2s, *sem_ready, *sem_wclient;

int main() {
	int ready_num, sem_status;
	char msg[MAPPED_SIZE] = "";
	sprintf(pid, "%u", (unsigned int)getpid());

	//open memory map
	opmem();
	//open semaphores
	opsem();

	//game start
	sem_wait(sem_ready);
	do {
		sem_getvalue(sem_ready, &(ready_num));
	} while(ready_num > 0);
	
	printf("======================\n");
	printf("     Hello %s\n", pid);
	printf("     Game Start!!\n");
	printf("======================\n");

	/*sequence here*/
	if(sem_trywait(sem_wclient) < 0) {
		sem_wait(sem_s2c);
		strcpy(msg, recv());
		recv_controller(msg);
		sem_wait(sem_wclient);
	}
	while(end_state == 0) {
		strcpy(msg, send_controller());
		send(msg);
		sem_post(sem_c2s);
		sem_wait(sem_s2c);
		strcpy(msg, recv());
		recv_controller(msg);
		sem_post(sem_wclient);
		if(end_state != 0) {
			break;
		}
		do {
			sem_getvalue(sem_wclient, &(sem_status));
		} while(sem_status > 0);
		sem_wait(sem_s2c);
		strcpy(msg, recv());
		recv_controller(msg);
		sem_wait(sem_wclient);
	}
	
	//end msg
	if(end_state == -1) {
		printf("You Lose\n");
	} else if(end_state == 1) {
		printf("You Win\n");
	} else {
		perror("finished fail");
	}
	sem_post(sem_ready);

	del_all();
	return 0;
}

const char* send_controller() {
	int input_num;
	char c_num[5] = "";
	static char buf[MAPPED_SIZE] = "";

	//get num
	do {
		printf("input num(100-999) : ");
		scanf("%d", &input_num);
		getchar();
	} while((input_num < 100) || (input_num > 999));
	sprintf(c_num, "%d", input_num);

	//create send msg
	strcpy(buf, pid);
	strcat(buf, "=");
	strcat(buf, c_num);

	return buf;
}

void recv_controller(char *msg) {
	int i;
	int pid_num_boundary = 0, num_strike_boundary = 0, strike_ball_boundary;
	int msg_len = strlen(msg);
	char c_pid[10] = "";
	char c_num[5] = "";
	char c_strike[3] = "";
	char c_ball[3] = "";

	//parse msg
	for(i = 0; i < msg_len; i++) {
		pid_num_boundary = msg[i] == '=' ? i : pid_num_boundary;
		num_strike_boundary = msg[i] == ':' ? i : num_strike_boundary;
		strike_ball_boundary = msg[i] == '&' ? i : strike_ball_boundary;
	}

	//get pid
	for(i = 0; i < pid_num_boundary; i++) {
		c_pid[i] = msg[i];
	}

	//get num
	for(i = pid_num_boundary + 1; i < num_strike_boundary; i++) {
		c_num[i - pid_num_boundary - 1] = msg[i];
	}
	
	//get strike
	for(i = num_strike_boundary + 1; i < strike_ball_boundary; i++) {
		c_strike[i - num_strike_boundary - 1] = msg[i];
	}

	//get ball
	for(i = strike_ball_boundary + 1; i < msg_len; i++) {
		c_ball[i - strike_ball_boundary - 1] = msg[i];
	}

	//print msg
	print_msg(c_pid, c_num, c_strike, c_ball);

	//end check
	if(atoi(c_strike) == 3) {
		if(strcmp(c_pid, pid) == 0) {
			end_state = 1;
		} else {
			end_state = -1;
		}
	}

}

void print_msg(char *pid, char *num, char *strike, char *ball) {
	printf("[%s] %s : %s strike, %s ball\n", pid, num, strike, ball);
}

void send(char *msg) {
	strcpy(mem_c2s, msg);
	msync(mem_c2s, MAPPED_SIZE, MS_SYNC);
}

const char* recv() {
	static char recv_msg[MAPPED_SIZE] = "";
	msync(mem_s2c, MAPPED_SIZE, MS_SYNC);
	strcpy(recv_msg, mem_s2c);
	return recv_msg;
}

void opmem() {
	//open mamory map
	int f_s2c, f_c2s;

	//open server to client
	if((f_s2c = open(FILE_S2C, O_RDWR|O_SYNC)) < 0) {
		perror("open fail");
		exit(1);
	}
	if((mem_s2c = (char *)(mmap(0, MAPPED_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, f_s2c, PHYS_MEM))) == MAP_FAILED) {
		perror("map fail");
		exit(1);
	}
	close(f_s2c);

	//open client to server
	if((f_c2s = open(FILE_C2S, O_RDWR|O_SYNC)) < 0) {
		perror("open fail");
		exit(1);
	}
	if((mem_c2s = (char *)(mmap(0, MAPPED_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, f_c2s, PHYS_MEM))) == MAP_FAILED) {
		perror("map fail");
		exit(1);
	}
	close(f_c2s);
}

void opsem() {
	int i, sem_status;

	//open semaphore - server to client
	if((sem_s2c = sem_open("sem_s2c", O_CREAT, 0644, 0)) == SEM_FAILED) {
		exit(1);
	}

	//open semaphore - client to server
	if((sem_c2s = sem_open("sem_c2s", O_CREAT, 0644, 0)) == SEM_FAILED) {
		exit(1);
	}

	//open semaphore - client ready
	if((sem_ready = sem_open("sem_ready", O_CREAT, 0644, 2)) == SEM_FAILED) {
		exit(1);
	}

	//make semaphore - communication of clients
	if((sem_wclient = sem_open("sem_wclient", O_CREAT, 0644, 1)) == SEM_FAILED) {
		exit(1);
	}
	sem_getvalue(sem_wclient, &(sem_status));
	if(sem_status > 1) {
		for(i = 0; i < sem_status - 1; i++) {
			sem_wait(sem_wclient);
		}
	} else if(sem_status < 1) {
		for(i = 0; i < 1 - sem_status; i++) {
			sem_post(sem_wclient);
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
	sem_unlink("sem_wclient");

}