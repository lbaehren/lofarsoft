CONTROLPATH=$1
export PYTHONPATH=$3

mkdir -p $CONTROLPATH

PIDFILE=$CONTROLPATH/ipc.pid

case "$2" in
  start) 
         if [ ! -f $PIDFILE ]; then
             /sbin/start-stop-daemon --start -b -m --pidfile $PIDFILE \
               --exec /opt/pipeline/dependencies/bin/ipcontroller -- -xy \
               --engine-furl-file=$CONTROLPATH/engine.furl \
               --task-furl-file=$CONTROLPATH/task.furl \
               --multiengine-furl-file=$CONTROLPATH/multiengine.furl \
               --logfile=$CONTROLPATH/ipc.log
         else
            echo "ipcontroller already running on `hostname`"
         fi
         ;;
  stop)
         /sbin/start-stop-daemon --stop --pidfile $PIDFILE
         rm $PIDFILE
         ;;
  *)
         echo "Usage: $0 <controlpath> {start|stop}" >&2
         exit 1
         ;;
esac

exit 0
