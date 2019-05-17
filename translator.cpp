#include <iostream>
#include <cstring>
#include <ctype.h>
#include <cstdio>
#include <stack>
#include <vector>
using namespace std;

int ns=1;
string err;
int f=1;


//// Типы лексем ////
enum type_of_lex {
  LEX_NULL,      /*0*/
  LEX_AND, LEX_BOOL, LEX_BREAK, LEX_ELSE, LEX_IF, LEX_FALSE, LEX_INT, 
  LEX_NOT, LEX_OR, LEX_PROGRAM, LEX_READ, LEX_UMIN, LEX_TRUE, LEX_WHILE, LEX_WRITE, LEX_STRING,
  LEX_FIN,       /*17*/
  LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_ASSIGN, LEX_LPAREN, LEX_RPAREN, LEX_EQ, LEX_LSS, 
  LEX_GTR, LEX_PLUS, LEX_MINUS, LEX_TIMES, LEX_SLASH, LEX_LEQ, LEX_NEQ, LEX_GEQ, LEX_KL, LEX_KR, /*35*/
  LEX_NUM,     
  LEX_ID, 
  LEX_STR,   
  POLIZ_LABEL,   
  POLIZ_ADDRESS,
  POLIZ_GO,     
  POLIZ_FGO
}; 

//// Лексема ////
class Lex{
	type_of_lex t_lex;
	int v_lex;
	
public:
	Lex ( type_of_lex t = LEX_NULL, int v = 0){
		t_lex = t;
		v_lex = v;
	}
	
	type_of_lex get_type (){
		return t_lex;
	}
	
	int get_value (){
		return v_lex;
	}
	
	friend ostream& operator << ( ostream &s, Lex l );
};

//// Идентификатор ////
class Ident{
	char * name;
	bool declare;
	type_of_lex type;
	bool assign;
	int value;
	public:
	
	Ident (){
		declare = false;
		assign = false;
	}
	
	char* get_name (){
		return name;
	}
	
	void put_name (const char *n){
		name = new char [ strlen(n) + 1 ];
		strcpy ( name, n );
	}
	
	bool get_declare (){
		return declare;
	}
	
	void put_declare (){
		declare = true;
	}
	
	type_of_lex get_type (){
		return type;
	}
	
	void put_type ( type_of_lex t ){
		type = t;
	}
	
	bool get_assign (){
		return assign;
	}
	
	void put_assign (){
		assign = true;
	}
	
	int get_value (){
		return value;
	}
	
	void put_value (int v){
		value = v;
	}
};

//// Таблица идентификаторов ////
class Tabl_Ident{
	Ident* p;
	int size;
	int top;
	
public:
	Tabl_Ident ( int max_size ){
		p = new Ident[size = max_size];
		top = 1;
	}
	
	~Tabl_Ident (){
		delete [] p;
	}
	
	Ident& operator[] ( int k ){
		return p[k];
	}
	
	int put ( const char *buf );
	
	void print(){
		for( int i=1; i < top; ++i)
		cout << p[i].get_name() << endl; 
	}
};

int Tabl_Ident::put ( const char *buf ){
	for ( int j=1; j<top; ++j )
		if ( !strcmp(buf, p[j].get_name()) )
			return j;
			
	p[top].put_name(buf);
	++top;
	return top-1;
}

//// Таблица строковых констант ////
class Tabl_Str{
	char* p[200];
	int top;
	
public:
	Tabl_Str(){
		top = 1;
	}
	
	~Tabl_Str(){
		for( int i = 0; i < top ; ++i)
		delete [] p[i];	
	}
	
	char* operator[] ( int k ){
		return p[k];
	}
	
	int put ( const char *buf ){
		p[top] = new char[strlen(buf)+1];
		strcpy(p[top],buf);
		++top;
		return top-1;
	}
	
	void print(){
		for( int i=1; i < top; ++i)
		cout << p[i] << endl; 
	}
	
};

Tabl_Ident TID(200);
Tabl_Str Tstr;

//// Лексический анализатор ////
class Scanner{
	
	FILE * fp;
	char c;
	
	char buf[100];
	int buf_top;
	
