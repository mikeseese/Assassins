<?php
include_once("config.php");
include_once("security.php");

$errors = "";

if($_POST['join'] == "true")
{
	$name = $_POST['game_name'];
	
	// Check for errors
	
	//check if required fields are entered
	if($name == "")
	{
		$errors = "Please enter a tournament name to join it.";
	}
	else
	{
		// To protect SQL injection
		$name = stripslashes($name);
		$name = mysql_real_escape_string($name);
		
		//check if game name used
		$query = sprintf("SELECT * FROM games WHERE name='%s'", $name);
		$result = mysql_query($query);
		if(mysql_num_rows($result) == 1)
		{
			//join game
			$row = mysql_fetch_assoc($result);
			$gameid = $row['gameid'];
			$state = $row['state'];
			
			//check state of the game
			if($state == 0)
			{
				//game not started, join game
				$query = sprintf("UPDATE users SET gameid='%s' WHERE userid='%s'", $gameid, $userid);
				$result = mysql_query($query);
				header("Location: /");
			}
			else if($state == 1)
			{
				//game started
				$errors = "Tournament has already begun.";
			}
			else if($state == 2)
			{
				//registration closed
				$errors = "Registration is currently closed for this tournament.";
			}
		}
		else
			$errors = "Tournament doesn't exist. Would you like to <a href=\"/create.php\">create it</a>?";
	}
}
else if($_REQUEST['leave'] == "true")
{
	//remove game id
	$query = sprintf("UPDATE users SET gameid=NULL WHERE userid='%s'", $userid);
	$result = mysql_query($query);
	header("Location: /");
}
?>

<html>
	<head>
		<title>Assassins of the Spoon - Join Tournament</title>
		<link rel="stylesheet" type="text/css" href="style.css" />
	</head>
	
	<body>
		<a href="/">Home</a><br>
		<br>
		<form action="join.php" method="POST">
			Tournament Name:<br>
			<input type="text" name="game_name" /><br>
			<input type="hidden" name="join" value="true" />
			<input type="submit" value="Join" /><br>
			<?php echo($errors); ?>
		</form>
	</body>
</html>
