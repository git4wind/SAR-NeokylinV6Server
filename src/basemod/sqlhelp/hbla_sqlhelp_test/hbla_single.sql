create table baseInfo(clientID char(32),buildNo int, policyTime char(24),person char(128), department char(128), unit char(128), isSign int, managerPass char(32), logPass char(32));
insert into baseInfo values(NULL, 1100, '1970-00-00 00:00:00',NULL,NULL,NULL, 0, 'admin', 'logadmin');
create table policyRule(moduleNo int, rule blob, state int);
create table auditMsg(id INTEGER PRIMARY KEY AUTOINCREMENT, msg blob , mno int, cno int, state int);