	void clear (){
		buf_top = 0;
		for ( int j = 0; j < 100; ++j )
		buf[j] = '\0';
	}
	
	void add (){
		buf [ buf_top ++ ] = c;
	}
	
	int look ( const char *buf, const char **list ){
		int i = 0;
		while ( list[i] ){
			if ( !strcmp(buf, list[i]) )
				return i;
			++i;
		}
		return 0;
	}
	
	void gc (){
		c = fgetc (fp);
	}
	
public:
	static const char * TW[];
	static const char * TD[];
	Lex get_lex ();
	Scanner ( const char * program ){
		if((fp = fopen ( program, "r" )) == 0)
			throw "\033[1;31mOpen File Error\033[0m";
	}
};

const char * Scanner::TW[] = {"NULL", "and", "bool", "break", "else", "if", "false", "int", "not", "or", "program", "read", "Unar_minus", "true", "while", "write", "string", NULL};
const char * Scanner::TD[] = {"EOF",";", ",", ":", "=", "(", ")", "==", "<", ">", "+", "-", "*", "/", "<=", "!=", ">=", "{", "}", NULL};

ostream& operator << ( ostream &s, Lex l ){
	char t[30];
	if (l.t_lex <= 16)
		strcpy(t, Scanner::TW[l.t_lex]);
	else if (l.t_lex >= 17 && l.t_lex <= 35)
		strcpy(t, Scanner::TD[l.t_lex-17]);
    else if (l.t_lex == 36)
        strcpy(t,"NUMBER");
    else if (l.t_lex == 37)
        strcpy(t,TID[l.v_lex].get_name());
    else if (l.t_lex == 38)
        strcpy(t,Tstr[l.v_lex]);
    else if (l.t_lex == 39)
        strcpy(t,"Label");
    else if(l.t_lex == 40)
        strcpy(t,"Addr");
    else if (l.t_lex == 41)
        strcpy(t ,"!");
    else if (l.t_lex == 42) 
        strcpy(t,"!F");
    else
		throw l;
    s << "\"" << t << "\" ";
    return s;
}

Lex Scanner::get_lex (){
	enum state { H, IDENT, NUMB, COM, ALE, DELIM, NEQ, KAV };
    int d, j;
    state CS = H;
    clear ();
    
	do{
		gc();
		switch ( CS ){
			
			case H:	
				if ( c ==' ' || c=='\r' || c =='\t' );
				else if ( c == '\n')
					++ns;
				else if ( isalpha(c) ){
					add ();
					CS = IDENT;
				}
				else if ( isdigit(c) ){
					d = c - '0';
					CS = NUMB;
				}
				else if ( c== '/' ){
					gc();
					if (c == '*')
						CS = COM;
					else{
						ungetc(c, fp);
						ungetc('/', fp);
						CS = DELIM;
					}
				}
				else if ( c== '=' || c== '<' || c== '>'){
					add ();
					CS = ALE;
				}
				else if ( c == EOF )
					return Lex(LEX_FIN);
				else if ( c == '!' ){
					add ();
					CS = NEQ;
				}
				else if ( c == '"')
					CS = KAV;
				else{
					CS = DELIM;
					ungetc(c,fp);
				}
				break;
			case IDENT:
				if ( isalpha(c) || isdigit(c) )
					add ();
				else{
					ungetc(c,fp);
					if ( (j = look (buf, TW)) )
						return Lex((type_of_lex)j, j);
					else{
						j = TID.put(buf);
						return Lex (LEX_ID,j);
					}
				}
				break;
			case NUMB:
				if ( isdigit(c) )
					d = d * 10 +(c - '0');
				else {
					ungetc (c, fp);
					return Lex (LEX_NUM, d);
				}
				break;
			case COM:
				if ( c == '*' ){
					gc();
					if (c == '/')
						CS = H;
					else
						ungetc(c,fp);
				}
				else if (c == EOF)
				   throw c;
				break;
			case ALE:
				if ( c== '=') {
					add();
					j = look ( buf, TD );
					return Lex ((type_of_lex)(j+(int)LEX_FIN), j);
				}
				else {
					ungetc (c, fp);
					j = look ( buf, TD );
					return Lex ((type_of_lex)(j+(int)LEX_FIN), j);
				}
				break;
			case NEQ:
				if (c == '=') {
					add();
					j = look ( buf, TD );
					return Lex ( LEX_NEQ, j );
				}
				else
					throw '!';
				break;
			case KAV:
				if ( c!= '"')
					add();
				else{
					j = Tstr.put(buf);
					return Lex (LEX_STR,j);
				}
				break;
			case DELIM:
				add ();
				if ( (j = look(buf, TD)) )
					return Lex ((type_of_lex)(j+(int)LEX_FIN), j);
                else
                    throw c;
				break;
		}
	} while ( true );
}

