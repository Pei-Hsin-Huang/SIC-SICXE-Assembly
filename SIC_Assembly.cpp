// 10827145 ¶Àè®©ý
// ¨Ï¥ÎDev-C++½sÄ¶
#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <string>
#include <cstdlib>
#include <ctype.h>
#include <algorithm>
#define WORD 11
#define BYTE 28

using namespace std;

typedef char Str100[100] ;
typedef Str100 Table[100];

struct Token {
	Str100 item;
	int table;
	int num;
	Str100 com;
	Token *next;
};

typedef struct Symbol {
	Str100 symbol;
	int addr;
} SymbolType;

typedef Token * TokenPtr ;

struct HeadToken {
	TokenPtr token;
	HeadToken *nextHead;
};

typedef struct Line {
	int line;
	int loc;
	int format;
	Str100 objcode;
	bool havecode;
	bool error;
	bool base;
	bool equ;
	bool com;
	bool end;
	bool noLoc;
	bool literal;
	bool extraLiteral;
	int litNum;
	Str100 errorM; 
	Str100 comM;
} LineType;

typedef struct Literal {
	Str100 name;
	int addr;
	int type;
	int num;
} LiteralType;

typedef HeadToken * HeadTokenPtr;

HeadTokenPtr ghead = NULL; // First Token 
HeadTokenPtr grear = NULL; // First Token of Last line

class SIC {
	public:
		SymbolType symtab[100];
		int empty[256];
		int e;
		int curLine;
		int curLoc;
		int index;
		int curIndex;
		string outFileName;
		TokenPtr cur;
		vector <LineType> line;
		void ClearSymTab();
		bool InsertSymbol( Str100 str, int addr, int num );
		int GetSymbolAddr( int num );
		void Type1( LineType & temp, TokenPtr t );
		void Pseudo( LineType & temp, TokenPtr t );
		void CheckEqu( LineType & temp, TokenPtr t );
		void Equ( LineType & temp, TokenPtr t );
		void RealCode( LineType & temp, TokenPtr t );
		void CreatLine( LineType & temp );
		void Allocate( LineType & temp, TokenPtr nextToken );
		void FirstLine();
		void Pass1( HeadTokenPtr curHead );
		int GetOp( Str100 s );
		void FillObjcode( TokenPtr t );
		void EquPass2( TokenPtr t );
		void LinePass2();
		void Pass2();
		void SICMain();
		void ClearOutFile();
		void OutPutLine( LineType l );
};

class SICXE {
	public:
		SymbolType symtab[100];
		LiteralType literTab[100];
		vector <LiteralType> literBuf;
		int newLiter;
		int empty[256];
		int e;
		int curLine;
		int curLoc;
		int index;
		int curIndex;
		int baseReg;
		TokenPtr cur;
		string outFileName;
		vector <LineType> line;
		void ClearOutFile();
		void OutPutLine( LineType l );
		void ClearSymTab();
		bool InsertSymbol( Str100 str, int addr, int num );
		int GetSymbolAddr( int num );
		void Type1( LineType & temp, TokenPtr t );
		void Type2( LineType & temp, TokenPtr t );
		int InsertLiteral( Str100 str, int i, int & num );
		void Literal( LineType & temp, TokenPtr t );
		void RealCode( LineType & temp, TokenPtr t );
		int GetFormat( Str100 s );
		void CheckFormat2( LineType & temp, TokenPtr t );
		void Pseudo( LineType & temp, TokenPtr t );
		void CheckEqu( LineType & temp, TokenPtr t );
		void Equ( LineType & temp, TokenPtr t );
		void CreatLiteral( LineType & temp );
		void CreatLine( LineType & temp );
		void Allocate( LineType & temp, TokenPtr nextToken );
		void FirstLine();
		void Pass1( HeadTokenPtr curHead );
		void SICXEMain();
		int GetOp( Str100 s );
		int GetR( Str100 r );
		void Format3Obj( TokenPtr t );
		void Format4Obj( TokenPtr t );
		void FillObjcode( TokenPtr t );
		void LinePass2();
		void FillBase( TokenPtr t );
		void EquPass2( TokenPtr t );
		void Pass2();
		void FillExtraLiteral();
		void FillExtraObj( LineType & temp );
};

int FindTable( Str100 str, Table table ) {
	int hash = 0;
	int sum = 0;
	int i = 0;
	while ( str[i] != '\0' ) {
		sum = sum + str[i];
		i++;
	} // end while
	
	hash = sum % 100;
	if ( table[hash][0] == '\0'  ) {
		strcpy( table[hash], str );
		return hash;
	} // end if
	
	else {
		if ( strcmp( table[hash], str ) == 0 ) {
			return hash;
		} // end if
		
		else {
			hash++;
			int count = 0;
			while ( table[hash][0] != '\0' ) {
				if ( strcmp( table[hash], str ) == 0 ) {
			        return hash;
		        } // end if
		        
		        if ( hash == 99 ) {
		        	hash = 0;
				} // end if
				
				else {
					hash++;
				} // end else
				count++;
				if ( count > 100 ) {
					printf( "### table full ###\n" );
					return -1;
				} // end if
			} // end while
			
			strcpy( table[hash], str );
			return hash;
		} // end else
	} // end else
	
} // FindTable()

bool IsNum( Str100 temp ) {
	int i = 0;
	bool n = true;
	while ( temp[i] != '\0' ) {
		if ( ( temp[i] < '0' ) || ( temp[i] > '9' ) ) {
			n = false;
		} // end if
		i++;
	} // end while
	
	return n;
} // IsNum()

void DealInt( Str100 temp, TokenPtr & curToken, Table table6 ) {
	int n = FindTable( temp, table6 );
	TokenPtr newToken = new Token;
	strcpy( newToken->item, temp );
	newToken->table = 6;
	newToken->num = n;
	newToken->next = NULL;
	
	if ( curToken == NULL ) {
		HeadTokenPtr newhead = new HeadToken;
		newhead->token = newToken;
		newhead->nextHead = NULL;
		curToken = newToken;
		
		if ( ghead == NULL ) {
			ghead = newhead;
			grear = newhead;
		} // end if
		
		else {
			grear->nextHead = newhead;
			grear = grear->nextHead;
		} // end else
	} // end if
	
	else {
		curToken->next = newToken;
		curToken = curToken->next;
	} // end else
	
} // DealInt()

int SearchTable( FILE *tableFile, Str100 str ) {
	Str100 t;
	int i = 1;

	while ( !feof( tableFile ) ) {
		fscanf( tableFile, "%s", t );
		if ( strcmp( str, t ) == 0 ) {
			return i;
		} // end if
		
		i++;
	} // end while
	
	return -1;
} // SearchTable

void DealStr( Str100 temp, TokenPtr & curToken, Table table5 ) {
	FILE *tableFile = NULL;
	TokenPtr newToken = new Token;
	Str100 upper,lower, orig;
	int n = -1;
	strcpy( orig, temp );
	strcpy( upper, strupr(temp) );
	strcpy( lower, strlwr(temp) );
	strcpy( newToken->item, orig );
	newToken->next = NULL;
	
	tableFile = fopen("Table1.table","r");
	n = SearchTable( tableFile, lower );
	fclose(tableFile);
	if ( n != -1 ) { // Instruction
		newToken->table = 1;
		newToken->num = n;
	} // end if
	
	else {
		tableFile = fopen("Table2.table","r");
		n = SearchTable( tableFile, upper );
		fclose(tableFile);
		if ( n != -1 ) { // Pseudo and Extra
			newToken->table = 2;
			newToken->num = n;
		} // end if
		
		else {
			tableFile = fopen("Table3.table","r");
			n = SearchTable( tableFile, upper );
			fclose(tableFile);
			if ( n != -1 ) { // Register
				newToken->table = 3;
				newToken->num = n;
			} // end if
			
			else { // Symbol
				n = FindTable( orig, table5 );
				newToken->table = 5;
				newToken->num = n;
			} // end else
		} // end else
	} // end else
	
	if ( curToken == NULL ) {
		HeadTokenPtr newhead = new HeadToken;
		newhead->token = newToken;
		newhead->nextHead = NULL;
		curToken = newToken;
		
		if ( ghead == NULL ) {
			ghead = newhead;
			grear = newhead;
		} // end if
		
		else {
			grear->nextHead = newhead;
			grear = grear->nextHead;
		} // end else
	} // end if
	
	else {
		curToken->next = newToken;
		curToken = curToken->next;
	} // end else
} // DealStr()

void DealDelimiter( FILE *infile, FILE *outfile, char & ch, Str100 temp, TokenPtr & curToken, Table table5, Table table6, Table table7, int s ) {
	FILE *tableFile = NULL;
	TokenPtr newToken = new Token;
	tableFile = fopen("Table4.table","r");
	int n = 1;
	Str100 t;
	bool find = false;
	
	while ( !feof(tableFile) ) {
		fscanf( tableFile, "%s", &t );
		if ( ch == t[0] ) {
			find = true;
			newToken->item[0] = ch;
			newToken->item[1] = '\0';
			newToken->table = 4;
			newToken->num = n;
			newToken->next = NULL;
		} // end if
		
		n++;
	} // end while
	
	if ( find == false ) {
		printf( "### unexcept delimiter %c ###\n", ch );
		fscanf( infile, "%c", &ch );
		return;
	} // end if
	
	fclose(tableFile);
	
	if ( ch == '.' ) { // skip comment
		fscanf( infile, "%c", &ch ); 
		int e = 0;
		while ( ( ch != '\n' ) && ( !feof(infile) ) ) {
			newToken->com[e] = ch;
			fprintf( outfile, "%c", ch );
			fscanf( infile, "%c", &ch );
			e++;
		} // end while
		newToken->com[e] = '\0';
		
		if ( s > 0 ) {
			temp[s] = '\0';
			if ( IsNum( temp ) ) {
				DealInt( temp, curToken, table6 );
			} // end if
			
			else {
				DealStr( temp, curToken, table5 );
			} // end else
		} // end if
	} // end if
	
	else if ( ch == '\'' ) {
		int index = 0;
		Str100 str;
		if ( ( temp[0] == 'c' ) || ( temp[0] == 'C' ) ) {
			temp[0] = 'C';
			fscanf( infile, "%c", &ch );
			while ( ( ch != '\'' ) && ( !feof(infile) ) && ( ch != '\n' ) ) {
				fprintf( outfile, "%c", ch );
				str[index] = ch;
				temp[index+1] = ch;
				fscanf( infile, "%c", &ch );
				index++;
			} // end while
			
			// deal '
			fprintf( outfile, "%c", ch );
			TokenPtr tempToken = new Token;
			strcpy( tempToken->item, newToken->item );
			tempToken->table = newToken->table;
			tempToken->num = newToken->num;
			tempToken->next = NULL;
			
			if ( curToken == NULL ) {
		        HeadTokenPtr newhead = new HeadToken;
		        newhead->token = tempToken;
		        newhead->nextHead = NULL;
		        curToken = tempToken;
		
	        	if ( ghead == NULL ) {
			        ghead = newhead;
			        grear = newhead;
		        } // end if
		
		        else {
			       grear->nextHead = newhead;
			       grear = grear->nextHead;
		        } // end else
	        } // end if
	
	        else {
		        curToken->next = tempToken;
	        	curToken = curToken->next;
	        } // end else
			
			// deal String
			str[index] = '\0';
			temp[index+1] = '\0';
			int l = FindTable( str, table7 );
			TokenPtr strToken = new Token;
			strcpy( strToken->item, temp );
			strToken->table = 7;
			strToken->num = l;
			strToken->next = NULL;
			
			curToken->next = strToken;
			curToken = curToken->next;
			
			fscanf( infile, "%c", &ch );
		} // end if
		
		else if ( ( temp[0] == 'x' ) || ( temp[0] == 'X' ) ) {
			temp[0] = 'X';
			fscanf( infile, "%c", &ch );
			while ( ( ch != '\'' ) && ( !feof(infile) ) && ( ch != '\n' ) ) {
				fprintf( outfile, "%c", ch );
				str[index] = ch;
				temp[index+1] = ch;
				fscanf( infile, "%c", &ch );
				index++;
			} // end while
			
			// deal '
			fprintf( outfile, "%c", ch );
			TokenPtr tempToken = new Token;
			strcpy( tempToken->item, newToken->item );
			tempToken->table = newToken->table;
			tempToken->num = newToken->num;
			tempToken->next = NULL;
			
			if ( curToken == NULL ) {
		        HeadTokenPtr newhead = new HeadToken;
		        newhead->token = tempToken;
		        newhead->nextHead = NULL;
		        curToken = tempToken;
		
	        	if ( ghead == NULL ) {
			        ghead = newhead;
			        grear = newhead;
		        } // end if
		
		        else {
			       grear->nextHead = newhead;
			       grear = grear->nextHead;
		        } // end else
	        } // end if
	
	        else {
		        curToken->next = tempToken;
	        	curToken = curToken->next;
	        } // end else
			
			// deal Int
			str[index] = '\0';
			temp[index+1] = '\0';
			int l = FindTable( str, table6 );
			TokenPtr strToken = new Token;
			strcpy( strToken->item, temp );
			strToken->table = 6;
			strToken->num = l;
			strToken->next = NULL;
			
			curToken->next = strToken;
			curToken = curToken->next;
			
			fscanf( infile, "%c", &ch );
		} // end else
		
		else {
			fscanf( infile, "%c", &ch );
		} // end else
		
	} // end else if
	
	else {
		if ( s > 0 ) {
			temp[s] = '\0';
			if ( IsNum( temp ) ) {
				DealInt( temp, curToken, table6 );
			} // end if
			
			else {
				DealStr( temp, curToken, table5 );
			} // end else
		} // end if
		
		fscanf( infile, "%c", &ch );
	} // end else
	
	if ( curToken == NULL ) {
		HeadTokenPtr newhead = new HeadToken;
		newhead->token = newToken;
		newhead->nextHead = NULL;
		curToken = newToken;
		if ( ghead == NULL ) {
			ghead = newhead;
			grear = newhead;
		} // end if
		
		else {
			grear->nextHead = newhead;
			grear = grear->nextHead;
		} // end else
	} // end if
	
	else {
		curToken->next = newToken;
		curToken = curToken->next;
	} // end else
} // DealDelimiter()

bool IsChNum( char ch ) {
	if ( ( ch >= 'a' ) && ( ch <= 'z' ) ) {
		return true;
	} // end if
	
	else if ( ( ch >= 'A' ) && ( ch <= 'Z' ) ) {
		return true;
	} // end else if
	
	else if ( ( ch >= '0' ) && ( ch <= '9' ) ) {
		return true;
	} // end else if
	
	else {
		return false;
	} // end else
} // IsChNum()

void PrintTokenInfo( FILE *outfile, TokenPtr curToken ) {
	if ( ( grear != NULL ) && ( curToken != NULL ) ) {
		TokenPtr temp = grear->token;
		fprintf( outfile, "\n" );
		
		while ( temp != NULL ) {
			fprintf( outfile, "(%d,%d)", temp->table, temp->num );
			temp = temp->next;
		} // end while
		
	} // end if
	
} // PrintTokenInfo()

void DealALine( FILE *infile, string outFileName, Table table5, Table table6, Table table7 ) {
	char ch = '\0';
	FILE *outfile = NULL;
	Str100 temp;
	TokenPtr curToken = NULL;
	outfile = fopen(outFileName.c_str(),"a");
	
	fscanf( infile, "%c", &ch );
	if ( ghead != NULL ) {
		fprintf( outfile, "\n" );
	} // end if
	
	while ( ( ( ch == ' ' ) || ( ch == '\t' ) ) && ( !feof(infile) ) ) { // get first char
		fprintf( outfile, "%c", ch );
		fscanf( infile, "%c", &ch );
	} // end while
	
	while ( ( ch != '\n' ) && ( !feof(infile) ) ) {
		int i = 0;
		while ( ( IsChNum( ch ) == true ) && ( !feof(infile) ) ) { // read until white or delimiter
		    temp[i] = ch;
			fprintf( outfile, "%c", ch );
			fscanf( infile, "%c", &ch );
			i++;
		} // end while
		
		if ( ( ch != ' ' ) && ( ch != '\t' ) && ( ch != '\n' ) && ( !feof(infile) ) ) {
			temp[i] = '\0';
			fprintf( outfile, "%c", ch );
			DealDelimiter( infile, outfile, ch, temp, curToken, table5, table6, table7, i );
			while ( ( ( ch == ' ' ) || ( ch == '\t' ) ) && ( !feof(infile) ) ) {
				fprintf( outfile, "%c", ch );
				fscanf( infile, "%c", &ch );
			} // end while
			
		} // end if
		
		else {
			temp[i] = '\0';
			while ( ( ( ch == ' ' ) || ( ch == '\t' ) ) && ( !feof(infile) ) ) { // skip white
				fprintf( outfile, "%c", ch );
				fscanf( infile, "%c", &ch );
			} // end while
			
			if ( IsNum( temp ) ) {
				DealInt( temp, curToken, table6 );
			} // end if
			
			else {
				DealStr( temp, curToken, table5 );
			} // end else
			
		} // end else
		
	} // end while
	
	PrintTokenInfo( outfile, curToken );
	fclose(outfile);
} // DealALine()

void DealToken( FILE *infile, Table table5, Table table6, Table table7 ) {
	FILE *outfile = NULL;
	string outFileName;
	
	if ( !feof(infile) ) { // clear output file
	    //printf( "\nInput the output token file name (e.g., TokenOutput.txt ...):" );
	    outFileName = "TokenOutput.txt";
	    outfile = fopen(outFileName.c_str(),"w");
	    if ( outfile == NULL ) {
	    	cout << endl << "### open " << outFileName << " failed ###" << endl;
		} // end if
	    
	    fclose(outfile);
	} // end if
	
	else {
		cout << endl << "### input file is empty ###" << endl;
	} // end else
	
	while ( !feof(infile) ) {
	    DealALine( infile, outFileName, table5, table6, table7 );
	    
    } // end while
	
	fclose(infile);
} // DealToken

void ClearTable( Table table ) {
	for ( int i = 0; i < 100; i++ ) {
		table[i][0] = '\0';
	} // end for
	
} // ClearTable()

void PrintTable( string t, Table table ) {
	FILE *tableFile = NULL;
	string name = "Table" + t + ".table";
	tableFile = fopen( name.c_str(),"w" );
	int i = 0;
	while ( i < 100 ) {
		if ( table[i][0] != '\0' ) {
			fprintf( tableFile, "%s", table[i] );
		} // end if
		
		fprintf( tableFile, "\n" );
		i++;
	} // end while
	
	fclose(tableFile);
} // PrintTable

void GetToken( string & fileName ) {
	FILE *infile = NULL;
	ghead = NULL;
	grear = NULL;
	Table table5, table6, table7;
	ClearTable( table5 );
	ClearTable( table6 );
	ClearTable( table7 );
	printf( "\nInput a file name (e.g., input.txt ...):" );
	cin >> fileName;
	infile = fopen(fileName.c_str(),"r");
	
	if ( infile != NULL ) {
		DealToken( infile, table5, table6, table7 );
		PrintTable( "5", table5 );
		PrintTable( "6", table6 );
		PrintTable( "7", table7 );
		printf( "Token output at TokenOutput.txt\n" );
	} // end if
	
	else
	    cout << endl << "### " << fileName << " does not exist! ###" << endl;
	    
} // GetToken()

int D_To_H( Str100 d ) {
	int h = 0x0;
	int t;
	sscanf( d, "%d", &t );
	h = t;
	return h;
} // D_To_H()

void SIC::ClearSymTab() {
	for ( int i = 0; i < 100; i++ ) {
		symtab[i].symbol[0] = '\0';
	} // end for
} // ClearSymTab()

bool SIC::InsertSymbol( Str100 str, int addr, int num ) {
	if ( symtab[num].symbol[0] == '\0' ) {
		strcpy( symtab[num].symbol, str );
		symtab[num].addr = addr;
		return true;
	} // end if
	
	else {
		return false;
	} // end else
} // InsertSymbol()

