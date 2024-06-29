#include <ngx_core.h>
#include <ngx_http.h>
#include <vips/vips.h>

ngx_int_t ngx_http_vips_get_version(ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data)
{
  v->len = sizeof(VIPS_VERSION_STRING) - 1;
  v->data = (u_char *)VIPS_VERSION_STRING;

  v->valid = 1;
  v->no_cacheable = 0;
  v->not_found = 0;

  return NGX_OK;
}
