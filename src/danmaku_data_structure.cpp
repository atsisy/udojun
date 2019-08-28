#include "enemy_character.hpp"

DanmakuCallEssential::DanmakuCallEssential(FunctionCallEssential fe,
                                           u64 sec, DanmakuType type, std::wstring *name,
                                           std::string enemy_sched_json, GameData *game_data)
        : func_essential(fe), time_limit(sec), enemy_sched(game_data, enemy_sched_json.data())
{
        this->type = type;
        this->danmaku_name = name;
}

DanmakuScheduler::DanmakuScheduler(std::vector<DanmakuCallEssential> s)
	: schedule(s)
{
        std::reverse(std::begin(schedule), std::end(schedule));
}

bool DanmakuScheduler::function_is_coming(u64 count)
{
	if (schedule.size()) {
		return schedule.back().func_essential.time == count;
	}else{
                return false;
	}
}

DanmakuCallEssential DanmakuScheduler::drop_top(void)
{
        DanmakuCallEssential f = schedule.back();
        schedule.pop_back();
        return f;
}

DanmakuCallEssential DanmakuScheduler::top(void)
{
        return schedule.back();
}

size_t DanmakuScheduler::size(void)
{
	return schedule.size();
}

void DanmakuScheduler::push_back(DanmakuCallEssential e)
{
        schedule.push_back(e);
}

