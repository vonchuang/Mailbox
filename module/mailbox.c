#include "mailbox.h"

MODULE_LICENSE("Dual BSD/GPL");

static void get_process_name(char *ouput_name);
static ssize_t mailbox_read(struct kobject *kobj,
                            struct kobj_attribute *attr, char *buf);
static ssize_t mailbox_write(struct kobject *kobj,
                             struct kobj_attribute *attr, const char *buf, size_t count);

static struct kobject *hw2_kobject;
static struct kobj_attribute mailbox_attribute
    = __ATTR(mailbox, 0660, mailbox_read, mailbox_write);

static int num_entry_max = 2;
static int mail_count = 0;
struct mailbox_head_t mailbox_head;

module_param(num_entry_max, int, S_IRUGO);

static void get_process_name(char *ouput_name)
{
	memcpy(ouput_name, current->comm, sizeof(current->comm));
}
/*
static void print_List(struct list_head *head){
    struct list_head *listptr;
    struct mailbox_head_t *entry;

    printk("\n*********************************************************************************\n");
    printk("(HEAD addr =  %p, next = %p, prev = %p)\n",head, head->next, head->prev);
    list_for_each(listptr, head) {
        entry = list_entry(listptr, struct mailbox_head_t, head);
        printk("entry->buf= %s\n",
                entry->nuf);
    }
    printk("*********************************************************************************\n");
}
*/
//This function is called whenever the mailbox is being read from user space,
//    i.e. data is being sent from the mailbox to the user.
static ssize_t mailbox_read(struct kobject *kobj,
                            struct kobj_attribute *attr, char *buf)
{
	char process_name[100];
	struct mailbox_head_t *mailbox_read;
	char word[32];
	char file[4096];
	char word_count[64];

	//INIT_LIST_HEAD(&mailbox_read->head);
	mailbox_read = list_entry(mailbox_head.head.next, struct mailbox_head_t, head);
	printk("mailbox_read(sent from the mailbox to the user)!\n");
	//printk("Create mailbox_entry!\n");
	printk("from master: %d\n", mailbox_read->from_master);
	get_process_name(process_name);
	//if(strcmp(process_name, "master") == 0 && (mailbox_read->from_master == 0))
	if((strcmp(process_name, "master") == 0) && (mail_count > 0)) {
//====================== master read ============================
		//print_List(&mailbox_head.head);
		printk("process name: %s\n", process_name);
		//copy query_word to buf
		printk("Master Read: mail_head.buf: %s\n", mailbox_read->buf);
		memcpy(buf, &mailbox_read->buf, strlen(mailbox_read->buf));
		printk("Master Read: buf: %s\n", buf);
		memcpy(word_count, buf, sizeof(word_count));
		printk("Master Read: word_count: %s\n", word_count);
		memcpy(file, buf+sizeof(word_count), sizeof(file));
		printk("Master Read: file: %s\n", file);

		mail_count--;

		//del mailbox_entry from mail_head
		printk("list: %p, list->next: %p, list->prev: %p\n", &mailbox_head.head.next,
		       &mailbox_head.head.next->next, &mailbox_head.head.next->prev);
		//list_del(mailbox_head.head.next);

		return sizeof(struct mailbox_entry_t);
		//} else if((strcmp(process_name, "slave") == 0) && (mailbox_read->from_master == 1)) {
	} else if((strcmp(process_name, "slave") == 0) && (mail_count > 0)) {
//====================== slave read =============================
		//print_List(&mailbox_head.head);
		printk("process name: %s\n", process_name);
		//copy query_word to buf
		printk("Slave Read: mail_head.buf: %s\n", mailbox_read->buf);
		memcpy(buf, &mailbox_read->buf, strlen(mailbox_read->buf));
		printk("Slave Read: buf: %s\n", buf);
		memcpy(word, buf, sizeof(word));
		printk("Slave Read: word: %s\n", word);
		memcpy(file, buf+32, sizeof(file));
		printk("Slave Read: file: %s\n", file);

		mail_count--;

		//del mailbox_entry from mail_head
		printk("list: %p, list->next: %p, list->prev: %p\n", &mailbox_head.head.next,
		       &mailbox_head.head.next->next, &mailbox_head.head.next->prev);
		//list_del(mailbox_head.head.next);
		/*
		if(mailbox_read->head.next == NULL){
			mailbox_head.head.next = NULL;
			mailbox_read->head.prev = NULL;
		}else{
			mailbox_head.head.next->next->prev = &mailbox_head.head;
			mailbox_head.head.next = mailbox_head.head.next->next;
		}
		*/
		//list_add(&mailbox_entry.entry, &mailbox_head.head);
		return sizeof(struct mailbox_entry_t);
	} else if(mail_count <= 0) {
		return ERR_EMPTY;
	}
}

