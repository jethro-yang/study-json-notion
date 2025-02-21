#pragma once

#include "Common.h"

class CCURL
{
private:
    CCURL();
    ~CCURL();
private:
    static CCURL* mInst;
private:
    const std::string HEADER_AUTHORIZATION = "Authorization: ";
    const std::string HEADER_NOTION_VERSION = "Notion-Version: ";
    const std::string HEADER_CONTENT_TYPE = "Content-Type: application/json";

public:
    const std::string CACERT_PATH = "./pem/cacert.pem";
    const std::string METHOD_POST = "POST";
    const std::string METHOD_PATCH = "PATCH";
    const std::string METHOD_DELETE = "DELETE";
    
public:
    static CCURL* GetInstance()
    {
        if (mInst == nullptr)
            mInst = new CCURL();
        return mInst;
    }

    static void ReleaseInstance()
    {
        if (mInst)
            delete mInst;
    }

    std::string SendRequest(const std::string& InURL, const std::string& InMethod, const std::string& InJsonData = "");
};
