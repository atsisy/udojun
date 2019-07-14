#pragma once

#include <queue>
#include <vector>
#include <functional>
#include "move_func.hpp"
#include "programable.hpp"

constexpr auto c = [](const BulletData *d1, const BulletData *d2){
                           return d1->appear_time > d2->appear_time;
                   };
constexpr auto func_scheduler_compare = [](const FunctionCallEssential *e1, const FunctionCallEssential *e2) {
                                                return e1->time > e2->time;
                                        };

class FunctionScheduler {
private:
        std::priority_queue<
        FunctionCallEssential *, std::vector<FunctionCallEssential *>,
        std::function<bool(const FunctionCallEssential *, const FunctionCallEssential *)> >
        queue;

public:
        FunctionScheduler(void);
        void add_function(FunctionCallEssential *e);
	void add_function(std::string fn, u64 t);
        void clear_func_sched(void);
        size_t size(void);
        FunctionCallEssential *head(void);
	FunctionCallEssential *pop(void);
};

class BulletScheduler {
private:
    std::priority_queue<
	    BulletData *, std::vector<BulletData *>,
        std::function<bool(const BulletData *, const BulletData *)> > queue;

public:
        BulletScheduler(void);
        BulletData *next();
        void drop(void);
        bool empty(void);
        size_t size(void);
        void add(BulletData *data);
        void add(std::vector<BulletData *> data);
	void add(std::vector<BulletData *> *data);
        void clear(void);
};

class DanmakuCallEssential {
public:
        FunctionCallEssential func_essential;
        u64 time_limit;
        
        DanmakuCallEssential(FunctionCallEssential fe, u64 sec);
        
};

class DanmakuScheduler {
private:
        std::vector<DanmakuCallEssential> schedule;
        
public:
        DanmakuScheduler(std::vector<DanmakuCallEssential> s);
        bool function_is_coming(u64 count);
        DanmakuCallEssential drop_top(void);
	DanmakuCallEssential top(void);
        size_t size(void);
};