int SIC::GetSymbolAddr( int num ) {
	if ( symtab[num].symbol[0] != '\0' ) {
		return symtab[num].addr;
	} // end if
	
	else {
		return -1;
	} // end else
} // GetSymbolAddr()

void SIC::Type1( LineType & temp, TokenPtr t ) {
	TokenPtr tNext = t->next;
	if ( tNext != NULL ) {
		if ( tNext->table == 5 ) {
			if ( tNext->next != NULL ) {
				if ( strcmp( tNext->next->item, "," ) == 0 ) {
					if ( tNext->next->next != NULL ) {
						Str100 u;
						strcpy( u , tNext->next->next->item );
						strupr( u );
						if ( strcmp( u, "X" ) == 0 ) {
							if ( tNext->next->next->next == NULL ) {
								temp.error = false;
		                        temp.havecode = true;
	        	                temp.line = curLine;
		                        temp.loc = curLoc;
		                        temp.base = true;
		                        curLoc = curLoc + 0x3;
							} // end if
							
							else if ( strcmp( tNext->next->next->next->item, "." ) == 0 ) {
								temp.error = false;
		                        temp.havecode = true;
	        	                temp.line = curLine;
		                        temp.loc = curLoc;
		                        temp.base = true;
		                        curLoc = curLoc + 0x3;
							} // end else if
							
							else {
								temp.error = true;
		                        temp.line = curLine;
		                        sprintf( temp.errorM, "syntax error" );
		                        printf( "%d: syntax error\n", temp.line );
							} // end else
						} // end if
						
						else {
							temp.error = true;
		                    temp.line = curLine;
		                    sprintf( temp.errorM, "syntax error" );
		                    printf( "%d: syntax error\n", temp.line );
						} // end else
					} // end if
					
					else {
						temp.error = true;
		                temp.line = curLine;
		                sprintf( temp.errorM, "syntax error" );
		                printf( "%d: syntax error\n", temp.line );
					} // end else
				} // end if
				
				else if ( strcmp( tNext->next->item, "." ) == 0 ) {
					temp.error = false;
		            temp.havecode = true;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            curLoc = curLoc + 0x3;
				} // end else if
				
				else {
					temp.error = true;
		            temp.line = curLine;
		            sprintf( temp.errorM, "syntax error" );
		            printf( "%d: syntax error\n", temp.line );
				} // end else
				
			} // end if
			
			else {
				temp.error = false;
		        temp.havecode = true;
	        	temp.line = curLine;
		        temp.loc = curLoc;
		        temp.base = false;
		        curLoc = curLoc + 0x3;
			} // end else
			
		} // end if
		
		else {
			temp.error = true;
		    temp.line = curLine;
		    sprintf( temp.errorM, "syntax error" );
		    printf( "%d: syntax error\n", temp.line );
		} // end else
	} // end if
	
	else { // syntax error
		temp.error = true;
		temp.line = curLine;
		sprintf( temp.errorM, "syntax error" );
		printf( "%d: syntax error\n", temp.line );
	} // end else
} // Type1()

void SIC::Pseudo( LineType & temp, TokenPtr t ) {
	Str100 u;
	strcpy( u, t->item );
	strupr( u );
	if ( strcmp( u, "BYTE" ) == 0 ) {
		TokenPtr tNext = t->next;
		if ( tNext != NULL ) {
			if ( strcmp( tNext->item, "\'" ) == 0 ) {
				if ( tNext->next != NULL ) {
					if ( tNext->next->table == 6 ) { // X
						if ( tNext->next->next != NULL ) {
						    if ( strcmp( tNext->next->next->item, "\'" ) == 0 ) {
						    	if ( tNext->next->next->next == NULL ) {
						    		temp.error = false;
		                            temp.havecode = true;
	        	                    temp.line = curLine;
		                            temp.loc = curLoc;
		                            temp.base = false;
		                            int length = strlen( tNext->next->item ) / 2;
		                            curLoc = curLoc + length;
								} // end if
								
								else if ( strcmp( tNext->next->next->next->item, "." ) == 0 ) {
									temp.error = false;
		                            temp.havecode = true;
	        	                    temp.line = curLine;
		                            temp.loc = curLoc;
		                            temp.base = false;
		                            int length = strlen( tNext->next->item ) / 2;
		                            curLoc = curLoc + length;
								} // end else if
								
								else {
									temp.error = true;
		                            temp.line = curLine;
		                            sprintf( temp.errorM, "syntax error" );
		                            printf( "%d: syntax error\n", temp.line );
								} // end else
							} // end if
							
							else {
								temp.error = true;
		                        temp.line = curLine;
		                        sprintf( temp.errorM, "syntax error" );
		                        printf( "%d: syntax error\n", temp.line );
							} // end else
					    } // end if
						
						else {
							temp.error = true;
		                    temp.line = curLine;
		                    sprintf( temp.errorM, "syntax error" );
		                    printf( "%d: syntax error\n", temp.line );
						} // end else
					} // end if
					
					else if ( tNext->next->table == 7 ) { // C
		                if ( tNext->next->next != NULL ) {
						    if ( strcmp( tNext->next->next->item, "\'" ) == 0 ) {
						    	if ( tNext->next->next->next == NULL ) {
						    		temp.error = false;
		                            temp.havecode = true;
	        	                    temp.line = curLine;
		                            temp.loc = curLoc;
		                            temp.base = false;
		                            int length = strlen( tNext->next->item ) - 1;
		                            curLoc = curLoc + length;
								} // end if
								
								else if ( strcmp( tNext->next->next->next->item, "." ) == 0 ) {
									temp.error = false;
		                            temp.havecode = true;
	        	                    temp.line = curLine;
		                            temp.loc = curLoc;
		                            temp.base = false;
		                            int length = strlen( tNext->next->item ) - 1;
		                            curLoc = curLoc + length;
								} // end else if
								
								else {
									temp.error = true;
		                            temp.line = curLine;
		                            sprintf( temp.errorM, "syntax error" );
		                            printf( "%d: syntax error\n", temp.line );
								} // end else
							} // end if
							
							else {
								temp.error = true;
		                        temp.line = curLine;
		                        sprintf( temp.errorM, "syntax error" );
		                        printf( "%d: syntax error\n", temp.line );
							} // end else
					    } // end if
						
						else {
							temp.error = true;
		                    temp.line = curLine;
		                    sprintf( temp.errorM, "syntax error" );
		                    printf( "%d: syntax error\n", temp.line );
						} // end else
					} // end else if
					
					else {
						temp.error = true;
		                temp.line = curLine;
		                sprintf( temp.errorM, "syntax error" );
		                printf( "%d: syntax error\n", temp.line );
					} // end else
					
				} // end if
				
				else {
					temp.error = true;
		            temp.line = curLine;
		            sprintf( temp.errorM, "syntax error" );
		            printf( "%d: syntax error\n", temp.line );
				} // end else
				
			} // end if
			
			else if ( tNext->table == 6 ) {
				if ( tNext->next == NULL ) {
					int h = 0x0;
					h = D_To_H( tNext->item );
					Str100 s;
					sprintf( s, "%x", h );
					int len = strlen( s ) / 2;
					temp.error = false;
		            temp.havecode = true;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            curLoc = curLoc + len;
				} // end if
				
				else if ( strcmp( tNext->next->item, "." ) == 0 ) {
					int h = 0x0;
					h = D_To_H( tNext->item );
					Str100 s;
					sprintf( s, "%x", h );
					int len = strlen( s ) / 2;
					temp.error = false;
		            temp.havecode = true;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            curLoc = curLoc + len;
				} // end else if
				
				else {
					temp.error = true;
		            temp.line = curLine;
		            sprintf( temp.errorM, "syntax error" );
		            printf( "%d: syntax error\n", temp.line );
				} // end else
			} // end else if
			
			else {
				temp.error = true;
		        temp.line = curLine;
		        sprintf( temp.errorM, "syntax error" );
		        printf( "%d: syntax error\n", temp.line );
			} // end else
		} // end if
		
		else {
			temp.error = true;
			temp.line = curLine;
			sprintf( temp.errorM, "syntax error" );
			printf( "%d: syntax error\n", temp.line );
		} // end else
		
	} // end if
	
	else if ( strcmp( u, "WORD" ) == 0 ) {
		TokenPtr tNext = t->next;
		if ( tNext != NULL ) {
			if ( strcmp( tNext->item, "\'" ) == 0 ) {
				if ( tNext->next != NULL ) {
					if ( tNext->next->table == 6 ) { // X
						if ( tNext->next->next != NULL ) {
						    if ( strcmp( tNext->next->next->item, "\'" ) == 0 ) {
						    	if ( tNext->next->next->next == NULL ) {
						    		temp.error = false;
		                            temp.havecode = true;
	        	                    temp.line = curLine;
		                            temp.loc = curLoc;
		                            temp.base = false;
		                            curLoc = curLoc + 3;
								} // end if
								
								else if ( strcmp( tNext->next->next->next->item, "." ) == 0 ) {
									temp.error = false;
		                            temp.havecode = true;
	        	                    temp.line = curLine;
		                            temp.loc = curLoc;
		                            temp.base = false;
		                            curLoc = curLoc + 3;
								} // end else if
								
								else {
									temp.error = true;
		                            temp.line = curLine;
		                            sprintf( temp.errorM, "syntax error" );
		                            printf( "%d: syntax error\n", temp.line );
								} // end else
							} // end if
							
							else {
								temp.error = true;
		                        temp.line = curLine;
		                        sprintf( temp.errorM, "syntax error" );
		                        printf( "%d: syntax error\n", temp.line );
							} // end else
					    } // end if
						
						else {
							temp.error = true;
		                    temp.line = curLine;
		                    sprintf( temp.errorM, "syntax error" );
		                    printf( "%d: syntax error\n", temp.line );
						} // end else
					} // end if
					
					else if ( tNext->next->table == 7 ) { // C
		                if ( tNext->next->next != NULL ) {
						    if ( strcmp( tNext->next->next->item, "\'" ) == 0 ) {
						    	if ( tNext->next->next->next == NULL ) {
						    		temp.error = false;
		                            temp.havecode = true;
	        	                    temp.line = curLine;
		                            temp.loc = curLoc;
		                            temp.base = false;
		                            curLoc = curLoc + 3;
								} // end if
								
								else if ( strcmp( tNext->next->next->next->item, "." ) == 0 ) {
									temp.error = false;
		                            temp.havecode = true;
	        	                    temp.line = curLine;
		                            temp.loc = curLoc;
		                            temp.base = false;
		                            curLoc = curLoc + 3;
								} // end else if
								
								else {
									temp.error = true;
		                            temp.line = curLine;
		                            sprintf( temp.errorM, "syntax error" );
		                            printf( "%d: syntax error\n", temp.line );
								} // end else
							} // end if
							
							else {
								temp.error = true;
		                        temp.line = curLine;
		                        sprintf( temp.errorM, "syntax error" );
		                        printf( "%d: syntax error\n", temp.line );
							} // end else
					    } // end if
						
						else {
							temp.error = true;
		                    temp.line = curLine;
		                    sprintf( temp.errorM, "syntax error" );
		                    printf( "%d: syntax error\n", temp.line );
						} // end else
					} // end else if
					
					else {
						temp.error = true;
		                temp.line = curLine;
		                sprintf( temp.errorM, "syntax error" );
		                printf( "%d: syntax error\n", temp.line );
					} // end else
					
				} // end if
				
				else {
					temp.error = true;
		            temp.line = curLine;
		            sprintf( temp.errorM, "syntax error" );
		            printf( "%d: syntax error\n", temp.line );
				} // end else
				
			} // end if
			
			else if ( tNext->table == 6 ) {
				if ( tNext->next == NULL ) {
					temp.error = false;
		            temp.havecode = true;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            curLoc = curLoc + 3;
				} // end if
				
				else if ( strcmp( tNext->next->item, "." ) == 0 ) {
					temp.error = false;
		            temp.havecode = true;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            curLoc = curLoc + 3;
				} // end else if
				
				else {
					temp.error = true;
		            temp.line = curLine;
		            sprintf( temp.errorM, "syntax error" );
		            printf( "%d: syntax error\n", temp.line );
				} // end else
			} // end else if
			
			else {
				temp.error = true;
		        temp.line = curLine;
		        sprintf( temp.errorM, "syntax error" );
		        printf( "%d: syntax error\n", temp.line );
			} // end else
		} // end if
		
		else {
			temp.error = true;
			temp.line = curLine;
			sprintf( temp.errorM, "syntax error" );
			printf( "%d: syntax error\n", temp.line );
		} // end else
	} // end else if
	
	else if ( strcmp( u, "RESW" ) == 0 ) {
		TokenPtr tNext = t->next;
		if ( tNext != NULL ) {
			if ( tNext->table == 6 ) {
				if ( tNext->next == NULL ) {
					temp.error = false;
		            temp.havecode = false;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            int h = 0x0;
		            h = D_To_H( tNext->item );
		            curLoc = curLoc + 3 * h;
				} // end if
				
				else if ( strcmp( tNext->next->item, "." ) == 0 ) {
					temp.error = false;
		            temp.havecode = false;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            int h = 0x0;
		            h = D_To_H( tNext->item );
		            curLoc = curLoc + 3 * h;
				} // end else if
				
				else {
					temp.error = true;
		            temp.line = curLine;
		            sprintf( temp.errorM, "syntax error" );
		            printf( "%d: syntax error\n", temp.line );
				} // end else
			} // end if
			
			else {
				temp.error = true;
		        temp.line = curLine;
		        sprintf( temp.errorM, "syntax error" );
		        printf( "%d: syntax error\n", temp.line );
			} // end else
			
		} // end if
		
		else {
			temp.error = true;
		    temp.line = curLine;
		    sprintf( temp.errorM, "syntax error" );
		    printf( "%d: syntax error\n", temp.line );
		} // end else
	} // end else if
	
	else if ( strcmp( u, "RESB" ) == 0 ) {
		TokenPtr tNext = t->next;
		if ( tNext != NULL ) {
			if ( tNext->table == 6 ) {
				if ( tNext->next == NULL ) {
					temp.error = false;
		            temp.havecode = false;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            int h = 0x0;
		            h = D_To_H( tNext->item );
		            curLoc = curLoc + h;
				} // end if
				
				else if ( strcmp( tNext->next->item, "." ) == 0 ) {
					temp.error = false;
		            temp.havecode = false;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            int h = 0x0;
		            h = D_To_H( tNext->item );
		            curLoc = curLoc + h;
				} // end else if
				
				else {
					temp.error = true;
		            temp.line = curLine;
		            sprintf( temp.errorM, "syntax error" );
		            printf( "%d: syntax error\n", temp.line );
				} // end else
			} // end if
			
			else {
				temp.error = true;
		        temp.line = curLine;
		        sprintf( temp.errorM, "syntax error" );
		        printf( "%d: syntax error\n", temp.line );
			} // end else
			
		} // end if
		
		else {
			temp.error = true;
		    temp.line = curLine;
		    sprintf( temp.errorM, "syntax error" );
		    printf( "%d: syntax error\n", temp.line );
		} // end else
	} // end else if
	
	else if ( strcmp( u, "END" ) == 0 ) {
		TokenPtr tNext = t->next;
		if ( tNext != NULL ) {
			if ( tNext->table == 5 ) {
				if ( tNext->next == NULL ) {
					temp.error = false;
		            temp.havecode = false;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            temp.end = true;
				} // end if
				
				else if ( strcmp( tNext->next->item, "." ) == 0 ) {
					temp.error = false;
		            temp.havecode = false;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            temp.end = true;
				} // end else if
				
				else {
					temp.error = true;
		            temp.line = curLine;
		            sprintf( temp.errorM, "syntax error" );
		            printf( "%d: syntax error\n", temp.line );
				} // end else
			} // end if
			
			else if ( strcmp( tNext->item, "." ) == 0 ) {
				temp.error = false;
		        temp.havecode = false;
	        	temp.line = curLine;
		        temp.loc = curLoc;
		        temp.base = false;
		        temp.end = true;
		    } // end else if
			
			else {
				temp.error = true;
		        temp.line = curLine;
		        sprintf( temp.errorM, "syntax error" );
		        printf( "%d: syntax error\n", temp.line );
			} // end else
		} // end if
		
		else {
			temp.error = false;
		    temp.havecode = false;
	        temp.line = curLine;
		    temp.loc = curLoc;
		    temp.base = false;
		    temp.end = true;
		} // end else
		
	} // end else if
	
	else {
		temp.error = true;
		temp.line = curLine;
		sprintf( temp.errorM, "error pseudo instr" );
		printf( "%d: error pseudo instr\n", temp.line );
	} // end else
} // Pseudo()

void SIC::CheckEqu( LineType & temp, TokenPtr t ) {
	bool label = false;
	bool error = false;
	bool stop = false;
	TokenPtr walk = t;
	while ( ( walk != NULL ) && ( error == false ) && ( stop == false ) ) {
		if ( label == false ) {
			if ( walk->table != 5 ) {
				error = true;
			} // end if
			
			else {
				label = true;
			} // end else
		} // end if
		
		else {
			if ( ( strcmp( walk->item, "+" ) != 0 ) && ( strcmp( walk->item, "-" ) != 0 ) && 
			     ( strcmp( walk->item, "*" ) != 0 ) && strcmp( walk->item, "/" ) != 0 ) {
				if ( strcmp( walk->item, "." ) == 0 ) {
					stop = true;
				} // end if
				
				else {
					error = true;
				} // end else
				
			} // end if
			
			else {
				label = false;
			} // end else
		} // end if
		
		walk = walk->next;
	} // end while
	
	if ( ( error == false ) && ( label == true ) ) {
		temp.error = false;
		temp.havecode = false;
	    temp.line = curLine;
		temp.loc = 0x0;
		temp.base = false;
		temp.equ = false;
	} // end if
	
	else {
		temp.error = true;
		temp.line = curLine;
		sprintf( temp.errorM, "equ syntax error" );
		printf( "%d: error equ syntax\n", temp.line );
	} // end else
	
} // CheckEqu()

void SIC::Equ( LineType & temp, TokenPtr t ) {
	TokenPtr tNext = t->next;
	if ( tNext != NULL ) {
		if ( tNext->table == 6 ) {
			if ( tNext->next == NULL ) {
				temp.error = false;
		        temp.havecode = false;
	        	temp.line = curLine;
		        temp.base = false;
		        int h = 0x0;
		        h = D_To_H( tNext->item );
		        temp.loc = h;
		        temp.equ = true;
			} // end if
			
			else if ( strcmp( tNext->next->item, "." ) == 0 ) {
				temp.error = false;
		        temp.havecode = false;
	        	temp.line = curLine;
		        temp.base = false;
		        int h = 0x0;
		        h = D_To_H( tNext->item );
		        temp.loc = h;
		        temp.equ = true;
			} // end else if
			
			else {
				temp.error = true;
		        temp.line = curLine;
		        sprintf( temp.errorM, "EQU syntax error" );
		        printf( "%d: EQU syntax error\n", temp.line );
			} // end else
		} // end if
		
		else if ( tNext->table == 5 ) {
			if ( tNext->next == NULL ) {
				int addr = 0x0;
				addr = GetSymbolAddr( tNext->num );
				if ( addr != -1 ) {
					temp.error = false;
		            temp.havecode = false;
	        	    temp.line = curLine;
		            temp.loc = addr;
		            temp.base = false;
		            temp.equ = true;
				} // end if
				
				else { // undefine
					temp.error = false;
		            temp.havecode = false;
	        	    temp.line = curLine;
		            temp.loc = 0x0;
		            temp.base = false;
		            temp.equ = false;
				} // end else
			} // end if
			
			else if ( strcmp( tNext->next->item, "." ) == 0 ) {
				int addr = 0x0;
				addr = GetSymbolAddr( tNext->num );
				if ( addr != -1 ) {
					temp.error = false;
		            temp.havecode = false;
	        	    temp.line = curLine;
		            temp.loc = addr;
		            temp.base = false;
		            temp.equ = true;
				} // end if
				
				else { // undefine
				    temp.error = false;
		            temp.havecode = false;
	        	    temp.line = curLine;
		            temp.loc = 0x0;
		            temp.base = false;
		            temp.equ = false;
				} // end else
				
			} // end else if
			
			else {
				CheckEqu( temp, tNext );
			} // end else
		} // end else if
		
		else if ( strcmp( tNext->item, "*" ) == 0 ) {
			if ( tNext->next == NULL ) {
				temp.error = false;
		        temp.havecode = false;
	        	temp.line = curLine;
		        temp.base = false;
		        temp.loc = curLoc;
		        temp.equ = true;
			} // end if
			
			else if ( strcmp( tNext->next->item, "." ) == 0 ) {
				temp.error = false;
		        temp.havecode = false;
	        	temp.line = curLine;
		        temp.base = false;
		        temp.loc = curLoc;
		        temp.equ = true;
			} // end else if
			
			else {
				temp.error = true;
		        temp.line = curLine;
		        sprintf( temp.errorM, "EQU syntax error" );
		        printf( "%d: EQU syntax error\n", temp.line );
			} // end else
		} // end else if
		
		else {
			temp.error = true;
		    temp.line = curLine;
		    sprintf( temp.errorM, "EQU syntax error" );
		    printf( "%d: EQU syntax error\n", temp.line );
		} // end else
		
	} // end if
	 
	else {
		temp.error = true;
		temp.line = curLine;
		sprintf( temp.errorM, "EQU syntax error" );
		printf( "%d: EQU syntax error\n", temp.line );
	} // end else
} // Equ()

