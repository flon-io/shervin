
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

//#include <unistd.h>
#include <stdlib.h>
//#include <ctype.h>
//#include <string.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <errno.h>
//#include <ev.h>

#include "flutil.h"
//#include "flutim.h"
//#include "gajeta.h"
//#include "shervin.h"
#include "shv_protected.h"


//
// fshv_response

//fshv_response *fshv_response_malloc(short status_code)
//{
//  fshv_response *r = calloc(1, sizeof(fshv_response));
//  r->status_code = status_code;
//  r->headers = flu_list_malloc();
//  r->body = flu_list_malloc();
//
//  return r;
//}

void fshv_response_free(fshv_response *r)
{
  if (r == NULL) return;

  flu_list_free_all(r->headers);
  flu_list_free_all(r->body);
  free(r);
}

