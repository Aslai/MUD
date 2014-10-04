#ifndef MUD_EXPPARSE_EXPPARSE_HPP
#define MUD_EXPPARSE_EXPPARSE_HPP

#include <vector>
#include <string>
namespace GlobalMUD{
    class ExpressionParser{
    public:
        class Word{
        public:
            union Type{
                enum class PossessiveType{
                    singular_1,
                    singular_2,
                    singular_3,
                    plural_1,
                    plural_2,
                    plural_3
                } Possessive;
                enum class GeneralType{
                    standard
                } General;
            };
            enum class Types{
                General,
                Possessive
            };
        private:
            Type t;
            Types ttype;
            std::string Text;
        public:
            Word(std::string text);
            void SetType(Type::PossessiveType type);
            void SetType(Type::GeneralType type);
            bool IsFirstPersonPossessive();
            operator std::string();
            bool operator==(std::string other);
            bool operator==(Word other);
            Word& operator=(std::string other);
            Word& operator=(Word other);
            std::string substr( size_t start, size_t end = std::string::npos );
            size_t size();
        };
        struct Action{
            std::string Who, Subject, Verb, Delimiter, Preposition, Modifier, ModifierWho;
        };
    private:
        static std::vector<Word> Verbs;
        static std::vector<Word> Who;
        static std::vector<Word> Delimiters;
        static std::vector<Word> Pronouns;
        static std::vector<Word> Articles;
        static std::vector<Word> Prepositions;
        std::vector<Word> BonusVerbs;
        std::vector<Action> Actions;
        size_t ActionPosition;
    public:
        ExpressionParser();
        static void SetVerbs( std::vector<std::string> wordlist );
        static void SetWho( std::vector<std::string> wordlist );
        static void SetDelimiters( std::vector<std::string> wordlist );
        static void SetPronouns( std::vector<std::string> wordlist );
        static void SetArticles( std::vector<std::string> wordlist );
        static void SetPrepositions( std::vector<std::string> wordlist );
        void SetBonusVerbs( std::vector<std::string> wordlist );
        int Parse(std::string message);
        Action GetAction();
    };
}

#endif