void SIC::RealCode( LineType & temp, TokenPtr t ) {
	if ( strcmp( t->item, "." ) == 0 ) {
		temp.error = false;
		temp.havecode = false;
		temp.line = curLine;
		temp.loc = curLoc;
		temp.com = true;
	} // end if
	
	else if ( t->table == 1 ) {
		Str100 low;
		strcpy( low, t->item );
		strlwr(low);
		if ( strcmp( low, "rsub" ) == 0 ) {
			if ( t->next == NULL ) {
				temp.error = false;
		        temp.havecode = true;
	        	temp.line = curLine;
		        temp.loc = curLoc;
		        temp.base = false;
		        curLoc = curLoc + 0x3;
			} // end if
			
			else {
				if ( strcmp( t->next->item, "." ) == 0 ) {
					temp.error = false;
		            temp.havecode = true;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            curLoc = curLoc + 0x3;
				} // end if
				
				else { // syntax error
					temp.error = true;
					temp.line = curLine;
					sprintf( temp.errorM, "syntax error" );
					printf( "%d: syntax error\n", temp.line );
				} // end else
			} // end else
		} // end if
		
		else {
			Type1( temp, t );
		} // end else
	} // end else if
	
	else if ( t->table == 2 ) {
		Pseudo( temp, t );
	} // end else if
	
	else {
		temp.error = true;
		temp.line = curLine;
		sprintf( temp.errorM, "syntax error" );
		printf( "%d: syntax error\n", temp.line );
	} // end else
	
} // RealCode()

void SIC::CreatLine( LineType & temp ) {
	if ( cur != NULL ) {
		if ( cur->table == 5 ) {
			TokenPtr nextToken = cur->next;
			if ( nextToken != NULL ) {
				Str100 u;
				strcpy( u, nextToken->item );
				strupr( u );
				if ( strcmp( nextToken->item, "." ) == 0 ) { // code null
					temp.line = curLine;
				    temp.error = true;
				    sprintf( temp.errorM, "code cannot be null" );
				    printf( "%d: code cannot be null\n", temp.line );
				} //end if
				
				else if ( strcmp( u, "EQU" ) == 0 ) {
					Equ( temp, nextToken );
					if ( line[curIndex].equ == true ) {
						symtab[cur->num].addr = line[curIndex].loc;
					} // end if
					
					
				} // end else if
				
				else {
					RealCode( temp, nextToken );
				} // end else
			} // end if
			
			else {
				temp.line = curLine;
			    temp.error = true;
			    sprintf( temp.errorM, "code cannot be null" );
			    printf( "%d: code cannot be null\n", temp.line );
			} // end else
			
			if ( temp.error == false ) {
				if ( InsertSymbol( cur->item, temp.loc, cur->num ) == false ) {
					temp.line = curLine;
			        temp.error = true;
			        sprintf( temp.errorM, "duplicate symbol" );
			        printf( "%d: duplicate symbol\n", temp.line );
				} // end if
				
			} // end if
		} // end if
		
		else {
			RealCode( temp, cur );
		} // end
	} // end if
	
} // CreatLine()

void SIC::Allocate( LineType & temp, TokenPtr nextToken ) {
	Str100 u;
	strcpy( u, nextToken->item );
	strupr( u );
	if ( strcmp( u, "START" ) == 0 ) {
		TokenPtr nnToken = nextToken->next;
		if ( nnToken != NULL ) {
		    if ( nnToken->table == 6 ) {
				if ( nnToken->next == NULL ) {
					sscanf( nnToken->item, "%x", &curLoc );
					temp.error = false;
					temp.havecode = false;
					temp.line = curLine;
					temp.loc = curLoc;
				} // end if
							
				else {
					if ( strcmp( nnToken->next->item, "." ) == 0 ) { // comment
						sscanf( nnToken->item, "%x", &curLoc );
						temp.error = false;
						temp.havecode = false;
						temp.line = curLine;
						temp.loc = curLoc;
					} // end if
								
					else { // START syntax error
						temp.line = curLine;
						temp.error = true;
						sprintf( temp.errorM, "START syntax error" );
						printf( "%d: START syntax error\n", temp.line );
					} // end else
				} // end else
			} // end if
						
			else { // START syntax error
				temp.line = curLine;
				temp.error = true;
				sprintf( temp.errorM, "START syntax error" );
				printf( "%d: START syntax error\n", temp.line );
			} // end else
		} // end if
					
		else { // START syntax error
			temp.line = curLine;
			temp.error = true;
			sprintf( temp.errorM, "START syntax error" );
			printf( "%d: START syntax error\n", temp.line );
		} // end else
	} // end if
				
	else {
		curLoc = 0x0000;
		CreatLine( temp );
	} // end else
} // Allocate()

void SIC::FirstLine() {
	if ( cur != NULL ) {
		LineType temp;
		
		if ( cur->table == 5 ) { // Symble
			TokenPtr nextToken = cur->next;
			if ( nextToken != NULL ) {
				if ( strcmp( nextToken->item, "." ) == 0 ) { // code null
					temp.line = curLine;
				    temp.error = true;
				    sprintf( temp.errorM, "code canot be null" );
				    printf( "%d: code cannot be null\n", temp.line );
				} //end if
				
				else {
					Allocate( temp, nextToken );
				} // end else
				
			} // end if
			
			else { // code null
				temp.line = curLine;
				temp.error = true;
				sprintf( temp.errorM, "code cannot be null" );
				printf( "%d: code cannot be null\n", temp.line );
			} // end else
			
			if ( temp.error == false ) {
				if ( InsertSymbol( cur->item, curLoc, cur->num ) == false ) { // duplicate
					temp.error = true;
					sprintf( temp.errorM, "duplicate symbol" );
					printf( "%d: duplicate symbol\n", temp.line );
				} // end if
			} // end if
		} // end if
		
		else {
			Allocate( temp, cur );
		} // end else
		
		line.push_back( temp );
		index++;
	} // end if
	
	else {
		printf( "Empty File\n" );
	} // end else
} // FirstLine()

void SIC::Pass1( HeadTokenPtr curHead ) {
	FirstLine();
	curHead = curHead->nextHead;
	while ( curHead != NULL ) {
		curLine = curLine + 5;
		LineType temp;
		temp.com = false;
		temp.end = false;
		temp.equ = false;
		temp.error = true;
		cur = curHead->token;
		CreatLine( temp );
		line.push_back( temp );
		index++;
		curHead = curHead->nextHead;
	} // end while
	
} // Pass1()

int SIC::GetOp( Str100 s ) {
	Str100 l;
	strcpy( l, s );
	strlwr( l );
	int ans = 0x0;
	if ( strcmp( l, "add" ) == 0 ) {
		ans = 0x18;
	} // end if
	
	else if ( strcmp( l, "and" ) == 0 ) {
		ans = 0x40;
	} // end else if
	
	else if ( strcmp( l, "comp" ) == 0 ) {
		ans = 0x28;
	} // end else if
	
	else if ( strcmp( l, "div" ) == 0 ) {
		ans = 0x24;
	} // end else if
	
	else if ( strcmp( l, "j" ) == 0 ) {
		ans = 0x3c;
	} // end else if
	
	else if ( strcmp( l, "jeq" ) == 0 ) {
		ans = 0x30;
	} // end else if
	
	else if ( strcmp( l, "jgt" ) == 0 ) {
		ans = 0x34;
	} // end else if
	
	else if ( strcmp( l, "jlt" ) == 0 ) {
		ans = 0x38;
	} // end else if
	
	else if ( strcmp( l, "jsub" ) == 0 ) {
		ans = 0x48;
	} // end else if
	
	else if ( strcmp( l, "lda" ) == 0 ) {
		ans = 0x00;
	} // end else if
	
	else if ( strcmp( l, "ldch" ) == 0 ) {
		ans = 0x50;
	} // end else if
	
	else if ( strcmp( l, "ldl" ) == 0 ) {
		ans = 0x08;
	} // end else if
	
	else if ( strcmp( l, "ldx" ) == 0 ) {
		ans = 0x04;
	} // end else if
	
	else if ( strcmp( l, "mul" ) == 0 ) {
		ans = 0x20;
	} // end else if
	
	else if ( strcmp( l, "or" ) == 0 ) {
		ans = 0x44;
	} // end else if
	
	else if ( strcmp( l, "rd" ) == 0 ) {
		ans = 0xd8;
	} // end else if
	
	else if ( strcmp( l, "rsub" ) == 0 ) {
		ans = 0x4c;
	} // end else if
	
	else if ( strcmp( l, "sta" ) == 0 ) {
		ans = 0x0c;
	} // end else if
	
	else if ( strcmp( l, "stch" ) == 0 ) {
		ans = 0x54;
	} // end else if
	
	else if ( strcmp( l, "stl" ) == 0 ) {
		ans = 0x14;
	} // end else if
	
	else if ( strcmp( l, "stsw" ) == 0 ) {
		ans = 0xe8;
	} // end else if
	
	else if ( strcmp( l, "stx" ) == 0 ) {
		ans = 0x10;
	} // end else if
	
	else if ( strcmp( l, "sub" ) == 0 ) {
		ans = 0x1c;
	} // end else if
	
	else if ( strcmp( l, "td" ) == 0 ) {
		ans = 0xe0;
	} // end else if
	
	else if ( strcmp( l, "tix" ) == 0 ) {
		ans = 0x2c;
	} // end else if
	
	else if ( strcmp( l, "wd" ) == 0 ) {
		ans = 0xdc;
	} // end else if
	
	return ans;
} // GetOp()

void SIC::FillObjcode( TokenPtr t ) {
	if ( t->table == 1 ) {
		if ( t->next != NULL ) {
			int addr = 0x0;
			int op = 0x0;
			addr = GetSymbolAddr( t->next->num );
			if ( addr != -1 ) {
				op = GetOp( t->item );
				if ( line[curIndex].base == true ) {
					int bin = 0b0;
					bin = addr;
					bin = bin | ( 1 << 15 );
					addr = bin;
					sprintf( line[curIndex].objcode, "%02X%04X", op, addr );
				} // end if
				
				else {
					sprintf( line[curIndex].objcode, "%02X%04X", op, addr );
				} // end else
			} // end if
			
			else { // undefine
				line[curIndex].error = true;
				sprintf( line[curIndex].errorM, "undefine symbol" );
				printf( "%d: undefine symbol\n", line[curIndex].line );
			} // end else
			
		} // end if
		
		else {
			Str100 l;
			strcpy( l, t->item );
			strlwr( l );
			if ( strcmp( l, "rsub" ) == 0 ) {
				int op = 0x0;
				op = GetOp( t->item );
				sprintf( line[curIndex].objcode, "%02X0000", op );
			} // end if
			
		} // end else
		
	} // end if
	
	else if ( t->table == 2 ) {
		Str100 u;
		strcpy( u, t->item );
		strupr(u);
		if ( strcmp( u, "BYTE" ) == 0 ) {
			if ( t->next != NULL ) { // ' or dec
			    if ( t->next->table == 6 ) {
					int o = 0x0;
					o = D_To_H( t->next->item );
					sprintf( line[curIndex].objcode, "%X", o );
				} // end if
				
				else if ( t->next->next != NULL ) {
					TokenPtr tnn = t->next->next;
					if ( tnn->table == 7 ) {
						int i = 2;
						Str100 buf;
						if ( tnn->item[1] != '\0' ) {
							sprintf( buf, "%02X", tnn->item[1] );
						} // end if
						
						while ( tnn->item[i] != '\0' ) {
							sprintf( buf, "%s%02X", buf, tnn->item[i] );
							i++;
						} // end while
						
						strcpy( line[curIndex].objcode, buf );
					} // end if
					
					else if ( tnn->table == 6 ) {
						int i = 1;
						while ( tnn->item[i] != '\0' ) {
							line[curIndex].objcode[i-1]  = tnn->item[i];
							i++;
						} // end while
						
						line[curIndex].objcode[i-1] = '\0';
					} // end else if
					
				} // end else if
				
			} // end if
		} // end if
		
		else if ( strcmp( u, "WORD" ) == 0 ) {
			if ( t->next != NULL ) { // ' or dec
			    if ( t->next->table == 6 ) {
					int len = strlen( t->next->item );
					if ( len > 6 ) {
						line[curIndex].error = true;
						sprintf( line[curIndex].errorM, "WORD overflow" );
						printf( "%d: WORD overflow\n", line[curIndex].line );
					} // end if
					
					else {
						int o = 0x0;
					    o = D_To_H( t->next->item );
					    sprintf( line[curIndex].objcode, "%06X", o );
					} // end else
					
				} // end if
				
				else if ( t->next->next != NULL ) {
					TokenPtr tnn = t->next->next;
					if ( tnn->table == 7 ) {
						int i = 2;
						Str100 buf;
						if ( tnn->item[1] != '\0' ) {
							sprintf( buf, "%02X", tnn->item[1] );
						} // end if
						
						while ( tnn->item[i] != '\0' ) {
							sprintf( buf, "%s%02X", buf, tnn->item[i] );
							i++;
						} // end while
						
						int len = strlen( buf );
						if ( len > 6 ) { // overflow
							line[curIndex].error = true;
							sprintf( line[curIndex].errorM, "WORD overflow" );
							printf( "%d: WORD overflow\n", line[curIndex].line );
						} // end if
						
						else {
							int j = 0;
						    while( len < 6 ) {
							    line[curIndex].objcode[j] = '0';
							    len++;
							    j++;
						    } // end while
						    
						    int g = 0;
						    while ( buf[g] != '\0' ) {
							    line[curIndex].objcode[j] = buf[g];
							    g++;
							    j++;
						    } // end while
						    
						    line[curIndex].objcode[j] = '\0';
						} // end else
					} // end if
					
					else if ( tnn->table == 6 ) {
						int i = 1;
						Str100 buf;
						while ( tnn->item[i] != '\0' ) {
							buf[i-1]  = tnn->item[i];
							i++;
						} // end while
						
						buf[i-1] = '\0';
						int len = strlen( buf );
						if ( len > 6 ) { // overflow
							line[curIndex].error = true;
							sprintf( line[curIndex].errorM, "WORD over flow" );
							printf( "%d: WORD overflow\n", line[curIndex].line );
						} // end if
						
						else {
							int j = 0;
						    while( len < 6 ) {
							    line[curIndex].objcode[j] = '0';
							    len++;
							    j++;
						    } // end while
						    
						    int g = 0;
						    while ( buf[g] != '\0' ) {
							    line[curIndex].objcode[j] = buf[g];
							    g++;
							    j++;
						    } // end while
						    
						    line[curIndex].objcode[j] = '\0';
						} // end else
					} // end else if
					
				} // end else if
				
			} // end if
		} // end else if
		
	} // end else if
} // FillObjcode()

void SIC::EquPass2( TokenPtr t ) {
	int addr[100];
	char o[100];
	int ans = 0x0;
	int i = 0;
	int oi = -1;
	bool error = false;
	addr[0] = GetSymbolAddr( t->num );
	if ( addr[0] == -1 ) {
		line[curIndex].error = true;
	} // end if
	
	else {
		if ( t->next == NULL ) {
			ans = addr[0];
		    line[curIndex].loc = ans;
		    line[curIndex].equ = true;
		} // end if
		
		else {
			TokenPtr walk = t->next;
			bool stop = false;
			while ( ( walk != NULL ) && ( stop == false ) ) {
				if ( strcmp( walk->item, "." ) == 0 ) {
					stop = true;
				} // end if
				
				else if ( walk->table == 5 ) {
					i++;
					addr[i] = GetSymbolAddr( walk->num );
					if ( addr[i] == -1 ) {
						error = true;
						stop = true;
					} // end if
				} // end else if
				
				else {
					if ( strcmp( walk->item, "*" ) == 0 ) {
						walk = walk->next;
						if ( walk != NULL ) {
							i++;
					        addr[i] = GetSymbolAddr( walk->num );
					        if ( addr[i] == -1 ) {
						        error = true;
						        stop = true;
						    } // end if
						    
						    else {
						    	addr[i-1] = addr[i-1] * addr[i];
						    	i--;
							} // end else
					    } // end if
					} // end if
					
					else if ( strcmp( walk->item, "/" ) == 0 ) {
						walk = walk->next;
						if ( walk != NULL ) {
							i++;
					        addr[i] = GetSymbolAddr( walk->num );
					        if ( addr[i] == -1 ) {
						        error = true;
						        stop = true;
						    } // end if
						    
						    else {
						    	addr[i-1] = addr[i-1] / addr[i];
						    	i--;
							} // end else
					    } // end if
					    
					} // end else if
					
					else {
						oi++;
						o[oi] = walk->item[0];
					} // end else
				} // end else
				
				walk = walk->next;
			} // end while
			
			if ( error == false ) {
				int j = 1;
				int a = addr[0];
				int k = 0;
				while ( k <= oi ) {
					if ( o[k] == '+' ) {
						a = a + addr[j];
					} // end if
					
					else if ( o[k] == '-' ) {
						a = a - addr[j];
					} // end else if
					
					j++;
					k++;
				} // end while
				
				ans = a;
				line[curIndex].loc = ans;
				line[curIndex].equ = true;
			} // end if
			
			else {
				line[curIndex].error = true;
				sprintf( line[curIndex].errorM, "EQU undefine symbol" );
				printf( "%d: EQU undefine symbol\n", line[curIndex].line );
			} // end else
			
		} // end else
		
	} // end else
	
} // EquPass2()

void SIC::LinePass2() {
	if ( line[curIndex].error == false ) {
		TokenPtr t = cur;
		if ( line[curIndex].havecode == true ) {
			if ( t != NULL ) {
				if ( t->table == 5 ) {
					if ( t->next != NULL ) {
						FillObjcode( t->next );
					} // end if
				} // end if
				
				else {
					FillObjcode( t );
				} // end else
			} // end if
		} // end if
		
		else if ( t != NULL ) {
			if ( t->next != NULL ) {
				Str100 u;
		        strcpy( u, t->next->item );
	        	strupr(u);
				if ( strcmp( u, "EQU" ) == 0 ) {
					if ( line[curIndex].equ == false ) {
						if ( t->next->next != NULL ) {
							EquPass2( t->next->next );
							if ( line[curIndex].equ == true ) {
						        symtab[t->num].addr = line[curIndex].loc;
					        } // end if
					        
					        else {
					        	line[curIndex].error = true;
					        	sprintf( line[curIndex].errorM, "EQU undefine symbol" );
					        	printf( "%d: EQU undefine symbol\n", line[curIndex].line );
							} // end if
							
						} // end if
					} // end if
				} // end if
			} // end if
		} // end else if
		
	} // end if
} // LinePass2()

void SIC::Pass2() {
	curIndex = 0;
	HeadTokenPtr curHead = ghead;
	while ( curHead != NULL ) {
		cur = curHead->token;
		if ( cur != NULL ) {
			LinePass2();
			OutPutLine( line[curIndex] );
		} // end if
		
		curHead = curHead->nextHead;
		curIndex++;
	} // end while
} // Pass2()

