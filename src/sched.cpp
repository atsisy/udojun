#include "sched.hpp"
#include <algorithm>
#include "picojson.h"
#include <fstream>

FunctionScheduler::FunctionScheduler()
        : queue(func_scheduler_compare)
{}


void FunctionScheduler::add_function(FunctionCallEssential *e)
{
        queue.emplace(e);
}

void FunctionScheduler::add_function(std::string fn, u64 t)
{
	queue.emplace(new FunctionCallEssential(fn, t));
}

void FunctionScheduler::clear_func_sched(void)
{
	while (queue.size()) {
		delete queue.top();
		queue.pop();
	}
}

size_t FunctionScheduler::size(void)
{
	return queue.size();
}

FunctionCallEssential *FunctionScheduler::head(void)
{
        return queue.top();
}

FunctionCallEssential *FunctionScheduler::pop(void)
{
        FunctionCallEssential *ret = head();
        queue.pop();
        return ret;
}

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
                                           u64 sec, DanmakuType type, std::wstring *name)
        : func_essential(fe), time_limit(sec)
{
        this->type = type;
        this->danmaku_name = name;
}

DanmakuScheduler::DanmakuScheduler(std::vector<DanmakuCallEssential> s)
	: schedule(s)
{
        std::reverse(std::begin(schedule), std::end(schedule));
}

AbstractDanmakuData::AbstractDanmakuData(std::string f_name,
                                         std::wstring *d_name, u64 time_limit,
                                         DanmakuType type)
        : func_name(f_name)
{
        this->time_limit = time_limit;
        this->type = type;
        this->danmaku_name = d_name;
}

AbstractDanmakuSchedule::AbstractDanmakuSchedule(std::vector<const char *> json_path_set)
{
        for(const char *json_path : json_path_set){
                std::ifstream ifs(json_path, std::ios::in);
                std::vector<AbstractDanmakuData> *tmp_buf = new std::vector<AbstractDanmakuData>;
                
                std::cout << "Loading Danmaku Schedule listed in " << json_path << "..." << std::endl;

                if (ifs.fail()) {
                        DEBUG_PRINT_HERE();
                        std::cerr << "failed to read json file" << std::endl;
                        exit(1);
                }

                const std::string json((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
                ifs.close();

                picojson::value v;
                const std::string err = picojson::parse(v, json);
                if (err.empty() == false) {
                        DEBUG_PRINT_HERE();
                        std::cerr << err << std::endl;
                        exit(1);
                }
        
                picojson::object &obj = v.get<picojson::object>();
                picojson::array &array = obj["danmaku_list"].get<picojson::array>();

                for(auto &array_element : array){
                        picojson::object &elem = array_element.get<picojson::object>();
                        tmp_buf->emplace_back(
                                elem["function"].get<std::string>(),
                                util::utf8_str_to_widechar_str(elem["danmaku_name"].get<std::string>()),
                                elem["time_limit"].get<double>(),
                                str_to_dnmaku_type(elem["type"].get<std::string>().data()));
                }

                data_list.push_back(tmp_buf);
        }
        
        std::cout << "Danmaku Schedule have been loaded." << std::endl;
}

std::vector<AbstractDanmakuData> *AbstractDanmakuSchedule::get_front(void)
{
        return data_list.front();
}

std::vector<AbstractDanmakuData> *AbstractDanmakuSchedule::at(int index)
{
        return data_list[index];
}

size_t AbstractDanmakuSchedule::size(void)
{
        return data_list.size();
}

void AbstractDanmakuSchedule::pop_front(void)
{
        data_list.erase(std::begin(data_list));
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

