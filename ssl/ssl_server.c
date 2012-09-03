/*
 * Compile command:
 * gcc ssl_server.c -o server -I/usr/local/ssl/include -lcrypto -lssl
 *
 * To gernerate key files:
 * openssl genrsa -out privkey.pem 2048
 * openssl req -new -x509 -key privkey.pem -out cacert.pem -days 1095
 */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define BUFSIZE 1024 
#define PORT    39728
#define ADDR    "127.0.0.1"

int main()
{
    int sockfd, client_fd;
    socklen_t len;
    struct sockaddr_in src, dest;
    char buf[BUFSIZE];
    SSL_CTX *ctx;
    SSL *ssl;

    /* ssl lib init */
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ctx = SSL_CTX_new(SSLv23_server_method());
    if (ctx == NULL) {
        ERR_print_errors_fp(stdout);
        return 1;
    }

    /* load user certificate (contain public key) and send it to client */
    if (SSL_CTX_use_certificate_file(ctx, "cacert.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stdout);
        return 1;
    }

    /* load client private key */
    if (SSL_CTX_use_PrivateKey_file(ctx, "privkey.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stdout);
        return 1;
    }

    /* check client private key */
    if (!SSL_CTX_check_private_key(ctx)) {
        ERR_print_errors_fp(stdout);
        return 1;
    }

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    memset(&src, 0, sizeof(src));
    src.sin_family = AF_INET;
    src.sin_port = htons(PORT);
    src.sin_addr.s_addr = inet_addr(ADDR);

    if (bind(sockfd, (struct sockaddr *)&src, sizeof(src)) < 0) {
        perror("bind");
        return -1;
    }

    if (listen(sockfd, 5) < 0) {
        perror("listen");
        return -1;
    }

    while (1) {
        SSL *ssl;
        len = sizeof(struct sockaddr);
        if ((client_fd = accept(sockfd, (struct sockaddr *)&dest, &len)) < 0) {
            perror("accept");
            return -1;
        }
        printf("client %s %d connected\n", inet_ntoa(dest.sin_addr), ntohs(dest.sin_port));

        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client_fd);
        if (SSL_accept(ssl) < 0) {
            perror("ssl accept");
            close(client_fd);
            break;
        }

        strcpy(buf, "server to client");
        len = SSL_write(ssl, buf, strlen(buf) + 1);
        if (len <= 0) {
            perror("ssl write");
            goto finish;
        } else
            printf("send %d bytes\n", len);

        len = SSL_read(ssl, buf, BUFSIZE);
        if (len > 0)
            printf("read %d bytes: %s\n", len, buf); 
        else
            perror("ssl read");

finish:
        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(client_fd);
    }

    close(sockfd);
    SSL_CTX_free(ctx);
    return 0;
}
