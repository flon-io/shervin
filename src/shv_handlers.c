
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

// handlers

#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <string.h>

#include "flutil.h"
#include "flutim.h"
#include "gajeta.h"
#include "shervin.h"
#include "shv_protected.h"


int fshv_serve_files(fshv_env *env, char *root)
{
  char *p = flu_list_get(env->bag, "**");
  if (p == NULL) return 0;
  //{
    //char *path = (char *)flu_list_get(env->bag, "path");
    //char *rpath = (char *)flu_list_get(req->uri_d, "_path");

    //if (path && strstr(path, "**")) return 0;

    //char *s = (char *)flu_list_get(params, "start");
    //if (s == NULL) s = (char *)flu_list_get(params, "s");

    //if (s)
    //{
    //  size_t sl = strlen(s);
    //  if (strncmp(rpath, s, sl) != 0) return 0;
    //  p = rpath + sl;
    //}
    //else
    //{
    //  p = rpath;
    //}
  //}

  //fgaj_d("p: %s", p);

  if (strstr(p, "..")) return 0;

  char *path = flu_path("%s/%s", root, p);

  ssize_t s = fshv_serve_file(env, path);

  if (s < 0) { free(path); return 0; }

  if (s == 0 && flu_list_get(env->res->headers, "fshv_file") == NULL)
  {
    char *i = flu_list_getod(env->conf, "index", "index.html");
    flu_list *is = flu_split(i, ",");

    for (flu_node *n = is->first; n; n = n->next)
    {
      char *p = flu_path("%s/%s", path, (char *)n->item);
      s = fshv_serve_file(env, p);
      free(p);
      if (s > 0) break;
    }

    flu_list_free_all(is);
  }

  free(path);

  return (s > 0 || flu_list_get(env->res->headers, "fshv_file") != NULL);
}

/*
int fshv_debug_handler(
  fshv_request *req, fshv_response *res, int mode, flu_dict *params)
{
  res->status_code = 200;
  //flu_list_set(res->headers, "content-type", "text/plain; charset=utf-8");

  char *suri = flu_list_to_s(req->uri_d);

  // prepare response body

  flu_sbuffer *b = flu_sbuffer_malloc();

  flu_sbprintf(
    b, "%s %s HTTP/1.1\r\n", fshv_char_to_method(req->method), req->uri);

  for (flu_node *fn = req->headers->first; fn; fn = fn->next)
  {
    flu_sbprintf(b, "%s: %s\r\n", fn->key, fn->item);
  }
  flu_sbprintf(b, "method: %s\r\n", fshv_char_to_method(req->method));
  flu_sbprintf(b, "path: %s\r\n", req->uri);
  flu_sbprintf(b, "uri_d: %s\r\n", suri);
  flu_sbputs(b, "\r\n");
  if (req->body) flu_sbputs(b, req->body);

  flu_list_add(res->body, flu_sbuffer_to_string(b));

  // log request

  long long us = flu_gets('u') % 1000000;

  fgaj_d(
    "|%05x| %s %s HTTP/1.1\r\n",
    us, fshv_char_to_method(req->method), req->uri);
  fgaj_d(
    "|%05x| uri_d: %s",
    us, suri);

  for (flu_node *fn = req->headers->first; fn; fn = fn->next)
  {
    fgaj_d("|%05x|  * %s: \"%s\"", us, fn->key, fn->item);
  }

  ssize_t l = req->body ? strlen(req->body) : -1;
  size_t maxl = 35;
  if (l > maxl)
    fgaj_d("|%05x| body: >%.*s...< l%lli", us, maxl, req->body, l);
  else
    fgaj_d("|%05x| body: >%s< l%lli", us, req->body, l);

  // done

  free(suri);

  return 1;
}
*/

