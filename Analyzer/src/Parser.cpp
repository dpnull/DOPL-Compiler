// Lexical Analyser 
#include <iostream>
#include <string>
#include <map>

using namespace std;

enum Symbol {
	START, FINISH, INTEGER, CHARACTER, LOGICAL, IDENTIFIER,
	INTEGER_CONSTANT, CHARACTER_CONSTANT, PLUS, MINUS, MUL, DIV,
	NOT, AND, OR, EQ, NE, LT, GT, LE, GE, SEMICOLON, COMMA, ASSIGN,
	PRINT, IF, THEN, ELSE, ENDIF, LOOPIF, DO, ENDLOOP,
	L_PAREN, R_PAREN, EOFIELD
};

map<string, Symbol> reservedWords = {
	{"start", START}, {"finish", FINISH}, {"integer", INTEGER},
	{"character", CHARACTER}, {"logical", LOGICAL},
	{"if", IF}, {"then", THEN}, {"else", ELSE}, {"endif", ENDIF},
	{"loopif", LOOPIF}, {"do", DO}, {"endloop", ENDLOOP},
	{"print", PRINT}
};

class LexicalAnalyser {
private:
	string text;
	int pos;
	Symbol symbol;
	string lexeme;
public:
	LexicalAnalyser(string text) {
		this->text = text;
		pos = 0;
		this->symbol = EOFIELD;
		this->lexeme = "";
	}

	void getNextToken() {
		// Skip whitespace
		while (isspace(text[pos])) pos++;

		// end of source reached
		if (pos >= text.length()) {
			symbol = EOFIELD;
			return;
		}

		// identifiers
		if (isalpha(text[pos])) {
			lexeme = "";
			while (isalnum(text[pos]) || text[pos] == '_') {
				lexeme += text[pos];
				pos++;
			}
			if (reservedWords.find(lexeme) != reservedWords.end()) {
				symbol = reservedWords[lexeme];
			}
			else {
				symbol = IDENTIFIER;
			}
			return;
		}

		// constants for integers
		if (isdigit(text[pos])) {
			lexeme = "";
			while (isdigit(text[pos])) {
				lexeme += text[pos];
				pos++;
			}
			symbol = INTEGER_CONSTANT;
			return;
		}

		// character constants
		if (text[pos] == '"') {
			pos++;
			lexeme = text[pos];
			pos += 2;
			symbol = CHARACTER_CONSTANT;
			return;
		}

		// ops
		if (text[pos] == '.') {
			lexeme = "";
			while (text[pos] == '.') {
				lexeme += text[pos];
				pos++;
			}
			if (lexeme == ".plus.") symbol = PLUS;
			else if (lexeme == ".minus.") symbol = MINUS;
			else if (lexeme == ".mul.") symbol = MUL;
			else if (lexeme == ".div.") symbol = DIV;
			else if (lexeme == ".and.") symbol = AND;
			else if (lexeme == ".or.") symbol = OR;
			else if (lexeme == ".eq.") symbol = EQ;
			else if (lexeme == ".ne.") symbol = NE;
			else if (lexeme == ".lt.") symbol = LT;
			else if (lexeme == ".gt.") symbol = GT;
			else if (lexeme == ".le.") symbol = LE;
			else if (lexeme == ".ge.") symbol = GE;
			return;
		}

		// misc tokens (breaks when more is added)
		lexeme = text[pos];
		switch (lexeme[0]) {
			case ';': symbol = SEMICOLON; break;
			case '<': symbol = ASSIGN; break;
			case '(': symbol = L_PAREN; break;
			case ')': symbol = R_PAREN; break;
			default: break;
		}
		pos++;
	}

	Symbol getSymbol() { return symbol; }
	string getLexeme() { return lexeme; }
};

// Recursive Descent Parser
#include <iostream>
#include <string>
#include <stack>

using namespace std;

class Parser {
private:
	LexicalAnalyser lex;
	Symbol currSym;
public:
	Parser(string text) : lex(text) {
		lex.getNextToken();
		currSym = lex.getSymbol();
	}

