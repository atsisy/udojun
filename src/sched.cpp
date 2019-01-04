#include "sched.hpp"

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
        for(auto d : data){
                add(d);
        }
}
