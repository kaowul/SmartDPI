/********************************************************************\
 * This program is free software; you can redistribute it and/or    *
 * modify it under the terms of the GNU General Public License as   *
 * published by the Free Software Foundation; either version 2 of   *
 * the License, or (at your option) any later version.              *
 *                                                                  *
 * This program is distributed in the hope that it will be useful,  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    *
 * GNU General Public License for more details.                     *
 *                                                                  *
 * You should have received a copy of the GNU General Public License*
 * along with this program; if not, contact:                        *
 *                                                                  *
 * Free Software Foundation           Voice:  +1-617-542-5942       *
 * 59 Temple Place - Suite 330        Fax:    +1-617-542-2652       *
 * Boston, MA  02111-1307,  USA       gnu@gnu.org                   *
 *                                                                  *
 \********************************************************************/

/* $Id$ */
/** @file conf.c
  @brief Config file parsing
  @author Copyright (C) 2004 Philippe April <papril777@yahoo.com>
  @author Copyright (C) 2007 Benoit Grégoire, Technologies Coeus inc.
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

#include <pthread.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>
#include "conf.h"
#include "utils.h"

//#include "util.h"

/** @internal
 * Holds the current configuration of the gateway */
static s_config config;
/** @internal
 The different configuration options */
typedef enum {
	oBadOption,
	oDevID,
	oGatewayInterface,
	oLogServer,
	oServHostname,
	oServLogPort,
} OpCodes;

/** @internal
 The config file keywords for the different configuration options */
static const struct {
	const char *name;
	OpCodes opcode;
} keywords[] = {
	{ "devid",          		oDevID },
	{ "gatewayinterface",   	oGatewayInterface },
	{ "logserver",         		oLogServer },
	{ "hostname",			oServHostname },
	{ "Logport",			oServLogPort },
	{ NULL,				oBadOption },
};

static void config_notnull(const void *parm, const char *parmname);
static void parse_server(FILE *, const char *, int *,int);
static OpCodes config_parse_token(const char *cp, const char *filename, int linenum);

/** Accessor for the current gateway configuration
@return:  A pointer to the current config.  The pointer isn't opaque, but should be treated as READ-ONLY
 */
s_config *
config_get_config(void)
{
    return &config;
}

/** Sets the default config parameters and initialises the configuration system */
void
config_init(void)
{
	char		*host = NULL;
	int		logport;
	t_serv		*new;

	strncpy(config.configfile, DEFAULT_CONFIGFILE, sizeof(config.configfile));
	//config.gw_id = safe_strdup(DEFAULT_GATEWAYID);
	config.dev_id = safe_strdup(DEFAULT_DEV);
	config.gw_mac = NULL;
	config.gw_interface = NULL;
	config.log_servers = NULL;
	/*
	new = malloc(sizeof(t_serv));
	host = safe_strdup(DEFAULT_LOGSERVER);	
	logport = DEFAULT_LOGSERVPORT;
	memset(new, 0, sizeof(t_serv)); 
	new->serv_hostname = host;
	new->serv_port = logport;


	if (config.log_servers == NULL) {
		config.log_servers = new;
	} */

}

/** @internal
Parses a single token from the config file
*/
static OpCodes
config_parse_token(const char *cp, const char *filename, int linenum)
{
	int i;

	for (i = 0; keywords[i].name; i++)
		if (strcasecmp(cp, keywords[i].name) == 0)
			return keywords[i].opcode;

	return oBadOption;
}

