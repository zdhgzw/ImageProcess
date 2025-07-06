#include "ArgumentParser.h"
#include <iostream>
#include <algorithm>

ArgumentParser::ArgumentParser(const std::string& programName, const std::string& description)
    : m_programName(programName), m_description(description) {
}

void ArgumentParser::addArgument(const std::string& shortName, 
                                const std::string& longName,
                                const std::string& description,
                                bool required,
                                bool hasValue) {
    ArgumentInfo info;
    info.shortName = shortName;
    info.longName = longName;
    info.description = description;
    info.required = required;
    info.hasValue = hasValue;
    
    m_argumentInfos.push_back(info);
}

bool ArgumentParser::parse(int argc, char* argv[]) {
    m_parsedArguments.clear();
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Check if it's a help argument
        if (arg == "-h" || arg == "--help") {
            showHelp();
            return false;
        }

        std::string argName = findArgumentName(arg);
        if (argName.empty()) {
            std::cerr << "Error: Unknown argument '" << arg << "'" << std::endl;
            showUsage();
            return false;
        }

        // Find argument information
        auto it = std::find_if(m_argumentInfos.begin(), m_argumentInfos.end(),
            [&argName](const ArgumentInfo& info) {
                return info.shortName == argName || info.longName == argName;
            });

        if (it == m_argumentInfos.end()) {
            std::cerr << "Error: Undefined argument '" << arg << "'" << std::endl;
            return false;
        }

        if (it->hasValue) {
            if (i + 1 >= argc) {
                std::cerr << "Error: Argument '" << arg << "' requires a value" << std::endl;
                return false;
            }
            
            m_parsedArguments[it->shortName] = argv[++i];
            m_parsedArguments[it->longName] = argv[i];
        } else {
            m_parsedArguments[it->shortName] = "true";
            m_parsedArguments[it->longName] = "true";
        }
    }
    
    // Check required arguments
    for (const auto& info : m_argumentInfos) {
        if (info.required && !hasArgument(info.shortName)) {
            std::cerr << "Error: Missing required argument '" << info.longName << "'" << std::endl;
            showUsage();
            return false;
        }
    }
    
    return true;
}

std::string ArgumentParser::getValue(const std::string& name) const {
    auto it = m_parsedArguments.find(name);
    return (it != m_parsedArguments.end()) ? it->second : "";
}

bool ArgumentParser::hasArgument(const std::string& name) const {
    return m_parsedArguments.find(name) != m_parsedArguments.end();
}

void ArgumentParser::showHelp() const {
    std::cout << m_description << std::endl << std::endl;
    showUsage();
    std::cout << std::endl << "Arguments:" << std::endl;

    for (const auto& info : m_argumentInfos) {
        std::cout << "  " << info.shortName << ", " << info.longName;
        if (info.hasValue) {
            std::cout << " <value>";
        }
        std::cout << std::endl;
        std::cout << "        " << info.description;
        if (info.required) {
            std::cout << " (required)";
        }
        std::cout << std::endl << std::endl;
    }

    std::cout << "  -h, --help" << std::endl;
    std::cout << "        Show this help message" << std::endl;
}

void ArgumentParser::showUsage() const {
    std::cout << "Usage: " << m_programName;

    for (const auto& info : m_argumentInfos) {
        if (info.required) {
            std::cout << " " << info.longName;
            if (info.hasValue) {
                std::cout << " <value>";
            }
        } else {
            std::cout << " [" << info.longName;
            if (info.hasValue) {
                std::cout << " <value>";
            }
            std::cout << "]";
        }
    }

    std::cout << " [-h|--help]" << std::endl;
}

std::string ArgumentParser::findArgumentName(const std::string& arg) const {
    if (arg.length() < 2) {
        return "";
    }
    
    if (arg[0] == '-') {
        if (arg[1] == '-') {
            // Long argument name
            return arg;
        } else {
            // Short argument name
            return arg;
        }
    }
    
    return "";
}
