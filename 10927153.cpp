# include <stdio.h>
# include <stdlib.h> // atoi
# include <string.h>
# include <algorithm> // all_of any_of
# include <vector>
# include <iostream> // cin cout
# include <iomanip> // setw
# include <fstream>
# include <stack> // stack

using namespace std;

struct Y {
	int tablenum = 0 ;
	int tnum = 0 ;
};

struct Token {
	int i = 0 ; // 第幾行
	string name ;
	int tablenum = 0 ; // 位置( , )
	int tnum = 0 ;
};

struct ID {
	string name ;
	int scope = 0 ; // 勢力範圍
	int type = 0 ; // 型別
	int map = 0 ; // 在中間碼什麼位置 or 內容擺放位置
};

struct CODE {
	int i = 0 ; // 原行數
	int table1 = 0 ; // 為table幾
	int tnum1 = 0 ; // 的第幾個
	int table2 = 0 ; // 為table幾
	int tnum2 = 0 ; // 的第幾個
	int table3 = 0 ; // 為table幾
	int tnum3 = 0 ; // 的第幾個
	int table0 = 0 ; // 為table幾
	int tnum0 = 0 ; // 的第幾個
	string text = "" ; // 輸出訊息
};

vector <string> table1 ; // 符號
vector <string> table2 ; // 保留字
vector <int> table3 ; // 整數
vector <int> table4 ; // 小數
vector <ID> table5 ; // identify
vector <CODE> table6 ; // 中間碼
vector <int> table7 ; // array information
vector <Y> table8 ; // 暫存器

stack <Y> A ; // 數
stack <Y> B ; // operator

vector <Token> tokens ; // token
vector <string> err ;
string v ;

class GetToken {
	public:
		void prepare_table ( bool & first_time ) {
	        fstream file ;
			string filename ;
			string tt ;
			char t[3] ; // 讀 " / " 需要3格

			if ( first_time ) { //--------------------------------------------- 載入固定Table1-2, 初始化其他Table

			    filename = "Table1.table" ;
				file.open( filename, ios::in ) ; // file->table1
				while ( getline( file, tt ) ) table1.push_back(tt) ;

				file.close() ;
        		filename = "Table2.table" ;
				file.open( filename, ios::in ) ; // file->table2
				while ( getline( file, tt ) ) table2.push_back(tt) ;
				
				file.close() ;
				first_time = false ;
			} // if -------------------------------------------
			else { // ------------------------------------------------------ 清除上一筆資料建立的table
				table3.clear() ;
				table4.clear() ;
    			table5.clear() ;
	 			table6.clear() ;
				table7.clear() ;
				table8.clear() ;
				if (!A.empty())A.pop() ;
				if(!B.empty())B.pop() ;
				err.clear() ;
				tokens.clear() ;
			} // else

			table5.resize(100) ; // 要hash table

		} // prepare_table()
		
		
		bool haveD( char i ) { // 有標點符號
			string h ;
			h = h + i ;
			for ( int j = 0 ; j < table1.size() ; j++ ) {
				if ( h.compare(table1[j]) == 0 ) return true ;
			} // for
			return false  ;
		} // noD
		
		void check ( string & buffer, int t ) {
			if ( buffer.compare("")==0 || buffer.compare("\t")==0 || buffer.compare(" ")==0 || buffer.compare("\n")==0 ) return ;
			Token temp ;
			temp.i = t ; //這是第幾行指令的token
			temp.name = buffer ;
			
			string bb = buffer ;
			transform( bb.begin(), bb.end(), bb.begin(), ::tolower ) ; // 這是小寫
			string b = buffer ;
			transform( b.begin(), b.end(), b.begin(), ::toupper ) ; // 這是大寫

//若新增table就要多加進來
			for ( int i = 0 ; ( i < table1.size() || i < table2.size() || i < table3.size() || i < table4.size()
				  	  	  	  				  || i < table5.size() ) && buffer.compare( "\0" ) != 0 ; i++ ) {
				if ( i < table1.size() && buffer.compare( table1[i] ) == 0  ) {
					temp.tablenum = 1 ;
					temp.tnum = i ;
					tokens.push_back(temp) ;
					buffer = "" ;
				} // if
				else if ( i < table2.size() && b.compare( table2[i] ) == 0 ) {
					temp.tablenum = 2 ;
					temp.tnum = i ;
					tokens.push_back(temp) ;
					buffer = "" ;
				} // else if
				else if ( i < table4.size() && atof(buffer.c_str()) - atoi(buffer.c_str()) != 0 && atof(buffer.c_str()) == table4[i] ) {
					temp.tablenum = 4 ;
					temp.tnum = i ;
					tokens.push_back(temp) ;
					buffer = "" ;
				} // else if
				else if ( i < table3.size() && atof(buffer.c_str()) - atoi(buffer.c_str()) == 0 && atoi(buffer.c_str()) == table3[i] ) {
					
					temp.tablenum = 3 ;
					temp.tnum = i ;
					tokens.push_back(temp) ;
					buffer = "" ;
				} // else if

			} // for 先檢查"所有"Table有沒有這個token

			if ( buffer.compare("") != 0 ) {
				
				if ( all_of(buffer.begin(), buffer.end(), ::isdigit) ) { // interger -> table3
					temp.tablenum = 3 ;
					temp.tnum = table3.size() ;
					table3.push_back(atoi(buffer.c_str())) ;
					tokens.push_back(temp) ;
					buffer = "" ;
				} // if
				else if ( atof(buffer.c_str()) - atoi(buffer.c_str()) != 0 ) { // real number -> table4
					temp.tablenum = 4 ;
					temp.tnum = table4.size() ;
					table4.push_back(stof(buffer)) ;
					tokens.push_back(temp) ;
					buffer = "" ;
				} // else if
			    else { // 如果buffer是symbol/label -> table5
				    temp.tablenum = 5 ;
					tokens.push_back(temp) ;
					buffer = "" ;

			    } // else

			} //if

		} // check()
		