void SIC::SICMain() {
	curLine = 5;
	index = 0;
	ClearSymTab();
	line.clear();
	HeadTokenPtr curHead = ghead;
	if ( curHead != NULL ) {
		ClearOutFile();
		cur = curHead->token;
		Pass1( curHead );
		Pass2();
	} // end if
	
	else {
		printf( "Empty File\n" );
	} // end else
} // SICMain()

void SIC::ClearOutFile() {
	FILE *outfile = NULL;
	
	printf( "\nInput the output token file name (e.g., SICXE_Output.txt ...):" );
	cin >> outFileName ;
	outfile = fopen(outFileName.c_str(),"w");
	
	if ( outfile == NULL ) {
	    cout << endl << "### open " << outFileName << " failed ###" << endl;
	} // end if
	
	else {
		fprintf( outfile, "Line\tLoc\tSource statement\t\t\tObject code\n" );
	} // end else
	
	fclose(outfile);
} // ClearOutFile

void SIC::OutPutLine( LineType l ) {
	FILE *outfile = NULL;
	outfile = fopen(outFileName.c_str(),"a");
	TokenPtr w = NULL;
	
	if ( outfile != NULL ) {
	    if ( l.error == false ) {
	    	if ( ( l.com == true ) || ( l.end == true ) ) {
	    		fprintf( outfile, "\n%3d\t\t", l.line );
	    		w = cur;
	    		while ( w != NULL ) {
	    			if ( strcmp( w->item, "." ) == 0 ) {
	    				fprintf( outfile, "%s", w->item );
	    				int e = 0;
	    				while ( w->com[e] != '\0' ) {
	    					fprintf( outfile, "%c", w->com[e] );
	    					e++;
						} // end while
	    				
					} // end if
					
					else {
						fprintf( outfile, "%s  ", w->item );
					} // end else
					
	    			w = w->next;
				} // end while
	    		
			} // end if
			
			else if ( l.havecode == true ) {
				fprintf( outfile, "\n%3d\t%04X\t", l.line, l.loc );
				w = cur;
				while ( w != NULL ) {
					if ( strcmp( w->item, "\'" ) == 0 ) { // '
						w = w->next; // X or C
						if ( w != NULL ) {
							fprintf( outfile, "%c\'", w->item[0] ); // X or C
							int c = 1;
							while ( w->item[c] != '\0' ) {
								fprintf( outfile, "%c", w->item[c] );
								c++;
							} // end while
							
							w = w->next; //'
							if ( w != NULL ) {
								fprintf( outfile, "\'" );
							} // end if
							
						} // end if
						
					} // end if
					
					else if ( strcmp( w->item, "." ) == 0 ) {
						fprintf( outfile, "%s", w->item );
	    				int e = 0;
	    				while ( w->com[e] != '\0' ) {
	    					fprintf( outfile, "%c", w->com[e] );
	    					e++;
						} // end while
	    				
					} // end else if
					
					else {
						fprintf( outfile, "%s  ", w->item );
					} // end else
					
					w = w->next;
				} // end while
				
				fprintf( outfile, "\t%s", l.objcode );
				
			} // end else if
			
			else {
				fprintf( outfile, "\n%3d\t%04X\t", l.line, l.loc );
				w = cur;
	    		while ( w != NULL ) {
	    			if ( strcmp( w->item, "." ) == 0 ) {
						fprintf( outfile, "%s", w->item );
	    				int e = 0;
	    				while ( w->com[e] != '\0' ) {
	    					fprintf( outfile, "%c", w->com[e] );
	    					e++;
						} // end while
	    				
					} // end if
					
					else {
						fprintf( outfile, "%s  ", w->item );
					} // end else
	    			
	    			w = w->next;
				} // end while
	    		
	    		
			} // end else
			
		} // end if
		
		else {
			fprintf( outfile, "\n%3d\tError: ", l.line );
			int p = 0;
			while ( l.errorM[p] != '\0' ) {
				fprintf( outfile, "%c", l.errorM[p] );
				p++;
			} // end while
			
		} // end else
		
	} // end if
	
	else {
		cout << endl << "### open " << outFileName << " failed ###" << endl;
	} // end else
	
	fclose(outfile);
} // OutPutLine()

void DealSIC() {
	string fileName;
	GetToken( fileName );
	SIC sic;
	sic.SICMain();
	
} // SIC()

void SICXE::ClearOutFile() {
	FILE *outfile = NULL;
	
	printf( "\nInput the output token file name (e.g., SICXE_Output.txt ...):" );
	cin >> outFileName ;
	outfile = fopen(outFileName.c_str(),"w");
	
	if ( outfile == NULL ) {
	    cout << endl << "### open " << outFileName << " failed ###" << endl;
	} // end if
	
	else {
		fprintf( outfile, "Line\tLoc\tSource statement\t\t\tObject code\n" );
		fprintf( outfile, "---- \t--------\t-------------------------\t\t-----------\n" );
	} // end else
	
	fclose(outfile);
	
} // ClearOutFile()

void SICXE::ClearSymTab() {
	for ( int i = 0; i < 100; i++ ) {
		symtab[i].symbol[0] = '\0';
		literTab[i].name[0] = '\0';
	} // end for
} // ClearSymTab()

bool SICXE::InsertSymbol( Str100 str, int addr, int num ) {
	if ( symtab[num].symbol[0] == '\0' ) {
		strcpy( symtab[num].symbol, str );
		symtab[num].addr = addr;
		return true;
	} // end if
	
	else {
		return false;
	} // end else
} // InsertSymbol()

int SICXE::GetSymbolAddr( int num ) {
	if ( symtab[num].symbol[0] != '\0' ) {
		return symtab[num].addr;
	} // end if
	
	else {
		return -1;
	} // end else
} // GetSymbolAddr()

int SICXE::InsertLiteral( Str100 str, int i, int & num ) {
	int hash = 0;
	int sum = 0;
	while ( str[i] != '\0' ) {
		sum = sum + str[i];
		i++;
	} // end while
	
	hash = sum % 100;
	if ( literTab[hash].name[0] == '\0'  ) {
		strcpy( literTab[hash].name, str );
		return hash;
	} // end if
	
	else {
		if ( strcmp( literTab[hash].name, str ) == 0 ) {
			num = hash;
			return -1;
		} // end if
		
		else {
			hash++;
			int count = 0;
			while ( literTab[hash].name[0] != '\0' ) {
				if ( strcmp( literTab[hash].name, str ) == 0 ) {
					num = hash;
			        return -1;
		        } // end if
		        
		        if ( hash == 99 ) {
		        	hash = 0;
				} // end if
				
				else {
					hash++;
				} // end else
				count++;
				if ( count > 100 ) {
					printf( "###Literal table full ###\n" );
					return -2;
				} // end if
			} // end while
			
			strcpy( literTab[hash].name, str );
			return hash;
		} // end else
	} // end else
} // InsertLiteral()

void SICXE::Literal( LineType & temp, TokenPtr t ) {
	LiteralType liter;
	if ( strcmp( t->item, "\'" ) == 0 ) { // '
		TokenPtr tNext = t->next;
		liter.type = BYTE;
		if ( tNext != NULL ) {
			if ( tNext->table == 6 ) { // X
				if ( tNext->next != NULL ) {
					if ( strcmp( tNext->next->item, "\'" ) == 0 ) { // '
						if ( tNext->next->next == NULL ) {
							strcpy( liter.name, tNext->item );
							temp.error = false;
		                    temp.havecode = true;
	        	            temp.line = curLine;
		                    temp.loc = curLoc;
		                    temp.base = false;
		                    temp.literal = true;
						} // end if
						
						else if ( strcmp( tNext->next->next->item, "." ) == 0 ) {
							strcpy( liter.name, tNext->item );
							temp.error = false;
		                    temp.havecode = true;
	        	            temp.line = curLine;
		                    temp.loc = curLoc;
		                    temp.base = false;
		                    temp.literal = true;
						} // end else if
						
						else {
							temp.error = true;
		                    temp.line = curLine;
		                    sprintf( temp.errorM, "literal syntax error" );
						} // end else
						
					} // end if
					
					else {
						temp.error = true;
		                temp.line = curLine;
		                sprintf( temp.errorM, "literal syntax error" );
					} // end else
					
				} // end if
				
				else {
					temp.error = true;
		            temp.line = curLine;
		            sprintf( temp.errorM, "literal syntax error" );
				} // end else
			} // end if
			
			else if ( tNext->table == 7 ) { // C
				if ( tNext->next != NULL ) {
					if ( strcmp( tNext->next->item, "\'" ) == 0 ) { // '
						if ( tNext->next->next == NULL ) {
							strcpy( liter.name, tNext->item );
							temp.error = false;
		                    temp.havecode = true;
	        	            temp.line = curLine;
		                    temp.loc = curLoc;
		                    temp.base = false;
		                    temp.literal = true;
						} // end if
						
						else if ( strcmp( tNext->next->next->item, "." ) == 0 ) {
							strcpy( liter.name, tNext->item );
							temp.error = false;
		                    temp.havecode = true;
	        	            temp.line = curLine;
		                    temp.loc = curLoc;
		                    temp.base = false;
		                    temp.literal = true;
						} // end else if
						
						else {
							temp.error = true;
		                    temp.line = curLine;
		                    sprintf( temp.errorM, "literal syntax error" );
						} // end else
						
					} // end if
					
					else {
						temp.error = true;
		                temp.line = curLine;
		                sprintf( temp.errorM, "literal syntax error" );
					} // end else
					
				} // end if
				
				else {
					temp.error = true;
		            temp.line = curLine;
		            sprintf( temp.errorM, "literal syntax error" );
				} // end else
				
			} // end if
			
			else {
				temp.error = true;
		        temp.line = curLine;
		        sprintf( temp.errorM, "literal syntax error" );
			} // end else
			
		} // end if
		
		else {
			temp.error = true;
		    temp.line = curLine;
		    sprintf( temp.errorM, "literal syntax error" );
		} // end else
		
	} // end if
	
	else if ( t->table == 6 ) {
		liter.type = WORD;
		if ( t->next == NULL ) {
			strcpy( liter.name, t->item );
			temp.error = false;
		    temp.havecode = true;
	        temp.line = curLine;
		    temp.loc = curLoc;
		    temp.base = false;
		    temp.literal = true;
		} // end if
		
		else if ( strcmp( t->next->item, "." ) == 0 ) {
			strcpy( liter.name, t->item );
			temp.error = false;
		    temp.havecode = true;
	        temp.line = curLine;
		    temp.loc = curLoc;
		    temp.base = false;
		    temp.literal = true;
		} // end else if
		
		else {
			temp.error = true;
		    temp.line = curLine;
		    sprintf( temp.errorM, "literal syntax error" );
		} // end else
		
	} // end else if
	
	else {
		temp.error = true;
		temp.line = curLine;
		sprintf( temp.errorM, "literal syntax error" );
	} // end else
	
	if ( temp.literal = true ) {
		int star;
		if ( liter.type == WORD ) {
			star = 0;
		} // end if
		
		else {
			star = 1;
		} // end else
		
		int l;
		int ln = InsertLiteral( liter.name, star, l );
		if ( ln != -2 ) {
			if ( ln != -1 ) {
				literTab[ln].type = liter.type;
				literTab[ln].num = ln;
				liter.num = ln;
				literBuf.push_back( liter );
				temp.litNum = ln;
			} // end if
			
			else { // duplicate literal
				temp.litNum = l;
			} // end else
			
		} // end if
		
		else { // table full
			temp.error = true;
		    temp.line = curLine;
		    sprintf( temp.errorM, "literal table full" );
		} // end else
		
	} // end if
	
} // Literal()

void SICXE::Type2( LineType & temp, TokenPtr t ) {
	if ( strcmp( t->item, "#" ) == 0 ) {
		if ( t->next != NULL ) {
			if ( t->next->table == 5 ) {
				if ( t->next->next == NULL ) {
					temp.error = false;
		            temp.havecode = true;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
				} // end if
				
				else if ( strcmp( t->next->next->item, "." ) == 0 ) {
					temp.error = false;
		            temp.havecode = true;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
				} // end else if
				
				else {
					temp.error = true;
		            temp.line = curLine;
		            sprintf( temp.errorM, "immediate syntax error" );
				} // end else
			} // end if
			
			else if ( t->next->table == 6 ) {
				if ( t->next->next == NULL ) {
					temp.error = false;
		            temp.havecode = true;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
				} // end if
				
				else if ( strcmp( t->next->next->item, "." ) == 0 ) {
					temp.error = false;
		            temp.havecode = true;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
				} // end else if
				
				else {
					temp.error = true;
		            temp.line = curLine;
		            sprintf( temp.errorM, "immediate syntax error" );
				} // end else
			} // end else if
			
			else {
				temp.error = true;
		        temp.line = curLine;
		        sprintf( temp.errorM, "immediate syntax error" );
			} // end else
		} // end if
		
		else {
			temp.error = true;
		    temp.line = curLine;
		    sprintf( temp.errorM, "immediate syntax error" );
		} // end else
	} // end if
	
	else if ( strcmp( t->item, "@" ) == 0 ) {
		if ( t->next != NULL ) {
			if ( t->next->table == 5 ) {
				if ( t->next->next == NULL ) {
					temp.error = false;
		            temp.havecode = true;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
				} // end if
				
				else if ( strcmp( t->next->next->item, "." ) == 0 ) {
					temp.error = false;
		            temp.havecode = true;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
				} // end else if
				
				else {
					temp.error = true;
		            temp.line = curLine;
		            sprintf( temp.errorM, "indirect syntax error" );
				} // end else
				
			} // end if
			
			else {
				temp.error = true;
		        temp.line = curLine;
		        sprintf( temp.errorM, "indirect syntax error" );
			} // end else
		} // end if
		
		else {
			temp.error = true;
		    temp.line = curLine;
		    sprintf( temp.errorM, "indirect syntax error" );
		} // end else
		
	} // end else if
	
	else if ( strcmp( t->item, "=" ) == 0 ) {
		if ( t->next != NULL ) {
			Literal( temp, t->next );
		} // end if
		
		else {
			temp.error = true;
		    temp.line = curLine;
		    sprintf( temp.errorM, "literal syntax error" );
		} // end else
	} // end else if
	
	else {
		temp.error = true;
		temp.line = curLine;
		sprintf( temp.errorM, "syntax error" );
	} // end else
	
} // Type2()

void SICXE::Type1( LineType & temp, TokenPtr t ) {
	TokenPtr tNext = t->next;
	if ( tNext != NULL ) {
		if ( tNext->table == 5 ) {
			if ( tNext->next != NULL ) {
				if ( strcmp( tNext->next->item, "," ) == 0 ) {
					if ( tNext->next->next != NULL ) {
						Str100 u;
						strcpy( u , tNext->next->next->item );
						strupr( u );
						if ( strcmp( u, "X" ) == 0 ) {
							if ( tNext->next->next->next == NULL ) {
								temp.error = false;
		                        temp.havecode = true;
	        	                temp.line = curLine;
		                        temp.loc = curLoc;
		                        temp.base = true;
							} // end if
							
							else if ( strcmp( tNext->next->next->next->item, "." ) == 0 ) {
								temp.error = false;
		                        temp.havecode = true;
	        	                temp.line = curLine;
		                        temp.loc = curLoc;
		                        temp.base = true;
							} // end else if
							
							else {
								temp.error = true;
		                        temp.line = curLine;
		                        sprintf( temp.errorM, "syntax error" );
							} // end else
						} // end if
						
						else {
							temp.error = true;
		                    temp.line = curLine;
		                    sprintf( temp.errorM, "syntax error" );
						} // end else
					} // end if
					
					else {
						temp.error = true;
		                temp.line = curLine;
		                sprintf( temp.errorM, "syntax error" );
					} // end else
				} // end if
				
				else if ( strcmp( tNext->next->item, "." ) == 0 ) {
					temp.error = false;
		            temp.havecode = true;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
				} // end else if
				
				else {
					temp.error = true;
		            temp.line = curLine;
		            sprintf( temp.errorM, "syntax error" );
				} // end else
				
			} // end if
			
			else {
				temp.error = false;
		        temp.havecode = true;
	        	temp.line = curLine;
		        temp.loc = curLoc;
		        temp.base = false;
			} // end else
			
		} // end if
		
		else if ( tNext->table == 6 ) {
			if ( tNext->next == NULL ) {
				temp.error = false;
		        temp.havecode = true;
	        	temp.line = curLine;
		        temp.loc = curLoc;
		        temp.base = false;
			} // end if
			
			else if ( strcmp( tNext->next->item, "." ) == 0 ) {
				temp.error = false;
		        temp.havecode = true;
	        	temp.line = curLine;
		        temp.loc = curLoc;
		        temp.base = false;
			} // end else if
			
			else {
				temp.error = true;
				temp.line = curLine;
				sprintf( temp.errorM, "syntax error" );
			} // end else
			
		} // end else if
		
		else if ( tNext->table == 4 ) {
			Type2( temp, tNext );
		} // end else if
		
		else {
			temp.error = true;
		    temp.line = curLine;
		    sprintf( temp.errorM, "syntax error" );
		} // end else
		
	} // end if
	
	else { // syntax error
		temp.error = true;
		temp.line = curLine;
		sprintf( temp.errorM, "syntax error" );
	} // end else
	
} // Type1()

int SICXE::GetFormat( Str100 s ) {
	if ( strcmp( s, "addr" ) == 0 ) {
		return 2;
	} // end else if
	
	else if ( strcmp( s, "clear" ) == 0 ) {
		return 2;
	} // end else if
	
	else if ( strcmp( s, "compr" ) == 0 ) {
		return 2;
	} // end else if
	
	else if ( strcmp( s, "divr" ) == 0 ) {
		return 2;
	} // end else if
	
	else if ( strcmp( s, "fix" ) == 0 ) {
		return 1;
	} // end else if
	
	else if ( strcmp( s, "float" ) == 0 ) {
		return 1;
	} // end else if
	
	else if ( strcmp( s, "hio" ) == 0 ) {
		return 1;
	} // end else if
	
	else if ( strcmp( s, "mulr" ) == 0 ) {
		return 2;
	} // end else if
	
	else if ( strcmp( s, "norm" ) == 0 ) {
		return 1;
	} // end else if
	
	else if ( strcmp( s, "rmo" ) == 0 ) {
		return 2;
	} // end else if
	
	else if ( strcmp( s, "shiftl" ) == 0 ) {
		return 2;
	} // end else if
	
	else if ( strcmp( s, "shiftr" ) == 0 ) {
		return 2;
	} // end else if
	
	else if ( strcmp( s, "sio" ) == 0 ) {
		return 1;
	} // end else if
	
	else if ( strcmp( s, "subr" ) == 0 ) {
		return 2;
	} // end else if
	
	else if ( strcmp( s, "svc" ) == 0 ) {
		return 2;
	} // end else if
	
	else if ( strcmp( s, "tio" ) == 0 ) {
		return 1;
	} // end else if
	
	else if ( strcmp( s, "tixr" ) == 0 ) {
		return 2;
	} // end else if
	
	else {
		return 0;
	} // end else
	
} // GetFormat

