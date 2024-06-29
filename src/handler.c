#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#include "ngx_http_vips_module.h"

ngx_int_t ngx_http_vips_handler(ngx_http_request_t *r)
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
