#include "Compiler.h"

#include "BackEnd/BackEnd.h"
#include "File/File.h"
#include "FrontEnd/FrontEnd.h"

namespace Compiler {
void Compile(const std::string &input, std::string output) {
    File input_file = File(input);
    if (input_file.typeName() != "txt") {
        throw "path must ended with `.txt`";
    }

    std::string input_str = input_file.read();
    auto words = FrontEnd::Lexer(input_str);
    auto blocks = FrontEnd::Parser(words);
    BackEnd::CodeGenerator gen(blocks);

    if (output.empty()) {
        throw "output_dir is empty";
    }
    if (output.back() != '/' && output.back() != '\\') {
        output.push_back('/');
    }

    File(output + input_file.baseName() + ".h").write(gen.header());
    File(output + input_file.baseName() + ".cc")
        .write(gen.source(input_file.baseName()));
}
}  // namespace Compiler