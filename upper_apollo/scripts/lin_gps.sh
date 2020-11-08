#!/usr/bin/env bash

case $1 in
    start)
      nohup python /apollo/modules/control/lin_gps.py >/dev/null 2>&1 &
      ;;
    stop)
      rosnode kill /lin_gps
      ;;
    *)
      nohup python /apollo/modules/control/lin_gps.py >/dev/null 2>&1 &
    ;;
esac

