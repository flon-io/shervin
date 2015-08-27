
//
// specifying shervin
//
// Wed Sep 17 06:18:59 JST 2014
//

#include "shervin.h"
#include "shv_protected.h" // direct access to fshv_request methods


context "uri"
{
  before each
  {
    fshv_uri *u = NULL;
  }
  after each
  {
    fshv_uri_free(u);
  }

  describe "fshv_parse_uri()"
  {
    it "returns the \"_path\""
    {
      u = fshv_parse_uri("/a/b/c");

      ensure(u != NULL);
      ensure(u->scheme == NULL);
      ensure(u->host == NULL);
      ensure(u->port == 80);
      ensure(u->path === "/a/b/c");
      ensure(u->query == NULL);
      ensure(u->fragment == NULL);

      ensure(u->qentries->size zu== 0);
    }

    it "returns the query string"
    {
      u = fshv_parse_uri("/a?b=c&d=e");

      ensure(u != NULL);
      ensure(u->scheme == NULL);
      ensure(u->host == NULL);
      ensure(u->port == 80);
      ensure(u->path === "/a");
      ensure(u->query === "?b=c&d=e");
      ensure(u->fragment == NULL);

      ensure(u->qentries->size zu== 2);
      ensure(flu_list_get(u->qentries, "b") === "c");
      ensure(flu_list_get(u->qentries, "d") === "e");
    }

    it "returns the fragment"
    {
      u = fshv_parse_uri("/a#frag");

      ensure(u != NULL);
      ensure(u->scheme == NULL);
      ensure(u->host == NULL);
      ensure(u->port == 80);
      ensure(u->path === "/a");
      ensure(u->query == NULL);
      ensure(u->fragment === "frag");

      ensure(u->qentries->size zu== 0);
    }

    it "returns _scheme, _host and _port"
    {
      u = fshv_parse_uri("https://www.example.com:80/a#frag");

      ensure(u != NULL);
      ensure(u->scheme === "https");
      ensure(u->host === "www.example.com");
      ensure(u->port == 80);
      ensure(u->path === "/a");
      ensure(u->query == NULL);
      ensure(u->fragment === "frag");

      ensure(u->qentries->size zu== 0);
    }

    it "unescapes \%hex entries"
    {
      u = fshv_parse_uri("/a?x=x%20y%20z");

      ensure(u != NULL);
      ensure(u->scheme == NULL);
      ensure(u->host == NULL);
      ensure(u->port == 80);
      ensure(u->path === "/a");
      ensure(u->query === "?x=x%20y%20z");
      ensure(u->fragment == NULL);

      ensure(u->qentries->size zu== 1);
      ensure(flu_list_get(u->qentries, "x") === "x y z");
    }

    it "accepts query entries without values"
    {
      u = fshv_parse_uri("/a?x");

      ensure(u != NULL);
      ensure(u->scheme == NULL);
      ensure(u->host == NULL);
      ensure(u->port == 80);
      ensure(u->path === "/a");
      ensure(u->query === "?x");
      ensure(u->fragment == NULL);

      ensure(u->qentries->size zu== 1);
      ensure(flu_list_get(u->qentries, "x") === "");
    }
  }

  describe "fshv_parse_host_and_path()"
  {
    it "returns _host, _path and co"
    {
      u = fshv_parse_host_and_path("http://example.com", "/a?x=x%20y%20z");

      ensure(u != NULL);
      ensure(u->scheme === "http");
      ensure(u->host === "example.com");
      ensure(u->port == 80);
      ensure(u->path === "/a");
      ensure(u->query === "?x=x%20y%20z");
      ensure(u->fragment == NULL);

      ensure(u->qentries->size zu== 1);
      ensure(flu_list_get(u->qentries, "x") === "x y z");
    }

    it "returns _host, _port, _path and co"
    {
      u = fshv_parse_host_and_path("http://example.com:8080", "/a/b#anchor");

      ensure(u != NULL);
      ensure(u->scheme === "http");
      ensure(u->host === "example.com");
      ensure(u->port == 8080);
      ensure(u->path === "/a/b");
      ensure(u->query == NULL);
      ensure(u->fragment === "anchor");

      ensure(u->qentries->size zu== 0);
    }

    it "defaults to http://"
    {
      u = fshv_parse_host_and_path("example.com:8080", "/a/b#anchor");

      ensure(u != NULL);
      ensure(u->scheme === "http");
      ensure(u->host === "example.com");
      ensure(u->port == 8080);
      ensure(u->path === "/a/b");
      ensure(u->query == NULL);
      ensure(u->fragment === "anchor");

      ensure(u->qentries->size zu== 0);
    }
  }

  describe "fshv_absolute_uri()"
  {
    context "fshv_abs()"
    {
      it "returns an absolute uri (0)"
      {
        u = fshv_parse_host_and_path("example.com:8080", "/a/b#anchor");

        expect(fshv_abs(0, u) ===f "http://example.com:8080/a/b#anchor");
        expect(fshv_abs(1, u) ===f "https://example.com:8080/a/b#anchor");
      }

      it "returns an absolute uri (1)"
      {
        u = fshv_parse_uri("/a?b=c&d=e");

        expect(fshv_abs(0, u) ===f "http://127.0.0.1/a?b=c&d=e");
        expect(fshv_abs(1, u) ===f "https://127.0.0.1/a?b=c&d=e");
      }

      it "returns an absolute uri (2)"
      {
        u = fshv_parse_uri("/a#frag");

        expect(fshv_abs(0, u) ===f "http://127.0.0.1/a#frag");
        expect(fshv_abs(1, u) ===f "https://127.0.0.1/a#frag");
      }
    }

    context "fshv_rel()"
    {
      it "returns an absolute uri (0)"
      {
        u = fshv_parse_host_and_path("example.com:8080", "/a/b#anchor");

        expect(fshv_rel(0, u, "c") ===f ""
          "http://example.com:8080/a/b/c#anchor");
        expect(fshv_rel(0, u, "/c") ===f ""
          "http://example.com:8080/c#anchor");
        expect(fshv_rel(0, u, "..") ===f ""
          "http://example.com:8080/a/#anchor");
      }

      it "returns an absolute uri (2)"
      {
        u = fshv_parse_uri("http://a.example.com/a/index.htm#frag");

        expect(fshv_rel(0, u, "..") ===f ""
          "http://a.example.com/#frag");
        expect(fshv_rel(0, u, "toto.htm") ===f ""
          "http://a.example.com/a/toto.htm#frag");
      }

      it "returns an absolute uri (3)"
      {
        u = fshv_parse_uri("http://a.example.com/a/index.htm/#frag");

        expect(fshv_rel(0, u, "..") ===f ""
          "http://a.example.com/a/#frag");
        expect(fshv_rel(0, u, "toto.htm") ===f ""
          "http://a.example.com/a/index.htm/toto.htm#frag");
      }

      it "composes its path"
      {
        u = fshv_parse_uri("http://a.example.com/a/b");

        expect(fshv_rel(0, u, "../%s", "c") ===f "http://a.example.com/a/c");
      }
    }
  }
}

