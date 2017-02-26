<?php
chdir('../..');
require_once('vendor/autoload.php');

$queue = ORM::for_table('log')
  ->where('id', $_POST['queue_id'])
  ->where_null('date_started')
  ->find_one();
$queue->delete();
