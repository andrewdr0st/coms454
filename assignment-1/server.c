#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

int port = 4443;
int logging = 1;

int main(int argc, char* argv[]) {
  //Read command line args
  int opt;
  while ((opt = getopt(argc, argv, "sp:")) != -1) {
    switch (opt) {
      case 's': //silent mode
        logging = 0;
        break;
      case 'p': //set port
        port = atoi(optarg);
        break;
      default:
        fprintf(stderr, "Usage: ./server [-s] [-p portnum]\n");
        return 1;
    }
  }

  //Init SSL context
  SSL_load_error_strings();
  const SSL_METHOD* method = TLS_server_method();
  SSL_CTX* ctx = SSL_CTX_new(method);
  if (!ctx) {
    ERR_print_errors_fp(stderr);
    return 1;
  }

  //Load cert and key
  if (SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM) <= 0) {
    ERR_print_errors_fp(stderr);
    return 1;
  }
  if (SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM) <= 0) {
    ERR_print_errors_fp(stderr);
    return 1;
  }

  //Create socket
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
  bind(sock, (struct sockaddr*)&addr, sizeof(addr));
  listen(sock, 1);
  if (logging) {
    printf("Server listening on port %d\n", port);
  }

  while (1) {
    int client = accept(sock, NULL, NULL);
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, client);
    if (SSL_accept(ssl) <= 0) {
      ERR_print_errors_fp(stderr);
    } else {
      char buf[1024] = {0};
      SSL_read(ssl, buf, sizeof(buf) - 1);
      if (logging) {
        printf("Message recieved from client:\n%s\n", buf);
      }
    }
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(client);
  }

  close(sock);
  SSL_CTX_free(ctx);
  return 0;
}
