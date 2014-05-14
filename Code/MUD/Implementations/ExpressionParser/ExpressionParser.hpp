#ifndef MUD_EXPPARSE_EXPPARSE_HPP
#define MUD_EXPPARSE_EXPPARSE_HPP

#include <vector>
#include <string>
namespace MUD{
    class ExpressionParser{
        class Word{
            enum Types{
                ERROR = 0,
                NOUN,
                VERB,
                CONJUNCTION,
                ARTICLE,
                ADJECTIVE,
                ADVERB,
                PRONOUN,
                PPREPOSITION
            };
            int Type;
            std::string Text;
        public:
            Word(std::string text, MUD::ExpressionParser::Types type);
            operator std::string();
            operator int();
            class Pronoun : public Word{
                Pronoun(std::string text, MUD::ExpressionParser::Types type );
            };
        };
        std::vector<Word> Words;
        std::vector<Word> Dictionary1;
        std::vector<Word> Dictionary2;
    public:
        ExpressionParser();
        void SetDictionary1( std::vector<MUD::ExpressionParser::Word> wordlist )
    };
}

#endif
