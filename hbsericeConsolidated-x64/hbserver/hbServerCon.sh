#/bin/bash
#
# the client of hbla
#
#author:yg
#time:2015-11-23 15:21
EXEC_PATH=/usr/local/hbserver/
EXEC=run_hbSC
PID_FILE=/var/run/hbla.pid
DAEMON=/usr/local/hbserver/run_hbSC
if ! [ -x $EXEC_PATH/$EXEC ] ; then
       echo "ERROR: $EXEC_PATH/$EXEC not found"
       exit 1
fi
start()
{
       echo "Starting $EXEC ..."
       $DAEMON > /dev/null &
       sleep 1
       echo "Starting $EXEC: [  OK  ]"        
}
stop()
{
       echo "Stoping $EXEC ..."
       killall -9 run_hbSC >/dev/null &
       sleep 1
       killall -9 hbServerCon >/dev/null &
       sleep 1
       echo "Shutting down $EXEC: [  OK  ]"     
}
case "$1" in
	start)
	start
	;;
	stop)
	stop
	;;
	status)
	status -p $PID_FILE $DAEMON
	;;
	*)
	exit 1
esac
exit $?
