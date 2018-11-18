<?php
chdir('../..');
require_once('vendor/autoload.php');

$recipe = ORM::for_table('recipes')->create();
$recipe->name = $_POST['name'];

$photo = md5(time().$_FILES['photo']['tmp_name']).'.jpg';
copy($_FILES['photo']['tmp_name'], dirname(__FILE__).'/../images/'.$photo);
$recipe->photo = $photo;

$recipe->save();

header('Location: recipe.php?id='.$recipe->id);
