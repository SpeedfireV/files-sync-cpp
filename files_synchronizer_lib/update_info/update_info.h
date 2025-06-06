//
// Created by grzeg on 25/05/11.
//

#ifndef UPDATE_INFO_H
#define UPDATE_INFO_H
#include "../modification_event/ModificationEvent.h"
#include "../who_enum/who_enum.h"


class UpdateInfo {
private:
    WhoEnum newer;
    ModificationEvent modificationEvent;
public:
    UpdateInfo(WhoEnum newer, ModificationEvent modificationEvent);

    WhoEnum getNewer();
    ModificationEvent getModificationEvent();


};



#endif //UPDATE_INFO_H
