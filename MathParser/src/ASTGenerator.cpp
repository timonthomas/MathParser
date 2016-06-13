#include "ASTGenerator.h"

shared_ptr<node> ast_generator::pop_value() {
    if (value_stack.size() == 0) {
        cout << "ast-generator error -------------" << endl;
        cout << "tried to pop value from the stack" << endl;
        cout << "in line " << line_pos << " at position " << line_pos << endl;
        return nullptr;
    } else {
        auto result = value_stack.top();
        value_stack.pop();
        return result.second;
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
    ++line_pos;
    ++pos;
}

bool ast_generator::push_operator() {
    auto node = op_stack.top();
    op_stack.pop();

    node.second->right = pop_value();
    node.second->left = pop_value();

    if (!node.second->right || !node.second->left) {
        return false;
    }

    node.second->right->parent = node.second;
    node.second->left->parent = node.second;

    value_stack.push(node);

    return true;
}

unsigned short ast_generator::get_precedence(token_type op) {
    if (op == token_type::opadd) {
        return 1;
    }

    if (op == token_type::opsub) {
        return 1;
    }

    if (op == token_type::opmul) {
        return 2;
    }

    if (op == token_type::opdiv) {
        return 2;
    }

    if (op == token_type::opexp) {
        return 3;
    }

    if (op == token_type::opmod) {
        return 3;
    }

    if (op == token_type::pare_open) {
        return 9;
    }

    if (op == token_type::pare_close) {
        return 9;
    }

    if (op == token_type::assignment) {
        return 10;
    }
    return 0;
}

operator_code ast_generator::get_opcode(token_type op) {
    if (op == token_type::opadd) {
        return operator_code::add;
    }

    if (op == token_type::opsub) {
        return operator_code::sub;
    }

    if (op == token_type::opmul) {
        return operator_code::mul;
    }

    if (op == token_type::opdiv) {
        return operator_code::div;
    }

    if (op == token_type::opexp) {
        return operator_code::mul;
    }

    if (op == token_type::opmod) {
        return operator_code::mod;
    }

    return operator_code::none;
}

bool ast_generator::add_operator(token token) {

    if (token.type == token_type::pare_close) {
        if (op_stack.size() > 0) {
            auto previous_op = op_stack.top();

            while (true) {
                if (previous_op.first.type == token_type::pare_open) {
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
    } else if (token.type == token_type::pare_open) {
        op_stack.push(make_pair(token, nullptr));
    } else {
        auto pre = get_precedence(token.type);

        if (op_stack.size() > 0) {
            auto previous_op = op_stack.top();

            while (previous_op.second->precedence >= pre &&
                previous_op.first.type != token_type::pare_open) {
                auto result = push_operator();

                if (!result) {
                    return false;
                }

                if (op_stack.size() == 0) {
                    break;
                } else {
                    previous_op = op_stack.top();
                }
            }
        }

        shared_ptr<operation_node> new_op = make_shared<operation_node>();
        new_op->opcode = get_opcode(token.type);
        new_op->precedence = pre;

        op_stack.push(make_pair(token, new_op));
        return true;
    }
}

bool ast_generator::tokenize_buffer() {
    while (true) {
        switch (get_char()) {
        case 0:
            return true;
        case '\n':
            ++line;
            line_pos = 0;
        case ' ':
        case '\t':
            inc_pos();
            break;
        case '+':
            token_list.push_back(token{ token_type::opadd , pos, 1, line, line_pos });
            inc_pos();
            break;
        case '-':
            token_list.push_back(token{ token_type::opsub , pos, 1, line, line_pos });
            inc_pos();
            break;
        case '*':
            token_list.push_back(token{ token_type::opmul , pos, 1, line, line_pos });
            inc_pos();
            break;
        case '/':
            token_list.push_back(token{ token_type::opdiv , pos, 1, line, line_pos });
            inc_pos();
            break;
        case '^':
            token_list.push_back(token{ token_type::opexp , pos, 1, line, line_pos });
            inc_pos();
            break;
        case '%':
            token_list.push_back(token{ token_type::opmod , pos, 1, line, line_pos });
            inc_pos();
            break;
        case '(':
            token_list.push_back(token{ token_type::pare_open , pos, 1, line, line_pos });
            inc_pos();
            break;
        case ')':
            token_list.push_back(token{ token_type::pare_close , pos, 1, line, line_pos });
            inc_pos();
            break;
        case ';':
            token_list.push_back(token{ token_type::end_statement , pos, 1, line, line_pos });
            inc_pos();
            break;
        case '=':
            token_list.push_back(token{ token_type::assignment , pos, 1, line, line_pos });
            inc_pos();
            break;
        default:
            if (isdigit(get_char())) {
                uint32_t num_pos = pos;
                uint32_t num_line_pos = line_pos;
                uint32_t num_len = 1;

                inc_pos();

                while (isdigit(get_char())) {
                    num_len++;
                    inc_pos();
                }

                token_list.push_back(token{ token_type::integer , num_pos, num_len, line, num_line_pos });
            } else if (isalpha(get_char())) {
                uint32_t id_pos = pos;
                uint32_t id_line_pos = line_pos;
                uint32_t id_len = 1;

                inc_pos();

                while (isalnum(get_char()) || get_char() == '_' || get_char() == '-') {
                    id_len++;
                    inc_pos();
                }

                token_list.push_back(token{ token_type::identifier , id_pos, id_len, line, id_line_pos });
            } else {
                cout << "parser error --------------------" << endl;
                cout << "unexpected symbol" << endl;
                cout << "in line " << line_pos << " at position " << line_pos << endl;
                return false;
            }
            break;
        }
    }

    return true;
}

shared_ptr<node> ast_generator::create_ast()
{
    uint32_t token_pos = 0;

    shared_ptr<node> result = nullptr;
    shared_ptr<node> current_node = nullptr;

    while (token_pos < token_list.size()) {
        auto& token = token_list[token_pos];
        string content = get_token_content(token);

        switch (token.type)
        {
        case token_type::end_statement:
            ++token_pos;
            continue;
        case token_type::identifier: {

            auto next_statement = get_next_statement(token_pos);

            if (next_statement->type == node_type::declare_statement) {

                if (result) {
                    append_node(current_node, next_statement);
                } else {
                    result = next_statement;
                }                

                auto declare_node = dynamic_pointer_cast<declare_statement_node>(next_statement);

                // check if a assignement node is attached to the declaration node
                if (declare_node->next_node) {
                    current_node = declare_node->next_node;
                } else {
                    current_node = next_statement;
                }

            } else {
                if (result) {
                    append_node(current_node, next_statement);
                    current_node = next_statement;
                } else {
                    result = next_statement;
                    current_node = result;
                }
            }

            continue;
        }
        default:
            break;
        }
    }


    return result;
}

shared_ptr<node> ast_generator::get_next_statement(uint32_t& token_pos)
{
    shared_ptr<node> result = nullptr;

    if (is_declaration(token_pos)) {

        auto declare_node = make_shared<declare_statement_node>();
        declare_node->type_name = get_token_content(token_list[token_pos]);
        declare_node->variable_name = get_token_content(token_list[token_pos + 1]);

        result = declare_node;

        auto assign_node = make_shared<assign_statement_node>();
        assign_node->variable_name = get_token_content(token_list[token_pos + 1]);
        
        // get value of assignment
        token_pos += 3;
        assign_node->node_content = get_next_value(token_pos);
        assign_node->node_content->parent = assign_node;
        append_node(result, assign_node);

        return result;
    }

    if (is_assignment(token_pos)) {
        auto assign_node = make_shared<assign_statement_node>();
        assign_node->variable_name = get_token_content(token_list[token_pos]);

        // get value of assignment
        token_pos += 2;
        assign_node->node_content = get_next_value(token_pos);
        assign_node->node_content->parent = assign_node;

        result = assign_node;

        return result;
    }

    return nullptr;
}

shared_ptr<node> ast_generator::get_next_value(uint32_t& token_pos)
{
    while (token_pos < token_list.size()) {
        auto& token = token_list[token_pos];

        switch (token.type)
        {
        case token_type::end_statement:
            ++token_pos;

            while (op_stack.size() > 0) {
                push_operator();
            }
            return value_stack.top().second;

            continue;
        case token_type::opadd:
        case token_type::opsub:
        case token_type::opmul:
        case token_type::opdiv:
        case token_type::opexp:
        case token_type::opmod:
        case token_type::pare_open:
        case token_type::pare_close:
            add_operator(token);
            ++token_pos;
            continue;
        case token_type::integer: {
            shared_ptr<value_node> new_number = make_shared<value_node>();
            new_number->value = stoi(string(buffer, token.pos, token.len));

            value_stack.push(make_pair(token, new_number));
            ++token_pos;
            continue;
        }
        case token_type::identifier: {
            shared_ptr<variable_node> new_variable = make_shared<variable_node>();
            new_variable->name = get_token_content(token);
            
            value_stack.push(make_pair(token, new_variable));
            ++token_pos;
            continue;
        }
        default:
            ++token_pos;
            break;
        }
    }

    return nullptr;
}

string ast_generator::get_token_content(token token) const
{
    return string(buffer, token.pos, token.len);
}

bool ast_generator::is_assignment(uint32_t offset) const
{
    bool result = true;

    if (token_list[offset].type != token_type::identifier) {
        result = false;
    }

    if (token_list[offset + 1].type != token_type::assignment) {
        result = false;
    }

    return result;
}

bool ast_generator::is_declaration(uint32_t offset) const
{
    bool result = true;

    if (token_list[offset].type != token_type::identifier) {
        result = false;
    }    
    
    if (token_list[offset + 1].type != token_type::identifier) {
        result = false;
    }

    if (token_list[offset + 2].type != token_type::assignment) {
        result = false;
    }

    return result;
}

shared_ptr<node> ast_generator::parse(string input) {
    buffer = input;
    pos = 0;
    line_pos = 0;
    line = 1;

    bool result = true;


    if (result) {
        result = tokenize_buffer();
    } else {
        return nullptr;
    }

    if (result) {
        return create_ast();
    } else {
        return nullptr;
    }
}

void append_node(shared_ptr<node> base, shared_ptr<node> leaf)
{
    leaf->parent = base;

    if (base->type == node_type::assign_statement) {
        (dynamic_pointer_cast<assign_statement_node>(base))->next_node = leaf;
    }

    if (base->type == node_type::declare_statement) {
        (dynamic_pointer_cast<declare_statement_node>(base))->next_node = leaf;
    }
}
