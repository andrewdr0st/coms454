#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

int port = 4443;

int main(int argc, char* argv[]) {
  //Read command line args
  int opt;
  while ((opt = getopt(argc, argv, "p:")) != -1) {
    switch (opt) {
      case 'p': //set port
        port = atoi(optarg);
        break;
      default:
        fprintf(stderr, "Usage: ./client [-p portnum]\n");
        return 1;
    }
  }

  //Init SSL context
  SSL_load_error_strings();
  const SSL_METHOD *method = TLS_client_method();
  SSL_CTX *ctx = SSL_CTX_new(method);
  if (!ctx) {
    ERR_print_errors_fp(stderr);
    return 1;
  }

  //Get user input
  int input = 0;
  printf("Type 1 to ask server for the current time\nType 2 to ask server for the IP of the localhost\n");
  scanf("%d", &input);
  if (input != 1 && input != 2) {
    fprintf(stderr, "You muest enter 1 or 2\n");
    return 1;
  }

  //Connect to server
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);
  connect(sock, (struct sockaddr*)&server, sizeof(server));
  SSL *ssl = SSL_new(ctx);
  SSL_set_fd(ssl, sock);
  if (SSL_connect(ssl) <= 0) {
    ERR_print_errors_fp(stderr);
  } else {
    //send input to server
    SSL_write(ssl, input, sizeof(int));
  }

  SSL_free(ssl);
  close(sock);
  SSL_CTX_free(ctx);
  return 0;
}
