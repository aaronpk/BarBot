<?php
chdir('../..');
require_once('vendor/autoload.php');
?>
<!doctype html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>BarBot - Manual Pour</title>
  <link rel="stylesheet" href="/assets/styles.css">
  <style type="text/css">
    body {
      font-family: "AmericanTypewriter", "Baskerville", "Roboto";
      background: #fdf5e8;
      margin: 0;
      padding: 0;
    }
    table {
      width: 100vw;
      height: 100vh;
    }
    table td {
      text-align: center;
    }
    table td.name {
      text-align: left;
    }
    a {
      color: #111;
    }
  </style>
</head>
<body>

<table>
<?php
  $ingredients = ORM::for_table('ingredients')
    ->select('ingredients.*')
    ->join('pumps', ['ingredients.id', '=', 'pumps.ingredient_id'])
    ->where('available', 1)
    ->order_by_asc('name')
    ->find_many();
  foreach($ingredients as $g):
    ?>
    <tr>
      <td class="name"><?= $g->name ?></td>
      <td>
        <div><a href="javascript:dispense(<?= $g->id ?>, 0.02);">dash</a></div>
        <div><?= sprintf('$%0.02f', calculate_ingredient_cost($g->id, 0.02)) ?></div>
      </td>
      <td>
        <div><a href="javascript:dispense(<?= $g->id ?>, 0.25);">&frac14; oz</a></div>
        <div><?= sprintf('$%0.02f', calculate_ingredient_cost($g->id, 0.25)) ?></div>
      </td>
      <td>
        <div><a href="javascript:dispense(<?= $g->id ?>, 0.5);">&frac12; oz</a></div>
        <div><?= sprintf('$%0.02f', calculate_ingredient_cost($g->id, 0.5)) ?></div>
      </td>
      <td>
        <div><a href="javascript:dispense(<?= $g->id ?>, 0.75);">&frac34; oz</a></div>
        <div><?= sprintf('$%0.02f', calculate_ingredient_cost($g->id, 0.75)) ?></div>
      </td>
      <td>
        <div><a href="javascript:dispense(<?= $g->id ?>, 1);">1 oz</a></div>
        <div><?= sprintf('$%0.02f', calculate_ingredient_cost($g->id, 1)) ?></div>
      </td>
      <td>
        <div><a href="javascript:dispense(<?= $g->id ?>, 1.5);">1&frac12; oz</a></div>
        <div><?= sprintf('$%0.02f', calculate_ingredient_cost($g->id, 1.5)) ?></div>
      </td>
      <td>
        <div><a href="javascript:dispense(<?= $g->id ?>, 2);">2 oz</a></div>
        <div><?= sprintf('$%0.02f', calculate_ingredient_cost($g->id, 2)) ?></div>
      </td>
    </tr>
    <?php
  endforeach;
?>
</table>
  
</body>
</html>