//// Синтаксический анализатор ////
class Parser{
	Lex curr_lex, pred_lex, pro_lex;
	Lex c_lex;
	type_of_lex c_type;
	int c_val,val,znach,whil,c,rt,ass;
	Scanner scan;
    stack <type_of_lex> st_lex;
    vector <int> st_whil;
    stack <int> cw;
	
	// Процедуры РС-метода
	void P(); //начало
	void OS(); //описания
	void O(); //описание
	void OPS(); //операторы
	void OP(); //оператор
	
	// Выражения
	void E();   
	void E0();
	void E1();
	void E2();
	void E9();
	void T();
	void F();
	
	// Семантический анализ
	void dec(type_of_lex type);
	void check_id ();
	void check_op ();
	void check_not ();
	void check_int ();
	void eq_bool ();
	void check_id_in_read ();
	
	void gl (){
		if(rt){
			pred_lex=curr_lex;
			curr_lex=pro_lex;
			c_type = pro_lex.get_type();
			c_val = pro_lex.get_value();
		}
		else{
			pred_lex=curr_lex;
			curr_lex = scan.get_lex();
			c_type = curr_lex.get_type();
			c_val = curr_lex.get_value();
		}
		rt=0;
			
	}
	
	void rl(){
		pro_lex=curr_lex;
		curr_lex=pred_lex;
		c_type = pred_lex.get_type();
		c_val = pred_lex.get_value();
		rt=1;
	}
	
public:
	vector <Lex> poliz;
	Parser ( const char *program) : scan (program) {val =0; whil=0; curr_lex=Lex(); rt=0;}
	void analyze();
};

void Parser::analyze (){
    gl();
    P();
    if (c_type != LEX_FIN)
		throw curr_lex;
}

//начало
void Parser::P () {
    if (c_type == LEX_PROGRAM){
		gl();
    }
    else 
        throw curr_lex;    
    if (c_type == LEX_KL){
		gl();		
    }
    else 
        throw curr_lex; 
    OS();
    OPS();
    
    if (c_type == LEX_KR){
		gl();
    }
    else 
        throw curr_lex; 
}

//описания
void Parser::OS () {
	while(c_type == LEX_INT || c_type == LEX_STRING || c_type == LEX_BOOL)
		O();
}

void Parser::O () {
	if (c_type == LEX_INT){
        do {
			gl();
			if (c_type != LEX_ID)
				throw curr_lex;
			else{
				c_lex = curr_lex;
				gl();
				if(c_type == LEX_ASSIGN){
					gl();
					if(c_type == LEX_MINUS || c_type == LEX_PLUS){
						val = c_type == LEX_MINUS;
						gl();
					}
					if (c_type != LEX_NUM)
						throw curr_lex;
					else{
						if(val)
							val=-c_val;
						else
							val=c_val;
						znach = 1;
						gl();
					}	
				}
				dec ( LEX_INT );
				znach = 0;
				if(c_type != LEX_COMMA && c_type != LEX_SEMICOLON)
					throw curr_lex;
			}
	 
		} while (c_type == LEX_COMMA);
	}
		
	if (c_type == LEX_STRING){
        do {
			gl();
			if (c_type != LEX_ID)
				throw curr_lex;
			else{
				c_lex = curr_lex;
				gl();
				if(c_type == LEX_ASSIGN){
					gl();
					if (c_type != LEX_STR)
						throw curr_lex;
					else{
						znach = 1;
						val = c_val;
						gl();
					}
				}
				dec ( LEX_STRING );
				znach = 0;
				if(c_type != LEX_COMMA && c_type != LEX_SEMICOLON)
					throw curr_lex;
			}
	 
		} while (c_type == LEX_COMMA);
	}
	
	if (c_type == LEX_BOOL){
        do {
			gl();
			if (c_type != LEX_ID)
				throw curr_lex;
			else{
				c_lex = curr_lex;
				gl();
				if(c_type == LEX_ASSIGN){
					gl();
					if (c_type != LEX_TRUE && c_type != LEX_FALSE)
						throw curr_lex;
					else{
						znach = 1;
						val = c_type == LEX_TRUE;
						gl();
					}
				}
				dec ( LEX_BOOL );
				znach = 0;
				if(c_type != LEX_COMMA && c_type != LEX_SEMICOLON)
					throw curr_lex;
			}
	 
		} while (c_type == LEX_COMMA);
	}
	gl();	
}

