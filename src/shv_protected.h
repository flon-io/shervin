
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

#ifndef FLON_SHV_PROTECTED_H
#define FLON_SHV_PROTECTED_H

//#include <netinet/in.h>
//#include <ev.h>

#include "flutil.h"
#include "shervin.h"


//
// misc

char fshv_method_to_char(char *s);
char *fshv_char_to_method(char c);


//
// request

fshv_request *fshv_parse_request_head(char *s);

void fshv_request_free(fshv_request *r);
ssize_t fshv_request_content_length(fshv_request *r);

//void fshv_handle(struct ev_loop *l, struct ev_io *eio);

int fshv_request_is_https(fshv_request *r);


//
// uri

flu_dict *fshv_parse_uri(char *uri);
flu_dict *fshv_parse_host_and_path(char *host, char *path);

/* Renders the uri_d as an absolute URI. When ssl is set to 1, the
 * scheme will be "https://".
 */
char *fshv_absolute_uri(int ssl, flu_dict *uri_d, const char *rel, ...);

#define fshv_abs(ssl, uri_d) fshv_absolute_uri(ssl, uri_d, NULL)
#define fshv_rel(ssl, uri_d, ...) fshv_absolute_uri(ssl, uri_d, __VA_ARGS__)

#endif // FLON_SHV_PROTECTED_H

