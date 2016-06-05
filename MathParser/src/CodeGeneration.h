#pragma once
#include "ASTGenerator.h"
#include <stack>
#include <queue>
#include <vector>
#include <algorithm>
#include <iterator>

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
	int value_0 = 0;
	int value_1 = 0;
};

class script {
public:
	void reset();
	void create(shared_ptr<node> root);
private:
	vector<shared_ptr<node>> visited_nodes;
	stack<shared_ptr<node>> nodes;
	queue<task> tasks;
};
