#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "../libarg.h"

struct opt {
	char *cname;
	char *lname;
	char *sname;
	uint8_t id;
	
	bool is_act : 1;
};

struct cmd {
	char       *name;
	uint8_t     id;

	struct opt *optr;
	uint8_t     optc;
	uint8_t	    optac;
};

struct runtime {
	int          argc;
	char       **argv;

	struct cmd  *cmdr;	// register cmd
	uint8_t      cmda;	// cmd active
	uint8_t      cmdac;
	uint8_t      cmdc;	// cmd count
};

static struct runtime rt;

static void        init(int argc, char **argv);
static int         identify(char *str);
static bool        parse(void);
static void        error(const uint8_t err_code, const char *msg);
static struct cmd *findc(char *name);
static struct opt *findo(char *cmd, char *opt);
static void        view(void);

static struct cmd *findc(char *name) {
	for (int i = 0; i < rt.cmdc + 1; i++) {
		if (strcmp(rt.cmdr[i].name, name) == 0)
			return &rt.cmdr[i];
	}

	return NULL;
};

static struct opt *findo(char *cmd, char *opt)
{
	struct cmd *tmp = findc(cmd);
	if (tmp == NULL)
		goto not_found;
	
	int i = 0;
	for (; i < tmp->optc; i++) {
		if ((strcmp(tmp->optr[i].lname, opt) == 0) ||
			(strcmp(tmp->optr[i].sname, opt) == 0))
			goto found;
	}

	goto not_found;

found:
	return &tmp->optr[i];
not_found:
	return NULL;
}

bool arge(char *cmd, char *opt)
{
	if (cmd == NULL)
		return false;

	if (opt == NULL)
		return (findc(cmd) == NULL);
	else
		return (findo(cmd, opt) == NULL);

	return false;
}

static void init(int argc, char **argv)
{
	rt.argc  = argc;
	rt.argv  = argv;
	rt.cmdc  = 0;
	rt.cmdac = 0;
	rt.cmdr  = (struct cmd *)malloc(sizeof(struct cmd));

	// Set default command
	struct cmd *new = (struct cmd *)malloc(sizeof(struct cmd));
	new->name  = "default";
	new->id    = 0;
	new->optc  = 0;
	new->optac = 0;
	new->optr  = NULL;
	rt.cmdr[0] = *new;
	rt.cmda    = 0;
}


void argc(char *name)
{
	struct cmd *new = (struct cmd *)malloc(sizeof(struct cmd));
	new->name  = name;
	new->id    = ++rt.cmdc;
	new->optc  = 0;
	new->optac = 0;
	new->optr  = NULL;
	rt.cmdr    = (struct cmd *)realloc(rt.cmdr, (rt.cmdc + 1) * sizeof(struct cmd));
	rt.cmdr[rt.cmdc] = *new;

	free(new);
}

void argo(char *cmd, char *sname, char *lname)
{
	struct cmd *tmp = findc(cmd);
	if (tmp == NULL)
		goto err;
	
	struct opt *new = (struct opt *)malloc(sizeof(struct opt));
	new->cname  = cmd;
	new->sname  = sname;
	new->lname  = lname;
	new->id     = tmp->optc++;
	new->is_act = false;
	tmp->optr   = (struct opt *)realloc(tmp->optr ,(tmp->optc) * sizeof(struct opt));
	tmp->optr[new->id] = *new;

	return;
err:
	error(222, "Command not found.");
}


static void error(const uint8_t err_code, const char *msg)
{
	printf("libarg - error %d - %s\n", err_code, msg);
}

static int identify(char *str)
{
	if (str == NULL)
		goto err;

	if (str[0] == '-') {
		if (str[1] == '-')
			return 3;
		else
			return 2;
	} else {
		struct cmd *tmp = findc(str);
		if (tmp != NULL)
			return 1;
		else
			return 0;
	}

err:
	error(1, "unkown");
	return -1;
}

static bool parse(void)
{
	for (int i = 1; i < rt.argc; i++) {
		char *cur  = rt.argv[i];
		int   type = identify(cur);
		int   len  = strlen(cur);
		if (type == 1) {
			struct cmd *cur_cmd = findc(cur);
			if (rt.cmda == 0)
				rt.cmda = cur_cmd->id;
			else
				goto err;
		}

		if (type == 2) {
			for (int j = 1; j < len; j++) {
				char tmpc[2];
				tmpc[0] = cur[j];
				tmpc[1] = '\0';
				struct opt *cur_opt = findo(rt.cmdr[rt.cmda].name, tmpc);
				if (cur_opt != NULL) {
					cur_opt->is_act = true;
					rt.cmdr[rt.cmda].optac++;
				} else {
					goto err;
				}
			}
		}

		if (type == 3) {
			struct opt *cur_opt = findo(rt.cmdr[rt.cmda].name, &cur[2]);
			if (cur_opt != NULL) {
				cur_opt->is_act = true;
				rt.cmdr[rt.cmda].optac++;
			} else {
				goto err;
			}
		}
	}

	return true;

err:
	error(2, "parse error");
	return false;
}

int argi(int argc, char **argv, void (*reg)(void), void (*guide)(void))
{
	if (argc < 2)
		return -1;

	init(argc, argv);
	reg();
	parse();
	guide();
	return 0;
}


static void view(void)
{
	int cmdc = rt.cmdc;
	printf(".\n");
	for (int i = 0; i < cmdc + 1; i++) {
		struct cmd *ccmd = &rt.cmdr[i];
		int optc = ccmd->optc;
		printf("\n> %s (id = %d, optc = %d, optac = %d)\n",
			ccmd->name, ccmd->id, ccmd->optc, ccmd->optac);
		
		for (int j = 0; j < optc; j++) {
			struct opt *copt = &ccmd->optr[j];
			printf("        > %s(%s) (id = %d, is_act = %d)\n", copt->lname, copt->sname, copt->id, copt->is_act);
		}
	}
};

