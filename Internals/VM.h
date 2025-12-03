#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace ZppVM {
	using Value = int64_t;
}

enum class OpCodes : uint8_t { SKIP = 0x0, PUSH_I64, ADD, HALT, SUB, MUL, DIV, MOD, POP, SWAP };

enum class OperationResult { Ok, Halt, Error };

class VM
{
public:
	VM();

	bool loadProgram(const std::string& path);
	void run();

private:
	std::vector<ZppVM::Value> stack;
	std::vector<ZppVM::Value> globals;
	std::vector<uint8_t> code;

	size_t ip;

	int64_t readI64LE();
	ZppVM::Value popStack();
	OperationResult executeInstruction();
};

