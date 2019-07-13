#include "sched.hpp"
#include <algorithm>

BulletScheduler::BulletScheduler()
        : queue(c)
{}

BulletData *BulletScheduler::next()
{
        return queue.top();
}

void BulletScheduler::drop()
{
        queue.pop();
}

bool BulletScheduler::empty()
{
        return queue.empty();
}

size_t BulletScheduler::size()
{
        return queue.size();
}

void BulletScheduler::add(BulletData *data)
{
        queue.push(data);
}

void BulletScheduler::add(std::vector<BulletData *> data)
{
        for(auto &&d : data){
                add(d);
        }
}

void BulletScheduler::add(std::vector<BulletData *> *data)
{
	for (auto &&d : *data) {
		add(d);
	}
}

DanmakuCallEssential::DanmakuCallEssential(FunctionCallEssential fe,
					    u64 sec)
        : func_essential(fe), time_limit(sec)
{}

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

void BulletScheduler::clear(void)
{
        while(queue.size()){
                delete queue.top();
                queue.pop();
        }
}
