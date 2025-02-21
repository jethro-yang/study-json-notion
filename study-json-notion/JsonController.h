#pragma once

#include "Common.h"

namespace nlohmann {
    template <>
    struct adl_serializer<FUserInfo> {
        static void to_json(json& j, const FUserInfo& userInfo) {
            j = nlohmann::json{
                {PROP_NAME, userInfo.Name},
                {PROP_PHONE_NUMBER, userInfo.PhoneNumber},
                {PROP_AGE, userInfo.Age},
                {PROP_IS_MARRIAGE, userInfo.bIsMarriage}
            };
        }

        static void from_json(const json& j, FUserInfo& userInfo) {
            j.at(PROP_NAME).get_to(userInfo.Name);
            j.at(PROP_PHONE_NUMBER).get_to(userInfo.PhoneNumber);
            j.at(PROP_AGE).get_to(userInfo.Age);
            j.at(PROP_IS_MARRIAGE).get_to(userInfo.bIsMarriage);
        }
    };
}


class CJsonController
{
private:
    CJsonController();
    ~CJsonController();

private:
    static CJsonController* mInst;
    
public:
    static CJsonController* GetInstance()
    {
        if (mInst == nullptr)
            mInst = new CJsonController();
        return mInst;
    }

    static void ReleaseInstance()
    {
        if (mInst)
            delete mInst;
    }

    nlohmann::json ConvertToJson(const FUserInfo& userInfo);
    bool ParseUserInfo(const nlohmann::json& jsonData, std::map<std::string, FUserInfo>& users);
};
