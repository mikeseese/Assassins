#ifndef MUSOCKET
#define MUSOCKET
#include "MikeUSocket.h"
#endif

#ifndef INCLUDES
#define INCLUDES
#include "includes.h"
#endif

class pop
{
	public:
		pop(bool display = false);
		MUSocket popSocket;
		void Connect(string host, bool is_name = true, bool ssl = false, int port = 110);
		void Login(string user, string pass);
		int NumUnread();
		void Retrieve(int num = 1);
		string GetMessageText();
		string GetMessage();
		string GetFrom();
		bool Delete(int num = 1);
		bool Status();
		void Close();
		
	private:
		string current_message;
		bool debug;
};

pop::pop(bool display)
{
	debug = display;
	popSocket = MUSocket(debug);
}

void pop::Close()
{
	popSocket.Send("quit");
	popSocket.Close();
	if(debug)
		cout << "POP connection closed." << endl;
}

void pop::Connect(string host, bool is_name, bool ssl, int port)
{
	char buf[256];
	popSocket.Start();
	popSocket.Connect(host, port, is_name);
	if(Status())
	{
		//connected!
		if(debug)
			cout << "Connected to POP server. Please login." << endl;
	}
}

void pop::Login(string user, string pass)
{
	popSocket.Send("user " + user + "\n");
	if(Status())
	{
		//username ok, put in password
		if(debug)
			cout << "Username good. Enter password." << endl;
		popSocket.Send("pass " + pass + "\n");
		if(Status())
		{
			//logged in
			if(debug)
				cout << "Logged in as " << user << "." << endl;
		}
		else
		{
			if(debug)
				cout << "Password isn't good." << endl;
		}
	}
	else
	{
		if(debug)
			cout << "No such username." << endl;
	}
}

int pop::NumUnread()
{
	char buf[256];
	string s;
	popSocket.Send("stat\n");
	
	popSocket.Receive(buf, 256, 0);
	if(buf[0] == '+' && buf[1] == 'O' && buf[2] == 'K')
	{
		int i = 4;
		while(buf[i] != ' ')
		{
			s += buf[i];
			i++;
		}
	}
	
	if(debug)
		cout << "Number of Unread Messages: " << s.c_str() << "." << endl;
		
	return atoi(s.c_str());
}

string pop::GetMessage()
{
	return current_message;
}

void pop::Retrieve(int num)
{
	//retrieve message
	char buf[256];
	string message;
	stringstream send;
	
	send << "retr " << num << endl;
	popSocket.Send(send.str());
	if(Status())
	{
		while(FindStringInArray(message, "\r\n.\r\n") == -1)
		{
			memset(buf, '\0', 256);
			popSocket.Receive(buf, 256, 0);
			message += buf;
		}
		
		if(debug)
			cout << "Message " << num << " retrieved." << endl;
			
		current_message = message;
	}
}

string pop::GetFrom()
{
	string full_message = GetMessage();
	string text = "";
	int pos = FindStringInArray(full_message, "From: ");
	
	if(pos != -1)
	{
		full_message = full_message.substr(pos);
		pos = FindStringInArray(full_message, "\r\n");
		if(pos != -1)
		{
			text = full_message.substr(0, pos - 2);
		}
	}
	
	return text;
}

string pop::GetMessageText()
{
	string full_message = GetMessage();
	string text = "";
	int pos = FindStringInArray(full_message, "Content-Type: text/plain");
	
	if(pos != -1)
	{
		full_message = full_message.substr(pos);
		pos = FindStringInArray(full_message, "\r\n\r\n");
		if(pos != -1)
		{
			text = full_message.substr(pos + 1, full_message.length() - pos - 8);
		}
	}
	
	return text;
}

bool pop::Delete(int num)
{
	stringstream send;
	send << "dele " << num << endl;
	popSocket.Send(send.str());
	if(Status())
	{
		if(debug)
			cout << "Message " << num << " deleted successfully." << endl;
			
		return true;
	}
	else
	{
		if(debug)
			cout << "Could not delete message " << num << "." << endl;
			
		return false;
	}
}

bool pop::Status()
{
	char buf[256];
	
	popSocket.Receive(buf, 256, 0);
	
	//if(debug)
	//	cout << buf;
		
	if(buf[0] == '+' && buf[1] == 'O' && buf[2] == 'K')
	{
		return true;
	}
	
	return false;
}
