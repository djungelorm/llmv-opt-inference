#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <cassert>
#include <cstring>

struct Instr {
  std::string result;
  std::string opcode;
  std::vector<std::string> operands;
};

std::ostream& operator<<(std::ostream& stream, const Instr& instr) {
  if (!instr.result.empty())
    stream << instr.result << " = ";
  stream << instr.opcode << " i32 ";
  for (auto it = instr.operands.begin(); it != instr.operands.end(); it++) {
    stream << (*it);
    if (it != instr.operands.end()-1)
      stream << ", ";
  }
  return stream;
}

std::vector<std::string> parseWords(const std::string& line) {
  std::vector<std::string> words;
  std::string word = "";
  for (auto chr : line) {
    if (chr == ' ') {
      words.push_back(word);
      word = "";
    } else {
      word += chr;
    }
  }
  words.push_back(word);
  return words;
}

Instr parseInstr(const std::string& line) {
  auto words = parseWords(line);
  Instr instr;
  instr.result = words[0];
  instr.opcode = words[1];
  for (int i = 2; i < words.size(); i++)
    instr.operands.push_back(words[i]);
  return instr;
}

void unassignedTransform(int inputs, std::vector<Instr>& instrs) {
  std::set<std::string> assigned;
  for (auto i = 1; i < inputs+1; i++)
    assigned.insert("%r"+std::to_string(i));

  for (auto& instr : instrs) {
    for (auto& operand : instr.operands) {
      if (operand[0] == '%' && assigned.find(operand) == assigned.end())
        operand = "0";
    }

    if (!instr.result.empty())
      assigned.insert(instr.result);
  }
};

void ssaTransform(int inputs, std::vector<Instr>& instrs) {
  std::map<std::string, int> seen;
  for (auto i = 1; i < inputs+1; i++)
    seen["%r"+std::to_string(i)] = 0;

  for (auto& instr : instrs) {
    for (auto i = 0; i < instr.operands.size(); i++) {
      auto operand = instr.operands[i];
      if (operand[0] == '%') {
        auto it = seen.find(operand);
        if (it != seen.end() && it->second > 0) {
          instr.operands[i] = operand + "." + std::to_string(it->second);
        }
      }
    }

    auto it = seen.find(instr.result);
    if (it != seen.end()) {
      it->second++;
      instr.result = instr.result + "." + std::to_string(it->second);
    } else {
      seen[instr.result] = 0;
    }
  }
}

void execute(const std::vector<std::string>& cmd) {
  const char ** argv = new const char * [cmd.size()+1];
  unsigned int i = 0;
  for (auto arg : cmd) {
    char * tmp = new char[arg.size()+1];
    arg.copy(tmp, arg.size());
    tmp[arg.size()] = '\0';
    argv[i] = tmp;
    i++;
  }
  argv[i] = NULL;

  int child_status;
  pid_t child_pid = fork();
  if (child_pid == 0) {
    execv(argv[0], const_cast<char**>(argv));
    std::cerr << "execv failed (" << strerror(errno) << ")" << std::endl;
    for (auto arg : cmd)
      std::cerr << arg << " ";
    std::cerr << std::endl;
    exit(1);
  } else {
    pid_t pid = waitpid(child_pid, &child_status, 0);
    if (pid != child_pid) {
      std::cerr << "wait failed" << std::endl;
      for (auto arg : cmd)
        std::cerr << arg << " ";
      std::cerr << std::endl;
      exit(1);
    }
  }
  if (child_status != 0) {
    std::cerr << "command failed" << std::endl;
    for (auto arg : cmd)
      std::cerr << arg << " ";
    std::cerr << std::endl;
    exit(1);
  }
  for (unsigned int i = 0; i < cmd.size(); i++)
    delete[] argv[i];
  delete[] argv;
}

int main(int argc, char** argv) {
  std::string path(argv[1]);

  std::vector<Instr> instrs;
  std::ifstream input;
  input.open(path+".instrs");
  std::string line;

  // read num of inputs
  getline(input, line);
  int inputs = std::stoi(line);

  // read instructions
  while (!input.eof()) {
    getline(input, line);
    if (!line.empty())
      instrs.push_back(parseInstr(line));
  }
  input.close();

  Instr ret;
  ret.opcode = "ret";
  ret.operands.push_back("%r0");
  instrs.push_back(ret);

  // apply transformations
  unassignedTransform(inputs, instrs);
  ssaTransform(inputs, instrs);

  // output code
  {
    std::ofstream output;
    output.open(path+".ll");
    output << "; ModuleID = 'program'" << std::endl << std::endl;
    output << "define i32 @program(i32* %inputs) {" << std::endl;
    output << "body:" << std::endl;
    for (auto i = 0; i < inputs; i++) {
      auto reg = "%r"+std::to_string(i+1);
      output << "  "+reg+".ptr = getelementptr inbounds i32, i32* %inputs, i64 "+std::to_string(i) << std::endl;
      output << "  "+reg+" = load i32, i32* "+reg+".ptr" << std::endl;
    }
    for (auto x : instrs)
      output << "  " << x << std::endl;
    output << "}" << std::endl;
    output.close();
  }
  execute({"/home/alex/workspaces/llvm/3.8/bin/llc", "-O0", "-filetype", "obj", "-o", path+".o", path+".ll"});
  execute({"/usr/bin/gcc", "-O0", "bin/utils.o", path+".o", "-o", path+".exe"});

 return 0;
}
