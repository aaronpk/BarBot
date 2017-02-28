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
  $oz = (double)$g->fluid_oz;
  $gravity = (double)$g->gravity;

  $ml = oz_to_ml($oz);
  $grams = round($ml * 1000 / $gravity);
  
  // temporary fix since it seems to be overpouring
  $grams = $grams * 0.85;

  $pumps[] = [
    'number' => (int)$g->number,
    'name' => $g->name,
    'gravity' => (double)$g->gravity,
    'oz' => (double)$g->fluid_oz,
    'ml' => $ml,
    'weight' => $grams
  ];
}

redis()->set('barbot-queue', json_encode([
  'pumps' => $pumps,
  'queue_id' => $queue->id
]));


echo json_encode($pumps, JSON_PRETTY_PRINT);

