<?php

    /* PAS Deblokkeren
    * return 1, pas gedeblokkeerd
    * return 2, pas was al gedeblokkeerd
    * stlt = sleutel , mgrkn = rekeningnummer , mdw = medewerker gebruikersnaam , mdwww = medewerker wachtwoord
    *  
    *
    */

    $servername = "127.0.0.1"; 				    //database ip
    $dbname = "BoejieBank";				        //database name
    $username = "root";					        //database usr
    $password = "#DreamTeam2";				    //database ww

    $authkeyLand = "cPjMPzKzbU2tv9GNwSMb"; 		//key van landserver
    $authkeyBank = "de3w2jbn7eif1nw9e"; 		//key van bank

    $sltl= $mgrkn = $mdw = $mdwww = "";
    $sltl = test_input($_GET["sltl"]);
    $mgrkn = test_input($_GET["mgrkn"]);
    $mdw = test_input($_GET["mdw"]);
    $mdwww = test_input($_GET["mdwww"]);

    if($sltl != $authkeyBank){
        echo "Authkey incorrect!";
    } else if($authkeyBank == $sltl) {

        if(strlen($mdw)<20 && strlen($mdwww)<20 && strlen($mgrkn)==16){

            $conn = new mysqli($servername, $username, $password, $dbname); 							                                                                                //connectie met database

            if ($conn->connect_error) {
                die("Connection failed: " . $conn->connect_error); 								                                                                                        //connectie gefaald
            }

            $sqlMedewerker          =   "SELECT medewerker_gebruikersnaam, medewerker_wachtwoord FROM medewerkers WHERE medewerker_gebruikersnaam='$mdw'"; 		                        //sql querry
            $resultMedewerker       =   $conn->query($sqlMedewerker);											                                                                        //sql querry uitvoeren

            if($resultMedewerker->num_rows > 0) {
                while ($rowMedewerker = $resultMedewerker->fetch_assoc()){
                    if ($_SERVER["REQUEST_METHOD"] == "GET") {
                        if($rowMedewerker['medewerker_wachtwoord'] == $mdwww){
                            $sql        =   "SELECT rekeningnummer, pogingen FROM pincodes WHERE rekeningnummer='$mgrkn'"; 		                            //sql querry
                            $result     =   $conn->query($sql);											                                                    //sql querry uitvoeren
                            if ($result->num_rows > 0) {
                                while ($row = $result->fetch_assoc()){
                                    if ($_SERVER["REQUEST_METHOD"] == "GET") {
                                        if($authkeyBank == $sltl){ 							                                                                //als key van bank is
                                            if($row['pogingen'] != 0){ 									                                                //als nieuw balans nog niet het huidige balans is
                                                $sql        =   "UPDATE pincodes SET `pogingen` = 0 WHERE rekeningnummer= '$mgrkn'"; 	                    //zet nieuw balans
                                                $conn->query($sql); 									                                                    //voer sql uit      
                                                echo "1";										                                                            //geef 1 terug, balans is aangepast
                                            } else if($row['pogingen'] == 0){
                                                echo "2";                                                                                                   //geef 2 terug, nieuw balans was al huidige balans
                                            }
                                        }
                                    }
                                }
                            } else {
                                echo "Error:" . $sql . "<br>" . $conn->error;
                            }
                        }
                    }
                }
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