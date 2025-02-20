#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include "ConstString.h"

struct FUserInfo
{
    std::string Name;
    std::string PhoneNumber;
    int Age;
    bool bIsMarriage;
};

void PrintRecords(const std::map<std::string, FUserInfo>& users);
bool CreateUserRecord(const FUserInfo& userInfo);
bool ReadRecords(std::map<std::string, FUserInfo>& users);
bool UpdateRecord(const std::string& page_id);
bool DeleteRecord(const std::string& page_id);

void to_json(nlohmann::json& j, const FUserInfo& userInfo)
{
    j = nlohmann::json{
        {PROP_NAME, userInfo.Name}, {PROP_PHONE_NUMBER, userInfo.PhoneNumber}, {PROP_AGE, userInfo.Age},
        {PROP_IS_MARRIAGE, userInfo.bIsMarriage}
    };
}

void from_json(const nlohmann::json& j, FUserInfo& userInfo)
{
    j.at(PROP_NAME).get_to(userInfo.Name);
    j.at(PROP_PHONE_NUMBER).get_to(userInfo.PhoneNumber);
    j.at(PROP_AGE).get_to(userInfo.Age);
    j.at(PROP_IS_MARRIAGE).get_to(userInfo.bIsMarriage);
}

bool ParseUserInfo(const nlohmann::json& jsonData, std::map<std::string, FUserInfo>& users)
{
    users.clear();

    if (jsonData.contains(RESULT) && jsonData[RESULT].is_array())
    {
        for (const auto& item : jsonData[RESULT])
        {
            FUserInfo user;
            std::string pageID = item[ID];

            // name
            if (item.contains(PROPERTIES)
                && item[PROPERTIES].contains(PROP_NAME))
            {
                auto titleArray = item[PROPERTIES][PROP_NAME][ATT_TITLE];
                if (!titleArray.empty() && titleArray[0].contains(ATT_PLAIN_TEXT))
                {
                    user.Name = titleArray[0][ATT_PLAIN_TEXT].get<std::string>();
                }
            }

            // phone_number
            if (item[PROPERTIES].contains(PROP_PHONE_NUMBER)
                && item[PROPERTIES][PROP_PHONE_NUMBER].contains(PROP_PHONE_NUMBER))
            {
                user.PhoneNumber = item[PROPERTIES][PROP_PHONE_NUMBER][PROP_PHONE_NUMBER].get<std::string>();
            }

            // age
            if (item[PROPERTIES].contains(PROP_AGE)
                && item[PROPERTIES][PROP_AGE].contains(ATT_NUMBER))
            {
                user.Age = item[PROPERTIES][PROP_AGE][ATT_NUMBER].get<int>();
            }

            // is_marriage
            if (item[PROPERTIES].contains(PROP_IS_MARRIAGE)
                && item[PROPERTIES][PROP_IS_MARRIAGE].contains(ATT_CHECKBOX))
            {
                user.bIsMarriage = item[PROPERTIES][PROP_IS_MARRIAGE][ATT_CHECKBOX].get<bool>();
            }

            users.emplace(std::pair<std::string, FUserInfo>(pageID, user));
        }
    }

    if (users.empty())
        return false;

    return true;
}

nlohmann::json ConvertToJson(const FUserInfo& userInfo)
{
    nlohmann::json jsonData;

    // 부모 데이터베이스 ID 설정
    jsonData[ATT_PARENT][ATT_DATABASE_ID] = DATABASE_ID;
    jsonData[ATT_PARENT][ATT_TYPE] = ATT_DATABASE_ID;

    // 개별 속성 설정
    nlohmann::json ageProperty;
    ageProperty[ATT_NUMBER] = userInfo.Age;
    ageProperty[ATT_TYPE] = ATT_NUMBER;

    nlohmann::json marriedProperty;
    marriedProperty[ATT_CHECKBOX] = userInfo.bIsMarriage;
    marriedProperty[ATT_TYPE] = ATT_CHECKBOX;

    nlohmann::json nameProperty;
    nameProperty =
    {
        {ATT_TITLE, {
                        {
                            {"text",
                                {{"content", userInfo.Name}}
                            },
                            {ATT_PLAIN_TEXT, userInfo.Name}
                        }
        }}
    };

    /*
        // origin.
        {
            "title": [
                {
                    "text": {
                        "content": "Yeb"
                    },
                    "plain_text": "Yeb"
                }
            ]
        }

        // nlohmann json format in c++.
        {
            {"title", {
                    {
                        {"text", {{"content", "Yeb"}}},
                        {"plain_text", "Yeb"}
                    }
            }}
        };

        // convert to my code.
        {
            {ATT_TITLE, {
                    {
                        {"text", {{"content", userInfo.Name}}},
                        {ATT_PLAIN_TEXT, userInfo.Name}
                    }
            }}
        };
    */

    std::cout << nameProperty.dump(4) << std::endl;

    nlohmann::json phoneProperty;
    phoneProperty[PROP_PHONE_NUMBER] = userInfo.PhoneNumber;
    phoneProperty[ATT_TYPE] = PROP_PHONE_NUMBER;

    // 모든 속성을 합쳐서 properties에 추가
    jsonData[PROPERTIES] = {
        {PROP_AGE, ageProperty},
        {PROP_IS_MARRIAGE, marriedProperty},
        {PROP_NAME, nameProperty},
        {PROP_PHONE_NUMBER, phoneProperty}
    };

    return jsonData;
}


