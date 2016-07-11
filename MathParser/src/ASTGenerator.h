#pragma once
#include <stack>
#include <memory>
#include <string>
#include <iostream>
#include <vector>

using namespace std;

enum class node_type {
    none,
    value,
    opcode,
    assign_statement,
    declare_statement,
    variable,
    function_call,
    function_call_statement
};

enum class operator_code {
    none,
    add,
    sub,
    mul,
    div,
    exp,
    mod,
    push,
    print,
    halt,
    store,
    load,
    call
};


class node {
public:
    node() {
        type = node_type::none;
    }
    virtual ~node(){ };
    node_type type;
    shared_ptr<node> parent = nullptr;
};

class value_node : public node {
public:
    value_node() {
        type = node_type::value;
    }

    int value = 0;
}; 

class variable_node : public node {
public:
    variable_node() {
        type = node_type::variable;
    }

    string name;
};

class operation_node : public node {
public:
    operation_node() {
        type = node_type::opcode;
        opcode = operator_code::none;
    }

    unsigned short precedence = 0;
    operator_code opcode;
    shared_ptr<node> left = nullptr;
    shared_ptr<node> right = nullptr;
};

class assign_statement_node : public node {
public:
    assign_statement_node() {
        type = node_type::assign_statement;
    }

    string variable_name;
    shared_ptr<node> node_content = nullptr;
    shared_ptr<node> next_node = nullptr;
};

class declare_statement_node : public node {
public:
    declare_statement_node() {
        type = node_type::declare_statement;
    }

    string type_name;
    string variable_name;
    shared_ptr<node> next_node = nullptr;
};

class function_call_node : public node {
public:
    function_call_node() {
        type = node_type::function_call;
    }

    string function_name;
    vector<shared_ptr<node>> arguments;
}; 

class function_call_statement_node : public function_call_node {
public:
    function_call_statement_node() {
        type = node_type::function_call_statement;
    }
    shared_ptr<node> next_node = nullptr;
};

enum class token_type {
    opadd,
    opsub,
    opmul,
    opdiv,
    opexp,
    opmod,
    integer,
    floating,
    assignment,
    identifier,
    end_statement,
    pare_open,
    pare_close,
    argument_seperator
};

struct token {
    token_type type;
    uint32_t pos;
    uint32_t len;
    uint32_t line;
    uint32_t line_pos;
};

void append_node(shared_ptr<node> base, shared_ptr<node> leaf); 

class ast_generator {
private:
    stack<pair<token, shared_ptr<node>>> value_stack;
    stack<pair<token, shared_ptr<operation_node>>> op_stack;
    vector<token> token_list;
    
    string buffer;
    unsigned int pos;
    unsigned int line;
    unsigned int line_pos;
    
    char get_char();
    void inc_pos();

    unsigned short get_precedence(token_type op);
    operator_code get_opcode(token_type op);
    
    shared_ptr<node> pop_value();
    bool add_operator(token token);

    bool push_operator();
    bool tokenize_buffer();
    shared_ptr<node> create_ast();
    shared_ptr<node> get_next_statement(uint32_t& offset);
    shared_ptr<node> get_next_value(uint32_t& offset);
    shared_ptr<node> get_next_func_call(uint32_t& offset, bool as_statement);

    string get_token_content(token token) const;
    bool is_function_call(uint32_t pos) const;
    bool is_assignment(uint32_t pos) const;
    bool is_declaration(uint32_t pos) const;
    token& get_next_token(uint32_t pos, uint32_t offset);
public:
    shared_ptr<node> parse(std::string input);
};
