#
FROM alpine:latest

#MAINTAINER
MAINTAINER flyingyi <flyingyizi@gmail.com>


# Docker Build Arguments
ARG WRK_VERSION="4.0.2"
ARG WRK_LUAJIT_VERSION="2.1.0-beta1"

COPY ./wrk-4.0.2.tar.gz     /tmp/

# Some Environment Variables
ENV HOME /root
# use ustc mirror
RUN sed -i 's/dl-cdn.alpinelinux.org/mirrors.ustc.edu.cn/g' /etc/apk/repositories   \
    && apk update  \
    && apk add --no-cache --virtual .build-deps \
        build-base \
        curl \
        make \
        python-dev \
        libffi-dev \
        openssl-dev   \
        perl \
        git  \
    && apk add --no-cache \
        libgcc \
    && cd /tmp \
    && curl -fSL http://luajit.org/download/LuaJIT-${WRK_LUAJIT_VERSION}.tar.gz   -o LuaJIT-${WRK_LUAJIT_VERSION}.tar.gz \
    && tar xzf LuaJIT-${WRK_LUAJIT_VERSION}.tar.gz \    
    && cd /tmp/LuaJIT-${WRK_LUAJIT_VERSION} \
    && make \
    && make install \
    && ln -sf /usr/local/bin/luajit-${WRK_LUAJIT_VERSION}  /usr/local/bin/luajit \
   && cd /usr/local/include \   
   && ln -s luajit-2.1  luajit-2.0  \   
    && cd /tmp \    
##    && git clone -b ${WRK_VERSION} git@github.com:wg/wrk.git  wrk-${WRK_VERSION} \
##    && cd /tmp/wrk-${WRK_VERSION} \    
    && tar xzf wrk-4.0.2.tar.gz \    
    && cd /tmp/wrk-4.0.2 \
    && make  \    
    && chmod 755 ./wrk    \
    && mkdir -p /opt/wrk \
    && cp wrk /opt/wrk   \
    && cd /tmp/LuaJIT-${WRK_LUAJIT_VERSION} \
    && make uninstall \    
    && cd /tmp \
    && rm -rf  *  \        
    && apk del .build-deps 
    
# Add additional binaries into PATH for convenience
#ENV PATH=$PATH:/usr/local/openresty/luajit/bin/:/usr/local/openresty/nginx/sbin/:/usr/local/openresty/bin/

# Define default command.
ENTRYPOINT ["/opt/wrk/wrk"]


