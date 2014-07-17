
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

#define _POSIX_C_SOURCE 200809L

#include "aabro.h"


abr_parser *request_parser = NULL;

void shv_init_parser()
{
  if (request_parser != NULL) return;

  abr_parser *sp = abr_string(" ");
  abr_parser *crlf = abr_string("\r\n");
  abr_parser *lws = abr_regex("\r\n[ \t]*");

  abr_parser *token =
    abr_regex("[^\(\)<>@,;:\\\"\/\[\]\?=\{\} \t]+");

  abr_parser *method =
    abr_n_alt(
      "method",
      abr_string("OPTIONS"), abr_string("GET"), abr_string("HEAD"),
      abr_string("POST"), abr_string("PUT"), abr_string("DELETE"),
      abr_string("TRACE"), abr_string("CONNECT"),
      abr_name("extension_method", token),
      NULL);
  abr_parser *request_uri =
    NULL;
  abr_parser *http_version =
    abr_n_regex("http_version", "^HTTP\/\d+\.\d+");

  abr_parser *request_line =
    abr_seq(method, sp, request_uri, sp, http_version, crlf, NULL);

  abr_parser *field_name = abr_name("field_name", token);
  abr_parser *field_value = NULL;

  abr_parser *message_header =
    abr_seq(field_name, abr_string(":"), field_value, NULL);

  abr_parser *message_body =
    NULL;

  request_parser =
    abr_seq(
      request_line,
      abr_rep(
        abr_seq(message_header, crlf, NULL),
        0, -1),
      crlf,
      abr_rep(message_body, 0, 1),
      NULL);
}

shv_request *shv_parse_request(char *s)
{
  shv_init_parser();

  shv_request *req = calloc(1, sizeof(shv_request));

  return req;
}

void shv_request_free(shv_request *r)
{
  free(r->source);
  free(r);
}

