
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

#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

#include "flutim.h"
#include "shv_protected.h"


//
// request

char fshv_method_to_char(char *s)
{
  if (strncmp(s, "GET", 3) == 0) return 'g';
  if (strncmp(s, "PUT", 3) == 0) return 'u';
  if (strncmp(s, "POST", 4) == 0) return 'p';
  if (strncmp(s, "HEAD", 4) == 0) return 'h';
  if (strncmp(s, "TRACE", 5) == 0) return 't';
  if (strncmp(s, "DELETE", 6) == 0) return 'd';
  if (strncmp(s, "OPTIONS", 7) == 0) return 'o';
  if (strncmp(s, "CONNECT", 7) == 0) return 'c';
  return '?';
}

char *fshv_char_to_method(char c)
{
  if (c == 'g') return "GET";
  if (c == 'u') return "PUT";
  if (c == 'p') return "POST";
  if (c == 'h') return "HEAD";
  if (c == 't') return "TRACE";
  if (c == 'd') return "DELETE";
  if (c == 'o') return "OPTIONS";
  if (c == 'c') return "CONNECT";
  return "???";
}


//
// response


//
// connection

fshv_con *fshv_con_malloc(struct sockaddr_in *client, fshv_route **routes)
{
  fshv_con *c = calloc(1, sizeof(fshv_con));
  c->client = client;
  //c->startus = flu_gets('u');
  c->routes = routes;
  fshv_con_reset(c);
  c->rqount = -1;
  return c;
}

void fshv_con_reset(fshv_con *c)
{
  flu_sbuffer_free(c->head);
  c->head = NULL;
  c->hend = 0;

  flu_sbuffer_free(c->body);
  c->body = NULL;
  c->blen = 0;

  fshv_request_free(c->req);
  c->req = NULL;

  fshv_response_free(c->res);
  c->res = NULL;
}

void fshv_con_free(fshv_con *c)
{
  if (c == NULL) return;

  fshv_con_reset(c);
  free(c->client);
  free(c);
}


//
// auth

void fshv_set_user(fshv_request *req, const char *auth, const char *user)
{
  flu_list_setk(
    req->routing_d, flu_sprintf("_%s_user", auth), strdup(user), 0);
}

char *fshv_get_user(fshv_request *req, const char *auth)
{
  if (auth)
  {
    char *k = flu_sprintf("_%s_user", auth);
    char *r = flu_list_get(req->routing_d, k);
    free(k);

    return r;
  }

  for (flu_node *fn = req->routing_d->first; fn; fn = fn->next)
  {
    if (*fn->key != '_') continue;
    char *u = strrchr(fn->key, '_');
    if (u == NULL || strcmp(u, "_user") != 0) continue;

    return fn->item;
  }

  return NULL;
}

