<?php

    /* Instroom geld
    * return "aantal briefjes in geldlade", als het gelukt is
    * 
    * 
    * stlt = sleutel , brfj = waarde van briefje , hvhbrfj = hoeveel briefjes worden opgenomen , mdw = medewerker gebruikersnaam , mdwww = medewerker wachtwoord
    *  
    *
    */

    $servername = "127.0.0.1"; 				    //database ip
    $dbname = "BoejieBank";				        //database name
    $username = "root";					        //database usr
    $password = "#DreamTeam2";				    //database ww

    $authkeyLand = "cPjMPzKzbU2tv9GNwSMb"; 		//key van landserver
    $authkeyBank = "de3w2jbn7eif1nw9e"; 		//key van bank

    $sltl= $brfj = $hvhbrfj = $mdw = $mdwww = "";
    $sltl = test_input($_GET["sltl"]);
    $brfj = test_input($_GET["brfj"]);
    $hvhbrfj = test_input ($_GET["hvhbrfj"]);
    $mdw = test_input($_GET["mdw"]);
    $mdwww = test_input($_GET["mdwww"]);

    if($sltl != $authkeyBank){
        echo "Authkey incorrect!";
    } else if($authkeyBank == $sltl) {

        if(strlen($mdw)<20 && strlen($mdwww)<20 && strlen($brfj) <= 2 && strlen($hvhbrfj) <3){

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
                            $sql        =   "SELECT aantal_briefjes, briefwaarde FROM geldinmachine WHERE briefwaarde ='$brfj'"; 		                            //sql querry
                            $result     =   $conn->query($sql);											                                                    //sql querry uitvoeren
                            if ($result->num_rows > 0) {
                                while ($row = $result->fetch_assoc()){
                                    if ($_SERVER["REQUEST_METHOD"] == "GET") {
                                        if($authkeyBank == $sltl){ 							                                                                //als key van bank is
                                                $NieuwAantal = ($row['aantal_briefjes']) + ($hvhbrfj);
                                                $sql        =   "UPDATE geldinmachine SET `aantal_briefjes` = '$NieuwAantal'  WHERE briefwaarde = '$brfj'";
                                                $conn->query($sql);
                                                echo $NieuwAantal;
                                        }
                                        
                                    }
                                }
                            } else {
                                echo "Briefwaarde niet gevonden" . $sql . "<br>" . $conn->error;
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