<?php
require_once(dirname(__FILE__).'/config.php');

ORM::configure('mysql:host=' . Config::$dbHost . ';dbname=' . Config::$dbName);
ORM::configure('username', Config::$dbUsername);
ORM::configure('password', Config::$dbPassword);  

function redis() {
  static $client = false;
  if(!$client)
    $client = new Predis\Client('tcp://127.0.0.1:6379');
  return $client;
}

function oz_to_ml($oz) {
  return $oz * 29.5735;
}

function ml_to_oz($ml) {
  return $ml / 29.5735;
}
