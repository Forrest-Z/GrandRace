#!/bin/bash
function is_proc_ok()
{
  proc=`ps -ef | grep $1 | grep modules`
  if [ $? -eq 0 ]
  then
    echo -e "$1\t ok."
  else
    echo -e "$1\t nok." 
  fi
}

is_proc_ok perception
is_proc_ok planning 
is_proc_ok control 
is_proc_ok relative_map 
is_proc_ok guardian 
is_proc_ok localization 
is_proc_ok prediction 
is_proc_ok gnss 
