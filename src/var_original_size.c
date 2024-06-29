#include <ngx_core.h>
#include <ngx_http.h>
#include <vips/vips.h>

#include "ngx_http_vips_module.h"

ngx_int_t ngx_http_vips_get_size(ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data)
{
  u_char *p;
  ngx_http_vips_filter_ctx_t *ctx;

  ctx = ngx_http_get_module_ctx(r, ngx_http_vips_module);
  if (ctx == NULL)
  {
    v->not_found = 1;
    return NGX_OK;
  }

  p = ngx_pnalloc(r->pool, NGX_OFF_T_LEN);
  if (p == NULL)
  {
    return NGX_ERROR;
  }

  v->data = p;
  v->len = ngx_sprintf(p, "%O", ctx->original_size) - p;
  v->valid = 1;
  v->no_cacheable = 0;
  v->not_found = 0;

  return NGX_OK;
}
