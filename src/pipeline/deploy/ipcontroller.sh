CONTROLPATH=$1
export PYTHONPATH=$3

mkdir -p $CONTROLPATH

case "$2" in
  start) 
         /sbin/start-stop-daemon --start -b -m --pidfile $CONTROLPATH/ipc.pid \
           --exec /opt/pipeline/dependencies/bin/ipcontroller -- -xy \
           --engine-furl-file=$CONTROLPATH/engine.furl \
           --task-furl-file=$CONTROLPATH/task.furl \
           --multiengine-furl-file=$CONTROLPATH/multiengine.furl \
           --logfile=$CONTROLPATH/ipc.log
         ;;
  stop)
         /sbin/start-stop-daemon --stop --pidfile $CONTROLPATH/ipc.pid
         ;;
  *)
         echo "Usage: $0 <controlpath> {start|stop}" >&2
         exit 1
         ;;
esac

exit 0
