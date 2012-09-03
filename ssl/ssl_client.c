/*
 * Compile command:
 * gcc ssl_client.c -o client -I/usr/local/ssl/include -lcrypto -lssl
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
#include <assert.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define BUFSIZE 1024 
#define PORT    39728
#define ADDR    "127.0.0.1"

void show_certs(SSL *ssl)
{
    X509 *cert;
    char *line;

    assert(ssl);
    cert = SSL_get_peer_certificate(ssl);
    if (cert) {
        printf("Digital certificate info:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Certificate: %s\n", line);
        free(line);
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Auther: %s\n", line);
        free(line);
        X509_free(cert);
    } else
        printf("No certificate info!\n");
}

int main(int argc, char **argv)
{
    int sockfd, len;
    struct sockaddr_in dest;
    char buf[BUFSIZE];
    SSL_CTX *ctx;
    SSL *ssl;

    /* ssl lib init */
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ctx = SSL_CTX_new(SSLv23_client_method());
    if (ctx == NULL) {
        ERR_print_errors_fp(stdout);
        return 1;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return errno;
    }

    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(PORT);
    dest.sin_addr.s_addr = inet_addr(ADDR);

    if (connect(sockfd, (struct sockaddr *)&dest, sizeof(dest)) != 0) {
        perror("connect");
        return -1;
    }
    printf("server %s port %d connected\n", ADDR, PORT);

    /* create a new SSL */
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sockfd);
    if (SSL_connect(ssl) < 0)
        ERR_print_errors_fp(stderr);
    else {
        printf("connected with %s encryption\n", SSL_get_cipher(ssl));
        show_certs(ssl);
    }

    memset(buf, 0, BUFSIZE);
    strcpy(buf, "from client to server");
    len = SSL_write(ssl, buf, strlen(buf));
    if (len < 0)
        perror("SSL_write");
    else
        printf("send %d bytes\n", len);

finish:
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sockfd);
    SSL_CTX_free(ctx);
    return 0;
}
