scriptpath=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

ngx_version=1.27.0
ngx_image=${ngx_version}-alpine3.19
ngx_dir=/usr/src/nginx-${ngx_version}

cleanup-dev-headers:
	@rm -rf ${scriptpath}/dev/{src,objs}

docker-dev-headers-image: Dockerfile.headers
	@docker build  -f Dockerfile.headers -t nginx-headers .

setup-dev-headers: cleanup-dev-headers docker-dev-headers-image
	@docker run nginx-headers tar -c ${ngx_dir}/src ${ngx_dir}/objs | tar x -C ${scriptpath}/dev --strip-components 3 "*.h"

docker-image:
	@docker build -t foo-nginx .

docker-run:
	@docker run --rm --name foo-nginx-container -p 8000:80 -v ${scriptpath}/dev/nginx.conf:/etc/nginx/nginx.conf:ro foo-nginx

clear:
	@rm -r $(keys_path)
