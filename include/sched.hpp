#pragma once

#include <queue>
#include <vector>
#include <functional>
#include <list>
#include "move_func.hpp"

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

enum DanmakuType {
        NORMAL_DANMAKU = 0,
        SPELL_CARD_DANMAKU,
        UNKNOWN_DANMAKU_TYPE,
};

inline DanmakuType str_to_dnmaku_type(const char *str)
{
        if (!strcmp(str, "normal")) {
		return NORMAL_DANMAKU;
	} else if (!strcmp(str, "spell")) {
		return SPELL_CARD_DANMAKU;
	}
        
	std::cout << "Unknown Danmaku Type: " << str << std::endl;

	return UNKNOWN_DANMAKU_TYPE;
}

class AbstractDanmakuData {
public:
        std::string func_name;
        std::wstring *danmaku_name;
        u64 time_limit;
        DanmakuType type;

        AbstractDanmakuData(std::string f_name, std::wstring *d_name, u64 time_limit, DanmakuType type);
};

class AbstractDanmakuSchedule {
private:
        std::vector<std::vector<AbstractDanmakuData> *> data_list;

public:
        AbstractDanmakuSchedule(std::vector<const char *> json_path_set);

        std::vector<AbstractDanmakuData> *get_front(void);
        void pop_front(void);
        size_t size(void);
        std::vector<AbstractDanmakuData> *at(int index);
};

class DanmakuCallEssential {
public:
        FunctionCallEssential func_essential;
        u64 time_limit;
        DanmakuType type;
        std::wstring *danmaku_name;
        
        DanmakuCallEssential(FunctionCallEssential fe, u64 sec, DanmakuType type, std::wstring *name);
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
        void push_back(DanmakuCallEssential e);
};