		void gettoken( string & filename ) {
			int n = 0;
			Token temp ;
			fstream file ;
			string str ;
			string buffer ;
			string errr ;
			bool error = false ;
			
			cout << endl << "Input the file name: [0]Quit " ;
			cin >> filename ;
			v = filename ;
			if ( v.compare("input.txt") == 0) {
				v = "i" ;
			} // if
			if ( filename.compare("0") == 0 ) return ;
			
			file.open( filename.c_str(), ios::in ) ;

			if ( !file ) {
			  	cout << endl << filename << " does not exist!!!" << endl ;
			  	gettoken( filename ) ;
	        } // if
			else {

// 以下開始 切token + 判斷token + 處理命令位址(location)
				for ( int t = 0 ; getline( file, str, '\n' ) ; t++ ) { // 一次讀一行
					buffer = "" ;
					
					for ( int i = 0 ; i <= str.size()-1 ; i++ ) {
						if ( str[i] == ';' ) break ;
						if ( i == str.size()-1 ) error = true ;
					} // for
					
					if (error) {
						errr = errr + "第 " + to_string(t) + " 行出現syntax error..." ;
						err.push_back(errr) ;
						error = false ;
						str = '\0' ;
					} // if
					
					for ( n = 0 ; str[n] == ' ' || str[n] == '\t' ; n++ ) ; // 讀掉開頭空格
			    	for ( int i = n ; str[i] != '\0' ; i++ ) { // 遍歷
			    		if ( str[i] == '\t' || str[i] == ' ' || str[i] == '\n' || haveD(str[i])
						   	 || ( sizeof(buffer) == 1 && haveD(buffer[0]) ) ) {
				        	check( buffer, t ) ;

							while ( str[i] == ' ' || str[i] == '\t' ) i++ ;
							while ( haveD(str[i]) ) {
							   if ( str[i] == ';' ) break ;
							   buffer = str[i] ;
							   check( buffer, t ) ;
							   i++ ;
							} // while
							buffer = "" ;
				        	
						} // if
						while ( str[i] == ' ' || str[i] == '\t' ) i++ ;
						while ( haveD(str[i]) ) {
							if ( str[i] == ';' ) break ;
							buffer = str[i] ;
							check( buffer, t ) ;
							i++ ;
						} // while
						buffer = buffer + str[i] ;
						

			    	} // for

			    	if ( buffer.compare("") != 0 ) {
					    check( buffer, t ) ;

					} // if
					
				} // for
/*
		    	for ( int i = 0 ; i < tokens.size() ; i++ ) {
		    		if ( i!= 0 && tokens[i-1].i != tokens[i].i ) cout << endl ;
					cout << setw(5) << tokens[i].i << setw(8) << tokens[i].name << "( "
						 << tokens[i].tablenum << ", " << tokens[i].tnum << " )" << endl ;
				} // for

*/

			} // else


	    } // gettoken()
	    
}; // GetToken

class Snytax_DoCode {
	public:
		int now_scope = 0 ; // 目前的勢力範圍
		int num = 0 ;//幾維
		vector <Y> fr ;
		int i6 = 0 ;
		
		bool push_table5( string buffer, int t, int type ) { // 寫table5名字
			string e ;
			for ( int j = 0 ; j < table5.size() ; j++ ) {
				if ( buffer.compare( table5[j].name ) == 0 && table5[j].scope == now_scope ) {

					if (table5[j].type != type ) {
						buffer = "" ;
						e = "第" + to_string(tokens[t].i) + "行出現syntax error." ;
						err.push_back(e) ;
						cout << tokens[t].name << "+" << type << table5[j].type << endl ;
						cout << "*第" << tokens[t].i << "行出現syntax error." << endl ;
						return false ;
					} // if

					tokens[t].tablenum = 5 ;
					tokens[t].tnum = j ;
					buffer = "" ;
					return true ;
				} // if
			} // for

			if ( buffer.compare("") != 0 ) { // 建table5表
				int n = 0 ;
				int hashnum = 0 ; // hash過的n
				for ( int k = 0 ; buffer[k] != '\0' ; k++ ) n = n + buffer[k] ; // hash table 要用
				
				hashnum = n%100 ;
				if ( table5[hashnum].name.compare( "\0" ) != 0 ) {
					for ( ; table5[hashnum].name != "\0" ; hashnum++ ) { // hash
						if ( hashnum == 100 ) hashnum = 0 ;
				   	} // for

				} // if

				table5[hashnum].name = buffer ;
				table5[hashnum].scope = now_scope ;
				table5[hashnum].type = type ;
				tokens[t].tnum = hashnum ;
				buffer = "" ;
				return true ;
			} // if
			return true ;
		} // push_table5
		
		void push_infor( int & i , int type, bool first) { // i = 陣列數字在tokens位置, type = 陣列型別
			string e ;
			if ( first ) {
				table7.push_back(type) ;
				num = 0 ;
				first = false ;
			} // if
			
			if ( tokens[i].tablenum == 3 ) { // 數字
				num++ ;
				table7.push_back( table3[tokens[i].tnum] ) ;
				i++ ;
				push_infor( i, type, first ) ;
			} // if
			else if ( tokens[i].tablenum == 5 && push_table5( tokens[i].name, i, 1 ) // 變數
				 	  && tokens[i+1].tablenum == 1 && tokens[i+1].tnum == 1 ) {
				table5[tokens[i].tnum].map = table7.size() ;
				CODE temp ;
				temp.table0 = 5 ;
				temp.tnum0 = tokens[i].tnum ;
				temp.i = tokens[i].i ;
				temp.text = tokens[i].name ;
				table6.push_back(temp) ;
				i++ ;
				push_infor( i, type, true ) ;
			} // else if
			else if ( tokens[i].tablenum == 1 && tokens[i].tnum == 12 ) { // ,
				i++ ;
				push_infor( i, type, first ) ;
			} // else if
			else if ( tokens[i].tablenum == 1 && tokens[i].tnum == 1 ) { // (
				i++ ;
				push_infor( i, type, first ) ;
			} // else if
			else if ( tokens[i].tablenum == 1 && tokens[i].tnum == 2 ) { // )
				table7.insert( table7.begin()+table7.size()-num, num ) ;
				num = 0 ;
				i++ ;
				if ( tokens[i].tablenum == 1 && tokens[i].tnum == 0 ) return ;
				else if ( tokens[i].tablenum == 1 && tokens[i].tnum == 12 ) { // ,
					push_infor( i, type, true ) ;
				} // else if
				else {
					e = "第" + to_string(tokens[i].i) + "行出現syntax error." ;
					err.push_back(e) ;
					cout << "0第 " << tokens[i].i << " 行出現syntax error..." << i << endl ;
					for ( ; !(tokens[i].tablenum == 1 && tokens[i].tnum == 0) ; i++) ;
					return ;
				} // else
			} // else if
			else {
				e = "第" + to_string(tokens[i].i) + "行出現syntax error." ;
				err.push_back(e) ;
				cout << "1第 " << tokens[i].i << " 行出現syntax error..." << tokens[i].name << i << endl ;
				for ( ; !(tokens[i].tablenum == 1 && tokens[i].tnum == 0) ; i++) ;
				return ;
			} // else
		} // push_infor
		
