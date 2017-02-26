<?php
chdir('..');
require_once('vendor/autoload.php');

while(true) {

  while(!($data=redis()->get('barbot-queue'))) {
    usleep(50000);
  }
  
  redis()->set('barbot-active', 1);

  $job = json_decode($data);
  print_r($job);
  echo "\n";
  
  foreach($job->pumps as $pump) {
    $line = sprintf("%02d %05d go", $pump->number, $pump->weight)."\n";
    echo $line;
    $complete = false;

    sleep(4);

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
  
  redis()->set('barbot-active', 0);

  echo "Completed drink\n";
  redis()->del('barbot-queue');
  sleep(1);
}

