
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

#include <shervin.h>

#include <stdlib.h>
#include <string.h>
//#include <unistd.h> // for close(fd)
#include <netinet/in.h>
#include <ev.h>

#include "flutil.h"

// TODO: logging

#define SHV_BUFFER_SIZE 2048


typedef struct shv_con {
  short state; // 0 created, 1 headers read, 2 body read
  shv_route **routes;
  shv_route *route;
  flu_sbuffer *src;
  shv_request req;
  shv_response res;
} shv_con;

static shv_con *shv_con_malloc(const shv_route **routes)
{
  shv_con *c = calloc(1, sizeof(shv_con));
  c->routes = routes;
  c->state = 0;
  c->src = flu_sbuffer_malloc();
  return c;
}

static void shv_con_free(shv_con *c)
{
  // TODO
}


static void shv_handle_cb(struct ev_loop *l, struct ev_io *eio, int revents)
{
printf("watcher: %p\n", eio);
  if (EV_ERROR & revents) { perror("read invalid event"); return; }

  // TODO: use a memstream
  char buffer[SHV_BUFFER_SIZE];

  ssize_t r = recv(eio->fd, buffer, SHV_BUFFER_SIZE, 0);

  if (r < 0) { /*perror("read error");*/ return; }

  if (r == 0)
  {
    ev_io_stop(l, eio);
    free(eio);
    perror("*** client closing ***");
    return;
  }

  printf("  >%s< %zu\n", buffer, strlen(buffer));

  //send(eio->fd, buffer, r, 0);
  //memset(buffer, 0, r);

  // TODO: read until the double crlf
  // TODO: handle the incoming requests
}

static void shv_accept_cb(struct ev_loop *l, struct ev_io *eio, int revents)
{
  struct sockaddr_in ca; // client address
  socklen_t cal = sizeof(struct sockaddr_in);

  struct ev_io *ceio = calloc(1, sizeof(struct ev_io));
  ceio->data = shv_con_malloc((const shv_route **)eio->data);

  if (EV_ERROR & revents) { /*perror("accept invalid event");*/ return; }

  int csd = accept(eio->fd, (struct sockaddr *)&ca, &cal);

  if (csd < 0) { /*perror("accept error");*/ return; }

  // client connected...

  ev_io_init(ceio, shv_handle_cb, csd, EV_READ);
  ev_io_start(l, ceio);
}

void shv_serve(int port, const shv_route **routes)
{
  struct ev_io eio;
  struct ev_loop *l = ev_default_loop(0);

  int sd = socket(PF_INET, SOCK_STREAM, 0);

  if (sd < 0) { perror("socket error"); exit(1); }

  struct sockaddr_in a;
  memset(&a, 0, sizeof(struct sockaddr_in));
  a.sin_family = AF_INET;
  a.sin_port = htons(port);
  a.sin_addr.s_addr = INADDR_ANY;

  int r;

  r = bind(sd, (struct sockaddr *)&a, sizeof(struct sockaddr_in));
  if (r != 0) { perror("bind error"); exit(2); }

  r = listen(sd, 2);
  if (r < 0) { perror("listen error"); exit(3); }

  ev_io_init(&eio, shv_accept_cb, sd, EV_READ);
  eio.data = routes;
  ev_io_start(l, &eio);

  ev_loop(l, 0);

  //printf("closing...\n");
  //r = close(sd);
  //if (r != 0) { perror("close error"); /*exit(4);*/ }
}

