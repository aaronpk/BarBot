<?php
require_once(dirname(__FILE__).'/config.php');

ORM::configure('mysql:host=' . Config::$dbHost . ';dbname=' . Config::$dbName . ';charset=utf8');
ORM::configure('username', Config::$dbUsername);
ORM::configure('password', Config::$dbPassword);

function new_db() {
  return new PDO('mysql:host=' . Config::$dbHost . ';dbname=' . Config::$dbName . ';charset=utf8', Config::$dbUsername, Config::$dbPassword);
}

function redis() {
  static $client = false;
  if(!$client)
    $client = new Predis\Client('tcp://127.0.0.1:6379');
  return $client;
}

function oz_to_ml($oz) {
  return $oz * 29.5735;
}

function ml_to_oz($ml) {
  return $ml / 29.5735;
}

function calculate_drink_cost($id) {
  $ingredients = ORM::for_table('recipe_ingredients')
    ->join('ingredients', ['ingredients.id', '=', 'recipe_ingredients.ingredient_id'])
    ->where('recipe_id', $id)
    ->find_many();
  return array_sum(array_map(function($g) { 
      return oz_to_ml($g->fluid_oz) * ($g->cost / $g->ml);
    }, $ingredients));
}

function calculate_ingredient_cost($id, $fluid_oz) {
  $ingredient = ORM::for_table('ingredients')->find_one($id);
  return oz_to_ml($fluid_oz) * ($ingredient->cost / $ingredient->ml);
}

function build_redis_queue_item($ingredient, $fluid_oz) {
  $oz = (double)$fluid_oz;
  $gravity = (double)$ingredient->gravity;

  $ml = oz_to_ml($oz);
  $grams = round($ml * 1000 / $gravity);
  
  // temporary fix since it seems to be overpouring
  $grams = $grams * 0.85;

  return [
    'number' => (int)$ingredient->number,
    'name' => $ingredient->name,
    'gravity' => (double)$ingredient->gravity,
    'oz' => (double)$fluid_oz,
    'ml' => $ml,
    'weight' => $grams
  ];
}

function tz() {
  static $tz;
  if(!isset($tz))
    $tz = new DateTimeZone('America/Los_Angeles');
  return $tz;
}
