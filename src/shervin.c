
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
#include <time.h>
//#include <unistd.h> // for close(fd)
#include <netinet/in.h>
#include <ev.h>

#include "flutil.h"
#include "shv_protected.h"

// TODO: logging

#define SHV_BUFFER_SIZE 2048


static void shv_close(struct ev_loop *l, struct ev_io *eio)
{
  ev_io_stop(l, eio);
  free(eio);
  perror("*** client closing ***");
  // TODO: free con
}

static char *shv_reason(short status_code)
{
  if (status_code == 200) return "OK";

  if (status_code == 400) return "Bad Request";
  if (status_code == 404) return "Not Found";

  if (status_code == 500) return "Internal Server Error";

  if (status_code == 100) return "Continue";
  if (status_code == 101) return "Switching Protocols";
  if (status_code == 201) return "Created";
  if (status_code == 202) return "Accepted";
  if (status_code == 203) return "Non-Authoritative Information";
  if (status_code == 204) return "No Content";
  if (status_code == 205) return "Reset Content";
  if (status_code == 206) return "Partial Content";
  if (status_code == 300) return "Multiple Choices";
  if (status_code == 301) return "Moved Permanently";
  if (status_code == 302) return "Found";
  if (status_code == 303) return "See Other";
  if (status_code == 304) return "Not Modified";
  if (status_code == 305) return "Use Proxy";
  if (status_code == 307) return "Temporary Redirect";
  if (status_code == 401) return "Unauthorized";
  if (status_code == 402) return "Payment Required";
  if (status_code == 403) return "Forbidden";
  if (status_code == 405) return "Method Not Allowed";
  if (status_code == 406) return "Not Acceptable";
  if (status_code == 407) return "Proxy Authentication Required";
  if (status_code == 408) return "Request Time-out";
  if (status_code == 409) return "Conflict";
  if (status_code == 410) return "Gone";
  if (status_code == 411) return "Length Required";
  if (status_code == 412) return "Precondition Failed";
  if (status_code == 413) return "Request Entity Too Large";
  if (status_code == 414) return "Request-URI Too Large";
  if (status_code == 415) return "Unsupported Media Type";
  if (status_code == 416) return "Requested range not satisfiable";
  if (status_code == 417) return "Expectation Failed";
  if (status_code == 501) return "Not Implemented";
  if (status_code == 502) return "Bad Gateway";
  if (status_code == 503) return "Service Unavailable";
  if (status_code == 504) return "Gateway Time-out";
  if (status_code == 505) return "HTTP Version not supported";
  return "(no reason-phrase)";
}

static void shv_respond(short status_code, struct ev_loop *l, struct ev_io *eio)
{
  shv_con *con = (shv_con *)eio->data;

  if (status_code == -1)
  {
    if (con->res) status_code = con->res->status_code;
    else status_code = con->req->status_code;
  }

  time_t tt; time(&tt);
  struct tm *tm; tm = gmtime(&tt);
  char *dt = asctime(tm); // TODO: upgrade to rfc1123

  char *ct = "text/plain; charset=utf-8";
  size_t cl = 1;
  char *lo = "northpole";
    //
    // FIXME

  flu_sbuffer *b = flu_sbuffer_malloc();
  flu_sbprintf(b, "HTTP/1.1 %i %s\r\n", status_code, shv_reason(status_code));
  flu_sbprintf(b, "server: shervin %s\r\n", SHV_VERSION);
  flu_sbprintf(b, "content-type: %s\r\n", ct);
  flu_sbprintf(b, "content-length: %zu\r\n", cl);
  flu_sbprintf(b, "date: %s\r\n", dt);
  flu_sbprintf(b, "location: %s\r\n", lo);
  flu_sbprintf(b, "\r\n");

  //free(dt); // not necessary

  flu_sbprintf(b, ".");

  flu_sbuffer_close(b);

  send(eio->fd, b->string, b->len, 0);

  flu_sbuffer_free(b);
  shv_con_free(con);
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

    char *head = flu_sbuffer_to_string(con->head);
    con->head = NULL;

    con->req = shv_parse_request(head);

    free(head);

    printf("con->req->status_code %i\n", con->req->status_code);

    if (con->req->status_code != 200)
    {
      shv_respond(-1, l, eio);
      return;
    }
  }

  printf("con->req content-length %zd\n", shv_request_content_length(con->req));

  if (
    (con->req->method == 'p' || con->req->method == 'u') &&
    (con->blen < shv_request_content_length(con->req))
  ) return; // request body not yet complete

  for (i = 0; ; ++i)
  {
    shv_route *route = con->routes[i];

    if (route == NULL) break;
    if (route->guard(con->req, route->params) != 1) continue;

    con->res = shv_response_malloc(-1);
    route->handler(con->req, con->res, route->params);
    shv_respond(-1, l, eio);
    return;
  }

  shv_respond(404, l, eio);
}

static void shv_accept_cb(struct ev_loop *l, struct ev_io *eio, int revents)
{
  struct sockaddr_in ca; // client address
  socklen_t cal = sizeof(struct sockaddr_in);

  struct ev_io *ceio = calloc(1, sizeof(struct ev_io));
  ceio->data = shv_con_malloc((shv_route **)eio->data);

  if (EV_ERROR & revents) { /*perror("accept invalid event");*/ return; }

  int csd = accept(eio->fd, (struct sockaddr *)&ca, &cal);

  if (csd < 0) { /*perror("accept error");*/ return; }

  // client connected...

  ev_io_init(ceio, shv_handle_cb, csd, EV_READ);
  ev_io_start(l, ceio);
}

void shv_serve(int port, shv_route **routes)
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

