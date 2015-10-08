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

OpCodes
conf_parse_line(const char *line,int paramnum);

bool startsWith(const char *pre, const char *str);

char* parse_http(char *ptr);

char* inSubChar(char *ptrcopy);

char* parse_http2(char *ptr);

char* parse_agent2(char *ptr);

char* parse_agent(char *ptr);

char* parse_ip(char *ptr);

int ReadLine(char *buff, int size, FILE *fp);

