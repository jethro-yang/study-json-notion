#include "NotionDBController.h"
#include "CURL.h"
#include "JsonController.h"

CNotionDBController* CNotionDBController::mInst = nullptr;
CNotionDBController::CNotionDBController()
{
}

CNotionDBController::~CNotionDBController()
{
}

bool CNotionDBController::CreateUserRecord(const FUserInfo& userInfo)
{
    const std::string url = BASE_URL + "pages";
    nlohmann::json jsonData = CJsonController::GetInstance()->ConvertToJson(userInfo);

    std::string jsonString = jsonData.dump(4); // JSON을 문자열로 변환

    std::string response = CCURL::GetInstance()->SendRequest(url
        , CCURL::GetInstance()->METHOD_POST
        , jsonString);

    if (response.empty())
        return false;

    // std::cout << "jsonString:\n" << jsonString << "\n";
    // std::cout << "\n";
    // std::cout << "Response:\n" << response << "\n";
    return true;
}

bool CNotionDBController::ReadRecords(std::map<std::string, FUserInfo>& users)
{
    std::cout << "ReadRecords()" << "\n";
    const std::string url = BASE_URL + "databases/" + DATABASE_ID + "/query";

    // Notion API는 GET이 아니라 POST 사용!
    std::string response = CCURL::GetInstance()->SendRequest(url
        , CCURL::GetInstance()->METHOD_POST
        , "{}"); 

    if (response.empty())
        return false;
    
    nlohmann::json jsonData = nlohmann::json::parse(response);
    if (CJsonController::GetInstance()->ParseUserInfo(jsonData, users) == false)
        return false;

    PrintRecords(users);
    return true;
}

bool CNotionDBController::UpdateRecord(const std::string& page_id)
{
    const std::string url = BASE_URL + "pages/" + page_id;
    nlohmann::json requestBody;
    requestBody[PROPERTIES][PROP_PHONE_NUMBER][PROP_PHONE_NUMBER] = "010-1234-5678";

    // 변경한 해당 레코드를 반환해준다.
    std::string response = CCURL::GetInstance()->SendRequest(url
        , CCURL::GetInstance()->METHOD_PATCH
        , requestBody.dump());

    if (response.empty())
        return false;
    
    std::cout << "Update Response:\n" << response << "\n";
    return true;
}

bool CNotionDBController::DeleteRecord(const std::string& page_id)
{
    const std::string url = BASE_URL + "pages/" + page_id;
    nlohmann::json jsonData;
    jsonData[ATT_ARCHIVED] = true;

    std::string response = CCURL::GetInstance()->SendRequest(url
    , CCURL::GetInstance()->METHOD_PATCH
    , jsonData.dump());

    if (response.empty())
        return false;

    std::cout << "Delete (Archive) Response:\n" << response << "\n";
    return true;
}