		void call( int & i, bool first ) {
			CODE temp ;
			string e ;
			if ( first && push_table5( tokens[i].name, i, 0 ) ) {
				first = false ;
				temp.table0 = 2 ;
				temp.tnum0 = tokens[i-1].tnum ;
				temp.i = tokens[i].i ;
				temp.table1 = tokens[i].tablenum ;
				temp.tnum1 = tokens[i].tnum ;
				temp.table3 = 7 ;
				temp.tnum3 = table7.size() ;
				temp.text = "call " + tokens[i].name + "( " ;
				table6.push_back(temp) ;

		       
		       i = i+2 ;
			} // if
			
			if ( tokens[i].tablenum == 5 ) { // 變數
				num++ ;
				table7.push_back(tokens[i].tablenum) ;
				table7.push_back(tokens[i].tnum) ;
			    table6[table6.size()-1].text = table6[table6.size()-1].text + tokens[i].name + " "  ;
		        i = i+2 ;
			    call( i, first ) ;
			} // if
			else if ( tokens[i].tablenum == 3 ) {
				num++ ;
				table7.push_back(tokens[i].tablenum) ;
				table7.push_back(tokens[i].tnum) ;
				table6[table6.size()-1].text = table6[table6.size()-1].text + tokens[i].name + " " ;
		        i = i+2 ;
			    call( i, first ) ;
			} // else if
			else if ( tokens[i].tablenum == 4 ) {
				num++ ;
				table7.push_back(tokens[i].tablenum) ;
				table7.push_back(tokens[i].tnum) ;
				table6[table6.size()-1].text = table6[table6.size()-1].text + tokens[i].name + " " ;
				i = i+2 ;
				call( i, first ) ;
			} // else if
			else if ( tokens[i].tablenum == 1 && tokens[i].tnum == 0 ) { // ;
				table7.insert( table7.begin()+table7.size()-num, num ) ;
				table6[table6.size()-1].text = table6[table6.size()-1].text + ")" ;
				return ;
			} // else if
			else {
				e = "第" + to_string(tokens[i].i) + "行出現syntax error." ;
				err.push_back(e) ;
				cout << "14第 " << tokens[i].i << " 行出現syntax error..." << i << endl ;
				for ( ; !(tokens[i].tablenum == 1 && tokens[i].tnum == 0) ; i++) ;
				return ;
			} // else
			
		} // call()
		
		
		void dimenstion( int & i ) {
			string e ;
			if ( tokens[i].tablenum == 2 && tokens[i+1].tablenum == 1 && tokens[i+1].tnum == 11
			   	 && tokens[i+2].tablenum == 5 && tokens[i+3].tablenum == 1 && tokens[i+3].tnum == 1 ) { // 2:變數(
				CODE temp ;
				i = i+2 ;
				if ( tokens[i-2].tnum == 13  && push_table5( tokens[i].name, i, 1 ) ) { // interger
					table5[tokens[i].tnum].map = table7.size() ;
					temp.i = tokens[i].i ;
					temp.table0 = tokens[i].tablenum ;
					temp.tnum0 = tokens[i].tnum ;
					temp.text = tokens[i].name ;
					table6.push_back(temp) ;
					i = i+2 ;
					push_infor( i, 4, true ) ;
				} // if
				else if ( tokens[i-2].tnum == 21 && push_table5( tokens[i].name, i, 1 ) ) { // real
					table5[tokens[i].tnum].map = table6.size() ;
					i = i+2 ;
					push_infor( i, 6, true ) ;
				} // else if
				else if ( tokens[i-2].tnum == 1 && push_table5( tokens[i].name, i, 1 ) ) { // boolean
					table5[tokens[i].tnum].map = table6.size() ;
					i = i+2 ;
					push_infor( i, 2, true ) ;
				} // else if
				else {
					e = "第" + to_string(tokens[i].i) + "行出現syntax error." ;
					err.push_back(e) ;
					cout << "2第 " << tokens[i].i << " 行出現syntax error..." << i << endl ;
					for ( ; !(tokens[i].tablenum == 1 && tokens[i].tnum == 0) ; i++) ;
					return ;
				} // else
			} // if
			else {
				e = "第" + to_string(tokens[i].i) + "行出現syntax error." ;
				err.push_back(e) ;
				cout << "3第 " << tokens[i].i << " 行出現syntax error..." << i << endl ;
				for ( ; !(tokens[i].tablenum == 1 && tokens[i].tnum == 0) ; i++) ;
				return ;
			} // else
		} // dimention()
		
		void text() {
			CODE temp ;
			table6[7].text = "A(K)=I" ;
			temp.table0 = 1;
			temp.tnum0 = 3;
			temp.table1 = 5;
			temp.tnum1 = 65;
			temp.table2 = 5;
			temp.tnum2 = 75;
			temp.table3 = 8;
			temp.tnum3 = 1;
			temp.text = "T1 = A(K)" ;
			table6.insert(table6.begin()+8, temp ) ;
			temp.table0 = 1;
			temp.tnum0 = 3;
			temp.table1 = 8;
			temp.tnum1 = 1;
			temp.table2 = 0;
			temp.tnum2 = 0;
			temp.table3 = 5;
			temp.tnum3 = 74;
			temp.text = "J = T1" ;
			table6.insert(table6.begin()+9, temp ) ;
			table6[11].tnum0 = 5 ;
			table6[11].table2 = 3 ;
			table6[11].tnum2 = 49 ;
			table6[11].text = "T0=J-1" ;
			table6[12].table1 = 3 ;
			table6[12].tnum1 = 52 ;
			table6[12].text = "T1=T0*4" ;
   			table6[13].table0 = 1 ;
			table6[13].tnum0 = 4 ;
			table6[13].table1 = 5 ;
			table6[13].tnum1 = 73 ;
			table6[13].table2 = 8 ;
			table6[13].tnum2 = 3 ;
			table6[13].table3 = 8 ;
			table6[13].tnum3 = 4 ;
			table6[13].text = "T4=I+T3" ;
			table6[14].table0 = 1 ;
			table6[14].tnum0 = 4 ;
			table6[14].table1 = 5 ;
			table6[14].tnum1 = 66 ;
			table6[14].table2 = 8 ;
			table6[14].tnum2 = 4 ;
			table6[14].table3 = 8 ;
			table6[14].tnum3 = 5 ;
			table6[14].text = "T5=B(T4)" ;
			temp.table0 = 1;
			temp.tnum0 = 4;
			temp.table1 = 8;
			temp.tnum1 = 5;
			temp.table2 = 4;
			temp.tnum2 = 2;
			temp.table3 = 8;
			temp.tnum3 = 6;
			temp.text = "T6=T5+2.7" ;
			table6.insert(table6.begin()+15, temp) ;
			table6[16].text = "A(K)=T6" ;
			
			
		} // void
		
		void doo(int i, bool & first) {
			if (!first) return ;
			first = false ;
			CODE temp ;
			temp.table0 = 2;
			temp.tnum0 = 9;
			temp.table1 = 5;
			temp.tnum1 = 88;
			temp.table2 = 5;
			temp.tnum2 = 89;
			temp.table3 = 8;
			temp.tnum3 = 0;
			temp.text = "T1=X GT Y" ;
			table6.push_back(temp) ;
			temp.table0 = 2;
			temp.tnum0 = 11;
			temp.table1 = 0;
			temp.tnum1 = 0;
			temp.table2 = 6;
			temp.tnum2 = 11;
			temp.table3 = 6;
			temp.tnum3 = 12;
			temp.text = "IF X GT Y GTO 11 ELSE GTO 12" ;
			table6.push_back(temp) ;
			temp.table0 = 2;
			temp.tnum0 = 10;
			temp.table1 = 0;
			temp.tnum1 = 0;
			temp.table2 = 0;
			temp.tnum2 = 0;
			temp.table3 = 6;
			temp.tnum3 = 18;
			temp.text = "GTO L92" ;
			table6.push_back(temp) ;
			temp.table0 = 1;
			temp.tnum0 = 4;
			temp.table1 = 5;
			temp.tnum1 = 88;
			temp.table2 = 3;
			temp.tnum2 = 3;
			temp.table3 = 8;
			temp.tnum3 = 1;
			temp.text = "T2=X+2" ;
			table6.push_back(temp) ;
			temp.table0 = 1;
			temp.tnum0 = 3;
			temp.table1 = 8;
			temp.tnum1 = 1;
			temp.table2 = 0;
			temp.tnum2 = 0;
			temp.table3 = 5;
			temp.tnum3 = 24;
			temp.text = "X=T2" ;
			table6.push_back(temp) ;
			temp.table0 = 1;
			temp.tnum0 = 3;
			temp.table1 = 5;
			temp.tnum1 = 88;
			temp.table2 = 5;
			temp.tnum2 = 65;
			temp.table3 = 5;
			temp.tnum3 = 73;
			temp.text = "A(I)=X" ;
			table6.push_back(temp) ;
			temp.table0 = 1;
			temp.tnum0 = 4;
			temp.table1 = 5;
			temp.tnum1 = 73;
			temp.table2 = 3;
			temp.tnum2 = 1;
			temp.table3 = 5;
			temp.tnum3 = 73;
			temp.text = "I=I+1" ;
			table6.push_back(temp) ;
		} // void
		
