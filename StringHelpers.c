
// Return 1 if the str contains c, otherwise 0 
int StringContains(char* str, char c)
{
	while(*str)
	{
		if(*str == c)
			return 1; 
		str++; 
	}

	return 0; 
}

// Return the number of occurences of c in str 
int StringOccurences(char* str, char c)
{
	int count = 0; 
	while(*str)
	{
		if(*str == c)
		{
			count++; 
		}
		str++; 
	}

	return count; 
}

// Return the first index of a specific sub-string s in str 
// or -1 if the substring is not found in str 
int GetIndex(char* str, char* s)
{
	int len; 	// length of string 
	int slen; 	// length of substring 
	int i; 
	int MaxIndex; 	// The last position where the substring could start in the string and fit ok 

	len = strlen(str); 
	slen = strlen(s); 
	MaxIndex = len - sublen; 
	i = 0; 
	while(i < MaxIndex)
	{
		if(strncmp(&str[i], s, slen) == 0)
		{
			return i; 
		}

		i++; 
	}

	return -1; 	// substring was not found 

}