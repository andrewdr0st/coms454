#include <stdio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

int main(int argc, char* argv[]) {
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

  return 0;
}
