export PYTHONPATH="/home/swinbank/apps/lib/python2.5/site-packages:/opt/pyrap/lib"
export PATH="/home/swinbank/apps/bin:/usr/local/bin:/usr/bin:/bin"
export LD_LIBRARY_PATH="/home/swinbank/apps/lib/:/opt/pyrap/lib:/opt/casacore/lib"
CONTROLPATH=$1

mkdir -p $CONTROLPATH

case "$2" in
  start) 
         /sbin/start-stop-daemon --start -b -m --pidfile $CONTROLPATH/ipc.pid \
           --exec /home/swinbank/apps/bin/ipcontroller -- -xy \
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
