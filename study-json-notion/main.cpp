#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <curl/curl.h> 

struct FUserInfo
{
	std::string Name;
	std::string PhoneNumber;
	int Age;
	bool bIsMarriage;
};

void to_json(nlohmann::json& j, const FUserInfo& userInfo)
{
	j = nlohmann::json{ {"name", userInfo.Name}
						, {"phone_number", userInfo.PhoneNumber}
						, {"age", userInfo.Age}
						, {"is_marriage", userInfo.bIsMarriage} };
}

void from_json(const nlohmann::json& j, FUserInfo& userInfo)
{
	j.at("name").get_to(userInfo.Name);
	j.at("phone_number").get_to(userInfo.PhoneNumber);
	j.at("age").get_to(userInfo.Age);
	j.at("is_marriage").get_to(userInfo.bIsMarriage);
}

std::vector<FUserInfo> ParseUserInfo(const nlohmann::json& jsonData)
{
	std::vector<FUserInfo> userList;

	if (jsonData.contains("results") && jsonData["results"].is_array())
	{
		for (const auto& item : jsonData["results"])
		{
			FUserInfo user;

			// name
			if (item.contains("properties")
				&& item["properties"].contains("name"))
			{
				auto titleArray = item["properties"]["name"]["title"];
				if (!titleArray.empty() && titleArray[0].contains("plain_text"))
				{
					user.Name = titleArray[0]["plain_text"].get<std::string>();
				}
			}

			// phone_number
			if (item["properties"].contains("phone_number")
				&& item["properties"]["phone_number"].contains("phone_number"))
			{
				user.PhoneNumber = item["properties"]["phone_number"]["phone_number"].get<std::string>();
			}

			// age
			if (item["properties"].contains("age")
				&& item["properties"]["age"].contains("number"))
			{
				user.Age = item["properties"]["age"]["number"].get<int>();
			}

			// is_marriage
			if (item["properties"].contains("is_marriage")
				&& item["properties"]["is_marriage"].contains("checkbox"))
			{
				user.bIsMarriage = item["properties"]["is_marriage"]["checkbox"].get<bool>();
			}

			userList.push_back(user);
		}
	}
	return userList;
}

// Notion API 설정
const std::string NOTION_API_KEY = "Bearer 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";  // 여기에 API 키 입력
const std::string DATABASE_ID = "19e45759635e8028adb0d83e3cf969ff";  // 여기에 Notion 데이터베이스 ID 입력
const std::string BASE_URL = "https://api.notion.com/v1/";

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
	curl_easy_setopt(curl, CURLOPT_CAINFO, "./pem/cacert.pem");

	std::string response;
	struct curl_slist* headers = NULL;

	// 헤더 추가
	headers = curl_slist_append(headers, ("Authorization: " + NOTION_API_KEY).c_str());
	headers = curl_slist_append(headers, "Notion-Version: 2022-06-28");
	headers = curl_slist_append(headers, "Content-Type: application/json");

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	// HTTP Method 설정
	if (method == "POST")
	{
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
	}
	else if (method == "PATCH")
	{
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
	}
	else if (method == "DELETE")
	{
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
	}

	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		std::cerr << "cURL request failed: " << curl_easy_strerror(res) << std::endl;
	}

	// 리소스 해제
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	return response;
}

void CreateRecord()
{
	std::cout << "CreateRecord()" << std::endl;
	std::string url = BASE_URL + "pages";

	FUserInfo user;
	user.Name = "Yeb";
	user.PhoneNumber = "321-654-9878";
	user.Age = 28;
	user.bIsMarriage = true;

	nlohmann::json jsonData = user;

	std::string response = SendRequest(url, "POST", jsonData.dump());
	std::cout << "Create Response:\n" << response << std::endl;
}

void ReadRecords()
{
	std::cout << "ReadRecords()" << std::endl;
	std::string url = BASE_URL + "databases/" + DATABASE_ID + "/query";
	std::string response = SendRequest(url, "POST", "{}"); // Notion API는 GET이 아니라 POST 사용!

	// JSON 파싱
	nlohmann::json jsonData = nlohmann::json::parse(response);
	std::vector<FUserInfo> users = ParseUserInfo(jsonData);

	// 결과 출력
	for (const auto& user : users)
	{
		std::cout << "Name: " << user.Name <<
			", Phone: " << user.PhoneNumber <<
			", Age: " << user.Age <<
			", Married: " << (user.bIsMarriage ? "Yes" : "No") << std::endl;
	}
}

void UpdateRecord(const std::string& page_id)
{
	std::string url = BASE_URL + "pages/" + page_id;
	std::string jsonData = R"({
        "properties": {
            "Status": { "select": { "name": "Completed" } }
        }
    })";

	std::string response = SendRequest(url, "PATCH", jsonData);
	std::cout << "Update Response:\n" << response << std::endl;
}

void DeleteRecord(const std::string& page_id)
{
	std::string url = BASE_URL + "pages/" + page_id;
	std::string jsonData = R"({ "archived": true })";

	std::string response = SendRequest(url, "PATCH", jsonData);
	std::cout << "Delete (Archive) Response:\n" << response << std::endl;
}

int main()
{
	//1. 새 레코드 추가
	CreateRecord();

	//2. 레코드 조회
	ReadRecords();

	////3. 특정 레코드 수정 (Page ID는 직접 설정해야 함)
	//std::string page_id = "YOUR_PAGE_ID_HERE"; // 실제 ID로 변경해야 함
	//UpdateRecord(page_id);

	////4. 특정 레코드 삭제 (아카이브)
	//DeleteRecord(page_id);

	return 0;
}
