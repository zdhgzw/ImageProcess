#ifndef ARGUMENT_PARSER_H
#define ARGUMENT_PARSER_H

#include <string>
#include <vector>
#include <map>

/**
 * 命令行参数解析器
 * 提供统一的命令行参数处理功能
 */
class ArgumentParser {
public:
    /**
     * 构造函数
     * @param programName 程序名称
     * @param description 程序描述
     */
    ArgumentParser(const std::string& programName, const std::string& description);

    /**
     * 添加参数定义
     * @param shortName 短参数名（如 -i）
     * @param longName 长参数名（如 --input）
     * @param description 参数描述
     * @param required 是否为必需参数
     * @param hasValue 是否需要值
     */
    void addArgument(const std::string& shortName, 
                    const std::string& longName,
                    const std::string& description,
                    bool required = false,
                    bool hasValue = true);

    /**
     * 解析命令行参数
     * @param argc 参数个数
     * @param argv 参数数组
     * @return 解析是否成功
     */
    bool parse(int argc, char* argv[]);

    /**
     * 获取参数值
     * @param name 参数名（短名或长名）
     * @return 参数值，如果不存在返回空字符串
     */
    std::string getValue(const std::string& name) const;

    /**
     * 检查参数是否存在
     * @param name 参数名
     * @return 参数是否存在
     */
    bool hasArgument(const std::string& name) const;

    /**
     * 显示帮助信息
     */
    void showHelp() const;

    /**
     * 显示使用示例
     */
    void showUsage() const;

private:
    struct ArgumentInfo {
        std::string shortName;
        std::string longName;
        std::string description;
        bool required;
        bool hasValue;
    };

    std::string m_programName;
    std::string m_description;
    std::vector<ArgumentInfo> m_argumentInfos;
    std::map<std::string, std::string> m_parsedArguments;

    std::string findArgumentName(const std::string& arg) const;
};

#endif // ARGUMENT_PARSER_H
