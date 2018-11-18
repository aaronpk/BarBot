<?php
chdir('../..');
require_once('vendor/autoload.php');

$recipe = ORM::for_table('recipes')->find_one($_POST['recipe_id']);
$recipe->enabled = $_POST['enabled'] ? 1 : 0;
$recipe->save();

