#pragma once
#include <string>
#include <variant>

namespace lex
{
    // Forward declarations for user-defined types
    class LexFunction;
    class LexClass;
    class LexInstance;


    // Runtime value representation
    using Value = std::variant<
    std::monostate,  // nil
    bool,            // boolean
    double,          // number
    std::string      // string
    >;

    // Helper functions for working with values
    inline bool is_truthy(const Value& value)
    {
        // nil and false are falsey, everything else is truthy
        if (std::holds_alternative<std::monostate>(value))
            return false;
        if (std::holds_alternative<bool>(value))
            return std::get<bool>(value);
        return true;
    }

    inline bool values_equal(const Value& a, const Value& b)
    {
        if (a.index() != b.index()) return false;

        if (std::holds_alternative<std::monostate>(a)) return true;
        if (std::holds_alternative<bool>(a))
            return std::get<bool>(a) == std::get<bool>(b);
        if (std::holds_alternative<double>(a))
            return std::get<double>(a) == std::get<double>(b);
        if (std::holds_alternative<std::string>(a))
            return std::get<std::string>(a) == std::get<std::string>(b);

        return false; // For future types
    }

    // Convert value to string for printing
    std::string value_to_string(const Value& value);
}