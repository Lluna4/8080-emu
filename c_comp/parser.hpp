#include <iostream>
#include <vector>
#include <variant>

enum tock{ 
    T_INT,
    T_MAIN,
    T_BRACES,
    T_ID,
    T_INT_LIT,
    T_OPEN_PARENTESIS,
    T_SEMICOLON,
    T_CLOSE_PARENTESIS,
    T_RETURN,
    T_OPEN_BRACES,
    T_CLOSE_BRACES,
    T_CHAR,
    T_VAR_NAME,
    T_VAR_VALUE
};

struct kw_argument
{
    int token_type;
    std::string value;
};

struct kw_return
{
    int return_num;
};

struct token
{
    int int_token;
    std::string code;
};

struct kw_var
{
    int type_token;
    std::string identifier;
    std::string value;
};

struct func
{
    std::string name;
    std::vector<struct kw_argument> args;
    std::vector<struct kw_var> variables;
    std::string return_type;
    struct kw_return return_thing;
};

std::vector<struct func> parsee(std::vector<struct token> tokens)
{
    std::vector<struct func> parsed;
    std::vector<kw_argument> args;
    std::vector<kw_var> variables;
    struct kw_return returned = {10};

    for(int i = 0; i < tokens.size(); i++)
    {
        if (tokens[i].int_token == T_INT)
        {
            if (tokens[i + 1].int_token == T_MAIN)
            {
                if (tokens[i + 2].int_token == T_OPEN_PARENTESIS)
                {
                    /* TODO: rework args
                    for (int j = i + 3; j < tokens.size() || tokens[j].int_token == T_CLOSE_PARENTESIS; j++)
                    {
                        if (tokens[j].int_token == T_INT || tokens[j].int_token == T_CHAR)
                        {
                            if (tokens[j + 1].int_token == T_VAR_NAME)
                            {
                                struct kw_argument arg = {tokens[j].int_token, tokens[j + 1].code};
                                args.push_back(arg);
                            }
                            else
                            {
                                std::cout << "ERROR: Argument without type" << std::endl;
                            }
                        }
                    }*/
                }
                returned = {tokens[i].int_token};
                if (args.empty())
                {
                    struct kw_argument arg = {0, ""};
                    args.push_back(arg);
                }
            }
        }

        if (tokens[i].int_token == T_VAR_NAME)
        {
            variables.push_back({tokens[i - 1].int_token, tokens[i].code, tokens[i + 1].code});
        }

        if (tokens[i].int_token == T_RETURN)
        {
            if (tokens[i + 1].int_token == T_INT_LIT && returned.return_num == T_INT)
            {
                struct kw_return kw = {atoi(tokens[i + 1].code.c_str())};
                if (variables.empty())
                {
                    variables.push_back({0, "", ""});
                }
                struct func function= {"main", args, variables, "int", kw};
                parsed.push_back(function);
                parsed.back().return_thing = kw;
            }
            else
            {
                std::cout << "PARSE ERROR: return doest have a value or is not the same type than the return value" << std::endl;
                exit(1);
            }
        }
    }
    return parsed;
}
