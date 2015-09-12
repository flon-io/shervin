
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

#define _POSIX_C_SOURCE 200809L

//#include <stdlib.h>
#include <string.h>

//#include "flutil.h"
#include "gajeta.h"
#include "shervin.h"
//#include "shv_protected.h"


//static int login_handler(
//  fshv_request *req, fshv_response *res, int mode, flu_dict *params)
//{
//  res->status_code = 401;
//
//  if (req->body == NULL) return 1;
//
//  // user=x;pass=y
//  char *u = strchr(req->body, '=');
//
//  if (u == NULL)
//  {
//    char *csid = strdup(flu_list_getd(req->headers, "cookie", ""));
//    fshv_stop_session(req, res, params, strchr(csid, '=') + 1);
//    free(csid);
//
//    res->status_code = 200;
//    return 1;
//  }
//
//  char *ue = strchr(u, ';');
//  char *p = strchr(ue, '=');
//
//  if (strncmp(u, p, ue - u) != 0) return 1;
//
//  u = strndup(u + 1, ue - u - 1);
//  fshv_start_session(req, res, params, u);
//  free(u);
//
//  res->status_code = 200;
//  return 1;
//}

static int hello(fshv_env *env)
{
  env->res->status_code = 200;

  flu_list_add(
    env->res->body,
    flu_sprintf("hello %s\n", flu_list_get(env->bag, "name")));

  return 1;
}

static int redir(fshv_env *env)
{
  env->res->status_code = 303;

  flu_list_set(env->res->headers, "location", strdup("/somewhere/else"));
  flu_list_add(env->res->body, strdup(""));

  return 1;
}

static int secret(fshv_env *env)
{
  if ( ! fshv_get_user(env, NULL)) return fshv_status(env, 401);

  env->res->status_code = 200;

  flu_list_add( env->res->body, strdup("there are no secrets."));

  return 1;
}

static char *bauth(
  fshv_env *e, const char *realm, const char *user, const char *pass)
{
  if (strcmp(user, pass) == 0) return strdup(user);
  return NULL;
}

static int root(fshv_env *env)
{
  // public zone

  if (fshv_m(env, "/mirror")) return fshv_mirror(env, 1);
  if (fshv_m(env, "/hello/:name")) return hello(env);
  if (fshv_m(env, "/redir")) return redir(env);
  if (fshv_m(env, "/files/**")) return fshv_serve_files(env, "../spec/www");

  // 'private' zone and friends

  //if ( ! fshv_basic_auth(env, "tst_realm", bauth)) return 1;
  //if (fshv_m(env, "GET /secret")) return secret(env);
    // OR
  fshv_basic_auth(env, "tst_realm", bauth);
  if (fshv_m(env, "GET /secret")) return secret(env);

  //if (fshv_m(env, "POST /login")) return login(env);
  //if ( ! fshv_session_auth_filter(env)) return fshv_status(env, 401);

  return 0;
    // OR
  //return fshv_status(env, 404);
}

int main()
{
  fgaj_conf_get()->logger = fgaj_grey_logger;
  //fgaj_conf_get()->level = 5;
  fgaj_conf_get()->out = stderr;
  fgaj_conf_get()->params = "5p";

  fshv_serve(4001, root, NULL);
}

