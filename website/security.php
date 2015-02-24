<?php
include_once("config.php");

//this php file goes to index, sets $userid in the circumstance you're logged in
if(session_id() != "")
{
	$sessionid = session_id();
	$ipaddr = $_SERVER['REMOTE_ADDR'];
	$query = sprintf("SELECT * FROM sessions WHERE sessionid='%s'",$sessionid);
	$result = mysql_query($query);
	if(mysql_num_rows($result) == 0)
	{
		header("Location: /");
	}
	$row = mysql_fetch_assoc($result);
	if($row['sessionid'] == $sessionid)
	{
		if($row['ipaddr'] == $ipaddr)
		{
			//do nothing? you're logged in
			$userid = $row['userid'];
		}
		else
		{
			header("Location: /");
		}
	}
}
?>