<?php
chdir('../..');
require_once('vendor/autoload.php');

header('Content-type: application/json');

echo redis()->get('barbot-status');