//This function is called whenever the mailbox is being written to from user space,
//    i.e. data is sent to the mailbox from the user.
static ssize_t mailbox_write(struct kobject *kobj,
                             struct kobj_attribute *attr, const char *buf, size_t count)
{
	char process_name[100];
	printk("mailbox_write(sent to the mailbox from the user)!\n");
	get_process_name(process_name);
	if((strcmp(process_name, "master") == 0) && (mail_count <= num_entry_max)) {
//====================== master write =========================
		struct mailbox_entry_t mailbox_entry;
		struct mailbox_head_t *mailbox_read;
		char word[32] = {0};
		char file[4096] = {0};
		int i;
		for(i = 0; i < 4129; i++)	mailbox_entry.buf[i] = '\0';
		printk("process name: %s\n", process_name);
		//copy buf to mailbox_entry
		//printk("Master write: buf: %s\n", buf);
		printk("Master write: strlen buf: %ld\n", strlen(buf));
		//for(i=0; i<sizeof(buf); i++)	printk("%c\n", buf[i]);
		memcpy(word, buf, sizeof(word)-1);
		printk("Master write: buf word: %s\n", word);
		memcpy(file, buf+32, sizeof(file)-1);
		printk("Master write: buf file: %s\n", file);
		memcpy(&mailbox_entry.buf, buf, sizeof(mailbox_entry.buf));
		printk("Master write: sizeof(mailbox_entry.buf)+1: %ld\n",
		       sizeof(mailbox_entry.buf));
		printk("Master write: mailbox_entry.buf: %s\n", mailbox_entry.buf);
		memcpy(word, mailbox_entry.buf, 32*sizeof(word));
		printk("Master write: word: %s\n", word);
		//memcpy(file, mailbox_entry.buf+32, 4096*sizeof(file));
		printk("Master write: file: %s\n", file);

		mailbox_entry.from_master = 1;
		mail_count++;

		//add mailbox_entry to mail_head
		list_add(&mailbox_entry.entry, &mailbox_head.head);
		//(V) check
		mailbox_read = list_entry(mailbox_head.head.next, struct mailbox_head_t, head);
		printk("Master write:head buf: %s\n", mailbox_read->buf);
//			print_List(&mailbox_head.head);
		return sizeof(struct mailbox_entry_t);

	} else if((strcmp(process_name, "slave" ) == 0)
	          && (mail_count <= num_entry_max)) {
//========================= slave write ==============================
		struct mailbox_entry_t mailbox_entry;
		struct mailbox_head_t *mailbox_read;
		char word[64] = {0};
		char file[4096] = {0};
		int i;
		for(i = 0; i < 4129; i++)	mailbox_entry.buf[i] = '\0';
		printk("process name: %s\n", process_name);
		//copy buf to mailbox_entry
		//printk("Master write: buf: %s\n", buf);
		printk("Slave write: strlen buf: %ld\n", strlen(buf));
		//for(i=0; i<sizeof(buf); i++)	printk("%c\n", buf[i]);
		memcpy(word, buf, sizeof(word)-1);
		printk("Slave write: buf count: %s\n", word);
		memcpy(file, buf+32, sizeof(file)-1);
		printk("Slave write: buf file: %s\n", file);
		memcpy(&mailbox_entry.buf, buf, sizeof(mailbox_entry.buf));
		printk("Slave write: sizeof(mailbox_entry.buf)+1: %ld\n",
		       sizeof(mailbox_entry.buf));
		printk("Slave write: mailbox_entry.buf: %s\n", mailbox_entry.buf);
		memcpy(word, mailbox_entry.buf, 32*sizeof(word));
		printk("Slave write: count: %s\n", word);
		//memcpy(file, mailbox_entry.buf+32, 4096*sizeof(file));
		printk("Slave write: file: %s\n", file);

		mailbox_entry.from_master = 1;
		mail_count++;

		//add mailbox_entry to mail_head
		list_add(&mailbox_entry.entry, &mailbox_head.head);
		//(V) check
		mailbox_read = list_entry(mailbox_head.head.next, struct mailbox_head_t, head);
		printk("Master write:head buf: %s\n", mailbox_read->buf);
		//			print_List(&mailbox_head.head);
		return sizeof(struct mailbox_entry_t);



		/*
				struct mailbox_entry_t mailbox_entry;
				struct mailbox_head_t *mailbox_read;

				//INIT_LIST_HEAD(&mailbox_entry.entry);
				printk("process name: %s\n", process_name);
				//copy buf to mailbox_entry
				printk("Slave write: buf: %s\n", buf);
				memcpy(mailbox_entry.buf, buf, sizeof(mailbox_entry.buf));
				printk("Slave write: mailbox_entry.buf: %s\n", mailbox_entry.buf);

				//check(send from slave)
				mailbox_entry.from_master = 0;
				mail_count++;

				//add mailbox_entry to mail_head
				list_add(&mailbox_entry.entry, &mailbox_head.head);
				/*
				if(mailbox_head.head.next == NULL){
					mailbox_head.head.next = &mailbox_entry.entry;
					mailbox_entry.entry.prev = &mailbox_head.head;
				}else{
					mailbox_head.head.next->prev = &mailbox_entry.entry;
					mailbox_entry.entry.next = mailbox_head.head.next;
					mailbox_head.head.next = &mailbox_entry.entry;
					mailbox_entry.entry.prev = &mailbox_head.head;
				}

				//(V) check
				mailbox_read = list_entry(mailbox_head.head.next, struct mailbox_head_t, head);
				printk("Slave write:head buf: %s\n", mailbox_read->buf);
		//			print_List(&mailbox_head.head);
				return sizeof(struct mailbox_entry_t);
		*/
	} else if(mail_count > num_entry_max) {
		return ERR_FULL;
	}
}

static int __init mailbox_init(void)
{
	printk("=======================================================\n");
	printk("mailbox init\n");
	printk("num_entry_max: %d\n", num_entry_max);
	hw2_kobject = kobject_create_and_add("hw2", kernel_kobj);
	sysfs_create_file(hw2_kobject, &mailbox_attribute.attr);


	INIT_LIST_HEAD(&mailbox_head.head);

	return 0;
}

static void __exit mailbox_exit(void)
{
	printk("Remove\n");
	kobject_put(hw2_kobject);
}

module_init(mailbox_init);
module_exit(mailbox_exit);

