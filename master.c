#include "master.h"
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<memory.h>
#include<fcntl.h>
#include<errno.h>

int main(int argc, char **argv)
{
	int i;
	char query_word[32] = {0};
	char file_path[4096] = {0};
	char k[2] = {'2', '\0'};
	char director[10] = "./slave";
	int child_num = 2;
	for(i = 1; i < argc; ++i) { //argument
		if(strcmp(argv[i], "-q") == 0) {
			strncpy(query_word, argv[++i], sizeof(query_word)-1);
		} else if(strcmp(argv[i], "-d") == 0) {
			strncpy(file_path, argv[++i], sizeof(file_path)-1);
		} else if(strcmp(argv[i], "-s") == 0) {
			strncpy(k, argv[++i], sizeof(k)-1);
		}
	}

	child_num = atoi(k);

	//create and initialize the mail
	struct mail_t mail;
	struct mail_t mail_recv;

	for(i=0; i<32; i++)		mail.data.query_word[i] = '\0';
	for(i=0; i<4096; i++)	mail.file_path[i] = '\0';

	memcpy(mail.data.query_word, query_word, strlen(query_word));
	memcpy(mail.file_path, file_path, strlen(file_path));

	/* Start children. */
	pid_t pids[100];
	for (i = 0; i < child_num; ++i) {
		if ((pids[i] = fork()) < 0) {
			perror("fork");
			abort();
		} else if (pids[i] == 0) {
			if(execlp(director, "slave", query_word, file_path, (char*)0) == -1) {
				printf("Error: Unable to load the slave.\n");
				exit(EXIT_FAILURE);
			}
			//NEVER REACHED
		}
	}

	/* Wait for children to exit. */
	int status;
	pid_t pid;
	while (child_num > 0) {
		if(strlen(mail_recv.file_path) != 0 ) {
			break;
		}
		//send mail to mailbox
		int sysfs_fd = open("/sys/kernel/hw2/mailbox", O_RDWR, 0660);
		printf("Master: sysfs_fd: %d\n", sysfs_fd);
		//printf("Master: ERROR %d\n", errno);		//13: permission denied
		send_to_fd(sysfs_fd, &mail);

		//wait for mail
		pid = wait(&status);
		printf("Child with PID %ld exited with status 0x%x.\n", (long)pid, status);
		--child_num;

		//receive mail from mailbox
		receive_from_fd(sysfs_fd, &mail_recv);
	}

	//kill slaves
	for (i = 0; i < child_num; ++i) {
		printf("Kill: slave[%d]", pids[i]);
		kill(pids[i], SIGKILL);
	}
	//ref: https://stackoverflow.com/questions/876605/multiple-child-process
	return EXIT_SUCCESS;
}

int send_to_fd(int sysfs_fd, struct mail_t *mail)
{
	printf("size of mail: %ld\n", sizeof(*mail));
	printf("mail->data.query_word: %s\n", mail->data.query_word);
	printf("mail->file_path: %s\n", mail->file_path);

	int ret_val = write(sysfs_fd, mail, sizeof(*mail));
	lseek(sysfs_fd, 0, SEEK_SET);
	if (ret_val == ERR_FULL) {
		printf("Master: Mailbox is full!\n");
	} else {

	}

}

int receive_from_fd(int sysfs_fd, struct mail_t *mail)
{

	char buf[4160];
	char count[64];
	int ret_val = read(sysfs_fd, buf, sizeof(buf));
	lseek(sysfs_fd, 0, SEEK_SET);
	printf("strlen of buf: %ld\n", strlen(buf));
	if (ret_val == ERR_EMPTY) {
		printf("Master: Mailbox is empty...\n");
	} else {
		memcpy(count, buf, sizeof(count));
		memcpy(mail->file_path, buf+sizeof(count), sizeof(mail->file_path));
		mail->data.word_count = atoi(count);
		printf("Master receive: mail->data.word_count: %d\n", mail->data.word_count);
		printf("Master receive: mail->file_path: %s\n", mail->file_path);

	}
}