void SICXE::CheckFormat2( LineType & temp, TokenPtr t ) {
	Str100 low;
	strcpy( low, t->item );
	strlwr(low);
	TokenPtr tNext = t->next;
	if ( ( strcmp( low, "clear" ) == 0 ) || ( strcmp( low, "tixr" ) == 0 ) ) {
		if ( tNext != NULL ) {
			if ( tNext->table == 3 ) { // r1
				if ( tNext->next == NULL ) {
					temp.error = false;
		            temp.havecode = true;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            temp.format = 2;
		            curLoc = curLoc + 2;
				} // end if
				
				else if ( strcmp( tNext->next->item, "." ) == 0 ) {
					temp.error = false;
		            temp.havecode = true;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            temp.format = 2;
		            curLoc = curLoc + 2;
				} // end else if
				
				else {
					temp.error = true;
		            temp.line = curLine;
		            sprintf( temp.errorM, "format2 syntax error" );
				} // end else
				
			} // end if
			
			else {
				temp.error = true;
		        temp.line = curLine;
		        sprintf( temp.errorM, "format2 syntax error" );
			} // end else
			
		} // end if
		
		else {
			temp.error = true;
		    temp.line = curLine;
		    sprintf( temp.errorM, "format2 syntax error" );
		} // end else
		
	} // end if
	
	else if ( ( strcmp( low, "shiftl" ) == 0 ) || ( strcmp( low, "shiftr" ) == 0 ) ) {
		if ( tNext != NULL ) {
			if ( tNext->table == 3 ) { // r1
				TokenPtr tnn = tNext->next;
				if ( tnn != NULL ) {
					if( strcmp( tnn->item, "," ) == 0 ) { // ,
						if ( tnn->next != NULL ) {
							if ( tnn->next->table == 6 ) { // n
								if ( tnn->next->next == NULL ) {
									temp.error = false;
		                            temp.havecode = true;
	        	                    temp.line = curLine;
		                            temp.loc = curLoc;
		                            temp.base = false;
		                            temp.format = 2;
		                            curLoc = curLoc + 2;
								} // end if
								
								else if ( strcmp( tnn->next->next->item, "." ) == 0 ) {
									temp.error = false;
		                            temp.havecode = true;
	        	                    temp.line = curLine;
		                            temp.loc = curLoc;
		                            temp.base = false;
		                            temp.format = 2;
		                            curLoc = curLoc + 2;
								} // end else if
								
								else {
									temp.error = true;
					                temp.line = curLine;
					                sprintf( temp.errorM, "format2 syntax error" );
								} // end else
								
							} // end if
							
							else {
								temp.error = true;
					            temp.line = curLine;
					            sprintf( temp.errorM, "format2 syntax error" );
							} // end else
						} // end if
						
						else {
							temp.error = true;
					        temp.line = curLine;
					        sprintf( temp.errorM, "format2 syntax error" );
						} // end else
						
					} // end if
					
					else {
						temp.error = true;
					    temp.line = curLine;
					    sprintf( temp.errorM, "format2 syntax error" );
					} // end else
					
				} // end if
				
				else {
					temp.error = true;
					temp.line = curLine;
					sprintf( temp.errorM, "format2 syntax error" );
				} // end else
				
			} // end if
			
			else {
				temp.error = true;
				temp.line = curLine;
				sprintf( temp.errorM, "format2 syntax error" );
			} // end else
			
		} // end if
		
		else {
			temp.error = true;
			temp.line = curLine;
			sprintf( temp.errorM, "format2 yntax error" );
		} // end else
		
	} // end else if
	
	else if ( strcmp( low, "svc" ) == 0 ) {
		if ( tNext != NULL ) {
			if ( tNext->table == 6 ) { // n
				if ( tNext->next == NULL ) {
					temp.error = false;
		            temp.havecode = true;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            temp.format = 2;
		            curLoc = curLoc + 2;
				} // end if
				
				else if ( strcmp( tNext->next->item, "." ) == 0 ) {
					temp.error = false;
		            temp.havecode = true;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            temp.format = 2;
		            curLoc = curLoc + 2;
				} // end else if
				
				else {
					temp.error = true;
					temp.line = curLine;
					sprintf( temp.errorM, "format2 syntax error" );
				} // end else
				
			} // end if
			
			else {
				temp.error = true;
				temp.line = curLine;
				sprintf( temp.errorM, "format2 syntax error" );
			} // end else
			
		} // end if
		
		else {
			temp.error = true;
			temp.line = curLine;
			sprintf( temp.errorM, "format2 syntax error" );
		} // end else
		
	} // end else if
	
	else {
		if ( tNext != NULL ) {
			if ( tNext->table == 3 ) { // r1
				TokenPtr tnn = tNext->next;
				if ( tnn != NULL ) {
					if( strcmp( tnn->item, "," ) == 0 ) { // ,
						if ( tnn->next != NULL ) {
							if ( tnn->next->table == 3 ) { // r2
								if ( tnn->next->next == NULL ) {
									temp.error = false;
		                            temp.havecode = true;
	        	                    temp.line = curLine;
		                            temp.loc = curLoc;
		                            temp.base = false;
		                            temp.format = 2;
		                            curLoc = curLoc + 2;
								} // end if
								
								else if ( strcmp( tnn->next->next->item, "." ) == 0 ) {
									temp.error = false;
		                            temp.havecode = true;
	        	                    temp.line = curLine;
		                            temp.loc = curLoc;
		                            temp.base = false;
		                            temp.format = 2;
		                            curLoc = curLoc + 2;
								} // end else if
								
								else {
									temp.error = true;
					                temp.line = curLine;
					                sprintf( temp.errorM, "format2 syntax error" );
								} // end else
								
							} // end if
							
							else {
								temp.error = true;
					            temp.line = curLine;
					            sprintf( temp.errorM, "format2 syntax error" );
							} // end else
						} // end if
						
						else {
							temp.error = true;
					        temp.line = curLine;
					        sprintf( temp.errorM, "format2 syntax error" );
						} // end else
						
					} // end if
					
					else {
						temp.error = true;
					    temp.line = curLine;
					    sprintf( temp.errorM, "format2 syntax error" );
					} // end else
					
				} // end if
				
				else {
					temp.error = true;
					temp.line = curLine;
					sprintf( temp.errorM, "format2 syntax error" );
				} // end else
				
			} // end if
			
			else {
				temp.error = true;
				temp.line = curLine;
				sprintf( temp.errorM, "format2 syntax error" );
			} // end else
			
		} // end if
		
		else {
			temp.error = true;
			temp.line = curLine;
			sprintf( temp.errorM, "format2 syntax error" );
		} // end else
		
	} // end else
	
} // CheckFormat2()

void SICXE::CheckEqu( LineType & temp, TokenPtr t ) {
	bool label = false;
	bool error = false;
	bool stop = false;
	TokenPtr walk = t;
	while ( ( walk != NULL ) && ( error == false ) && ( stop == false ) ) {
		if ( label == false ) {
			if ( walk->table != 5 ) {
				error = true;
			} // end if
			
			else {
				label = true;
			} // end else
		} // end if
		
		else {
			if ( ( strcmp( walk->item, "+" ) != 0 ) && ( strcmp( walk->item, "-" ) != 0 ) && 
			     ( strcmp( walk->item, "*" ) != 0 ) && strcmp( walk->item, "/" ) != 0 ) {
				if ( strcmp( walk->item, "." ) == 0 ) {
					stop = true;
				} // end if
				
				else {
					error = true;
				} // end else
				
			} // end if
			
			else {
				label = false;
			} // end else
		} // end if
		
		walk = walk->next;
	} // end while
	
	if ( ( error == false ) && ( label == true ) ) {
		temp.error = false;
		temp.havecode = false;
	    temp.line = curLine;
		temp.loc = 0x0;
		temp.base = false;
		temp.equ = false;
	} // end if
	
	else {
		temp.error = true;
		temp.line = curLine;
		sprintf( temp.errorM, "EQU syntax error" );
	} // end else
	
} // CheckEqu()

void SICXE::Equ( LineType & temp, TokenPtr t ) {
	TokenPtr tNext = t->next;
	if ( tNext != NULL ) {
		if ( tNext->table == 6 ) {
			if ( tNext->next == NULL ) {
				temp.error = false;
		        temp.havecode = false;
	        	temp.line = curLine;
		        temp.base = false;
		        int h = 0x0;
		        h = D_To_H( tNext->item );
		        temp.loc = h;
		        temp.equ = true;
			} // end if
			
			else if ( strcmp( tNext->next->item, "." ) == 0 ) {
				temp.error = false;
		        temp.havecode = false;
	        	temp.line = curLine;
		        temp.base = false;
		        int h = 0x0;
		        h = D_To_H( tNext->item );
		        temp.loc = h;
		        temp.equ = true;
			} // end else if
			
			else {
				temp.error = true;
		        temp.line = curLine;
		        sprintf( temp.errorM, "EQU syntax error" );
			} // end else
			
		} // end if
		
		else if ( tNext->table == 5 ) {
			if ( tNext->next == NULL ) {
				int addr = 0x0;
				addr = GetSymbolAddr( tNext->num );
				if ( addr != -1 ) {
					temp.error = false;
		            temp.havecode = false;
	        	    temp.line = curLine;
		            temp.loc = addr;
		            temp.base = false;
		            temp.equ = true;
				} // end if
				
				else { // undefine
					temp.error = false;
		            temp.havecode = false;
	        	    temp.line = curLine;
		            temp.loc = 0x0;
		            temp.base = false;
		            temp.equ = false;
				} // end else
			} // end if
			
			else if ( strcmp( tNext->next->item, "." ) == 0 ) {
				int addr = 0x0;
				addr = GetSymbolAddr( tNext->num );
				if ( addr != -1 ) {
					temp.error = false;
		            temp.havecode = false;
	        	    temp.line = curLine;
		            temp.loc = addr;
		            temp.base = false;
		            temp.equ = true;
				} // end if
				
				else { // undefine
				    temp.error = false;
		            temp.havecode = false;
	        	    temp.line = curLine;
		            temp.loc = 0x0;
		            temp.base = false;
		            temp.equ = false;
				} // end else
				
			} // end else if
			
			else {
				CheckEqu( temp, tNext );
			} // end else
		} // end else if
		
		else if ( strcmp( tNext->item, "*" ) == 0 ) {
			if ( tNext->next == NULL ) {
				temp.error = false;
		        temp.havecode = false;
	        	temp.line = curLine;
		        temp.base = false;
		        temp.loc = curLoc;
		        temp.equ = true;
			} // end if
			
			else if ( strcmp( tNext->next->item, "." ) == 0 ) {
				temp.error = false;
		        temp.havecode = false;
	        	temp.line = curLine;
		        temp.base = false;
		        temp.loc = curLoc;
		        temp.equ = true;
			} // end else if
			
			else {
				temp.error = true;
		        temp.line = curLine;
		        sprintf( temp.errorM, "EQU syntax error" );
			} // end else
			
		} // end else if
		
		else {
			temp.error = true;
		    temp.line = curLine;
		    sprintf( temp.errorM, "EQU syntax error" );
		} // end else
		
	} // end if
	 
	else {
		temp.error = true;
		temp.line = curLine;
		sprintf( temp.errorM, "EQU syntax error" );
	} // end else
} // Equ()

bool Com( LiteralType a, LiteralType b ) {
	return a.num < b.num;
} // Com()

void SICXE::CreatLiteral( LineType & temp ) {
	if ( literBuf.size() > 0 ) {
		sort( literBuf.begin(), literBuf.end(), Com );
		int i = 0;
		while ( i < literBuf.size() ) {
		    line.push_back( temp );
			curLine = curLine + 5;
			index++;
			    
		    temp.line = curLine;
		    temp.havecode = true;
		    temp.error = false;
		    temp.noLoc = false;
		    temp.literal = false;
			temp.extraLiteral = true;
			temp.end = false;
			temp.loc = curLoc;
			temp.litNum = literBuf[i].num;
			
			literTab[literBuf[i].num].addr = curLoc;
			if ( literBuf[i].type == WORD ) {
			    curLoc = curLoc + 3;
			} // end if
				
			else { // BYTE
				int length = 0x0;
				if ( ( literBuf[i].name[0] == 'c' ) || ( literBuf[i].name[0] == 'C' ) ) {
				    length = strlen( literBuf[i].name ) - 1;
				} // end if
					
				else { // X
					length = strlen( literBuf[i].name ) / 2;
				} // end else
					
		        curLoc = curLoc + length;
			} // end else
				
		    	i++;
		} // end while
		
		literBuf.clear();
	} // end if
	
} // CreatLiteral()

void SICXE::Pseudo( LineType & temp, TokenPtr t ) {
	Str100 u;
	strcpy( u, t->item );
	strupr( u );
	if ( strcmp( u, "BYTE" ) == 0 ) {
		TokenPtr tNext = t->next;
		if ( tNext != NULL ) {
			if ( strcmp( tNext->item, "\'" ) == 0 ) {
				if ( tNext->next != NULL ) {
					if ( tNext->next->table == 6 ) { // X
						if ( tNext->next->next != NULL ) {
						    if ( strcmp( tNext->next->next->item, "\'" ) == 0 ) {
						    	if ( tNext->next->next->next == NULL ) {
						    		temp.error = false;
		                            temp.havecode = true;
	        	                    temp.line = curLine;
		                            temp.loc = curLoc;
		                            temp.base = false;
		                            int length = strlen( tNext->next->item ) / 2;
		                            curLoc = curLoc + length;
								} // end if
								
								else if ( strcmp( tNext->next->next->next->item, "." ) == 0 ) {
									temp.error = false;
		                            temp.havecode = true;
	        	                    temp.line = curLine;
		                            temp.loc = curLoc;
		                            temp.base = false;
		                            int length = strlen( tNext->next->item ) / 2;
		                            curLoc = curLoc + length;
								} // end else if
								
								else {
									temp.error = true;
		                            temp.line = curLine;
		                            sprintf( temp.errorM, "BYTE syntax error" );
								} // end else
								
							} // end if
							
							else {
								temp.error = true;
		                        temp.line = curLine;
		                        sprintf( temp.errorM, "BYTE syntax error" );
							} // end else
					    } // end if
						
						else {
							temp.error = true;
		                    temp.line = curLine;
		                    sprintf( temp.errorM, "BYTE syntax error" );
						} // end else
						
					} // end if
					
					else if ( tNext->next->table == 7 ) { // C
		                if ( tNext->next->next != NULL ) {
						    if ( strcmp( tNext->next->next->item, "\'" ) == 0 ) {
						    	if ( tNext->next->next->next == NULL ) {
						    		temp.error = false;
		                            temp.havecode = true;
	        	                    temp.line = curLine;
		                            temp.loc = curLoc;
		                            temp.base = false;
		                            int length = strlen( tNext->next->item ) - 1;
		                            curLoc = curLoc + length;
								} // end if
								
								else if ( strcmp( tNext->next->next->next->item, "." ) == 0 ) {
									temp.error = false;
		                            temp.havecode = true;
	        	                    temp.line = curLine;
		                            temp.loc = curLoc;
		                            temp.base = false;
		                            int length = strlen( tNext->next->item ) - 1;
		                            curLoc = curLoc + length;
								} // end else if
								
								else {
									temp.error = true;
		                            temp.line = curLine;
		                            sprintf( temp.errorM, "BYTE syntax error" );
								} // end else
								
							} // end if
							
							else {
								temp.error = true;
		                        temp.line = curLine;
		                        sprintf( temp.errorM, "BYTE syntax error" );
							} // end else
					    } // end if
						
						else {
							temp.error = true;
		                    temp.line = curLine;
		                    sprintf( temp.errorM, "BYTE syntax error" );
						} // end else
						
					} // end else if
					
					else {
						temp.error = true;
		                temp.line = curLine;
		                sprintf( temp.errorM, "BYTE syntax error" );
					} // end else
					
				} // end if
				
				else {
					temp.error = true;
		            temp.line = curLine;
		            sprintf( temp.errorM, "BYTE syntax error" );
				} // end else
				
			} // end if
			
			else if ( tNext->table == 6 ) {
				if ( tNext->next == NULL ) {
					int h = 0x0;
					h = D_To_H( tNext->item );
					Str100 s;
					sprintf( s, "%x", h );
					int len = strlen( s ) / 2;
					temp.error = false;
		            temp.havecode = true;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            curLoc = curLoc + len;
				} // end if
				
				else if ( strcmp( tNext->next->item, "." ) == 0 ) {
					int h = 0x0;
					h = D_To_H( tNext->item );
					Str100 s;
					sprintf( s, "%x", h );
					int len = strlen( s ) / 2;
					temp.error = false;
		            temp.havecode = true;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            curLoc = curLoc + len;
				} // end else if
				
				else {
					temp.error = true;
		            temp.line = curLine;
		            sprintf( temp.errorM, "BYTE syntax error" );
				} // end else
			} // end else if
			
			else {
				temp.error = true;
		        temp.line = curLine;
		        sprintf( temp.errorM, "BYTE syntax error" );
			} // end else
			
		} // end if
		
		else {
			temp.error = true;
			temp.line = curLine;
			sprintf( temp.errorM, "BYTE syntax error" );
		} // end else
		
	} // end if
	
	else if ( strcmp( u, "WORD" ) == 0 ) {
		TokenPtr tNext = t->next;
		if ( tNext != NULL ) {
			if ( strcmp( tNext->item, "\'" ) == 0 ) {
				if ( tNext->next != NULL ) {
					if ( tNext->next->table == 6 ) { // X
						if ( tNext->next->next != NULL ) {
						    if ( strcmp( tNext->next->next->item, "\'" ) == 0 ) {
						    	if ( tNext->next->next->next == NULL ) {
						    		temp.error = false;
		                            temp.havecode = true;
	        	                    temp.line = curLine;
		                            temp.loc = curLoc;
		                            temp.base = false;
		                            curLoc = curLoc + 3;
								} // end if
								
								else if ( strcmp( tNext->next->next->next->item, "." ) == 0 ) {
									temp.error = false;
		                            temp.havecode = true;
	        	                    temp.line = curLine;
		                            temp.loc = curLoc;
		                            temp.base = false;
		                            curLoc = curLoc + 3;
								} // end else if
								
								else {
									temp.error = true;
		                            temp.line = curLine;
		                            sprintf( temp.errorM, "WORD syntax error" );
								} // end else
								
							} // end if
							
							else {
								temp.error = true;
		                        temp.line = curLine;
		                        sprintf( temp.errorM, "WORD syntax error" );
							} // end else
					    } // end if
						
						else {
							temp.error = true;
		                    temp.line = curLine;
		                    sprintf( temp.errorM, "WORD syntax error" );
						} // end else
						
					} // end if
					
					else if ( tNext->next->table == 7 ) { // C
		                if ( tNext->next->next != NULL ) {
						    if ( strcmp( tNext->next->next->item, "\'" ) == 0 ) {
						    	if ( tNext->next->next->next == NULL ) {
						    		temp.error = false;
		                            temp.havecode = true;
	        	                    temp.line = curLine;
		                            temp.loc = curLoc;
		                            temp.base = false;
		                            curLoc = curLoc + 3;
								} // end if
								
								else if ( strcmp( tNext->next->next->next->item, "." ) == 0 ) {
									temp.error = false;
		                            temp.havecode = true;
	        	                    temp.line = curLine;
		                            temp.loc = curLoc;
		                            temp.base = false;
		                            curLoc = curLoc + 3;
								} // end else if
								
								else {
									temp.error = true;
		                            temp.line = curLine;
		                            sprintf( temp.errorM, "WORD syntax error" );
								} // end else
							} // end if
							
							else {
								temp.error = true;
		                        temp.line = curLine;
		                        sprintf( temp.errorM, "WORD syntax error" );
							} // end else
							
					    } // end if
						
						else {
							temp.error = true;
		                    temp.line = curLine;
		                    sprintf( temp.errorM, "WORD syntax error" );
						} // end else
					} // end else if
					
					else {
						temp.error = true;
		                temp.line = curLine;
		                sprintf( temp.errorM, "WORD syntax error" );
					} // end else
					
				} // end if
				
				else {
					temp.error = true;
		            temp.line = curLine;
		            sprintf( temp.errorM, "WORD syntax error" );
				} // end else
				
			} // end if
			
			else if ( tNext->table == 6 ) {
				if ( tNext->next == NULL ) {
					temp.error = false;
		            temp.havecode = true;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            curLoc = curLoc + 3;
				} // end if
				
				else if ( strcmp( tNext->next->item, "." ) == 0 ) {
					temp.error = false;
		            temp.havecode = true;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            curLoc = curLoc + 3;
				} // end else if
				
				else {
					temp.error = true;
		            temp.line = curLine;
		            sprintf( temp.errorM, "WORD syntax error" );
				} // end else
			} // end else if
			
			else {
				temp.error = true;
		        temp.line = curLine;
		        sprintf( temp.errorM, "WORD syntax error" );
			} // end else
		} // end if
		
		else {
			temp.error = true;
			temp.line = curLine;
			sprintf( temp.errorM, "WORD syntax error" );
		} // end else
		
	} // end else if
	
	else if ( strcmp( u, "RESW" ) == 0 ) {
		TokenPtr tNext = t->next;
		if ( tNext != NULL ) {
			if ( tNext->table == 6 ) {
				if ( tNext->next == NULL ) {
					temp.error = false;
		            temp.havecode = false;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            int h = 0x0;
		            h = D_To_H( tNext->item );
		            curLoc = curLoc + 3 * h;
				} // end if
				
				else if ( strcmp( tNext->next->item, "." ) == 0 ) {
					temp.error = false;
		            temp.havecode = false;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            int h = 0x0;
		            h = D_To_H( tNext->item );
		            curLoc = curLoc + 3 * h;
				} // end else if
				
				else {
					temp.error = true;
		            temp.line = curLine;
		            sprintf( temp.errorM, "RESW syntax error" );
				} // end else
				
			} // end if
			
			else {
				temp.error = true;
		        temp.line = curLine;
		        sprintf( temp.errorM, "RESW syntax error" );
			} // end else
			
		} // end if
		
		else {
			temp.error = true;
		    temp.line = curLine;
		    sprintf( temp.errorM, "RESW syntax error" );
		} // end else
		
	} // end else if
	
	else if ( strcmp( u, "RESB" ) == 0 ) {
		TokenPtr tNext = t->next;
		if ( tNext != NULL ) {
			if ( tNext->table == 6 ) {
				if ( tNext->next == NULL ) {
					temp.error = false;
		            temp.havecode = false;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            int h = 0x0;
		            h = D_To_H( tNext->item );
		            curLoc = curLoc + h;
				} // end if
				
				else if ( strcmp( tNext->next->item, "." ) == 0 ) {
					temp.error = false;
		            temp.havecode = false;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            int h = 0x0;
		            h = D_To_H( tNext->item );
		            curLoc = curLoc + h;
				} // end else if
				
				else {
					temp.error = true;
		            temp.line = curLine;
		            sprintf( temp.errorM, "RESB syntax error" );
				} // end else
				
			} // end if
			
			else {
				temp.error = true;
		        temp.line = curLine;
		        sprintf( temp.errorM, "RESB syntax error" );
			} // end else
			
		} // end if
		
		else {
			temp.error = true;
		    temp.line = curLine;
		    sprintf( temp.errorM, "RESB syntax error" );
		} // end else
		
	} // end else if
	
	else if ( strcmp( u, "END" ) == 0 ) {
		TokenPtr tNext = t->next;
		if ( tNext != NULL ) {
			if ( tNext->table == 5 ) {
				if ( tNext->next == NULL ) {
					temp.error = false;
		            temp.havecode = false;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            temp.end = true;
		            temp.noLoc = true;
		            CreatLiteral( temp );
				} // end if
				
				else if ( strcmp( tNext->next->item, "." ) == 0 ) {
					temp.error = false;
		            temp.havecode = false;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            temp.end = true;
		            temp.noLoc = true;
		            CreatLiteral( temp );
				} // end else if
				
				else {
					temp.error = true;
		            temp.line = curLine;
		            sprintf( temp.errorM, "END syntax error" );
				} // end else
				
			} // end if
			
			else if ( strcmp( tNext->item, "." ) == 0 ) {
				temp.error = false;
		        temp.havecode = false;
	        	temp.line = curLine;
		        temp.loc = curLoc;
		        temp.base = false;
		        temp.end = true;
		        temp.noLoc = true;
		        CreatLiteral( temp );
			} // end else if
			
			else {
				temp.error = true;
		        temp.line = curLine;
		        sprintf( temp.errorM, "END syntax error" );
			} // end else
			
		} // end if
		
		else {
			temp.error = false;
		    temp.havecode = false;
	        temp.line = curLine;
		    temp.loc = curLoc;
		    temp.base = false;
		    temp.end = true;
		    temp.noLoc = true;
		    CreatLiteral( temp );
		} // end else
		
	} // end else if
	
	else if ( strcmp( u, "LTORG" ) == 0 ) {
		if ( t->next == NULL ) {
			temp.error = false;
		    temp.havecode = false;
	        temp.line = curLine;
		    temp.loc = curLoc;
		    temp.base = false;
		    temp.end = false;
		    temp.noLoc = true;
		    CreatLiteral( temp );
		} // end if
		
		else if ( strcmp( t->next->item, "." ) == 0 ) {
			temp.error = false;
		    temp.havecode = false;
	        temp.line = curLine;
		    temp.loc = curLoc;
		    temp.base = false;
		    temp.end = false;
		    temp.noLoc = true;
		    CreatLiteral( temp );
		} // end else if
		
		else {
			temp.error = true;
		    temp.line = curLine;
		    sprintf( temp.errorM, "LTORG syntax error" );
		} // end else
		
	} // end else if
	
	else if ( strcmp( u, "BASE" ) == 0 ) {
		TokenPtr tNext = t->next;
		if ( tNext != NULL ) {
			if ( tNext->table == 5 ) {
				if ( tNext->next == NULL ) {
					temp.error = false;
		            temp.havecode = false;
	                temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            temp.end = false;
		            temp.noLoc = true;
				} // end if
				
				else if ( strcmp( tNext->next->item, "." ) == 0 ) {
					temp.error = false;
		            temp.havecode = false;
	                temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            temp.end = false;
		            temp.noLoc = true;
				} // end else if
				
				else {
					temp.error = true;
		            temp.line = curLine;
		            sprintf( temp.errorM, "BASE syntax error" );
				} // end else
				
			} // end if
			
			else if ( tNext->table == 6 ) {
				if ( tNext->next == NULL ) {
					temp.error = false;
		            temp.havecode = false;
	                temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            temp.end = false;
		            temp.noLoc = true;
				} // end if
				
				else if ( strcmp( tNext->next->item, "." ) == 0 ) {
					temp.error = false;
		            temp.havecode = false;
	                temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            temp.end = false;
		            temp.noLoc = true;
				} // end else if
				
				else {
					temp.error = true;
		            temp.line = curLine;
		            sprintf( temp.errorM, "BASE syntax error" );
				} // end else
				
			} // end else if
			
			else {
				temp.error = true;
		        temp.line = curLine;
		        sprintf( temp.errorM, "BASE syntax error" );
			} // end else
			
		} // end if
		
		else {
			temp.error = true;
		    temp.line = curLine;
		    sprintf( temp.errorM, "BASE syntax error" );
		} // end else
		
	} // end else if
	
	else {
		temp.error = true;
		temp.line = curLine;
		sprintf( temp.errorM, "error pseudo instr" );
	} // end else
	
} // Pseudo()

