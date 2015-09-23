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

#define MAX_LEN		40960
#define MAXLINE 4096
#define true 1
#define false 0


#define ERR_EXIT(m) \
    do { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while(0)

typedef enum {
	oBadOption,
	oStatus,
	oHttp,
	oAgent,
	oIp,
} OpCodes;

/** @internal
 The config file keywords for the different configuration options */
static const struct {
	const char *name;
	OpCodes opcode;
} keywords[] = {
	{ "status",             	oStatus },
	{ "http",         	oHttp },
	{ "agent",  	oAgent },
	{ "ip",   	oIp },
	{ NULL,				oBadOption },
};




static OpCodes
conf_parse_line(const char *line,int paramnum)
{
	int i;

	for (i = 0; keywords[i].name; i++)
		if (strcasecmp(line, keywords[i].name) == 0)
			return keywords[i].opcode;
	
	return oBadOption;
}

bool startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}


#define TO_NEXT_CONF(s, e) do { \
	while (*s != '\0' && *s != '\t') { \
		s++; \
	} \
	if (*s != '\0') { \
		*s = '\0'; \
		s++; \
	} else { \
		e = 1; \
	} \
} while (0)


char * safe_strdup(const char *s) {
	char * retval = NULL;
	if (!s) {
		exit(1);
	}
	retval = strdup(s);
	if (!retval) {
		exit(1);
	}
	return (retval);
}



char* parse_http(char *ptr){
	char *ptrcopy = NULL;
	char *p1;
	ptrcopy = safe_strdup(ptr);
	if ((p1 = strchr(ptrcopy, '\t'))) {
		p1[0] = '\0';
	}
	return ptrcopy;
}

char* inSubChar(char *ptrcopy){
	char *p1=ptrcopy;
	//if((p1 = strchr(ptrcopy, '-')) || (p1=strchr(ptrcopy, ' ')) || (p1 =strchr(ptrcopy, ',') )|| (p1=strchr(ptrcopy, '(') )||(p1 = strchr(ptrcopy, ')'))||(p1 = strchr(ptrcopy, ';')) )
	while(*p1!='\0'){
		if( (*p1 =='-')||(*p1 ==' ') ||(*p1 ==',')||(*p1 =='(')||(*p1 ==')')||(*p1 ==';'))	
			return p1;
		else
			p1++;
	}
	return NULL;
}


char* parse_http2(char *ptr){
	char *ptrcopy = NULL;
	char *p1=NULL,*p2=NULL,*p3=NULL,*s1=NULL;	
	ptrcopy = (char *)malloc(MAXLINE);
	memcpy(ptrcopy,ptr,sizeof(ptr));

	if ((p3 = strchr(ptrcopy, '\t'))) {
		p3[0] = '\0';
	}
	s1 = ptrcopy;
	int i =0;
	while(p1=inSubChar(s1)){
		p2 =p1;	
		//memcpy(p1+1, p1, strlen(p1)); 
		//p1[0] = "\\";	
		p2++;
		//p2++;
		s1 = p2;
		//printf("llop1");
		i++; 
	}
	
	printf("%d",i);
	return ptrcopy;
}

char* parse_agent2(char *ptr){
	char *ptrcopy = (char *)malloc(MAXLINE);
	char *p1=NULL,*p2=NULL,*p3=NULL,*s1=NULL;	
	if ((p1 = strchr(ptr, '\t'))) {
		p1[0] = '\0';
	}
	memcpy(ptrcopy,ptr,strlen(ptr)+1); 
	s1 = ptrcopy;
	int i =0;
	char tmp[MAXLINE];
	tmp[0] = '\\';	
	while(p1=inSubChar(s1)){
		p2 =p1;	
		memcpy(tmp+1, p1, strlen(p1)+1); 
		p1[0] = '\\';
		memcpy(p1, tmp, strlen(p1)+2);	
		p2++;
		p2++;
		s1 = p2;
		//printf("%c",*p1);
		i++; 
	}
	
	//printf("%d",i);
	return ptrcopy;
}

char* parse_agent(char *ptr){
	char *ptrcopy = NULL;
	char *p1;
	ptrcopy = safe_strdup(ptr);
	if ((p1 = strchr(ptrcopy, '\t'))) {
		p1[0] = '\0';
	}
	return ptrcopy;
}

