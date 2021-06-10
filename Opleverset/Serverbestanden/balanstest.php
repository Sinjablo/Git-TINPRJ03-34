<?php

$url = 'https://145.24.222.66:8000/';

//Initiate cURL.
$ch = ($url);

//The JSON data.
$jsonData = array(
    'body' =>  array(
					'account' => 'KC21BOEB06102001'
					'pin' => '5467'
					),
    'header' => array(
					'receiveCountry' => 'KC'
					'receiveBankName' => 'BOEB'
					)
);

//$jsonDataEncoded = "body": {"pin": "5467", "account": "KC21BOEB06102001"},"header": {"receiveCountry": "KC", "receiveBankName": "BOEB"}
//$jsonDataEncoded = json_encode("{'body': {'pin': '8824', 'account': 'KC21FAIR52172584'},'header': {'receiveCountry': 'KC', 'receiveBankName': 'FAIR'}}");

//Encode the array into JSON.
$jsonDataEncoded = json_encode($jsonData);

//Tell cURL that we want to send a POST request.
curl_setopt($ch, CURLOPT_POST, 1);

//Attach our encoded JSON string to the POST fields.
curl_setopt($ch, CURLOPT_POSTFIELDS, $jsonDataEncoded);

//Set the content type to application/json
curl_setopt($ch, CURLOPT_HTTPHEADER, array('Content-Type: application/json')); 

//Execute the request
$result = ($ch);

echo $result;


/*
$url = 'http://145.24.222.66:8000/';

$jsonDataEncoded = json_encode("{'body': {'pin': '8824', 'account': 'KC21FAIR52172584'},'header': {'receiveCountry': 'KC', 'receiveBankName': 'FAIR'}}");

//Initiate cURL.
$ch = curl_init($url);

curl_setopt($ch, CURLOPT_URL, $url);

//Tell cURL that we want to send a POST request.
//curl_setopt($ch, CURLOPT_GET, true);

//Attach our encoded JSON string to the POST fields.
curl_setopt($ch, CURLOPT_POSTFIELDS, http_build_query($jsonDataEncoded));

//Set the content type to application/json
//curl_setopt($ch,CURLOPT_URL,$url);
//curl_setopt($ch, CURLOPT_HTTPHEADER, array('Content-Type:application/json'));
//curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);

// Return response instead of outputting
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);

//Execute the request
$response = curl_exec($ch);

// Close cURL resource
//curl_close($ch);

echo $jsonDataEncoded;

if($response==null){
    echo "leeg";
}

echo $response;

$response = json_decode(file_get_contents('php://input'), true);

echo $response;

echo file_get_contents('php://input');

*/
?>