//операторы
void Parser::OPS () {
	while((c_type == LEX_IF || c_type == LEX_WHILE || c_type == LEX_WRITE || c_type == LEX_READ || c_type == LEX_ID || c_type == LEX_KL || c_type == LEX_BREAK || \
		   c_type == LEX_FALSE || c_type == LEX_TRUE || c_type == LEX_NOT || c_type == LEX_LPAREN || c_type == LEX_MINUS) && f == 1)
		OP();
	f = 1;
}

void Parser::OP () {
	int pl0, pl1, pl2, pl3;
	
	if(c_type == LEX_IF){       ////if
		gl();
		if(c_type != LEX_LPAREN)
			throw curr_lex;
		gl();
		E();
		eq_bool();
		
		pl2 = poliz.size();
        poliz.push_back (Lex());
        poliz.push_back (Lex(POLIZ_FGO));
        
		if(c_type != LEX_RPAREN)
			throw curr_lex;
		gl();
        if (c_type != LEX_KL)
			OP();
		else
			OPS();
			
		pl3 = poliz.size();
        poliz.push_back (Lex());
        poliz.push_back (Lex(POLIZ_GO));
        poliz[pl2] = Lex(POLIZ_LABEL, poliz.size());

		if (c_type == LEX_ELSE) {
			gl();
			if (c_type != LEX_KL)
				OP();
			else
				OPS();
		
			poliz[pl3] = Lex(POLIZ_LABEL, poliz.size());
        
		}
		else
			throw curr_lex;		
	}
	
	else if(c_type == LEX_WHILE){    ////while
		++whil;
		cw.push(0);
		gl();
		if(c_type != LEX_LPAREN)
			throw curr_lex;
			
		pl0 = poliz.size();
		
		gl();
		E();
		eq_bool();
		
		pl1 = poliz.size();
        poliz.push_back (Lex());
        poliz.push_back (Lex(POLIZ_FGO));
  
		if(c_type != LEX_RPAREN)
			throw curr_lex;
		gl();
        if (c_type != LEX_KL)
			OP();
		else
			OPS();
		
        poliz.push_back (Lex(POLIZ_LABEL,pl0));
        poliz.push_back (Lex(POLIZ_GO));
        poliz[pl1] = Lex(POLIZ_LABEL, poliz.size());
        
        c=cw.top();
        cw.pop();
		
		for( int i = 0; i < c; ++i)
			poliz[st_whil[st_whil.size()-1-i]] = Lex(POLIZ_LABEL, poliz.size());	
		st_whil.erase(st_whil.end()-c,st_whil.end());
		
		--whil;	
	}
	
	else if(c_type == LEX_WRITE){    ////write
		gl();
        if (c_type == LEX_LPAREN) {
			gl();
            E();
            poliz.push_back (Lex(LEX_WRITE));
            while (c_type == LEX_COMMA){
				gl();
				E();
				poliz.push_back (Lex(LEX_WRITE));
			}
            if (c_type == LEX_RPAREN){
				gl();
				if (c_type != LEX_SEMICOLON)
					throw curr_lex;
				gl();
			}
            else
				throw curr_lex;
		}
        else
			throw curr_lex;
	}
	
	else if(c_type == LEX_READ){      ////read
		gl();
        if (c_type == LEX_LPAREN) {
			gl();
            if (c_type == LEX_ID){
				check_id_in_read ();
				poliz.push_back (Lex( POLIZ_ADDRESS, c_val));
				gl();
			}
            else
				throw curr_lex;
            if ( c_type == LEX_RPAREN ){
				gl();
				poliz.push_back (Lex (LEX_READ));
				if (c_type != LEX_SEMICOLON)
					throw curr_lex;
				gl();
			}	
            else
				throw curr_lex;
		}
        else  
			throw curr_lex;
	}
	
	else if(c_type == LEX_KL){    ////kav
		gl();
		OPS();
		if (c_type != LEX_KR)
			throw curr_lex;
		gl();
		f = 0;
	}
	
	else if (c_type == LEX_BREAK){  //break
		if(whil == 0)
			throw curr_lex;
		else{
		
		c=cw.top();
		cw.pop();
		c++;
		cw.push(c);
		
		st_whil.push_back(poliz.size());
		poliz.push_back (Lex());
        poliz.push_back (Lex(POLIZ_GO));
        
        gl();
        if(c_type != LEX_SEMICOLON)
			throw curr_lex;
		gl();
		}
        
	}
	
	else{
		ass = 0;
		E();
		if (ass > 1 )
			throw Lex(LEX_ASSIGN);
		if (c_type != LEX_SEMICOLON)
				throw curr_lex;
		gl();
	}
}


