
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

#include "flu64.h"
#include "shv_protected.h"


int fshv_basic_auth(
  fshv_env *env, const char *realm, fshv_user_pass_authentifier *a)
{
  int authentified = 0;
  char *user = NULL;

  char *auth = flu_list_get(env->req->headers, "authorization");
  if (auth == NULL) goto _over;

  if (strncmp(auth, "Basic ", 6) != 0) goto _over;

  user = flu64_decode(auth + 6, -1);
  char *pass = strchr(user, ':');
  if (pass == NULL) goto _over;

  *pass = 0; pass = pass + 1;

  char *nuser = a(realm, user, pass);
  authentified = (nuser != NULL);

  if (nuser) flu_list_set(env->bag, "_basic_user", nuser);
    // the authentifier is tasked with delivering a new string in nuser

_over:

  free(user);

  if ( ! authentified)
  {
    env->res->status_code = 401;
      // users of the auth are free to override that downstream

    if (realm)
    {
      flu_list_set(
        env->res->headers,
        "WWW-Authenticate", flu_sprintf("Basic realm=\"%s\"", realm));
    }
  }

  return authentified;
}

