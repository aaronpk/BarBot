<?php
$port = '/dev/ttyACM0';

shell_exec('stty -F '.$port.' cs8 57600 ignbrk -brkint -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts');
