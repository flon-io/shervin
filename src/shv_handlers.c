
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

// handlers, and guards

#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <string.h>

#include "shervin.h"
#include "shv_protected.h"


//
// guards

flu_dict *shv_any_guard(
  shv_request *req, flu_dict *guard, shv_response *res, flu_dict *params)
{
  return flu_list_malloc(); // that's a yes
}

// IDEA:
//   "/book/:name" for any method
//   "GET /book/:name" to limit to GET

flu_dict *shv_path_guard(
  shv_request *req, flu_dict *guard, shv_response *res, flu_dict *params)
{
  flu_dict *r = flu_list_malloc();

  char *path = (char *)flu_list_get(params, "path");
  char *rpath = (char *)flu_list_get(req->uri_d, "_path");

  short success = 1;

  while (1)
  {
    char *slash = strchr(path, '/');
    char *rslash = strchr(rpath, '/');

    if (slash == NULL) slash = strchr(path, '\0');
    if (rslash == NULL) rslash = strchr(rpath, '\0');

    if (path[0] == ':')
    {
      char *k = strndup(path + 1, slash - path - 1);
      flu_list_set(r, k, strndup(rpath, rslash - rpath));
      free(k);
    }
    else
    {
      if (strncmp(path, rpath, slash - path) != 0) { success = 0; break; }
    }

    if (slash[0] == '\0' && rslash[0] == '\0') break;
    if (slash[0] == '\0' || rslash[0] == '\0') { success = 0; break; }

    path = slash + 1;
    rpath = rslash + 1;
  }

  if (success) return r;

  flu_list_free_all(r);
  return NULL;
}

//
// handlers

