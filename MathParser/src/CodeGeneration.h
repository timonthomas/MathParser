#pragma once
#include "ASTGenerator.h"
#include <stack>
#include <queue>
#include <vector>
#include <algorithm>
#include <iterator>
#include <map>

using namespace std;

class task {
public:
	task(operator_code code) {
		opcode = code;
	}
	
	task(operator_code code, int value) {
		opcode = code;
		value_0 = value;
	}
	
	operator_code opcode = operator_code::none;
    uint32_t value_0 = 0;
    uint32_t value_1 = 0;
};

class script;

class vm {
public:
    void execute(script& sc);

private:
    vector<uint8_t> memory;
    vector<uint32_t> stack;

    inline uint32_t pop_stack() {
        auto result = stack.back();
        stack.pop_back();
        return result;
    }

    inline void push_stack(uint32_t value) {
        stack.push_back(value);
    }
};

struct type_declaration {
    string name;
    uint32_t size;
};

struct variable_declaration {
    string name;
    uint32_t size;
    uint32_t offset;
};

class script {
    friend class code_generator;
    friend class vm;
public:
	void reset();
	void create(shared_ptr<node> root);
protected:
    vector<variable_declaration> variables;
    vector<task> tasks;
};

struct type_def {
    uint32_t size;
};

class code_generator {
public:
    code_generator();

    script create(shared_ptr<node> root);
private:

    map<string, type_def> known_types;
    vector<shared_ptr<node>> visited_nodes;
    stack<shared_ptr<node>> nodes;
};