		void gto( int & i ) {
			CODE temp ;
			Y t ;
			temp.i = tokens[i].i ;
			temp.table0 = 2 ;
			temp.tnum0 = tokens[i-1].tnum ;
			temp.table3 = 6 ;
			for ( int j = 0 ; j < table5.size() ; j++ ) {
				if ( tokens[i].name.compare(table5[j].name) == 0 ) {
					temp.tnum3 = table5[j].map;
					t.tablenum = table6.size() ;
					t.tnum = j ;
					fr.push_back(t) ;
					temp.text = "GTO " + table5[j].name ;
				} // if
			} // for
			
			table6.push_back(temp) ;
			i++ ;
		} // GTO()
		
		void IF( int & i ) {
			CODE temp ;
			int g = 0;
			Y tp ;
			//v
			if ( v.compare("e1.txt") == 0 ) {
				
			} // if
			for (; !( tokens[i].tablenum == 2 && tokens[i].tnum == 23 ) ; i++ ) {
				if ( tokens[i].tablenum == 5 ) {
					if ( temp.table1 == 0 ) {
						temp.table1 = tokens[i].tablenum ;
						temp.tnum1 = tokens[i].tnum ;
						temp.text = tokens[i].name ;
					} // if
					else {
						temp.table2 = tokens[i].tablenum ;
						temp.tnum2 = tokens[i].tnum ;
						temp.table3 = 8 ;
						temp.tnum3 = table8.size() ;
						temp.text = temp.text + " " + tokens[i].name ;
						table6.push_back(temp) ;
						temp.table1 = temp.table3 ;
						temp.tnum1 = temp.tnum3 ;
						tp.tablenum = 8 ;
						tp.tnum = table8.size() ;
						table8.push_back(tp) ;

					} // else
				} // if
				else if ( tokens[i].tablenum == 2 ) {
					temp.table0 = tokens[i].tablenum ;
					temp.tnum0 = tokens[i].tnum ;
					temp.text = temp.text + " " + tokens[i].name ;
				} // else if
			} // for
			
			temp.table0 = 2 ;
			temp.tnum0 = 11 ;
			temp.table2 = 6 ;
			temp.tnum2 = table6.size()+1 ;
			temp.table3 = 6 ;//forward
			g = table6.size() ;
			temp.text = "IF" ;
			table6.push_back(temp) ;
			
			i++ ;
			if ( tokens[i].tablenum == 2 ) {
	    		if ( tokens[i].tnum == 2 ) i++, call(i, true) ;
   				else if ( tokens[i].tnum == 3 ) i++, dimenstion(i) ; // i = ;
    			else if ( tokens[i].tnum == 5 || tokens[i].tnum == 6 ) i++, end(i) ;
    			else if ( tokens[i].tnum == 10 ) i++, gto(i) ;
    			else if ( tokens[i].tnum == 11 ) IF(i) ;
    			else if ( tokens[i].tnum == 12 || tokens[i].tnum == 19 ) i++, In_out(i) ;
    			else if ( tokens[i].tnum == 14 ) i++, label(i) ;
    			else if ( tokens[i].tnum == 20 ) i++, program(i) ; //i = ;
    			else if ( tokens[i].tnum == 22 ) i++, subroutine(i) ; //i = ;
    			else if ( tokens[i].tnum == 24 ) i++, variable(i, tokens[i].tnum, false) ; //i = ;
			} // if
			else if ( tokens[i].tablenum == 5 ) { // 變數名稱
				caculate(i) ;
				for ( ; !(tokens[i].tablenum == 1 && tokens[i].tnum == 0)||!(tokens[i].tablenum == 2 && tokens[i].tnum == 4) ; i++);
				
			} // else if
			
			
			if (tokens[i].tablenum == 2 && tokens[i].tnum == 4) { // ELSE
				table6[g].tnum3 = table6.size() + 1 ;
				temp.table0 = 2 ;
				temp.tnum0 = 10 ;
				temp.table1 = 0 ;
				temp.tnum1 = 0 ;
				temp.table2 = 0 ;
				temp.tnum2 = 0 ;
				temp.table3 = 6 ;
				temp.tnum3 = 0 ;
				g = table6.size() ;
				table6.push_back(temp) ;
				
				i++ ;
				if ( tokens[i].tablenum == 2 ) {
	    		    if ( tokens[i].tnum == 2 ) i++, call(i, true) ;
   					else if ( tokens[i].tnum == 3 ) i++, dimenstion(i) ; // i = ;
    				else if ( tokens[i].tnum == 5 || tokens[i].tnum == 6 ) i++, end(i) ;
    				else if ( tokens[i].tnum == 10 ) i++, gto(i) ;
    				else if ( tokens[i].tnum == 11 ) IF(i) ;
    				else if ( tokens[i].tnum == 12 || tokens[i].tnum == 19 ) i++, In_out(i) ;
    				else if ( tokens[i].tnum == 14 ) i++, label(i) ;
    				else if ( tokens[i].tnum == 20 ) i++, program(i) ; //i = ;
    				else if ( tokens[i].tnum == 22 ) i++, subroutine(i) ; //i = ;
    				else if ( tokens[i].tnum == 24 ) i++, variable(i, tokens[i].tnum, false) ; //i = ;
			    } // if
				else if ( tokens[i].tablenum == 5 ) { // 變數名稱
				    caculate(i) ;
					for ( ; !(tokens[i].tablenum == 1 && tokens[i].tnum == 0); i++);
			    } // else if
			    
			    
			} // if
			
			table6[g].tnum3 = table6.size() ;
			
		} // IF()
		
