#ifndef MUD_GAME_INTERACTION_EVENT_HPP
#define MUD_GAME_INTERACTION_EVENT_HPP



namespace GlobalMUD{
    namespace Game{
        class Event{
        public:
            enum class Operator{
                LessThan,
                LessThanOrEqual,
                Equal,
                NotEqual,
                GreaterThan,
                GreaterThanOrEqual,
                Plus,
                Minus,
                Times,
                DivideBy,
                Value
            };

        };
    }
}

#endif
