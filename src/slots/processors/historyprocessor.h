#ifndef HISTORYPROCESSOR_H
#define HISTORYPROCESSOR_H

#include <slots/data/basicdata.h>
#include <slots/data/gamecontext.h>

BEGIN_NAMESPACE(slots)

class HistoryProcessor{
 public:
    HistoryProcessor();
    ~HistoryProcessor();

    bool process(GameContext &context) const;
 private:
    void processGameDetail(SlotsUserPtr &user, SingleGameDetail &data) const;
    void processExp(SlotsUserPtr &user, SingleGameDetail &data) const;
    void processMoney(SlotsUserPtr &user, SlotsEventData &data) const;
};

END_NAMESPACE
#endif
