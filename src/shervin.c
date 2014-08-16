
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

  shv_route **routes;
  shv_route *route;

  flu_sbuffer *head;
  short hend;

  flu_sbuffer *body;
  size_t blen;

  shv_request *req;
  shv_response *res;
} shv_con;

static shv_con *shv_con_malloc(const shv_route **routes)
{
  shv_con *c = calloc(1, sizeof(shv_con));
  c->routes = routes;
  c->head = flu_sbuffer_malloc();
  c->hend = 0;
  //c->body = NULL;
  c->blen = 0;
  return c;
}

static void shv_con_free(shv_con *c)
{
  // TODO
}

static void shv_close(struct ev_loop *l, struct ev_io *eio)
{
  ev_io_stop(l, eio);
  free(eio);
  perror("*** client closing ***");
  // TODO: free con
}

static void shv_respond(struct ev_loop *l, struct ev_io *eio)
{
  //send(eio->fd, buffer, r, 0);
}

static void shv_handle_cb(struct ev_loop *l, struct ev_io *eio, int revents)
{
  if (EV_ERROR & revents) { perror("read invalid event"); return; }

  shv_con *con = (shv_con *)eio->data;

  char buffer[SHV_BUFFER_SIZE];

  ssize_t r = recv(eio->fd, buffer, SHV_BUFFER_SIZE, 0);

  if (r < 0) { perror("read error"); return; }
  if (r == 0) { shv_close(l, eio); return; }

  printf("in >%s<\n", buffer);

  ssize_t i = -1;
  if (con->hend < 4) for (i = 0; i < r; ++i)
  {
    if (con->hend == 4) break; // head found

    if (
      ((con->hend == 0 || con->hend == 2) && buffer[i] == '\r') ||
      ((con->hend == 1 || con->hend == 3) && buffer[i] == '\n')
    ) ++con->hend; else con->hend = 0;
  }

  printf("i %zu, con->hend %i\n", i, con->hend);

  if (i < 0)
  {
    flu_sbwrite(con->body, buffer, r);
    con->blen += r;
  }
  else
  {
    flu_sbwrite(con->head, buffer, i + 1);
    con->body = flu_sbuffer_malloc();
    flu_sbwrite(con->body, buffer + i, r - i);
    con->blen = r - i;
  }

  printf("con->blen %zu\n", con->blen);

  if (con->req == NULL)
  {
    if (con->hend < 4) return;
      // end of head not yet found

    flu_sbuffer_close(con->head);

    con->req = shv_parse_request(con->head->string);

    printf("con->req->status_code %i\n", con->req->status_code);
    printf("con->req content-length %zu\n", shv_request_content_length(con->req));

    if (con->req->status_code != 200)
    {
      con->res = shv_response_malloc(con->req->status_code);
      shv_respond(l, eio);
      return;
    }

    return;
  }
  // else we have a req, probably reading the body

  // TODO...
  //if (con->req != NULL)
  //{
  //  if (shv_request_content_length(con->req) == con->blen)
  //  {
  //    printf("@@@\nbody >%s<\n@.@\n", flu_sbuffer_to_string(con->body));
  //  }
  //}
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

  printf("+++ serving +++\n");

  ev_loop(l, 0);

  //printf("closing...\n");
  //r = close(sd);
  //if (r != 0) { perror("close error"); /*exit(4);*/ }
}

