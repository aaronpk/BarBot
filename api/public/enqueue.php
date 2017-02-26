<?php
chdir('..');
require_once('vendor/autoload.php');

if(!isset($_POST['recipe_id']) || !isset($_POST['username'])) {
  die("Invalid Request");
}

$user = ORM::for_table('users')
  ->where('username', $_POST['username'])
  ->find_one();
if(!$user) {
  $user = ORM::for_table('users')->create();
  $user->username = $_POST['username'];
  $user->date_created = date('Y-m-d H:i:s');
  $user->remote_addr = $_SERVER['REMOTE_ADDR'];
  $user->user_agent = $_SERVER['HTTP_USER_AGENT'];
  $user->save();
}

$queue = ORM::for_table('log')->create();
$queue->recipe_id = $_POST['recipe_id'];
$queue->user_id = $user->id;
$queue->date_queued = date('Y-m-d H:i:s');
$queue->user_agent = $_SERVER['HTTP_USER_AGENT'];
$queue->cost = calculate_drink_cost($_POST['recipe_id']);
$queue->save();

echo json_encode([
  'result' => 'queued'
]);
