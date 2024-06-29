#ifndef _NGX_VIPS_VAR_VERSION_H_INCLUDED_
#define _NGX_VIPS_VAR_VERSION_H_INCLUDED_

#include <ngx_core.h>
#include <ngx_http.h>

ngx_int_t ngx_http_vips_get_version(ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);

#endif
