use alarm;
CREATE TABLE IF NOT EXISTS description
(
id_description int unsigned not null auto_increment,
name varchar(128) not null,
instance varchar(64) not null,
active tinyint(1) not null,
time_sec int unsigned not null,
formula varchar(4096) not null,
time_threshold int unsigned,
silent_time int default -1,
level enum('log','warning','fault') not null,
grp varchar(128) not null,
msg varchar(255),
action varchar(255),
PRIMARY KEY (id_description)
);
CREATE TABLE IF NOT EXISTS alarms
(
id_alarms int unsigned not null auto_increment,
time_sec int unsigned not null,
time_usec int(6) unsigned not null,
status enum('ALARM','NORMAL') not null,
ack enum('ACK','NACK') not null,
id_description int unsigned not null,
attr_values varchar(4096),
PRIMARY KEY (id_alarms)
);
