#include "sched.hpp"
#include <algorithm>
#include "picojson.h"
#include <fstream>

FunctionScheduler::FunctionScheduler()
{}


void FunctionScheduler::add_function(FunctionCallEssential *e)
{
        queue.push_back(e);
}

void FunctionScheduler::add_function(std::string fn, u64 t)
{
	queue.push_back(new FunctionCallEssential(fn, t, SHOT_MASTER_UNDEFINED));
}

void FunctionScheduler::clear_func_sched(void)
{
        for(FunctionCallEssential *p : queue){
                delete p;
        }
        queue.clear();
}

void FunctionScheduler::clear_func_sched(SHOT_MASTER_ID id)
{
        queue.remove_if([=](FunctionCallEssential *f_essential){
                                return f_essential->shot_master_id == id;
                        });
}

size_t FunctionScheduler::size(void)
{
	return queue.size();
}

FunctionCallEssential *FunctionScheduler::head(void)
{
        return queue.front();
}

FunctionCallEssential *FunctionScheduler::pop(void)
{
        FunctionCallEssential *ret = head();
        queue.pop_front();
        return ret;
}

std::list<FunctionCallEssential *>::iterator FunctionScheduler::begin(void)
{
        return std::begin(queue);
}

std::list<FunctionCallEssential *>::iterator FunctionScheduler::end(void)
{
        return std::end(queue);
}

void FunctionScheduler::remove_if(std::function<bool(FunctionCallEssential *)> fn)
{
        queue.remove_if(fn);
}

BulletScheduler::BulletScheduler()
{}

BulletData *BulletScheduler::next()
{
        return queue.front();
}

void BulletScheduler::drop()
{
        queue.pop_front();
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
        queue.push_back(data);
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

AbstractDanmakuData::AbstractDanmakuData(std::string f_name,
                                         std::wstring *d_name, u64 time_limit,
                                         DanmakuType type, std::string enemy_schedule_path)
        : func_name(f_name), enemy_object_schedule_path(enemy_schedule_path)
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
                                str_to_dnmaku_type(elem["type"].get<std::string>().data()),
                                elem["enemy_schedule_json"].get<std::string>());
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

void BulletScheduler::clear(void)
{
        for(BulletData *b : queue){
                delete b;
        }
        queue.clear();
}

void BulletScheduler::clear(SHOT_MASTER_ID id)
{
        queue.remove_if(
                [=](BulletData *b){
                        if(b->shot_master_id == id){
                                delete b;
                                return true;
                        }else{
                                return false;
                        }
                });
}

void BulletScheduler::remove_if(std::function<bool(BulletData *)> fn)
{
        queue.remove_if(fn);
}
