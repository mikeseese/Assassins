#ifndef MUSOCKET
#define MUSOCKET
#include "MikeUSocket.h"
#endif

#ifndef INCLUDES
#define INCLUDES
#include "includes.h"
#endif

class smtp
{
	public:
		smtp(bool display = false);
		MUSocket smtpSocket;
		void Connect(string host, bool is_name = true, bool ssl = false, int port = 25);
		void Retrieve(int num = 1);
		void SetMessageText(string text);
		void SetFrom(string from);
		void SetDomain(string domain);
		void SetTo(string to);
		//void SetName(string name);
		int Status();
		void Close();
		
	private:
		string current_message;
		string senderName;
		bool debug;
};

smtp::smtp(bool display)
{
	debug = display;
	smtpSocket = MUSocket(debug);
}

void smtp::Close()
{
	stringstream send;
	send << "QUIT";
	smtpSocket.Send(send.str());
	smtpSocket.Close();
}

void smtp::Connect(string host, bool is_name, bool ssl, int port)
{
	char buf[256];
	smtpSocket.Start();
	smtpSocket.Connect(host, port, is_name);
	if(Status() == 220)
	{
		//connected!
		if(debug)
			cout << "Connected to SMTP server. Please send HELO." << endl;
	}
}

void smtp::SetDomain(string domain)
{
	stringstream send;
	send << "HELO " << domain << endl;
	smtpSocket.Send(send.str());
	if(Status() == 250)
	{
		//HELO accepted
		if(debug)
			cout << "You greeted the server correctly." << endl;
	}
}

void smtp::SetFrom(string from)
{
	stringstream send;
	send << "MAIL FROM:" << from << endl;
	smtpSocket.Send(send.str());
	if(Status() == 250)
	{
		//From accepted
		if(debug)
			cout << "You set the sender successfully. Please specify recipient address." << endl;
	}
}

void smtp::SetTo(string to)
{
	stringstream send;
	send << "RCPT TO:<" << to << ">" << endl;
	smtpSocket.Send(send.str());
	if(Status() == 250)
	{
		//Rcpt accepted
		if(debug)
			cout << "You set the recipient successfully. Please send the message text." << endl;
	}
}

void smtp::SetMessageText(string text)
{
	stringstream send;
	send << "DATA" << endl;
	smtpSocket.Send(send.str());
	if(Status() == 354)
	{
		//Waiting for message
		stringstream send2;
		send2 << text << endl << "." << endl;
		smtpSocket.Send(send2.str());
		if(Status() == 250)
		{
			//message accepted and sent
			if(debug)
				cout << "You sent the message successfully. Send another email or close the connection." << endl;
		}
	}
}

int smtp::Status()
{
	char buf[256];
	int code;
	
	smtpSocket.Receive(buf, 256, 0);
	
	//if(debug)
	//	cout << buf << endl;
	
	code = atoi(buf);
	//cout << code << endl;
	
	return code;
}
