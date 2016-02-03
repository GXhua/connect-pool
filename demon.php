<?php
//*******************use master slave(最新版本支持了读写分离和从库的负载均衡 用法如下)***********************/
$config = array(
    'master' => array(
        'data_source' => "mysql:host=192.168.1.19;dbname=mz_gay_group2;charset=utf8",
        'username' => "public_user",
        'pwd' => "1qa2ws3ed",
        'options' => array(
            PDO::ATTR_ERRMODE=>PDO::ERRMODE_EXCEPTION,
            PDO::ATTR_TIMEOUT => 3,
            PDO::ATTR_CASE =>PDO::CASE_UPPER,
        ),
    ),
    'slave' => array(
        "0" => array(
            //'data_source' => "mysql:host=192.168.1.19;dbname=mz_gay_group",
            'data_source' => "mysql:host=192.168.1.19;dbname=mz_gay_group;charset=utf8",
            'username' => "public_user",
            'pwd' => "1qa2ws3ed",
            'options' => array(
                PDO::ATTR_ERRMODE=>PDO::ERRMODE_EXCEPTION,
                PDO::ATTR_TIMEOUT => 3,
                PDO::ATTR_CASE =>PDO::CASE_UPPER,
            ),
        ),
        "1" => array(
            //'data_source' => "mysql:host=192.168.1.19;dbname=mz_gay_group",
            'data_source' => "mysql:host=192.168.1.19;dbname=mz_gay_group;charset=utf8",
            'username' => "public_user",
            'pwd' => "1qa2ws3ed",
            'options' => array(
                PDO::ATTR_ERRMODE=>PDO::ERRMODE_EXCEPTION,
                PDO::ATTR_TIMEOUT => 3,
                PDO::ATTR_CASE =>PDO::CASE_LOWER,
            ),
        ),
    ),
);
/***************************"select"和"show"开头的语句 走随机从库***********/
$obj1 = new pdo_connect_pool($config);
$rs = $obj1->query("select * from test limit 5");
var_dump($rs->fetchAll());//走随机从库
$obj1->release();
die;
/****************************读强行走主库****************************/
$obj1->enable_slave = false;
$rs = $obj1->query("select * from test limit 1");
var_dump($rs->fetchAll());//读主库
$obj1->release();

/***************************除了"select"和"show"开头的语句 都走主库***********/
$sql = "insert into `test` (tid) values (5)";
$rs = $obj1->exec($sql);//走主库
$obj1->release();

/* tips：
 * 1、The relase() method will release the connections to the pool that the process holds.
 * 2、after rshutdown/mshutdown will trigger the release() function.
 */


/* 说明：
 * 1、relase方法：通知中间件,可以将这个进程持有的链接放回连接池
 * 2、请求结束（rshutdown/mshutdown阶段）会调用自动调用release
 */
