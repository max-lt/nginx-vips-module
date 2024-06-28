#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

typedef struct {
  ngx_str_t foo;
} ngx_http_foo_loc_conf_t;

static ngx_int_t ngx_http_foo_handler(ngx_http_request_t *r);

// Configuration functions
static ngx_int_t ngx_http_foo_init(ngx_conf_t *cf);
static void * ngx_http_foo_create_conf(ngx_conf_t *cf);
static char * ngx_http_foo_merge_conf(ngx_conf_t *cf, void *parent, void *child);

static ngx_command_t ngx_http_foo_commands[] = {
  // foo $variable | off | on;
  { ngx_string("foo"),
    NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
    ngx_conf_set_str_slot,
    NGX_HTTP_LOC_CONF_OFFSET,
    offsetof(ngx_http_foo_loc_conf_t, foo),
    NULL },

  ngx_null_command
};


static ngx_http_module_t ngx_http_foo_module_ctx = {
  NULL,                          /* preconfiguration */
  ngx_http_foo_init,      /* postconfiguration */

  NULL,                          /* create main configuration */
  NULL,                          /* init main configuration */

  NULL,                          /* create server configuration */
  NULL,                          /* merge server configuration */

  ngx_http_foo_create_conf,             /* create location configuration */
  ngx_http_foo_merge_conf               /* merge location configuration */
};


ngx_module_t ngx_http_foo_module = {
  NGX_MODULE_V1,
  &ngx_http_foo_module_ctx,     /* module context */
  ngx_http_foo_commands,        /* module directives */
  NGX_HTTP_MODULE,                     /* module type */
  NULL,                                /* init master */
  NULL,                                /* init module */
  NULL,                                /* init process */
  NULL,                                /* init thread */
  NULL,                                /* exit thread */
  NULL,                                /* exit process */
  NULL,                                /* exit master */
  NGX_MODULE_V1_PADDING
};


static ngx_int_t ngx_http_foo_handler(ngx_http_request_t *r)
{
  const ngx_http_foo_loc_conf_t *conf;
  conf = ngx_http_get_module_loc_conf(r, ngx_http_foo_module);

  ngx_chain_t      out;
  ngx_int_t        rc;
  ngx_buf_t        *b;

  b = ngx_create_temp_buf(r->pool, conf->foo.len);
  if (b == NULL) {
    ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
    return NGX_DONE;
  }

  r->headers_out.status = NGX_HTTP_OK;
  r->headers_out.content_length_n = conf->foo.len;
  ngx_str_set(&r->headers_out.content_type, "text/plain");

  /* Set header X-Foo: foo */
  ngx_table_elt_t  *h = ngx_list_push(&r->headers_out.headers);
  if (h == NULL) {
      return NGX_ERROR;
  }

  h->hash = 1;
  ngx_str_set(&h->key, "ETag");
  ngx_str_set(&h->value, "foo");

  rc = ngx_http_send_header(r);

  if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
    ngx_http_finalize_request(r, rc);
    return NGX_DONE;
  }

  // Write to buffer
  size_t len = ngx_min(((size_t) (b->end - b->pos)), conf->foo.len);
  b->last = ngx_copy(b->pos, conf->foo.data, len);
  b->last_buf = 1;
  b->last_in_chain = 1;

  out.buf = b;
  out.next = NULL;

  rc = ngx_http_output_filter(r, &out);
  ngx_http_finalize_request(r, rc);

  return NGX_DONE;
}


static ngx_int_t ngx_http_foo_init(ngx_conf_t *cf)
{
  ngx_http_handler_pt        *h;
  ngx_http_core_main_conf_t  *cmcf;

  cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

  h = ngx_array_push(&cmcf->phases[NGX_HTTP_PRECONTENT_PHASE].handlers);
  if (h == NULL)
  {
    return NGX_ERROR;
  }

  *h = ngx_http_foo_handler;

  return NGX_OK;
}


static void * ngx_http_foo_create_conf(ngx_conf_t *cf)
{
  ngx_http_foo_loc_conf_t *conf;

  conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_foo_loc_conf_t));
  if (conf == NULL)
  {
    ngx_conf_log_error(NGX_LOG_WARN, cf, 0, "foo create conf: conf==NULL");
    return NULL;
  }

  // Initialize variables
  ngx_str_null(&conf->foo);

  return conf;
}


static char * ngx_http_foo_merge_conf(ngx_conf_t *cf, void *parent, void *child)
{
  ngx_http_foo_loc_conf_t *prev = parent;
  ngx_http_foo_loc_conf_t *conf = child;

  ngx_conf_merge_str_value(conf->foo, prev->foo, "");

  return NGX_CONF_OK;
}
