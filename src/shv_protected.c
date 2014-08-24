
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

#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>

#include "shv_protected.h"


shv_con *shv_con_malloc(shv_route **routes)
{
  shv_con *c = calloc(1, sizeof(shv_con));
  c->routes = routes;
  shv_con_reset(c);
  return c;
}

static void shv_con_free_members(shv_con *c)
{
  if (c->head) flu_sbuffer_free(c->head);
  if (c->body) flu_sbuffer_free(c->body);
  if (c->req) shv_request_free(c->req);
  if (c->res) shv_response_free(c->res);
}

void shv_con_free(shv_con *c)
{
  shv_con_free_members(c);
  free(c);
}

void shv_con_reset(shv_con *c)
{
  shv_con_free_members(c);
  c->head = flu_sbuffer_malloc();
  c->hend = 0;
  c->body = NULL;
  c->blen = 0;
}

