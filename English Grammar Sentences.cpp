#include "std_lib_facilites.h"

bool isNoun(string s)
{
	if (s == "birds" || s == "fish" || s == "C++")
		return true;
	else
		return false;
}

bool isVerb(string s)
{
	if (s == "rules" || s == "fly" || s == "swim")
		return true;
	else
		return false;
}

bool isConjuction(string s)
{
	if (s == "and" || s == "or" || s == "but")
		return true;
	else
		return false;
}

bool isArticle(string s)
{
	if (s == "the")
		return true;
	else
		return false;
}
bool isEnd(string s)
{
	char c;
	if (s == ".")
	{
		cin.get(c);
		if (cin) {
			if (c == ' ')
				return true;
		}
		return false;
	}
	return false;
}

bool isPrimary()
{
	string word;
	cin >> word;
	if (!isEnd(word))
	{
		if (isArticle(word))
		{
			cin >> word;
			if (!isEnd(word))
			{
				if (isNoun(word))
				{
					cin >> word;
					if (!isEnd(word))
					{
						if (isVerb(word))
							return true;
					}
					return false;
				}
				return false;

			}
			return false;
		}

		else if (isNoun(word))
		{
			cin >> word;
			if (!isEnd(word))
			{
				if (isVerb(word))
					return true;
			}
			return false;

		}
		return false;

	}
	return false;

}

bool isSentence()
{
	string word;

	bool isleft = isPrimary();
	if (isleft)
	{
		cin >> word;
		if (!isEnd(word))
		{
			if (isConjuction(word))
			{
				bool isRight = isPrimary();
				if (isRight) {
					cin >> word;
					if (isEnd(word))
						return true;
				}

				return false;
			}
			else
				return false;
		}
		return true;
	}
	return false;
}

int main()
{
	cout << "Enter a sentence and end it with \" . \": ";
	if (isSentence() == true)
		cout << "OK" << endl;
	else
		cout << "not OK" << endl;
	return 0;
}