	bool program() {
		if (currSym == START) {
			match(START);
			if (declarations() && statements()) {
				match(FINISH);
				return true;
			}
		}
		return false;
	}

	bool declarations() {
		if (currSym == INTEGER || currSym == CHARACTER || currSym == LOGICAL) {
			if (declaration()) {
				while (currSym == SEMICOLON) {
					match(SEMICOLON);
					if (!declaration()) return true;
				}
				return true;
			}
		}
		return true;
	}

	bool declaration() {
		Symbol type = dataType();
		if (type != EOFIELD) {
			if (identifiers(type)) return true;
		}
		return false;
	}

	Symbol dataType() {
		if (currSym == INTEGER) {
			match(INTEGER);
			return INTEGER;
		}
		if (currSym == CHARACTER) {
			match(CHARACTER);
			return CHARACTER;
		}
		if (currSym == LOGICAL) {
			match(LOGICAL);
			return LOGICAL;
		}
		return EOFIELD;
	}

	bool identifiers(Symbol type) {
		if (currSym == IDENTIFIER) {
			match(IDENTIFIER);
			while (currSym == COMMA) {
				match(COMMA);
				match(IDENTIFIER);
			}
			return true;
		}
		return false;
	}

	// parses a series of statements and returns true if they are valid
	bool statements() {
		if (currSym == IDENTIFIER || currSym == PRINT
			|| currSym == IF || currSym == LOOPIF) {
			if (statement()) {
				while (currSym == SEMICOLON) {
					match(SEMICOLON);
					if (!statement()) return false;
				}
				return true;
			}
		}
		return true;
	}

	bool statement() {
		switch (currSym) {
			case IDENTIFIER:
				return assignment();
			case PRINT:
				return print();
			case IF:
				return conditional();
			case LOOPIF:
				return loop();
			default:
				return false;
		}
	}

	bool assignment() {
    if (currSym == IDENTIFIER) {
        match(IDENTIFIER);
        if (match(ASSIGN) && expression()) {
            return true;
        }
    }
    return false;
}
	// implements a parser for a conditional statement
	bool conditional() {
		if (currSym == IF) {
			match(IF);
			if (expression() && match(THEN)) {
				if (statements()) {
					if (currSym == ELSE) {
						match(ELSE);
						if (!statements()) return false;
					}
					if (match(ENDIF)) return true;
				}
			}
		}
		return false;
	}

	bool print() {
		if (currSym == PRINT) {
			match(PRINT);
			if (expression()) return true;
		}
		return false;
	}

	// checks for the presence of a loopif statement in the code
	bool loop() {
		if (currSym == LOOPIF) {
			match(LOOPIF);
			if (expression() && match(DO) && statements()) {
				if (match(ENDLOOP)) return true;
			}
		}
		return false;
	}

	bool expression() {
		if (term() && binaryOpTerm()) return true;
		return false;
	}

	// checks if the current symbol is a binary operator
	bool binaryOpTerm() {
		if (currSym == PLUS || currSym == MINUS
			|| currSym == MUL || currSym == DIV
			|| currSym == AND || currSym == OR
			|| currSym == EQ || currSym == NE
			|| currSym == LT || currSym == GT
			|| currSym == LE || currSym == GE) {
			Symbol op = currSym;
			match(op);
			if (term()) return true;
		}
		return true;
	}

	// checks if the current symbol is valid for a term
	bool term() {
		switch (currSym) {
			case INTEGER_CONSTANT:
			case CHARACTER_CONSTANT:
			case IDENTIFIER:
				match(currSym);
				return true;
			case L_PAREN:
				match(L_PAREN);
				if (expression() && match(R_PAREN)) return true;
				return false;
			case MINUS:
			case NOT:
				Symbol op = currSym;
				match(op);
				if (term()) return true;
				return false;
		}
        return false;
	}

	bool match(Symbol s) {
		if (currSym == s) {
			lex.getNextToken();
			currSym = lex.getSymbol();
			return true;
		}
		return false;
	}
};
