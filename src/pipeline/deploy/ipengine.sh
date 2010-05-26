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
             TMPFURL=1
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
                 PIDFILE=$PIDPATH/ipengine$PROC.pid
                 if [ ! -f $PIDFILE ]; then
                     /sbin/start-stop-daemon --start -b -m               \
                       --pidfile $PIDFILE                                \
                       --exec /opt/pipeline/dependencies/bin/ipengine -- \
                       --furl-file=$FURLFILE --logfile=$PIDPATH/log
                     start_time=`date +%s`
                     while :
                        do
                            sleep 0.1
                            PID=`cat $PIDFILE 2> /dev/null`
                            grep "engine registration succeeded" $PIDPATH/log$PID.log > /dev/null 2>&1
                            if [ $? -eq 0 ]; then
                                break
                            fi
                            # Time out after 30 seconds
                            end_time=`date +%s`
                            if [ $(($end_time-$start_time)) -ge 30 ]; then
                                break
                            fi
                        done
#                     ps -p `cat $PIDFILE` > /dev/null || echo "ipengine failed to start on `hostname`"
                else
                    echo "ipengine already running on `hostname`"
                fi
            done
         if [ $TMPFURL ]; then
             rm $FURLFILE
         fi
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