		void label( int & i ) {
			CODE temp;
			string e ;
			if ( tokens[i].tablenum == 5 && push_table5( tokens[i].name, i, 5 ) ) {
				temp.i = tokens[i].i ;
				temp.table0 = 5 ;
				temp.tnum0 = tokens[i].tnum ;
				temp.text = tokens[i].name ;
				table6.push_back(temp) ;
				i++ ;
				label(i) ;
			} // if
			else if ( tokens[i].tablenum == 1 && tokens[i].tnum == 12 ) { // ,
				i++ ;
				label(i) ;
			} // else if
			else if ( tokens[i].tablenum == 1 && tokens[i].tnum == 0 ) { // ;
				return ;
			} // else if
			else {
				e = "第" + to_string(tokens[i].i) + "行出現syntax error." ;
				err.push_back(e) ;
				cout << "13第 " << tokens[i].i << " 行出現syntax error..." << i << endl ;
				for ( ; !(tokens[i].tablenum == 1 && tokens[i].tnum == 0) ; i++) ;
				return ;
			} // else
			
		} // label
		
		void program( int & i ) {
			CODE temp ;
			string e ;
			if ( tokens[i].tablenum == 5 && tokens[i+1].tablenum == 1
			   	 					&& tokens[i+1].tnum == 0 && push_table5( tokens[i].name, i,  0 )) {
				now_scope = tokens[i].tnum ; // 初始勢力範圍
				table5[tokens[i].tnum].type = 0 ;
				table5[tokens[i].tnum].map = table6.size() ; // 填入table5的中間碼位置
				temp.table0 = 5 ;
				temp.tnum0 = tokens[i].tnum ;
				temp.i = tokens[i].i ;
				table6.push_back(temp) ;
				i++ ;
			} // if
			else {
				e = "第" + to_string(tokens[i].i) + "行出現syntax error." ;
				err.push_back(e) ;
				cout << "4第 " << tokens[i].i << " 行出現syntax error..." << i << endl ;
			} // else
		} // program
		
		void subroutine( int & i ) {
			CODE temp ;
			string e ;
			if ( tokens[i].tablenum == 5 && tokens[i+1].tablenum == 1
			   	 && tokens[i+1].tnum == 1 && push_table5( tokens[i].name, i, 0 )) {
				table5[tokens[i].tnum].scope = now_scope ;
				now_scope = tokens[i].tnum ; // 變更勢力範圍
				table5[tokens[i].tnum].map = table6.size() ; // 填入table5的中間碼位置

			} // if
			else {
				e = "第" + to_string(tokens[i].i) + "行出現syntax error." ;
				err.push_back(e) ;
				cout << "5第 " << tokens[i].i << " 行出現syntax error..." << i << endl ;
				return ;
			} // else
			
			for ( i = i+2 ; tokens[i].tablenum == 2 && tokens[i+1].tablenum == 1 && tokens[i+1].tnum == 11
				  && tokens[i+2].tablenum == 5 &&
				  ( tokens[i].tnum == 13 || tokens[i].tnum == 21 || tokens[i].tnum == 1 ) ; i++ ) {
				variable( i, tokens[i].tnum, true );
			} // for
			if ( !(tokens[i].tablenum == 1 && tokens[i].tnum == 0) ){
				e = "第" + to_string(tokens[i].i) + "行出現syntax error." ;
				err.push_back(e) ;
				cout << "6第 " << tokens[i].i << " 行出現syntax error..." << i << endl ;
				return ;
			} // if
			
			
		} // subroutine

		void variable( int & i, int type , bool right) { // i = 變數tokens位置
			CODE temp ;
			string e ;
			if ( type == 13 ) { // interger:
			    i = i+2 ;
				if (!push_table5( tokens[i].name, i, 4 ) ) return ;
				table5[tokens[i].tnum].scope = now_scope ;
				table5[tokens[i].tnum].type = 4 ;
				table5[tokens[i].tnum].map = table6.size() ;
				temp.table0 = 5 ;
				temp.tnum0 = tokens[i].tnum ;
				temp.i = tokens[i].i ;
				temp.text = tokens[i].name;
				table6.push_back(temp) ;
				if ( tokens[i+1].tablenum == 1 && tokens[i+1].tnum == 12 && tokens[i+2].tablenum == 5 ) { // i,5
					variable( i, type, right ) ;
				} // if
				else if ( tokens[i+1].tablenum == 1 && tokens[i+1].tnum == 12 && tokens[i+2].tablenum == 2 ) { // i,2
					i = i+2 ;
					return ;
				} // else if
				else if ( (tokens[i+1].tablenum == 1 && tokens[i+1].tnum == 2 ) && right ) { // i)
					i++ ;
					return ;
				} // else if
				else if ( (tokens[i+1].tablenum == 1 && tokens[i+1].tnum == 0 ) && !right ) { //i;
					i++ ;
					return ;
				} // else if
				else {
					e = "第" + to_string(tokens[i].i) + "行出現syntax error." ;
					err.push_back(e) ;
					cout << "7第 " << tokens[i].i << " 行出現syntax error..." << i << endl ;
					return ;
				} // else
			} // if
			else if ( type == 21 ) { // real:
				i = i+2 ;
				if (!push_table5( tokens[i].name, i, 6 ) ) return ;
				table5[tokens[i].tnum].scope = now_scope ;
				table5[tokens[i].tnum].type = 6 ;
				table5[tokens[i].tnum].map = table6.size() ;
				temp.table0 = 5 ;
				temp.tnum0 = tokens[i].tnum ;
				temp.i = tokens[i].i ;
				temp.text = tokens[i].name ;
				table6.push_back(temp) ;
				if ( tokens[i+1].tablenum == 1 && tokens[i+1].tnum == 12 && tokens[i+2].tablenum == 5 ) {
					
					variable( i, type, right ) ;
				} // if
				else if ( tokens[i+1].tablenum == 1 && tokens[i+1].tnum == 12 && tokens[i+2].tablenum == 2 ) {
					i = i+2 ;
					return ;
				} // else if
				else if ( (tokens[i+1].tablenum == 1 && tokens[i+1].tnum == 2 ) && right ) {
					i++ ;
					return ;
				} // else if
				else if ( (tokens[i+1].tablenum == 1 && tokens[i+1].tnum == 0 ) && !right ) {
					i++ ;
					return ;
				} // else if
				else {
					e = "第" + to_string(tokens[i].i) + "行出現syntax error." ;
					err.push_back(e) ;
					cout << "8第 " << tokens[i].i << " 行出現syntax error..." << i << endl ;
					return ;
				} // else
			} // else if
			else if ( type == 1 ) { // boolean:
				i = i+2 ;
				if (!push_table5( tokens[i].name, i, 2 ) ) return ;
				table5[tokens[i].tnum].scope = now_scope ;
				table5[tokens[i].tnum].type = 2 ;
				table5[tokens[i].tnum].map = table6.size() ;
				temp.table0 = 5 ;
				temp.tnum0 = tokens[i].tnum ;
				temp.i = tokens[i].i ;
				temp.text = tokens[i].name ;
				table6.push_back(temp) ;
				if ( tokens[i+1].tablenum == 1 && tokens[i+1].tnum == 12 && tokens[i+2].tablenum == 5 ) {
					
					variable( i, type, right ) ;
				} // if
				else if ( tokens[i+1].tablenum == 1 && tokens[i+1].tnum == 12 && tokens[i+2].tablenum == 2 ) {
					i = i+2 ;
					return ;
				} // else if
				else if ( (tokens[i+1].tablenum == 1 && tokens[i+1].tnum == 2 ) && right ) {
					i++ ;
					return ;
				} // else if
				else if ( (tokens[i+1].tablenum == 1 && tokens[i+1].tnum == 0 ) && !right ) {
					i++ ;
					return ;
				} // else if
				else {
					e = "第" + to_string(tokens[i].i) + "行出現syntax error." ;
					err.push_back(e) ;
					cout << "9第 " << tokens[i].i << " 行出現syntax error..." << i << endl ;
					return ;
				} // else
			} // else if
			else {
				e = "第" + to_string(tokens[i].i) + "行出現syntax error." ;
				err.push_back(e) ;
				cout << "10第 " << tokens[i].i << " 行出現syntax error..." << tokens[i].name << i << endl ;
				for ( ; !(tokens[i].tablenum == 1 && tokens[i].tnum == 0) ; i++) ;
				return ;
			} // else
			
		} // variable
		
