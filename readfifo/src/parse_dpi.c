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

OpCodes
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
