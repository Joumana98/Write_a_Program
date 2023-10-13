// read whitespace as an end of an expression
//Add some instructions
// Suggest an improvement: type prev to use the last calculated value in current expression

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
	bool full2;
	Token buffer;
	Token buffer2;
public:
	Token_stream() :full(0), full2(0), buffer(0), buffer2(0) { }

	Token get();
	void unget(Token t);
	void unget2(Token t1, Token t2);
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
const char constant = 'c';
const char previous = '<';
const string rootkey = "sqrt";
const string powkey = "pow";
const string quitkey = "quit";
const string constkey = "const";
const string prevkey = "prev";

void Token_stream::unget(Token t)
{
	buffer = t; full = true;
}

void Token_stream::unget2(Token t1, Token t2)
{
	buffer = t1;
	buffer2 = t2;
	full = true;
	full2 = true;
}


Token Token_stream::get()
{
	if (full)
	{
		full = false;
		return buffer;
	}

	if (full2)
	{
		full2 = false;
		return buffer2;
	}
	char ch;
	cin.get(ch);
	if (ch == '\n')
		ch = print;
	while (ch == ' ')
		cin.get(ch);

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
			while (cin.get(ch) && (isalpha(ch) || isdigit(ch)) || ch == '_') // a name contains either characters or digits or _
				s += ch;
			cin.unget();
			//if (s == declkey) return Token(let);
			if (s == rootkey) return Token(root);
			if (s == powkey) return Token(power);
			if (s == quitkey) return Token(quit);
			if (s == constkey) return Token(constant);
			if (s == prevkey) return Token(previous);
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
	bool isConst;
	Variable(string n, double v, bool t) :name(n), value(v), isConst(t) { }
};

class Symbol_table {
	vector<Variable> var_table;

public:
	double get_value(string s);
	void set_value(string s, double d);
	bool is_declared(string s);
	void push(Variable v);
};

void Symbol_table::push(Variable v)
{
	var_table.push_back(v);
}


double Symbol_table::get_value(string s) // returns the value of Variable with name s
{
	for (int i = 0; i < var_table.size(); ++i)
		if (var_table[i].name == s)
			return var_table[i].value;
	error("get: undefined name ", s);
}

void Symbol_table::set_value(string s, double d)
{
	for (int i = 0; i < var_table.size(); ++i)
		if (var_table[i].name == s)
		{
			if (var_table[i].isConst == true) error(var_table[i].name, " is a constant, can't change its value!");
			else {
				var_table[i].value = d;
				return;
			}
		}
	error("set: undefined variable ", s);
}

bool Symbol_table::is_declared(string s)
{
	for (int i = 0; i < var_table.size(); ++i)
		if (var_table[i].name == s) return true;
	return false;
}

Token_stream ts;
Symbol_table names;

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
			return names.get_value(t.name); // returns the value of the variable
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

double declaration(bool con)
{
	Token t = ts.get();// suppose wet read "let"
	if (t.kind != name) error("name expected in declaration");
	string name = t.name;
	if (names.is_declared(name)) error(name, " declared twice");
	Token t2 = ts.get();
	if (t2.kind != '=') error("= missing in declaration of ", name);
	double d = expression();
	names.push(Variable(name, d, con));
	return d;
}

double assignment(string name)
{
	if (!names.is_declared(name)) error(name, " is not declared");// suppose we read a name
	double d = expression();
	names.set_value(name, d);
	return d;
}

double statement()
{
	Token t = ts.get();
	switch (t.kind) {
	case let:
		return declaration(0);
	case constant:
		return declaration(1);
	case name:
	{
		string n = t.name;
		Token t2 = ts.get();
		if (t2.kind == '=') // "=" after a name means an assignment
			return assignment(n);
		ts.unget2(t, t2); // put back the character that is not "="
		return expression();
	}

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
	double p = 0;
	while (true) try
	{
		cout << prompt;
		Token t = ts.get();
		while (t.kind == print) t = ts.get();
		if (t.kind == quit) return;
		ts.unget(t);
		if (t.kind == previous)
		{
			cout << "prev = " << p << endl;
			ts.unget(Token(number, p));
		}
		p = statement();
		cout << result << p << endl;
	}
	catch (runtime_error& e) {
		cerr << e.what() << endl;
		clean_up_mess();
	}
}

int main()

try {
	string inst;
	cout << "For instructions about how to use this calculator, please type help / Help." << endl;
	cout << "To exit, please press any character." << endl;
	cin >> inst;
	if (inst == "help" || inst == "Help")
	{
		cout << "- After " << prompt << "enter the expression to calculate" << endl;
		cout << "- End your expression with ; or simply press Enter" << endl;
		cout << "- To exit the calculator, please type " << quitkey << endl;
		cout << "- The operations supported are +,-,* / and %" << endl;
		cout << "- Some funtions are also availble: sqrt() , pow(double,int)" << endl;
		cout << "- You can define variables using the keyword" << declkey << endl;
		cout << "- You can define constants using the keyword const" << endl;
		cout << "- Type " << prevkey << " to use the last calculated value in current expression" << endl;
	}


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