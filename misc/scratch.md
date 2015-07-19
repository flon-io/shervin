
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

### sub match

```c
  if (fshv_match(env, "GET /doc/**")) return x(env);
  if (fshv_match(env, "/doc/**")) return x(env);
    // vs
  if (fshv_match(env, "doc/:id")) return x(env);
    //
    // absolute vs relative,
    // the first matches the whole URI,
    // the second matches what's left (**)

  // allows for

  void subhandler(fshv_env *env)
  {
    if (fshv_match(env, "books/:id")) return y(env);
    // ...
  }
  // ...
  if (fshv_match(env, "GET /doc/**")) return subhandler(env);
```


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

## more

```c
// shervin.h

typedef int fshv_handler(fshv_env *env);

short fshv_match(fshv_env *env, char *path);

void fshv_serve(int port, fshv_handler *root_handler, flu_dict *conf);

// x.c

static int _serve(fshv_env *env)
{
  if (fshv_match(env, "/doc/**")) return fshv_serve_files(env, "../static/");
  // ...
}

int main()
{
  // ...

  fshv_serve(80, _serve, conf);
}
```

What usage am I trying to support?

```
typedef short fshv_handler(fsvh_env *env);
  //
  // 0 not handled
  // 1 handled

// well, only the root handler needs that super simple signature...
// the other handlers accept at least env then some special info...

// conventions? 0 or 1, or NULL or something?
// NULL or a fshv_handler?

// what about submatches?

// use the strength of the programming language, not some clever data structure

// -1 break
//  0 continue
//  1 success, break
  //
// 0 failure
// 1 success
```

```pseudo
static void _serve(fshv_env *env)
{
  if (fshv_match(env, "/static/stuff")) return _serve_static(env, "../static");

  //if ( ! fshv_authentify) return fsvh_
  if (fsvh_authentify(env, x) == FSHV_OVER) return
}
```

What's need to be supported:
```c
  fshv_route *routes[] =
  {
    fshv_r(
      NULL,
      fshv_basic_auth_filter,
      "func", flon_auth_enticate, "realm", "flon", NULL),

    fshv_rp("GET /i", flon_i_handler, NULL),
    fshv_rp("POST /i/in", flon_in_handler, NULL),
    fshv_rp("GET /i/executions", flon_exes_handler, NULL),
    fshv_rp("GET /i/executions/:id", flon_exe_handler, NULL),
    fshv_rp("GET /i/executions/:id/:sub", flon_exe_sub_handler, NULL),
    fshv_rp("GET /i/msgs/:id", flon_msg_handler, NULL),
    fshv_rp("GET /i/metrics", flon_metrics_handler, NULL),
    fshv_rp("GET /**", fshv_dir_handler, "r", "var/www", NULL),

    NULL
  };
```

