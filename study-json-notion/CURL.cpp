#include "CURL.h"

#include "NotionDBController.h"

CCURL* CCURL::mInst = nullptr;
CCURL::CCURL(){}

CCURL::~CCURL(){}

// 응답 데이터 저장 콜백 함수
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output)
{
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

std::string CCURL::SendRequest(const std::string& InURL, const std::string& InMethod, const std::string& InJsonData)
{
    CURL* curl = curl_easy_init();
    if (!curl) return "Failed to initialize cURL";

    // SSL 인증서 파일 설정 (SSL 검증을 위한 인증서 경로)
    curl_easy_setopt(curl, CURLOPT_CAINFO, CACERT_PATH.c_str());

    std::string response;
    std::string auth = HEADER_AUTHORIZATION+CNotionDBController::GetInstance()->GetNotionAPIKey();
    std::string version = HEADER_NOTION_VERSION + CNotionDBController::GetInstance()->GetLastUpdateDate();

    // 헤더 추가
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, auth.c_str());
    headers = curl_slist_append(headers, version.c_str());
    headers = curl_slist_append(headers, HEADER_CONTENT_TYPE.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, InURL.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // HTTP Method 설정
    if (InMethod == METHOD_POST)
    {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, InJsonData.c_str());
    }
    else if (InMethod == METHOD_PATCH)
    {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, METHOD_PATCH.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, InJsonData.c_str());
    }
    else if (InMethod == METHOD_DELETE)
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
