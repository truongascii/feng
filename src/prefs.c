/* * 
 *  $Id: prefs.c 406 2006-10-17 13:19:51Z shawill $
 *  
 *  This file is part of Fenice
 *
 *  Fenice -- Open Media Server
 *
 *  Copyright (C) 2004 by
 *  	
 *	- Giampaolo Mancini	<giampaolo.mancini@polito.it>
 *	- Francesco Varano	<francesco.varano@polito.it>
 *	- Marco Penno		<marco.penno@polito.it>
 *	- Federico Ridolfo	<federico.ridolfo@polito.it>
 *	- Eugenio Menegatti 	<m.eu@libero.it>
 *	- Stefano Cau
 *	- Giuliano Emma
 *	- Stefano Oldrini
 * 
 *  Fenice is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Fenice is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Fenice; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *  
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fenice/prefs.h>

CREATE_PREFS_DATA;

void prefs_init(char *fileconf)
{
	FILE *f;
	char opened = 0;
	char line[256];
	char *p, *q, *cont;
	int l;
	pref_id i;

	prefs_use_default(PREFS_ALL);

	if ((f = fopen(fileconf, "rt")) == NULL) {
		printf("Error opening file %s, trying default (%s):\n",
		       fileconf, FENICE_CONF_PATH_DEFAULT_STR);
		if ((f = fopen(FENICE_CONF_PATH_DEFAULT_STR, "rt")) == NULL) {
			printf
			    ("Error opening default file, using internal defaults:\n");
		} else
			opened = 1;
	} else
		opened = 1;

	if (opened) {
		do {
			cont = fgets(line, 80, f);

			if (cont && line[0] != '#') {
				p = NULL;
				for (i = PREFS_FIRST;
				     i < PREFS_LAST && p == NULL; i++) {
					p = strstr(line, prefs[i].tag);
					if (p != NULL) {
						p = strstr(p, "=");
						if (p != NULL) {
							q = p + 1;
							if (prefs[i].type ==
							    STRING) {
								p = strstr(q,
									   "\n");
								if (p != NULL) {
									*p = '\0';
									SET_STRING_DATA
									    (i,
									     q);
								}
							} else if (prefs[i].
								   type ==
								   INTEGER) {
								if (sscanf
								    (q, "%i",
								     &l) == 1) {
									SET_INTEGER_DATA
									    (i,
									     l);
								}
							}
						}
					}
				}
			}
		} while (cont);
		fclose(f);
	}
	// PREFS_HOSTNAME
	gethostname(line, sizeof(line));
	l = strlen(line);
	if (getdomainname(line + l + 1, sizeof(line) - l) != 0) {
		line[l] = '.';
	}
	SET_STRING_DATA(PREFS_HOSTNAME, line);

	printf("\n");
	printf("\tavroot directory is: %s\n", prefs_get_serv_root());
	printf("\thostname is: %s\n", prefs_get_hostname());
	printf("\trtsp listening port for TCP is: %d\n", prefs_get_port());
#ifdef HAVE_SCTP_FENICE
	if (prefs_get_sctp_port() > 0)
		printf("\trtsp listening port for SCTP is: %d\n",
		       prefs_get_sctp_port());
#endif
	printf("\tlog file is: %s\n", prefs_get_log());
	printf("\n");
}

void prefs_use_default(pref_id index)
{
	char buffer[256];

	switch (index) {
	case PREFS_ROOT:
		strcpy(buffer, FENICE_AVROOT_DIR_DEFAULT_STR);
		strcat(buffer, "/");
		SET_STRING_DATA(PREFS_ROOT, buffer);
		break;
	case PREFS_TCP_PORT:
		SET_INTEGER_DATA(PREFS_TCP_PORT, FENICE_RTSP_PORT_DEFAULT);
		break;
	case PREFS_SCTP_PORT:
		SET_INTEGER_DATA(PREFS_SCTP_PORT, -1);
		break;
	case PREFS_MAX_SESSION:
		SET_INTEGER_DATA(PREFS_MAX_SESSION, FENICE_MAX_SESSION_DEFAULT);
		break;
	case PREFS_LOG:
		SET_STRING_DATA(PREFS_LOG, FENICE_LOG_FILE_DEFAULT_STR);
		break;
	case PREFS_ALL:
		strcpy(buffer, FENICE_AVROOT_DIR_DEFAULT_STR);
		strcat(buffer, "/");
		SET_STRING_DATA(PREFS_ROOT, buffer);
		SET_INTEGER_DATA(PREFS_TCP_PORT, FENICE_RTSP_PORT_DEFAULT);
		SET_INTEGER_DATA(PREFS_SCTP_PORT, -1);
		SET_INTEGER_DATA(PREFS_MAX_SESSION, FENICE_MAX_SESSION_DEFAULT);
		SET_STRING_DATA(PREFS_LOG, FENICE_LOG_FILE_DEFAULT_STR);
		break;
	default:
		break;
	}
}

void *get_pref(pref_id id)
{
	if (id >= PREFS_FIRST && id < PREFS_LAST)
		return prefs[id].data;
	else
		return NULL;
}