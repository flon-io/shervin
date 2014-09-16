
//
// Copyright (c) 2013-2014, John Mettraux, jmettraux+flon@gmail.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// Made in Japan.
//

#ifndef FLON_SHV_PROTECTED_H
#define FLON_SHV_PROTECTED_H

#include <netinet/in.h>
#include <ev.h>

#include "flutil.h"
#include "shervin.h"


//
// request

shv_request *shv_parse_request(char *s);
void shv_request_free(shv_request *r);
ssize_t shv_request_content_length(shv_request *r);

char shv_method_to_char(char *s);
char *shv_char_to_method(char c);

flu_dict *shv_parse_uri(char *uri);

//
// response

shv_response *shv_response_malloc(short status_code);
void shv_response_free(shv_response *r);

void shv_respond(short status_code, struct ev_loop *l, struct ev_io *eio);


//
// connection

typedef struct shv_con {

  struct sockaddr_in *client;
  long long startMs;

  shv_route **routes;

  flu_sbuffer *head;
  short hend;

  flu_sbuffer *body;
  size_t blen;

  ssize_t rqount;
  shv_request *req;
  shv_response *res;
} shv_con;

shv_con *shv_con_malloc(struct sockaddr_in *client, shv_route **routes);
void shv_con_reset(shv_con *c);
void shv_con_free(shv_con *c);

#endif // FLON_SHV_PROTECTED_H

