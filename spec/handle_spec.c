
//
// specifying shervin
//
// Thu Sep 18 17:49:27 JST 2014
//

#include "shervin.h"
#include "shv_protected.h" // direct access to shv_request methods


context "handle"
{

  int gua(
    shv_request *req, flu_dict *rod, shv_response *res, flu_dict *params)
  {
    char *g = flu_list_get(params, "gua");
    return (g && strcmp(g, "true") == 0);
  }

  int han(
    shv_request *req, flu_dict *guard, shv_response *res, flu_dict *params)
  {
    flu_list_set(
      res->headers, "x-handled", rdz_strdup(flu_list_get(params, "han")));

    return 1; // yes it's over
  }

  int fil(
    shv_request *req, flu_dict *guard, shv_response *res, flu_dict *params)
  {
    flu_list_set(
      res->headers, "x-filtered", rdz_strdup(flu_list_get(params, "fil")));

    return 0; // ok, I've dealt with it, let others see it
  }

  before each
  {
    struct ev_io *eio = calloc(1, sizeof(shv_con));
    shv_con *con = calloc(1, sizeof(shv_con));
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
    shv_request_free(con->req);
    shv_response_free(con->res);
    free(con);
    free(eio);
  }

  describe "shv_handle()"
  {
    it "triggers handler when guard says 1"
    {
      con->routes = (shv_route *[]){
        shv_route_malloc(gua, han, "gua", "false", "han", "a", NULL),
        shv_route_malloc(gua, han, "gua", "true", "han", "b", NULL),
        NULL // do not forget it
      };

      con->req = shv_parse_request(""
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      shv_handle(NULL, eio);

      ensure(flu_list_get(con->res->headers, "x-handled") === "b");
    }

    it "triggers next handler when guard says 1"
    {
      con->routes = (shv_route *[]){
        shv_route_malloc(gua, NULL, "gua", "false", NULL),
        shv_route_malloc(NULL, han, "han", "a", NULL),
        shv_route_malloc(gua, NULL, "gua", "true", NULL),
        shv_route_malloc(NULL, han, "han", "b", NULL),
        NULL // do not forget it
      };

      con->req = shv_parse_request(""
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      shv_handle(NULL, eio);

      ensure(flu_list_get(con->res->headers, "x-handled") === "b");
    }

    it "triggers handlers until one says 1"
    {
      con->routes = (shv_route *[]){
        shv_route_malloc(gua, fil, "gua", "true", "fil", "z", NULL),
        shv_route_malloc(NULL, han, "gua", "true", "han", "a", NULL),
        NULL // do not forget it
      };

      con->req = shv_parse_request(""
        "GET /x HTTP/1.1\r\n"
        "Host: http://www.example.com\r\n"
        "\r\n");

      shv_handle(NULL, eio);

      ensure(flu_list_get(con->res->headers, "x-handled") === "a");
      ensure(flu_list_get(con->res->headers, "x-filtered") === "z");
    }
  }

  //shv_route **routes = (shv_route *[]){
  //  shv_route_malloc(shv_pre_guard, handler1, ps1),
  //  shv_route_malloc(guard0, handler0, ps0),
  //  shv_route_malloc(guard1, handler1, ps1),
  //  shv_route_malloc(shv_post_guard, handler1, ps1),
  //  NULL
  //};
}

