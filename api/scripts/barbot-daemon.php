<?php
chdir('..');
require_once('vendor/autoload.php');

while(!($data=redis()->get('barbot-queue'))) {
  usleep(50000);
}

echo $data."\n";

