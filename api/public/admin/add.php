<?php
chdir('../..');
require_once('vendor/autoload.php');

$recipe = ORM::for_table('recipes')->create();
$recipe->name = $_POST['recipe_name'];

$photo = basename($_POST['recipe_photo']);
$img = file_get_contents($_POST['recipe_photo']);
file_put_contents(dirname(__FILE__).'/../images/'.$photo, $img);

$recipe->photo = $photo;
$recipe->save();

foreach($_POST['ingredient'] as $i=>$ingredient_id) {
  if($_POST['amount'][$i] !== '-') {
    $ingredient = ORM::for_table('recipe_ingredients')->create();
    $ingredient->recipe_id = $recipe->id;
    $ingredient->ingredient_id = $ingredient_id;
    $ingredient->fluid_oz = (double)$_POST['amount'][$i];
    $ingredient->order = $i;
    $ingredient->ingredient_name = $_POST['ingredient_name'][$i];
    $ingredient->measurement = $_POST['ingredient_amount'][$i];
    $ingredient->save();
  }
}

header('Location: /admin/recipe.php?id='.$recipe->id);