/** @internal
Parses auth server information
*/
static void
parse_server(FILE *file, const char *filename, int *linenum,int type)
{
	char		*host = NULL,
			line[MAX_BUF],
			*p1,
			*p2;
	int		logport,
			opcode;
	t_serv		*new,
			*tmp;

	/* Defaults */
	logport = DEFAULT_LOGSERVPORT;

	/* Parsing loop */
	while (memset(line, 0, MAX_BUF) && fgets(line, MAX_BUF - 1, file) && (strchr(line, '}') == NULL)) {
		(*linenum)++; /* increment line counter. */

		/* skip leading blank spaces */
		for (p1 = line; isblank(*p1); p1++);

		/* End at end of line */
		if ((p2 = strchr(p1, '#')) != NULL) {
			*p2 = '\0';
		} else if ((p2 = strchr(p1, '\r')) != NULL) {
			*p2 = '\0';
		} else if ((p2 = strchr(p1, '\n')) != NULL) {
			*p2 = '\0';
		}

		/* next, we coopt the parsing of the regular config */
		if (strlen(p1) > 0) {
			p2 = p1;
			/* keep going until word boundary is found. */
			while ((*p2 != '\0') && (!isblank(*p2)))
				p2++;

			/* Terminate first word. */
			*p2 = '\0';
			p2++;

			/* skip all further blanks. */
			while (isblank(*p2))
				p2++;
			
			/* Get opcode */
			opcode = config_parse_token(p1, filename, *linenum);
			
			switch (opcode) {
				case oServHostname:
					host = safe_strdup(p2);
					break;
				case oServLogPort:
					logport = atoi(p2);
					break;
				case oBadOption:
				default:
					break;
			}
		}
	}

	/* only proceed if we have an host and a path */
	if (host == NULL)
		return;
	

	/* Allocate memory */
	new = malloc(sizeof(t_serv));
	
	/* Fill in struct */
	memset(new, 0, sizeof(t_serv)); /*< Fill all with NULL */
	new->serv_hostname = host;
	new->serv_port = logport;
	
	struct in_addr *h_addr;
	char * ip;
	
	
	switch (type) {
		case 1:
			if (config.log_servers == NULL) {
				config.log_servers = new;
			} else {
				for (tmp = config.log_servers; tmp->next != NULL;
						tmp = tmp->next);
				tmp->next = new;
			}
			break;
		default:
			break;
	}


}

/**
Advance to the next word
@param s string to parse, this is the next_word pointer, the value of s
	 when the macro is called is the current word, after the macro
	 completes, s contains the beginning of the NEXT word, so you
	 need to save s to something else before doing TO_NEXT_WORD
@param e should be 0 when calling TO_NEXT_WORD(), it'll be changed to 1
	 if the end of the string is reached.
*/
#define TO_NEXT_WORD(s, e) do { \
	while (*s != '\0' && !isblank(*s)) { \
		s++; \
	} \
	if (*s != '\0') { \
		*s = '\0'; \
		s++; \
		while (isblank(*s)) \
			s++; \
	} else { \
		e = 1; \
	} \
} while (0)


/**
@param filename Full path of the configuration file to be read 
*/
void
config_read(const char *filename)
{
	FILE *fd;
	char line[MAX_BUF], *s, *p1, *p2;
	int linenum = 0, opcode, value, len;

	printf("Reading configuration file '%s'", filename);

	if (!(fd = fopen(filename, "r"))) {
		printf( "Could not open configuration file '%s', "
				"exiting...", filename);
		exit(1);
	}

	while (!feof(fd) && fgets(line, MAX_BUF, fd)) {
		linenum++;
		s = line;

		if (s[strlen(s) - 1] == '\n')
			s[strlen(s) - 1] = '\0';

		if ((p1 = strchr(s, ' '))) {
			p1[0] = '\0';
		} else if ((p1 = strchr(s, '\t'))) {
			p1[0] = '\0';
		}

		if (p1) {
			p1++;

			// Trim leading spaces
			len = strlen(p1);
			while (*p1 && len) {
				if (*p1 == ' ')
					p1++;
				else
					break;
				len = strlen(p1);
			}


			if ((p2 = strchr(p1, ' '))) {
				p2[0] = '\0';
			} else if ((p2 = strstr(p1, "\r\n"))) {
				p2[0] = '\0';
			} else if ((p2 = strchr(p1, '\n'))) {
				p2[0] = '\0';
			}
		}

		if (p1 && p1[0] != '\0') {
			/* Strip trailing spaces */

			if ((strncmp(s, "#", 1)) != 0) {
				opcode = config_parse_token(s, filename, linenum);

				switch(opcode) {
				case oDevID:
					config.dev_id = safe_strdup(p1);
					break;
				case oGatewayInterface:
					config.gw_interface = safe_strdup(p1);
					break;
				case oLogServer:
					printf("oLogServer");
					parse_server(fd, filename,
							&linenum,1);
					break;
				case oBadOption:
					break;
			}
		}
	}

	}
	fclose(fd);
}


t_serv *
get_log_server(void)
{

        /* This is as good as atomic */
        return config.log_servers;
}


