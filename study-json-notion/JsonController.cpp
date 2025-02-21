#include "JsonController.h"
#include "NotionDBController.h"

CJsonController* CJsonController::mInst = nullptr;

CJsonController::CJsonController()
{
}

CJsonController::~CJsonController()
{
}

nlohmann::json CJsonController::ConvertToJson(const FUserInfo& userInfo)
{
    nlohmann::json jsonData;

    // 부모 데이터베이스 ID 설정
    jsonData[ATT_PARENT][ATT_DATABASE_ID] = CNotionDBController::GetInstance()->GetDatabaseID();
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
                                {"text", {{"content", userInfo.Name}}},
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


bool CJsonController::ParseUserInfo(const nlohmann::json& jsonData, std::map<std::string, FUserInfo>& users)
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
