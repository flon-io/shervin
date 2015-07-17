
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

#include "flutil.h"
#include "aabro.h"
#include "shv_protected.h"


static fabr_tree *_amp(fabr_input *i) { return fabr_str(NULL, i, "&"); }
static fabr_tree *_qmark(fabr_input *i) { return fabr_str(NULL, i, "?"); }
static fabr_tree *_equal(fabr_input *i) { return fabr_str(NULL, i, "="); }
static fabr_tree *_sharp(fabr_input *i) { return fabr_rex(NULL, i, "#"); }
static fabr_tree *_colslasla(fabr_input *i) { return fabr_str(NULL, i, "://"); }
static fabr_tree *_colon(fabr_input *i) { return fabr_str(NULL, i, ":"); }

static fabr_tree *_qkey(fabr_input *i)
{ return fabr_rex("qkey", i, "[^ \t=&#]+"); }

static fabr_tree *_val(fabr_input *i)
{ return fabr_rex("qval", i, "[^ \t&#]+"); }

static fabr_tree *_qval(fabr_input *i)
{ return fabr_seq(NULL, i, _equal, _val, NULL); }

static fabr_tree *_qentry(fabr_input *i)
{ return fabr_seq("qentry", i, _qkey, _qval, fabr_qmark, NULL); }

static fabr_tree *_query(fabr_input *i)
{ return fabr_eseq(NULL, i, _qmark, _qentry, _amp, NULL); }

static fabr_tree *_ragment(fabr_input *i)
{ return fabr_rex("fragment", i, ".+"); }

static fabr_tree *_fragment(fabr_input *i)
{ return fabr_seq(NULL, i, _sharp, _ragment, NULL); }

static fabr_tree *_path(fabr_input *i)
{ return fabr_rex("path", i, "[^\\?#]+"); }

static fabr_tree *_scheme(fabr_input *i)
{ return fabr_rex("scheme", i, "https?"); }

static fabr_tree *_host(fabr_input *i)
{ return fabr_rex("host", i, "[^:/]+"); }

static fabr_tree *_ort(fabr_input *i)
{ return fabr_rex("port", i, "[1-9][0-9]*"); }

static fabr_tree *_port(fabr_input *i)
{ return fabr_seq(NULL, i, _colon, _ort, NULL); }

static fabr_tree *_shp(fabr_input *i)
{ return fabr_seq(NULL, i,
    _scheme, _colslasla, _host, _port, fabr_qmark,
    NULL); }

static fabr_tree *_uri(fabr_input *i)
{ return fabr_seq(NULL, i,
    _shp, fabr_qmark, _path, _query, fabr_qmark, _fragment, fabr_qmark,
    NULL); }

flu_dict *fshv_parse_uri(char *uri)
{
  printf("fshv_parse_uri() >[1;33m%s[0;0m<\n", uri);

  //fabr_tree *tt = fabr_parse_f(uri, _uri, FABR_F_ALL);
  //printf("fshv_parse_uri():\n"); fabr_puts_tree(tt, uri, 1);
  //fabr_tree_free(tt);

  fabr_tree *r = fabr_parse_all(uri, _uri);
  //printf("fshv_parse_uri() (pruned):\n"); fabr_puts(r, uri, 3);

  fabr_tree *t = NULL;

  flu_dict *d = flu_list_malloc();

  t = fabr_tree_lookup(r, "scheme");
  if (t != NULL) flu_list_set(d, "_scheme", fabr_tree_string(uri, t));
  t = fabr_tree_lookup(r, "host");
  if (t != NULL) flu_list_set(d, "_host", fabr_tree_string(uri, t));
  t = fabr_tree_lookup(r, "port");
  if (t != NULL) flu_list_set(d, "_port", fabr_tree_string(uri, t));

  t = fabr_tree_lookup(r, "path");
  flu_list_set(d, "_path", fabr_tree_string(uri, t));
  //printf("_path >%s<\n", flu_list_get(d, "_path"));

  t = fabr_tree_lookup(r, "query");
  if (t) flu_list_set(d, "_query", fabr_tree_string(uri, t));

  flu_list *l = fabr_tree_list_named(r, "qentry");
  for (flu_node *n = l->first; n != NULL; n = n->next)
  {
    t = fabr_tree_lookup((fabr_tree *)n->item, "qkey");
    char *k = fabr_tree_string(uri, t);

    char *v = NULL; char *vv = NULL;
    t = fabr_tree_lookup((fabr_tree *)n->item, "qval");
    if (t) { v = fabr_tree_string(uri, t); vv = flu_urldecode(v, -1); }
    else { vv = strdup(""); }

    flu_list_set(d, k, vv);

    free(k); // since flu_list_set() copies it
    free(v);
  }
  flu_list_free(l);

  t = fabr_tree_lookup(r, "fragment");
  if (t != NULL) flu_list_set(d, "_fragment", fabr_tree_string(uri, t));

  fabr_tree_free(r);

  return d;
}

flu_dict *fshv_parse_host_and_path(char *host, char *path)
{
  if (host == NULL) return fshv_parse_uri(path);

  char *s = NULL;

  if (strncmp(host, "http://", 7) == 0 || strncmp(host, "https://", 8) == 0)
    s = flu_sprintf("%s%s", host, path);
  else
    s = flu_sprintf("http://%s%s", host, path);

  flu_dict *d = fshv_parse_uri(s);

  free(s);

  return d;
}

char *fshv_absolute_uri(int ssl, flu_dict *uri_d, const char *rel, ...)
{
  //for (flu_node *n = uri_d->first; n; n = n->next)
  //  printf("      * %s: %s\n", n->key, (char *)n->item);

  char *s = NULL;

  char *scheme = flu_list_getd(uri_d, "_scheme", "http");
  if (ssl) scheme = "https";

  char *port = "";
  s = flu_list_get(uri_d, "_port");
  if (s) port = flu_sprintf(":%s", s);

  char *frag = "";
  s = flu_list_get(uri_d, "_fragment");
  if (s) frag = flu_sprintf("#%s", s);

  char *query = "";
  s = flu_list_get(uri_d, "_query");
  if (s) query = flu_sprintf("?%s", s);

  char *path = strdup(flu_list_getd(uri_d, "_path", "/"));

  char *rl = NULL;
  if (rel)
  {
    va_list ap; va_start(ap, rel);
    rl = flu_svprintf(rel, ap);
    va_end(ap);
  }

  if (rl && *rl == '/')
  {
    free(path);
    path = rl;
  }
  else if (rl)
  {
    char *end = strrchr(path, '/');
    if (strchr(end, '.')) *end = 0;
    s = flu_canopath("%s/%s", path, rl);
    free(path);
    free(rl);
    path = s;
  }

  s = flu_sprintf(
    "%s://%s%s%s%s%s",
    scheme,
    flu_list_getd(uri_d, "_host", "127.0.0.1"),
    port,
    path,
    query,
    frag);

  if (*port != 0) free(port);
  if (*query != 0) free(query);
  if (*frag != 0) free(frag);
  free(path);

  return s;
}

