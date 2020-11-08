#!/usr/bin/env bash

CMD="python /apollo/modules/control/chassis_control.py"

function start() {
    NUM_PROCESSES="$(pgrep -c -f "${CMD}")"
    if [ "${NUM_PROCESSES}" -eq 0 ]; then
       eval "nohup "${CMD}" >/dev/null 2>&1 &"
    fi
}

function stop() {
    pid=$(ps -ef | grep "${CMD}" | grep -v 'grep' | awk '{print $2}')
    if [ "${pid}" ]; then
       kill -9 ${pid}
    fi
}

function run() {
    case $1 in
        start)
            start
            ;;
        stop)
            stop
            ;;
        *)
            start
            ;;
    esac
}

run "$1"