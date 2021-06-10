<?php

    /*
    * return balans, als gelukt
    * return x5, rekening niet gevonden
    * 
    * 
    * 
    */

    $servername = "127.0.0.1"; 				    //database ip
    $dbname = "BoejieBank";				        //database name
    $username = "root";					        //database usr
    $password = "#DreamTeam2";				    //database ww

    $authkeyLand = "cPjMPzKzbU2tv9GNwSMb"; 		//key van landserver
    $authkeyBank = "de3w2jbn7eif1nw9e"; 		//key van bank

    $sltl= $mgrkn = $mgpc = $bdg = "";
    $sltl = test_input($_GET["sltl"]);
    $mgrkn = test_input($_GET["mgrkn"]);
    $mgpc = test_input($_GET["mgpc"]);
    $bdg = test_input($_GET["bdg"]);

    $NieuwBalans = 0;

    if($sltl != $authkeyLand && $sltl != $authkeyBank){
        echo "Authkey incorrect!";
    } else if($authkeyLand == $sltl || $authkeyBank == $sltl) {

        if(strlen($mgpc)==4 && strlen($mgrkn)==16){

            $conn = new mysqli($servername, $username, $password, $dbname); 							                                        //connectie met database

            if ($conn->connect_error) {
                die("Connection failed: " . $conn->connect_error); 								                                                //connectie gefaald
            }
            
            $sql        =   "SELECT klant_id, pincode, rekeningnummer, pogingen, balans FROM pincodes WHERE rekeningnummer='$mgrkn'"; 		    //sql querry
            $result     =   $conn->query($sql);											                                                        //sql querry uitvoeren
            
            if ($result->num_rows > 0) {
            
                while ($row = $result->fetch_assoc()){

                    if ($_SERVER["REQUEST_METHOD"] == "GET") {
                        if($authkeyLand == $sltl || $authkeyBank == $sltl){ 							                                        //als key 1 van de keys is
                                    if($row['balans'] >= $bdg){
                                        $NieuwBalans = ($row['balans']) + ($bdg);
                                        $sql        =   "UPDATE pincodes SET `balans` = '$NieuwBalans' WHERE rekeningnummer= '$mgrkn'";         //pas balans aan
                                        $conn->query($sql); 									                                                //voer sql uit 
                                        echo $NieuwBalans;
                                    } else {
                                        echo "x5";                                                                                              //rekening niet gevonden
                                    }
                        }
                    }
                }
            } else {
                echo "Error:" . $sql . "<br>" . $conn->error;
            }
            
            $conn->close();
        } else {echo "onjuiste gegevens";} 											                                                            //verkeerde lengte pincode
    }
    function test_input($data) {
        $data = trim($data);
        $data = stripslashes($data);
        $data = htmlspecialchars($data);
        return $data;
    }
?>