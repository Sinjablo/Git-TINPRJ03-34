-- MySQL dump 10.13  Distrib 8.0.22, for Win64 (x86_64)
--
-- Host: localhost    Database: bank
-- ------------------------------------------------------
-- Server version	8.0.22

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `pincodes`
--

DROP TABLE IF EXISTS `pincodes`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `pincodes` (
  `klant_id` int NOT NULL,
  `pincode` int NOT NULL,
  `rekeningnummer` varchar(45) NOT NULL,
  PRIMARY KEY (`klant_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `pincodes`
--

LOCK TABLES `pincodes` WRITE;
/*!40000 ALTER TABLE `pincodes` DISABLE KEYS */;
INSERT INTO `pincodes` VALUES (1,1232,'NL21HAHA010032003'),(2,2343,'NL21HAHA002102002'),(3,3454,'NL21HAHA006102001'),(4,4565,'NL21HAHA026091997');
/*!40000 ALTER TABLE `pincodes` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `user1`
--

DROP TABLE IF EXISTS `user1`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `user1` (
  `transactie_id` varchar(45) NOT NULL,
  `datum` datetime NOT NULL,
  `plus_of_min` varchar(45) NOT NULL,
  `bedrag` int NOT NULL,
  `balans` int NOT NULL,
  `achternaam` varchar(45) NOT NULL,
  PRIMARY KEY (`transactie_id`),
  KEY `achternaam_idx` (`achternaam`),
  CONSTRAINT `achternaam1` FOREIGN KEY (`achternaam`) REFERENCES `users` (`achternaam`) ON DELETE RESTRICT ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `user1`
--

LOCK TABLES `user1` WRITE;
/*!40000 ALTER TABLE `user1` DISABLE KEYS */;
INSERT INTO `user1` VALUES ('A1A1A1','2021-04-05 00:00:00','+',100,100,'Burgstad');
/*!40000 ALTER TABLE `user1` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `user2`
--

DROP TABLE IF EXISTS `user2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `user2` (
  `transactie_id` varchar(45) NOT NULL,
  `datum` datetime NOT NULL,
  `plus_of_min` varchar(45) NOT NULL,
  `bedrag` int NOT NULL,
  `balans` int NOT NULL,
  `achternaam` varchar(45) NOT NULL,
  PRIMARY KEY (`transactie_id`),
  KEY `achternaam_idx` (`achternaam`),
  CONSTRAINT `achternaam2` FOREIGN KEY (`achternaam`) REFERENCES `users` (`achternaam`) ON DELETE RESTRICT ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `user2`
--

LOCK TABLES `user2` WRITE;
/*!40000 ALTER TABLE `user2` DISABLE KEYS */;
INSERT INTO `user2` VALUES ('A1A1A2','2021-04-05 00:00:00','+',100,100,'Vielvoije');
/*!40000 ALTER TABLE `user2` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `user3`
--

DROP TABLE IF EXISTS `user3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `user3` (
  `transactie_id` varchar(45) NOT NULL,
  `datum` datetime NOT NULL,
  `plus_of_min` varchar(45) NOT NULL,
  `bedrag` int NOT NULL,
  `balans` int NOT NULL,
  `achternaam` varchar(45) NOT NULL,
  PRIMARY KEY (`transactie_id`),
  KEY `achternaam3_idx` (`achternaam`),
  CONSTRAINT `achternaam3` FOREIGN KEY (`achternaam`) REFERENCES `users` (`achternaam`) ON DELETE RESTRICT ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `user3`
--

LOCK TABLES `user3` WRITE;
/*!40000 ALTER TABLE `user3` DISABLE KEYS */;
INSERT INTO `user3` VALUES ('A1A1A3','2021-04-05 00:00:00','+',100,100,'Vuijk');
/*!40000 ALTER TABLE `user3` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `user4`
--

DROP TABLE IF EXISTS `user4`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `user4` (
  `transactie_id` varchar(45) NOT NULL,
  `datum` datetime NOT NULL,
  `plus_of_min` varchar(45) NOT NULL,
  `bedrag` int NOT NULL,
  `balans` int NOT NULL,
  `achternaam` varchar(45) NOT NULL,
  PRIMARY KEY (`transactie_id`),
  KEY `achternaam4_idx` (`achternaam`),
  CONSTRAINT `achternaam4` FOREIGN KEY (`achternaam`) REFERENCES `users` (`achternaam`) ON DELETE RESTRICT ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `user4`
--

LOCK TABLES `user4` WRITE;
/*!40000 ALTER TABLE `user4` DISABLE KEYS */;
INSERT INTO `user4` VALUES ('A1A1A4','2021-04-05 00:00:00','+',100,100,'Buijing');
/*!40000 ALTER TABLE `user4` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `users`
--

DROP TABLE IF EXISTS `users`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `users` (
  `klant_id` int NOT NULL,
  `voornaam` varchar(45) NOT NULL,
  `achternaam` varchar(45) NOT NULL,
  `geslacht` varchar(45) DEFAULT NULL,
  `geboortedatum` datetime NOT NULL,
  PRIMARY KEY (`achternaam`),
  KEY `user_id_idx` (`klant_id`),
  CONSTRAINT `klant_id` FOREIGN KEY (`klant_id`) REFERENCES `pincodes` (`klant_id`) ON DELETE RESTRICT ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `users`
--

LOCK TABLES `users` WRITE;
/*!40000 ALTER TABLE `users` DISABLE KEYS */;
INSERT INTO `users` VALUES (4,'Marjon','Buijing','V','1997-09-26 00:00:00'),(1,'Ramon','Burgstad','M','2003-03-10 00:00:00'),(2,'Sander','Vielvoije','M','2002-10-02 00:00:00'),(3,'Jarno','Vuijk','M','2001-10-06 00:00:00');
/*!40000 ALTER TABLE `users` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2021-04-07 13:53:22