void SICXE::RealCode( LineType & temp, TokenPtr t ) {
	if ( strcmp( t->item, "." ) == 0 ) {
		temp.error = false;
		temp.havecode = false;
		temp.line = curLine;
		temp.loc = curLoc;
		temp.com = true;
		temp.noLoc = true;
	} // end if
	
	else if ( t->table == 1 ) {
		Str100 low;
		strcpy( low, t->item );
		strlwr(low);
		int f = GetFormat( low );
		
		if ( strcmp( low, "rsub" ) == 0 ) {
			if ( t->next == NULL ) {
				temp.error = false;
		        temp.havecode = true;
	        	temp.line = curLine;
		        temp.loc = curLoc;
		        temp.base = false;
		        temp.format = 3;
		        curLoc = curLoc + 0x3;
			} // end if
			
			else {
				if ( strcmp( t->next->item, "." ) == 0 ) {
					temp.error = false;
		            temp.havecode = true;
	        	    temp.line = curLine;
		            temp.loc = curLoc;
		            temp.base = false;
		            temp.format = 3;
		            curLoc = curLoc + 0x3;
				} // end if
				
				else { // syntax error
					temp.error = true;
					temp.line = curLine;
					sprintf( temp.errorM, "syntax error" );
				} // end else
				
			} // end else
			
		} // end if
		
		else if ( f == 1 ) {
			if ( t->next == NULL ) {
				temp.error = false;
		        temp.havecode = true;
	        	temp.line = curLine;
		        temp.loc = curLoc;
		        temp.base = false;
		        temp.format = 1;
		        curLoc = curLoc + 1;
			} // end if
			
			else if ( strcmp( t->next->item, "." ) == 0 ) {
				temp.error = false;
		        temp.havecode = true;
	        	temp.line = curLine;
		        temp.loc = curLoc;
		        temp.base = false;
		        temp.format = 1;
		        curLoc = curLoc + 1;
			} // end else if
			
			else {
				temp.error = true;
		        temp.line = curLine;
		        sprintf( temp.errorM, "format1 syntax error" );
			} // end else
			
		} // end else if
		
		else if ( f == 2 ) {
			CheckFormat2( temp, t );
		} // end else if
		
		else { // format 3
			Type1( temp, t );
			if ( temp.error == false ) {
				temp.format = 3;
				curLoc = curLoc + 3;
			} // end if
			
		} // end else
		
	} // end else if
	
	else if ( strcmp( t->item, "+" ) == 0 ) {
		TokenPtr tNext = t->next;
		if ( tNext != NULL ) {
			if ( tNext->table == 1 ) { // ins
				Str100 low;
		        strcpy( low, tNext->item );
		        strlwr(low);
		        int f = GetFormat( low );
		        if ( strcmp( low, "rsub" ) == 0 ) {
			        if ( tNext->next == NULL ) {
				        temp.error = false;
		                temp.havecode = true;
	        	        temp.line = curLine;
		                temp.loc = curLoc;
		                temp.base = false;
		                temp.format = 4;
		                curLoc = curLoc + 0x4;
			        } // end if
			
			        else {
				        if ( strcmp( tNext->next->item, "." ) == 0 ) {
					        temp.error = false;
		                    temp.havecode = true;
	        	            temp.line = curLine;
		                    temp.loc = curLoc;
		                    temp.base = false;
		                    temp.format = 4;
		                    curLoc = curLoc + 0x4;
				        } // end if
				
				        else { // syntax error
					        temp.error = true;
					        temp.line = curLine;
					        sprintf( temp.errorM, "format4 syntax error" );
				        } // end else
				        
			        } // end else
			    
		        } // end if
		    
		        else if ( f == 0 ) {
		        	Type1( temp, tNext );
			        if ( temp.error == false ) {
				        curLoc = curLoc + 4;
				        temp.format = 4;
			        } // end if
			        
				} // end else if
				
				else {
					temp.error = true;
		            temp.line = curLine;
		            sprintf( temp.errorM, "format4 syntax error" );
				} // end else
				
			} // end if
		    
		    else {
		    	temp.error = true;
		        temp.line = curLine;
		        sprintf( temp.errorM, "syntax error" );
			} // end else
		    
		} // end if
		
		else {
			temp.error = true;
		    temp.line = curLine;
		    sprintf( temp.errorM, "syntax error" );
		} // end else
		
	} // end else if
	
	else if ( t->table == 2 ) {
		Pseudo( temp, t );
	} // end else if
	
	else {
		temp.error = true;
		temp.line = curLine;
		sprintf( temp.errorM, "syntax error" );
	} // end else
	
} // RealCode()

void SICXE::CreatLine( LineType & temp ) {
	if ( cur != NULL ) {
		if ( cur->table == 5 ) {
			TokenPtr nextToken = cur->next;
			if ( nextToken != NULL ) {
				Str100 u;
				strcpy( u, nextToken->item );
				strupr( u );
				if ( strcmp( nextToken->item, "." ) == 0 ) { // code null
					temp.line = curLine;
				    temp.error = true;
				    sprintf( temp.errorM, "code cannot be null" );
				} //end if
				
				else if ( strcmp( u, "EQU" ) == 0 ) {
					Equ( temp, nextToken );
					if ( temp.equ == true ) {
						symtab[cur->num].addr = temp.loc;
					} // end if
				} // end else if
				
				else {
					RealCode( temp, nextToken );
				} // end else
			} // end if
			
			else {
				temp.line = curLine;
			    temp.error = true;
			    sprintf( temp.errorM, "code cannot be null" );
			} // end else
			
			if ( temp.error == false ) {
				bool du = InsertSymbol( cur->item, temp.loc, cur->num );
				if ( du == false ) {
					temp.line = curLine;
			        temp.error = true;
			        sprintf( temp.errorM, "duplicate symbol" );
				} // end if
				
			} // end if
			
		} // end if
		
		else {
			RealCode( temp, cur );
		} // end else
		
	} // end if
	
} // CreatLine()

void SICXE::Allocate( LineType & temp, TokenPtr nextToken ) {
	Str100 u;
	strcpy( u, nextToken->item );
	strupr( u );
	if ( strcmp( u, "START" ) == 0 ) {
		TokenPtr nnToken = nextToken->next;
		if ( nnToken != NULL ) {
		    if ( nnToken->table == 6 ) {
				if ( nnToken->next == NULL ) {
					sscanf( nnToken->item, "%x", &curLoc );
					temp.error = false;
					temp.havecode = false;
					temp.line = curLine;
					temp.loc = curLoc;
				} // end if
							
				else {
					if ( strcmp( nnToken->next->item, "." ) == 0 ) { // comment
						sscanf( nnToken->item, "%x", &curLoc );
						temp.error = false;
						temp.havecode = false;
						temp.line = curLine;
						temp.loc = curLoc;
					} // end if
								
					else { // START syntax error
						temp.line = curLine;
						temp.error = true;
						sprintf( temp.errorM, "START syntax error" );
					} // end else
				} // end else
			} // end if
						
			else { // START syntax error
				temp.line = curLine;
				temp.error = true;
				sprintf( temp.errorM, "START syntax error" );
			} // end else
		} // end if
					
		else { // START syntax error
			temp.line = curLine;
			temp.error = true;
			sprintf( temp.errorM, "START syntax error" );
		} // end else
		
	} // end if
				
	else {
		curLoc = 0x0000;
		CreatLine( temp );
	} // end else
} // Allocate()

void SICXE::FirstLine() {
	if ( cur != NULL ) {
		LineType temp;
		temp.com = false;
		temp.end = false;
		temp.equ = false;
		temp.noLoc = false;
		temp.literal = false;
		temp.extraLiteral = false;
		temp.error = true;
		
		if ( cur->table == 5 ) { // Symble
			TokenPtr nextToken = cur->next;
			if ( nextToken != NULL ) {
				if ( strcmp( nextToken->item, "." ) == 0 ) { // code null
					temp.line = curLine;
				    temp.error = true;
				    sprintf( temp.errorM, "code cannot be null" );
				} //end if
				
				else {
					Allocate( temp, nextToken );
				} // end else
				
			} // end if
			
			else { // code null
				temp.line = curLine;
				temp.error = true;
				sprintf( temp.errorM, "code cannat be null" );
			} // end else
			
			if ( temp.error == false ) {
				if ( InsertSymbol( cur->item, curLoc, cur->num ) == false ) { // duplicate
					temp.error = true;
					sprintf( temp.errorM, "duplicate symbol" );
				} // end if
			} // end if
			
		} // end if
		
		else {
			Allocate( temp, cur );
		} // end else
		
		line.push_back( temp );
		index++;
	} // end if
	
	else {
		printf( "Empty File\n" );
	} // end else
	
} // FirstLine()

void SICXE::Pass1( HeadTokenPtr curHead ) {
	FirstLine();
	curHead = curHead->nextHead;
	while ( curHead != NULL ) {
		curLine = curLine + 5;
		LineType temp;
		temp.com = false;
		temp.end = false;
		temp.equ = false;
		temp.noLoc = false;
		temp.literal = false;
		temp.extraLiteral = false;
		temp.error = true;
		
		cur = curHead->token;
		CreatLine( temp );
		line.push_back( temp );
		index++;
		curHead = curHead->nextHead;
	} // end while
	
} // Pass1()

int SICXE::GetOp( Str100 s ) {
	Str100 l;
	strcpy( l, s );
	strlwr( l );
	int ans = 0x0;
	if ( strcmp( l, "add" ) == 0 ) {
		ans = 0x18;
	} // end if
	
	else if ( strcmp( l, "addf" ) == 0 ) {
		ans = 0x58;
	} // end else if
	
	else if ( strcmp( l, "addr" ) == 0 ) {
		ans = 0x90;
	} // end else if
	
	else if ( strcmp( l, "and" ) == 0 ) {
		ans = 0x40;
	} // end else if
	
	else if ( strcmp( l, "clear" ) == 0 ) {
		ans = 0xb4;
	} // end else if
	
	else if ( strcmp( l, "comp" ) == 0 ) {
		ans = 0x28;
	} // end else if
	
	else if ( strcmp( l, "compf" ) == 0 ) {
		ans = 0x88;
	} // end else if
	
	else if ( strcmp( l, "compr" ) == 0 ) {
		ans = 0xa0;
	} // end else if
	
	else if ( strcmp( l, "div" ) == 0 ) {
		ans = 0x24;
	} // end else if
	
	else if ( strcmp( l, "divf" ) == 0 ) {
		ans = 0x64;
	} // end else if
	
	else if ( strcmp( l, "divr" ) == 0 ) {
		ans = 0x9c;
	} // end else if
	
	else if ( strcmp( l, "fix" ) == 0 ) {
		ans = 0xc4;
	} // end else if
	
	else if ( strcmp( l, "float" ) == 0 ) {
		ans = 0xc0;
	} // end else if
	
	else if ( strcmp( l, "hio" ) == 0 ) {
		ans = 0xf4;
	} // end else if
	
	else if ( strcmp( l, "j" ) == 0 ) {
		ans = 0x3c;
	} // end else if
	
	else if ( strcmp( l, "jeq" ) == 0 ) {
		ans = 0x30;
	} // end else if
	
	else if ( strcmp( l, "jgt" ) == 0 ) {
		ans = 0x34;
	} // end else if
	
	else if ( strcmp( l, "jlt" ) == 0 ) {
		ans = 0x38;
	} // end else if
	
	else if ( strcmp( l, "jsub" ) == 0 ) {
		ans = 0x48;
	} // end else if
	
	else if ( strcmp( l, "lda" ) == 0 ) {
		ans = 0x00;
	} // end else if
	
	else if ( strcmp( l, "ldb" ) == 0 ) {
		ans = 0x68;
	} // end else if
	
	else if ( strcmp( l, "ldch" ) == 0 ) {
		ans = 0x50;
	} // end else if
	
	else if ( strcmp( l, "ldf" ) == 0 ) {
		ans = 0x70;
	} // end else if
	
	else if ( strcmp( l, "ldl" ) == 0 ) {
		ans = 0x08;
	} // end else if
	
	else if ( strcmp( l, "lds" ) == 0 ) {
		ans = 0x6c;
	} // end else if
	
	else if ( strcmp( l, "ldt" ) == 0 ) {
		ans = 0x74;
	} // end else if
	
	else if ( strcmp( l, "ldx" ) == 0 ) {
		ans = 0x04;
	} // end else if
	
	else if ( strcmp( l, "lps" ) == 0 ) {
		ans = 0xd0;
	} // end else if
	
	else if ( strcmp( l, "mul" ) == 0 ) {
		ans = 0x20;
	} // end else if
	
	else if ( strcmp( l, "mulf" ) == 0 ) {
		ans = 0x60;
	} // end else if
	
	else if ( strcmp( l, "mulr" ) == 0 ) {
		ans = 0x98;
	} // end else if
	
	else if ( strcmp( l, "norm" ) == 0 ) {
		ans = 0xc8;
	} // end else if
	
	else if ( strcmp( l, "or" ) == 0 ) {
		ans = 0x44;
	} // end else if
	
	else if ( strcmp( l, "rd" ) == 0 ) {
		ans = 0xd8;
	} // end else if
	
	else if ( strcmp( l, "rmo" ) == 0 ) {
		ans = 0xac;
	} // end else if
	
	else if ( strcmp( l, "rsub" ) == 0 ) {
		ans = 0x4c;
	} // end else if
	
	else if ( strcmp( l, "shiftl" ) == 0 ) {
		ans = 0xa4;
	} // end else if
	
	else if ( strcmp( l, "shiftr" ) == 0 ) {
		ans = 0xa8;
	} // end else if
	
	else if ( strcmp( l, "sio" ) == 0 ) {
		ans = 0xf0;
	} // end else if
	
	else if ( strcmp( l, "ssk" ) == 0 ) {
		ans = 0xec;
	} // end else if
	
	else if ( strcmp( l, "sta" ) == 0 ) {
		ans = 0x0c;
	} // end else if
	
	else if ( strcmp( l, "stb" ) == 0 ) {
		ans = 0x78;
	} // end else if
	
	else if ( strcmp( l, "stch" ) == 0 ) {
		ans = 0x54;
	} // end else if
	
	else if ( strcmp( l, "stf" ) == 0 ) {
		ans = 0x80;
	} // end else if
	
	else if ( strcmp( l, "sti" ) == 0 ) {
		ans = 0xd4;
	} // end else if
	
	else if ( strcmp( l, "stl" ) == 0 ) {
		ans = 0x14;
	} // end else if
	
	else if ( strcmp( l, "sts" ) == 0 ) {
		ans = 0x7c;
	} // end else if
	
	else if ( strcmp( l, "stsw" ) == 0 ) {
		ans = 0xe8;
	} // end else if
	
	else if ( strcmp( l, "stt" ) == 0 ) {
		ans = 0x84;
	} // end else if
	
	else if ( strcmp( l, "stx" ) == 0 ) {
		ans = 0x10;
	} // end else if
	
	else if ( strcmp( l, "sub" ) == 0 ) {
		ans = 0x1c;
	} // end else if
	
	else if ( strcmp( l, "subf" ) == 0 ) {
		ans = 0x5c;
	} // end else if
	
	else if ( strcmp( l, "subr" ) == 0 ) {
		ans = 0x94;
	} // end else if
	
	else if ( strcmp( l, "svc" ) == 0 ) {
		ans = 0xb0;
	} // end else if
	
	else if ( strcmp( l, "td" ) == 0 ) {
		ans = 0xe0;
	} // end else if
	
	else if ( strcmp( l, "tio" ) == 0 ) {
		ans = 0xf8;
	} // end else if
	
	else if ( strcmp( l, "tix" ) == 0 ) {
		ans = 0x2c;
	} // end else if
	
	else if ( strcmp( l, "tixr" ) == 0 ) {
		ans = 0xb8;
	} // end else if
	
	else if ( strcmp( l, "wd" ) == 0 ) {
		ans = 0xdc;
	} // end else if
	
	return ans;
} // GetOp()

