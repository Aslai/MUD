#include "ExpressionParser/ExpressionParser.hpp"
#include "Strings/Strings.hpp"
#include <cctype>

namespace GlobalMUD{
    std::vector<ExpressionParser::Word> ExpressionParser::Verbs;
    std::vector<ExpressionParser::Word> ExpressionParser::Delimiters;
    std::vector<ExpressionParser::Word> ExpressionParser::Pronouns;
    std::vector<ExpressionParser::Word> ExpressionParser::Irrelevant;
    std::vector<ExpressionParser::Word> ExpressionParser::Prepositions;


    ExpressionParser::Word::Word(std::string text){
        Text = text;
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

    void ExpressionParser::SetIrrelevant( std::vector<std::string> wordlist ){
        for( size_t i = 0; i < wordlist.size(); ++i ){
            ExpressionParser::Irrelevant.push_back( wordlist[i] );
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
    static bool VectorContains(std::vector<T> v, T value){
        for( size_t i = 0; i < v.size(); ++i ){
            if( v[i] == value ){
                return true;
            }
        }
        return false;
    }

    int ExpressionParser::Parse(std::string message){
        message = StringToLower( message );
        std::string word = "";
        std::vector<Word> words;
        char inquotes = 0;
        for( size_t i = 0; i < message.size(); ++i ){
            if( message[i] == '"' || message[i] == '\'' ){
                if( !inquotes ){
                    inquotes = message[i];
                    continue;
                }
                else if( message[i] == inquotes ){
                    inquotes = 0;
                    words.push_back( word );
                    continue;
                }
            }
            if( ( isspace(message[i]) || ispunct(message[i]) ) && !inquotes ){
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
        a.Subject = a.Verb = a.Delimiter = a.Preposition = a.Modifier = "";
        aprev = a;
        for( size_t i = 0; i < words.size(); ++i ){
            if( VectorContains(Irrelevant, words[i] ) ){
                continue;
            }
            else if( VectorContains( Delimiters, words[i] ) ){
                if( a.Verb == "" ){
                    a.Verb = aprev.Verb;
                }
                if( !(a.Subject == "" || a.Verb == "") ){
                    Actions.push_back(a);
                    aprev = a;
                }
                a.Preposition = a.Modifier = "";
                a.Subject = a.Verb = "";
                a.Delimiter = words[i];
            }
            else if( a.Verb == "" && ( VectorContains( Verbs, words[i] ) || VectorContains( BonusVerbs, words[i] ) ) ){
                a.Verb = words[i];
            }
            else if( a.Subject == "" ){
                if( VectorContains( Pronouns, words[i] ) ){
                    a.Subject = aprev.Subject;
                }
                else if( !VectorContains( Prepositions, words[i] ) ){
                    a.Subject = words[i];
                }
            }
            else if( a.Preposition == "" && VectorContains( Prepositions, words[i] ) ){
                a.Preposition = words[i];
            }
            else if( a.Preposition != "" ){
                if( a.Modifier != "" ){
                    a.Modifier += " ";
                }
                a.Modifier += words[i];
            }
            else if( a.Subject != "" && a.Verb != "" ){
                a.Subject += " ";
                a.Subject += words[i];
            }
        }
        if( a.Verb == "" ){
            a.Verb = aprev.Verb;
        }
        if( a.Subject == "" || a.Verb == "" ){
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
