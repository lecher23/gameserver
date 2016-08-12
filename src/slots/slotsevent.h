#ifndef SLOTSEVENT_H
#define SLOTSEVENT_H

#include "slotsconfig.h"
#include "slotsdb.h"

namespace slots{
    struct SlotsEventData{
	int64_t earned;
	int64_t bet;
    };
    class SlotsEvent{
    public:
        SlotsEvent();
        ~SlotsEvent();

	void init();

	void processData(SlotsUserPtr user, SlotsEventData &date);

	void fortuneChange(UserResource &uRes, int64_t money);
	void expChange(UserResource &uRes, int64_t money);
	void vipPointChange(UserResource &res, int64_t money);
	void vipLevelUp(UserResource &res, int64_t money);

    private:
	SlotsConfig _slotsConfig;
    };
}
#endif
