#ifndef INCLUDES
#define INCLUDES
#include "includes.h"
#endif

string StringToLower(string s)
{
	transform(s.begin(), s.end(), s.begin(), ::tolower);
		
	return s;
}

void terminateCharArray(char Array[])
{
	//Terminate the string with a null byte
	long int i;

	for(i = 0; i < strlen(Array); i++)
	{
		if(Array[i] == -52)
			Array[i] = '\0';
	}
}

string itos(int i)
{
	std::stringstream ss;
  ss << i;
	return ss.str();
}

void emptyCharArray(char Array[])
{
	//Empty character array
	long int i;

	for(i = 0; i < strlen(Array); i++)
	{
		Array[i] = '\0';
	}
}

void CharArrayCopySelection(char Source[], char Target[], long int BeginningSource, long int Length, long int BeginningTarget)
{
	//Copy a selection from one array to another, starting at a specified location
	long int i;

	for(i = 0; i < Length; i++)
	{
		Target[BeginningTarget + i] = Source[BeginningSource + i];
	}
}

int ArrayToStringCompare(char chInput[], string strCompare, int startPos, bool caseSensitive)
{
	int i = startPos;
	int length = strCompare.length();
	bool buildString = true;
	string strInput = "";
	string strChar = "";

	while(buildString)
	{
		strChar = chInput[i];
		strInput = strInput.append(strChar);
		i++;
		if(strInput.length() == strCompare.length())
			buildString = false;
	}
	if(!caseSensitive)
	{
		for(int k = 0; k < length; k++)
		{
			strInput[k] = toupper(strInput[k]);
			strCompare[k] = toupper(strCompare[k]);
		}
	}
	if(strInput == strCompare)
		return i; //Return the position in the array that follows the string
	else
		return -1;
}

int FindStringInArray(string chInput, string strFind, bool caseSensitive = false)
{
	int startPos = 0;
	int i;
	int length = strFind.length();
	bool buildString;
	bool findString = true;
	string strInput = "";
	string strChar = "";

	while(findString)
	{
		i = startPos;
		strInput = "";
		if(chInput.length() - startPos < length) //There is not enough space in chInput to search for the string
			return -1; //String was not in the array
		buildString = true;
		while(buildString)
		{
			strChar = chInput[i];
			strInput = strInput.append(strChar);
			i++;
			if(strInput.length() == strFind.length())
				buildString = false;
		}
		if(!caseSensitive)
		{
			for(int k = 0; k < length; k++)
			{
				strInput[k] = toupper(strInput[k]);
				strFind[k] = toupper(strFind[k]);
			}
		}
		if(strInput == strFind)
			return i; //Return the position in the array that follows the string
		else
			startPos++;
	}
}

string ReplaceInstancesInString(string strInput, string strReplacee, string strReplacer)
{
	int i;
	int count = 0;
	int pos;

	while(true)
	{
		pos = FindStringInArray(strInput, strReplacee) - strReplacee.length(); //want beginning of
		if(pos == -1 - strReplacee.length())
			break;
		strInput.erase(pos, strReplacee.length());
		strInput.insert(pos, strReplacer);
	}
	
	return strInput; //return number of replaced
}