char* parse_ip(char *ptr){
	char *ptrcopy = NULL;
	char *p1;
	ptrcopy = safe_strdup(ptr);
	return ptrcopy;
}

int main(int argc, char *argv[])
{


    size_t len = 0;
    ssize_t read;
    FILE *fp;
    fp = fopen("td", "r");
    if (fp == NULL)
        ERR_EXIT("open error");

    char buf[MAXLINE];
    char *http = NULL;
    char *ip = NULL;
    char *agent = NULL;
    char *s,*p1;

    
    struct hostent *host;
    int n, sock, port;
    struct sockaddr_in server;
    int len1 = sizeof(struct sockaddr_in);
    char *logmsg = NULL;
    
    host = gethostbyname("42.123.76.25");
    port = atoi("5140");
    if ((sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
	perror("socket");
	return 1;
    }
    memset((char *) &server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr = *((struct in_addr*) host->h_addr);

    char *uploadmsg = (char *)malloc(MAX_LEN);
    memset(uploadmsg, 0, sizeof(uploadmsg));
    sprintf(uploadmsg,"{\"pubinfo\":\"%s\"\,\"log\":[","hot");
    int paramnum =1 ;
    while( ReadLine(buf, MAXLINE, fp) ) {
	
        if(startsWith("status:200",buf)==false){
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
    				
    				logmsg = (char *)malloc(MAXLINE);
				if (paramnum<3){
					if(paramnum==0){
					
    						uploadmsg = (char *)malloc(MAX_LEN);
    						memset(uploadmsg, 0, sizeof(uploadmsg));
						sprintf(uploadmsg,"{\"pubinfo\":\"%s\"\,\"log\":[","hot");
					}
					sprintf(logmsg,"{\"log_value\":\"%s\"\,\"user_agent\":\"%s\"},",http,agent);
					//sprintf(uploadmsg+strlen(uploadmsg),logmsg);
					strcat(uploadmsg,logmsg); 
					paramnum++;
					//printf("%s\n",logmsg);		
				}else{
    					//*uploadmsg = (char *)malloc(MAX_LEN);
					//sprintf(logmsg,"{\"log_value\":\"%s\"\,\"user_agent\":\"%s\"}]}",http,agent);
					//length += sprintf(uploadmsg+length,logmsg);
					//printf("%s\n",uploadmsg);		
					//if (sendto(sock, uploadmsg, strlen(uploadmsg), 0, (struct sockaddr *) &server, len1) == -1) {
							//perror("sendto()");
							//return 1;
    					//}
					sprintf(logmsg,"{\"log_value\":\"%s\"\,\"user_agent\":\"%s\"}]}",http,agent);
					strcat(uploadmsg,logmsg); 
					//sprintf(uploadmsg+strlen(uploadmsg),logmsg);
					//printf("%s\n",uploadmsg);		
					if (sendto(sock, uploadmsg, strlen(uploadmsg), 0, (struct sockaddr *) &server, len1) == -1) {
							perror("sendto()");
							return 1;
    					}
					paramnum=0;
    					free(uploadmsg);
					//ilength = 0; 
    					//memset(uploadmsg, 0, sizeof(uploadmsg));
    					//length += sprintf(uploadmsg,"{\"pubinfo\":\"%s\"\,\"log\":\"[{","hot");
						
				}
				//printf("%s",sendmsg);		
				//if (sendto(sock, sendmsg, strlen(sendmsg), 0, (struct sockaddr *) &server, len1) == -1) {
				//perror("sendto()");
				//return 1;
    			//}
			free(logmsg);
			free(http);
			free(agent);
			free(ip);
		}
	}
	
    }

    fclose(fp);
    unlink("td"); // delete a name and possibly the file it refers to
    return 0;
}

int ReadLine(char *buff, int size, FILE *fp)
{
  buff[0] = '\0';
  buff[size - 1] = '\0';             /* mark end of buffer */
  char *tmp;

  if (fgets(buff, size, fp) == NULL) {
      *buff = '\0';                   /* EOF */
      return false;
  }
  else {
      /* remove newline */
      if ((tmp = strrchr(buff, '\n')) != NULL) {
        *tmp = '\0';
      }
  }
  return true;
}

