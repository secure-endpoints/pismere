#pragma once

class ParseOpts
{
public:
    struct Opts {
        char* pszString;
        char* pszEndpoint;
        unsigned int cMinCalls;
        unsigned int cMaxCalls;
        unsigned int fDontWait;
        bool bDontProtect;
        bool bShutdown;
        bool bSecCallback;
        bool bConsole;
    };

    ParseOpts(char* valid_opts);
    ParseOpts();
    ~ParseOpts();
    void SetValidOpts(char* valid_opts);
    void Parse(Opts& opts, int argc, char **argv);

private:
    bool IsValidOpt(char ch);
    void PrintOpt(char ch, char* text);
    void UsageOpts(char* program, int code = 0);

    char m_ValidOpts[256];
};
