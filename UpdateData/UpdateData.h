#ifndef UPDATE_DATA_H
#define UPDATE_DATA_H

enum ChangeType {
    None = 0,
    Created,
    Deleted,
    Updated
};

enum ChangeSource {
    Path1,
    Path2,
    Both
};

struct UpdateData {
    ChangeType change;
    ChangeSource source;
};

#endif // UPDATE_DATA_H