#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <vips/vips.h>

#include "var_version.h"

typedef struct
{
  ngx_flag_t vips;
} ngx_http_vips_loc_conf_t;

static ngx_int_t ngx_http_vips_handler(ngx_http_request_t *r);

// Configuration functions
static ngx_int_t ngx_http_vips_init(ngx_conf_t *cf);
static ngx_int_t ngx_http_vips_add_variables(ngx_conf_t *cf);
static void *ngx_http_vips_create_conf(ngx_conf_t *cf);
static char *ngx_http_vips_merge_conf(ngx_conf_t *cf, void *parent, void *child);

static ngx_command_t ngx_http_vips_commands[] = {
    // Syntax: vips off | on;
    {ngx_string("vips"),
     NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF | NGX_CONF_FLAG,
     ngx_conf_set_flag_slot,
     NGX_HTTP_LOC_CONF_OFFSET,
     offsetof(ngx_http_vips_loc_conf_t, vips),
     NULL},

    ngx_null_command};

static ngx_http_variable_t ngx_http_vips_vars[] = {
    {ngx_string("vips_version"), NULL, ngx_http_vips_get_version, 0, 0, 0},
    ngx_http_null_variable};

static ngx_http_module_t ngx_http_vips_module_ctx = {
    ngx_http_vips_add_variables, /* preconfiguration */
    ngx_http_vips_init,          /* postconfiguration */

    NULL, /* create main configuration */
    NULL, /* init main configuration */

    NULL, /* create server configuration */
    NULL, /* merge server configuration */

    ngx_http_vips_create_conf, /* create location configuration */
    ngx_http_vips_merge_conf   /* merge location configuration */
};

ngx_module_t ngx_http_vips_module = {
    NGX_MODULE_V1,
    &ngx_http_vips_module_ctx, /* module context */
    ngx_http_vips_commands,    /* module directives */
    NGX_HTTP_MODULE,           /* module type */
    NULL,                      /* init master */
    NULL,                      /* init module */
    NULL,                      /* init process */
    NULL,                      /* init thread */
    NULL,                      /* exit thread */
    NULL,                      /* exit process */
    NULL,                      /* exit master */
    NGX_MODULE_V1_PADDING};

// static ngx_http_output_header_filter_pt ngx_http_next_header_filter;
// static ngx_http_output_body_filter_pt ngx_http_next_body_filter;

static ngx_int_t ngx_http_vips_handler(ngx_http_request_t *r)
{
  const ngx_http_vips_loc_conf_t *conf;
  conf = ngx_http_get_module_loc_conf(r, ngx_http_vips_module);

  ngx_chain_t out;
  ngx_int_t rc;
  ngx_buf_t *b;
  ngx_str_t message = ngx_string("Hello, VIPS!");

  // Decline if vips is off
  if (conf->vips < 1)
  {
    return NGX_DECLINED;
  }

  r->headers_out.status = NGX_HTTP_OK;
  r->headers_out.content_length_n = message.len;
  ngx_str_set(&r->headers_out.content_type, "text/plain");

  /* Set header X-Foo: vips */
  ngx_table_elt_t *h = ngx_list_push(&r->headers_out.headers);
  if (h == NULL)
  {
    return NGX_ERROR;
  }

  h->hash = 1;
  ngx_str_set(&h->key, "X-Vips");
  ngx_str_set(&h->value, "vips");

  rc = ngx_http_send_header(r);

  if (rc == NGX_ERROR || rc > NGX_OK || r->header_only)
  {
    return rc;
  }

  /* send body */

  b = ngx_calloc_buf(r->pool);
  if (b == NULL)
  {
    return NGX_ERROR;
  }

  b->last_buf = (r == r->main) ? 1 : 0;
  b->last_in_chain = 1;

  b->memory = 1;

  b->pos = message.data;
  b->last = b->pos + message.len;

  out.buf = b;
  out.next = NULL;

  return ngx_http_output_filter(r, &out);
}

static ngx_int_t ngx_http_vips_add_variables(ngx_conf_t *cf)
{
  ngx_http_variable_t *var, *v;

  for (v = ngx_http_vips_vars; v->name.len; v++)
  {
    var = ngx_http_add_variable(cf, &v->name, v->flags);
    if (var == NULL)
    {
      return NGX_ERROR;
    }

    var->get_handler = v->get_handler;
    var->data = v->data;
  }

  return NGX_OK;
}

static ngx_int_t ngx_http_vips_init(ngx_conf_t *cf)
{
  ngx_http_handler_pt *h;
  ngx_http_core_main_conf_t *cmcf;

  cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

  //
  VIPS_INIT("nginx");

  h = ngx_array_push(&cmcf->phases[NGX_HTTP_CONTENT_PHASE].handlers);
  if (h == NULL)
  {
    return NGX_ERROR;
  }

  *h = ngx_http_vips_handler;

  return NGX_OK;
}

static void *ngx_http_vips_create_conf(ngx_conf_t *cf)
{
  ngx_http_vips_loc_conf_t *conf;

  conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_vips_loc_conf_t));
  if (conf == NULL)
  {
    ngx_conf_log_error(NGX_LOG_WARN, cf, 0, "vips create conf: conf==NULL");
    return NULL;
  }

  conf->vips = NGX_CONF_UNSET;

  return conf;
}

static char *ngx_http_vips_merge_conf(ngx_conf_t *cf, void *parent, void *child)
{
  ngx_http_vips_loc_conf_t *prev = parent;
  ngx_http_vips_loc_conf_t *conf = child;

  ngx_conf_log_error(NGX_LOG_ERR, cf, 0, "vips merge conf: %d -> %d", prev->vips, conf->vips);

  ngx_conf_merge_value(conf->vips, prev->vips, 0);

  ngx_conf_log_error(NGX_LOG_ERR, cf, 0, "vips merged conf: %d -> %d", prev->vips, conf->vips);

  return NGX_CONF_OK;
}
