
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

// shervin.h

#ifndef FLON_SHERVIN_H
#define FLON_SHERVIN_H

#define SHV_VERSION "1.0.0"

// request

typedef struct shv_request {
  char method;
  char *uri;
  char **headers;
  char *body;
  short status_code; // 4xx code set by shervin, 200 else
} shv_request;

char *shv_request_header(shv_request *r, char *header_name);

// response

typedef struct shv_response {
  short status_code; // 200, 404, 500, ...
  char *body;
} shv_response;

// route

typedef int shv_guard(shv_request *req, void **params);
typedef void shv_handler(shv_request *req, shv_response *res, void **params);

typedef struct shv_route {
  shv_guard *guard;
  shv_handler *handler;
  void **params;
} shv_route;

// guards

int shv_any_guard(shv_request *req, void **params);
int shv_path_guard(shv_request *req, void **params);

// handlers

void shv_dir_handler(shv_request *req, shv_response *res, void **params);

// serving

void shv_serve(int port, shv_route **routes);
  // NULL terminated route array

// TODO: shv_stop_serve?

#endif // FLON_SHERVIN_H

