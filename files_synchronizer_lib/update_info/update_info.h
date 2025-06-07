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
    UpdateInfo() = default;
    UpdateInfo(WhoEnum newer, ModificationEvent modificationEvent);

    [[nodiscard]] WhoEnum getNewer() const;
    [[nodiscard]] ModificationEvent getModificationEvent() const;
};

#endif //UPDATE_INFO_H
