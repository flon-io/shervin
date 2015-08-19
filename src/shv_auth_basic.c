
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


#include "shv_protected.h"


int fshv_basic_auth(fshv_env *env, fshv_basic_authentifier *a)
{
  int authentified = 0;
  char *user = NULL;

  char *auth = flu_list_get(env->req->headers, "authorization");
  if (auth == NULL) goto _over;

  printf("auth >%s<\n", auth);

  if (strncmp(auth, "Basic ", 6) != 0) goto _over;

  user = flu64_decode(auth + 6, -1);
  char *pass = strchr(user, ':');
  if (pass == NULL) goto _over;

  *pass = 0; pass = pass + 1;

  printf("user >%s< pass >%s<\n", user, pass);

_over:

  free(user);

  return authentified;
}

  // shervin 1.0.0
  //
//int fshv_basic_auth_filter(
//  fshv_request *req, fshv_response *res, int mode, flu_dict *params)
//{
//  int authentified = 0;
//  char *user = NULL;
//
//  if (params == NULL) goto _over;
//
//  if (flu_list_get(req->uri_d, "logout")) goto _over;
//    // /?logout logs out...
//
//  char *auth = flu_list_get(req->headers, "authorization");
//  if (auth == NULL) goto _over;
//
//  if (strncmp(auth, "Basic ", 6) != 0) goto _over;
//
//  user = flu64_decode(auth + 6, -1);
//  char *pass = strchr(user, ':');
//  if (pass == NULL) goto _over;
//
//  *pass = 0; pass = pass + 1;
//
//  fshv_authenticate *a = flu_list_get(params, "func");
//  if (a == NULL) a = flu_list_get(params, "a");
//  if (a == NULL) a = no_auth;
//
//  if (a(user, pass, req, params) == 0) goto _over;
//
//  authentified = 1;
//  fshv_set_user(req, "basic", user);
//
//_over:
//
//  if ( ! authentified)
//  {
//    flu_list_set(
//      res->headers,
//      "WWW-Authenticate",
//      flu_sprintf(
//        "Basic realm=\"%s\"", flu_list_getd(params, "realm", "shervin")));
//
//    res->status_code = 401;
//  }
//
//  free(user);
//
//  return 0;
//}
