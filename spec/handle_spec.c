
//
// specifying shervin
//
// Thu Sep 18 17:49:27 JST 2014
//

#include "shervin.h"
#include "shv_protected.h" // direct access to shv_request methods

//int myGuard(
//  shv_request *req, flu_dict *rod, shv_response *res, flu_dict *params)
//{
//  if (flu_list_get(params, "accept")) return 1;
//  return 0;
//}
//
//int myHandler(
//  shv_request *req, flu_dict *guard, shv_response *res, flu_dict *params)
//{
//  flu_list_set(res->headers, "x-handled", flu_list_get(params, "mh_val"));
//
//  return 1;
//}


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

