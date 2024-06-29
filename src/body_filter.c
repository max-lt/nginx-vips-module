#include <ngx_core.h>
#include <ngx_http.h>

#include "ngx_http_vips_module.h"

static ngx_http_output_body_filter_pt next;

static ngx_int_t vips_body_filter(ngx_http_request_t *r, ngx_chain_t *in);

void register_vips_body_filter(ngx_conf_t *cf, ngx_http_output_body_filter_pt *prev)
{
  ngx_conf_log_error(NGX_LOG_WARN, cf, 0, "[VBF] register Vips Body Filter");

  next = *prev;
  *prev = vips_body_filter;
}

static ngx_int_t vips_body_filter(ngx_http_request_t *r, ngx_chain_t *in)
{
  ngx_http_vips_loc_conf_t *conf;

  conf = ngx_http_get_module_loc_conf(r, ngx_http_vips_module);
  if (conf->vips == 0)
  {
    ngx_log_error(NGX_LOG_WARN, r->connection->log, 0, "[VBF] skip");
    return next(r, in);
  }

  ngx_log_error(NGX_LOG_WARN, r->connection->log, 0, "[VBF] process buf size: %d", in->buf->last - in->buf->pos);
  ngx_log_error(NGX_LOG_WARN, r->connection->log, 0, "[VBF] process buffered: %d", r->connection->buffered);

  return NGX_OK;
}