//выражения
void Parser::E (){
	E9();
	if (c_type == LEX_ASSIGN){
		++ass;
		st_lex.push (c_type);
		gl();
		E9();
		check_op ();
	}
}

void Parser::E9 (){
	E0();
	if (c_type == LEX_OR){
		st_lex.push (c_type);
		gl();
		E0();
		check_op ();
	}
}

void Parser::E0 (){
	E1();
	while ( c_type == LEX_AND ){
		st_lex.push (c_type);
		gl();
		E2();
		check_op ();
	}
}

void Parser::E1 (){
	E2();
	while ( c_type == LEX_EQ || c_type == LEX_LSS || c_type == LEX_GTR || c_type == LEX_LEQ || c_type == LEX_GEQ || c_type == LEX_NEQ ){
		st_lex.push (c_type);
		gl();
		E2();
		check_op();
	}
}

void Parser::E2 () {
	T();
	while ( c_type == LEX_PLUS || c_type == LEX_MINUS ) {
		st_lex.push (c_type);
		gl();
		T();
		check_op ();
  }
}

void Parser::T (){
	F();
	while ( c_type == LEX_TIMES || c_type==LEX_SLASH ){
		st_lex.push (c_type);
		gl();
		F();
		check_op ();
	}
}

void Parser::F () {
	if ( c_type == LEX_ID ){
		check_id ();
		gl();
		if(c_type == LEX_ASSIGN){
			rl();
			poliz.push_back(Lex(POLIZ_ADDRESS,c_val));
		}
		else{
			rl();
			poliz.push_back(curr_lex);
		}
		gl();
	}
    else if ( c_type == LEX_NUM ){
		st_lex.push(LEX_INT);
		poliz.push_back(curr_lex);
		gl();
	}
	else if (c_type == LEX_STR ){
		st_lex.push(LEX_STRING);
		poliz.push_back(curr_lex);
		gl();
	}
	else if (c_type == LEX_FALSE ){
		st_lex.push(LEX_BOOL);
		poliz.push_back(Lex(LEX_FALSE,0));
		gl();
	}
	else if (c_type == LEX_TRUE ){
		st_lex.push(LEX_BOOL);
		poliz.push_back(Lex(LEX_TRUE,1));
		gl();
	}
    else if (c_type == LEX_NOT){
		gl();
		F(); 
		check_not ();
	}
	else if (c_type == LEX_MINUS){
		gl();
		F();
		check_int();
	}
	else if ( c_type == LEX_LPAREN ) {
		gl(); 
		E();
		if ( c_type == LEX_RPAREN)
			gl();
		else 
			throw curr_lex;
	}
	else 
		throw curr_lex;
}

