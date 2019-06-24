<?php

require_once 'auth.php';


header("Access-Control-Allow-Origin: *");
//header("Access-Control-Allow-Credentials: true");
header("Access-Control-Max-Age: 1000");
header("Access-Control-Allow-Headers: X-Requested-With, Content-Type, Origin, Cache-Control, Pragma, Authorization, Accept, Accept-Encoding");
header("Access-Control-Allow-Methods: PUT, POST, GET, OPTIONS, DELETE");



$headers = apache_request_headers();
$key = $headers['Authorization'] ;
if($key == $SECRETKEY){
    $content = file_get_contents('./data.txt');
    $json_output = "[".rtrim($content, ",\n")."]";
    echo($json_output);
} else{
    echo('Nope!');
    throw new Exception('Bad Auth');
}

?>
