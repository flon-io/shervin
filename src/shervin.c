
//
// Copyright (c) 2013-2015, John Mettraux, jmettraux+flon@gmail.com
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

#include <ev.h>

#include "gajeta.h"
#include "shervin.h"
#include "shv_protected.h"


static ssize_t subjecter(
  char *buffer, size_t off,
  const char *file, int line, const char *func, const void *subject)
{
  size_t ooff = off;
  size_t rem = fgaj_conf_get()->subject_maxlen - off;
  int w = 0;

  if (subject)
  {
    struct ev_io *eio = (struct ev_io *)subject;
    w = snprintf(buffer + off, rem, "i%p d%i ", eio, eio->fd);
    if (w < 0) return -1; off += w; rem -= w;

    fshv_con *con = eio->data; if (con)
    {
      w = snprintf(buffer + off, rem, "c%p rq%li ", con, con->rqount);
      if (w < 0) return -1; off += w; rem -= w;

      fshv_request *req = con->req; if (req)
      {
        char *met = fshv_char_to_method(req->method);
        w = snprintf(buffer + off, rem, "%s %s ", met, req->uri);
        if (w < 0) return -1; off += w; rem -= w;
      }
    }
  }

  off += fgaj_default_subjecter(buffer, off, file, line, func, NULL);

  return off - ooff;
}

void fshv_serve(int port, fshv_handler *root_handler, flu_dict *conf)
{
  fgaj_conf_get()->subjecter = subjecter;

  int r;

  struct ev_io *eio = calloc(1, sizeof(struct ev_io));
  struct ev_loop *l = ev_default_loop(0);

  fgaj_dr("preparing");

  int sd = socket(PF_INET, SOCK_STREAM, 0);
  if (sd < 0) { fgaj_r("socket error"); exit(1); }
  fgaj_dr("prepared socket"); errno = 0;

  int v = 1; r = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v));
  if (r != 0) { fgaj_r("couldn't set SO_REUSEADDR"); exit(1); }
  fgaj_dr("set SO_REUSEADDR"); errno = 0;

  r = fcntl(sd, F_GETFL);
  if (r == -1) { fgaj_r("couldn't read main socket flags"); exit(1); }
  fgaj_dr("read socket flags"); errno = 0;

  r = fcntl(sd, F_SETFL, r | O_NONBLOCK);
  if (r != 0) { fgaj_r("couldn't set main socket to O_NONBLOCK"); exit(1); }
  fgaj_dr("set socket to O_NONBLOCK"); errno = 0;

  struct sockaddr_in a;
  memset(&a, 0, sizeof(struct sockaddr_in));
  a.sin_family = AF_INET;
  a.sin_port = htons(port);
  a.sin_addr.s_addr = INADDR_ANY;

  r = bind(sd, (struct sockaddr *)&a, sizeof(struct sockaddr_in));
  if (r != 0) { fgaj_r("bind error"); exit(2); }
  fgaj_dr("bound");

  r = listen(sd, 2);
  if (r < 0) { fgaj_r("listen error"); exit(3); }
  fgaj_dr("listening");

  ev_io_init(eio, fshv_accept_cb, sd, EV_READ);
  eio->data = routes;
  ev_io_start(l, eio);

  fgaj_i("serving on %d...", port);

  ev_loop(l, 0);

  //fgaj_i("closing...");
  //r = close(sd);
  //if (r != 0) { fgaj_r("close error"); /*exit(4);*/ }
}

