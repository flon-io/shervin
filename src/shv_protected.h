
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

// https://github.com/flon-io/shervin

#ifndef FLON_SHV_PROTECTED_H
#define FLON_SHV_PROTECTED_H

#include <netinet/in.h>
#include <ev.h>

#include "flutil.h"
#include "shervin.h"


//
// request

fshv_request *fshv_parse_request_head(char *s);

void fshv_request_free(fshv_request *r);
ssize_t fshv_request_content_length(fshv_request *r);

void fshv_handle(struct ev_loop *l, struct ev_io *eio);

int fshv_request_is_https(fshv_request *r);

//
// response

fshv_response *fshv_response_malloc(short status_code);
void fshv_response_free(fshv_response *r);

void fshv_respond(struct ev_loop *l, struct ev_io *eio);


//
// connection

typedef struct {

  struct sockaddr_in *client;
  long long startus;

  fshv_route **routes;

  flu_sbuffer *head;
  short hend;

  flu_sbuffer *body;
  size_t blen;

  ssize_t rqount;
  fshv_request *req;
  fshv_response *res;
} fshv_con;

fshv_con *fshv_con_malloc(struct sockaddr_in *client, fshv_route **routes);
void fshv_con_reset(fshv_con *c);
void fshv_con_free(fshv_con *c);


//
// uri

flu_dict *fshv_parse_uri(char *uri);
flu_dict *fshv_parse_host_and_path(char *host, char *path);

/* Renders the uri_d as an absolute URI. When ssl is set to 1, the
 * scheme will be "https://".
 */
char *fshv_absolute_uri(int ssl, flu_dict *uri_d, const char *rel, ...);

#define fshv_abs(ssl, uri_d) fshv_absolute_uri(ssl, uri_d, NULL)
#define fshv_rel(ssl, uri_d, ...) fshv_absolute_uri(ssl, uri_d, __VA_ARGS__)


//
// auth

typedef struct {
  char *sid;
  char *user;
  char *id;
  long long mtimeus; // microseconds
} fshv_session;

char *fshv_session_to_s(fshv_session *s);

// auth, default (memory) session store

flu_list *fshv_session_store();

fshv_session *fshv_session_add(
  const char *user, const char *id, const char *sid, long long nowus);

void fshv_session_store_reset();


//
// spec tools

fshv_request *fshv_parse_request_head_f(const char *s, ...);
int fshv_do_route(char *path, fshv_request *req);

#endif // FLON_SHV_PROTECTED_H

