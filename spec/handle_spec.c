
//
// specifying shervin
//
// Thu Sep 18 17:49:27 JST 2014
//

#include "shervin.h"
#include "shv_protected.h" // direct access to fshv_request methods


context "handle"
{
  int gyes(fshv_request *req, fshv_response *res, flu_dict *params) { return 1; }
  int gno(fshv_request *req, fshv_response *res, flu_dict *params) { return 0; }

  int gfil(fshv_request *req, fshv_response *res, flu_dict *params)
  {
    return -1; // force handlers to behave like filters
  }

  int han(fshv_request *req, fshv_response *res, flu_dict *params)
  {
    if (flu_list_get(params, "sta"))
    {
      char *k = flu_sprintf("x-stamp-%s", flu_list_get(params, "han"));
      flu_list_set(res->headers, k, rdz_strdup("seen"));
      free(k);
    }
    else
    {
      flu_list_set(
        res->headers, "x-handled", rdz_strdup(flu_list_get(params, "han")));
    }

    return 1; // yes, it's over
  }

  int fil(fshv_request *req, fshv_response *res, flu_dict *params)
  {
    flu_list_set(
      res->headers, "x-filtered", rdz_strdup(flu_list_get(params, "fil")));

    return 0; // ok, I've dealt with it, let others see it
  }

  before each
  {
    struct ev_io *eio = calloc(1, sizeof(ev_io));
    fshv_con *con = calloc(1, sizeof(fshv_con));
    eio->data = con;
  }
  after each
  {
    for (size_t i = 0; con->routes[i] != NULL; ++i)
    {
      flu_list_free(con->routes[i]->params);
      free(con->routes[i]);
    }
    //free(con->routes); // not malloc'ed
    fshv_request_free(con->req);
    fshv_response_free(con->res);
    free(con);
    free(eio);
  }

  describe "fshv_handle()"
  {
    it "triggers handler when guard says 1"
    {
      con->routes = (fshv_route *[]){
        fshv_r(gno, han, "han", "a", NULL),
        fshv_r(gyes, han, "han", "b", NULL),
        NULL // do not forget it
      };

      con->req = fshv_parse_request_head(
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      fshv_handle(NULL, eio);

      ensure(flu_list_get(con->res->headers, "x-handled") === "b");
    }

    it "triggers next handler when guard says 1"
    {
      con->routes = (fshv_route *[]){
        fshv_r(gno, NULL, NULL),
        fshv_r(NULL, han, "han", "a", NULL),
        fshv_r(gyes, NULL, NULL),
        fshv_r(NULL, han, "han", "b", NULL),
        NULL // do not forget it
      };

      con->req = fshv_parse_request_head(
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      fshv_handle(NULL, eio);

      ensure(flu_list_get(con->res->headers, "x-handled") === "b");
    }

    it "triggers handlers until one says 1"
    {
      con->routes = (fshv_route *[]){
        fshv_r(gyes, fil, "fil", "z", NULL),
        fshv_r(NULL, han, "gua", "true", "han", "a", NULL),
        NULL // do not forget it
      };

      con->req = fshv_parse_request_head(
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      fshv_handle(NULL, eio);

      ensure(flu_list_get(con->res->headers, "x-handled") === "a");
      ensure(flu_list_get(con->res->headers, "x-filtered") === "z");
    }

    it "triggers pre-filters"
    {
      con->routes = (fshv_route *[]){
        fshv_r(fshv_filter_guard, han, "sta", "true", "han", "a", NULL),
        fshv_r(gyes, han, "han", "b", NULL),
        NULL // do not forget it
      };

      con->req = fshv_parse_request_head(
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      fshv_handle(NULL, eio);

      ensure(flu_list_get(con->res->headers, "x-handled") === "b");
      ensure(flu_list_get(con->res->headers, "x-stamp-a") === "seen");
    }

    it "triggers post-filters"
    {
      con->routes = (fshv_route *[]){
        fshv_r(gyes, han, "han", "a", NULL),
        fshv_r(fshv_filter_guard, han, "sta", "true", "han", "b", NULL),
        NULL // do not forget it
      };

      con->req = fshv_parse_request_head(
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      fshv_handle(NULL, eio);

      ensure(flu_list_get(con->res->headers, "x-handled") === "a");
      ensure(flu_list_get(con->res->headers, "x-stamp-b") === "seen");
    }

    it "triggers filters in row"
    {
      con->routes = (fshv_route *[]){
        //
        fshv_r(fshv_filter_guard, NULL, NULL),
        fshv_r(NULL, han, "sta", "true", "han", "w", NULL), //
        fshv_r(NULL, han, "sta", "true", "han", "x", NULL), // <-- sees
        //
        fshv_r(gno, han, "han", "a", NULL),
        fshv_r(gyes, han, "han", "b", NULL), // <-- handles
        //
        fshv_r(fshv_filter_guard, NULL, NULL),
        fshv_r(NULL, han, "sta", "true", "han", "y", NULL), //
        fshv_r(NULL, han, "sta", "true", "han", "z", NULL), // <-- sees
        //
        fshv_r(gyes, han, "han", "c", NULL), // doesn't handle
        NULL // do not forget it
      };

      con->req = fshv_parse_request_head(
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      fshv_handle(NULL, eio);

      ensure(flu_list_get(con->res->headers, "x-handled") === "b");
      ensure(flu_list_get(con->res->headers, "x-stamp-w") === "seen");
      ensure(flu_list_get(con->res->headers, "x-stamp-x") === "seen");
      ensure(flu_list_get(con->res->headers, "x-stamp-y") === "seen");
      ensure(flu_list_get(con->res->headers, "x-stamp-z") === "seen");
    }

    it "treats handlers like filters when guard says -1"
    {
      con->routes = (fshv_route *[]){
        fshv_r(gfil, han, "sta", "true", "han", "a", NULL),
        fshv_r(NULL, han, "sta", "true", "han", "b", NULL),
        fshv_r(gyes, han, "han", "c", NULL),
        fshv_r(gfil, han, "sta", "true", "han", "d", NULL), // no
        NULL // do not forget it
      };

      con->req = fshv_parse_request_head(
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      fshv_handle(NULL, eio);

      ensure(flu_list_get(con->res->headers, "x-stamp-a") === "seen");
      ensure(flu_list_get(con->res->headers, "x-stamp-b") === "seen");
      ensure(flu_list_get(con->res->headers, "x-stamp-d") == NULL);
      ensure(flu_list_get(con->res->headers, "x-handled") === "c");
    }

    it "accepts fshv_rp(path, handler, ...) (get)"
    {
      con->routes = (fshv_route *[]){
        fshv_rp("POST /nada", han, "han", "a", NULL),
        fshv_rp("/nada", han, "han", "b", NULL),
        NULL // do not forget it
      };

      con->req = fshv_parse_request_head(
        "GET /nada HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      fshv_handle(NULL, eio);

      ensure(flu_list_get(con->res->headers, "x-handled") === "b");
    }

    it "accepts fshv_rp(path, handler, ...) (post)"
    {
      con->routes = (fshv_route *[]){
        fshv_rp("POST /nada", han, "han", "a", NULL),
        fshv_rp("/nada", han, "han", "b", NULL),
        NULL // do not forget it
      };

      con->req = fshv_parse_request_head(
        "POST /nada HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "Content-Length: 1\r\n"
        "\r\n"
        "x");

      fshv_handle(NULL, eio);

      ensure(flu_list_get(con->res->headers, "x-handled") === "a");
    }

    it "accepts fshv_rp(path, handler, ...) (head)"
    {
      con->routes = (fshv_route *[]){
        fshv_rp("POST /nada", han, "han", "a", NULL),
        fshv_rp("DELETE /nada", han, "han", "b", NULL),
        fshv_rp("GET /nada", han, "han", "c", NULL),
        NULL // do not forget it
      };

      con->req = fshv_parse_request_head(
        "HEAD /nada HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      fshv_handle(NULL, eio);

      ensure(flu_list_get(con->res->headers, "x-handled") === "c");
    }
  }
}

