
#include <vector>
#include "Program.h"
#include "Declarations.h"
#include "Statements.h"
#include "Exprs.h"
#include "Terms.h"
#include <iostream>


// Generated from MiniC.g4 by ANTLR 4.9

#pragma once


#include "antlr4-runtime.h"




class  MiniCParser : public antlr4::Parser {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, T__9 = 10, T__10 = 11, T__11 = 12, T__12 = 13, T__13 = 14, 
    T__14 = 15, T__15 = 16, T__16 = 17, T__17 = 18, T__18 = 19, T__19 = 20, 
    T__20 = 21, T__21 = 22, T__22 = 23, T__23 = 24, T__24 = 25, T__25 = 26, 
    T__26 = 27, T__27 = 28, T__28 = 29, T__29 = 30, T__30 = 31, T__31 = 32, 
    T__32 = 33, T__33 = 34, T__34 = 35, ID = 36, INT = 37, WS = 38, COMMENT = 39
  };

  enum {
    RuleProg = 0, RulePreamble = 1, RuleDecl = 2, RuleVardecl = 3, RuleScope = 4, 
    RuleStmt = 5, RuleVarlist = 6, RuleVarlistentry = 7, RuleVartype = 8, 
    RuleRettype = 9, RuleParameters = 10, RuleParameterlist = 11, RuleParameterentry = 12, 
    RuleExpropt = 13, RuleExpr = 14, RuleVar = 15, RuleArguments = 16, RuleArgumentlist = 17, 
    RuleVarname = 18, RuleFuncname = 19, RuleParametername = 20
  };

  explicit MiniCParser(antlr4::TokenStream *input);
  ~MiniCParser();

  virtual std::string getGrammarFileName() const override;
  virtual const antlr4::atn::ATN& getATN() const override { return _atn; };
  virtual const std::vector<std::string>& getTokenNames() const override { return _tokenNames; }; // deprecated: use vocabulary instead.
  virtual const std::vector<std::string>& getRuleNames() const override;
  virtual antlr4::dfa::Vocabulary& getVocabulary() const override;


  class ProgContext;
  class PreambleContext;
  class DeclContext;
  class VardeclContext;
  class ScopeContext;
  class StmtContext;
  class VarlistContext;
  class VarlistentryContext;
  class VartypeContext;
  class RettypeContext;
  class ParametersContext;
  class ParameterlistContext;
  class ParameterentryContext;
  class ExproptContext;
  class ExprContext;
  class VarContext;
  class ArgumentsContext;
  class ArgumentlistContext;
  class VarnameContext;
  class FuncnameContext;
  class ParameternameContext; 

  class  ProgContext : public antlr4::ParserRuleContext {
  public:
    minicc::Program * val;
    MiniCParser::DeclContext *declContext = nullptr;
    ProgContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EOF();
    PreambleContext *preamble();
    std::vector<DeclContext *> decl();
    DeclContext* decl(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ProgContext* prog();

  class  PreambleContext : public antlr4::ParserRuleContext {
  public:
    PreambleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  PreambleContext* preamble();

  class  DeclContext : public antlr4::ParserRuleContext {
  public:
    minicc::Declaration * val;
    minicc::FuncDeclaration * func;
    MiniCParser::VardeclContext *vardeclContext = nullptr;
    MiniCParser::RettypeContext *rettypeContext = nullptr;
    MiniCParser::FuncnameContext *funcnameContext = nullptr;
    MiniCParser::ParametersContext *parametersContext = nullptr;
    MiniCParser::ScopeContext *scopeContext = nullptr;
    DeclContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    VardeclContext *vardecl();
    RettypeContext *rettype();
    FuncnameContext *funcname();
    ParametersContext *parameters();
    ScopeContext *scope();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  DeclContext* decl();

  class  VardeclContext : public antlr4::ParserRuleContext {
  public:
    minicc::VarDeclaration * val;
    MiniCParser::VartypeContext *vartypeContext = nullptr;
    MiniCParser::VarlistContext *varlistContext = nullptr;
    VardeclContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    VartypeContext *vartype();
    VarlistContext *varlist();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  VardeclContext* vardecl();

  class  ScopeContext : public antlr4::ParserRuleContext {
  public:
    minicc::ScopeStatement * val;
    size_t declcnt;
    MiniCParser::VardeclContext *vardeclContext = nullptr;
    MiniCParser::StmtContext *stmtContext = nullptr;
    ScopeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<VardeclContext *> vardecl();
    VardeclContext* vardecl(size_t i);
    std::vector<StmtContext *> stmt();
    StmtContext* stmt(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ScopeContext* scope();

  class  StmtContext : public antlr4::ParserRuleContext {
  public:
    minicc::Statement * val;
    MiniCParser::ExprContext *exprContext = nullptr;
    MiniCParser::StmtContext *stmtContext = nullptr;
    MiniCParser::StmtContext *s1 = nullptr;
    MiniCParser::StmtContext *s2 = nullptr;
    MiniCParser::ExproptContext *e1 = nullptr;
    MiniCParser::ExproptContext *e2 = nullptr;
    MiniCParser::ExproptContext *e3 = nullptr;
    MiniCParser::ScopeContext *scopeContext = nullptr;
    StmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExprContext *expr();
    std::vector<StmtContext *> stmt();
    StmtContext* stmt(size_t i);
    std::vector<ExproptContext *> expropt();
    ExproptContext* expropt(size_t i);
    ScopeContext *scope();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  StmtContext* stmt();

  class  VarlistContext : public antlr4::ParserRuleContext {
  public:
    std::vector<minicc::VarReference*> val;
    MiniCParser::VarlistContext *v1 = nullptr;
    MiniCParser::VarlistentryContext *varlistentryContext = nullptr;
    VarlistContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    VarlistentryContext *varlistentry();
    VarlistContext *varlist();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  VarlistContext* varlist();
  VarlistContext* varlist(int precedence);
  class  VarlistentryContext : public antlr4::ParserRuleContext {
  public:
    minicc::VarReference* val;
    minicc::ConstantLiteralExpr * tmp;
    MiniCParser::VarnameContext *varnameContext = nullptr;
    antlr4::Token *intToken = nullptr;
    VarlistentryContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    VarnameContext *varname();
    antlr4::tree::TerminalNode *INT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  VarlistentryContext* varlistentry();

  class  VartypeContext : public antlr4::ParserRuleContext {
  public:
    minicc::TypeReference* val;
    VartypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  VartypeContext* vartype();

  class  RettypeContext : public antlr4::ParserRuleContext {
  public:
    minicc::TypeReference* val;
    MiniCParser::VartypeContext *vartypeContext = nullptr;
    RettypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    VartypeContext *vartype();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  RettypeContext* rettype();

  class  ParametersContext : public antlr4::ParserRuleContext {
  public:
    std::vector<minicc::Parameter*> val;
    MiniCParser::ParameterlistContext *parameterlistContext = nullptr;
    ParametersContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ParameterlistContext *parameterlist();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ParametersContext* parameters();

  class  ParameterlistContext : public antlr4::ParserRuleContext {
  public:
    std::vector<minicc::Parameter*> val;
    MiniCParser::ParameterlistContext *p1 = nullptr;
    MiniCParser::ParameterentryContext *parameterentryContext = nullptr;
    ParameterlistContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ParameterentryContext *parameterentry();
    ParameterlistContext *parameterlist();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ParameterlistContext* parameterlist();
  ParameterlistContext* parameterlist(int precedence);
  class  ParameterentryContext : public antlr4::ParserRuleContext {
  public:
    minicc::Parameter* val;
    MiniCParser::VartypeContext *vartypeContext = nullptr;
    MiniCParser::ParameternameContext *parameternameContext = nullptr;
    ParameterentryContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    VartypeContext *vartype();
    ParameternameContext *parametername();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ParameterentryContext* parameterentry();

  class  ExproptContext : public antlr4::ParserRuleContext {
  public:
    minicc::Expr * val;
    MiniCParser::ExprContext *exprContext = nullptr;
    ExproptContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExprContext *expr();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ExproptContext* expropt();

  class  ExprContext : public antlr4::ParserRuleContext {
  public:
    minicc::Expr * val;
    bool compare;
    minicc::VarReference * tmp;
    bool minus_flag;
    minicc::Expr * inttmp;
    MiniCParser::ExprContext *e1 = nullptr;
    antlr4::Token *intToken = nullptr;
    MiniCParser::ExprContext *exprContext = nullptr;
    MiniCParser::VarContext *varContext = nullptr;
    MiniCParser::FuncnameContext *funcnameContext = nullptr;
    MiniCParser::ArgumentsContext *argumentsContext = nullptr;
    MiniCParser::ParameternameContext *parameternameContext = nullptr;
    antlr4::Token *op = nullptr;
    MiniCParser::ExprContext *e2 = nullptr;
    ExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *INT();
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    VarContext *var();
    FuncnameContext *funcname();
    ArgumentsContext *arguments();
    ParameternameContext *parametername();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ExprContext* expr();
  ExprContext* expr(int precedence);
  class  VarContext : public antlr4::ParserRuleContext {
  public:
    minicc::VarReference * val;
    MiniCParser::VarnameContext *varnameContext = nullptr;
    MiniCParser::ExprContext *exprContext = nullptr;
    VarContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    VarnameContext *varname();
    ExprContext *expr();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  VarContext* var();

  class  ArgumentsContext : public antlr4::ParserRuleContext {
  public:
    std::vector<minicc::Expr*> val;
    MiniCParser::ArgumentlistContext *argumentlistContext = nullptr;
    ArgumentsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ArgumentlistContext *argumentlist();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ArgumentsContext* arguments();

  class  ArgumentlistContext : public antlr4::ParserRuleContext {
  public:
    std::vector<minicc::Expr*> val;
    MiniCParser::ArgumentlistContext *a1 = nullptr;
    MiniCParser::ExprContext *exprContext = nullptr;
    ArgumentlistContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExprContext *expr();
    ArgumentlistContext *argumentlist();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ArgumentlistContext* argumentlist();
  ArgumentlistContext* argumentlist(int precedence);
  class  VarnameContext : public antlr4::ParserRuleContext {
  public:
    minicc::Identifier* val;
    antlr4::Token *idToken = nullptr;
    VarnameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ID();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  VarnameContext* varname();

  class  FuncnameContext : public antlr4::ParserRuleContext {
  public:
    minicc::Identifier* val;
    antlr4::Token *idToken = nullptr;
    FuncnameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ID();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  FuncnameContext* funcname();

  class  ParameternameContext : public antlr4::ParserRuleContext {
  public:
    minicc::Identifier* val;
    antlr4::Token *idToken = nullptr;
    ParameternameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ID();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ParameternameContext* parametername();


  virtual bool sempred(antlr4::RuleContext *_localctx, size_t ruleIndex, size_t predicateIndex) override;
  bool varlistSempred(VarlistContext *_localctx, size_t predicateIndex);
  bool parameterlistSempred(ParameterlistContext *_localctx, size_t predicateIndex);
  bool exprSempred(ExprContext *_localctx, size_t predicateIndex);
  bool argumentlistSempred(ArgumentlistContext *_localctx, size_t predicateIndex);

private:
  static std::vector<antlr4::dfa::DFA> _decisionToDFA;
  static antlr4::atn::PredictionContextCache _sharedContextCache;
  static std::vector<std::string> _ruleNames;
  static std::vector<std::string> _tokenNames;

  static std::vector<std::string> _literalNames;
  static std::vector<std::string> _symbolicNames;
  static antlr4::dfa::Vocabulary _vocabulary;
  static antlr4::atn::ATN _atn;
  static std::vector<uint16_t> _serializedATN;


  struct Initializer {
    Initializer();
  };
  static Initializer _init;
};

