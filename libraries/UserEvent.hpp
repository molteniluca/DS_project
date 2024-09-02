#ifndef USER_EVENT_HPP
#define USER_EVENT_HPP

#include <string>

enum class UserEvent {
    CREATE_ROOM,
    SEND_MESSAGE,
    RESEND_CREATION,
    ASK_MESSAGES,
    DELETE_ROOM
};

std::string ue_toString(UserEvent ue);
UserEvent ue_fromString(std::string ue);

#endif // USER_EVENT_HPP