void Parser::dec ( type_of_lex type ) {
	
	if ( TID[c_lex.get_value()].get_declare() ) {
		err = "\033[1;31mTwice declare: \033[0m";
		string s = TID[c_lex.get_value()].get_name();
		err+=s;
		throw err;
	}
	else {
		TID[c_lex.get_value()].put_declare();
		TID[c_lex.get_value()].put_type(type);
		if (znach){
			TID[c_lex.get_value()].put_assign();
			TID[c_lex.get_value()].put_value(val);
		}
	}
	
}

void Parser::check_id () {
	if ( TID[c_val].get_declare() )
		st_lex.push (TID[c_val].get_type());
	else {
		err = "\033[1;31mNot declared: \033[0m";
		string s = TID[c_val].get_name();
		err+=s;
		throw err;
	}
}

void Parser::check_op () {
	type_of_lex t1, t2, op, t = LEX_INT, r = LEX_BOOL, s = LEX_STRING;
 
	t2 = st_lex.top();
	st_lex.pop();
	op = st_lex.top();
	st_lex.pop();
	t1 = st_lex.top();
	st_lex.pop();
	
	if ((op == LEX_PLUS || op == LEX_MINUS || op == LEX_TIMES || op == LEX_SLASH || op == LEX_ASSIGN) && (t1 == t && t2 == t))
		st_lex.push(t);	
	else if (( op == LEX_EQ || op == LEX_LSS || op == LEX_GTR || op == LEX_LEQ || op == LEX_GEQ || op == LEX_NEQ) && t1 == t && t2 == t)
		st_lex.push(r);	
	else if ((op == LEX_PLUS || op == LEX_ASSIGN) && t1 == s && t2 == s)
		st_lex.push(s);
	else if (( op == LEX_EQ || op == LEX_LSS || op == LEX_GTR || op == LEX_LEQ || op == LEX_GEQ || op == LEX_NEQ) && t1 == s && t2 == s)
		st_lex.push(r);	
	else if ((op == LEX_OR || op == LEX_AND || op == LEX_ASSIGN) && (t1 == r && t2 == r))
		st_lex.push(r);
	else if (( op == LEX_EQ || op == LEX_NEQ) && t1 == r && t2 == r)
		st_lex.push(r);	
	else{
		err = "\033[1;31mWrong types are in operation: \033[0m";
		string s = Scanner::TD[op-17];
		err+=s;
		throw err;
	}
	
	poliz.push_back(Lex(op));
}

void Parser::check_not () {
	if (st_lex.top() != LEX_BOOL)
		throw "\033[1;31mWrong type in NOT \033[0m";
		
	poliz.push_back(Lex(LEX_NOT));
}

void Parser::check_int () {
	if (st_lex.top() != LEX_INT)
		throw "\033[1;31mWrong type in MINUS \033[0m";
		
	poliz.push_back(Lex(LEX_UMIN));
}

void Parser::eq_bool () {
	if ( st_lex.top() != LEX_BOOL )
		throw "\033[1;31mExpression is not boolean\033[0m";
	st_lex.pop();
}
 
void Parser::check_id_in_read () {
	if ( !TID [c_val].get_declare() ){
		err = "\033[1;31mNot declared: \033[0m";
		string s = TID[c_val].get_name();
		err+=s;
		throw err;
	}
}

class Executer {
	Lex pc_el;
public:
	void execute ( vector <Lex> & poliz );
};

