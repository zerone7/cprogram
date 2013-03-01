#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define RFILE "./robots.txt"

int main(int argc, char **argv)
{
    int sockfd, filefd, n, len = 0;
	struct hostent	*host;
    struct sockaddr_in haddr;
    char page[2000000];
    char http_cmd[500];
    char *page_pos;
	
	if (argc != 3 && argc != 2) {
		fprintf(stderr, "USAGE: getrob <inet_address> [file]\n");
		exit(1);
	}

    memset(&haddr, 0, sizeof(haddr));
    memset(http_cmd, 0, 500);
    memset(page, 0, 2000000);

    if (argv[1][0] > '0' && argv[1][0] <= '9') {
	    if (inet_pton(AF_INET, argv[1], &haddr.sin_addr) <= 0) {
		    printf("inet_pton err\n");
            exit(-1);
        } else {
            printf("Addr: %s\n", argv[1]);
        }
    } else {
	    if ((host = gethostbyname(argv[1])) == NULL) {
	    	fprintf(stderr, "nslookup failed on %s \n", argv[1]);
	    	exit(1);
	    }
	    printf("Name: %s, Addrtype: %d, Addrlen: %d\n", host->h_name, \
            host->h_addrtype, host->h_length);

	    haddr.sin_addr.s_addr = *(unsigned long *) host->h_addr_list[0];
	    printf("Addr: %s\n", inet_ntoa(haddr.sin_addr));
    }

    haddr.sin_port = htons(80);
    haddr.sin_family = AF_INET;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(sockfd, (struct sockaddr *)&haddr, sizeof(haddr)) < 0)
		printf("connect error\n"), exit(-1);

    //Send http request to get web page or robots.txt
    sprintf(http_cmd, "GET %s HTTP/1.0\r\nHost: %s\r\nUser-Agent: larbin (larbin@unspecified.mail)\r\n\r\n", argc == 2 ? "":argv[2], argv[1]);
    n = write(sockfd, http_cmd, strlen(http_cmd));
    printf("writen %d: %s\n", n, http_cmd);

    page_pos = page;
    n = 0;
	while ((n = read(sockfd, page_pos, 2000)) > 0) {
		page_pos += n;
        len += n;
    }

	if (n < 0)
		printf("read error\n");
    else {
        printf("read %d bytes\n", len);
       // printf("%s", page);
    }

    filefd = open(RFILE, O_WRONLY | O_CREAT | O_TRUNC, 0666);    
    write(filefd, page, len);

    return 0;
}
