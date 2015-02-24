<?php
include_once("config.php");
include_once("security.php");

$errors = "";
$missing_required = "";


$query = sprintf("SELECT * FROM users WHERE userid='%s'", $userid);
$result = mysql_query($query);
$row = mysql_fetch_assoc($result);
$email = $row['email'];
$mobile = $row['mobile'];

function error($errors, $message)
{
	if($errors == "")
		$errors .= $message;
	else
		$errors .= "<br>" . $message;
	return $errors;
}

function missing_required($missing_required, $field)
{
	if($missing_required == "")
		$missing_required = "Missing required fields: " . $field;
	else
		$missing_required .= ", " . $field;
	return $missing_required;
}

function stripPhoneNumber($numberStr)
{
	$number = "";
	for($i = 0; $i < strlen($numberStr); $i++)
	{
		if(is_numeric($numberStr[$i]))
		{
			$number = $number . $numberStr[$i];
		}
	}
	
	return $number;
}

if($_POST['change_settings'] == true)
{
	$current_pass = $_POST['current_password'];
	$new_pass = $_POST['new_password'];
	$new_email = $_POST['new_email'];
	$confirm_pass = $_POST['confirm_password'];
	$confirm_email = $_POST['confirm_email'];
	
	//sql protection
	$current_pass = stripslashes($current_pass);
	$new_pass = stripslashes($new_pass);
	$new_email = stripslashes($new_email);
	$confirm_pass = stripslashes($confirm_pass);
	$confirm_email = stripslashes($confirm_email);
	$new_mobile = stripslashes($new_mobile);
	$current_pass = mysql_real_escape_string($current_pass);
	$new_pass = mysql_real_escape_string($new_pass);
	$new_email = mysql_real_escape_string($new_email);
	$confirm_pass = mysql_real_escape_string($confirm_pass);
	$confirm_email = mysql_real_escape_string($confirm_email);
	$new_mobile = mysql_real_escape_string($new_mobile);
	
	if($current_pass != "")
	{
		$query = sprintf("SELECT * FROM users WHERE userid='%s'", $userid);
		$result = mysql_query($query);
		$row = mysql_fetch_assoc($result);
		
		//check if current pass is correct
		$salt = '.-=0+4,3!0=-^';
		$pass = md5(md5($salt.$current_pass).$salt);
		if($pass == $row['password'])
		{
			//correct password, change settings
			if($new_pass != "")
			{
				if(strlen($new_pass) < 4)
				{
					$errors = error($errors, "Password needs to be longer than four characters.");
				}
				else if($new_pass == $confirm_pass)
				{
					$set_pass = true;
				}
				else
				{
					$errors = error($errors, "Your confirm password does not match the password given.");
				}
			}
			if($new_email != "")
			{
				if($new_email == $confirm_email)
				{
					$set_email = true;
				}
				else
				{
					$errors = error($errors, "Your confirm email does not match the email given.");
				}
			}
			if($new_mobile != "")
			{
				$new_mobile = stripPhoneNumber($new_mobile);
				if(strlen($new_mobile) != 10)
				{
					$errors = error($errors, "The mobile you provided is not a 10 digit number." . $new_mobile);
				}
				else
				{
					$set_mobile = true;
				}
			}
		}
		else
		{
			$errors = error($errors, "The current password you entered was incorrect.");
		}
	}
	else
	{
		$errors = error($errors, "You need to enter your current password.");
	}
	
	//if no errors, save user details
	if($errors == "" && $missing_required == "")
	{
		if($set_pass)
		{
			$update_pass = md5(md5($salt.$new_pass).$salt);
			header("Location: /login.php?logout=true");
		}
		else
		{
			$update_pass = $row['password'];
			header("Location: /");
		}
		
		if($set_email)
		{
			$update_email = $new_email;
		}
		else
		{
			$update_email = $row['email'];
		}
		
		if($set_mobile)
		{
			$update_mobile = $new_mobile;
		}
		else
		{
			$update_mobile = $row['mobile'];
		}
		
		$query = sprintf("UPDATE users SET password='%s', email='%s', mobile='%s' WHERE userid='%d'", $update_pass, $update_email, $update_mobile, $userid);
		$result = mysql_query($query);
	}
}
?>

<html>
	<head>
		<title>Assassins of the Spoon - Settings</title>
	</head>
	
	<body>
		<a href="/">Home</a><br>
		<br>
		<?php
			echo("Email: " . $email . "<br>");
			if(strlen($mobile) > 10)
			{
				//mobile verified
				echo("Mobile verified: " . $mobile . "<br>");
			}
			else if(strlen($mobile) > 0)
			{
				//mobile entered, not verified
				echo("Mobile not verified: " . $mobile . ". Please text the word 'verify' to assassins@assassins.myprotosite.com from your mobile.<br>");
			}
		?>
		<form action="settings.php" method="POST">
		<table>
			<tr><td>Current Password:</td><td><input type="password" name="current_password" /></td></tr>
			<tr><td>New Password:</td><td><input type="password" name="new_password" /></td></tr>
			<tr><td>Confirm Password:</td><td><input type="password" name="confirm_password" /></td></tr>
			<tr><td>New Email:</td><td><input type="text" name="new_email" /></td></tr>
			<tr><td>Confirm Email:</td><td><input type="text" name="confirm_email" /></td></tr>
			<tr><td>New Mobile:</td><td><input type="text" name="new_mobile" /> After you successfully set you number, text the word 'verify' to assassins@assassins.myprotosite.com.</td></tr>
			<input type="hidden" name="change_settings" value="true" />
			<tr><td></td><td><input type="submit" value="Save" /></td></tr>
		</table>
		</form>
		<?php echo($errors); ?>
	</body>
</html>