void Executer::execute ( vector <Lex> & poliz){
	stack <Lex> args;
	int i, j, k, index = 0, size = poliz.size();
	Lex crr,prr;
	char ss[128];
	
	cout << "\033[7;32m Start of executing \033[0m" << endl;
	
	while ( index < size ) {
		
		pc_el = poliz [ index ];
 
		switch ( pc_el.get_type () ) {
 
			case LEX_TRUE: case LEX_FALSE: case LEX_STR: case LEX_NUM: case POLIZ_ADDRESS: case POLIZ_LABEL:
				args.push ( Lex(pc_el.get_type(),pc_el.get_value ()) );
				break;
 
			case LEX_ID:
				i = pc_el.get_value ();
				if ( TID[i].get_assign () ) {
					args.push ( Lex(TID[i].get_type(),TID[i].get_value ()) );
					break;
				}	
				else
					throw "\033[1;31mPOLIZ: Indefinite Identifier\033[0m";
			
			case LEX_UMIN:
				crr = args.top();
				args.pop();
				args.push( Lex(LEX_NUM,-crr.get_value()) );
				break;
			
			case LEX_NOT:
				crr = args.top();
				args.pop();
				args.push( Lex(LEX_BOOL,!(crr.get_value())) );
				break;
 
			case LEX_OR:
				crr = args.top();
				args.pop();
				prr = args.top();
				args.pop();
				args.push ( Lex(LEX_BOOL, prr.get_value() || crr.get_value()) );
				break;
 
			case LEX_AND:
				crr = args.top();
				args.pop();
				prr = args.top();
				args.pop();
				args.push ( Lex(LEX_BOOL, prr.get_value() && crr.get_value()) );
				break;
 
			case POLIZ_GO:
				i = args.top().get_value();
				args.pop();
				index = i - 1;
				break;
 
			case POLIZ_FGO:
				i = args.top().get_value();
				args.pop();
				j = args.top().get_value();
				args.pop();
				if ( !j ) index = i-1;
				break;
 
			case LEX_WRITE:
				crr = args.top();
				args.pop();
				if(crr.get_type() == LEX_STR || crr.get_type() == LEX_STRING)
					cout << Tstr[crr.get_value()] << endl;
				else if (crr.get_type() == LEX_BOOL || crr.get_type() == LEX_FALSE || crr.get_type() == LEX_TRUE)
					cout << (1==crr.get_value()) << endl;
				else 
					cout << crr.get_value() << endl;
				break;
 
			case LEX_READ:
				i = args.top ().get_value();
				args.pop();
				if ( TID[i].get_type() == LEX_INT ) {
					cout << "\033[1;33mInput INT value for: \033[0m" << TID[i].get_name () << endl;
					cin >> k;
					TID[i].put_value (k);
					TID[i].put_assign ();
				}
				else if (TID[i].get_type() == LEX_STRING ){
					cout << "\033[1;33mInput STRING value for: \033[0m" << TID[i].get_name () << endl;
					scanf("%s",ss);
					k = Tstr.put(ss);
					TID[i].put_value (k);
					TID[i].put_assign ();
				} 
				else {
					string j;
					while (1){
						cout << "\033[1;33mInput BOOL value (true or false) for: \033[0m" << TID[i].get_name() << endl;
						cin >> j;
						if (j != "true" && j != "false"){
							cout << "\033[1;31mError in input: true/false\n\033[0m" << endl;
							continue;
						}
						k = (j == "true")? 1 : 0 ;
						TID[i].put_value (k);
						TID[i].put_assign ();
						break;
					}
				}
				break;
 
			case LEX_PLUS:
				
				crr = args.top();
				args.pop();
				prr = args.top();
				args.pop();
				if(crr.get_type() == LEX_STR || crr.get_type() == LEX_STRING){
					sprintf(ss,"%s%s",Tstr[prr.get_value()],Tstr[crr.get_value()]);
					k = Tstr.put(ss);
					args.push ( Lex(LEX_STR,k) );
				}
				else
					args.push ( Lex(LEX_NUM,prr.get_value() + crr.get_value()) );
				break;
 
			case LEX_TIMES:
				i = args.top().get_value();
				args.pop();
				j = args.top().get_value();
				args.pop();
				args.push ( Lex(LEX_NUM, i * j) );
				break;
 
			case LEX_MINUS:
				i = args.top().get_value();
				args.pop();
				j = args.top().get_value();
				args.pop();
				args.push ( Lex(LEX_NUM, j - i) );
				break;
 
			case LEX_SLASH:
				i = args.top().get_value();
				args.pop();
				j = args.top().get_value();
				args.pop();
				if (i) {
					args.push ( Lex(LEX_NUM, j / i) );
					break;
				}
				else
					throw "\033[1;31mPOLIZ: Divide by zero\033[0m";
 
			case LEX_EQ:
				crr = args.top();
				args.pop();
				prr = args.top();
				args.pop();
				if(crr.get_type() == LEX_STR || crr.get_type() == LEX_STRING)
					args.push ( Lex(LEX_BOOL, strcmp(Tstr[crr.get_value()],Tstr[prr.get_value()])==0?1:0) );
				else
					args.push ( Lex(LEX_BOOL,(prr.get_value() == crr.get_value())?1:0) );
				break;
 
			case LEX_LSS:
				crr = args.top();
				args.pop();
				prr = args.top();
				args.pop();
				if(crr.get_type() == LEX_STR || crr.get_type() == LEX_STRING)
					args.push ( Lex(LEX_BOOL, strcmp(Tstr[crr.get_value()],Tstr[prr.get_value()])<0?1:0) );
				else
					args.push ( Lex(LEX_BOOL,(prr.get_value() < crr.get_value())?1:0) );
				break;
 
			case LEX_GTR:
				crr = args.top();
				args.pop();
				prr = args.top();
				args.pop();
				if(crr.get_type() == LEX_STR || crr.get_type() == LEX_STRING)
					args.push ( Lex(LEX_BOOL, strcmp(Tstr[crr.get_value()],Tstr[prr.get_value()])>0?1:0) );
				else
					args.push ( Lex(LEX_BOOL,(prr.get_value() > crr.get_value())?1:0) );
				break;
 
			case LEX_LEQ:
				crr = args.top();
				args.pop();
				prr = args.top();
				args.pop();
				if(crr.get_type() == LEX_STR || crr.get_type() == LEX_STRING)
					args.push ( Lex(LEX_BOOL, strcmp(Tstr[crr.get_value()],Tstr[prr.get_value()])<=0?1:0) );
				else
					args.push ( Lex(LEX_BOOL,(prr.get_value() <= crr.get_value())?1:0) );
				break;
		 
			case LEX_GEQ:
				crr = args.top();
				args.pop();
				prr = args.top();
				args.pop();
				if(crr.get_type() == LEX_STR || crr.get_type() == LEX_STRING)
					args.push ( Lex(LEX_BOOL, strcmp(Tstr[crr.get_value()],Tstr[prr.get_value()])>=0?1:0) );
				else
					args.push ( Lex(LEX_BOOL,(prr.get_value() >= crr.get_value())?1:0) );
				break;
		 
			case LEX_NEQ:
				crr = args.top();
				args.pop();
				prr = args.top();
				args.pop();
				if(crr.get_type() == LEX_STR || crr.get_type() == LEX_STRING)
					args.push ( Lex(LEX_BOOL, strcmp(Tstr[crr.get_value()],Tstr[prr.get_value()])==0?0:1) );
				else
					args.push ( Lex(LEX_BOOL,(prr.get_value() == crr.get_value())?0:1) );
				break;
 
			case LEX_ASSIGN:
				crr = args.top();
				args.pop();
				prr = args.top();
				args.pop();
					TID[prr.get_value()].put_assign(); 
					TID[prr.get_value()].put_value(crr.get_value());
				break;
 
			default:
				throw "\033[1;31mPOLIZ: Unexpected element\033[0m";
		}//end of switch
		
		++index;
	};//end of while
	
	cout << "\033[7;32m Finish of executing \033[0m" << endl;
}

class Interpretator {
	Parser   pars;
	Executer E;
public:
	Interpretator  (const char* program): pars (program) {}
	void interpretation ();
};
 
void Interpretator::interpretation () {
	pars.analyze ();
	E.execute ( pars.poliz );
}

int main(int argc, char** argv){
	
	if (argc != 2){
		cout << "\033[1;31mError of parametrs!\033[0m" << endl;
		return 1;
	}
	
	try{
		
		Interpretator PROG(argv[1]);
		
		PROG.interpretation();
		
	}
	catch (char c) {
		cout << "\033[1;31mUnexpected symbol \"\033[0m" << c <<"\" \033[1;31min " << ns << " line\033[0m" << endl;
		return 1;
	}
	catch (Lex c) {
		cout << "\033[1;31mUnexpected lexem \033[0m" << c << " \033[1;31min " << ns << " line\033[0m" << endl;
		return 1;
	}
	catch(string & s){
		cout << s << endl;
	}
	catch(const char * s){
		cout << s << endl;
	}
	catch(...){
		cout << "\033[1;31mError!\033[0m" << endl;
	}
	
	return 0;
}
