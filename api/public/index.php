<?php
chdir('..');
require_once('vendor/autoload.php');
?>
<!doctype html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<style type="text/css">
  body {
    padding: 20px;
  }
  input {
    font-size: 40px;
    display: block;
    width: 100%;
    height: 80px;
    border: 1px #ccc solid;
  }
</style>
</head>
<body>

  <form action="/api.php" method="post">
    <?php
    $recipes = ORM::for_table('recipes')->find_many();
    foreach($recipes as $recipe):
      $ingredients = ORM::for_table('recipe_ingredients')
        ->join('ingredients', ['ingredients.id', '=', 'recipe_ingredients.ingredient_id'])
        ->where('recipe_id', $recipe->id)
        ->order_by_asc('order')
        ->find_many();
      ?>
      <div>
        <input type="submit" name="recipe" value="<?= $recipe->name ?>">
        <span class="cost">
          <?= sprintf("$%.02f", array_sum(array_map(function($g) { 
            return oz_to_ml($g->fluid_oz) * ($g->cost / $g->ml);
          }, $ingredients))) ?>
        </span>
        <span class="ingredients">
          <?= implode(', ', array_map(function($g){ return $g->name; }, $ingredients)) ?>
        </span>
      </div>
      <?php
    endforeach;
    ?>
  </form>

</body>
</html>