// 응답 데이터 저장 콜백 함수
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output)
{
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

// cURL 요청을 실행하는 함수
std::string SendRequest(const std::string& url, const std::string& method, const std::string& jsonData = "")
{
    CURL* curl = curl_easy_init();
    if (!curl) return "Failed to initialize cURL";

    // SSL 인증서 파일 설정 (SSL 검증을 위한 인증서 경로)
    curl_easy_setopt(curl, CURLOPT_CAINFO, CACERT_PATH.c_str());

    std::string response;
    struct curl_slist* headers = NULL;

    // 헤더 추가
    headers = curl_slist_append(headers, (HEADER_AUTHORIZATION + NOTION_API_KEY).c_str());
    headers = curl_slist_append(headers, HEADER_NOTION_VERSION.c_str());
    headers = curl_slist_append(headers, HEADER_CONTENT_TYPE.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // HTTP Method 설정
    if (method == METHOD_POST)
    {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
    }
    else if (method == METHOD_PATCH)
    {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, METHOD_PATCH.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
    }
    else if (method == METHOD_DELETE)
    {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, METHOD_DELETE.c_str());
    }

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        std::cerr << "cURL request failed: " << curl_easy_strerror(res) << "\n";
    }

    // 리소스 해제
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return response;
}

bool CreateUserRecord(const FUserInfo& userInfo)
{
    const std::string url = BASE_URL + "pages";
    nlohmann::json jsonData = ConvertToJson(userInfo);

    std::string jsonString = jsonData.dump(4); // JSON을 문자열로 변환

    std::string response = SendRequest(url, METHOD_POST, jsonString);

    if (response.empty())
        return false;

        // std::cout << "jsonString:\n" << jsonString << "\n";
        // std::cout << "\n";
        // std::cout << "Response:\n" << response << "\n";
    return true;
}

bool ReadRecords(std::map<std::string, FUserInfo>& users)
{
    std::cout << "ReadRecords()" << "\n";
    const std::string url = BASE_URL + "databases/" + DATABASE_ID + "/query";
    std::string response = SendRequest(url, METHOD_POST, "{}"); // Notion API는 GET이 아니라 POST 사용!

    if (response.empty())
        return false;
    
    nlohmann::json jsonData = nlohmann::json::parse(response);
    if (ParseUserInfo(jsonData, users) == false)
        return false;

    PrintRecords(users);
    return true;
}

bool UpdateRecord(const std::string& page_id)
{
    const std::string url = BASE_URL + "pages/" + page_id;
    nlohmann::json requestBody;
    requestBody[PROPERTIES][PROP_PHONE_NUMBER][PROP_PHONE_NUMBER] = "010-1234-5678";

    // 변경한 해당 레코드를 반환해준다.
    std::string response = SendRequest(url, METHOD_PATCH, requestBody.dump());

    if (response.empty())
    {
        return false;
    }
    
    std::cout << "Update Response:\n" << response << "\n";
    return true;
}

bool DeleteRecord(const std::string& page_id)
{
    const std::string url = BASE_URL + "pages/" + page_id;
    nlohmann::json jsonData;
    jsonData[ATT_ARCHIVED] = true;

    std::string response = SendRequest(url, METHOD_PATCH, jsonData.dump());

    if (response.empty())
    {
        return false;
    }

    std::cout << "Delete (Archive) Response:\n" << response << "\n";
    return true;
}

void PrintRecords(const std::map<std::string, FUserInfo>& users)
{
    for (const auto& user : users)
    {
        std::cout << "Name: " << user.second.Name <<
            ", Phone: " << user.second.PhoneNumber <<
            ", Age: " << user.second.Age <<
            ", Married: " << (user.second.bIsMarriage ? "Yes" : "No") <<
            ", PageID: " << user.first << "\n";
    }
}

int main()
{
    //1. 새 레코드 추가
    FUserInfo user;
    user.Name = "Yeb";
    user.PhoneNumber = "999-8888-7777";
    user.Age = 28;
    user.bIsMarriage = true;
    CreateUserRecord(user);

    //2. 레코드 조회
    std::map<std::string, FUserInfo> users;
    ReadRecords(users);

    //3. 특정 레코드 수정 (Page ID는 직접 설정해야 함)
    auto iterEnd = users.end();
    --iterEnd;
    std::string page_id = iterEnd->first;
    UpdateRecord(page_id);

    //4. 특정 레코드 삭제 (아카이브)
    auto iterFirst = users.begin();
    page_id = iterFirst->first;
    DeleteRecord(page_id);

    return 0;
}
