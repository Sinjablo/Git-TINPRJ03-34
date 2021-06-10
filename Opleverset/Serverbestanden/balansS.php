<?php

    /*
    * return balans, pincode goed
    * return 2, pincode eerste keer fout
    * return 3, pincode tweede keer fout
    * return 4, pas geblokkeerd
    * returned balans als String
    */

    $servername = "127.0.0.1"; 				    //database ip
    $dbname = "BoejieBank";				        //database name
    $username = "root";					        //database usr
    $password = "#DreamTeam2";				    //database ww

    $authkeyLand = "cPjMPzKzbU2tv9GNwSMb"; 		//key van landserver
    $authkeyBank = "de3w2jbn7eif1nw9e"; 		//key van bank

    $sltl= $mgrkn = $mgpc = "";
    $sltl = test_input($_GET["sltl"]);
    $mgrkn = test_input($_GET["mgrkn"]);
    $mgpc = test_input($_GET["mgpc"]);

    if($sltl != $authkeyLand && $sltl != $authkeyBank){
        echo "Authkey incorrect!";
    } else if($authkeyLand == $sltl || $authkeyBank == $sltl) {

        if(strlen($mgpc)==4 && strlen($mgrkn)==16){

            $conn = new mysqli($servername, $username, $password, $dbname); 							                                        //connectie met database

            if ($conn->connect_error) {
                die("Connection failed: " . $conn->connect_error); 								                                                //connectie gefaald
            }
            
            $sql        =   "SELECT klant_id, pincode, rekeningnummer, pogingen, balans FROM pincodes WHERE rekeningnummer='$mgrkn'"; 		                    //sql querry
            $result     =   $conn->query($sql);											                                                        //sql querry uitvoeren
            
            if ($result->num_rows > 0) {
            
                while ($row = $result->fetch_assoc()){

                    if ($_SERVER["REQUEST_METHOD"] == "GET") {
                        if($authkeyLand == $sltl || $authkeyBank == $sltl){ 							                                        //als key 1 van de keys is
                            if($row['pogingen'] < 3){ 										                                                    //als pogingen minder is dan 3
                                if($row['pincode'] == $mgpc){ 									                                                //als pincode juist is
                                    $sql        =   "UPDATE pincodes SET `pogingen` = '0' WHERE rekeningnummer= '$mgrkn'"; 	                    //reset pogingen
                                    $conn->query($sql); 									                                                    //voer sql uit      
                                    echo $row['balans'];										                                                //geef 1 terug, pincode is goed
                                    
                                } else if($row['pogingen'] == 2) { 									                                            //derde keer fout pas geblokkeerd
                                    $sql        =   "UPDATE pincodes SET `pogingen` = '3' WHERE rekeningnummer= '$mgrkn'"; 		                //derde pogingen
                                    $conn->query($sql); 										                                                //voer sql uit
                                    echo"-3";
                                } else if($row['pogingen'] == 1) { 									                                            //tweede keer fout
                                    $sql        =   "UPDATE pincodes SET `pogingen` = '2' WHERE rekeningnummer= '$mgrkn'"; 		                //tweede pogingen
                                    $conn->query($sql); 										                                                //voer sql uit
                                    echo"-2";
                                } else if($row['pogingen'] == 0) { 									                                            //eerste keer fout
                                    $sql        =   "UPDATE pincodes SET `pogingen` = '1' WHERE rekeningnummer= '$mgrkn'"; 		                //eerste poging
                                    $conn->query($sql); 										                                                //voer sql uit
                                    echo"-1";
                                }
                            } if($row['pogingen'] >= 3){  
                                echo "-4";                                                                                                      //pas geblokkeerd
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