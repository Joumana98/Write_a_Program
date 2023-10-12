/*
	calculator08buggy.cpp

	Helpful comments removed.

	We have inserted 3 bugs that the compiler will catch and 3 that it won't.
*/

#include "std_lib_facilites.h"

struct Token {
	char kind;
	double value;
	string name;
	Token(char ch) :kind(ch), value(0) { }
	Token(char ch, double val) :kind(ch), value(val) { }
	Token(char ch, string n) : kind(ch), name(n) { }
};

class Token_stream {
	bool full;
	Token buffer;
public:
	Token_stream() :full(0), buffer(0) { }

	Token get();
	void unget(Token t) { buffer = t; full = true; }
	void ignore(char);
};

const char let = 'L';
const char quit = 'Q';
const char print = ';';
const char number = '8';
const char name = 'a';
const char declkey = '#';
const char root = 'r';
const char power = 'p';
const string rootkey = "sqrt";
const string powkey = "pow";
const string quitkey = "exit";

Token Token_stream::get()
{
	if (full) { full = false; return buffer; }
	char ch;
	cin >> ch;
	switch (ch) {
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case '=':
	case ',':
	case print:
		return Token(ch);
	case '.':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	{	cin.unget(); //puts back the last character extracted into the input stream
	double val;
	cin >> val;
	return Token(number, val);
	}
	case declkey:
		return Token(let);
	default:
		if (isalpha(ch)) {
			string s;
			s += ch;// a name starts with a character
			while (cin.get(ch) && (isalpha(ch) || isdigit(ch))) // a name conatins either characters or digits
				s += ch;
			cin.unget();
			//if (s == declkey) return Token(let);
			if (s == rootkey) return Token(root);
			if (s == powkey) return Token(power);
			if (s == quitkey) return Token(quit);
			return Token(name, s);
		}
		error("Bad token");
	}
}

void Token_stream::ignore(char c)
{
	if (full && c == buffer.kind) { // first check if buffer has c, if yes empty the buffer
		full = false;
		return;
	}
	full = false;

	char ch;
	while (cin >> ch) // read and ignore until we find c
		if (ch == c) return;
}

struct Variable {
	string name;
	double value;
	Variable(string n, double v) :name(n), value(v) { }
};

vector<Variable> names; // vector to store Variables

double get_value(string s) // returns the value of Variable with name s
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s)
			return names[i].value;
	error("get: undefined name ", s);
}

void set_value(string s, double d)
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s) {
			names[i].value = d;
			return;
		}
	error("set: undefined variable ", s);
}

bool is_declared(string s)
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s) return true;
	return false;
}

Token_stream ts;

double expression();

double primary()
{
	Token t = ts.get();
	if (t.kind == root)
		return  sqrt(expression());
	else if (t.kind == power)
	{
		t = ts.get();
		if (t.kind != '(') error(" '(' expected in pow()");
		double d1 = expression();
		t = ts.get();
		if (t.kind != ',') error(" ',' expected in pow()");
		double d2 = expression();
		int i = narrow_cast<int> (d2);
		t = ts.get();
		if (t.kind != ')') error(" ')' expected in pow()");
		return pow(d1, i);
	}
	else
	{
		switch (t.kind) {
		case '(':
		{	double d = expression();
		t = ts.get();
		if (t.kind != ')') error("'(' expected");
		return d;
		}
		case '-': // unary -
			return -primary();
		case '+': // unary =
			return +primary();
		case number:
			return t.value;
		case name:
			return get_value(t.name); // returns the value of the variable
		default:
			error("primary expected");
		}
	}
}

double term()
{
	double left = primary();
	while (true) {
		Token t = ts.get();
		switch (t.kind) {
		case '*':
			left *= primary();
			break;
		case '/':
		{	double d = primary();
		if (d == 0) error("divide by zero");
		left /= d;
		break;
		}
		case '%':
		{
			double d = primary();
			if (d == 0)
				error("Division by 0.");
			left = fmod(left, d);
			break;
		}
		break;
		default:
			ts.unget(t);
			return left;
		}
	}
}

double expression()
{
	double left = term();
	while (true) {
		Token t = ts.get();
		switch (t.kind) {
		case '+':
			left += term();
			break;
		case '-':
			left -= term();
			break;
		default:
			ts.unget(t);
			return left;
		}
	}
}

double declaration()
{
	Token t = ts.get();
	if (t.kind != name) error("name expected in declaration");
	string name = t.name;
	if (is_declared(name)) error(name, " declared twice");
	Token t2 = ts.get();
	if (t2.kind != '=') error("= missing in declaration of ", name);
	double d = expression();
	names.push_back(Variable(name, d));
	return d;
}

double statement()
{
	Token t = ts.get();
	switch (t.kind) {
	case let:
		return declaration();
	default:
		ts.unget(t);
		return expression();
	}
}

void clean_up_mess()
{
	ts.ignore(print);
}

const string prompt = "> ";
const string result = "= ";

void calculate()
{
	while (true) try
	{
		cout << prompt;
		Token t = ts.get();
		while (t.kind == print) t = ts.get();
		if (t.kind == quit) return;
		ts.unget(t);
		cout << result << statement() << endl;
	}
	catch (runtime_error& e) {
		cerr << e.what() << endl;
		clean_up_mess();
	}
}

int main()

try {
	const int k = 1000;

	calculate();
	return 0;
}
catch (exception& e) {
	cerr << "exception: " << e.what() << endl;
	char c;
	while (cin >> c && c != print);
	return 1;
}
catch (...) {
	cerr << "exception\n";
	char c;
	while (cin >> c && c != print);
	return 2;
}