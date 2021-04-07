<?php

extension = php_mysql.dll

    // Prepare variables for database connection
   
    $dbusername = "arduino";  // enter database username, I used "arduino" in step 2.2
    $dbpassword = "arduinotest";  // enter database password, I used "arduinotest" in step 2.2
    $server = "localhost"; // IMPORTANT: if you are using XAMPP enter "localhost", but if you have an online website enter its address, ie."www.yourwebsite.com"

    // Connect to your database

    $dbconnect = mysqli_pconnect($server, $dbusername, $dbpassword); //inloggen database
    $dbselect = mysqli_select_db("test",$dbconnect); //database selecteren

    // Prepare the SQL statement

    $sql = "INSERT INTO test.sensor (value) VALUES ('".$_GET["value"]."')";  //sql

    // Execute SQL statement

    mysql_query($sql); //sql versturen

?>