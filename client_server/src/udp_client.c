#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 9090
#define MAXLINE 1024

int main(int argc, char *argv[])
{
    printf("[HGH-TEST][%s %d]\n", __FUNCTION__, __LINE__);
    int rc = 0;
    int sockfd;
    char buf[MAXLINE];
    char *hello = "Hello from client";
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    int n;
    unsigned int len;

    sendto(sockfd, (const char *)hello, strlen(hello), 0,
        (const struct sockaddr *)&servaddr, sizeof(servaddr));
    printf("Hello message sent.\n");

    n = recvfrom(sockfd, (char*)buf, MAXLINE, MSG_WAITALL, (struct sockaddr*)&servaddr, &len);
    buf[n] = '\0';
    printf("Server : %s\n", buf);
    close(sockfd);

    return 0;
}
