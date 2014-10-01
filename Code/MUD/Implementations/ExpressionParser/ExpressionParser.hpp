#ifndef MUD_EXPPARSE_EXPPARSE_HPP
#define MUD_EXPPARSE_EXPPARSE_HPP

#include <vector>
#include <string>
namespace GlobalMUD{
    class ExpressionParser{
    public:
        class Word{
            std::string Text;
        public:
            Word(std::string text);
            operator std::string();
            bool operator==(std::string other);
            bool operator==(Word other);
            Word& operator=(std::string other);
            Word& operator=(Word other);
        };
        struct Action{
            std::string Subject, Verb, Delimiter, Preposition, Modifier;
        };
    private:
        static std::vector<Word> Verbs;
        static std::vector<Word> Delimiters;
        static std::vector<Word> Pronouns;
        static std::vector<Word> Irrelevant;
        static std::vector<Word> Prepositions;
        std::vector<Word> BonusVerbs;
        std::vector<Action> Actions;
        size_t ActionPosition;
    public:
        ExpressionParser();
        static void SetVerbs( std::vector<std::string> wordlist );
        static void SetDelimiters( std::vector<std::string> wordlist );
        static void SetPronouns( std::vector<std::string> wordlist );
        static void SetIrrelevant( std::vector<std::string> wordlist );
        static void SetPrepositions( std::vector<std::string> wordlist );
        void SetBonusVerbs( std::vector<std::string> wordlist );
        int Parse(std::string message);
        Action GetAction();
    };
}

#endif
