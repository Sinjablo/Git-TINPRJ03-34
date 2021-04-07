<?php

    $servername = "192.168.178.73";

    // REPLACE with your Database name
    $dbname = "bank_database";
    // REPLACE with Database user
    $username = "example_esp_board";
    // REPLACE with Database user password
    $password = "5467";

    $authkey = "sanderIsNBaas!";

    $sleutel= $meegegevenrekeningnummer = $meegegevenpincode = "";
    $sleutel = test_input($_GET["sleutel"]);

    $conn = new mysqli($servername, $username, $password, $dbname);
    // Check connection
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    }
    
    $sql        =   "SELECT user_id, pincode, rekeningnummer FROM pincodes WHERE rekeningnummer='NL21HAHA001012001'";
    $result     =   $conn->query($sql);
    
    if ($result->num_rows > 0) {
    
    while ($row = $result->fetch_assoc())
    {
        //echo $row['id'];
        //echo $row['value1'];
        echo " BankrekeningNummer " . $row['rekeningnummer'] . " met user_id: " . $row['user_id'];

        if ($_SERVER["REQUEST_METHOD"] == "GET") {
            if($authkey == $sleutel){
            echo "juiste sleutel ".$sleutel."\n";
            if($row['pincode'] == 1232){
                echo "\n\tPincode correct!";
            }
            }
        }
    }
        
    } else {
        echo "Error:" . $sql . "<br>" . $conn->error;
    }
    
    $conn->close();

    function test_input($data) {
        $data = trim($data);
        $data = stripslashes($data);
        $data = htmlspecialchars($data);
        return $data;
    }
?>