# This tag use ubuntu 14.04
FROM redis:alpine

#MAINTAINER
MAINTAINER flyingyi <flyingyizi@gmail.com>

# Some Environment Variables
ENV HOME /root
# use ustc mirror
RUN sed -i 's/dl-cdn.alpinelinux.org/mirrors.ustc.edu.cn/g' /etc/apk/repositories   \
    && apk update  \
    && apk --update add ruby ruby-rdoc ruby-irb supervisor git   \
    && /usr/bin/gem install tiller  \
    && /usr/bin/gem install redis  \
    && git clone -b 3.0.6 https://gitee.com/mirrors/redis.git  /redis

# Build redis from source
#RUN (cd /redis && make)

RUN mkdir /redis-conf && \
    mkdir /redis-data  && \
    mkdir -p /var/log/supervisor/ && \
    mkdir -p /etc/supervisor.d

# Add all config files for all clusters
ADD ./redis_conf-template.conf /redis-conf/redis.conf

# Add supervisord configuration
ADD ./supervisord-template.conf /etc/supervisor.d/supervisord.ini

ADD ./start.sh /start.sh
RUN chmod 755 /start.sh

#
#EXPOSE 7000 7001 7002 7003 7004 7005 7006 7007

CMD ["/bin/sh", "/start.sh"]
ENTRYPOINT ["/bin/sh", "/start.sh"]
