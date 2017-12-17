#include "slave.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>

int main(int argc, char **argv)
{
	struct mail_t mail;
	int sysfs_fd = open("/sys/kernel/hw2/mailbox", O_RDWR, 0660);
	printf("=========== slave =================\n");
	printf("Slave: sysfs_fd: %d\n", sysfs_fd);
	printf("strlen mail file path: %ld\n", strlen(mail.file_path));
	//receive from mailbox
	//while(sizeof(mail.))
	do {
		receive_from_fd(sysfs_fd, &mail);
		printf("strlen mail file path: %ld\n", strlen(mail.file_path));
		//} while(strlen(mail.file_path) == 0 || strlen(mail.data.query_word) == 0);
	} while(strlen(mail.data.query_word) == 0);
	printf("Slave: sysfs_fd: %d\n", sysfs_fd);

	printf("slave: mail query word: %s\n", mail.data.query_word);
	printf("slave: mail file path: %s\n", mail.file_path);

	//count number of the specific word int the text file.
	FILE *fp = freopen(argv[2], "r", stdin);
	if(fp == NULL) {
		printf("Error: slave can not open the file.\n");
		exit(EXIT_FAILURE);
	}

	char str[1000]; //read words from file.
	unsigned int count = 0;  //count the number of the word.
	char *token;
	while(fgets(str, sizeof(str), fp)) {
		for(token = strtok(str, " \"().\n\t") ; token != NULL ;
		    token = strtok(NULL, " \"().\n\t")) {
			//			printf("%s\n", token);
			if(strcmp(token, argv[1]) == 0)
				count++;

		}
	}

	mail.data.word_count = count;
	printf("slave:count:%d\n", count);

	//send to mailbox
	send_to_fd(sysfs_fd, &mail);

}

int send_to_fd(int sysfs_fd, struct mail_t *mail)
{
	printf("size of mail: %ld\n", sizeof(*mail));
	printf("Slave send: mail->data.query_word: %d\n", mail->data.word_count);
	printf("Slave send: mail->file_path: %s\n", mail->file_path);

	int ret_val = write(sysfs_fd, mail, sizeof(*mail));
	lseek(sysfs_fd, 0, SEEK_SET);
	if (ret_val == ERR_FULL) {
		printf("Slave: Mailbox is full!\n");
	} else {

	}

}

int receive_from_fd(int sysfs_fd, struct mail_t *mail)
{
	char buf[4160];
	int ret_val = read(sysfs_fd, buf, sizeof(buf));
	lseek(sysfs_fd, 0, SEEK_SET);
	printf("strlen of buf: %ld\n", strlen(buf));
	if (ret_val == ERR_EMPTY) {
		printf("Slave: Mailbox is empty...\n");
	} else {
		memcpy(mail->data.query_word, buf, sizeof(mail->data.query_word));
		memcpy(mail->file_path, buf+32, sizeof(mail->file_path));
		printf("Slave receive: mail->data.query_word: %s\n", mail->data.query_word);
		printf("Slave receive: mail->file_path: %s\n", mail->file_path);

	}

}
