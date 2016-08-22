BASE_PATH=$(cd `dirname $0`; pwd)
PORT=9876
SERVER_PID="`netstat -apn | grep ${PORT} | grep LISTEN | awk '{print $7}'|sed 's/\/server//g'`"
LOG_DIR="/root/logs"
TAG="`date +%Y-%m-%d`"
EXT_LIB_PATH="${BASE_PATH}/depend/libs"
SERVER_BIN="${BASE_PATH}/build/release/server"
echo $SERVER_PID
echo $TAG

exe_cmd(){
$cmd
if [ $? -eq 0 ];then
    echo "Execute cmd [$cmd] done."
else
    echo "Execute cmd [$cmd] failed."
    exit 0
fi
}

if [ $1 = "help" ];
then
    echo "help: get help information."
    echo "stop: stop current server."
    echo "start: start server."
    echo "status: get server status."
elif [ $1 = "stop" ];
then
    echo "Stop server."
    if [ -z ${SERVER_PID} ];
    then
	echo "Server not running."
    else
	cmd="kill -s 10 ${SERVER_PID}"
	exe_cmd
    fi
elif [ $1 = "compile" ];
then
    echo "Recompile code."
    cmd="cd ${BASE_PATH} && make clean && make"
    exe_cmd
    cmd="mv ${BASE_PATH}/build/bin/server ${SERVER_BIN}"
    exe_cmd
elif [ $1 = "start" ];
then
    echo "Start server."
    if [ -z ${SERVER_PID} ];
    then
	# ...
	cmd="cd ${BASE_PATH} && export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${EXT_LIB_PATH}; ${SERVER_BIN} > server.out"
	echo $cmd
	cd ${BASE_PATH} && export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${EXT_LIB_PATH}; ${SERVER_BIN} > server.out
    else
	echo "Start stopped. Server is running."
    fi
elif [ $1 = "status" ];
then
    if [ -z ${SERVER_PID} ];
    then
	echo "Server not runnig."
    else
	echo "Server runnig on port ${PORT} with PID[${SERVER_PID}]"
    fi
else
    echo "Invalid param. Use help to get usage."
fi
