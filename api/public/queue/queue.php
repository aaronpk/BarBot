<?php
chdir('../..');
require_once('vendor/autoload.php');

?>

<?php
$queue = ORM::for_table('log')
  ->select('log.id', 'log_id')
  ->select_expr('log.*')
  ->select_many('recipes.name', 'recipes.photo')
  ->select_many('users.username')
  ->join('recipes', ['recipes.id', '=', 'log.recipe_id'])
  ->join('users', ['users.id', '=', 'log.user_id'])
  ->where_null('date_started')
  ->where_null('date_finished')
  ->order_by_asc('date_queued')
  ->find_many();

if(count($queue) == 0) {
  echo '<h2>Visit http://bar.bot<br>to add a drink!</h2>';
} else {
  echo '<div class="help" style="text-align: center;">Tap to make the drink!</div>';
}

foreach($queue as $recipe):
  ?>
    <div class="menu-item" data-id="<?= $recipe->id ?>" data-name="<?= $recipe->name ?>">
      <span class="photo"><img src="/images/<?= $recipe->photo ?>"></span>
      <span class="details">
        <span>
          <span class="name"><?= $recipe->name ?></span>
          <span class="cost">
            <?= sprintf("$%.02f", $recipe->cost) ?>
          </span>
        </span>
        <span class="for-user">
          for <?= $recipe->username ?>
        </span>
        <span class="delete"><a href="javascript:deleteFromQueue(<?= $recipe->log_id ?>)">&times;</a></span>
      </span>
    </div>
  <?php
endforeach;

