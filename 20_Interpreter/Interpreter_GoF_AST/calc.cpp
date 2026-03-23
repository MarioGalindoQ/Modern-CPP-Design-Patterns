/**
 * ============================================================================
 * File: calc.cpp (Interpreter: GoF AST Version)
 * 
 * --- DESIGN OVERVIEW:
 * This implementation follows the classic Gang of Four Interpreter pattern.
 * 1. Parser (Recursive Descent): Reads the string and builds an Abstract 
 *    Syntax Tree (AST) of polymorphic objects in memory.
 * 2. Evaluation: We call evaluate() on the root node, which recursively 
 *    calls evaluate() on its children to compute the final result.
 * 
 * Usage: ./calc [-p] "<math_expression>"
 * The '-p' flag prints the object tree (AST) before evaluating.
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <memory>
#include <cmath>
#include <stdexcept>
#include <cctype>

//=============================================================================
// BLOCK 1: LEXER (Scanner)
//=============================================================================
enum class TokenType 
{ 
   NUMBER, FUNCTION, PLUS, MINUS, MUL, DIV, POW, LPAREN, RPAREN, END_OF_FILE 
};

struct Token 
{
   TokenType type;
   double value{0.0};
   std::string name{""};
};

class Lexer
{
private:
   std::string input_;
   size_t pos_{0};
   Token currentToken_;

   void advanceChar() { pos_++; }
   char peek() const { return (pos_ < input_.size()) ? input_[pos_] : '\0'; }

public:
   explicit Lexer(std::string input) : input_{std::move(input)} 
   {
      nextToken(); // Load first token
   }

   Token getToken() const { return currentToken_; }

   void nextToken()
   {
      while (isspace(peek())) advanceChar();

      if (pos_ >= input_.size())
      {
         currentToken_ = {TokenType::END_OF_FILE};
         return;
      }

      char c = peek();

      if (isdigit(c) || c == '.')
      {
         size_t length;
         double val = std::stod(input_.substr(pos_), &length);
         pos_ += length;
         currentToken_ = {TokenType::NUMBER, val, ""};
      }
      else if (isalpha(c))
      {
         std::string funcName = "";
         while (isalpha(peek()))
         {
            funcName += peek();
            advanceChar();
         }
         currentToken_ = {TokenType::FUNCTION, 0.0, funcName};
      }
      else
      {
         switch (c)
         {
            case '+': currentToken_ = {TokenType::PLUS}; break;
            case '-': currentToken_ = {TokenType::MINUS}; break;
            case '*': currentToken_ = {TokenType::MUL}; break;
            case '/': currentToken_ = {TokenType::DIV}; break;
            case '^': currentToken_ = {TokenType::POW}; break;
            case '(': currentToken_ = {TokenType::LPAREN}; break;
            case ')': currentToken_ = {TokenType::RPAREN}; break;
            default:  throw std::runtime_error(std::string("Syntax error: Unknown character: ") + c);
         }
         advanceChar();
      }
   }
};

//=============================================================================
// BLOCK 2: ABSTRACT SYNTAX TREE (GoF Interpreter Nodes)
//=============================================================================
class Expression
{
public:
   virtual ~Expression() = default;
   virtual double evaluate() const = 0;
   virtual void print(int indent = 0) const = 0;

protected:
   void printIndent(int indent) const
   {
      std::cout << "    ";
      for (int i = 0; i < indent; ++i) std::cout << "   ";
   }
};

using ExprPtr = std::unique_ptr<Expression>;

// --- Terminal Expression ---
class NumberNode : public Expression
{
private:
   double value_;
public:
   explicit NumberNode(double value) : value_{value} {}
   double evaluate() const override { return value_; }
   void print(int indent) const override 
   { 
      printIndent(indent); 
      std::cout << "Number: " << value_ << "\n"; 
   }
};

// --- Non-Terminal Expressions ---
class BinaryExpression : public Expression
{
protected:
   ExprPtr left_;
   ExprPtr right_;
public:
   BinaryExpression(ExprPtr left, ExprPtr right) 
      : left_{std::move(left)}, right_{std::move(right)} {}
};

class AddNode : public BinaryExpression
{
public:
   using BinaryExpression::BinaryExpression;
   double evaluate() const override { return left_->evaluate() + right_->evaluate(); }
   void print(int indent) const override
   {
      printIndent(indent); std::cout << "Add\n";
      left_->print(indent + 1);
      right_->print(indent + 1);
   }
};

class SubNode : public BinaryExpression
{
public:
   using BinaryExpression::BinaryExpression;
   double evaluate() const override { return left_->evaluate() - right_->evaluate(); }
   void print(int indent) const override
   {
      printIndent(indent); std::cout << "Subtract\n";
      left_->print(indent + 1);
      right_->print(indent + 1);
   }
};

class MulNode : public BinaryExpression
{
public:
   using BinaryExpression::BinaryExpression;
   double evaluate() const override { return left_->evaluate() * right_->evaluate(); }
   void print(int indent) const override
   {
      printIndent(indent); std::cout << "Multiply\n";
      left_->print(indent + 1);
      right_->print(indent + 1);
   }
};

class DivNode : public BinaryExpression
{
public:
   using BinaryExpression::BinaryExpression;
   double evaluate() const override 
   { 
      double r = right_->evaluate();
      if (r == 0.0) throw std::runtime_error("Error: Division by zero");
      return left_->evaluate() / r; 
   }
   void print(int indent) const override
   {
      printIndent(indent); std::cout << "Divide\n";
      left_->print(indent + 1);
      right_->print(indent + 1);
   }
};

class PowNode : public BinaryExpression
{
public:
   using BinaryExpression::BinaryExpression;
   double evaluate() const override { return std::pow(left_->evaluate(), right_->evaluate()); }
   void print(int indent) const override
   {
      printIndent(indent); std::cout << "Power\n";
      left_->print(indent + 1);
      right_->print(indent + 1);
   }
};

class UnaryMinusNode : public Expression
{
private:
   ExprPtr expr_;
public:
   explicit UnaryMinusNode(ExprPtr expr) : expr_{std::move(expr)} {}
   double evaluate() const override { return -expr_->evaluate(); }
   void print(int indent) const override
   {
      printIndent(indent); std::cout << "UnaryMinus\n";
      expr_->print(indent + 1);
   }
};

class MathFunctionNode : public Expression
{
private:
   std::string func_;
   ExprPtr expr_;
public:
   MathFunctionNode(std::string func, ExprPtr expr) 
      : func_{std::move(func)}, expr_{std::move(expr)} {}

   double evaluate() const override
   {
      double val = expr_->evaluate();
      if (func_ == "sin")  return std::sin(val);
      if (func_ == "cos")  return std::cos(val);
      if (func_ == "tan")  return std::tan(val);
      if (func_ == "asin") return std::asin(val);
      if (func_ == "acos") return std::acos(val);
      if (func_ == "atan") return std::atan(val);
      if (func_ == "exp")  return std::exp(val);
      if (func_ == "sqrt") {
         if (val < 0) throw std::runtime_error("Error: sqrt of negative number");
         return std::sqrt(val);
      }
      if (func_ == "ln") {
         if (val <= 0) throw std::runtime_error("Error: ln of non-positive number");
         return std::log(val);
      }
      if (func_ == "log") {
         if (val <= 0) throw std::runtime_error("Error: log of non-positive number");
         return std::log10(val);
      }
      throw std::runtime_error("Error: Unknown function: " + func_);
   }

   void print(int indent) const override
   {
      printIndent(indent); std::cout << "Function [" << func_ << "]\n";
      expr_->print(indent + 1);
   }
};

//=============================================================================
// BLOCK 3: PARSER (Builds the AST)
//=============================================================================
class Parser
{
private:
   Lexer lexer_;

   void match(TokenType expected)
   {
      if (lexer_.getToken().type == expected) lexer_.nextToken();
      else throw std::runtime_error("Syntax error: Unexpected token");
   }

   ExprPtr parsePrimary()
   {
      Token t = lexer_.getToken();
      if (t.type == TokenType::NUMBER)
      {
         match(TokenType::NUMBER);
         return std::make_unique<NumberNode>(t.value);
      }
      if (t.type == TokenType::LPAREN)
      {
         match(TokenType::LPAREN);
         ExprPtr expr = parseExpression();
         match(TokenType::RPAREN);
         return expr;
      }
      if (t.type == TokenType::FUNCTION)
      {
         std::string func = t.name;
         match(TokenType::FUNCTION);
         match(TokenType::LPAREN);
         ExprPtr expr = parseExpression();
         match(TokenType::RPAREN);
         return std::make_unique<MathFunctionNode>(func, std::move(expr));
      }
      throw std::runtime_error("Syntax error in primary expression");
   }

   ExprPtr parseSigned()
   {
      Token t = lexer_.getToken();
      if (t.type == TokenType::PLUS)
      {
         match(TokenType::PLUS);
         return parsePrimary();
      }
      if (t.type == TokenType::MINUS)
      {
         match(TokenType::MINUS);
         return std::make_unique<UnaryMinusNode>(parsePrimary());
      }
      return parsePrimary();
   }

   ExprPtr parseExponent()
   {
      ExprPtr left = parseSigned();
      while (lexer_.getToken().type == TokenType::POW)
      {
         match(TokenType::POW);
         ExprPtr right = parseSigned();
         left = std::make_unique<PowNode>(std::move(left), std::move(right));
      }
      return left;
   }

   ExprPtr parseMultiplicative()
   {
      ExprPtr left = parseExponent();
      while (lexer_.getToken().type == TokenType::MUL || 
             lexer_.getToken().type == TokenType::DIV)
      {
         TokenType op = lexer_.getToken().type;
         match(op);
         ExprPtr right = parseExponent();
         
         if (op == TokenType::MUL) left = std::make_unique<MulNode>(std::move(left), std::move(right));
         else left = std::make_unique<DivNode>(std::move(left), std::move(right));
      }
      return left;
   }

   ExprPtr parseAdditive()
   {
      ExprPtr left = parseMultiplicative();
      while (lexer_.getToken().type == TokenType::PLUS || 
             lexer_.getToken().type == TokenType::MINUS)
      {
         TokenType op = lexer_.getToken().type;
         match(op);
         ExprPtr right = parseMultiplicative();

         if (op == TokenType::PLUS) left = std::make_unique<AddNode>(std::move(left), std::move(right));
         else left = std::make_unique<SubNode>(std::move(left), std::move(right));
      }
      return left;
   }

   ExprPtr parseExpression()
   {
      return parseAdditive();
   }

public:
   explicit Parser(const std::string& source) : lexer_(source) { }

   ExprPtr parse()
   {
      ExprPtr tree = parseExpression();
      if (lexer_.getToken().type != TokenType::END_OF_FILE)
         throw std::runtime_error("Syntax error: Unexpected tokens at end");
      return tree;
   }
};

//=============================================================================
// BLOCK 4: COMMAND LINE INTERFACE (Main)
//=============================================================================
int main(int argc, char* argv[])
{
   std::cout << "=== INTERPRETER PATTERN (GoF AST VERSION) ===\n" << std::endl;

   if (argc < 2 || argc > 3)
   {
      std::cerr << "Usage: ./calc [-p] \"<math_expression>\"\n"
                << "Note: Trigonometric functions are in radians\n";
      return 1;
   }

   bool printInternals = false;
   std::string sourceCode;

   if (argc == 3)
   {
      std::string flag = argv[1];
      if (flag == "-p") printInternals = true;
      else
      {
         std::cerr << "Error: Unknown flag '" << flag << "'. Only '-p' is supported.\n";
         return 1;
      }
      sourceCode = argv[2];
   }
   else sourceCode = argv[1];

   try
   {
      // 1. Parse source code into an Abstract Syntax Tree
      Parser parser(sourceCode);
      ExprPtr astRoot = parser.parse();

      // 2. Optionally print the AST
      if (printInternals)
      {
         std::cout << "--- Abstract Syntax Tree ---\n";
         astRoot->print();
         std::cout << "----------------------------\n\n";
      }

      // 3. Evaluate the AST recursively and print the result
      double result = astRoot->evaluate();
      std::cout << "Result: " << result << "\n";
   }
   catch (const std::exception& e)
   {
      std::cerr << e.what() << "\n";
      return 1;
   }
}

//================================================================================ END
