//
// Created by grzeg on 25/05/11.
//

#include "update_info.h"

UpdateInfo::UpdateInfo(const WhoEnum newer, const ModificationEvent modificationEvent): newer(newer), modificationEvent(modificationEvent) {}

WhoEnum UpdateInfo::getNewer() {
    return newer;
}

ModificationEvent UpdateInfo::getModificationEvent() {
    return modificationEvent;
}
