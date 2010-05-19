CONTROLPATH=$1
export PYTHONPATH=$3
export LD_LIBRARY_PATH=$4
NPROC=$5
PIDPATH=$CONTROLPATH/engines/`hostname`

mkdir -p $PIDPATH

case "$2" in
  start) 
         if [ $NPROC ] && [ $NPROC -eq $NPROC ]; then
            NPROC=$NPROC
         else
            NPROC=`cat /proc/cpuinfo | grep ^processor | wc -l`
         fi
         for PROC in `seq 1 $NPROC`
            do
                 PIDFILE=$PIDPATH/ipengine$PROC.pid
                 if [ ! -f $PIDFILE ]; then
                     /sbin/start-stop-daemon --start -b -m               \
                       --pidfile $PIDFILE                                \
                       --exec /opt/pipeline/dependencies/bin/ipengine -- \
                       --furl-file=$CONTROLPATH/engine.furl              \
                       --logfile=$PIDPATH/log
                     sleep 5
                     ps -p `cat $PIDFILE` > /dev/null || echo "ipengine failed to start on `hostname`"
                else
                    echo "ipengine already running on `hostname`"
                fi
            done
         ;;
  stop)
         for PIDFILE in $PIDPATH/ipengine*.pid
             do
                 /sbin/start-stop-daemon --stop --pidfile $PIDFILE --oknodo
                 rm $PIDFILE
             done
         ;;
  *)
         echo "Usage: $0 <controlpath> {start|stop}" >&2
         exit 1
         ;;
esac

exit 0
