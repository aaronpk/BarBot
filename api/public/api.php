<?php
chdir('..');
require_once('vendor/autoload.php');


// Check if a job is currently running

$active = redis()->get('barbot-active');
if($active) {
  echo json_encode([
    'error' => 'in progress'
  ]);
  die();
}

$pumps = [];

$recipe = ORM::for_table('recipes')
  ->where('name', $_POST['recipe'])
  ->find_one();

$ingredients = ORM::for_table('recipe_ingredients')
  ->join('ingredients', ['ingredients.id', '=', 'recipe_ingredients.ingredient_id'])
  ->join('pumps', ['pumps.ingredient_id', '=', 'ingredients.id'])
  ->where('recipe_id', $recipe->id)
  ->order_by_asc('order')
  ->find_many();

foreach($ingredients as $g) {
  $oz = (double)$g->fluid_oz;
  $gravity = (double)$g->gravity;

  $ml = oz_to_ml($oz);
  $grams = round($ml * 1000 / $gravity);

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
  'pumps' => $pumps
]));


echo json_encode($pumps, JSON_PRETTY_PRINT);
die();



/*
$recipes = [
  'boulevardier' => [
    [
      'number' => 8,
      'weight' => 21310
    ],
    [
      'number' => 13,
      'weight' => 42620,
    ],
    [
      'number' => 15,
      'weight' => 21310
    ],
    [
      'number' => 7,
      'weight' => 500
    ]
  ]
];

if(array_key_exists($_POST['recipe'], $recipes)) {
  $recipe = $recipes[$_POST['recipe']];
  
  redis()->set('barbot-queue', json_encode([
    'pumps' => $recipe
  ]));
  
  echo "Making a ".$_POST['recipe']."\n";
} else {
  echo "Recipe Not Found\n";
}
*/

