<?php
chdir('../..');
require_once('vendor/autoload.php');
?>
<!doctype html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>BarBot Log</title>
  <link rel="stylesheet" href="/assets/styles.css">
<style type="text/css">
  body {
    padding: 10px;
    margin: 0;
    background: #fdf5e8;
    color: #111;
    font-size: 20px;
    font-family: sans-serif;
  }
  .log {
    min-width: 320px;
    border: 6px double #444;
    padding: 10px;
  }
  table {
    border-collapse: collapse;
  }
  table td {
    margin: 0;
    padding: 2px 6px;
    border: 1px #eee solid;
  }
</style>
</head>
<body>

<?php
$total = ORM::for_table('log')
  ->where('billable', 1)
  ->sum('cost');
?>

<div class="log">

  <div style="font-size: 24pt;">$<?= sprintf('%.02f', $total) ?></div>
  
  <table>
  <?php
  $logs = ORM::for_table('log')
    ->select('log.id', 'logid')
    ->select_expr('users.*')
    ->select_expr('recipes.*')
    ->select_expr('log.*')
    ->left_outer_join('recipes', ['recipes.id', '=', 'log.recipe_id'])
    ->join('users', ['users.id', '=', 'log.user_id'])
    ->order_by_desc('date_finished')
    ->find_many();
  foreach($logs as $log):
  ?>
    <tr>
      <td><?= $log->username ?></td>
      <td><?= $log->name ?></td>
      <td>$<?= sprintf('%.02f', $log->cost) ?></td>
      <td><?= (new DateTime($log->date_finished ?: $log->date_queued))->setTimeZone(tz())->format('M j, Y g:ia') ?></td>
      <td><input type="checkbox" data-log-id="<?= $log->logid ?>" class="billable" <?= $log->billable ? 'checked="checked"' : '' ?>></td>
    </tr>
  <?php
  endforeach;
  ?>
  </table>
</div>

<script>
for(var el of document.querySelectorAll(".billable")) {
  el.addEventListener("click", function(evt){
    save_billable(evt.target.dataset['logId'], evt.target.checked);
  });
};

function save_billable(id, val) {
  var request = new XMLHttpRequest();
  request.open('POST', '/log/billable.php', true);
  request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded; charset=UTF-8');
  request.send("id="+id+"&billable="+(val ? 1 : 0));
}
</script>  

</body>
</html>