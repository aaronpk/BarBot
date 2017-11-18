<?php
chdir('../..');
require_once('vendor/autoload.php');
?>
<!doctype html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>Recipes</title>
  <link rel="stylesheet" href="/assets/styles.css">
  <style type="text/css">
  body {
    max-width: 600px;
    margin: 0 auto;
    padding-top: 20px;
    font-family: sans-serif;
  }
  .edit-recipe input {
    width: 100%;
  }
  .edit-recipe .field {
    margin-bottom: 10px;
  }
  .edit-recipe .ingredient {
    margin-bottom: 10px;
  }
  .edit-recipe input.amount {
    width: auto;
  }
  </style>
</head>
<body>

<?php
if(isset($_GET['add'])):

  $ingredients = ORM::for_table('ingredients')->order_by_asc('name')->find_many();

  $html = file_get_contents($_GET['add']);
  $data = Mf2\parse($html);
  if($data && isset($data['items']) && count($data['items'])):
    $recipe = $data['items'][0]['properties'];
  ?>

  <form action="/edit/add.php" method="post" class="edit-recipe">

    <div class="field">
      Recipe Name<br>
      <input type="text" name="recipe_name" value="<?= htmlspecialchars($recipe['name'][0]) ?>">
    </div>

    <div class="field">
      Photo<br>
      <input type="url" name="recipe_photo" value="<?= htmlspecialchars($recipe['photo'][0]) ?>">
      <img src="<?= $recipe['photo'][0] ?>" width="120" style="margin: 0 auto; width: 120px; display: block; margin-top: 4px;">
    </div>

    <div class="field">
      Ingredients
      <div>
        <?php 
        foreach($recipe['ingredient'] as $i=>$g):
          if(preg_match('/(.+) - (.+)/', $g, $match)) {
            $name = $match[1];
            $amount = $match[2];
          } else {
            $name = '';
            $amount = '';
          }
          ?>
          <div class="ingredient">
            <div><?= $g ?></div>
            <select name="ingredient[<?= $i ?>]">
              <?php foreach($ingredients as $ing): ?>
                <option value="<?= $ing['id'] ?>" <?= strtolower($name) == strtolower($ing['name']) ? ' selected="selected"' : '' ?>"><?= $ing['name'] ?></option>
              <?php endforeach; ?>
              <option value="0">-- None --</option>
            </select>
            <input type="text" class="amount" name="amount[<?= $i ?>]">
            <input type="hidden" name="ingredient_name[<?= $i ?>]" value="<?= $name ?>">
            <input type="hidden" name="ingredient_amount[<?= $i ?>]" value="<?= $amount ?>">
          </div>
          <?php
        endforeach;
        ?>
      </div>
    </div>

    <hr>

    <input type="submit" value="Add Recipe">
  </form>

  <?php else: ?>
    <p>Could not find an h-recipe on the page</p>
  <?php endif; ?>

  <?php
else:
  ?>
  <form action="/edit/" method="get">
    <div><input type="url" name="add" placeholder="http://example.com/recipe" size="40"></div>
    <button>Parse Recipe</button>
  </form>
  <?php
endif;
?>
</body>
</html>