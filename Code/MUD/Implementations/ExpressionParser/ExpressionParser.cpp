#include "ExpressionParser/ExpressionParser.hpp"
#include "Strings/Strings.hpp"
#include <cctype>

namespace GlobalMUD{
    std::vector<ExpressionParser::Word> ExpressionParser::Verbs;
    std::vector<ExpressionParser::Word> ExpressionParser::Who;
    std::vector<ExpressionParser::Word> ExpressionParser::Delimiters;
    std::vector<ExpressionParser::Word> ExpressionParser::Pronouns;
    std::vector<ExpressionParser::Word> ExpressionParser::Articles;
    std::vector<ExpressionParser::Word> ExpressionParser::Prepositions;


    ExpressionParser::Word::Word(std::string text){
        Text = text;
        ttype = Types::General;
    }

    void ExpressionParser::Word::SetType(Type::PossessiveType type){
        ttype = Types::Possessive;
        t.Possessive = type;
        printf("Possessive: %s ", Text.c_str() );
        switch( type ){
            case Type::PossessiveType::singular_1: printf("singular_1\n"); break;
            case Type::PossessiveType::singular_2: printf("singular_2\n"); break;
            case Type::PossessiveType::singular_3: printf("singular_3\n"); break;
            case Type::PossessiveType::plural_1: printf("plural_1\n"); break;
            case Type::PossessiveType::plural_2: printf("plural_2\n"); break;
            case Type::PossessiveType::plural_3: printf("plural_3\n"); break;
        }
    }

    void ExpressionParser::Word::SetType(Type::GeneralType type){
        ttype = Types::General;
        t.General = type;
    }

    bool ExpressionParser::Word::IsFirstPersonPossessive(){
        if( ttype == Types::Possessive ){
            if( t.Possessive == Type::PossessiveType::singular_1 || t.Possessive == Type::PossessiveType::plural_1 ){
                return true;
            }
        }
        return false;
    }

    ExpressionParser::Word::operator std::string(){
        return Text;
    }

    bool ExpressionParser::Word::operator==(std::string other){
        return Text == other;
    }

    bool ExpressionParser::Word::operator==(Word other){
        return Text == other.Text;
    }

    std::string ExpressionParser::Word::substr( size_t start, size_t end ){
        return Text.substr( start, end );
    }

    size_t ExpressionParser::Word::size(){
        return Text.size();
    }

    ExpressionParser::Word& ExpressionParser::Word::operator=(std::string other){
        Text = other;
        return *this;
    }

    ExpressionParser::Word& ExpressionParser::Word::operator=(Word other){
        Text = other.Text;
        return *this;
    }

    ExpressionParser::ExpressionParser(){
        ActionPosition = 0;
    }

    void ExpressionParser::SetVerbs( std::vector<std::string> wordlist ){
        for( size_t i = 0; i < wordlist.size(); ++i ){
            ExpressionParser::Verbs.push_back( wordlist[i] );
        }
    }

    void ExpressionParser::SetWho( std::vector<std::string> wordlist ){
        for( size_t i = 0; i < wordlist.size(); ++i ){
            size_t mid = wordlist[i].find_first_of('\t');
            std::string a = wordlist[i].substr(0,mid);
            Word w = a;
            if( mid != std::string::npos ){
                std::string b = wordlist[i].substr( mid + 1 );
                b = StringToLower( b );
                switch( HashString( b ) ){
                    case HashString( "singular_1" ): w.SetType( Word::Type::PossessiveType::singular_1 ); break;
                    case HashString( "singular_2" ): w.SetType( Word::Type::PossessiveType::singular_2 ); break;
                    case HashString( "singular_3" ): w.SetType( Word::Type::PossessiveType::singular_3 ); break;
                    case HashString( "plural_1" ): w.SetType( Word::Type::PossessiveType::plural_1 ); break;
                    case HashString( "plural_2" ): w.SetType( Word::Type::PossessiveType::plural_2 ); break;
                    case HashString( "plural_3" ): w.SetType( Word::Type::PossessiveType::plural_3 ); break;
                    default: break;
                }
            }
            ExpressionParser::Who.push_back( w );
        }
    }

    void ExpressionParser::SetDelimiters( std::vector<std::string> wordlist ){
        for( size_t i = 0; i < wordlist.size(); ++i ){
            ExpressionParser::Delimiters.push_back( wordlist[i] );
        }
    }

    void ExpressionParser::SetPronouns( std::vector<std::string> wordlist ){
        for( size_t i = 0; i < wordlist.size(); ++i ){
            ExpressionParser::Pronouns.push_back( wordlist[i] );
        }
    }

    void ExpressionParser::SetArticles( std::vector<std::string> wordlist ){
        for( size_t i = 0; i < wordlist.size(); ++i ){
            ExpressionParser::Articles.push_back( wordlist[i] );
        }
    }

    void ExpressionParser::SetPrepositions( std::vector<std::string> wordlist ){
        for( size_t i = 0; i < wordlist.size(); ++i ){
            ExpressionParser::Prepositions.push_back( wordlist[i] );
        }
    }

    void ExpressionParser::SetBonusVerbs( std::vector<std::string> wordlist ){
        for( size_t i = 0; i < wordlist.size(); ++i ){
            ExpressionParser::BonusVerbs.push_back( wordlist[i] );
        }
    }

