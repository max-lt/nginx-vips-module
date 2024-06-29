#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <vips/vips.h>

#include "ngx_http_vips_module.h"
#include "var_version.h"
#include "var_original_size.h"
#include "body_filter.h"
#include "header_filter.h"

// Configuration functions
static ngx_int_t ngx_http_vips_filter_init(ngx_conf_t *cf);
static ngx_int_t ngx_http_vips_add_variables(ngx_conf_t *cf);
static void *ngx_http_vips_create_conf(ngx_conf_t *cf);
static char *ngx_http_vips_merge_conf(ngx_conf_t *cf, void *parent, void *child);

static ngx_command_t ngx_http_vips_commands[] = {
    // Syntax: vips off | on; Default: vips off;
    {ngx_string("vips"),
     NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF | NGX_CONF_FLAG,
     ngx_conf_set_flag_slot,
     NGX_HTTP_LOC_CONF_OFFSET,
     offsetof(ngx_http_vips_loc_conf_t, vips),
     NULL},

    ngx_null_command};

static ngx_http_variable_t ngx_http_vips_vars[] = {
    {ngx_string("vips_version"), NULL, ngx_http_vips_get_version, 0, 0, 0},
    {ngx_string("vips_image_size"), NULL, ngx_http_vips_get_size, 0, 0, 0},
    ngx_http_null_variable};

// https://nginx.org/en/docs/dev/development_guide.html#http_response_body_filters
static ngx_http_module_t ngx_http_vips_module_ctx = {
    ngx_http_vips_add_variables, /* preconfiguration */
    ngx_http_vips_filter_init,   /* postconfiguration */

    NULL, /* create main configuration */
    NULL, /* init main configuration */

    NULL, /* create server configuration */
    NULL, /* merge server configuration */

    ngx_http_vips_create_conf, /* create location configuration */
    ngx_http_vips_merge_conf   /* merge location configuration */
};

// Global variables
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

static ngx_int_t ngx_http_vips_filter_init(ngx_conf_t *cf)
{
  //
  ngx_conf_log_error(NGX_LOG_WARN, cf, 0, "vips filter init");

  VIPS_INIT("nginx");

  // Insert the vips filter before the default header filter
  register_vips_body_filter(cf, &ngx_http_top_body_filter);

  // Insert the vips filter before the default body filter
  register_vips_header_filter(cf, &ngx_http_top_header_filter);

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