int SICXE::GetR( Str100 r ) {
	Str100 u;
	strcpy( u, r );
	strupr(u);
	if ( strcmp( u, "A" ) == 0 ) {
		return 0;
	} // end if
	
	else if ( strcmp( u, "X" ) == 0 ) {
		return 1;
	} // end else if
	
	else if ( strcmp( u, "L" ) == 0 ) {
		return 2;
	} // end else if
	
	else if ( strcmp( u, "B" ) == 0 ) {
		return 3;
	} // end else if
	
	else if ( strcmp( u, "S" ) == 0 ) {
		return 4;
	} // end else if
	
	else if ( strcmp( u, "T" ) == 0 ) {
		return 5;
	} // end else if
	
	else if ( strcmp( u, "F" ) == 0 ) {
		return 6;
	} // end else if
	
	else if ( strcmp( u, "PC" ) == 0 ) {
		return 8;
	} // end else if
	
	else if ( strcmp( u, "X" ) == 0 ) {
		return 9;
	} // end else if
	
	else {
		return 7;
	} // end else
	
} // GetR()

void SICXE::Format3Obj( TokenPtr t ) {
	Str100 l;
	strcpy( l, t->item );
	strlwr( l );
	if ( strcmp( l, "rsub" ) == 0 ) {
		int op = 0x0;
		op = GetOp( t->item );
		sprintf( line[curIndex].objcode, "%02X0000", op );
	} // end if
	
	else {
		TokenPtr tNext = t->next;
		if ( tNext != NULL ) {
			if ( tNext->table == 5 ) {
				int addr = 0x0;
			    int op = 0x0;
			    addr = GetSymbolAddr( tNext->num );
			    if ( addr != -1 ) {
				    op = GetOp( t->item );
				    int disp;
				    int pc = line[curIndex].loc + 3;
				    int d = addr - pc;
				    if ( ( d < -2048 ) || ( d > 2047 ) ) { // use base reg
				    	if ( baseReg == -1 ) {
				    		line[curIndex].error = true;
				    		sprintf( line[curIndex].errorM, "didn\'t set base register" );
				    		
						} // end if
							
						else {
							disp = addr - baseReg;
							if ( ( disp < 4096 ) && ( disp >= 0 ) ) {
								int bin = 0b0;
							    int sicxe ;
							    if ( line[curIndex].base == true ) {
								    sicxe = 0b111;
							    } // end if
							
							    else {
								    sicxe = 0b110;
							    } // end else
							    
							    bin = ( op << 16 ) | ( sicxe << 15 ) | ( 1 << 14 ) | disp;
							    sprintf( line[curIndex].objcode, "%06X", bin );
							    
							} // end if
							
							else { // overflow
								line[curIndex].error = true;
								sprintf( line[curIndex].errorM, "disp using base register overflow" );
							} // end else
							
						} // end else
							
					} // end if
					    
					else { // use pc
					    disp = addr - pc;
					    int bin = 0b0;
					    int sicxe = 0b111;
					    if ( line[curIndex].base == true ) {
							sicxe = 0b111010;
						} // end if
							
						else {
							sicxe = 0b110010;
						} // end else
					    
					    disp = disp & 0xfff;
						bin = ( op << 16 ) | ( sicxe << 12 ) | disp;
						sprintf( line[curIndex].objcode, "%06X", bin );
					} // end else
				    
			    } // end if
			
			    else { // undefine
				    line[curIndex].error = true;
				    sprintf( line[curIndex].errorM, "undefine symbol" );
			    } // end else
			    
			} // end if
			
			else if ( tNext->table == 6 ) {
				int op = 0x0;
				int disp = D_To_H( tNext->item );
				if ( disp < 4096 ) {
					op = GetOp( t->item );
					int bin = 0b0;
					int sicxe = 0b110;
					bin = ( op << 16 ) | ( sicxe << 15 ) | disp;
					sprintf( line[curIndex].objcode, "%06X", bin );
				} // end if
						
				else { // overflow
					line[curIndex].error = true;
					sprintf( line[curIndex].errorM, "address disp overflow" );
				} // end else
				
			} // end else if
			
			else if ( strcmp( tNext->item, "#" ) == 0 ) {
				if ( tNext->next != NULL ) {
					if ( tNext->next->table == 5 ) {
						int addr = 0x0;
			            int op = 0x0;
			            addr = GetSymbolAddr( tNext->next->num );
			            if ( addr != -1 ) {
			            	op = GetOp( t->item );
			            	int disp;
				    	    int pc = line[curIndex].loc + 3;
				    	    int d = addr - pc;
				    	    if ( ( d < -2048 ) || ( d > 2047 ) ) { // use base reg
				    		    if ( baseReg == -1 ) {
				    			    line[curIndex].error = true;
				    			    sprintf( line[curIndex].errorM, "didn\'t set base register" );
							    } // end if
							
							    else {
								    disp = addr - baseReg;
								    if ( ( disp < 4096 ) && ( disp >= 0 ) ) {
								    	int bin = 0b0;
								        int sicxe = 0b010;
								        bin = ( op << 16 ) | ( sicxe << 15 ) | ( 1 << 14 ) | disp;
								        sprintf( line[curIndex].objcode, "%06X", bin );
									} // end if
								    
								    else {
								    	line[curIndex].error = true;
								    	sprintf( line[curIndex].errorM, "disp using base register overflow" );
									} // end else
									
							    } // end else
							
						    } // end if
					    
					        else { // use pc
					        	disp = addr - pc;
					    	    int bin = 0b0;
					    	    int sicxe = 0b010;
					    	    disp = disp & 0xfff;
							    bin = ( op << 16 ) | ( sicxe << 15 ) | ( 1 << 13 ) | disp;
							    sprintf( line[curIndex].objcode, "%06X", bin );
					    	
						    } // end else
			            	
						} // end if
						
						else { // undefine
							line[curIndex].error = true;
							sprintf( line[curIndex].errorM, "undefine symbol" );
						} // end else
			            
					} // end if
					
					else if ( tNext->next->table == 6 ) {
						int op = 0x0;
						int disp = D_To_H( tNext->next->item );
						if ( disp < 4096 ) {
							op = GetOp( t->item );
							int bin = 0b0;
							bin = ( op << 16 ) | ( 1 << 16 ) | disp;
							sprintf( line[curIndex].objcode, "%06X", bin );
						} // end if
						
						else { // overflow
							line[curIndex].error = true;
							sprintf( line[curIndex].errorM, "immediate overflow" );
						} // end else
						
					} // end else if
					
				} // end if
				
			} // end else if
			
			else if ( strcmp( tNext->item, "@" ) == 0 ) {
				if ( tNext->next != NULL ) {
					if ( tNext->next->table == 5 ) {
						int addr = 0x0;
			            int op = 0x0;
			            addr = GetSymbolAddr( tNext->next->num );
			            if ( addr != -1 ) {
			            	op = GetOp( t->item );
			            	int disp;
				    	    int pc = line[curIndex].loc + 3;
				    	    int d = addr - pc;
				    	    if ( ( d < -2048 ) || ( d > 2047 ) ) { // use base reg
				    		    if ( baseReg == -1 ) {
				    			    line[curIndex].error = true;
				    			    sprintf( line[curIndex].errorM, "didn\'t set base register" );
							    } // end if
							
							    else {
								    disp = addr - baseReg;
								    if ( ( disp < 4096 ) && ( disp >= 0 ) ) {
								    	int bin = 0b0;
								        int sicxe = 0b100;
								        bin = ( op << 16 ) | ( sicxe << 15 ) | ( 1 << 14 ) | disp;
								        sprintf( line[curIndex].objcode, "%06X", bin );
									} // end if
								    
								    else {
								    	line[curIndex].error = true;
								    	sprintf( line[curIndex].errorM, "disp using base register overflow" );
									} // end else
									
							    } // end else
							
						    } // end if
					    
					        else { // use pc
					        	disp = addr - pc;
					    	    int bin = 0b0;
					    	    int sicxe = 0b100;
					    	    disp = disp & 0xfff;
							    bin = ( op << 16 ) | ( sicxe << 15 ) | ( 1 << 13 ) | disp;
							    sprintf( line[curIndex].objcode, "%06X", bin );
					    	
						    } // end else
			            	
						} // end if
						
						else { // undefine
							line[curIndex].error = true;
							sprintf( line[curIndex].errorM, "undefine symbol" );
						} // end else
			            
					} // end if
					
				} // end if
				
				
			} // end else if
			
			else if ( strcmp( tNext->item, "=" ) == 0 ) {
				if ( tNext->next != NULL ) { // ' or dec
                    if ( tNext->next->table == 6 ) { // dec
                    	int addr = 0x0;
			            int op = 0x0;
			            addr = literTab[line[curIndex].litNum].addr;
			            if ( addr != -1 ) {
			            	op = GetOp( t->item );
			            	int disp;
				    	    int pc = line[curIndex].loc + 3;
				    	    int d = addr - pc;
				    	    if ( ( d < -2048 ) || ( d > 2047 ) ) { // use base reg
				    		    if ( baseReg == -1 ) {
				    			    line[curIndex].error = true;
				    			    sprintf( line[curIndex].errorM, "didn\'t set base register" );
							    } // end if
							
							    else {
								    disp = addr - baseReg;
								    if ( ( disp < 4096 ) && ( disp >= 0 ) ) {
								    	int bin = 0b0;
								        int sicxe = 0b110;
								        bin = ( op << 16 ) | ( sicxe << 15 ) | ( 1 << 14 ) | disp;
								        sprintf( line[curIndex].objcode, "%06X", bin );
									} // end if
									
								    else {
								    	line[curIndex].error = true;
								    	sprintf( line[curIndex].errorM, "disp using base register overflow" );
									} // end else
									
							    } // end else
							
						    } // end if
					    
					        else { // use pc
					        	disp = addr - pc;
					    	    int bin = 0b0;
					    	    int sicxe = 0b110;
					    	    disp = disp & 0xfff;
							    bin = ( op << 16 ) | ( sicxe << 15 ) | ( 1 << 13 ) | disp;
							    sprintf( line[curIndex].objcode, "%06X", bin );
					    	
						    } // end else
						    
						} // end if
						    
					} // end if
					
					else if ( tNext->next->next != NULL ) { // X or C 
						int addr = 0x0;
			            int op = 0x0;
			            addr = literTab[line[curIndex].litNum].addr;
			            if ( addr != -1 ) {
			            	op = GetOp( t->item );
			            	int disp;
				    	    int pc = line[curIndex].loc + 3;
				    	    int d = addr - pc;
				    	    if ( ( d < -2048 ) || ( d > 2047 ) ) { // use base reg
				    		    if ( baseReg == -1 ) {
				    			    line[curIndex].error = true;
				    			    sprintf( line[curIndex].errorM, "didn\'t set base register" );
							    } // end if
							
							    else {
								    disp = addr - baseReg;
								    if ( ( disp < 4096 ) && ( disp >= 0 ) ) {
								    	int bin = 0b0;
								        int sicxe = 0b110;
								        bin = ( op << 16 ) | ( sicxe << 15 ) | ( 1 << 14 ) | disp;
								        sprintf( line[curIndex].objcode, "%06X", bin );
									} // end if
									
									else {
								    	line[curIndex].error = true;
								    	sprintf( line[curIndex].errorM, "disp using base register overflow" );
									} // end else
									
							    } // end else
							
						    } // end if
					    
					        else { // use pc
					        	disp = addr - pc;
					    	    int bin = 0b0;
					    	    int sicxe = 0b110;
					    	    disp = disp & 0xfff;
							    bin = ( op << 16 ) | ( sicxe << 15 ) | ( 1 << 13 ) | disp;
							    sprintf( line[curIndex].objcode, "%06X", bin );
					    	
						    } // end else
						    
						} // end if
			            
					} // end else if
					
				} // end if
				
			} // end else if
			
		} // end if
		
	} // end else
	
} // Format3Obj

void SICXE::Format4Obj( TokenPtr t ) {
	Str100 l;
	strcpy( l, t->item );
	strlwr( l );
	if ( strcmp( l, "rsub" ) == 0 ) {
		int op = 0x0;
		op = GetOp( t->item );
		sprintf( line[curIndex].objcode, "%02X000000", op );
	} // end if
	
	else {
		TokenPtr tNext = t->next;
		if ( tNext != NULL ) {
			if ( tNext->table == 5 ) {
				int addr = 0x0;
			    int op = 0x0;
			    addr = GetSymbolAddr( tNext->num );
			    if ( addr != -1 ) {
				    op = GetOp( t->item );
				    if ( addr <= 0xfffff ) { 
						int bin = 0b0;
						int sicxe ;
						if ( line[curIndex].base == true ) {
							sicxe = 0b111001;
						} // end if
						
						else {
							sicxe = 0b110001;
						} // end else
						
						bin = ( op << 24 ) | ( sicxe << 20 ) | addr;
						sprintf( line[curIndex].objcode, "%08X", bin );
							
					} // end if
					    
					else { // overflow
					    line[curIndex].error = true;
					    sprintf( line[curIndex].errorM, "address overflow" );
					} // end else
				    
			    } // end if
			
			    else { // undefine
				    line[curIndex].error = true;
				    sprintf( line[curIndex].errorM, "undefine symbol" );
			    } // end else
			    
			} // end if
			
			else if ( tNext->table == 6 ) {
				int op = 0x0;
				int addr = D_To_H( tNext->item );
				if ( addr <= 0xfffff ) {
					op = GetOp( t->item );
					int bin = 0b0;
					int sicxe = 0b110001;
					bin = ( op << 24 ) | ( sicxe << 20 ) | addr;
					sprintf( line[curIndex].objcode, "%08X", bin );
				} // end if
						
				else { // overflow
					line[curIndex].error = true;
					sprintf( line[curIndex].errorM, "address overflow" );
				} // end else
				
			} // end else if
			
			else if ( strcmp( tNext->item, "#" ) == 0 ) {
				if ( tNext->next != NULL ) {
					if ( tNext->next->table == 5 ) {
						int addr = 0x0;
			            int op = 0x0;
			            addr = GetSymbolAddr( tNext->next->num );
			            if ( addr != -1 ) {
			            	if ( addr <= 0xfffff ) {
					            op = GetOp( t->item );
					            int bin = 0b0;
					            int sicxe = 0b010001;
					            bin = ( op << 24 ) | ( sicxe << 20 ) | addr;
					            sprintf( line[curIndex].objcode, "%08X", bin );
				            } // end if
			            	
			            	else { // overflow
			            		line[curIndex].error = true;
			            		sprintf( line[curIndex].errorM, "immediate address overflow" );
							} // end else
			            	
						} // end if
						
						else { // undefine
							line[curIndex].error = true;
							sprintf( line[curIndex].errorM, "undefine symbol" );
						} // end else
			            
					} // end if
					
					else if ( tNext->next->table == 6 ) {
						int op = 0x0;
						int addr = D_To_H( tNext->next->item );
						if ( addr <= 0xfffff ) {
							op = GetOp( t->item );
							int bin = 0b0;
							int sicxe = 0b010001;
					        bin = ( op << 24 ) | ( sicxe << 20 ) | addr;
					        sprintf( line[curIndex].objcode, "%08X", bin );
						} // end if
						
						else { // overflow
							line[curIndex].error = true;
							sprintf( line[curIndex].errorM, "immediate address overflow" );
						} // end else
						
					} // end else if
					
				} // end if
				
			} // end else if
			
			else if ( strcmp( tNext->item, "@" ) == 0 ) {
				if ( tNext->next != NULL ) {
					if ( tNext->next->table == 5 ) {
						int addr = 0x0;
			            int op = 0x0;
			            addr = GetSymbolAddr( tNext->next->num );
			            if ( addr != -1 ) {
			            	op = GetOp( t->item );
				    	    if ( addr <= 0xfffff ) {
				    		    int bin = 0b0;
								int sicxe = 0b100001;
								bin = ( op << 24 ) | ( sicxe << 20 ) | addr;
								sprintf( line[curIndex].objcode, "%08X", bin );
							
						    } // end if
					    
					        else { // overflow
					    	    line[curIndex].error = true;
					    	    sprintf( line[curIndex].errorM, "indirect address overflow" );
						    } // end else
			            	
						} // end if
						
						else { // undefine
							line[curIndex].error = true;
							sprintf( line[curIndex].errorM, "undefine symbol" );
						} // end else
			            
					} // end if
					
				} // end if
				
				
			} // end else if
			
			else if ( strcmp( tNext->item, "=" ) == 0 ) {
				if ( tNext->next != NULL ) { // ' or dec
                    if ( tNext->next->table == 6 ) { // dec
                    	int addr = 0x0;
			            int op = 0x0;
			            addr = literTab[line[curIndex].litNum].addr;
			            if ( addr != -1 ) {
			            	op = GetOp( t->item );
				    	    if ( addr <= 0xfffff ) { 
				    		    int bin = 0b0;
								int sicxe = 0b110001;
								bin = ( op << 24 ) | ( sicxe << 20 ) | addr;
								sprintf( line[curIndex].objcode, "%08X", bin );
							
						    } // end if
					    
					        else { // overflow
					        	line[curIndex].error = true;
								sprintf( line[curIndex].errorM, "literal address overflow" );	
						    } // end else
						    
						} // end if
					} // end if
					
					else if ( tNext->next->next != NULL ) { // X or C 
						int addr = 0x0;
			            int op = 0x0;
			            addr = literTab[line[curIndex].litNum].addr;
			            if ( addr != -1 ) {
			            	op = GetOp( t->item );
				    	    if ( addr <= 0xfffff ) { 
				    		    int bin = 0b0;
								int sicxe = 0b110001;
								bin = ( op << 24 ) | ( sicxe << 20 ) | addr;
								sprintf( line[curIndex].objcode, "%08X", bin );
							
						    } // end if
					    
					        else { // overflow
					        	line[curIndex].error = true;
					        	sprintf( line[curIndex].errorM, "literal address overflow" );
						    } // end else
						    
						} // end if
			            
					} // end else if
					
				} // end if
				
			} // end else if
			
		} // end if
		
	} // end else
	
} // void Format4Obj()