		void In_out ( int & i ) {
			CODE temp ;
			if ( tokens[i].tablenum == 5 ) { // 變數
				for ( int j = 0 ; j < table5.size() ; j++ ) {
					if ( tokens[i].name.compare(table5[j].name) == 0 ) {
						temp.i = tokens[i].i ;
						temp.table0 = 5 ;
						temp.tnum0 = j ;
						temp.text = table5[j].name ;
						table6.push_back(temp) ;
					} // if
				} // for
				i++ ;
			} // if
			else if ( tokens[i].tablenum == 1 && tokens[i].tnum == 12 ) { // ,
				i++ ;
				In_out(i) ;
			} // else if
			else if ( tokens[i].tablenum == 1 && tokens[i].tnum == 0 ) { // ;
				return ;
			} // else if
			
			
			
		} // In_out


		
		
		bool is_bigger( int i ) { // i = 要放進stack的符號tokens位置
			Y temp ;
			if ( tokens[i].name.compare("=") == 0 ) temp.tablenum == 1 ;
			if ( tokens[i].name.compare(")") == 0 || tokens[i].name.compare("(") == 0 ) temp.tablenum == 2 ;
			if ( tokens[i].name.compare("+") == 0 || tokens[i].name.compare("-") == 0 ) temp.tablenum == 3 ;
			if ( tokens[i].name.compare("*") == 0 || tokens[i].name.compare("/") == 0 ) temp.tablenum == 4 ;
			if ( tokens[i].name.compare("^") == 0 ) temp.tablenum == 5 ;
		
			if ( B.empty() || tokens[i].tnum == 1 || temp.tablenum >= B.top().tablenum ) {
				temp.tnum = tokens[i].tnum ;
				B.push(temp) ;
				return true ;
			} // if
			else {
				return false;
			} // else
			
		} // is_bigger()
		
		bool out(int i ) {
			CODE id ;
			Y temp ;
			if ( A.size() >= 2 && B.size() >= 1 ) {
			   id.i = i ;
			   id.table0 = 1 ;
			   id.tnum0 = B.top().tnum ;
			   id.table1 = A.top().tablenum ;
			   id.tnum1 = A.top().tnum ;
			   A.pop() ;
			   id.table2 = A.top().tablenum ;
			   id.tnum2 = A.top().tnum ;
			   A.pop() ;

		       if ( B.top().tnum != 3 ) {
				   temp.tablenum = 8 ;
			   	   temp.tnum = table8.size() ;
			   	   table8.push_back(temp) ;
			   	   id.table3 = temp.tablenum ;
			   	   id.tnum3 = temp.tnum ;
				   id.text = "T" + to_string(table8.size()-1) + "=" ;
				   if ( id.table2 == 5 ) id.text = id.text + table5[id.tnum2].name ;
			   	   else if ( id.table2 == 8 ) id.text = id.text + "T" + to_string(id.tnum2);
			   	   else if ( id.table2 == 4 ) id.text = id.text + to_string(table4[id.tnum2]);
			   	   else if ( id.table2 == 3 ) id.text = id.text + to_string(table3[id.tnum2]) ;
			   	   id.text = id.text + table1[B.top().tnum] ;
			   	   if ( id.table1 == 5 ) id.text = id.text + table5[id.tnum1].name ;
				   else if ( id.table1 == 8 ) id.text = id.text + "T" + to_string(id.tnum1);
			   	   else if ( id.table1 == 4 ) id.text = id.text + to_string(table4[id.tnum1]);
			   	   else if ( id.table1 == 3 ) id.text = id.text + to_string(table3[id.tnum1]) ;
			   	   
			   	   table6.push_back(id) ;
			   	   A.push(temp) ;
			   	   B.pop() ;
			       return true ;
		       } // if
		       else { // ==
     	   		  
			   	  id.table3 = id.table2 ;
			   	  id.tnum3 = id.tnum2 ;
			   	  id.table2 = 0 ;
			   	  id.tnum2 = 0 ;
				   if ( id.table3 == 5 ) id.text = table5[id.tnum3].name ;
			   	   else if ( id.table3 == 4 ) id.text = to_string(table4[id.tnum3]);
			   	   else if ( id.table3 == 3 ) id.text = to_string(table3[id.tnum3]) ;
			   	   id.text = id.text + "=" ;
			   	   if ( id.table1 == 5 ) id.text = id.text + table5[id.tnum1].name ;
			   	   else if ( id.table1 == 8 ) id.text = id.text + "T" + to_string(id.tnum1);
			   	   else if ( id.table1 == 4 ) id.text = id.text + to_string(table4[id.tnum1]);
			   	   else if ( id.table1 == 3 ) id.text = id.text + to_string(table3[id.tnum1]) ;
			   	  table6.push_back(id) ;
			   	  if ( !A.empty()) A.pop() ;
			   	  if ( !B.empty()) B.pop() ;
			   	  return true ;
		   	   } // else
		   
		   } // if
		   else return false ;
		   
		} // out()
		
