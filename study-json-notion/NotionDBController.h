#pragma once

#include "Common.h"

class CNotionDBController
{
private:
    CNotionDBController();
    ~CNotionDBController();

private:
    static CNotionDBController* mInst;

    const std::string NOTION_API_KEY = "Bearer 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";
    const std::string DATABASE_ID = "19e45759635e8028adb0d83e3cf969ff";
    const std::string BASE_URL = "https://api.notion.com/v1/";
    const std::string LAST_UPDATE_DATE = "2022-06-28";

public:
    static CNotionDBController* GetInstance()
    {
        if (!mInst)
            mInst = new CNotionDBController();
        return mInst;
    }

    static void ReleaseInstance()
    {
        if (mInst)
            delete mInst;
    }

    const std::string GetNotionAPIKey()
    {
        return NOTION_API_KEY;
    }

    const std::string GetLastUpdateDate()
    {
        return LAST_UPDATE_DATE;
    }

    const std::string GetDatabaseID()
    {
        return DATABASE_ID;
    }

    template <typename T>
    void PrintRecords(const std::map<std::string, T>& records)
    {
    }

    template <>
    void PrintRecords<FUserInfo>(const std::map<std::string, FUserInfo>& users)
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

    bool CreateUserRecord(const FUserInfo& userInfo);
    bool UpdateRecord(const std::string& page_id);
    bool DeleteRecord(const std::string& page_id);
    bool ReadRecords(std::map<std::string, FUserInfo>& users);
};
