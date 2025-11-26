/*
 *                           Maintainer 2025 Guaitan1st <guaitanth1st@gmail.com>
 *                                               <https://github.com/guaitan1st>
 *
 *                             Copyright (C) Guaitan1st <guaitanth1st@gmail.com>
 *
 *                                                           Version Alpha 0.0.1
 */
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct usr {
	uint16_t uid;
	char *name;
	char *passwd;

	union {
		uint8_t data;
		struct {
			bool    is_delete :1;
			uint8_t reserved  :7;
		};
	} flg;
};

struct cmt {
	uint16_t cid;
	char *content;
	
	uint16_t time_local[8];
	uint16_t time_utc[8];
};

struct tsk {
	union {
		uint16_t data[3];
		
		struct {
			uint16_t uid;
			uint16_t wid;
			uint16_t id;
		};
	} tid;

	union {
		uint8_t data;

		struct {
			bool     is_done    :1;
			bool     is_giveup  :1;
			uint8_t  reserved   :6;
		};
	} flg;

	struct {
		char *content;
		struct cmt *cmts;
		uint16_t    cmts_cnt;
	
		uint16_t time_local[8];
		uint16_t time_utc0[8];
	};
};

struct wek {
	uint64_t ver;
	uint16_t wid;
	
	struct tsk **tsks;
	uint16_t     tsks_cnt;
};

struct runtime {
	uint16_t time_local[8];
	uint16_t time_utc0[8];

	struct usr **usrs;
	struct usr  *usr_cur;
	uint16_t     usrs_cnt;

	struct wek **weks_load;
	struct wek  *wek_cur;
};

struct runtime rt;

struct tsk *tsk_fnd(uint16_t tid[3]);
int         tsk_add(char *content);
int         tsk_set_flg(unsigned int type, uint16_t tid[3]);
int         tsk_del(uint16_t tid[3]); 
int	    tsk_edt(uint16_t tid[3]);

int usr_add(char *name, char *passwd);
int usr_del(char *name);

int cmt_add(char *content);

void init(void);
void load(void);
void parse(void);
void save(void);
void quit(void);
int  login(char *name, char *passwd);

/* 

init() -> login() -> parse() -> ... ->dpy_tsk() ->quit() 

*/

void init(void)
{
	printf("Welcom to mow, a friendly to-do manager.\n");
	printf("Type help for instructions on how to use mow :)\n");

	// initiate rt user
	rt.usr_cur = (struct usr *)malloc(sizeof(struct usr));
	rt.usr_cur->name   = NULL;
	rt.usr_cur->passwd = NULL;
	rt.usrs            = NULL;
	rt.usrs_cnt        = 0;
}

int usr_add (char *name, char *passwd)
{
	// check if the name is token 
	for (int i = 0; i < rt.usrs_cnt; i++) {
		if (strcmp(name, rt.usrs[i]->name) == 0) {
			printf("Oops,the name has already been used.\n");
			return 1;
		}
	}
	
	// realloc rt.usrs
	int cnt = ++rt.usrs_cnt;
	struct usr **tmp;
	tmp = (struct usr **)realloc(rt.usrs, cnt * sizeof(struct usr *));
	if (tmp == NULL) {
		printf("Oops!Failed to add a user(realloc problem)!");
		return 2;
	}else {
		rt.usrs = tmp;
		rt.usrs[cnt-1] = (struct usr *)malloc(sizeof(struct usr));
	}
	
	// write user inform
	struct usr* user = rt.usrs[cnt-1];
	int name_len     = strlen(name) + 1;
	int passwd_len   = strlen(passwd) + 1;

	user->name   = (char *)malloc(name_len * sizeof(char));
	user->passwd = (char *)malloc(passwd_len * sizeof(char));
	strcpy(user->name, name);
	strcpy(user->passwd, passwd);

	user->flg.is_delete = false;

	return 0;
}


int login(char *name, char *passwd)
{
	int   cnt    = rt.usrs_cnt;

	for (int i = 0; i < cnt; i++) {
		if (strcmp(name, rt.usrs[i]->name) == 0){
			if(strcmp(passwd, rt.usrs[i]->passwd) == 0) {
				int name_len   = strlen(name) + 1;
				int passwd_len = strlen(passwd) + 1;
				rt.usr_cur->name   = (char *)malloc
						     (sizeof(char)*name_len);
				rt.usr_cur->passwd = (char *)malloc
						     (sizeof(char)*passwd_len);
			 	strcpy(rt.usr_cur->name, name);
				strcpy(rt.usr_cur->passwd, passwd);
				printf("Welcome to mow, %s", rt.usr_cur->name);
				return 0;
			} else {
				printf("Opps,wrong password!");
				return 1;
			}
		} 
	}
	printf("Opps,user <%s> not found!", name);
	return 2;
}

int tsk_add()




int main(void)
{	
	init();
	char *name   = "guaitan1st";
	char *passwd = "20040305"; 
	usr_add(name, passwd);
	usr_add(name, passwd);
	login(name, passwd);
	return 0;
}
