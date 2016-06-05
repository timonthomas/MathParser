#include "ASTGenerator.h"

	shared_ptr<node> ast_generator::pop_value() {
	    if (value_stack.size() == 0) {
	        std::cout << "error 1" << std::endl;
	        return nullptr; 
	    } else {
	        auto result = value_stack.top();
	        value_stack.pop();
	        return result;
	    }
	}
	
	char ast_generator::get_char() {
	    if (pos >= buffer.size()) {
	        return 0;
	    } else {
	        return buffer[pos];
	    }
	}
	
	void ast_generator::inc_pos() {
	    ++pos;
	}
	
	void ast_generator::push_operator() {
	    auto node = op_stack.top();
	    op_stack.pop();
	
	    node->right = pop_value();
	    node->right->parent = node;
	    node->left = pop_value();
	    node->left->parent = node;
	
	    value_stack.push(node);
	}
	
	unsigned short ast_generator::get_precedence(char op) {
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
	        return 9;
	    }
	    if (op == ')') {
	        return 9;
	    }
	    return 0;
	}
	
	operator_code ast_generator::get_opcode(char op) {
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
	
	void ast_generator::add_operator(char op) {
	    auto pre = get_precedence(op);
	
		
	    if (op_stack.size() > 0) {
	        auto previous_op = op_stack.top();
	   
	        while (previous_op->precedence >= pre && 
				   previous_op->opcode != operator_code::pare_open) {
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

	void ast_generator::travese_buffer() {
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
	            add_operator(get_char());
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
	                new_number->value = stoi(value);
	
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

	shared_ptr<node> ast_generator::parse(std::string input) {
		buffer = input;
		pos = 0;

		travese_buffer();
	    
    	while (op_stack.size() > 0) {
        	push_operator();
    	}


		return value_stack.top();
	}
