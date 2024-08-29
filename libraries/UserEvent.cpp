#include "UserEvent.hpp"

#include <stdexcept>

std::string ue_toString(UserEvent ue)
{
    switch(ue) {
        case UserEvent::CREATE_ROOM:
            return "CREATE_ROOM";
        case UserEvent::SEND_MESSAGE:
            return "SEND_MESSAGE";
    }
}

UserEvent ue_fromString(std::string ue)
{
    if(ue == "CREATE_ROOM")
        return UserEvent::CREATE_ROOM;
    else if(ue == "SEND_MESSAGE")
        return UserEvent::SEND_MESSAGE;
    
    throw std::invalid_argument("Invalid RandomEvent string");
}
