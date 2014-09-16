
//
// specifying shervin
//
// Wed Sep 17 06:18:59 JST 2014
//

#include "shervin.h"
#include "shv_protected.h" // direct access to shv_request methods


context "uri"
{
  describe "shv_parse_uri()"
  {
    before each
    {
      flu_dict *d = NULL;
    }
    after each
    {
      if (d != NULL) flu_list_and_items_free(d, free);
    }

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

    it "unescapes \%oct entries"
    {
      pending();
    }
  }
}

