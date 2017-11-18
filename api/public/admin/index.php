<?php
chdir('../..');
require_once('vendor/autoload.php');
?>
<!doctype html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>Recipes</title>
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
  .links li {
    padding: 6px;
  }
  </style>
</head>
<body>

  <ul class="links">
    <li><a href="pumps.php">Configure Pumps</a></li>
    <li><a href="import-recipe.php">Import Recipe</a></li>
    <li><a href="recipes.php">Edit Recipes</a></li>
    <li><a href="/manual/">Manual Pour</a></li>
    <li><a href="/log/">Log</a></li>
  </ul>

</body>
</html>