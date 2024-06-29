FROM nginx:1.27.0-alpine3.19-slim as base

FROM base as builder

ARG VIPS_MODULE_PATH=/usr/local/lib/ngx-vips-module

# Install build dependencies (should we use musl-dev instead of libc-dev?)
RUN apk add --no-cache --virtual .build-deps \
  # nginx
  gcc \
  libc-dev \
  make \
  linux-headers \
  curl \
  vips-dev

# Download and extract nginx source code
RUN mkdir -p /usr/src \
  && curl -fSL http://nginx.org/download/nginx-${NGINX_VERSION}.tar.gz | tar -zxC /usr/src

ADD config $VIPS_MODULE_PATH/config
ADD src $VIPS_MODULE_PATH/src

RUN ln -sf /usr/lib/glib-2.0/include/glibconfig.h /usr/include/glib-2.0/glibconfig.h

RUN cd /usr/src/nginx-${NGINX_VERSION} \
  && ./configure \
    --with-compat \
    --add-dynamic-module=$VIPS_MODULE_PATH \
  && make modules

# Add module to base image
FROM base as nginx-vips

RUN apk add vips

COPY --from=builder /usr/src/nginx-${NGINX_VERSION}/objs/ngx_http_vips_module.so /usr/lib/nginx/modules/ngx_http_vips_module.so
