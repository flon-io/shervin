
//
// specifying shervin
//
// Wed Sep 17 06:18:59 JST 2014
//

#include "shervin.h"
#include "shv_protected.h" // direct access to shv_request methods


context "uri"
{
  before each
  {
    flu_dict *d = NULL;
  }
  after each
  {
    if (d != NULL) flu_list_and_items_free(d, free);
  }

  describe "shv_parse_uri()"
  {
    it "returns the \"_path\""
    {
      d = shv_parse_uri("/a/b/c");

      ensure(d != NULL);
      ensure(d->size == 1);
      ensure(flu_list_get(d, "_path") === "/a/b/c");
    }

    it "returns the query string"
    {
      d = shv_parse_uri("/a?b=c&d=e");

      ensure(d != NULL);
      ensure(d->size == 3);
      ensure(flu_list_get(d, "_path") === "/a");
      ensure(flu_list_get(d, "b") === "c");
      ensure(flu_list_get(d, "d") === "e");
    }

    it "returns the fragment"
    {
      d = shv_parse_uri("/a#frag");

      ensure(d != NULL);
      ensure(d->size == 2);
      ensure(flu_list_get(d, "_path") === "/a");
      ensure(flu_list_get(d, "_fragment") === "frag");
    }

    it "returns _scheme, _host and _port"
    {
      d = shv_parse_uri("https://www.example.com:80/a#frag");

      ensure(d != NULL);
      ensure(d->size == 5);
      ensure(flu_list_get(d, "_scheme") === "https");
      ensure(flu_list_get(d, "_host") === "www.example.com");
      ensure(flu_list_get(d, "_port") === "80");
      ensure(flu_list_get(d, "_path") === "/a");
      ensure(flu_list_get(d, "_fragment") === "frag");
    }

    it "unescapes \%hex entries"
    {
      d = shv_parse_uri("/a?x=x%20y%20z");

      ensure(d != NULL);
      ensure(d->size == 2);
      ensure(flu_list_get(d, "_path") === "/a");
      ensure(flu_list_get(d, "x") === "x y z");
    }
  }

  describe "shv_parse_host_and_path()"
  {
    it "returns _host, _path and co"
    {
      d = shv_parse_host_and_path("http://example.com", "/a?x=x%20y%20z");

      ensure(d != NULL);
      ensure(d->size == 4);
      ensure(flu_list_get(d, "_scheme") === "http");
      ensure(flu_list_get(d, "_host") === "example.com");
      ensure(flu_list_get(d, "_path") === "/a");
      ensure(flu_list_get(d, "x") === "x y z");
    }

    it "returns _host, _port, _path and co"
    {
      d = shv_parse_host_and_path("http://example.com:8080", "/a/b#anchor");

      ensure(d != NULL);
      ensure(d->size == 5);
      ensure(flu_list_get(d, "_scheme") === "http");
      ensure(flu_list_get(d, "_host") === "example.com");
      ensure(flu_list_get(d, "_port") === "8080");
      ensure(flu_list_get(d, "_path") === "/a/b");
      ensure(flu_list_get(d, "_fragment") === "anchor");
    }
  }
}

