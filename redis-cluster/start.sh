supervisord
sleep 3

STATUS=$(echo "CLUSTER INFO" | redis-cli  | grep "cluster_state:ok"| wc -l)
if [ $STATUS -eq 1 ];
then
    exit
fi

IP=`ifconfig | grep "inet addr:17" | cut -f2 -d ":" | cut -f1 -d " "`
IP=127.0.0.1
tail -f /var/log/supervisor/redis.log
