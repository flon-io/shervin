
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

#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "flutil.h"
#include "aabro.h"
#include "shervin.h"
#include "shv_protected.h"

#include "gajeta.h"


abr_parser *request_parser = NULL;
abr_parser *uri_parser = NULL;

void shv_init_parser()
{
  abr_parser *sp = abr_string(" ");
  abr_parser *crlf = abr_string("\r\n");

  abr_parser *lws = abr_rex("(\r\n)?[ \t]+");

  //abr_parser *text =
  //  abr_alt(abr_rex("[^\x01-\x1F\x7F]"), lws, abr_r("+"));
  abr_parser *text =
    abr_rex("[^\x01-\x1F\x7F]+");

  abr_parser *token =
    abr_rex("[^\x01-\x1F\x7F()<>@,;:\\\\\"/[\\]?={} \t]+");

  abr_parser *method =
    abr_n_alt(
      "method",
      abr_rex("GET|POST|PUT|DELETE|HEAD|OPTIONS|TRACE|CONNECT|LINK|UNLINK"),
      abr_name("extension_method", token),
      NULL);
  abr_parser *request_uri =
    abr_n_rex("request_uri", "[^ \t\r\n]{1,2048}"); // arbitrary limit
  abr_parser *http_version =
    abr_n_rex("http_version", "HTTP/[0-9]+\\.[0-9]+");

  abr_parser *request_line =
    abr_seq(method, sp, request_uri, sp, http_version, crlf, NULL);

  abr_parser *field_content =
    text;

  abr_parser *field_name =
    abr_name("field_name", token);
  abr_parser *field_value =
    abr_n_rep("field_value", abr_alt(field_content, lws, NULL), 0, -1);

  abr_parser *message_header =
    abr_n_seq("message_header", field_name, abr_string(":"), field_value, NULL);

  //abr_parser *message_body =
  //  abr_n_regex("message_body", "^.+"); // well, the rest

  request_parser =
    abr_seq(
      request_line,
      abr_rep(
        abr_seq(message_header, crlf, NULL),
        0, -1),
      crlf,
      //abr_rep(message_body, 0, 1),
      NULL);
  // do not include the message_body

  //puts(abr_parser_to_string(request_parser));
}

shv_request *shv_parse_request(char *s)
{
  //
  // parse

  if (request_parser == NULL) shv_init_parser();

  abr_tree *r = abr_parse(s, 0, request_parser);
  //abr_tree *r = abr_parse_f(s, 0, request_parser, ABR_F_ALL);

  //puts(abr_tree_to_string_with_leaves(s, r));

  shv_request *req = calloc(1, sizeof(shv_request));
  req->startMs = flu_getMs();
  req->status_code = 400; // Bad Request

  if (r->result != 1) { abr_tree_free(r); return req; }

  req->status_code = 200; // ok, for now

  abr_tree *t = NULL;

  // method

  t = abr_tree_lookup(r, "method");
  req->method = shv_method_to_char(abr_tree_str(s, t));

  // uri

  t = abr_tree_lookup(r, "request_uri");
  req->uri = abr_tree_string(s, t);

  // version

  // reject when not 1.0 or 1.1?

  // headers

  flu_list *hs = abr_tree_list_named(r, "message_header");

  req->headers = flu_list_malloc();
  for (flu_node *h = hs->first; h != NULL; h = h->next)
  {
    abr_tree *th = (abr_tree *)h->item;
    abr_tree *tk = abr_tree_lookup(th, "field_name");
    abr_tree *tv = abr_tree_lookup(th, "field_value");
    char *sv = abr_tree_string(s, tv);
    flu_list_set(req->headers, abr_tree_string(s, tk), flu_strtrim(sv));
    free(sv);
  }

  flu_list_free(hs);

  //
  // over

  abr_tree_free(r);

  return req;
}

ssize_t shv_request_content_length(shv_request *r)
{
  char *cl = flu_list_get(r->headers, "content-length");

  return (cl == NULL) ? -1 : atol(cl);
}

void shv_request_free(shv_request *r)
{
  if (r->headers != NULL) flu_list_and_items_free(r->headers, free);
  if (r->uri != NULL) free(r->uri);
  free(r);
}

void shv_init_uri_parser()
{
  abr_parser *scheme =
    abr_n_rex("scheme", "https?");
  abr_parser *host =
    abr_n_rex("host", "[^:]+");
  abr_parser *port =
    abr_n_rex("port", ":[1-9][0-9]+");

  abr_parser *path =
    abr_n_rex("path", "[^\\?#]+");
  abr_parser *quentry =
    abr_n_seq("quentry",
      abr_n_rex("key", "[^=&#]"),
      abr_seq(abr_string("="), abr_n_rex("val", "[^&#]")), abr_q("?"),
      NULL);
  abr_parser *query =
    abr_n_seq("query",
      quentry,
      abr_seq(abr_string("&"), quentry), abr_q("*"),
      NULL);
  abr_parser *fragment =
    abr_rex(".+");

  abr_parser *shp =
    abr_seq(
      scheme,
      abr_string("://"),
      host,
      port, abr_q("?"),
      NULL);

  uri_parser =
    abr_seq(
      shp, abr_q("?"),
      path,
      abr_seq(abr_string("?"), query), abr_q("?"),
      abr_seq(abr_string("#"), fragment), abr_q("?"),
      NULL);
}

flu_dict *shv_parse_uri(char *uri)
{
  if (uri_parser == NULL) shv_init_uri_parser();

  abr_tree *r = abr_parse(uri, 0, uri_parser);
  abr_tree *t = NULL;

  //puts(abr_tree_to_string_with_leaves(uri, r));

  flu_dict *d = flu_list_malloc();

  t = abr_tree_lookup(r, "path");
  flu_list_set(d, "_path", abr_tree_string(uri, t));

  flu_list *l = abr_tree_list_named(r, "quentry");
  for (flu_node *n = l->first; n != NULL; n = n->next)
  {
    t = abr_tree_lookup((abr_tree *)n->item, "key");
    char *k = abr_tree_string(uri, t);

    t = abr_tree_lookup((abr_tree *)n->item, "val");
    char *v = abr_tree_string(uri, t);

    flu_list_set(d, k, v);
    free(k); // since flu_list_set() copies it
  }
  flu_list_free(l);

  return d;
}

