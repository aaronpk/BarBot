<?php
chdir('../..');
require_once('vendor/autoload.php');

$current = ORM::for_table('log')
  ->join('recipes', ['recipes.id', '=', 'log.recipe_id'])
  ->join('users', ['users.id', '=', 'log.user_id'])
  ->where_not_null('date_started')
  ->where_null('date_finished')
  ->find_one();

if($current): 

$ingredients = ORM::for_table('recipe_ingredients')
  ->join('ingredients', ['ingredients.id', '=', 'recipe_ingredients.ingredient_id'])
  ->where('recipe_id', $current->recipe_id)
  ->order_by_asc('order')
  ->find_many();

?>

<div class="dispensing">
  <h1>Dispensing</h1>

  <div class="name">
    <?= $current->name ?>
    <span class="cost"><?= sprintf("$%.02f", $current->cost) ?></span>
  </div>
  <div class="for-user">for <?= $current->username ?></div>

  <div class="ingredients">
    <?= implode(', ', array_map(function($g){ return $g->name; }, $ingredients)) ?>
  </div>

  <img src="/images/<?= $current->photo ?>" class="photo">
</div>

<?php else: ?>

<div class="dispensing">
</div>
<div class="last-made">
<?php
$last = ORM::for_table('log')
  ->join('recipes', ['recipes.id', '=', 'log.recipe_id'])
  ->join('users', ['users.id', '=', 'log.user_id'])
  ->where_not_null('date_started')
  ->where_not_null('date_finished')
  ->order_by_desc('date_finished')
  ->find_one();
if($last):
?>

  <h2>Last Made</h2>
  <div class="name">
    <?= $last->name ?>
    <span class="cost"><?= sprintf("$%.02f", $last->cost) ?></span>
  </div>
  <div class="for-user">for <?= $last->username ?></div>

<?php
endif;
?>
</div>

<?php endif ?>