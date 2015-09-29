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
/** @file conf.h
    @brief Config file parsing
    @author Copyright (C) 2004 Philippe April <papril777@yahoo.com>
*/


/*@{*/ 
/** Defines */
/** How many times should we try detecting the interface with the default route
 * (in seconds).  If set to 0, it will keep retrying forever */
#define NUM_EXT_INTERFACE_DETECT_RETRY 0
/** How often should we try to detect the interface with the default route
 *  if it isn't up yet (interval in seconds) */
#define EXT_INTERFACE_DETECT_RETRY_INTERVAL 1

/** Defaults configuration values */
#define DEFAULT_CONFIGFILE "/etc/wifidog.conf"
#define DEFAULT_DEV "RIXIN-RXMAR30P-20140715-f4b6c84f"
#define DEFAULT_LOGSERVPORT 5140
#define MAX_BUF 4096
#define DEFAULT_LOGSERVER "Wifi-admin.ctbri.com.cn"
/** Note that DEFAULT_AUTHSERVSSLAVAILABLE must be 0 or 1, even if the config file syntax is yes or no */
/** Note:  The path must be prefixed by /, and must be suffixed /.  Put / for the server root.*/
/*@}*/ 

/**
 * Information about the authentication server
 */
typedef struct _serv_t {
    char *serv_hostname;	/**< @brief Hostname of the central server */
    int serv_port;	/**< @brief Https port the central server
				     listens on */
    char *last_ip;	/**< @brief Last ip used by authserver */
    struct _serv_t *next;
} t_serv;

/**
 * Firewall targets
 */
/**
 * Configuration structure
 */
typedef struct {
    char configfile[255];	/**< @brief name of the config file */
    char *ssid;
    char *dev_id;
    char *gw_mac;	
    char *gw_interface;		/**< @brief Interface we will accept connections on */
    t_serv *log_servers;	/**< @brief portal servers list */
} s_config;

/** @brief Get the current gateway configuration */
s_config *config_get_config(void);

/** @brief Initialise the conf system */
void config_init(void);

/** @brief Initialize the variables we override with the command line*/

/** @brief Reads the configuration file */
void config_read(const char *filename);


t_serv *get_log_server(void);

char * safe_strdup(const char *s); 
