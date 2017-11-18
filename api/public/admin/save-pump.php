<?php
chdir('../..');
require_once('vendor/autoload.php');

$pump = ORM::for_table('pumps')->find_one($_POST['pump']);
$pump->ingredient_id = $_POST['ingredient'] ?: null;
$pump->save();

