#!/bin/sh
set -e  #"Exit immediately if a simple command exits with a non-zero status."
#
# This script is meant for quick & easy install via:
#   'curl -sSL http://acs-public-mirror.oss-cn-hangzhou.aliyuncs.com/docker-engine/internet | sh'
#

IMAGE_REPOSITORY="txq/redis-cluster"

#CID_FILE="/tmp/grokzen-redis-cluster.cid"
#CID=`cat $(CID_FILE)`

# check whether a command exist
command_exists() {
	command -v "$@" > /dev/null 2>&1
}

# param1: dockerfile path
# param2: image tag, optional
# example: "build_image  apache/  apacheTag"
build_image() {

	IMAGE="${IMAGE_REPOSITORY}:latest"
    if [ ! -z $2 ] ; then 
	   IMAGE=${IMAGE_REPOSITORY}:$2
	fi

    if command_exists docker; then
     	docker build -t $IMAGE $1
		 # docker build --no-cache=true -t ${IMAGE_REPOSITORY}:$1 $2
    fi    

}

# param1: the image tag that needed update
#
# example "upload_image_if_needed apache", "upload_image_if_needed monitoring"
upload_image_if_needed() {
    if [[ $(ssh $REMOTE_USERNAME@$REMOTE_HOST "docker images $IMAGE_REPOSITORY | grep $1 | tr -s ' ' | cut -d ' ' -f 3") != $(docker images $IMAGE_REPOSITORY | grep $1 | tr -s ' ' | cut -d ' ' -f 3) ]]
    then
        echo "$1 image changed, updating..."
        docker save $IMAGE_REPOSITORY:$1 | bzip2 | pv | ssh $REMOTE_USERNAME@$REMOTE_HOST 'bunzip2 | docker load'
    else
        echo "$1 image did not change"
    fi
}


# param1:  hostPort:containerPort. 
# param2:  image tag. optional
do_run()  {
	IMAGE="${IMAGE_REPOSITORY}:latest"
    if [ ! -z $2 ] ; then 
	   IMAGE=${IMAGE_REPOSITORY}:$2
	fi
	CONTAINER_NAME="`echo $1$IMAGE | sed 's/:/_/g ; s/\//_/g  ' `"

	echo "Running docker image...with port=$1."

    if command_exists docker; then
        docker run  -i -t -d -p $1 --name $CONTAINER_NAME  ${IMAGE}  echo "$1 redis"
    fi
}



#if [ ! -z $1 ] ; then docker_version=$1; fi

if [ $# -lt 2 ] ; then
echo "USAGE: $0 cmd"
echo " e.g.: $0 [build|run|clean]"
exit 1;
fi 

case $1 in
	build)
	    if [ $# -lt 2 ] ; then
		echo "USAGE: $0 build path [image-tag]  "
		exit  1;
		fi
		build_image $2 $3
	;;
	run)
	    if [ $# -lt 2 ] ; then
		echo "USAGE: $0 run  hostPort:containerPort [tag]"
		exit  1;
		fi
		do_run $2 $3
	;;
	*)
	#	if command_exists lsb_release; then
	#		dist_version="$(lsb_release --codename | cut -f2)"
	#	fi
	;;
esac