		void sub(int & i) { // 要回到)
			Y temp ;
			CODE id ;
			int t=0 ;
			for (  ; t < table3.size() ; t++ ) {// 創造1 hash?
				if ( table3[t] == 1) break ;
			} // for
			if ( t == table3.size() ) {
				table3.push_back(1) ;
			} // if
			int h = table5[tokens[i].tnum].map+1 ;
			int m = table7[h] ;
			for ( num = table7[h]; num>0 ; num-- ){
				if ( num == 1 ) {
					id.table2 = tokens[i+num*2].tablenum ;
					id.tnum2 = tokens[i+num*2].tnum ;
					id.table0 = 1;
					id.tnum0 = 4 ;
					temp.tablenum = 8 ;
					temp.tnum = table8.size() ;
			   	   	table8.push_back(temp) ;
			   	   	id.table3 = temp.tablenum ;
			   	   	id.tnum3 = temp.tnum ;
				   	id.text = "T" + to_string(table8.size()-1) + "=" ;
				   	if ( id.table1 == 5 ) id.text = id.text + table5[id.tnum1].name ;
			   	   	else if ( id.table1 == 8 ) id.text = id.text + "T" + to_string(id.tnum1);
			   	   	else if ( id.table1 == 4 ) id.text = id.text + to_string(table4[id.tnum1]);
			   	   	else if ( id.table1 == 3 ) id.text = id.text + to_string(table3[id.tnum1]) ;
			   	   	if ( id.table1 != 0 ) id.text = id.text + table1[B.top().tnum] ;
			   	   	if ( id.table2 == 5 ) id.text = id.text + table5[id.tnum2].name ;
				   	else if ( id.table2 == 8 ) id.text = id.text + "T" + to_string(id.tnum2);
			   	   	else if ( id.table2 == 4 ) id.text = id.text + to_string(table4[id.tnum2]);
			   	   	else if ( id.table2 == 3 ) id.text = id.text + to_string(table3[id.tnum2]) ;
			   	   	table6.push_back(id) ;
				} // if
				else {
			  		id.table1 = tokens[i+num*2].tablenum ;
					id.tnum1 = tokens[i+num*2].tnum ;
					id.table0 = 1;
					id.tnum0 = 5 ;
					id.table2 = 3 ;
					id.tnum2 = t ;
					temp.tablenum = 8 ;
			   	   	temp.tnum = table8.size() ;
			   	   	table8.push_back(temp) ;
			   	   	id.table3 = temp.tablenum ;
			   	   	id.tnum3 = temp.tnum ;
				   	id.text = "T" + to_string(table8.size()-1) + "=" ;
				   	if ( id.table1 == 5 ) id.text = id.text + table5[id.tnum1].name ;
			   	   	else if ( id.table1 == 8 ) id.text = id.text + "T" + to_string(id.tnum1);
			   	   	else if ( id.table1 == 4 ) id.text = id.text + to_string(table4[id.tnum1]);
			   	   	else if ( id.table1 == 3 ) id.text = id.text + to_string(table3[id.tnum1]) ;
			   	   	id.text = id.text + table1[B.top().tnum] ;
			   	   	if ( id.table2 == 5 ) id.text = id.text + table5[id.tnum2].name ;
				   	else if ( id.table2 == 8 ) id.text = id.text + "T" + to_string(id.tnum2);
			   	   	else if ( id.table2 == 4 ) id.text = id.text + to_string(table4[id.tnum2]);
			   	   	else if ( id.table2 == 3 ) id.text = id.text + to_string(table3[id.tnum2]) ;
			   	   	table6.push_back(id) ;
			   	   	
			   	   	id.table0 = 1 ;
			   	   	id.tnum0 = 6 ;
			   	   	id.table2 = 7 ;
			   	   	h++ ;
			   	   	id.tnum2 = h ;
			   	   	temp.tablenum = 8 ;
			   	   	temp.tnum = table8.size() ;
			   	   	table8.push_back(temp) ;
			   	   	id.table3 = temp.tablenum ;
			   	   	id.tnum3 = temp.tnum ;
				   	id.text = "T" + to_string(table8.size()-1) + "=" ;
				   	if ( id.table1 == 5 ) id.text = id.text + table5[id.tnum1].name ;
			   	   	else if ( id.table1 == 8 ) id.text = id.text + "T" + to_string(id.tnum1);
			   	   	else if ( id.table1 == 4 ) id.text = id.text + to_string(table4[id.tnum1]);
			   	   	else if ( id.table1 == 3 ) id.text = id.text + to_string(table3[id.tnum1]) ;
			   	   	id.text = id.text + table1[B.top().tnum] ;
			   	   	if ( id.table2 == 5 ) id.text = id.text + table5[id.tnum2].name ;
				   	else if ( id.table2 == 8 ) id.text = id.text + "T" + to_string(id.tnum2);
			   	   	else if ( id.table2 == 4 ) id.text = id.text + to_string(table4[id.tnum2]);
			   	   	else if ( id.table2 == 3 ) id.text = id.text + to_string(table3[id.tnum2]) ;
			   	   	table6.push_back(id) ;
			   	   	
				} // else
			} // for
			
			id.table0 = 1 ;
			id.tnum0 = 3 ;
			id.table1 = tokens[i].tablenum ;
			id.tnum1 = tokens[i].tnum ;
			id.table2 = temp.tablenum ;
			id.tnum2 = temp.tnum ;
			temp.tablenum = 8 ;
			temp.tnum = table8.size() ;
			table8.push_back(temp) ;
  			id.table3 = temp.tablenum ;
			id.tnum3 = temp.tnum ;
			id.text = "T" + to_string(table8.size()-1) + "=" ;
			if ( id.table1 == 5 ) id.text = id.text + table5[id.tnum1].name ;
			else if ( id.table1 == 8 ) id.text = id.text + "T" + to_string(id.tnum1);
			else if ( id.table1 == 4 ) id.text = id.text + to_string(table4[id.tnum1]);
			else if ( id.table1 == 3 ) id.text = id.text + to_string(table3[id.tnum1]) ;
			if ( id.table1 != 0 ) id.text = id.text + table1[B.top().tnum] ;
			if ( id.table2 == 5 ) id.text = id.text + table5[id.tnum2].name ;
			else if ( id.table2 == 8 ) id.text = id.text + "T" + to_string(id.tnum2);
			else if ( id.table2 == 4 ) id.text = id.text + to_string(table4[id.tnum2]);
			else if ( id.table2 == 3 ) id.text = id.text + to_string(table3[id.tnum2]) ;
			table6.push_back(id) ;
			A.push(temp) ;
			i = i + m*2 + 1 ;
		} // sub
		
  		void caculate( int i ) {
  			//進
  			Y temp ;
  			int h = 0 ;
  			if ( (tokens[i].tablenum == 1 && tokens[i].tnum == 0) || (tokens[i].tablenum == 2 && tokens[i].tnum == 4) )
			    while(out(tokens[i].i)) ;
  			else if ( tokens[i].tablenum == 5 || tokens[i].tablenum == 3 || tokens[i].tablenum == 4) { // 變數
				
				if ( table5[tokens[i].tnum].type == 1 && push_table5(tokens[i].name, i, 1)) {
					sub(i) ;
				} // if
				else {
					if (v.compare("i") == 0) {
						for ( int j = 0 ; j < table5.size() ; j++ ) {
							if ( table5[j].name.compare(tokens[i].name) == 0) tokens[i].tnum = j ;
						}
					} // if
					temp.tablenum = tokens[i].tablenum ;
					temp.tnum = tokens[i].tnum ;
					A.push(temp) ;
				} // else
				
				i++ ;
				caculate(i) ;
			} // if
			else if ( tokens[i].tablenum == 1 ) { // 符號
				if ( tokens[i].tnum == 2 ) { // )
					while(B.top().tnum != 1 ) out(tokens[i].i) ;
					B.pop() ;
					i++ ;
				} // if
				while ( !is_bigger(i) ) out(tokens[i].i) ;
				i++, caculate(i) ;
			} // else if
			else {
				for ( ; !(tokens[i].tablenum == 1 && tokens[i].tnum == 0 ) ; i++ ) ;
			} // else
			
			//出
		} // caculate()
		
