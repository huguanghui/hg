#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX 80
#define PORT 18080
#define SA struct sockaddr

void func(int sockfd)
{
    char buff[MAX];
    int n;

    for (;;) {
        bzero(buff, MAX);
        read(sockfd, buff, sizeof(buff));
        printf("From client: %s\t To client :", buff);
        bzero(buff, MAX);
        n = 0;
        while ((buff[n++] = getchar()) != '\n') {
            ;
        }

        write(sockfd, buff, sizeof(buff));

        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
    }

    return;
}

int main(int argc, char *argv[])
{
    int ret = 0;
    int rc = 0;
    int sockfd, connfd;
    unsigned int len;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket creation failed...\n");
        exit(0);
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    rc = bind(sockfd, (SA *)&servaddr, sizeof(servaddr));
    if (0 != rc) {
        perror("socket bind failed...\n");
        exit(0);
    }
    rc = listen(sockfd, 5);
    if (0 != rc) {
        perror("Listen failed...\n");
        exit(0);
    }
    len = sizeof(cli);
    connfd = accept(sockfd, (SA *)&cli, &len);
    if (connfd < 0) {
        perror("server accept failed...\n");
        exit(0);
    }
    func(connfd);
    close(sockfd);
    return 0;
}
