<?php
chdir('../..');
require_once('vendor/autoload.php');

header('Content-type: application/json');


// Check if a job is currently running

$active = redis()->get('barbot-active');
if($active) {
  echo json_encode([
    'error' => 'in progress'
  ]);
  die();
}


$queue = ORM::for_table('log')
  ->where('id', $_POST['queue_id'])
  ->where_null('date_started')
  ->find_one();

if(!$queue) {
  echo json_encode([
    'error' => 'Could not find item'
  ]);
  die();
}

$recipe = ORM::for_table('recipes')
  ->where('id', $queue->recipe_id)
  ->find_one();

$ingredients = ORM::for_table('recipe_ingredients')
  ->join('ingredients', ['ingredients.id', '=', 'recipe_ingredients.ingredient_id'])
  ->join('pumps', ['pumps.ingredient_id', '=', 'ingredients.id'])
  ->where('recipe_id', $recipe->id)
  ->order_by_asc('order')
  ->find_many();

$pumps = [];

foreach($ingredients as $g) {
  $pumps[] = build_redis_queue_item($g, $g->fluid_oz);
}

redis()->set('barbot-queue', json_encode([
  'pumps' => $pumps,
  'queue_id' => $queue->id
]));


echo json_encode($pumps, JSON_PRETTY_PRINT);


if($queue->user_id == 2) {
  // Send to YNAB and my website
  $cost = round($queue->cost, 2);
  $name = $recipe->name;
  
  $http = new p3k\HTTP();
  
  $today = new DateTime();
  $today->setTimeZone(new DateTimeZone('US/Pacific'));
  
  $http->post(Config::$micropubEndpoint, json_encode([
    'type' => ['h-entry'],
    'properties' => [
      'summary' => ['Just drank: '.$name],
      'drank' => [[
        'type' => ['h-food'],
        'properties' => [
          'name' => [$name]
        ]
      ]]
    ]
  ]), [
    'Content-type: application/json',
    'Authorization: Bearer '.Config::$micropubToken,
  ]);
  
  $http->post('https://api.youneedabudget.com/v1/budgets/'.Config::$ynabBudgetID.'/transactions', json_encode([
    'transaction' => [
      'account_id' => Config::$ynabAccountID,
      'date' => $today->format('Y-m-d'),
      'amount' => round($cost*(-1000)),
      'payee_id' => Config::$ynabPayeeID,
      'category_id' => Config::$ynabCategoryID,
      'memo' => $name,
      'cleared' => 'cleared',
      'approved' => false,
      'import_id' => 'barbot:'.$queue->id,
    ]
  ]), [
    'Content-type: application/json',
    'Authorization: Bearer '.Config::$ynabToken
  ]);
  
}