void SICXE::FillObjcode( TokenPtr t ) {
	if ( t->table == 1 ) { // format1 2 3
		TokenPtr tNext = t->next;
		if ( line[curIndex].format == 1 ) {
			int op = 0x0;
			op = GetOp( t->item );
			sprintf( line[curIndex].objcode, "%02X", op );
		} // end if
		
		else if ( line[curIndex].format == 2 ) {
			int op = 0x0;
			op = GetOp( t->item );
			Str100 l;
			strcpy( l, t->item );
			strlwr( l );
			if ( ( strcmp( l, "clear" ) == 0 ) || ( strcmp( l, "tixr" ) == 0 ) ) {
				int r1 = 0x0;
				if ( tNext != NULL ) {
					r1 = GetR( tNext->item );
				} // end if
				
				sprintf( line[curIndex].objcode, "%02X%X0", op, r1 );
			} // end if
			
			else if ( ( strcmp( l, "shiftl" ) == 0 ) || ( strcmp( l, "shiftr" ) == 0 ) ) {
				int r1 = 0x0;
				int n = 0x0;
				if ( tNext != NULL ) { // r1
					r1 = GetR( tNext->item );
					if ( tNext->next != NULL ) { // ,
						if ( tNext->next->next != NULL ) { // n
							n = D_To_H( tNext->next->next->item );
							if ( n > 16 ) {
								line[curIndex].error = true;
								sprintf( line[curIndex].errorM, "n overflow( n indicates an int between 1 and 16 )" );
							} // end if
							
							else {
								sprintf( line[curIndex].objcode, "%02X%X%X", op, r1, n );
							} // end else
							
						} // end if
						
					} // end if
					
				} // end if
				
			} // end else if
			
			else if ( strcmp( l, "svc" ) == 0 ) {
				int n = 0x0;
				if ( tNext != NULL ) {
					n = D_To_H( tNext->item );
				} // end if
				
				sprintf( line[curIndex].objcode, "%02X%X0", op, n );
			} // end else if
			
			else {
				int r1 = 0x0;
				int r2 = 0x0;
				if ( tNext != NULL ) { // r1
					r1 = GetR( tNext->item );
					if ( tNext->next != NULL ) { // ,
						if ( tNext->next->next != NULL ) { // r2
							r2 = GetR( tNext->next->next->item );
							sprintf( line[curIndex].objcode, "%02X%X%X", op, r1, r2 );
						} // end if
						
					} // end if
					
				} // end if
			} // end else
			
		} // end else if
		
		else {
			Format3Obj( t );

		} // end else
		
	} // end if
	
	else if ( strcmp( t->item, "+" ) == 0 ) { // format4
		if ( t->next != NULL ) {
			Format4Obj( t->next );
		} // end if
		
	} // end else if
	
	else if ( t->table == 2 ) {
		Str100 u;
		strcpy( u, t->item );
		strupr(u);
		if ( strcmp( u, "BYTE" ) == 0 ) {
			if ( t->next != NULL ) { // ' or dec
			    if ( t->next->table == 6 ) {
					int o = 0x0;
					o = D_To_H( t->next->item );
					sprintf( line[curIndex].objcode, "%X", o );
				} // end if
				
				else if ( t->next->next != NULL ) {
					TokenPtr tnn = t->next->next;
					if ( tnn->table == 7 ) {
						int i = 2;
						Str100 buf;
						if ( tnn->item[1] != '\0' ) {
							sprintf( buf, "%02X", tnn->item[1] );
						} // end if
						
						while ( tnn->item[i] != '\0' ) {
							sprintf( buf, "%s%02X", buf, tnn->item[i] );
							i++;
						} // end while
						
						strcpy( line[curIndex].objcode, buf );
					} // end if
					
					else if ( tnn->table == 6 ) {
						int i = 1;
						while ( tnn->item[i] != '\0' ) {
							line[curIndex].objcode[i-1]  = tnn->item[i];
							i++;
						} // end while
						
						line[curIndex].objcode[i-1] = '\0';
					} // end else if
					
				} // end else if
				
			} // end if
		} // end if
		
		else if ( strcmp( u, "WORD" ) == 0 ) {
			if ( t->next != NULL ) { // ' or dec
			    if ( t->next->table == 6 ) {
					int len = strlen( t->next->item );
					if ( len > 6 ) {
						line[curIndex].error = true;
						sprintf( line[curIndex].errorM, "WORD overflow" );
					} // end if
					
					else {
						int o = 0x0;
					    o = D_To_H( t->next->item );
					    sprintf( line[curIndex].objcode, "%06X", o );
					} // end else
					
				} // end if
				
				else if ( t->next->next != NULL ) {
					TokenPtr tnn = t->next->next;
					if ( tnn->table == 7 ) {
						int i = 2;
						Str100 buf;
						if ( tnn->item[1] != '\0' ) {
							sprintf( buf, "%02X", tnn->item[1] );
						} // end if
						
						while ( tnn->item[i] != '\0' ) {
							sprintf( buf, "%s%02X", buf, tnn->item[i] );
							i++;
						} // end while
						
						int len = strlen( buf );
						if ( len > 6 ) { // overflow
							line[curIndex].error = true;
							sprintf( line[curIndex].errorM, "WORD overflow" );
						} // end if
						
						else {
							int j = 0;
						    while( len < 6 ) {
							    line[curIndex].objcode[j] = '0';
							    len++;
							    j++;
						    } // end while
						    
						    int g = 0;
						    while ( buf[g] != '\0' ) {
							    line[curIndex].objcode[j] = buf[g];
							    g++;
							    j++;
						    } // end while
						    
						    line[curIndex].objcode[j] = '\0';
						} // end else
					} // end if
					
					else if ( tnn->table == 6 ) {
						int i = 1;
						Str100 buf;
						while ( tnn->item[i] != '\0' ) {
							buf[i-1]  = tnn->item[i];
							i++;
						} // end while
						
						buf[i-1] = '\0';
						int len = strlen( buf );
						if ( len > 6 ) { // overflow
							line[curIndex].error = true;
							sprintf( line[curIndex].errorM, "WORD overflow" );
						} // end if
						
						else {
							int j = 0;
						    while( len < 6 ) {
							    line[curIndex].objcode[j] = '0';
							    len++;
							    j++;
						    } // end while
						    
						    int g = 0;
						    while ( buf[g] != '\0' ) {
							    line[curIndex].objcode[j] = buf[g];
							    g++;
							    j++;
						    } // end while
						    
						    line[curIndex].objcode[j] = '\0';
						} // end else
						
					} // end else if
					
				} // end else if
				
			} // end if
			
		} // end else if
		
		
	} // end else if
	
} // FillObjcode()

void SICXE::FillBase( TokenPtr t ) {
	if ( t->table == 5 ) {
		int addr = GetSymbolAddr( t->num );
		if ( addr != -1 ) {
			baseReg = addr;
		} // end if
		
		else { // undefine
			line[curIndex].error = true;
			sprintf( line[curIndex].errorM, "undefine symbol" );
		} // end else
		
	} // end if
	
	else if ( t->table == 6 ) {
		baseReg = D_To_H( t->item );
	} // end else if
	
} // FillBase()

void SICXE::EquPass2( TokenPtr t ) {
	int addr[100];
	char o[100];
	int ans = 0x0;
	int i = 0;
	int oi = -1;
	bool error = false;
	addr[0] = GetSymbolAddr( t->num );
	if ( addr[0] == -1 ) {
		line[curIndex].error = true;
		sprintf( line[curIndex].errorM, "EQU undefine symbol" );
	} // end if
	
	else {
		if ( t->next == NULL ) {
			ans = addr[0];
		    line[curIndex].loc = ans;
		    line[curIndex].equ = true;
		} // end if
		
		else {
			TokenPtr walk = t->next;
			bool stop = false;
			while ( ( walk != NULL ) && ( stop == false ) ) {
				if ( strcmp( walk->item, "." ) == 0 ) {
					stop = true;
				} // end if
				
				else if ( walk->table == 5 ) {
					i++;
					addr[i] = GetSymbolAddr( walk->num );
					if ( addr[i] == -1 ) {
						error = true;
						stop = true;
					} // end if
				} // end else if
				
				else {
					if ( strcmp( walk->item, "*" ) == 0 ) {
						walk = walk->next;
						if ( walk != NULL ) {
							i++;
					        addr[i] = GetSymbolAddr( walk->num );
					        if ( addr[i] == -1 ) {
						        error = true;
						        stop = true;
						    } // end if
						    
						    else {
						    	addr[i-1] = addr[i-1] * addr[i];
						    	i--;
							} // end else
					    } // end if
					} // end if
					
					else if ( strcmp( walk->item, "/" ) == 0 ) {
						walk = walk->next;
						if ( walk != NULL ) {
							i++;
					        addr[i] = GetSymbolAddr( walk->num );
					        if ( addr[i] == -1 ) {
						        error = true;
						        stop = true;
						    } // end if
						    
						    else {
						    	addr[i-1] = addr[i-1] / addr[i];
						    	i--;
							} // end else
					    } // end if
					    
					} // end else if
					
					else {
						oi++;
						o[oi] = walk->item[0];
					} // end else
				} // end else
				
				walk = walk->next;
			} // end while
			
			if ( error == false ) {
				int j = 1;
				int a = addr[0];
				int k = 0;
				while ( k <= oi ) {
					if ( o[k] == '+' ) {
						a = a + addr[j];
					} // end if
					
					else if ( o[k] == '-' ) {
						a = a - addr[j];
					} // end else if
					
					j++;
					k++;
				} // end while
				
				ans = a;
				line[curIndex].loc = ans;
				line[curIndex].equ = true;
			} // end if
			
			else {
				line[curIndex].error = true;
				sprintf( line[curIndex].errorM, "EQU undefine symbol" );
			} // end else
			
		} // end else
		
	} // end else
	
} // EquPass2()

void SICXE::FillExtraObj( LineType & temp ) {
	int n = temp.litNum;
	if ( literTab[n].type == BYTE ) {
		if ( ( literTab[n].name[0] == 'c' ) || ( literTab[n].name[0] == 'C' ) ) { // C
			int i = 2;
			Str100 buf;
			if ( literTab[n].name[1] != '\0' ) {
				sprintf( buf, "%02X", literTab[n].name[1] );
			} // end if
						
			while ( literTab[n].name[i] != '\0' ) {
				sprintf( buf, "%s%02X", buf, literTab[n].name[i] );
				i++;
			} // end while
						
			strcpy( temp.objcode, buf );
			
		} // end if
		
		else { // X
			int i = 1;
			while ( literTab[n].name[i] != '\0' ) {
				temp.objcode[i-1]  = literTab[n].name[i];
				i++;
			} // end while
						
			temp.objcode[i-1] = '\0';
		} // end else
		
	} // end if
	
	else { // WORD
		int len = strlen( literTab[n].name );
		if ( len > 6 ) {
			temp.error = true;
			sprintf( temp.errorM, "literal WORD overflow" );
		} // end if
					
		else {
			int o = 0x0;
			o = D_To_H( literTab[n].name );
			sprintf( temp.objcode, "%06X", o );
		} // end else
		
	} // end else
	
} // FillExtraObj()

void SICXE::FillExtraLiteral() {
	int i = curIndex + 1;
	bool in = false;
	bool stop = false;
	while ( stop == false ) {
		if ( i >= line.size() ) {
			stop = true;
		} // end if
		
		else {
			if ( line[i].extraLiteral == true ) {
				in = true;
				OutPutLine( line[i-1] );
				FillExtraObj( line[i] );
			} // end if
			
			else {
				stop = true;
			} // end else
			
		} // end else
		
		i++;
	} // end while
	
	if ( in == true ) {
		curIndex = i - 2;
	} // end if
	
} // FillExtraLiteral()

void SICXE::LinePass2() {
	if ( line[curIndex].error == false ) {
		TokenPtr t = cur;
		if ( line[curIndex].havecode == true ) {
			if ( t != NULL ) {
				if ( t->table == 5 ) {
					if ( t->next != NULL ) {
						FillObjcode( t->next );
					} // end if
				} // end if
				
				else {
					FillObjcode( t );
				} // end else
				
			} // end if
			
		} // end if
		
		else {
			Str100 u;
		    strcpy( u, t->item );
	        strupr(u);
	        
			if ( strcmp( u, "BASE" ) == 0 ) {
				if ( t->next != NULL ) {
					FillBase( t->next );
				} // end if
				
			} // end if
			
			else if ( ( strcmp( u, "END" ) == 0 ) || ( strcmp( u, "LTORG" ) == 0 ) ) {
				FillExtraLiteral();
			} // end else if
			
			else if ( t->next != NULL ) {
		        strcpy( u, t->next->item );
	            strupr(u);
				if ( strcmp( u, "EQU" ) == 0 ) {
					if ( line[curIndex].equ == false ) {
						if ( t->next->next != NULL ) {
							EquPass2( t->next->next );
							if ( line[curIndex].equ == true ) {
						        symtab[t->num].addr = line[curIndex].loc;
					        } // end if
					        
					        else {
					        	line[curIndex].error = true;
					        	sprintf( line[curIndex].errorM, "EQU undefine symbol" );
							} // end if
							
						} // end if
					} // end if
				} // end if
				
				else if ( strcmp( u, "BASE" ) == 0 ) {
					if ( t->next->next != NULL ) {
					    FillBase( t->next->next );
				    } // end if
				    
				} // end else if
				
				else if ( ( strcmp( u, "END" ) == 0 ) || ( strcmp( u, "LTORG" ) == 0 ) ) {
				    FillExtraLiteral();
			    } // end else if
				
			} // end else if
			
		} // end else if
		
	} // end if
	
} // LinePass2()

void SICXE::OutPutLine( LineType l ) {
	FILE *outfile = NULL;
	outfile = fopen(outFileName.c_str(),"a");
	TokenPtr w = NULL;
	
	if ( outfile != NULL ) {
	    if ( l.error == false ) {
	    	if ( ( l.com == true ) || ( l.end == true ) || ( l.noLoc == true ) ) {
	    		fprintf( outfile, "\n%4d\t\t", l.line );
	    		w = cur;
	    		while ( w != NULL ) {
	    			if ( strcmp( w->item, "." ) == 0 ) {
						fprintf( outfile, "%s", w->item );
	    				int e = 0;
	    				while ( w->com[e] != '\0' ) {
	    					fprintf( outfile, "%c", w->com[e] );
	    					e++;
						} // end while
	    				
					} // end if
					
					else {
						fprintf( outfile, "%s  ", w->item );
					} // end else
					
	    			w = w->next;
				} // end while
	    		
			} // end if
			
			else if ( l.extraLiteral == true ) {
				fprintf( outfile, "\n%4d\t%04X\t", l.line, l.loc );
				if ( literTab[l.litNum].type == WORD ) {
					fprintf( outfile, "=%s  WORD  %s\t%s", literTab[l.litNum].name, literTab[l.litNum].name, l.objcode );
				} // end if
				
				else {
					int x = 1;
					int r = 0;
					Str100 name;
					while ( literTab[l.litNum].name[x] != '\0' ) {
						name[r] = literTab[l.litNum].name[x];
						r++;
						x++;
					} // end while
					
					name[r] = '\0';
					fprintf( outfile, "=%c\'%s\'  BYTE", literTab[l.litNum].name[0], name );
					fprintf( outfile, "  =%c\'%s\'\t%s", literTab[l.litNum].name[0], name, l.objcode );
				} // end else
				
			} // end else if
			
			else if ( l.havecode == true ) {
				fprintf( outfile, "\n%4d\t%04X\t", l.line, l.loc );
				w = cur;
				while ( w != NULL ) {
					if ( strcmp( w->item, "\'" ) == 0 ) { // '
						w = w->next; // X or C
						if ( w != NULL ) {
							fprintf( outfile, "%c\'", w->item[0] ); // X or C
							int c = 1;
							while ( w->item[c] != '\0' ) {
								fprintf( outfile, "%c", w->item[c] );
								c++;
							} // end while
							
							w = w->next; //'
							if ( w != NULL ) {
								fprintf( outfile, "\'" );
							} // end if
							
						} // end if
						
					} // end if
					
					else if ( strcmp( w->item, "." ) == 0 ) {
						fprintf( outfile, "%s", w->item );
	    				int e = 0;
	    				while ( w->com[e] != '\0' ) {
	    					fprintf( outfile, "%c", w->com[e] );
	    					e++;
						} // end while
	    				
					} // end if
					
					else {
						fprintf( outfile, "%s  ", w->item );
					} // end else
					
					w = w->next;
				} // end while
				
				fprintf( outfile, "\t%s", l.objcode );
				
			} // end else if
			
			else {
				fprintf( outfile, "\n%4d\t%04X\t", l.line, l.loc );
				w = cur;
	    		while ( w != NULL ) {
	    			if ( strcmp( w->item, "." ) == 0 ) {
						fprintf( outfile, "%s", w->item );
	    				int e = 0;
	    				while ( w->com[e] != '\0' ) {
	    					fprintf( outfile, "%c", w->com[e] );
	    					e++;
						} // end while
	    				
					} // end if
					
					else {
						fprintf( outfile, "%s  ", w->item );
					} // end else
	    			
	    			w = w->next;
				} // end while
	    		
	    		
			} // end else
			
		} // end if
		
		else {
			fprintf( outfile, "\n%4d\tError: ", l.line );
			int p = 0;
			while ( l.errorM[p] != '\0' ) {
				fprintf( outfile, "%c", l.errorM[p] );
				p++;
			} // end while
			
		} // end else
		
	} // end if
	
	else {
		cout << endl << "### open " << outFileName << " failed ###" << endl;
	} // end else
	
	fclose(outfile);
} // OutPutLine()

void SICXE::Pass2() {
	curIndex = 0;
	HeadTokenPtr curHead = ghead;
	while ( curHead != NULL ) {
		
		cur = curHead->token;
		LinePass2();
		OutPutLine( line[curIndex] );
		curHead = curHead->nextHead;
		
		curIndex++;
	} // end while
} // Pass2()

void SICXE::SICXEMain() {
	curLine = 5;
	index = 0;
	baseReg = -1;
	newLiter = -1; 
	ClearSymTab();
	line.clear();
	literBuf.clear();
	HeadTokenPtr curHead = ghead;
	if ( curHead != NULL ) {
		cur = curHead->token;
		ClearOutFile();
		Pass1( curHead );
		Pass2();
	} // end if
	
	else {
		printf( "Empty File\n" );
	} // end else
} // SICXEMain()

void DealSICXE() {
	string fileName;
	GetToken( fileName );
	SICXE sic;
	sic.SICXEMain();
	//sic.FindEmpty( fileName );
	//sic.OutPut( fileName );
} // SIC()

int main() {
	int command = 0;
	printf( "\n**** SIC/XE Assembly *****" );
	printf( "\n* 0. Quit                *" );
	printf( "\n* 1. Get Token           *" );
	printf( "\n* 2. SIC                 *" );
	printf( "\n* 3. SICXE               *" );
	printf( "\n**************************" );
	printf( "\nInput a command(0, 1, 2, 3):" );
	cin >> command;
	
	while ( command != 0){
		if ( command == 1) { // Get Token
		    string fileName;
		    GetToken( fileName );
		} // end if
		
		else if ( command == 2 ) {
			DealSIC();
		} // end else if
		
		else if ( command == 3 ) {
			DealSICXE();
		} // end else if
		
		else
		    cout << endl << "Command does not exist!" << endl;
        printf( "\n**** SIC/XE Assembly *****" );
	    printf( "\n* 0. Quit                *" );
	    printf( "\n* 1. Get Token           *" );
	    printf( "\n* 2. SIC                 *" );
	    printf( "\n* 3. SICXE               *" );
	    printf( "\n**************************" );
	    printf( "\nInput a command(0, 1, 2, 3):" );
	    cin >> command;
	} // end while 
	
	return 0;
} // main()
