#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define TIME_LEN 20

int get_ip(char ip[INET_ADDSTRLEN]);

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

  char ip[INET_ADDSTRLEN];
  if (get_ip(&ip) != 0) {
    if (logging) {
      fprintf(stderr, "Could not determine server's IP address\n");
    }
    return 1;
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

  //listen for clients
  while (1) {
    int client = accept(sock, NULL, NULL);
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, client);
    if (SSL_accept(ssl) <= 0) {
      ERR_print_errors_fp(stderr);
    } else {
      //read client message
      char buf[1024] = {0};
      int bytes = SSL_read(ssl, buf, sizeof(buf) - 1);
      if (bytes > 0) {
        if (logging) {
          printf("Message recieved from client:\n%s\n", buf);
        }
        int input = atoi(buf);
        if (input == 1) {
          //send time
          time_t now = time(NULL);
          struct tm *t = localtime(&now);
          char reply[TIME_LEN];
          strftime(reply, TIME_LEN, "%Y/%m/%d:%H:%M:%S", t);
          SSL_write(ssl, reply, TIME_LEN);
        } else if (input == 2) {
          //send ip
          SSL_write(ssl, ip, strlen(ip));
        }
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


//Establish a dummy connection to find IP, doesn't send any packets
int get_ip(char ip[INET_ADDSTRLEN]) {
  struct sockaddr_in serv;
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    return 1;
  }

  memset(&serv, 0, sizeof(serv));
  serv.sin_family = AF_INET;
  serv.sin_port = htons(80);
  inet_pton(AF_INET, "8.8.8.8", &serv.sin_addr);
  if (connect(sock, (struct sockaddr*)&serv, sizeof(serv)) < 0) {
    close(sock);
    return 1;
  }

  struct sockaddr_in name;
  socklen_t namelen = sizeof(name);
  if (getsockname(sock, (struct sockaddr*)&name, &namelen) == -1) {
    close(sock);
    return 1;
  }

  inet_ntop(AF_INET, &name.sin_addr, ip, sizeof(ip));
  close(sock);
  return 0;
}
