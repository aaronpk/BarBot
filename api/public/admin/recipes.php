<?php
chdir('../..');
require_once('vendor/autoload.php');
?>
<!doctype html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>Edit Recipes</title>
  <link rel="stylesheet" href="/assets/styles.css">
  <style type="text/css">
  body {
    font-family: "AmericanTypewriter", "Baskerville", "Roboto";
    max-width: 600px;
    margin: 0 auto;
    padding-top: 20px;
    background: #fdf5e8;
    color: #111;
  }
  td {
    padding: 10px 0;
  }
  </style>
  <script src="/assets/jquery-3.2.1.min.js"></script>
</head>
<body>

<h2>Edit Recipes</h2>


  <?php
  $recipes = ORM::for_table('recipes')
    ->order_by_asc('name')
    ->find_many();
  foreach($recipes as $recipe):
    $ingredients = ORM::for_table('recipe_ingredients')
      ->left_outer_join('ingredients', ['ingredients.id', '=', 'recipe_ingredients.ingredient_id'])
      ->left_outer_join('pumps', ['ingredients.id', '=', 'pumps.ingredient_id'])
      ->where('recipe_id', $recipe->id)
      ->order_by_asc('order')
      ->find_many();
    $all_ingredients_present = array_reduce($ingredients, function($carry, $g){
      return $carry && $g->available;
    }, true);
    ?>
    <div class="menu-item<?= $all_ingredients_present ? '' : ' missing' ?>" data-id="<?= $recipe->id ?>" data-name="<?= $recipe->name ?>">
      <span class="photo"><img src="/images/<?= $recipe->photo ?>"></span>
      <span class="details">
        <span>
          <span class="name"><a href="recipe.php?id=<?= $recipe->id ?>"><?= $recipe->name ?></a></span>
          <span class="cost">
            <?= sprintf("$%.02f", array_sum(array_map(function($g) { 
              if(!$g->ml) return 0;
              return oz_to_ml($g->fluid_oz) * ($g->cost / $g->ml);
            }, $ingredients))) ?>
          </span>
        </span>
        <span class="ingredients">
          <?= implode(', ', array_map(function($g){
            $str = '';
            if(!$g->number) {
              $str .= '<span class="not-in-cabinet">';
              $str .= $g->measurement . ' ';
            }
            $str .= $g->name ?: $g->ingredient_name;
            if(!$g->number)
              $str .= '</span>';
            return $str;
          }, $ingredients)) ?>
        </span>
      </span>
    </div>
    <?php
  endforeach;
  ?>

  <br><br>
  
  <h3>New Recipe</h3>
  <form action="create-recipe.php" method="post">
    <input type="text" name="name">
    <input type="file" name="photo">
    <input type="submit" value="Create">
  </form>

  <br><br>

  <h3>Import from h-recipe</h3>
  <form action="/edit/import-recipe.php" method="get">
    <div><input type="url" name="add" placeholder="http://example.com/recipe" size="40"></div>
    <button>Parse Recipe</button>
  </form>

  <br><br>
  <br><br>

</body>
</html>