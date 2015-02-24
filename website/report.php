<?php
include_once("config.php");
include_once("functions.php");

$errors = "";

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
			$userid = $row['userid'];
			
			$query = sprintf("SELECT * FROM users WHERE userid='%s'", $userid);
			$result = mysql_query($query);
			$row = mysql_fetch_assoc($result);
			if($row['targetid'] < 0)
			{
				header("Location: /"); //you cant report if you dont have a target (or dead)
			}
		}
		else
		{
			header("Location: /");
		}
	}
	if($_POST['report'] == "true")
	{
		$secret = $_POST['target_secret'];
		
		// Check for errors
		
		//check if required fields are entered
		if($secret == "")
		{
			$errors = "Please enter a target secret.";
		}
		else
		{
			// To protect SQL injection
			$secret = stripslashes($secret);
			$secret = mysql_real_escape_string($secret);
			
			//get current user row
			$query = sprintf("SELECT * FROM users WHERE userid='%s'", $userid);
			$result = mysql_query($query);
			
			//report kill
			$row = mysql_fetch_assoc($result);
			$targetid = $row['targetid'];
			$gameid = $row['gameid'];
			
			if($targetid == null)
			{
				//if target is NULL, then the game isnt started
				$errors = "Your tournament isn't running. <a href="/">Go Home</a>.";
			}
			else
			{
				//do it
				$query = sprintf("SELECT * FROM users WHERE userid='%s'", $targetid);
				$result = mysql_query($query);
				$row = mysql_fetch_assoc($result);
				$target_secret = $row['secret'];
				if($secret == $target_secret)
				{
					//you got the secret right, kill confirmed
					
					//inherit your target's contract
					$new_target = $row['targetid'];
					$query = sprintf("SELECT * FROM users WHERE targetid='-2'");
					$result = mysql_query($query);
					if(mysql_num_rows($result) > 0)
					{
						//this means that there are people waiting for assignment. there should only be one person at a time
						
						//switch contracts. we dont have to worry about giving the awaiting person himself because he has himself already
						//	we also dont have to worry about you getting yourself.
						$row = mysql_fetch_assoc($result);
						$awaitingid = $row['userid'];
						
						//set awaiting person to your new contract
						$query = sprintf("UPDATE users SET targetid='%s' WHERE userid='%s'", $new_target, $awaitingid);
						$result = mysql_query($query);
						notifyNewContract($awaitingid);
						
						//set your contract to the awaiting person
						$query = sprintf("UPDATE users SET targetid='%s' WHERE userid='%s'", $awaitingid, $userid);
						$result = mysql_query($query);
						notifyNewContract($userid);
						
						$errors = "Kill reported.";
					}
					else
					{
						if($new_target != $userid)
						{
							$query = sprintf("UPDATE users SET targetid='%s' WHERE userid='%s'", $new_target, $userid);
							$result = mysql_query($query);
							$errors = "Kill reported.";
						}
						else
						{
							//if the target's contract was you, you cannot kill yourself and progress through the game
							//	therefore we must wait for the next kill. when this kill occurs, you will switch contracts
							//	with the person that made the following kill. also, this bracket of code will only run if
							//	there is already no one waiting for assignement, so we dont need to check for that. set target
							//	to -2 to imply awaiting assignment
							$query = sprintf("UPDATE users SET targetid='-2' WHERE userid='%s'", $userid);
							$result = mysql_query($query);
							$errors = "Kill reported.";
						}
					}
					
					//reset target's target to -1, which implies death
					$query = sprintf("UPDATE users SET targetid='-1' WHERE userid='%s'", $targetid);
					$result = mysql_query($query);
					notifyNewContract($targetid);
					
					
					//check to see if you're last man standing
					$query = sprintf("SELECT * FROM users WHERE targetid<>-1 AND gameid='%s'", $gameid);
					$result = mysql_query($query);
					if(mysql_num_rows($result) == 1)
					{
						//you won
						$query = sprintf("UPDATE users SET targetid='-3' WHERE userid='%s'", $userid);
						$result = mysql_query($query);
					}
					
					notifyNewContract($userid);
				}
				else
				{
					$errors = "The secret you entered is incorrect. Try again.";
				}
			}
		}
	}
}
else
{
	header("Location: /");
}
	
?>

<html>
	<head>
		<title>Assassins of the Spoon - Report Kill</title>
		<link rel="stylesheet" type="text/css" href="style.css" />
	</head>
	
	<body>
		<a href="/">Home</a><br>
		<br>
		<form action="report.php" method="POST">
			Target's Secret:<br>
			<input type="text" name="target_secret" /><br>
			<input type="hidden" name="report" value="true" />
			<input type="submit" value="Report Kill" /><br>
			<?php echo($errors); ?>
		</form>
	</body>
</html>