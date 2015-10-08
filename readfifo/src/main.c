/*************************************************************************
    > File Name: process_.c
    > Author: Simba
    > Mail: dameng34@163.com
    > Created Time: Sat 23 Feb 2013 02:34:02 PM CST
 ************************************************************************/
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<signal.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>
#include "utils.h"
#include "conf.h"
#include "parse_dpi.h"


int main(int argc, char *argv[])
{

   s_config *config = config_get_config();
   config_init();
   config_read(config->configfile);
   t_serv *log_server = config->log_servers ;
   
    size_t len = 0;
    ssize_t read;
    FILE *fp;
    fp = fopen("/tmp/tp", "r");
    if (fp == NULL)
        ERR_EXIT("open error");

    char buf[MAXLINE];
    char *http = NULL;
    char *ip = NULL;
    char *mac = NULL;
    char *agent = NULL;
    char *s,*p1;
    time_t timer;
    char *time_buffer=NULL;
    struct tm* tm_info;


    
    struct hostent *host;
    int n, sock, port;
    struct sockaddr_in server;
    int len1 = sizeof(struct sockaddr_in);
    char *logmsg = NULL;
   
    
    if (!config->gw_mac) {
	    if ((config->gw_mac = get_iface_mac(config->gw_interface)) == NULL) {
		    printf("Could not get Hostname information of %s, exiting...");
		    exit(1);
	    }
    }

    if (!config->ssid) {
	    if ((config->ssid = ssidRead()) == NULL) {
		    printf("Could not get Hostname information of %s, exiting...");
		    exit(1);
	    }
    }



 
    host = gethostbyname(log_server->serv_hostname);
    if ((sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
	perror("socket");
	return 1;
    }
    memset((char *) &server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(log_server->serv_port);
    server.sin_addr = *((struct in_addr*) host->h_addr);

    char *uploadmsg = (char *)malloc(MAX_LEN);
    memset(uploadmsg, 0, sizeof(uploadmsg));
    sprintf(uploadmsg,"{\"gw_id\":\"%s\"\,\"gw_mac\":\"%s\"\,\"dev_id\":\"%s\"\,\"log\":[",config->ssid,config->gw_mac,config->dev_id);
    int paramnum =1 ;
    while( ReadLine(buf, MAXLINE, fp) ) {
	
        if((startsWith("status:200",buf)==false)||(strstr(buf,"htm")==NULL)){
		continue;		
	}

	s = buf;	
	int opcode,finished = 0;
   	http = NULL;
    	
	
	while(finished ==0){
		if((p1 = strchr(s, ':'))) {
			p1[0] = '\0';
		} 

		if(p1) {
			p1++;
		}
		if (p1 && p1[0] != '\0') {
			opcode = conf_parse_line(s,paramnum);
			switch(opcode) {
			case oHttp:
				http = parse_http(p1);
				break;
			case oAgent:
				agent = parse_agent(p1);
				//printf("%s",agent);
				break;
			case oIp:
				ip = parse_ip(p1);
				mac = arp_get(ip);
				//printf("oip");
				break;
			case oStatus:
				//printf("ostatus");
				break;
			case oBadOption:
				break;
			}
		}
		s = p1;
		TO_NEXT_CONF(s, finished);
		if(finished==1){
    				
    				
				time(&timer);
    				tm_info = localtime(&timer);
    				time_buffer = (char *)malloc(26);
    				strftime(time_buffer, 26, "%Y:%m:%d %H:%M:%S", tm_info);
				logmsg = (char *)malloc(MAXLINE);
				if (paramnum<3){
					if(paramnum==0){
					
    						uploadmsg = (char *)malloc(MAX_LEN);
    						memset(uploadmsg, 0, sizeof(uploadmsg));
    sprintf(uploadmsg,"{\"gw_id\":\"%s\"\,\"gw_mac\":\"%s\"\,\"dev_id\":\"%s\"\,\"log\":[",config->ssid,config->gw_mac,config->dev_id);
					}
					sprintf(logmsg,"{\"time\":\"%s\"\,\"user\":\"\"\,\"log_type\":\"1\"\,\"log_value\":\"%s\"\,\"user_agent\":\"%s\"\,\"mac\":\"%s\"}\,",time_buffer,http,agent,mac);
					strcat(uploadmsg,logmsg); 
					paramnum++;
				}else{
					sprintf(logmsg,"{\"time\":\"%s\"\,\"user\":\"\"\,\"log_type\":\"1\"\,\"log_value\":\"%s\"\,\"user_agent\":\"%s\"\,\"mac\":\"%s\"}]}",time_buffer,http,agent,mac);
					strcat(uploadmsg,logmsg); 
					if (sendto(sock, uploadmsg, strlen(uploadmsg), 0, (struct sockaddr *) &server, len1) == -1) {
							perror("sendto()");
							return 1;
    					}
					paramnum=0;
    					free(uploadmsg);
						
				}
			free(logmsg);
			free(time_buffer);
			free(http);
			free(agent);
			free(ip);
			free(mac);
		}
	}
	
    }
	

    fclose(fp);
    unlink("/tmp/tp"); // delete a name and possibly the file it refers to
    return 0;
}

