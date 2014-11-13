
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
    if (d != NULL) flu_list_free_all(d);
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
      ensure(d->size zu== 4);
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
      ensure(d->size zu== 3);
      ensure(flu_list_get(d, "_path") === "/a");
      ensure(flu_list_get(d, "x") === "x y z");
    }

    it "accepts query entries without values"
    {
      d = shv_parse_uri("/a?x");

      ensure(d != NULL);
      ensure(d->size zu== 3);
      ensure(flu_list_get(d, "_path") === "/a");
      ensure(flu_list_get(d, "x") === "");
    }
  }

  describe "shv_parse_host_and_path()"
  {
    it "returns _host, _path and co"
    {
      d = shv_parse_host_and_path("http://example.com", "/a?x=x%20y%20z");

      ensure(d != NULL);
      ensure(d->size zu== 5);
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

    it "defaults to http://"
    {
      d = shv_parse_host_and_path("example.com:8080", "/a/b#anchor");

      expect(d != NULL);
      expect(d->size zu== 5);
      expect(flu_list_get(d, "_scheme") === "http");
      expect(flu_list_get(d, "_host") === "example.com");
      expect(flu_list_get(d, "_port") === "8080");
      expect(flu_list_get(d, "_path") === "/a/b");
      expect(flu_list_get(d, "_fragment") === "anchor");
    }
  }

  describe "shv_absolute_uri()"
  {
    context "shv_abs()"
    {
      it "returns an absolute uri (0)"
      {
        d = shv_parse_host_and_path("example.com:8080", "/a/b#anchor");

        expect(shv_abs(0, d) ===f "http://example.com:8080/a/b#anchor");
        expect(shv_abs(1, d) ===f "https://example.com:8080/a/b#anchor");
      }

      it "returns an absolute uri (1)"
      {
        d = shv_parse_uri("/a?b=c&d=e");

        expect(shv_abs(0, d) ===f "http://127.0.0.1/a?b=c&d=e");
        expect(shv_abs(1, d) ===f "https://127.0.0.1/a?b=c&d=e");
      }

      it "returns an absolute uri (2)"
      {
        d = shv_parse_uri("/a#frag");

        expect(shv_abs(0, d) ===f "http://127.0.0.1/a#frag");
        expect(shv_abs(1, d) ===f "https://127.0.0.1/a#frag");
      }
    }

    context "shv_rel()"
    {
      it "returns an absolute uri (0)"
      {
        d = shv_parse_host_and_path("example.com:8080", "/a/b#anchor");

        expect(shv_rel(0, d, "c") ===f ""
          "http://example.com:8080/a/b/c#anchor");
        expect(shv_rel(0, d, "/c") ===f ""
          "http://example.com:8080/c#anchor");
        expect(shv_rel(0, d, "..") ===f ""
          "http://example.com:8080/a/#anchor");
      }

      it "returns an absolute uri (2)"
      {
        d = shv_parse_uri("http://a.example.com/a/index.htm#frag");

        expect(shv_rel(0, d, "..") ===f ""
          "http://a.example.com/#frag");
        expect(shv_rel(0, d, "toto.htm") ===f ""
          "http://a.example.com/a/toto.htm#frag");
      }

      it "returns an absolute uri (3)"
      {
        d = shv_parse_uri("http://a.example.com/a/index.htm/#frag");

        expect(shv_rel(0, d, "..") ===f ""
          "http://a.example.com/a/#frag");
        expect(shv_rel(0, d, "toto.htm") ===f ""
          "http://a.example.com/a/index.htm/toto.htm#frag");
      }

      it "composes its path"
      {
        d = shv_parse_uri("http://a.example.com/a/b");

        expect(shv_rel(0, d, "../%s", "c") ===f "http://a.example.com/a/c");
      }
    }
  }
}

