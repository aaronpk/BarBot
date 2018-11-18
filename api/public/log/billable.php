<?php
chdir('../..');
require_once('vendor/autoload.php');

$log = ORM::for_table('log')->find_one($_POST['id']);
$log->billable = $_POST['billable'];
$log->save();
