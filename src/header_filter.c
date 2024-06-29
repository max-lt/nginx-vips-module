#include <ngx_core.h>
#include <ngx_http.h>

#include "ngx_http_vips_module.h"

static ngx_http_output_header_filter_pt next;

static ngx_int_t ngx_http_vips_header_filter(ngx_http_request_t *r);

void register_vips_header_filter(ngx_conf_t *cf, ngx_http_output_header_filter_pt *prev)
{
  ngx_conf_log_error(NGX_LOG_WARN, cf, 0, "[VHF] register Vips Header Filter");

  next = *prev;
  *prev = ngx_http_vips_header_filter;
}

static ngx_int_t ngx_http_vips_header_filter(ngx_http_request_t *r)
{
  ngx_http_vips_loc_conf_t *conf;

  conf = ngx_http_get_module_loc_conf(r, ngx_http_vips_module);
  if (conf->vips == 0)
  {
    ngx_log_error(NGX_LOG_WARN, r->connection->log, 0, "[VHF] skip");
    return next(r);
  }

  ngx_log_error(NGX_LOG_WARN, r->connection->log, 0, "[VHF] process");

  ngx_log_error(NGX_LOG_WARN, r->connection->log, 0, "[VHF] h size: %d", r->header_size);

  return next(r);
}
