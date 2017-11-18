<?php
chdir('../..');
require_once('vendor/autoload.php');

$active = redis()->get('barbot-active');
if($active) {
  echo json_encode([
    'error' => 'in progress'
  ]);
  die();
}

$queue = ORM::for_table('log')->create();
$queue->ingredient_id = $_POST['ingredient_id'];
$queue->date_queued = date('Y-m-d H:i:s');
$queue->user_agent = $_SERVER['HTTP_USER_AGENT'];
$queue->cost = calculate_ingredient_cost($_POST['ingredient_id'], $_POST['amount']);
$queue->save();

$pumps = [];

$ingredient = ORM::for_table('ingredients')
  ->join('pumps', ['pumps.ingredient_id', '=', 'ingredients.id'])
  ->find_one($_POST['ingredient_id']);

$pumps[] = build_redis_queue_item($ingredient, $_POST['amount']);

redis()->set('barbot-queue', json_encode([
  'pumps' => $pumps,
  'queue_id' => $queue->id
]));

echo json_encode($pumps, JSON_PRETTY_PRINT);
