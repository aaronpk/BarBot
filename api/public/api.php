<?php
chdir('..');
require_once('vendor/autoload.php');

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

