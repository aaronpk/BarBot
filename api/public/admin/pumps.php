<?php
chdir('../..');
require_once('vendor/autoload.php');
?>
<!doctype html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>Configure Pumps</title>
  <link rel="stylesheet" href="/assets/styles.css">
  <style type="text/css">
  body {
    max-width: 600px;
    margin: 0 auto;
    padding-top: 20px;
    font-family: sans-serif;
    background: #fdf5e8;
    color: #111;
  }
  td {
    padding: 10px 0;
  }
  .pump-number {
    border: 1px #333 solid;
    border-radius: 4px;
    width: 40px;
    height: 40px;
    display: block;
    display: flex;
    align-items: center;
    justify-content: center;
    margin: 0 auto;
  }
  </style>
  <script src="/assets/jquery-3.2.1.min.js"></script>
</head>
<body>

<h2>Configure Pumps</h2>

<table class="pumps">
<?php

$ingredients = ORM::for_table('ingredients')->order_by_asc('name')->find_many();

function show_pump($pump) {
  global $ingredients;
?>
  <td>
    <span class="pump-number"><span><?= $pump->number ?></span></span>
    <select data-pump="<?= $pump->number ?>">
      <?
      $found = false;
      foreach($ingredients as $ing):
        $selected = (strtolower($pump->name) == strtolower($ing['name']));
        $found = $found || $selected;
        echo '<option value="' . $ing['id'] . '" ' . ($selected ? ' selected="selected"' : '' ) . '">' . $ing['name'] . '</option>';
      endforeach;
      echo '<option value="0" ' . (!$found ? ' selected="selected"' : '' ) . '>-- None --</option>';
      ?>
    </select>
  </td>
<?php
}

$pumps = ORM::for_table('pumps')
  ->left_outer_join('ingredients', ['pumps.ingredient_id','=','ingredients.id'])
  ->order_by_asc('pumps.number')
  ->find_many();
foreach($pumps as $i=>$pump):
  if($i % 4 == 0) echo '<tr>'."\n";
  show_pump($pump);
  if($i % 4 == 3) echo '</tr>'."\n";
endforeach;
?>
</table>
<script>
$(function(){
  
  $(".pumps select").change(function(){
    console.log($(this).data("pump")+" = "+$(this).val());
    $.post("/edit/save-pump.php", {
      pump: $(this).data("pump"),
      ingredient: $(this).val()
    });
  });
  
  
});
</script>
</body>
</html>