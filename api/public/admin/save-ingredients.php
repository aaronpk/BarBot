<?php
chdir('../..');
require_once('vendor/autoload.php');

foreach($_POST['ingredient'] as $id=>$data) {
  if($id == 'new') {
    if($data['ingredient_id']) {
      $ingredient = ORM::for_table('recipe_ingredients')->create();
      $ingredient->recipe_id = $_POST['recipe_id'];
      $ingredient->ingredient_id = $data['ingredient_id'];
      $ingredient->fluid_oz = $data['fluid_oz'];
      $ingredient->measurement = $data['measurement'];
      $ingredient->order = $data['order'];
      $ingredient->save();
    }
  } else {
    $ingredient = ORM::for_table('recipe_ingredients')->find_one($id);
    if($data['order'] == 0) {
      $ingredient->delete();
    } else {
      $ingredient->fluid_oz = $data['fluid_oz'];
      $ingredient->measurement = $data['measurement'];
      $ingredient->order = $data['order'];
      $ingredient->save();
    }
  }
}

header('Location: /admin/recipe.php?id='.$_POST['recipe_id']);
