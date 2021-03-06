-- MySQL dump 10.10
--
-- Host: db.sneezymud.com    Database: sneezy
-- ------------------------------------------------------
-- Server version	5.0.24a-standard

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `shopowned`
--

DROP TABLE IF EXISTS `shopowned`;
CREATE TABLE `shopowned` (
  `shop_nr` int(11) NOT NULL default '0',
  `profit_buy` double NOT NULL default '0',
  `profit_sell` double NOT NULL default '0',
  `max_num` int(11) default NULL,
  `corp_id` int(11) default NULL,
  `dividend` double default NULL,
  `reserve_max` int(11) default NULL,
  `reserve_min` int(11) default NULL,
  `no_such_item1` varchar(127) default NULL,
  `no_such_item2` varchar(127) default NULL,
  `do_not_buy` varchar(127) default NULL,
  `missing_cash1` varchar(127) default NULL,
  `missing_cash2` varchar(127) default NULL,
  `message_buy` varchar(127) default NULL,
  `message_sell` varchar(127) default NULL,
  `tax_nr` int(11) default NULL,
  `gold` int(11) default NULL,
  PRIMARY KEY  (`shop_nr`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
