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

<?
$recipe = ORM::for_table('recipes')->find_one($_GET['id']);
?>

<h2><?= $recipe->name ?></h2>

<form action="save-ingredients.php" method="post">
<table>
<tr>
  <td>Ingredient</td>
  <td>Ounces</td>
  <td>Measurement</td>
  <td>Order</td>
</tr>
<?
$order = 0;
$ingredients = ORM::for_table('recipe_ingredients')
  ->select('recipe_ingredients.*')->select('ingredients.name')
  ->left_outer_join('ingredients', ['ingredients.id', '=', 'recipe_ingredients.ingredient_id'])
  ->where('recipe_id', $_GET['id'])
  ->order_by_asc('order')
  ->find_many();
foreach($ingredients as $g):
  $order = max($order, $g->order);
?>
  <tr>
    <td><?= $g->name ?></td>
    <td><input type="text" value="<?= $g->fluid_oz ?>" name="ingredient[<?= $g->id ?>][fluid_oz]" style="width: 60px;"></td>
    <td><input type="text" value="<?= $g->measurement ?>" name="ingredient[<?= $g->id ?>][measurement]" style="width: 60px;"></td>
    <td><input type="number" value="<?= $g->order ?>" name="ingredient[<?= $g->id ?>][order]" style="width: 30px;"></td>
  </tr>
<?
endforeach;
?>
<tr>
  <td>
    <select name="ingredient[new][ingredient_id]">
      <option value=""></option>
    <?
      $all_ingredients = ORM::for_table('ingredients')->order_by_asc('name')->find_many();
      foreach($all_ingredients as $g) {
        echo '<option value="'.$g->id.'">'.$g->name.'</option>';
      }
    ?>
    </select>
  </td>
  <td><input type="text" value="1" name="ingredient[new][fluid_oz]" style="width: 60px;"></td>
  <td><input type="text" value="1 oz" name="ingredient[new][measurement]" style="width: 60px;"></td>
  <td><input type="number" value="<?= $order+1 ?>" name="ingredient[new][order]" style="width: 30px;"></td>
</tr>
</table>

<input type="hidden" name="recipe_id" value="<?= $recipe->id ?>">
<input type="submit" value="Save">
</form>


<br><br>
<ul>
  <li>Set order=0 to delete an ingredient</li>
</ul>


</body>
</html>