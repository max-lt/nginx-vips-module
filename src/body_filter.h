#ifndef _NGX_VIPS_BODY_FILTER_H_INCLUDED_
#define _NGX_VIPS_BODY_FILTER_H_INCLUDED_

#include <ngx_core.h>
#include <ngx_http.h>

void register_vips_body_filter(ngx_conf_t *cf, ngx_http_output_body_filter_pt *prev);

#endif
