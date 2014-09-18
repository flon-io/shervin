
//
// specifying shervin
//
// Thu Sep 18 17:49:27 JST 2014
//

#include "shervin.h"
#include "shv_protected.h" // direct access to shv_request methods


context "handle"
{
  before each
  {
    //flu_dict *d = NULL;
  }
  after each
  {
    //if (d != NULL) flu_list_free_all(d);
  }

  //shv_route **routes = (shv_route *[]){
  //  shv_route_malloc(guard0, handler0, ps0),
  //  shv_route_malloc(guard1, handler1, ps1),
  //  NULL
  //};

  //shv_route **routes = (shv_route *[]){
  //  shv_route_malloc(guard0, NULL, ps0),
  //  shv_route_malloc(NULL, handler0, ps1),
  //  shv_route_malloc(NULL, handler1, ps2),
  //  shv_route_malloc(guard1, NULL, ps3),
  //  shv_route_malloc(NULL, handler2, ps4),
  //  NULL
  //};

  describe "shv_handle()"
  {
    it "flips burgers"
    {
      pending();
    }
  }
}

