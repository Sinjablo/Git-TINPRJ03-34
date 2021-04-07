<?php

    $servername = "192.168.178.73";
    $dbname = "bank_database";
    $username = "example_esp_board";
    $password = "5467";

    $authkey = "de3w2jbn7eif1nw9e";

    $sltl= $mgrkn = $mgpc = "";
    $sltl = test_input($_GET["sltl"]);
    $mgrkn = test_input($_GET["mgrkn"]);
    $mgpc = test_input($_GET["mgpc"]);

    if($sltl != $authkey){
        echo "ERROR";
    }

    if($authkey == $sltl){
        if(strlen($mgpc)==4 && strlen($mgrkn)==17){

            $conn = new mysqli($servername, $username, $password, $dbname);
            // Check connection
            if ($conn->connect_error) {
                die("Connection failed: " . $conn->connect_error);
            }
            
            $sql        =   "SELECT klant_id, pincode, rekeningnummer FROM pincodes WHERE rekeningnummer='$mgrkn'";
            $result     =   $conn->query($sql);
            
            if ($result->num_rows > 0) {
            
            while ($row = $result->fetch_assoc())
            {
                //echo $row['id'];
                //echo $row['value1'];
                //echo " BankrekeningNummer " . $row['rekeningnummer'] . " met user_id: " . $row['user_id'];

                if ($_SERVER["REQUEST_METHOD"] == "GET") {
                    if($authkey == $sltl){
                        //echo "juiste sltl: ".$sltl."\n";
                        if($row['pincode'] == $mgpc){
                            echo true;
                        }
                    }
                } else {
                    echo"\n\tPincode incorrect!";
                }
            }
                
            } else {
                echo "Error:" . $sql . "<br>" . $conn->error;
            }
            
            $conn->close();
        } else {echo "onjuiste gegevens";} //verkeerde lengte pincode
    }
    function test_input($data) {
        $data = trim($data);
        $data = stripslashes($data);
        $data = htmlspecialchars($data);
        return $data;
    }
?>