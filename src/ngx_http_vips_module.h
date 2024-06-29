#ifndef _NGX_VIPS_CONFIG_H_INCLUDED_
#define _NGX_VIPS_CONFIG_H_INCLUDED_

#include <ngx_core.h>

typedef struct
{
  ngx_flag_t vips;
} ngx_http_vips_loc_conf_t;

typedef struct
{
  off_t original_size;
  off_t reduced_size;
} ngx_http_vips_filter_ctx_t;

extern ngx_module_t ngx_http_vips_module;

#endif
