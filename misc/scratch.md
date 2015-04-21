
# scratch

```c
// webapp.c

int main()
{
  fshv_route *routes[] =
  {
    // public zone

    fshv_rp("/mirror", fshv_debug_handler, NULL),
    fshv_rp("/hello/:name", hello_handler, NULL),
    fshv_rp("/redir", redir_handler, NULL),
    fshv_rp("/files/**", fshv_dir_handler, "r", "../spec/www", NULL),

    // authentified zone

    fshv_rp("POST /login", login_handler, NULL),
    fshv_r(fshv_any_guard, fshv_session_auth_filter, NULL),
    fshv_rp("GET /secret", secret_handler, NULL),

    NULL
  };
}
```


## building block

```c
typedef int fshv_handler(
  fshv_request *req, fshv_response *res, int mode, flu_dict *params);

typedef struct {
  fshv_handler *guard;
  fshv_handler *handler;
  flu_dict *params;
} fshv_route;

fshv_r(fshv_handler *guard, fshv_handler *handler, flu_dict *params);
```


## functions

Stop the routes nonsense. Everything is a handler.

```c
int public_zone_handler(
  fshv_request *req, fshv_response *res, flu_dict *params)
{
  // ...
}

int root_handler
  fshv_request *req, fshv_response *res, flu_dict *params)
{
  if (fshv_m(req, "/mirror", params))
    return fshv_debug_handler(req, res, params);

  if (fshv_m(req, "/hello/:name", params))
    return hello_handler(req, res, params);
}

root_handler(req, res, params);
```


## guards and handlers

```c
int fshv_guard(fshv_request *req, fshv_response *res, flu_dict *params);
  // a guard says yes or no
  // it also adds entries to params (for consumption by handlers downstream)

void fshv_handler(fshv_request *req, fshv_response *res, flu_dict *params);
  // a handler usually populates the response
```

What if we only have `_handler`? Guards could look like:
```c
int fshv_match(
  char *path, fshv_request *req, fshv_response *res, flu_dict *params);

// ...

  if (fshv_match("GET /doc", req, res, params)) return fshv_dir_handler(...);
```

There could be a complete zoo of guard functions and homogeneous handler functions.


## beef up the request

Like Rack's `env`?

Saves some typing.

```c
typedef struct {
  fshv_request *req;
  fshv_response *res;
  flu_dict *params;
} fshv_env;

int fshv_match(char *path, fshv_env *env);

// ...

  if (fshv_match("GET /doc", env)) return fshv_dir_handler(env);
```

## action plan

* build fshv_env
* build the new guards
* build the new handlers
* drop the routes, use a root handler
* integrate
* drop the old stuff

