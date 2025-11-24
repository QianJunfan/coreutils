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
	
	bool is_act: 1;
};

struct cmd {
	char       *name;
	uint8_t     id;

	struct opt *optr;
	uint8_t     optc;
	uint8_t	    optac;
};

struct runtime {
	char        *program_name;
	int          argc;
	char       **argv;

	struct cmd  *cmdr;
	uint8_t      cmda;
	uint8_t      cmdac;
	uint8_t      cmdc;
};

static struct       runtime rt;
static void         error(const uint8_t err_code, const char *msg);

static void         init(char *program_name, int argc, char **argv);
static int          identify(char *str);
static bool         parse(void);
static struct cmd  *findc(char *name);
static struct opt  *findo(char *cmd, char *opt);
static bool         act_opt(char *opt);
static void         view(void);

static void error(const uint8_t err_code, const char *msg)
{
	printf("%s - error %d - %s\n", rt.program_name, err_code, msg);
}

static struct cmd *findc(char *name)
{
	for (int i = 0; i < rt.cmdc + 1; i++) { 
		if (rt.cmdr[i].name != NULL && 
		    strcmp(rt.cmdr[i].name, name) == 0)
			return &rt.cmdr[i];
	}

	return NULL;
}

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

static bool act_opt(char *opt)
{
	struct opt *cur_opt = findo(rt.cmdr[rt.cmda].name, opt);

	if (cur_opt != NULL) {
		cur_opt->is_act = true;
		rt.cmdr[rt.cmda].optac++;
		return true;
	}

	return false;
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

static void init(char *program_name, int argc, char **argv)
{
	rt.program_name = program_name;
	rt.argc  = argc;
	rt.argv  = argv;
	rt.cmdc  = 0;
	rt.cmdac = 0;
	rt.cmdr  = (struct cmd *)malloc(sizeof(struct cmd));

	if (rt.cmdr == NULL)
		goto err;
	
	rt.cmdr[0].name  = "default";
	rt.cmdr[0].id    = 0;
	rt.cmdr[0].optc  = 0;
	rt.cmdr[0].optac = 0;
	rt.cmdr[0].optr  = NULL;
	rt.cmda          = 0;

	return;

err:
	error(99, "Failed to allocate memory for commands.");
}

static bool parse(void)
{
	for (int i = 1; i < rt.argc; i++) {
		char *cur  = rt.argv[i];
		int   type = identify(cur);
		int   len  = strlen(cur);

		switch (type) {
		case 1: {
			struct cmd *cur_cmd = findc(cur);
			if (rt.cmda == 0)
				rt.cmda = cur_cmd->id;
			else
				goto err;
			break;
		}
		case 2: {
			for (int j = 1; j < len; j++) {
				char tmpc[2] = {cur[j], '\0'};

				if (!act_opt(tmpc))
					goto err;
			}
			break;
		}
		case 3: {
				if (!act_opt(&cur[2]))
					goto err;

				break;
		}
		case 0:
		case -1:
			goto err;
				
		default:
			goto err;
		}
	}

	return true;

err:
	error(0, "unrecognized option");
	return false;
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
			printf("        > %s(%s) (id = %d, is_act = %d)\n", 
			copt->lname, copt->sname, copt->id, copt->is_act);
		}
	}
}

void argd(void)
{
	if (rt.cmdr == NULL)
		return;

	for (int i = 0; i <= rt.cmdc; i++) {
		if (rt.cmdr[i].optr != NULL)
			free(rt.cmdr[i].optr);
	}

	free(rt.cmdr);
	free(rt.program_name);
	rt.cmdr = NULL;
	rt.cmdc = 0;
	rt.cmda = 0;
}

bool arge(char *cmd, char *opt)
{
	if (cmd == NULL)
		return false;

	struct cmd *tmp_cmd = findc(cmd);

	if (tmp_cmd == NULL)
		return false; 

	if (opt == NULL) {
		return (tmp_cmd->id == rt.cmda);
	} else {
		struct opt *tmp_opt = findo(cmd, opt);

		if (tmp_opt == NULL)
			return false; 
            
		return tmp_opt->is_act; 
	}
}

void argc(char *name)
{
	rt.cmdc++;

	struct cmd *tmp = (struct cmd *)realloc(rt.cmdr, 
		(rt.cmdc + 1) * sizeof(struct cmd));

	if (tmp == NULL)
		goto err;
	
	rt.cmdr = tmp;
	rt.cmdr[rt.cmdc].name  = name;
	rt.cmdr[rt.cmdc].id    = rt.cmdc;
	rt.cmdr[rt.cmdc].optc  = 0;
	rt.cmdr[rt.cmdc].optac = 0;
	rt.cmdr[rt.cmdc].optr  = NULL;

	return;

err:
	error(100, "Failed to reallocate memory for commmand list.");
}

void argo(char *cmd, char *sname, char *lname)
{
	struct cmd *tmp_cmd = findc(cmd);

	if (tmp_cmd == NULL)
		goto err;

	struct opt *tmp_opt = (struct opt *)realloc(tmp_cmd->optr, 
		(tmp_cmd->optc + 1) * sizeof(struct opt));

	if (tmp_opt == NULL) {
		error(101, "Failed to reallocate memory for option list.");
		return;
	}

	tmp_cmd->optr = tmp_opt;

	struct opt *new_opt = &tmp_cmd->optr[tmp_cmd->optc];

	new_opt->cname  = cmd;
	new_opt->sname  = sname;
	new_opt->lname  = lname;
	new_opt->id     = tmp_cmd->optc;
	new_opt->is_act = false;
	
	tmp_cmd->optc++;

	return;

err:
	error(222, "Command not found.");
}

int argi(char *program_name, int argc, char **argv, 
		void (*reg)(void), void (*guide)(void))
{
	init(program_name, argc, argv);
	
	if (argc < 2)
		return -1;

	reg();
	
	if (!parse())
		guide();
	view();
	return 0;
}
