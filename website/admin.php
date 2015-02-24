<?php
include_once("config.php");
include_once("functions.php");
?>
<html>
	<head>
		<title>Assassins of the Spoon - Tournament Admin</title>
	</head>
	
	<body>
		<?php
			function randomAlphaNum($length)
			{ 
		    $alphNums = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";        
		    $newString = str_shuffle(str_repeat($alphNums, rand(1, $length))); 
		    
		    return substr($newString, rand(0,strlen($newString)-$length), $length); 
			}
			
			$doheader = false;
			
			if(session_id() != "")
			{
				$sessionid = session_id();
				$ipaddr = $_SERVER['REMOTE_ADDR'];
				$query = sprintf("SELECT * FROM sessions WHERE sessionid='%s'",$sessionid);
				$result = mysql_query($query);
				$row = mysql_fetch_assoc($result);
				if($row['sessionid'] == $sessionid)
				{
					if($row['ipaddr'] == $ipaddr)
					{
						//logged in
						$userid = $row['userid'];
						$query = sprintf("SELECT * FROM users WHERE userid='%s'",$userid);
						$result = mysql_query($query);
						$row = mysql_fetch_assoc($result);
						$logged_in = '
												 		Hello, ' . $row['first_name'] . ' ' . $row['last_name'] . '.<br>
												 		<a href="/">Home</a><br>
												 		<a href="/login.php?logout=true">Logout</a><br>
												 		<a href="/settings.php">Settings</a><br>
												 		<a href="/create.php">Create Tournament</a><br>
												 		<a href="/join.php">Join Tournament</a><br>
												 ';
						if($row['gameid'] != NULL)
						{
							//if game started?
							$gameid = $row['gameid'];
							$query = sprintf("SELECT * FROM users WHERE userid='%s'",$userid);
							$result = mysql_query($query);
							$row = mysql_fetch_assoc($result);
							$targetid = $row['targetid'];
							$query = sprintf("SELECT * FROM games WHERE gameid='%s'",$gameid);
							$result = mysql_query($query);
							$row = mysql_fetch_assoc($result);
							if($row['adminid'] == $userid)
							{
								//you're an admin
								$logged_in = $logged_in . '
																						<a href="/admin.php">Admin</a><br>
																						<br>
																						<form action="admin.php" method="POST">
																							Game State: <select name="state">
																														<option value="0" %s>Registration Open</option>
																														<option value="1" %s>Game Started</option>
																														<option value="2" %s>Registration Closed</option>
																													</select><br>
																							<input type="hidden" name="change_state" value="true" />
																							<input type="submit" name="submit" value="Save" />
																						</form><br>
																						<form action="admin.php" method="POST">
																							<input type="checkbox" name="delete" value="true" />Delete game? (there is no going back)<br>
																							<input type="submit" name="submit" value="Submit Deletion" />
																						</form><br>
																					';
								$state = $row['state'];
								if($_POST['delete'] == "true")
								{
									//delete the game!
									
									$query = sprintf("UPDATE users SET gameid=NULL, targetid=NULL, secret=NULL WHERE gameid='%s'", $gameid);
									$result = mysql_query($query);
									
									$query = sprintf("DELETE FROM games WHERE gameid='%s'", $gameid);
									$result = mysql_query($query);
									
									notifyDelete($gameid);
								}
								else if($_POST['change_state'] == "true")
								{
									//set state
									$state = $_POST['state'];
									$query = sprintf("UPDATE games SET state='%s' WHERE gameid='%s'", $state, $gameid);
									$result = mysql_query($query);
									
									//(de)assign players
									if($state == 0)
									{
										//game not running, clear targets
										$query = sprintf("UPDATE users SET targetid=NULL, secret=NULL WHERE gameid='%s'", $gameid);
										$result = mysql_query($query);
									}
									else if($state == 1)
									{
										//game started, assign targets (only if the game wasnt started. this prevents an admin accidentally pressing "save" on admin page and reassigning targets to a running game)
										if($state != $row['state'])
										{
											$query = sprintf("SELECT * FROM users WHERE gameid='%s'", $gameid);
											$result = mysql_query($query);
											$numofusers = mysql_num_rows($result);
											$userids = array(); //list of users
											$targetids = array(); //list of users assigned as a target
											for($i = 0; $i < $numofusers; $i++)
											{
												$row = mysql_fetch_assoc($result);
												$userids[$i] = $row['userid'];
											}
											foreach($userids as $i => $userid)
											{
												do
												{
													$istarget = false;
													$num = rand(0, $numofusers - 1);
													foreach($targetids as $j => $numtarget)
													{
														if($num == $numtarget)
														{
															$istarget = true;
															break;
														}
													}
												} while($num == $i || $istarget); //make sure the target id isn't equal to the current id
												//add num to targetids
												$targetids[] = $num; //this adds num to the end of the array. refer to php.net
												
												//generate secret
												$secret = randomAlphaNum(6);
												//set target
												$query = sprintf("UPDATE users SET targetid='%s', secret='%s' WHERE userid='%s'", $userids[$num], $secret, $userid);
												$result = mysql_query($query);
											}
											
											//notify all the users of their contracts. this will probably cause the page to fuck up. we might want to CHANGE HOW THIS WORKS!
											//	maybe we should signal for a c++ mailer? idk right now. not going to worry until large scale games are using engine
											foreach($userids as $i => $userid)
											{
												notifyNewContract($userid);
											}
										}
									}
									else if($state == 2)
									{
										//registration is closed, clear targets
										$query = sprintf("UPDATE users SET targetid=NULL, secret=NULL WHERE gameid='%s'", $gameid);
										$result = mysql_query($query);
										
									}
								}
								if($state == 0)
									$logged_in = sprintf($logged_in, 'selected="selected"', '', '');
								else if($state == 1)
									$logged_in = sprintf($logged_in, '', 'selected="selected"', '');
								else if($state == 2)
									$logged_in = sprintf($logged_in, '', '', 'selected="selected"');
							}
							else
								$doheader = true;
							$query = sprintf("SELECT * FROM games WHERE gameid='%s'",$gameid);
							$result = mysql_query($query);
							$row = mysql_fetch_assoc($result);
							$logged_in = $logged_in . '
																					<br>
																					You are currently administrating the tournament, ' . $row['name'] . '.<br>
																				';
						}
					}
					else
						$doheader = true;
				}
				else
					$doheader = true;
			}
			else
			{
				$doheader = true;
			}
			if($doheader)
				header("Location: /");
echo($logged_in);
?>
		
	</body>
</html>
