#include "CodeGeneration.h"


void script::reset() {
    
}

void script::create(shared_ptr<node> root) {
    auto next_node = root;
    bool go_up = false;
    
    while (next_node) {
        if (go_up) {
            if (next_node->type == node_type::opcode) {
                auto rhs = dynamic_pointer_cast<operation_node>(next_node)->right;
                
                if (find(begin(visited_nodes), end(visited_nodes), rhs) == end(visited_nodes)) {
                    next_node = rhs;
                    go_up = false;
                    continue;
                } else {
                    next_node = next_node->parent;
                    go_up = true;
                    continue;
                }
            }
        } else {
            nodes.push(next_node);
            visited_nodes.push_back(next_node);
            
            if (next_node->type == node_type::opcode) {
                next_node = dynamic_pointer_cast<operation_node>(next_node)->left;
                continue;
            }
            
            if (next_node->type == node_type::value) {
                // jump to next simbling if possible
                next_node = next_node->parent;
                go_up = true;
                continue;
            }    
        }        
    }
    
    while (nodes.size() > 0) {
        next_node = nodes.top();
        nodes.pop();
        
        switch (next_node->type) {
            
            case node_type::value: {
                auto v_node = dynamic_pointer_cast<value_node>(next_node);
            
                tasks.push(task(operator_code::push, v_node->value));
                continue;
            }
            
            case node_type::opcode: {
                auto op_node = dynamic_pointer_cast<operation_node>(next_node);
            
                tasks.push(task(op_node->opcode));
                continue;
            }
            
            default:
                break;
        }
        
    }
    
    tasks.push(task(operator_code::print));
    tasks.push(task(operator_code::halt));

    stack<int> vm_stack;

    while (!tasks.empty()) {
        switch (tasks.front().opcode) {
            case operator_code::push:
                cout << "push " << tasks.front().value_0 << endl;

                vm_stack.push(tasks.front().value_0);
                break;
                
            case operator_code::add: {
                cout << "add" << endl;
                
                int lhs = vm_stack.top();
                vm_stack.pop();
                int rhs = vm_stack.top();
                vm_stack.pop();
                vm_stack.push(lhs + rhs);
                break;
            }
                
            case operator_code::sub: {
                cout << "sub" << endl;
                
                int lhs = vm_stack.top();
                vm_stack.pop();
                int rhs = vm_stack.top();
                vm_stack.pop();
                vm_stack.push(lhs - rhs);
                break;
            }
                
            case operator_code::mul: {
                cout << "mul" << endl;
                
                int lhs = vm_stack.top();
                vm_stack.pop();
                int rhs = vm_stack.top();
                vm_stack.pop();
                vm_stack.push(lhs * rhs);
                break;
            }
            case operator_code::div: {
                cout << "div" << endl;
                
                int lhs = vm_stack.top();
                vm_stack.pop();
                int rhs = vm_stack.top();
                vm_stack.pop();
                vm_stack.push(lhs / rhs);
                break;
            }
                
            case operator_code::print:
                cout << vm_stack.top() << endl;
                break;
                
            case operator_code::halt:
                return;
                
            default:
                break;
        }
        
        tasks.pop();
    }
}
