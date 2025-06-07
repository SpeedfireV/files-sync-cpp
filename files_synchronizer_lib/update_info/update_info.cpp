//
// Created by grzeg on 25/05/11.
//

#include "update_info.h"

UpdateInfo::UpdateInfo(const WhoEnum newer, const ModificationEvent modificationEvent): newer(newer), modificationEvent(modificationEvent) {}

[[nodiscard]] WhoEnum UpdateInfo::getNewer() const {
    return newer;
}

[[nodiscard]] ModificationEvent UpdateInfo::getModificationEvent() const {
    return modificationEvent;
}
