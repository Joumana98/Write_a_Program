#include "std_lib_facilites.h"

class Token
{
public:
	char kind;
	double value;
};

class Token_stream
{
public:
	Token get();
	void pushback(Token t);
private:
	bool full;
	Token buffer;
};

void Token_stream::pushback(Token t)
{
	if (full)
		error("Buffer is full");
	full = true;
	buffer = t;
}

Token Token_stream::get()
{
	if (full)
	{
		full = false;
		return buffer;
	}
	char c;
	cin >> c;
	switch (c)
	{
	case ';': case 'q':
	case '(': case ')':
	case '+': case'-': case '*': case '/':
		return Token{ c };
		break;
	case'.':
	case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
	{   cin.putback(c);
	double val;
	cin >> val;
	return Token{ '8',val };
	}

	}
}

Token_stream ts; // make get() and putback() available;

double expression();

double primary()
{
	Token t = ts.get();
	switch (t.kind)
	{
	case '(':
	{
		double d = expression();
		t = ts.get();
		if (t.kind != ')')
			error(" ')' is expected!");
		break;
	}
	case '8':
		return t.value;
	default:
		error("Expected a primary!");

	}
}

double term()
{
	double left = primary();
	Token t = ts.get();
	while (true)
	{
		switch (t.kind)
		{
		case '*':
			left = left * primary();
			t = ts.get();
			break;
		case'/':
		{
			double d = primary();
			if (d == 0)
				error("Division by 0!");
			left = left / d;
			t = ts.get();
			break;
		}

		default:
			ts.pushback(t);
			return left;
		}

	}
}

double expression()
{
	double left = term();
	Token t = ts.get();
	while (true)
	{
		switch (t.kind)
		{
		case '+':
			left = left + term();
			t = ts.get();
			break;
		case'-':
			left = left - term();
			t = ts.get();
			break;
		default:
			ts.pushback(t);
			return left;
		}

	}
}

int main()
try {
	cout << "Enter an expression ended with ; :" << endl;
	cout << "Enter q to quit." << endl;

	double val = 0;
	while (cin)
	{
		Token t = ts.get();

		if (t.kind == 'q')
			break;
		if (t.kind == ';')
			cout << " = " << val << endl;
		else
			ts.pushback(t);
		val = expression();
	}


}
catch (exception& e)
{
	cerr << e.what() << endl;
	return 1;
}

catch (...)
{
	cerr << "Exception!!" << endl;
	return 2;
}
