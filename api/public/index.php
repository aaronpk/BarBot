<?php
chdir('..');
require_once('vendor/autoload.php');
?>
<!doctype html>
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>BarBot</title>
  <script src="/assets/jscookie.js"></script>
  <link rel="stylesheet" href="/assets/styles.css">
<style type="text/css">
  body {
    padding: 10px;
    margin: 0;
    background: #fdf5e8;
    color: #111;
    font-size: 22px;
  }
  .menu {
    min-width: 320px;
    border: 6px double #444;
    padding: 10px;
  }
  .username {
    border-top: 1px dotted #222;
    margin-top: 20px;
    padding-top: 6px;
    font-size: 12px;
  }
  .username a {
    color: #222;
  }
</style>
</head>
<body>

<div class="menu">

  <h1>Cocktails</h1>

  <?php
  $recipes = ORM::for_table('recipes')
    ->where('enabled', 1)
    ->order_by_asc('name')
    ->find_many();
  foreach($recipes as $recipe):
    $ingredients = ORM::for_table('recipe_ingredients')
      ->left_outer_join('ingredients', ['ingredients.id', '=', 'recipe_ingredients.ingredient_id'])
      ->left_outer_join('pumps', ['ingredients.id', '=', 'pumps.ingredient_id'])
      ->where('recipe_id', $recipe->id)
      ->order_by_asc('order')
      ->find_many();
    $all_ingredients_present = array_reduce($ingredients, function($carry, $g){
      return $carry && $g->available;
    }, true);
    ?>
    <div class="menu-item<?= $all_ingredients_present ? '' : ' missing' ?>" data-id="<?= $recipe->id ?>" data-name="<?= $recipe->name ?>">
      <span class="photo"><img src="/images/<?= $recipe->photo ?>"></span>
      <span class="details">
        <span>
          <span class="name"><?= $recipe->name ?></span>
          <span class="cost">
            <?= sprintf("$%.02f", array_sum(array_map(function($g) { 
              if(!$g->ml) return 0;
              return oz_to_ml($g->fluid_oz) * ($g->cost / $g->ml);
            }, $ingredients))) ?>
          </span>
        </span>
        <span class="ingredients">
          <?= implode(', ', array_map(function($g){
            $str = '';
            if(!$g->number) {
              $str .= '<span class="not-in-cabinet">';
              $str .= $g->measurement . ' ';
            }
            $str .= $g->name ?: $g->ingredient_name;
            if(!$g->number)
              $str .= '</span>';
            return $str;
          }, $ingredients)) ?>
        </span>
      </span>
    </div>
    <?php
  endforeach;
  ?>

  <div class="username hidden">
    Hello, <span></span>. Not you? <a href="javascript:logout()">Sign out</a>.
  </div>
</div>
<script>

var username = null;
username = Cookies.get("username");

showLoggedInUser();

var elements = document.querySelectorAll(".menu-item");
Array.prototype.forEach.call(elements, function(el, i){
  if(!el.classList.contains("missing")) {
    el.addEventListener("click", function(el){
      var drink_id = el.target.closest(".menu-item").dataset.id;
      var drink_name = el.target.closest(".menu-item").dataset.name;
  
      if(username == null) {
        while(username == null) {
          username = prompt("What is your name?");
          if(username == null) {
            break;
          } else {
            if(username.match(/^[A-Za-z0-9_\-\.]+$/) !== null) {
              Cookies.set("username", username, { expires: 30 });
              showLoggedInUser();
            } else {
              alert("Only letters, numbers and _-. are allowed. Try again.");
              username = null;
            }
          }
        }
      }
  
      if(username && confirm("Add a \""+drink_name+"\" to the queue?")) {
  
        var request = new XMLHttpRequest();
        request.open('POST', '/enqueue.php', true);
        request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded; charset=UTF-8');
        request.send("recipe_id="+drink_id+"&username="+username);
  
        alert("Got it! Visit BarBot to start your drink!");
      }
  
    });
  }
});

function logout() {
  username = null;
  Cookies.remove("username");
  document.querySelector(".username span").innerHTML = "";
  addClass(document.querySelector(".username"), "hidden");
}

function showLoggedInUser() {
  if(username) {
    document.querySelector(".username span").innerHTML = username;
    removeClass(document.querySelector(".username"), "hidden");
  }
}

function addClass(el, className) {
  if (el.classList)
    el.classList.add(className);
  else
    el.className += ' ' + className;  
}

function removeClass(el, className) {
  if (el.classList)
    el.classList.remove(className);
  else
    el.className = el.className.replace(new RegExp('(^|\\b)' + className.split(' ').join('|') + '(\\b|$)', 'gi'), ' ');  
}

</script>
</body>
</html>
