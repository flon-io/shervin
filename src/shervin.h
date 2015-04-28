
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

// shervin.h

#ifndef FLON_SHERVIN_H
#define FLON_SHERVIN_H

#include "flutil.h"


#define FSHV_VERSION "1.1.0"

#define FSHV_BUFFER_SIZE 4096


// request

typedef struct {
  long long startus; // microseconds since the Epoch
  char method;
  char *uri;
  flu_dict *uri_d;
  flu_dict *headers;
  char *body;
} fshv_request;

// response

typedef struct {
  short status_code; // 200, 404, 500, ...
  flu_dict *headers;
  flu_list *body;
} fshv_response;

// env

typedef struct {
  flu_dict *conf; // application level conf
  fshv_request *req;
  fshv_response *res;
  flu_dict *bag; // req-res scoped databag
} fshv_env;

// [root] handler

typedef int fshv_handler(fshv_env *env);

// handlers

int fshv_dir_handler(fshv_env *env, char *root);

// guards

int fshv_match(fshv_env *env, char *route);

// serve

void fshv_serve(int port, fshv_handler *root_handler, flu_dict *conf);
  // interfaces?

#endif // FLON_SHERVIN_H

