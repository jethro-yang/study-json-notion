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

			//std::cout << "Response:\n" << EncordedString << std::endl;

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




///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

//#include <iostream>
//#include <fstream>
//#include <nlohmann/json.hpp>
//
//using json = nlohmann::json;
//
///*
//	JSON 파일 저장 완료: data.json
//	불러온 JSON 데이터:
//	{
//		"age": 30,
//		"isEmployed": true,
//		"name": "John Doe",
//		"skills": [
//			"C++",
//			"Python",
//			"JavaScript"
//		]
//	}
//	이름: "John Doe"
//	나이: 30
//*/
//int main()
//{
//	// JSON 데이터 생성
//	json myData =
//	{
//		{"name", "John Doe"},
//		{"age", 30},
//		{"skills", {"C++", "Python", "JavaScript"}},
//		{"isEmployed", true}
//	};
//
//	// JSON 데이터를 파일로 저장
//	std::ofstream outFile("data.json");
//	if (outFile.is_open())
//	{
//		outFile << myData.dump(4); // 보기 좋게 정렬된 JSON 저장
//		outFile.close();
//		std::cout << "JSON 파일 저장 완료: data.json\n";
//	}
//	else
//	{
//		std::cerr << "파일을 열 수 없습니다!\n";
//		return 1;
//	}
//
//	// JSON 파일 읽기
//	std::ifstream inFile("data.json");
//	if (inFile.is_open())
//	{
//		json loadedData;
//		inFile >> loadedData;
//		inFile.close();
//
//		// 불러온 데이터 출력
//		std::cout << "불러온 JSON 데이터:\n" << loadedData.dump(4) << "\n";
//
//		// 특정 값 접근
//		std::cout << "이름: " << loadedData["name"] << "\n";
//		std::cout << "나이: " << loadedData["age"] << "\n";
//	}
//	else
//	{
//		std::cerr << "파일을 읽을 수 없습니다!\n";
//		return 1;
//	}
//
//	return 0;
//}



///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////


//#include <iostream>      // 표준 입출력 헤더 (std::cout, std::cerr 사용)
//#include <string>        // std::string 사용을 위한 헤더
//#include <curl/curl.h>   // libcurl 라이브러리 헤더 (HTTP 요청을 위해 필요)
//
//// 콜백 함수: 응답 데이터를 받아서 문자열에 저장
//size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
//    size_t totalSize = size * nmemb;        // 실제 데이터 크기 계산
//    output->append((char*)contents, totalSize); // 받은 데이터를 output 문자열에 추가
//    return totalSize;                        // 처리한 데이터 크기 반환
//}
//
//int main() {
//    CURL* curl;               // cURL 핸들
//    CURLcode res;             // cURL 요청 결과 코드
//    std::string response;     // 서버 응답을 저장할 문자열
//
//    curl_global_init(CURL_GLOBAL_ALL);  // cURL 라이브러리 초기화
//    curl = curl_easy_init();            // cURL 핸들 생성
//
//    if (curl) {  // cURL 핸들이 정상적으로 생성되었는지 확인
//        // CA 인증서 파일 설정 (SSL 검증을 위한 인증서 경로)
//        curl_easy_setopt(curl, CURLOPT_CAINFO, "./pem/cacert.pem");
//
//        // 요청할 URL 설정 (네이버 홈페이지 요청)
//        curl_easy_setopt(curl, CURLOPT_URL, "https://www.example.com/");
//
//        // 응답 데이터를 처리할 콜백 함수 설정
//        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
//        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response); // WriteCallback에 response 전달
//
//        //// SSL 인증서 검증 비활성화 (보안 취약하지만, 인증서 문제가 있을 때 해결 가능)
//        // curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
//        // curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
//
//        // HTTP 요청 실행
//        res = curl_easy_perform(curl);
//
//        // 요청이 성공했는지 확인
//        if (res != CURLE_OK) {
//            // 요청이 실패한 경우 오류 메시지 출력
//            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
//        }
//        else {
//            // 요청이 성공한 경우 서버 응답 출력
//            std::cout << "Response:\n" << response << std::endl;
//        }
//
//        curl_easy_cleanup(curl);  // cURL 핸들 정리
//    }
//
//    curl_global_cleanup();  // cURL 라이브러리 종료
//    return 0;               // 프로그램 종료
//}
