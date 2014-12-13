
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

// auth handlers/filters

#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <string.h>

#include "flutil.h"
#include "flu64.h"
#include "shervin.h"
#include "shv_protected.h"


static int no_auth(const char *user, const char *path, flu_dict *params)
{
  return 0;
}


//
// basic authentication

int shv_basic_auth_filter(
  shv_request *req, shv_response *res, flu_dict *params)
{
  int r = 1;
  char *user = NULL;

  if (params == NULL) goto _over;

  if (flu_list_get(req->uri_d, "logout")) goto _over;
    // /?logout logs out...

  char *auth = flu_list_get(req->headers, "authorization");
  if (auth == NULL) goto _over;

  if (strncmp(auth, "Basic ", 6) != 0) goto _over;

  user = flu64_decode(auth + 6, -1);
  char *pass = strchr(user, ':');
  if (pass == NULL) goto _over;

  *pass = 0; pass = pass + 1;

  shv_authenticate *a = flu_list_get(params, "func");
  if (a == NULL) a = flu_list_get(params, "a");
  if (a == NULL) a = no_auth;

  if (a(user, pass, params) == 0) goto _over;

  r = 0; // success
  flu_list_set(req->routing_d, "_user", strdup(user));

_over:

  if (r == 1)
  {
    flu_list_set(
      res->headers,
      "WWW-Authenticate",
      flu_sprintf(
        "Basic realm=\"%s\"", flu_list_getd(params, "realm", "shervin")));

    res->status_code = 401;
  }

  free(user);

  return r;
}


//
// session (cookie) authentication

flu_dict *memstore;

void shv_sauth_memstore_add(const char *uname, const char *val)
{
  if (memstore == NULL) memstore = flu_list_malloc();

  flu_list_set(memstore, uname, strdup(val));
}

void shv_sauth_memstore_reset()
{
  flu_list_free_all(memstore); memstore = NULL;
}

char *shv_sauth_memstore_authenticate(const char *cookie)
{
}

int shv_session_auth_filter(
  shv_request *req, shv_response *res, flu_dict *params)
{
  int r = 1; // handled (do not got to the next route)

  char *cname = flu_list_get(params, "name");
  if (cname == NULL) cname = flu_list_get(params, "n");
  if (cname == NULL) cname = "flon.io.shervin";

  char *cookies = flu_list_get(req->headers, "cookie");
  if (cookies == NULL) goto _over;

  puts(cookies);

  char *c = NULL;
  for (char *cs = cookies; cs; cs = strchr(cs, ';'))
  {
    while (*cs == ';' || *cs == ' ') ++cs;

    char *eq = strchr(cs, '=');
    if (eq == NULL) break;

    //printf(">%s<\n", strndup(cs, eq - cs));
    if (strncmp(cs, cname, eq - cs) != 0) continue;

    char *eoc = strchr(eq + 1, ';');
    c = eoc ? strndup(eq + 1, eoc - eq - 1) : strdup(eq + 1);
    break;
  }

  printf("c >%s<\n", c);
  char *user = NULL;
  // TODO: authenticate

  free(c);

  if (user == NULL) goto _over;

  r = 0; // success
  flu_list_set(req->routing_d, "_user", strdup(user));

_over:

  return r;
}

