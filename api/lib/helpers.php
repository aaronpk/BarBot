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
