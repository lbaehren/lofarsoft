#!/bin/bash
#
# ssh_cluster_action - this script performs command (first arg)
#                      on a list of nodes (sub5 cluster) (next args should be
#                      just a numbers of nodes, like 38 40 43
#                      option -p with expression like 38-41, 45)
#                      using SSH
#                      if command is a command with arguments it
#                      should be enclosed in quotes
#

if [ $# == 0 ]; then
 echo "Usage: `basename $0` <command> 38 40 43"
 echo "         -OR-"
 echo "       `basename $0` <command> -p 38-41,45"
 echo "         -OR-"
 echo "       `basename $0` <command>"
 echo
 echo "Instead of <command> you can use some predefined command aliases, namely:"
 echo "   -top     - prints two top processes on selected nodes"
 echo "              (this is alias for 'top -b -n 1 | head -n 9 | tail -n 3')"
 echo "   -df      - prints usage of /data directories"
 echo "              (this is alias for 'df -H | grep data | grep -v offline | grep -v net')"
 echo
 echo "If there is no nodes' list given then command will be executed in all lse and lce nodes in sub5"
 echo "If node number is between 13 and 15 then script recognizes it as storage node (lse)"
 echo "otherwise the node is considered to be compute node (lce)"
 exit 0
fi

# ssh options
sshopts="-X -T -q"
bold="\033[1m"
ul="\033[4m"
none="\033[0m"

# set the action
if [ "$1" == "-top" ]; then
 action="top -b -n 1 | head -n 9 | tail -n 3"
elif [ "$1" == "-df" ]; then
 action="df -H | grep data | grep -v offline | grep -v net"
 df -H | grep Filesystem
else 
 action=$1
fi

# shift the args list
shift 1

if [ "$1" == "-p" ] || [ $# == 0 ]; then
 if [ $# == 0 ]; then
  string="13-15,38-45"
 else
  shift 1
  string=$1
 fi
 parts=`echo $string | cut -d \, --output-delimiter=" " -f 1-`
 for n in $parts; do
  isdash=`echo $n | grep - | wc -l`
  # no range of nodes is specified
  if [ $isdash == 0 ]; then
   if [ $n -ge 13 ] && [ $n -le 15 ]; then
    node="lse0"$n
    echo -e $bold"$node..."$none
    ssh $sshopts "$node" $action
   elif [ $n -ge 38 ] && [ $n -le 45 ]; then
    node="lce0"$n
    echo -e $bold"$node..."$none
    ssh $sshopts "$node" $action
   else
    echo -e $bold"$node - non-sub5 node..."$none
   fi
  # if the range of nodes is specified
  else
   begin=`echo $n | cut -d - -f 1`
   end=`echo $n | cut -d - -f 2`
   for (( i=$begin ; $i<=$end ; i++ )); do 
    if [ $i -ge 13 ] && [ $i -le 15 ]; then
     node="lse0"$i
     echo -e $bold"$node..."$none
     ssh $sshopts "$node" $action
    elif [ $i -ge 38 ] && [ $i -le 45 ]; then
     node="lce0"$i
     echo -e $bold"$node..."$none
     ssh $sshopts "$node" $action
    else
     echo -e $bold"$node - non-sub5 node..."$none
    fi
   done
  fi
 done
else
 for n in $@; do
  if [ $n -ge 13 ] && [ $n -le 15 ]; then
   node="lse0"$n
   echo -e $bold"$node..."$none
   ssh $sshopts "$node" $action
  elif [ $n -ge 38 ] && [ $n -le 45 ]; then
   node="lce0"$n
   echo -e $bold"$node..."$none
   ssh $sshopts "$node" $action
  else
   echo -e $bold"$node - non-sub5 node..."$none
  fi
 done
fi
