#pragma once

#include <queue>
#include <vector>
#include <functional>
#include "move_func.hpp"

constexpr auto c = [](const BulletData *d1, const BulletData *d2){
                           return d1->appear_time > d2->appear_time;
                   };

class BulletScheduler {
private:
    std::priority_queue<
	    BulletData *, std::vector<BulletData *>,
        std::function<bool(const BulletData *, const BulletData *)> > queue;

public:
        BulletScheduler();
        BulletData *next();
        void drop();
        bool empty();
        size_t size();
        void add(BulletData *data);
        void add(std::vector<BulletData *> data);
};
