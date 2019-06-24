<?php

require_once 'auth.php';

//Make sure that it is a POST request.
if(strcasecmp($_SERVER['REQUEST_METHOD'], 'POST') != 0){ 
    echo('Request method must be POST!');
    throw new Exception('Request method must be POST!');
} 
 
//Make sure that the content type of the POST request has been set to application/json
$contentType = isset($_SERVER["CONTENT_TYPE"]) ? trim($_SERVER["CONTENT_TYPE"]) : '';
if(strcasecmp($contentType, 'application/json') != 0){
    echo('Content type must be: application/json<br>');
    echo($contentType);
    throw new Exception('Content type must be: application/json');
}

//Receive the RAW post data.
$content = trim(file_get_contents("php://input"));
 
//Attempt to decode the incoming RAW post data from JSON.
$decoded = json_decode($content, true);
 
//If json_decode failed, the JSON is invalid.
if(!is_array($decoded)){
   echo('Received content contained invalid JSON!');
    throw new Exception('Received content contained invalid JSON!');
}

$headers = apache_request_headers();
if($headers['Authorization'] == $SECRETKEY){
    echo "Logging:";
//    Add current timestamp
    $decoded['timestamp'] = time();
    print_r($decoded);
    file_put_contents("data.txt",json_encode($decoded).",\n", FILE_APPEND);
} else{
    echo('Nope!');
    throw new Exception('Bad Auth');
}




?>
