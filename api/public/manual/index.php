<?php
chdir('../..');
require_once('vendor/autoload.php');
?>
<!doctype html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>Manual Pour - BarBot</title>
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
    table td.name .number {
      color: #999;
    }
    a {
      color: #111;
    }
  </style>
  <script src="/assets/jquery-3.2.1.min.js"></script>
</head>
<body>

<table id="ingredients">
<?php
  $ingredients = ORM::for_table('ingredients')
    ->select('ingredients.*')->select('pumps.number')
    ->join('pumps', ['ingredients.id', '=', 'pumps.ingredient_id'])
    ->where('available', 1)
    ->order_by_asc('name')
    ->find_many();
  $amounts = [
    'dash' => 0.02,
    '&frac14; oz' => 0.25,
    '&frac12; oz' => 0.5,
    '&frac34; oz' => 0.75,
    '1 oz' => 1,
    '1&frac12; oz' => 1.5,
    '2 oz' => 2
  ];
  foreach($ingredients as $g):
    ?>
    <tr>
      <td class="name">
        <?= $g->name ?><br>
        <span class="number">(Pump <?= $g->number ?>)</span>
      </td>
      <? foreach($amounts as $amt_display=>$amt): ?>
        <td>
          <div><a href="#" data-ingredient="<?= $g->id ?>" data-ingredient-name="<?= $g->name ?>" data-amount="<?= $amt ?>"><?= $amt_display ?></a></div>
          <div><?= sprintf('$%0.02f', calculate_ingredient_cost($g->id, $amt)) ?></div>
        </td>
      <? endforeach; ?>
    </tr>
    <?php
  endforeach;
?>
</table>
<script>
$(function(){
  $("#ingredients td a").click(function(e){
    
    if(confirm("Pour "+$(this).text()+' of '+$(this).data('ingredient-name')+'?')) {
      $.post("/manual/queue.php", {
        ingredient_id: $(this).data('ingredient'),
        amount: $(this).data('amount')
      }, function(response){
        console.log(response);
        alert("Starting now");
      });
    }
    
    e.preventDefault();
  });
});  
</script>
</body>
</html>