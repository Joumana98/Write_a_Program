#include "std_lib_facilites.h"

class Name_value
{
public:
	string name;
	double value;

};

int main()
{
	vector<Name_value> v;
	string s;
	double val;
	cin >> s;
	cin >> val;
	while (s != "NoName" || val != 0)
	{
		v.push_back({ s,val });
		cin >> s;
		cin >> val;
		for (Name_value x : v)
		{
			if (x.name == s)
				cout << "Name " << s << " is entered multiple times!!" << endl;
		}
	}

	for (Name_value x : v)
		cout << x.name << "  " << x.value << endl;

	cout << endl;


	return 0;
}