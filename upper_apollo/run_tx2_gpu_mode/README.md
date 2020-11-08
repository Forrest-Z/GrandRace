Follow blow steps to run turtlebot3_integration automatically when system booting(suppose you have built the project 'turtle-suite'(source:git git@bitbucket.org:goldenridgerobotic/turtle-suite.git) successfully in your ros workspace):

step1:
复制'ros_tx2_gpu_mode.sh'文件 到你的工作目录

step2:
复制 tx2_gpu_mode.service 文件到 系统目录 /etc/systemd/system/

step3:Reload systemd manager configuration in a terminal
$systemctl daemon-reload

step4:Ebable tx2_gpu_mode.service automatic actived when system booting
$systemctl enable tx2_gpu_mode.service

step5:If roscore in your system is actived automatically when system booting,disable it(replace 'roscored.service' in blow command with your roscore service name)
$systemctl disable roscored.service

step6:
重启系统并且检查 tx2_gpu_mode.service 是否执行
$systemctl status tx2_gpu_mode.service
