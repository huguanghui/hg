#include <iostream>
#include "hv/http_client.h"
#include "hv/json.hpp"

using namespace std;
using nlohmann::json;

namespace nsa
{
    struct person {
        std::string name;
        std::string address;
        int age;
    };    

    void to_json(json& j, const person& p)
    {
        j = json{{"name", p.name}, {"address", p.address}, {"age": p.age}};
    }

    void from_json(const json& j, person& p)
    {
        j.at("name").get_to(p.name);
        j.at("address").get_to(p.address);
        j.at("age").get_to(p.age);
    }
} /* nsa */ 

static int log_send(int type, const char *log)
{
    HttpRequest req;
    HttpResponse res;

    req.method = HTTP_GET;
    req.url = "http://119.45.214.139/api/fv1/log";
    // 1. 获取IP
    // 2. 获取当前时间
    // 3. 设置日志信息
    req.body = "";
    int ret = http_client_send(&req, &res);
    printf("%s\n", res.Dump(true, true).c_str());
    return 0;
}

static void test_json()
{
    nsa::person p = {"NAF", "WUHAN", 28};
    json j = p;
    cout << j << endl;
    return;
}

int main(int argc, char** argv)
{
    test_json();
    return 0;
}
