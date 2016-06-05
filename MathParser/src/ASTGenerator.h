#pragma once
#include <stack>
#include <memory>
#include <string>
#include <iostream>

using namespace std;

enum class node_type {
    none,
    value,
    opcode
};

enum class operator_code {
    none,
    add,
    sub,
    mul,
    div,
    exp,
    mod,
    pare_open,
    pare_close,
    push,
    print,
    halt
};


class node {
public:
    node() {
        type = node_type::none;
    }
    virtual ~node(){ };
    node_type type;
    shared_ptr<node> parent;
};

class value_node : public node {
public:
    value_node() {
        type = node_type::value;
    }

    int value = 0;
};

class operation_node : public node {
public:
    operation_node() {
        type = node_type::opcode;
        opcode = operator_code::none;
    }

    unsigned short precedence = 0;
    operator_code opcode;
    shared_ptr<node> left;
    shared_ptr<node> right;
};

class ast_generator {
private:
    stack<shared_ptr<node>> value_stack;
    stack<shared_ptr<operation_node>> op_stack;
    
    std::string buffer;
    unsigned int pos;
    
    char get_char();
    void inc_pos();

    unsigned short get_precedence(char op);
    operator_code get_opcode(char op);
    
    shared_ptr<node> pop_value();
    void add_operator(char op);
    void push_operator();
    
    void travese_buffer();

public:
    shared_ptr<node> parse(std::string input);
};
