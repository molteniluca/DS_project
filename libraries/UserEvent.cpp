#include "UserEvent.hpp"

#include <stdexcept>

std::string ue_toString(UserEvent ue)
{
    switch(ue) {
        case UserEvent::CREATE_ROOM:
            return "CREATE_ROOM";
        case UserEvent::SEND_MESSAGE:
            return "SEND_MESSAGE";
        case UserEvent::RESEND_CREATION:
            return "RESEND_CREATION";
        case UserEvent::DELETE_ROOM:
            return "DELETE_ROOM";
        default:
            throw std::invalid_argument("Invalid UserEvent");
    }
}

UserEvent ue_fromString(std::string ue)
{
    if(ue == "CREATE_ROOM")
        return UserEvent::CREATE_ROOM;
    else if(ue == "SEND_MESSAGE")
        return UserEvent::SEND_MESSAGE;
    else if(ue == "RESEND_CREATION")
        return UserEvent::RESEND_CREATION;
    else if(ue == "DELETE_ROOM")
        return UserEvent::DELETE_ROOM;    
    
    throw std::invalid_argument("Invalid RandomEvent string");
}
