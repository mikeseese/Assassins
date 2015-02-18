#ifndef INCLUDES
#define INCLUDES
#include "../../include/includes.h"
#endif

#include "../../include/pop.h"
#include "../../include/smtp.h"
#include "/usr/local/include/mysql/mysql.h"

#include "config.h" //location of authentication details

#define USERID 0
#define USERNAME 1
#define PASSWORD 2
#define EMAIL 3
#define GAMEID 4
#define TARGETID 5
#define SECRET 6
#define FIRST_NAME 7
#define LAST_NAME 8
#define MOBILE 9

void mail(string recepient, string message)
{
	smtp s(false);
  s.Connect(MAILHOST);
  s.SetDomain(MAILHOST);
  s.SetFrom(MAILADDR);
	s.SetTo(recepient);
	s.SetMessageText(message);
}

void notifyNewContract(string userid, bool status = false)
{
	//mysql variables
	MYSQL mysql;
  MYSQL_RES *result;
  MYSQL_ROW row;
  string query;
  
	string to;
	string mobile;
	string targetid;
	string secret;
	string targetname;
	string message;
	
	//initiate mysql
	if(!mysql_init(&mysql))
  {
    cout << mysql_errno(&mysql) << ": " << mysql_error(&mysql) << endl;
    return;
  }
  //cout << "MySQL initialized." << endl;
  
  //connect to database
  if(!mysql_real_connect(&mysql, MYSQLHOST, MYSQLUSER, MYSQLPASS, MYSQLDB, 0, NULL, 0))
  {
    cout << mysql_errno(&mysql) << ": " << mysql_error(&mysql) << endl;
  	return;
  }
  //cout << "Connected to database." << endl;
	
	query = "SELECT * FROM users WHERE userid='" + userid + "'";
  if(mysql_query(&mysql, query.c_str()))
  {
		cout << mysql_errno(&mysql) << ": " << mysql_error(&mysql) << endl;
    return;
  }
  result = mysql_store_result(&mysql);
	if(row = mysql_fetch_row(result))
	{
		to = row[EMAIL];
		mobile = row[MOBILE];
		targetid = row[TARGETID];
		secret = row[SECRET];
	}
	mysql_free_result(result);
	
	//get name of target id
	int target = atoi(targetid.c_str());
	cout << "target: " << target << endl;
	if(target >= 0)
	{
		//you have a target
		query = "SELECT * FROM users WHERE userid='" + targetid + "'";
    if(mysql_query(&mysql, query.c_str()))
    {
  		cout << mysql_errno(&mysql) << ": " << mysql_error(&mysql) << endl;
      return;
    }
	  result = mysql_store_result(&mysql);
		if(row = mysql_fetch_row(result))
		{
			targetname = row[FIRST_NAME];
			targetname.append(" ");
			targetname.append(row[LAST_NAME]);
		}
		mysql_free_result(result);
		
		if(status)
			message = "From: Assassins Overlord <assassins@assassins.myprotosite.com>\r\nSubject: Current Status\r\n\r\nYour target's name is " + targetname + ". Your secret: " + secret + ".";
		else
			message = "From: Assassins Overlord <assassins@assassins.myprotosite.com>\r\nSubject: Your Next Contract\r\n\r\nYou have been assigned a new contract. Your target's name is " + targetname + ". Your secret: " + secret + ".";
	}
	else if(target == -1)
	{
		//you're dead
		if(status)
			message = "From: Assassins Overlord <assassins@assassins.myprotosite.com>\r\nSubject: Current Status\r\n\r\nAnother assassin has reported your death. Good luck next time.";
		else
			message = "From: Assassins Overlord <assassins@assassins.myprotosite.com>\r\nSubject: Reported Death\r\n\r\nAnother assassin has reported your death. Good luck next time.";
	}
	else if(target == -2)
	{
		//you dont have a target
		if(status)
			message = "From: Assassins Overlord <assassins@assassins.myprotosite.com>\r\nSubject: Current Status\r\n\r\nThe overlord hasn't found a suitable contract for you yet. Please wait while an assignment is made.";
		else
			message = "From: Assassins Overlord <assassins@assassins.myprotosite.com>\r\nSubject: Your Next Contract\r\n\r\nThe overlord hasn't found a suitable contract for you yet. Please wait while an assignment is made.";
	}
	else if(target == -3)
	{
		//you won the tournament
		if(status)
			message = "From: Assassins Overlord <assassins@assassins.myprotosite.com>\r\nSubject: Current Status\r\n\r\nCongratulations assassin, you have won the tournament. Everyone but you is dead.";
		else
			message = "From: Assassins Overlord <assassins@assassins.myprotosite.com>\r\nSubject: Good Work Assassin\r\n\r\nCongratulations assassin, you have won the tournament. Everyone but you is dead.";
	}
	
	//send email notifying of target
	if(!status) //send to email only if cell phone is looking for current status
	{
		mail(to, message);
		cout << "Email sent." << endl;
	}
	
	if(mobile.length() > 10)
	{
		mail(mobile, message);
		cout << "Text sent." << endl;
	}
}

