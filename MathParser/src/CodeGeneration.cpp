#include "CodeGeneration.h"


void script::reset() {
    
}

void script::create(shared_ptr<node> root) {
    
}

code_generator::code_generator()
{
    known_types["bool"] = type_def{ 1 };
    known_types["byte"] = type_def{ 1 };
    known_types["sbyte"] = type_def{ 1 };
    known_types["short"] = type_def{ 2 };
    known_types["ushort"] = type_def{ 2 };
    known_types["int"] = type_def{ 4 };
    known_types["uint"] = type_def{ 4 };
    known_types["long"] = type_def{ 8 };
    known_types["uint"] = type_def{ 8 };
}

script code_generator::create(shared_ptr<node> root)
{
    visited_nodes.clear();
    nodes = stack<shared_ptr<node>>();

    script result;

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

            if (next_node->type == node_type::declare_statement) {
                nodes.push(next_node);
                next_node = next_node->parent;
                go_up = true;
                continue;
            }

            if (next_node->type == node_type::assign_statement) {
                auto rhs = dynamic_pointer_cast<assign_statement_node>(next_node)->node_content;

                if (find(begin(visited_nodes), end(visited_nodes), rhs) == end(visited_nodes)) {
                    nodes.push(next_node);
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
            visited_nodes.push_back(next_node);

            if (next_node->type == node_type::opcode) {
                nodes.push(next_node);
                next_node = dynamic_pointer_cast<operation_node>(next_node)->left;
                continue;
            }

            if (next_node->type == node_type::declare_statement) {
                auto declare_node = dynamic_pointer_cast<declare_statement_node>(next_node);

                if (declare_node->next_node) {
                    next_node = declare_node->next_node;
                } else {
                    next_node = declare_node->parent;
                    go_up = true;
                }
                continue;
            }

            if (next_node->type == node_type::assign_statement) {
                auto assign_node = dynamic_pointer_cast<assign_statement_node>(next_node);

                if (assign_node->next_node) {
                    next_node = assign_node->next_node;
                } else {
                    nodes.push(next_node);
                    next_node = assign_node->node_content;
                }

                continue;
            }

            if (next_node->type == node_type::variable) {
                nodes.push(next_node);
                // jump to next simbling if possible
                next_node = next_node->parent;
                go_up = true;
                continue;
            }

            if (next_node->type == node_type::value) {
                nodes.push(next_node);
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


        case node_type::declare_statement: {
            auto d_node = dynamic_pointer_cast<declare_statement_node>(next_node);

            variable_declaration new_variable;
            new_variable.name = d_node->variable_name;
            new_variable.size = 4;
            new_variable.offset = result.variables.size() * 4;
            
            result.variables.push_back(new_variable);
            continue;
        }

        case node_type::assign_statement: {
            auto a_node = dynamic_pointer_cast<assign_statement_node>(next_node);

            auto var_dec_it = find_if(result.variables.begin(), result.variables.end(), [&](variable_declaration& dec) {
                return dec.name == a_node->variable_name;
            });

            if (var_dec_it == result.variables.end()) {
                // error
            } else {
                result.tasks.push_back(task(operator_code::store, var_dec_it->offset));
            }
            continue;
        }

        case node_type::variable: {
            auto v_node = dynamic_pointer_cast<variable_node>(next_node);

            auto var_dec_it = find_if(result.variables.begin(), result.variables.end(), [&](variable_declaration& dec) {
                return dec.name == v_node->name;
            });

            if (var_dec_it == result.variables.end()) {
                // error
            } else {
                result.tasks.push_back(task(operator_code::load, var_dec_it->offset));
            }
            continue;
        }

        case node_type::value: {
            auto v_node = dynamic_pointer_cast<value_node>(next_node);

            result.tasks.push_back(task(operator_code::push, v_node->value));
            continue;
        }

        case node_type::opcode: {
            auto op_node = dynamic_pointer_cast<operation_node>(next_node);

            result.tasks.push_back(task(op_node->opcode));
            continue;
        }

        default:
            break;
        }

    }

    result.tasks.push_back(task(operator_code::halt));
    return result;
}

void vm::execute(script & sc)
{
    memory.clear();

    uint32_t task_id = 0;

    while (true) {
        switch (sc.tasks[task_id].opcode) {
        case operator_code::push:
            push_stack(sc.tasks.front().value_0);
            break;

        case operator_code::add: {
            push_stack(pop_stack() + pop_stack());
            break;
        }

        case operator_code::sub: {
            push_stack(pop_stack() - pop_stack());
            break;
        }

        case operator_code::mul: {
            push_stack(pop_stack() * pop_stack());
            break;
        }
        case operator_code::div: {
            push_stack(pop_stack() / pop_stack());
            break;
        }

        case operator_code::print:
            cout << pop_stack() << endl;
            break;

        case operator_code::halt:
            return;

        default:
            break;
        }

        ++task_id;
    }
}
