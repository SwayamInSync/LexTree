#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <filesystem>

namespace fs = std::filesystem;

// Trim whitespace from both ends of a string
std::string trim(const std::string& str) {
    const auto start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    }
    const auto end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Split a string into a vector of strings based on delimiter
std::vector<std::string> split(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter, start);
    
    while (end != std::string::npos) {
        tokens.push_back(trim(str.substr(start, end - start)));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    
    tokens.push_back(trim(str.substr(start)));
    return tokens;
}

// Parse a definition line into class name and field list
std::pair<std::string, std::string> parse_definition(const std::string& line) {
    auto pos = line.find(':');
    if (pos == std::string::npos) {
        std::cerr << "Invalid definition line: " << line << std::endl;
        return {"", ""};
    }
    
    return {trim(line.substr(0, pos)), trim(line.substr(pos + 1))};
}

// Generate visitor interface
void define_visitor(std::ofstream& writer, const std::vector<std::pair<std::string, std::string>>& types) {
    writer << "// Visitor interface (for double dispatch)\n";
    writer << "class ExprVisitor {\n";
    writer << "public:\n";
    writer << "    virtual ~ExprVisitor() = default;\n\n";
    writer << "    // Declare a visit method for each expression type\n";
    
    for (const auto& [className, _] : types) {
        writer << "    virtual std::any visit" << className << "Expr(class " << className << "* expr) = 0;\n";
    }
    
    writer << "};\n\n";
}

// Generate base expr class
void define_base_expr(std::ofstream& writer) {
    writer << "// Base Expression class\n";
    writer << "class Expr {\n";
    writer << "public:\n";
    writer << "    virtual ~Expr() = default;\n";
    writer << "    virtual std::any accept(ExprVisitor* visitor) = 0;\n";
    writer << "};\n\n";
}

// Generate specific expression class
void define_type(std::ofstream& writer, const std::string& className, const std::string& fieldList) {
    writer << "// " << className << " expression\n";
    writer << "class " << className << " : public Expr {\n";
    writer << "public:\n";
    
    // Constructor
    writer << "    " << className << "(";
    
    // Parse field list
    std::vector<std::string> fields = split(fieldList, ",");
    bool first = true;
    
    for (const auto& field : fields) {
        if (!first) {
            writer << ", ";
        }
        writer << trim(field);
        first = false;
    }
    
    writer << ")\n";
    
    // Initialize fields
    if (!fields.empty()) {
        writer << "        : ";
        first = true;
        
        for (const auto& field : fields) {
            if (!first) {
                writer << ", ";
            }
            
            std::vector<std::string> parts = split(field, " ");
            if (parts.size() < 2) {
                continue;
            }
            
            std::string fieldName = parts[parts.size() - 1];
            writer << fieldName << "(std::move(" << fieldName << "))";
            first = false;
        }
        writer << " {}\n\n";
    } else {
        writer << "    {}\n\n";
    }
    
    // Accept method
    writer << "    std::any accept(ExprVisitor* visitor) override {\n";
    writer << "        return visitor->visit" << className << "Expr(this);\n";
    writer << "    }\n\n";
    
    // Fields
    for (const auto& field : fields) {
        writer << "    const " << trim(field) << ";\n";
    }
    
    writer << "};\n\n";
}

// Generate helper factory methods
void define_factory_methods(std::ofstream& writer, const std::vector<std::pair<std::string, std::string>>& types) {
    writer << "// Helper functions to create shared pointers for each expression type\n";
    
    for (const auto& [className, fieldList] : types) {
        writer << "inline ExprPtr make_" << std::regex_replace(className, std::regex("([a-z])([A-Z])"), "$1_$2") << "(";
        
        // Parameter list
        std::vector<std::string> fields = split(fieldList, ",");
        bool first = true;
        
        for (const auto& field : fields) {
            if (!first) {
                writer << ", ";
            }
            writer << trim(field);
            first = false;
        }
        
        writer << ") {\n";
        writer << "    return std::make_shared<" << className << ">(";
        
        // Arguments
        first = true;
        for (const auto& field : fields) {
            if (!first) {
                writer << ", ";
            }
            
            std::vector<std::string> parts = split(field, " ");
            if (parts.size() < 2) {
                continue;
            }
            
            std::string fieldName = parts[parts.size() - 1];
            writer << "std::move(" << fieldName << ")";
            first = false;
        }
        
        writer << ");\n";
        writer << "}\n\n";
    }
}

// Generate the full AST implementation
void define_ast(const std::string& outputDir, const std::string& file_path) {
    // Read the definition file
    std::ifstream def_file(file_path);
    if (!def_file.is_open()) {
        std::cerr << "Could not open file: " << file_path << std::endl;
        return;
    }
    
    // Parse all type definitions
    std::vector<std::pair<std::string, std::string>> types;
    std::string line;
    
    while (std::getline(def_file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        auto type_def = parse_definition(line);
        if (!type_def.first.empty()) {
            types.push_back(type_def);
        }
    }
    
    // Create output file
    fs::create_directories(outputDir);
    std::string output_path = (fs::path(outputDir) / "Expr.h").string();
    std::ofstream writer(output_path);
    
    if (!writer.is_open()) {
        std::cerr << "Could not create output file: " << output_path << std::endl;
        return;
    }
    
    // Write the file header
    writer << "This is auto-generated file, using the utility/generate_ast.cpp\n";
    writer << "#pragma once\n\n";
    writer << "#include \"Lexer/Token.h\"\n";
    writer << "#include <memory>\n";
    writer << "#include <utility>\n";
    writer << "#include <any>\n\n";
    writer << "namespace lex {\n\n";
    
    // Forward declarations
    writer << "// Forward declaration for recursive expressions\n";
    writer << "class Expr;\n";
    writer << "using ExprPtr = std::shared_ptr<Expr>;\n\n";
    
    // Define visitor interface
    define_visitor(writer, types);
    
    // Define base expr class
    define_base_expr(writer);
    
    // Define specific expression classes
    for (const auto& [className, fieldList] : types) {
        define_type(writer, className, fieldList);
    }
    
    // Define helper factory methods
    define_factory_methods(writer, types);
    
    // Close namespace
    writer << "} // namespace lex\n";
    writer.close();
    
    std::cout << "Generated: " << output_path << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: generate_ast <output_directory> <definition_file>" << std::endl;
        return 1;
    }
    
    std::string outputDir = argv[1];
    std::string defFile = argv[2];
    
    define_ast(outputDir, defFile);
    return 0;
}