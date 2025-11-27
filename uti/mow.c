/*
 *            Maintainer 2025 Guaitan1st & Qian Junfan  <guaitanth1st@gmail.com>
 *	                                                 <qianjunfan0@gmail.com>
 *
 *                                        Copyright (C) Guaitan1st & Qian Junfan
 *
 *                                                           Version Alpha 0.0.2
 */
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "libfile.h"
#include "libarg.h"
#include "libtio.h"
#include "libtime.h"
#include "libstr.h"

#define VERSION "0.0.2"


void help(void)
{
	printf("this is the help function\n");
}

void login(void)
{
	printf("Your name:");
	char *name = readl();
	printf("Your password:");
	char *passwd = readl();
}



void shell(void)
{
	printf(
		"Welcome to mow, the friendly to-do manager.\n"
	       	"Type help for instructions on how to use mow.\n\n"
		"You have not logged in." 
		"Please enter login or l to access your account.\n"
		"If you don't have an account, enter register or r to sign up."
	);

running:

	printf("mow $ ");
	char *cmd = io_readl();
	
	if (!strcmp(cmd, "h") || !strcmp(cmd, "help"))
		help();
	else if (!strcmp(cmd, "q") || strcmp(cmd, "quit"))
		goto quit;
	else if (!strcmp(cmd, "l") || strcmp(cmd, "login"))
		login();
	goto running;

quit:
	printf("Bye from mow :)\n");
}


int main(void) {
	shell();

	return 0;
}
