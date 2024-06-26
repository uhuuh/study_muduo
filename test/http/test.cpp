#include <iostream>
#include <memory>
#include <sstream>
#include <fstream>
#include "HTTPMessageParser.hpp"
using namespace std;

string read_file(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Could not open the file: " << filePath << endl;
        return "";
    }
    stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return buffer.str();
}

unique_ptr<HTTPRequestMessage> test_parser_request(const string& text, bool is_request=true) {
    HTTPMessageParser parser(is_request);
    auto flag = parser.parser_message(text.c_str(), text.size());
    if (flag && parser.is_completed()) {
        auto message = parser.get_request_message();
        cout << "success" << endl;
        return message;
    } else {
        cout << "error" << endl;
        return nullptr;
    }
}

unique_ptr<HTTPResponseMessage> test_parser_response(const string& text, bool is_request=false) {
    HTTPMessageParser parser(is_request);
    auto flag = parser.parser_message(text.c_str(), text.size());
    if (flag && parser.is_completed()) {
        auto message = parser.get_response_message();
        cout << "success" << endl;
        return message;
    } else {
        cout << "error" << endl;
        return nullptr;
    }
}


template <typename T>
void test_creater(T& me) {
    MessageWriteCallback cb = [] (const char *ch, int len) {
        cout << string(ch, len);
    };
    HTTPMessageWriter writer(cb);
    cout << "******" << endl;
    writer.write_message(me);
    cout << "******" << endl;
}

void test() {
    string text_get = read_file("../test/http/example_get.txt");
    auto me_get = test_parser_request(text_get, true);
    test_creater(*me_get.get());

    string text_post = read_file("../test/http/example_post.txt");
    auto me_post = test_parser_request(text_post, true);
    test_creater(*me_post.get());

    string text_res = read_file("../test/http/example_response.txt");
    auto me_res = test_parser_response(text_res, false);
    test_creater(*me_res.get());
}

int main() {
    test();

    return 0;
}
