#include "VM.h"

#include <fstream>
#include <iterator>
#include <filesystem>
#include <iostream>
#include <stdexcept>

VM::VM()
	:ip(0)
{}

bool VM::loadProgram(const std::string& path)
{
	std::ifstream file(path, std::ios::binary);
	if (!file) {
		std::cout << "Failed has failed to load\n";
		return false;
	}

	auto fileSize = std::filesystem::file_size(path);
	if (fileSize < 0x8) {
		std::cout << "file is smaller then header size.\n";
		return false;
	}

	char HEADER[8] = {};
	file.read(HEADER, 0x8);

	if (HEADER[0] != 0x7A || HEADER[1] != 0x70 || HEADER[2] != 0x70 || HEADER[3] != 0x00) { // test for zpp magic number.
		std::cout << "file type is not of zpp.\n";
		return false;
	}

	if (HEADER[4] != 0x01 || HEADER[5] != 0x00) { // test for file version... for now must be 1.
		std::cout << "file version is unsupported.\n";
		return false;
	}

	if (HEADER[6] != 0x00 || HEADER[7] != 0x00) { // test for flags... for now no flags exist.
		std::cout << "undefined flag status.\n";
		return false;
	}

	if (fileSize - 0x08 == 0) {
		std::cout << "no bytecode in file.\n";
		return false;
	}

	code.resize(fileSize - 0x08);
	file.read((char*)code.data(), fileSize - 0x08);
	if (!file) {
		std::cout << "failed to read bytecode section.\n";
		return false;
	}

	this->ip = 0;
	this->stack.clear();
	this->globals.clear();
	return true;
}

void VM::run()
{
	OperationResult status = OperationResult::Ok;
	while (status == OperationResult::Ok) {
		status = executeInstruction();
	}

	if (status == OperationResult::Halt) {
		std::cout << "Program Halted!, ip = " << ip << '\n';
		return;
	}

	if (status == OperationResult::Error) {
		std::cout << "Program Stopped!\n";
		return;
	}
}

OperationResult VM::executeInstruction()
{
	if (ip < code.size()) {

		auto instruction = static_cast<OpCodes>(code[ip++]);

		switch (instruction) {
		case OpCodes::SKIP:
		{
			return OperationResult::Ok;
		}

		case OpCodes::PUSH_I64:
		{
			try {
				int64_t val = readI64LE();
				this->stack.push_back(val);
			}
			catch (const std::exception& e) {
				std::cout << e.what() << '\n';
				return OperationResult::Error;
			}
			return OperationResult::Ok;
		}

		case OpCodes::ADD:
		{
			try {
				auto valB = popStack();
				auto valA = popStack();

				valA += valB;
				stack.push_back(valA);
				return OperationResult::Ok;
			}
			catch (const std::exception& e) {
				std::cout << e.what() << '\n';
				return OperationResult::Error;
			}
		}

		case OpCodes::HALT:
		{
			return OperationResult::Halt;
		}

		case OpCodes::SUB:
		{
			try {
				auto valB = popStack();
				auto valA = popStack();

				stack.push_back(valA - valB);
				return OperationResult::Ok;
			}
			catch (std::exception& e) {
				std::cout << e.what() << '\n';
				return OperationResult::Error;
			}
		}

		case OpCodes::MUL:
		{
			try {
				auto valB = popStack();
				auto valA = popStack();

				stack.push_back(valA * valB);
				return OperationResult::Ok;
			}
			catch (std::exception& e) {
				std::cout << e.what() << '\n';
				return OperationResult::Error;
			}
		}

		case OpCodes::DIV:
		{
			try {
				auto valB = popStack();
				auto valA = popStack();

				if (valB == 0) {
					std::cout << "DIV can't devide by zero\n";
					return OperationResult::Error;
				}

				stack.push_back(valA / valB);
				return OperationResult::Ok;
			}
			catch (std::exception& e) {
				std::cout << e.what() << '\n';
				return OperationResult::Error;
			}
		}

		case OpCodes::MOD:
		{
			try {
				auto valB = popStack();
				auto valA = popStack();

				if (valB == 0) {
					std::cout << "MOD can't devide by zero\n";
					return OperationResult::Error;
				}

				stack.push_back(valA % valB);
				return OperationResult::Ok;
			}
			catch (std::exception& e) {
				std::cout << e.what() << '\n';
				return OperationResult::Error;
			}
		}

		case OpCodes::POP:
		{
			try {
				popStack();
				return OperationResult::Ok;
			}
			catch (std::exception& e) {
				std::cout << e.what() << '\n';
				return OperationResult::Error;
			}
		}

		case OpCodes::SWAP:
		{
			try {
				auto valB = popStack();
				auto valA = popStack();

				stack.push_back(valB);
				stack.push_back(valA);
				return OperationResult::Ok;
			}
			catch (std::exception& e) {
				std::cout << e.what() << '\n';
				return OperationResult::Error;
			}
		}

		default:
		{
			std::cout << "Unhandled Instruction\n";
			return OperationResult::Error;
		}
		}
	}
	else
	{
		std::cout << "ip is out of code size\n";
		return OperationResult::Error;
	}
}

int64_t VM::readI64LE()
{
	if (ip + 8 > code.size()) {
		throw std::runtime_error("readI64LE is out of code range");
	}

	uint64_t val = 0;

	for (int i = 0; i < 8; ++i) {
		val |= (uint64_t)code[ip + i] << (8 * i);
	}
	ip += 8;

	return (int64_t)val;
}

ZppVM::Value VM::popStack()
{
	if (stack.empty())
		throw std::runtime_error("Stack Underflow");

	auto val = stack.back();
	stack.pop_back();
	return val;
}
