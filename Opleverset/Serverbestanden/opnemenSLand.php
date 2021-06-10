<?php

    /*
    * return balans, alles correct
    * 
    */

//=======Url landserver
$url = "http://145.24.222.66:8001/";

//=======Sleutels
$authkeyLand = "cPjMPzKzbU2tv9GNwSMb"; 		                    //key van landserver
$authkeyBank = "de3w2jbn7eif1nw9e"; 		                    //key van bank

//=======URL data ophalen
$sltl = $mgrkn = $mgpc = $bdg = $mgld = $mgbk ="";
$sltl = test_input($_GET["sltl"]);                              //sleutel
$mgrkn = test_input($_GET["mgrkn"]);                            //rekeningnummer
$mgpc = test_input($_GET["mgpc"]);                              //pincode (4cijfers)
$bdg = test_input($_GET["bdg"]);                                //bedrag
$mgld = test_input($_GET["mgld"]);                              //land
$mgbk = test_input($_GET["mgbk"]);                              //bank (4Letters)

if($sltl != $authkeyLand && $sltl != $authkeyBank){
    echo "Authkey incorrect!";
} else if($authkeyLand == $sltl || $authkeyBank == $sltl) {
    $data = array("body" => array("account" => $mgrkn, "pin" =>$mgpc, "amount" =>$bdg), "header" => array("receiveCountry" => $mgld, "receiveBankName" => $mgbk));

    $data_string = json_encode($data);

    //========cURL process
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $url);                        //url meegeven
    curl_setopt($ch, CURLOPT_POSTFIELDS, $data_string);         //Json meegeven
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);            //verificatie
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);             //returnwaarde opslaan
    curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "POST");            //methode (post)
    curl_setopt($ch, CURLOPT_HTTPHEADER, array(                 //return soort
        'Content-Type: application/json',
        'Content-Length: ' . strlen($data_string))
    );

    $responseJson = curl_exec($ch);                             //cURL uitvoeren

    $response = json_decode($responseJson);                     //decode response
    //print_r($response);                                         //print hele response Json


    //print_r($response->body);                                   //print body van response Json 
    $body = $response->body;                                    //selecteer body van response Json
    //print_r($body);                                             //print body van response Json
    $ReturnValue = $body->{'Return Value'};                     //selecteer Return Value van response Json
    //print_r($ReturnValue);                                      //print Return Value van response Json

    if($ReturnValue == "Pincode wrong"){
        echo "F1";
    } else if($ReturnValue == "Account not registered"){
        echo "F7";
    } else if($ReturnValue == "Json wrong"){
        echo "F8";
    } else if($ReturnValue == "Account blocked"){
        echo "F0";
    } else if($ReturnValue == "OK"){
        echo "1";
    } else {
        echo "F8";
    }

    curl_close($ch);                                            //afsluiten connectie cURL
}

//======URL parameters bewerken
function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}
?>