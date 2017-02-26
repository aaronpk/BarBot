<?php
chdir('..');
require_once('vendor/autoload.php');

ini_set("auto_detect_line_endings", true);

$port = '/dev/ttyACM0';

#shell_exec('stty -F '.$port.' cs8 57600 ignbrk -brkint -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts');

$ser = fopen($port, "w+");

sleep(2);
echo "Connecting...\n";

if(!$ser) {
  echo "failed to connect to serial port\n";
  sleep(1);
  die();
}

echo "Connected\n";

/*
$ready = false;
while(($line = fgets($ser)) !== false && !$ready) {
  redis()->publish('barbot-output', $line);
  echo $line;
  if(trim($line) == '{"mode":"ready"}')
    $ready = true;
}
*/

echo "BarBot Initialized\n";

while(true) {

  while(!($data=redis()->get('barbot-queue'))) {
    usleep(50000);
  }
  
  redis()->set('barbot-active', 1);

  $job = json_decode($data);
  print_r($job);
  echo "\n";
  
  foreach($job->pumps as $pump) {
    echo "Pump $pump->number Weight $pump->weight\n";
    fwrite($ser, sprintf("%02d %05d go", $pump->number, $pump->weight)."\r");
    $complete = false;
    while(($line = fgets($ser)) !== false && !$complete) {
      $data = @json_decode($line);
      if(trim($line)) {
        redis()->publish('barbot-output', $line);
        redis()->set('barbot-status', $line);
      }
      if($data && property_exists($data, 'mode') && $data->mode == 'complete') {
        #print_r($data);
        $complete = true;
        echo "Finished!\n";
        sleep(1);
      }
    }
  }
  
  redis()->set('barbot-active', 0);

  echo "Completed drink\n";
  redis()->del('barbot-queue');
  sleep(1);
}
