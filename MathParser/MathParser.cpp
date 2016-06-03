// MathParser.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
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
    halt
};


class node {
public:
    node() {
        type = node_type::none;
    }
    virtual ~node(){ };
    node_type type;
};

class value_node : public node {
public:
    value_node() {
        type = node_type::value;
    }

    long value = 0;
};

class operation_node : public node {
public:
    operation_node() {
        type = node_type::opcode;
    }

    unsigned short precedence = 0;
    operator_code opcode;
    shared_ptr<node> left;
    shared_ptr<node> right;
};

stack<shared_ptr<node>> value_stack;
stack<shared_ptr<operation_node>> op_stack;

shared_ptr<node> pop_value() {
    if (value_stack.size() == 0) {
        std::cout << "error" << std::endl;
        return nullptr; 
    } else {
        auto result = value_stack.top();
        value_stack.pop();
        return result;
    }
}

std::string input = "(6+9)/8";

unsigned int pos = 0;

char get_char() {
    if (pos >= input.size()) {
        return 0;
    } else {
        return input[pos];
    }
}

void inc_pos() {
    ++pos;
}

void push_operator() {
    auto node = op_stack.top();
    op_stack.pop();

    node->right = pop_value();
    node->left = pop_value();

    value_stack.push(node);
}

unsigned short get_precedence(char op) {
    if (op == '+') {
        return 1;
    }
    if (op == '-') {
        return 1;
    }
    if (op == '*') {
        return 2;
    }
    if (op == '/') {
        return 2;
    }
    if (op == '^') {
        return 3;
    }
    if (op == '%') {
        return 3;
    }
    if (op == '(') {
        return 0;
    }
    if (op == ')') {
        return 0;
    }
    return 0;
}

operator_code get_opcode(char op) {
    if (op == '+') {
        return operator_code::add;
    }
    if (op == '-') {
        return operator_code::sub;
    }
    if (op == '*') {
        return operator_code::mul;
    }
    if (op == '/') {
        return operator_code::div;
    }
    if (op == '^') {
        return operator_code::mul;
    }
    if (op == '%') {
        return operator_code::div;
    }
    if (op == '(') {
        return operator_code::pare_open;
    }
    if (op == ')') {
        return operator_code::pare_close;
    }
    return operator_code::none;
}

void add_op(char op) {
    auto pre = get_precedence(op);

    if (op_stack.size() > 0) {
        auto previous_op = op_stack.top();

        while (previous_op->precedence >= pre) {
            push_operator();

            if (op_stack.size() == 0) {
                break;
            } else {
                previous_op = op_stack.top();
            }
        }
    }


    shared_ptr<operation_node> new_op = make_shared<operation_node>();
    new_op->opcode = get_opcode(op);
    new_op->precedence = pre;

    op_stack.push(new_op);
}

void parse_input() {
    while (true) {
        switch (get_char()) {
        case 0:
            return;
        case ' ':
        case '\t':
        case '\n':
            inc_pos();
            break;
        case '+':
        case '-':
        case '/':
        case '*':
        case '^':
        case '(':
            add_op(get_char());
            inc_pos();
            break;
        case ')':

            if (op_stack.size() > 0) {
                shared_ptr<operation_node> previous_op = op_stack.top(); 

                while (true) {
                    if (previous_op->opcode == operator_code::pare_open) {
                        op_stack.pop();
                        break;
                    } else {
                        push_operator();
                    }


                    if (op_stack.size() == 0) {
                        break;
                    } else {
                        previous_op = op_stack.top();
                    }
                }
            }


            inc_pos();
            break;
        default:
            if (isdigit(get_char())) {
                std::string value;
                value.push_back(get_char());
                inc_pos();

                while (isdigit(get_char())) {
                    value.push_back(get_char());
                    inc_pos();
                }

                shared_ptr<value_node> new_number = make_shared<value_node>();
                new_number->value = stol(value);

                value_stack.push(new_number);
            } else {
                std::cout << get_char() << std::endl;
                std::cout << "error" << std::endl;
                return;
            }


            break;
        }
    }
}

int main()
{
    parse_input();

    while (op_stack.size() > 0) {
        push_operator();
    }

    std::cin.get();
    return 0;
}

