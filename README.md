# Notion API 연동 (nlohmann/json, curl)

# 들어간 기능

- [json in C++](https://www.notion.so/json-in-C-19e45759635e80b884e7d2521c57646b?pvs=21)
- [프로젝트 속성(경로 매크로)](https://www.notion.so/19e45759635e8049ae22ecdbaa06099d?pvs=21)
- [CURL For Windows](https://www.notion.so/CURL-For-Windows-19e45759635e80fe8580f987129be8e0?pvs=21)

# 할꺼

- raw 데이터 보고 파싱하는거 코드 한번 보기 어떻게 파싱하는지.
- POST 데이터 입력하기.

# 코드 - GET

![image.png](image.png)

```cpp
Name: Anthony, Phone: 031-757-7364, Age: 3, Married: No
Name: Jethro, Phone: 010-3212-6545, Age: 70, Married: No
Name: YangKkuk, Phone: 010-9973-7364, Age: 35, Married: Yes
```

```cpp
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <curl/curl.h>   // libcurl 라이브러리 헤더 (HTTP 요청을 위해 필요)

struct FUserInfo
{
	std::string Name;
	std::string PhoneNumber;
	int Age;
	bool bIsMarriage;
};

std::vector<FUserInfo> ParseUserInfo(const nlohmann::json& jsonData)
{
	std::vector<FUserInfo> userList;

	if (jsonData.contains("results") && jsonData["results"].is_array())
	{
		for (const auto& item : jsonData["results"])
		{
			FUserInfo user;

			// name
			if (item.contains("properties") && item["properties"].contains("name"))
			{
				auto titleArray = item["properties"]["name"]["title"];
				if (!titleArray.empty() && titleArray[0].contains("plain_text"))
				{
					user.Name = titleArray[0]["plain_text"].get<std::string>();
				}
			}

			// phone_number
			if (item["properties"].contains("phone_number") && item["properties"]["phone_number"].contains("phone_number"))
			{
				user.PhoneNumber = item["properties"]["phone_number"]["phone_number"].get<std::string>();
			}

			// age
			if (item["properties"].contains("age") && item["properties"]["age"].contains("number"))
			{
				user.Age = item["properties"]["age"]["number"].get<int>();
			}

			// is_marriage
			if (item["properties"].contains("is_marriage") && item["properties"]["is_marriage"].contains("checkbox"))
			{
				user.bIsMarriage = item["properties"]["is_marriage"]["checkbox"].get<bool>();
			}

			userList.push_back(user);
		}
	}
	return userList;
}

// 콜백 함수: 응답 데이터를 받아서 문자열에 저장
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output)
{
	size_t totalSize = size * nmemb;        // 실제 데이터 크기 계산
	output->append((char*)contents, totalSize); // 받은 데이터를 output 문자열에 추가
	return totalSize;                        // 처리한 데이터 크기 반환5
}

int main()
{
	CURL* curl;
	CURLcode res;
	std::string response;

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();

	if (curl)
	{
		// SSL 인증서 파일 설정 (SSL 검증을 위한 인증서 경로)
		curl_easy_setopt(curl, CURLOPT_CAINFO, "./pem/cacert.pem");

		// HTTP 헤더 설정
		struct curl_slist* headers = NULL;
		std::string BearerString = "Authorization: Bearer ";
		std::string SecretKey = "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";
		std::string AuthResult = BearerString + SecretKey;

		// 헤더 추가
		headers = curl_slist_append(headers, AuthResult.c_str());
		headers = curl_slist_append(headers, "Notion-Version: 2022-06-28");
		headers = curl_slist_append(headers, "Content-Type: application/json");

		// URL 설정
		std::string URL = "https://api.notion.com/v1/databases/19e45759635e8028adb0d83e3cf969ff/query";
		curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); // 헤더 적용

		// POST 요청 설정 (필수)
		curl_easy_setopt(curl, CURLOPT_POST, 1L);

		// POST 요청 본문 추가 (Notion API에 따라 다름)
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{}"); // 빈 JSON body 추가

		// 응답 데이터를 처리할 콜백 함수 설정
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		//// SSL 인증서 검증 해제 (테스트용, 필요하면 설정 가능)
		//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

		// HTTP 요청 실행
		res = curl_easy_perform(curl);

		if (res != CURLE_OK)
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		else
		{
			std::string EncordedString = response;
			// JSON 파싱
			nlohmann::json jsonData = nlohmann::json::parse(EncordedString);
			std::vector<FUserInfo> users = ParseUserInfo(jsonData);

			std::cout << "Response:\n" << EncordedString << std::endl;

			// 결과 출력
			for (const auto& user : users) 
			{
				std::cout << "Name: " << user.Name << 
					", Phone: " << user.PhoneNumber << 
					", Age: " << user.Age << 
					", Married: " << (user.bIsMarriage ? "Yes" : "No") << std::endl;
			}
		}

		// 메모리 해제
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);
	}

	curl_global_cleanup();
	return 0;
}
```