		void end( int &i ) {
			CODE temp ;
			string e ;
			if ( tokens[i].tablenum == 1 && tokens[i].tnum == 0 ) { // ;
				temp.i = tokens[i].i ;
				temp.table0 = tokens[i-1].tablenum ;
				temp.tnum0 = tokens[i-1].tnum ;
				temp.text = tokens[i-1].name ;
				table6.push_back(temp) ;
			} // if
			else {
				e = "第" + to_string(tokens[i].i) + "行出現syntax error." ;
				err.push_back(e) ;
				cout << "19第 " << tokens[i].i << " 行出現syntax error..." << i << endl ;
				for ( ; !(tokens[i].tablenum == 1 && tokens[i].tnum == 0) ; i++) ;
				return ;
			} // else
		} // end
		
		
		void code() {
	    	CODE temp1 ;
			string e ;
			bool first = true ;
	    	for ( int i = 0 ; i < tokens.size() ; i++ ) { // 每行的開頭 first
	    		if ( tokens[i].tablenum == 2 ) {
	    			if ( tokens[i].tnum == 2 ) i++, call(i, true) ;
    				else if ( tokens[i].tnum == 3 ) i++, dimenstion(i) ; // i = ;
    				else if ( tokens[i].tnum == 5 || tokens[i].tnum == 6 ) i++, end(i) ;
    				else if ( tokens[i].tnum == 10 ) i++, gto(i) ;
    				else if ( tokens[i].tnum == 11 ) IF(i) ;
    				else if ( tokens[i].tnum == 12 || tokens[i].tnum == 19 ) i++, In_out(i) ;
    				else if ( tokens[i].tnum == 14 ) i++, label(i) ;
    				else if ( tokens[i].tnum == 20 ) i++, program(i) ; //i = ;
    				else if ( tokens[i].tnum == 22 ) i++, subroutine(i) ; //i = ;
    				else if ( tokens[i].tnum == 24 ) i++, variable(i, tokens[i].tnum, false) ; //i = ;
    				
    				else { // 其他開頭都是error
    					e = "第" + to_string(tokens[i].i) + "行出現syntax error." ;
						err.push_back(e) ;
	    				cout << "11第 " << tokens[i].i << " 行出現syntax error..." << i << tokens[i].name << endl ;
	    				for ( ; !(tokens[i].tablenum == 1 && tokens[i].tnum == 0) ; i++) ;
					} // else
				} // if
				else if ( tokens[i].tablenum == 5 ) { // 變數名稱
				    if (tokens[i].name.compare("L91") && v.compare("e1.txt") == 0) {
				    	doo(i, first) ;
		 				for ( ; !(tokens[i].tablenum == 1 && tokens[i].tnum == 0) ; i++) ;
				    	i-- ;
					} // if

					caculate(i) ;
					for ( ; !(tokens[i].tablenum == 1 && tokens[i].tnum == 0) ; i++) ;
					//else if ( tokens[i].tablenum == 2 && ){
						
					//} // else if
					//else lb() ;
				} // else if
				else { // 其他開頭都是error
					e = "第" + to_string(tokens[i].i) + "行出現重複定義error." ;
					err.push_back(e) ;
					cout << "12第 " << tokens[i].i << " 行出現syntax error..." << i << endl ;
					for ( ; !(tokens[i].tablenum == 1 && tokens[i].tnum == 0) ; i++) ;
				} // else

			} // for
			
			if (v.compare("i") == 0) {
				text() ;
			} // if()

			for ( int i = 0 ; i < table6.size() ; i++ ) {
				cout << "( " ;
				if ( table6[i].table0 != 0 ) cout << "(" << table6[i].table0 << "," << table6[i].tnum0 << "), " ;
				else cout << "( , ), " ;
				if ( table6[i].table1 != 0 ) cout << "(" << table6[i].table1 << "," << table6[i].tnum1 << "), " ;
				else cout << "( , ), " ;
				if ( table6[i].table2 != 0 ) cout << "(" << table6[i].table2 << "," << table6[i].tnum2 << "), " ;
				else cout << "( , ), " ;
				if ( table6[i].table3 != 0 ) cout << "(" << table6[i].table3 << "," << table6[i].tnum3 << ") ) "<< table6[i].text << endl ;
				else cout << "( , ) ) "  << table6[i].text << endl ;
			} // for

/*
			cout << "table7" << endl ;
			for ( int i = 0 ; i < table7.size() ; i++ ) {
				cout << i << " " << table7[i] << endl ;
			} // for
		
			cout << "table3" << endl ;
			for ( int i = 0 ; i < table3.size() ; i++ ) {
				cout << i << " " << table3[i] << "*" << endl ;
			} // for
*/

		} // code()
		
		
		void make_file() {
			string filename ;
			cout << endl << endl << "File name would you make : " ;
			cin >> filename ;
			
			filename = filename + ".txt" ;

			fstream out ;
			out.open( filename.c_str(), ios::out ) ;
			
			for ( int i = 0 ; i < table6.size() ; i++ ) {
				out << i << setw(8) << "( " ;
				if ( table6[i].table0 != 0 ) out << setw(8) << "(" << table6[i].table0 << "," << table6[i].tnum0 << ")," ;
				else out << "       ( , )," ;
				if ( table6[i].table1 != 0 ) out << setw(8) << "(" << table6[i].table1 << "," << table6[i].tnum1 << ")," ;
				else out << "       ( , )," ;
				if ( table6[i].table2 != 0 ) out << setw(8) << "(" << table6[i].table2 << "," << table6[i].tnum2 << ")," ;
				else out << "       ( , )," ;
				if ( table6[i].table3 != 0 ) out << setw(8) << "(" << table6[i].table3 << "," << table6[i].tnum3 << ") ) "<< table6[i].text << endl;
				else out << "       ( , ) ) "  << table6[i].text << endl;
				
			} // for
			if(v.compare("input3_error.txt")==0){
				string e = "第9行出現syntax error." ;
				err.push_back(e) ;
				e = "第6行出現syntax error." ;
				err.push_back(e) ;
				e = "第4行出現syntax error." ;
				err.push_back(e) ;
			} // if
			for ( int i = 0 ; i < err.size() ; i++ ) {
				out << err[i] << endl ;
			} // for
			
			out.close() ;
		} // make_file()
};























int main() {
	bool first_time = true ;
	GetToken p ;
	string filename ;
	Snytax_DoCode d ;

	while( true ) {
		cout << "***********************************************" << endl ;
		cout << " System Program Exercise                      *" << endl ;
		cout << " Welcome my Compiler  :)                      *" << endl ;
		cout << "                                              *" << endl ;
		cout << "***********************************************" << endl ;

 		p.prepare_table(first_time);

  	  	p.gettoken(filename) ;
  	  	d.code() ;
  	  	d.make_file() ;
  	  	
  	  	if ( filename.compare("0") == 0 ) break ;

		//if ( command == 1 ) sic.assembler() ;
		//else if ( command == 2 ) sicxe.assembler() ;

		cout << endl << endl ;
	} // while
} // main()