int main()
{
	//mysql variables
	MYSQL mysql;
  MYSQL_RES *result;
  MYSQL_ROW row;
  string query;
	
	//initiate mysql
	if (!mysql_init(&mysql))
  {
    cout << mysql_errno(&mysql) << ": " << mysql_error(&mysql) << endl;
    return -1;
  }
  cout << "MySQL initialized." << endl;
  
  //connect to database
  if(!mysql_real_connect(&mysql, "assassins.myprotosite.com", "assassins", "password", "assassins", 0, NULL, 0))
  {
    cout << mysql_errno(&mysql) << ": " << mysql_error(&mysql) << endl;
  	return -1;
  }
  cout << "Connected to database." << endl;
  
	while(true)
	{
		pop p(false);
		p.Connect(MAILHOST);
		p.Login(MAILUSER,MAILPASS);
		if(p.NumUnread() > 0)
		{
			p.Retrieve();
			string from = p.GetFrom();
			cout << "Message from: " << from << endl;
			string message_text = p.GetMessageText();
			message_text = StringToLower(message_text);
			cout << "Message text: '" << message_text << "'" << endl;
			
			
			if(message_text.compare("verify") == 0)
			{
				//they are verifying their number
				//look up phone number in SQL table
				string phone_number = from.substr(0, 10);
				
				cout << "Phone number " << phone_number << " wants to verify. Finding user and updating." << endl;
				
				query = "UPDATE users SET mobile='" + from + "' WHERE mobile='" + phone_number + "';";
		    if(mysql_query(&mysql, query.c_str()))
		    {
	    		cout << mysql_errno(&mysql) << ": " << mysql_error(&mysql) << endl;
		      return -1;
		    }
			}
			
			if(message_text.compare("status") == 0)
			{
				//they want to know their current status
				string userid;
				
				query = "SELECT * FROM users WHERE mobile='" + from + "';";
		    if(mysql_query(&mysql, query.c_str()))
		    {
	    		cout << mysql_errno(&mysql) << ": " << mysql_error(&mysql) << endl;
		      return -1;
		    }
		    
		    result = mysql_store_result(&mysql);
				if(row = mysql_fetch_row(result))
				{
					userid = row[USERID];
				}
				mysql_free_result(result);
				
				notifyNewContract(userid, true);
			}
			
			if(message_text.length() >= 6)
			{
				if(message_text.substr(0, 6).compare("report") == 0)
				{
					if(message_text.length() >= 13)
					{
						//they want to report a kill
						
				    string userid;
				    string targetid;
				    string targetsecret;
				    string newtarget;
				    string gameid;
						string secret = message_text.substr(7, 6);
						cout << "'" << secret << "'" << endl;
						
						query = "SELECT * FROM users WHERE mobile='" + from + "';";
				    if(mysql_query(&mysql, query.c_str()))
				    {
			    		cout << mysql_errno(&mysql) << ": " << mysql_error(&mysql) << endl;
				      return -1;
				    }
				    
				    result = mysql_store_result(&mysql);
						if(row = mysql_fetch_row(result))
						{
							userid = row[USERID];
							targetid = row[TARGETID];
							gameid = row[GAMEID];
						}
						mysql_free_result(result);
						
						query = "SELECT * FROM users WHERE userid='" + targetid + "';";
				    if(mysql_query(&mysql, query.c_str()))
				    {
			    		cout << mysql_errno(&mysql) << ": " << mysql_error(&mysql) << endl;
				    	return -1;
				    }
				    
				    result = mysql_store_result(&mysql);
						if(row = mysql_fetch_row(result))
						{
							targetsecret = StringToLower(row[SECRET]);
							newtarget = row[TARGETID];
						}
						mysql_free_result(result);
						
						if(targetsecret.compare(secret) == 0)
						{
							//secret is correct, kill confirmed
							
							query = "UPDATE users SET targetid='-1' WHERE userid='" + targetid + "';";
							if(mysql_query(&mysql, query.c_str()))
							{
						  	cout << mysql_errno(&mysql) << ": " << mysql_error(&mysql) << endl;
							 	return -1;
							}
							
							query = "SELECT * FROM users WHERE targetid='-2';";
						  if(mysql_query(&mysql, query.c_str()))
						  {
					   		cout << mysql_errno(&mysql) << ": " << mysql_error(&mysql) << endl;
						    return -1;
						  }
						  
						  result = mysql_store_result(&mysql);
							if(row = mysql_fetch_row(result))
							{
								//people are waiting for assignments
								string awaitingid = row[USERID];
								
								query = "UPDATE users SET targetid='" + newtarget + "' WHERE userid='" + awaitingid + "';";
							  if(mysql_query(&mysql, query.c_str()))
							  {
						   		cout << mysql_errno(&mysql) << ": " << mysql_error(&mysql) << endl;
							    return -1;
							  }
							  notifyNewContract(awaitingid);
								
								query = "UPDATE users SET targetid='" + awaitingid + "' WHERE userid='" + userid + "';";
							  if(mysql_query(&mysql, query.c_str()))
							  {
						   		cout << mysql_errno(&mysql) << ": " << mysql_error(&mysql) << endl;
							    return -1;
							  }
							}
							else
							{
								//no one's waiting for assignment
								
								if(newtarget.compare(userid) != 0)
								{
									query = "UPDATE users SET targetid='" + newtarget + "' WHERE userid='" + userid + "';";
								  if(mysql_query(&mysql, query.c_str()))
								  {
							   		cout << mysql_errno(&mysql) << ": " << mysql_error(&mysql) << endl;
								    return -1;
								  }
								}
								else
								{
									//if the target's contract was you, you cannot kill yourself and progress through the game
									//	therefore we must wait for the next kill. when this kill occurs, you will switch contracts
									//	with the person that made the following kill. also, this bracket of code will only run if
									//	there is already no one waiting for assignement, so we dont need to check for that. set target
									//	to -2 to imply awaiting assignment
									
									query = "UPDATE users SET targetid='-2' WHERE userid='" + userid + "';";
								  if(mysql_query(&mysql, query.c_str()))
								  {
							   		cout << mysql_errno(&mysql) << ": " << mysql_error(&mysql) << endl;
								    return -1;
								  }
								}
							}
							
							//text back telling kill was reported
						  //mail(from, "Kill Reported."); //redundant. if the user is texting the daemon, phone is enabled, therefore
						  //															//		phone will will receive new contract notice.
							
							//check to see if you're last man standing
							query = "SELECT * FROM users WHERE targetid<>-1 AND gameid='" + gameid + "';";
						  if(mysql_query(&mysql, query.c_str()))
						  {
					   		cout << mysql_errno(&mysql) << ": " << mysql_error(&mysql) << endl;
						    return -1;
						  }
						  result = mysql_store_result(&mysql);
							if(mysql_num_rows(result) == 1)
							{
								//you won
								query = "UPDATE users SET targetid='-3' WHERE userid='" + userid + "'";
							  if(mysql_query(&mysql, query.c_str()))
							  {
						   		cout << mysql_errno(&mysql) << ": " << mysql_error(&mysql) << endl;
							    return -1;
							  }
							}
							
							notifyNewContract(userid);
							notifyNewContract(targetid);
						}
						else
						{
							//secret not correct
						  mail(from, "Secret was incorrrect.");
						}
					}
				}
			}
			
			p.Delete();
		}
		p.Close();
		sleep(5);
	}
	
	return 0;
}
