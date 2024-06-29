#ifndef _NGX_VIPS_HEADER_FILTER_H_INCLUDED_
#define _NGX_VIPS_HEADER_FILTER_H_INCLUDED_

#include <ngx_core.h>
#include <ngx_http.h>

void register_vips_header_filter(ngx_conf_t *cf, ngx_http_output_header_filter_pt *prev);

#endif
