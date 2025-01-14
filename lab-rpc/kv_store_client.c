/**
 * Client binary.
 */

#include "kv_store_client.h"
#include <stdlib.h>
#include <string.h>

#define HOST "localhost"

CLIENT* clnt_connect(char* host) {
  CLIENT* clnt = clnt_create(host, KVSTORE, KVSTORE_V1, "udp");
  if (clnt == NULL) {
    clnt_pcreateerror(host);
    exit(1);
  }
  return clnt;
}

int example(int input) {
  CLIENT* clnt = clnt_connect(HOST);

  int ret;
  int* result;

  result = example_1(&input, clnt);
  if (result == (int*)NULL) {
    clnt_perror(clnt, "call failed");
    exit(1);
  }
  ret = *result;
  xdr_free((xdrproc_t)xdr_int, (char*)result);

  clnt_destroy(clnt);

  return ret;
}

char* echo(char* input) {
  CLIENT* clnt = clnt_connect(HOST);

  char* ret;
  char** result;

  result = echo_1(&input, clnt);
  if (result == (char**)NULL) {
    clnt_perror(clnt, "call failed");
    exit(1);
  }
  ret = strdup(*result);
  xdr_free((xdrproc_t)xdr_int, (char*)result);

  clnt_destroy(clnt);

  return ret;
}

void put(buf key, buf value) {
  CLIENT* clnt = clnt_connect(HOST);

  void* result;
  put_request* request = (put_request*)malloc(sizeof(put_request));
  if (request == NULL) {
    perror("malloc failed");
    exit(1);
  }

  request->key = key;
  request->value = value;

  result = put_1(request, clnt);
  if (result == (void*)NULL) {
    clnt_perror(clnt, "call failed");
    free(request); // 释放 request
    clnt_destroy(clnt);
    exit(1);
  }

  xdr_free((xdrproc_t)xdr_int, (char*)result);
  free(request);
  clnt_destroy(clnt);
}

buf* get(buf key) {
  CLIENT* clnt = clnt_connect(HOST);

  buf* ret = (buf*)malloc(sizeof(buf));
  buf* result;

  result = get_1(&key, clnt);
  if (result == (buf*)NULL) {
    clnt_perror(clnt, "call failed");
    exit(1);
  }
  ret->buf_len = result->buf_len;
  ret->buf_val = (char*)malloc(result->buf_len);
  memcpy(ret->buf_val, result->buf_val, result->buf_len);

  xdr_free((xdrproc_t)xdr_int, (char*)result);

  clnt_destroy(clnt);

  return ret;
}