    template<class T>
    static int VectorContains(std::vector<T> v, T value){
        for( size_t i = 0; i < v.size(); ++i ){
            if( v[i] == value ){
                return i + 1;
            }
        }
        return 0;
    }

    int ExpressionParser::Parse(std::string message){
        message = StringToLower( message );
        std::string word = "";
        std::vector<Word> words;
        char inquotes = 0;
        for( size_t i = 0; i < message.size(); ++i ){
            if( message[i] == '"' || message[i] == '\'' ){
                if( !inquotes && word == "" ){
                    inquotes = message[i];
                    continue;
                }
                else if( inquotes && message[i] == inquotes ){
                    inquotes = 0;
                    words.push_back( word );
                    word = "";
                    continue;
                }
            }
            if( ( isspace(message[i]) || ispunct(message[i]) ) && !inquotes && message[i] != '\'' ){
                if( word != "" ){
                    words.push_back( word );
                    word = "";
                }
                if( ispunct(message[i]) ){
                    std::string temp = "";
                    temp += message[i];
                    words.push_back( temp );
                }
            }
            else{
                word += message[i];
            }
        }
        if( word != "" ){
            words.push_back( word );
            word = "";
        }

        Actions.clear();
        ActionPosition = 0;
        Action a, aprev;
        std::string subject = "";
        a.Subject = a.Verb = a.Delimiter = a.Preposition = a.Modifier = a.Who = "";
        aprev = a;
        for( size_t i = 0; i < words.size(); ++i ){
            if( VectorContains(Articles, words[i] ) ){
                a.Who = "";
                aprev.Who = "";
                continue;
            }
            else if( VectorContains( Delimiters, words[i] ) ){
                if( a.Verb == "" ){
                    a.Verb = aprev.Verb;
                }
                if( a.Who == "" ){
                    a.Who = aprev.Who;
                }
                if( a.ModifierWho == "" ){
                    a.ModifierWho = aprev.ModifierWho;
                }
                if( !(a.Subject == "" || a.Verb == "") ){
                    Actions.push_back(a);
                    aprev = a;
                }
                a.Preposition = a.Modifier = "";
                a.Subject = a.Verb = a.Who = a.ModifierWho = "";
                a.Delimiter = words[i];
            }
            else if( a.Verb == "" && ( VectorContains( Verbs, words[i] ) || VectorContains( BonusVerbs, words[i] ) ) ){
                a.Verb = words[i];
                aprev.Who = aprev.ModifierWho = "";
            }
            else if( a.Subject == "" ){
                if( a.Verb != "" || aprev.Verb != "" ){
                    if( a.Who == "" && VectorContains( Who, words[i] ) ){
                        Word w = Who[VectorContains( Who, words[i] )-1];
                        if( !w.IsFirstPersonPossessive() ){
                            if( subject != "" ){
                                a.Who = subject;
                            }
                            else if( aprev.Who != "" ){
                                a.Who = aprev.Who;
                            }
                            else{
                                a.Who = words[i];
                            }
                        }
                        else {
                            a.Who = words[i];
                        }
                    }
                    else if( a.Who == "" && words[i].substr(words[i].size() - 2) == "'s" ){
                        subject = words[i].substr( 0, words[i].size() - 2 );
                        a.Who = subject;
                    }
                    else if( VectorContains( Pronouns, words[i] ) ){
                        if( subject != "" ){
                            a.Subject = subject;
                        }
                        else{
                            a.Subject = aprev.Subject;
                        }
                    }
                    else if( !VectorContains( Prepositions, words[i] ) ){
                        a.Subject = words[i];
                        if( subject == "" ){
                            subject = a.Subject;
                        }
                    }
                }
            }
            else if( a.Preposition == "" && VectorContains( Prepositions, words[i] ) ){
                a.Preposition = words[i];
            }
            else if( a.Preposition != "" ){
                if( a.Modifier != "" ){
                    a.Modifier += " ";
                }
                else if( VectorContains( Who, words[i] ) ){
                    Word w = Who[VectorContains( Who, words[i] )-1];
                    if( !w.IsFirstPersonPossessive() ){
                        if( subject != "" ){
                            a.ModifierWho = subject;
                        }
                        else if( aprev.ModifierWho != "" ){
                            a.ModifierWho = aprev.ModifierWho;
                        }
                        else{
                            a.ModifierWho = words[i];
                        }
                    }
                    else {
                        a.ModifierWho = words[i];
                    }
                }
                else if( a.ModifierWho == "" && words[i].substr(words[i].size() - 2) == "'s" ){
                    subject = words[i].substr( 0, words[i].size() - 2 );
                    a.ModifierWho = subject;
                }
                else{
                    a.Modifier += words[i];
                }
            }
            else if( a.Subject != "" && a.Verb != "" ){
                a.Subject += " ";
                a.Subject += words[i];
            }
        }
        if( a.Verb == "" ){
            a.Verb = aprev.Verb;
        }
        if( a.Who == "" ){
            a.Who = aprev.Who;
        }
        if( a.ModifierWho == "" ){
            a.ModifierWho = aprev.Who;
        }
        if( a.Verb == "" ){
            return Actions.size();
        }
        Actions.push_back(a);
        return Actions.size();
    }

    ExpressionParser::Action ExpressionParser::GetAction(){
        if( ActionPosition < Actions.size() )
            return Actions[ActionPosition++];
        Action a;
        a.Subject = a.Verb = a.Delimiter = a.Preposition = a.Modifier = "";
        return a;
    }
}
