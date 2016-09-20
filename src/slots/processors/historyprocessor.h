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
    void processGameDetail(GameContext &context, SingleGameDetail &data) const;
    void processExp(GameContext &context, SingleGameDetail &data) const;
    void processMoney(GameContext &context, SingleGameDetail &data) const;
};

END_NAMESPACE
#endif
