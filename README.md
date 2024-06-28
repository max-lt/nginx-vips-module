# nginx-vips-module

VIPS module for nginx

## Development env
### Copy nginx source code into working directory
```bash
# copy nginx source code into dev/src (for editor completion)
make setup-dev-headers

# For MacOS, remove NGX_HAVE_BPF and replace NGX_LINUX by NGX_DARWIN in dev/objs/ngx_auto_headers.h
```

## Build image
```bash
make docker-image
```

## Run image
```bash
make docker-run
```
