<?php
chdir('..');
require_once('vendor/autoload.php');

$pumps = [];
$pumps[] = [
  'number' => 2,
  'weight' => 10000  
];

redis()->set('barbot-queue', json_encode([
  'pumps' => $pumps,
  'queue_id' => 0
]));
