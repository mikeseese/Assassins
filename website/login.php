<?php
include_once("config.php");

//log out
if($_REQUEST['logout'] == true)
{
	session_unset();
	setcookie("PHPSESSID", "");
	//session_regenerate_id();
	header("location: /");
}
else if($_POST['login'] == true)
{
	// username and password sent from form
	$username=$_POST['username'];
	$password=$_POST['password'];
	
	// To protect MySQL injection (more detail about MySQL injection)
	$username = stripslashes($username);
	$password = stripslashes($password);
	$username = mysql_real_escape_string($username);
	$password = mysql_real_escape_string($password);
	$salt = '.-=0+4,3!0=-^';
	$password = md5(md5($salt.$password).$salt);
	
	$query= sprintf("SELECT * FROM users WHERE username='%s'",$username);
	$result=mysql_query($query);
	$row = mysql_fetch_assoc($result);
	
	if($row['username'] == $username)
	{
		if($row['password'] == $password)
		{
			//logged in!
			$sessionid = session_id();
			$userid = $row['userid'];
			$ipaddr = $_SERVER['REMOTE_ADDR'];
			$query = sprintf("INSERT INTO sessions (sessionid,ipaddr,userid) VALUES ('%s','%s',%d)", $sessionid, $ipaddr, $userid);
			$result = mysql_query($query);
			header("location: /");
		}
		else
		{
			$display = "Wrong password. Please try again.";
		}
		// Register $username, $password and redirect to file "login_success.php"
	
	}
	else
	{
		$display = 'Username does not exit. Please register or fix any typo\'s.<br><br><form action="register.php"><input type="submit" value="Register" /></form>';
	}
}
?>

<html>
	<head>
		<title>Assassins of the Spoon - Login</title>
		<link rel="stylesheet" type="text/css" href="style.css" />
	</head>
	
	<body>
		<form action="login.php" method="POST">
			Username:<br>
			<input type="text" name="username" /><br>
			Password:<br>
			<input type="password" name="password" /><br>
			<input type="hidden" name="login" value="true" />
			<input type="submit" value="Login" />
		</form>
		<br>
		<?php
			echo($display);
		?>
	</body>
</html>
