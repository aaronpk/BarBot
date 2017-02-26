<?php
chdir('../..');
require_once('vendor/autoload.php');
?>
<!doctype html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>BarBot Queue</title>
  <link rel="stylesheet" href="/assets/styles.css">
  <meta name="apple-mobile-web-app-capable" content="yes">
  <meta name="mobile-web-app-capable" content="yes">
  <meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">
<style type="text/css">
.barbot .current {
  color: #fff;
}
</style>
</head>
<body class="barbot">
  <div class="current" id="current">
  </div>
  <div class="queue">
    <h1>Queued</h1>

    <div class="queue-list" id="queue-list">
    </div>
  </div>
<script>
showQueue();
showCurrent();

function bindListeners() {
  var elements = document.querySelectorAll(".menu-item");
  Array.prototype.forEach.call(elements, function(el, i){
    el.addEventListener("click", function(el){
      var queue_id = el.target.closest(".menu-item").dataset.id;
      startFromQueue(queue_id);
    });
  });  
}

function showQueue() {
  var request = new XMLHttpRequest();
  request.open('GET', '/queue/queue.php', true);
  request.onload = function() {
    if(request.status >= 200 && request.status < 400) {
      document.getElementById("queue-list").innerHTML = request.responseText;
      bindListeners();
    }
    setTimeout(showQueue, 2500);
  };
  request.send();
}

function showCurrent() {
  var request2 = new XMLHttpRequest();
  request2.open('GET', '/queue/current.php', true);
  request2.onload = function() {
    if(request2.status >= 200 && request2.status < 400) {
      document.getElementById("current").innerHTML = request2.responseText;
    }
    setTimeout(showCurrent, 1100);
  };
  request2.send();
}

function startFromQueue(id) {
  if(confirm("Start this drink now?")) {
    var request = new XMLHttpRequest();
    request.open('POST', '/queue/start.php', true);
    request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded; charset=UTF-8');
    request.send("queue_id="+id);
  }
}

function deleteFromQueue(id) {
  if(confirm("Delete this drink from the queue?")) {
    var request = new XMLHttpRequest();
    request.open('POST', '/queue/delete.php', true);
    request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded; charset=UTF-8');
    request.send("queue_id="+id);
  }
}

</script>
</body>
</html>
