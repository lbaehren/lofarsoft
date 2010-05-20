CONTROLPATH=$1
export PYTHONPATH=$3
export LD_LIBRARY_PATH=$4
FURL=$5
NPROC=$6
PIDPATH=$CONTROLPATH/engines/`hostname`

mkdir -p $PIDPATH

case "$2" in
  start) 
         if [ $FURL ]; then
             FURLFILE=`mktemp`
             echo $FURL > $FURLFILE
         else
             FURLFILE=$CONTROLPATH/engine.furl
         fi
         if [ $NPROC ] && [ $NPROC -eq $NPROC ]; then
            NPROC=$NPROC
         else
            NPROC=`cat /proc/cpuinfo | grep ^processor | wc -l`
         fi
         for PROC in `seq 1 $NPROC`
            do
                 cat $FURLFILE
                 PIDFILE=$PIDPATH/ipengine$PROC.pid
                 if [ ! -f $PIDFILE ]; then
                     /sbin/start-stop-daemon --start -b -m               \
                       --pidfile $PIDFILE                                \
                       --exec /opt/pipeline/dependencies/bin/ipengine -- \
                       --furl-file=$FURLFILE --logfile=$PIDPATH/log
                     sleep 5
                     ps -p `cat $PIDFILE` > /dev/null || echo "ipengine failed to start on `hostname`"
                else
                    echo "ipengine already running on `hostname`"
                fi
            done
         rm $FURLFILE
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
