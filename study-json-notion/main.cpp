#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <curl/curl.h>   // libcurl ���̺귯�� ��� (HTTP ��û�� ���� �ʿ�)

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

// �ݹ� �Լ�: ���� �����͸� �޾Ƽ� ���ڿ��� ����
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output)
{
	size_t totalSize = size * nmemb;        // ���� ������ ũ�� ���
	output->append((char*)contents, totalSize); // ���� �����͸� output ���ڿ��� �߰�
	return totalSize;                        // ó���� ������ ũ�� ��ȯ5
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
		// SSL ������ ���� ���� (SSL ������ ���� ������ ���)
		curl_easy_setopt(curl, CURLOPT_CAINFO, "./pem/cacert.pem");

		// HTTP ��� ����
		struct curl_slist* headers = NULL;
		std::string BearerString = "Authorization: Bearer ";
		std::string SecretKey = "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";
		std::string AuthResult = BearerString + SecretKey;

		// ��� �߰�
		headers = curl_slist_append(headers, AuthResult.c_str());
		headers = curl_slist_append(headers, "Notion-Version: 2022-06-28");
		headers = curl_slist_append(headers, "Content-Type: application/json");

		// URL ����
		std::string URL = "https://api.notion.com/v1/databases/19e45759635e8028adb0d83e3cf969ff/query";
		curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); // ��� ����

		// POST ��û ���� (�ʼ�)
		curl_easy_setopt(curl, CURLOPT_POST, 1L);

		// POST ��û ���� �߰� (Notion API�� ���� �ٸ�)
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{}"); // �� JSON body �߰�

		// ���� �����͸� ó���� �ݹ� �Լ� ����
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		//// SSL ������ ���� ���� (�׽�Ʈ��, �ʿ��ϸ� ���� ����)
		//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

		// HTTP ��û ����
		res = curl_easy_perform(curl);

		if (res != CURLE_OK)
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		else
		{
			std::string EncordedString = response;
			// JSON �Ľ�
			nlohmann::json jsonData = nlohmann::json::parse(EncordedString);
			std::vector<FUserInfo> users = ParseUserInfo(jsonData);

			//std::cout << "Response:\n" << EncordedString << std::endl;

			// ��� ���
			for (const auto& user : users) 
			{
				std::cout << "Name: " << user.Name << 
					", Phone: " << user.PhoneNumber << 
					", Age: " << user.Age << 
					", Married: " << (user.bIsMarriage ? "Yes" : "No") << std::endl;
			}
		}

		// �޸� ����
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
//	JSON ���� ���� �Ϸ�: data.json
//	�ҷ��� JSON ������:
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
//	�̸�: "John Doe"
//	����: 30
//*/
//int main()
//{
//	// JSON ������ ����
//	json myData =
//	{
//		{"name", "John Doe"},
//		{"age", 30},
//		{"skills", {"C++", "Python", "JavaScript"}},
//		{"isEmployed", true}
//	};
//
//	// JSON �����͸� ���Ϸ� ����
//	std::ofstream outFile("data.json");
//	if (outFile.is_open())
//	{
//		outFile << myData.dump(4); // ���� ���� ���ĵ� JSON ����
//		outFile.close();
//		std::cout << "JSON ���� ���� �Ϸ�: data.json\n";
//	}
//	else
//	{
//		std::cerr << "������ �� �� �����ϴ�!\n";
//		return 1;
//	}
//
//	// JSON ���� �б�
//	std::ifstream inFile("data.json");
//	if (inFile.is_open())
//	{
//		json loadedData;
//		inFile >> loadedData;
//		inFile.close();
//
//		// �ҷ��� ������ ���
//		std::cout << "�ҷ��� JSON ������:\n" << loadedData.dump(4) << "\n";
//
//		// Ư�� �� ����
//		std::cout << "�̸�: " << loadedData["name"] << "\n";
//		std::cout << "����: " << loadedData["age"] << "\n";
//	}
//	else
//	{
//		std::cerr << "������ ���� �� �����ϴ�!\n";
//		return 1;
//	}
//
//	return 0;
//}



///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////


//#include <iostream>      // ǥ�� ����� ��� (std::cout, std::cerr ���)
//#include <string>        // std::string ����� ���� ���
//#include <curl/curl.h>   // libcurl ���̺귯�� ��� (HTTP ��û�� ���� �ʿ�)
//
//// �ݹ� �Լ�: ���� �����͸� �޾Ƽ� ���ڿ��� ����
//size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
//    size_t totalSize = size * nmemb;        // ���� ������ ũ�� ���
//    output->append((char*)contents, totalSize); // ���� �����͸� output ���ڿ��� �߰�
//    return totalSize;                        // ó���� ������ ũ�� ��ȯ
//}
//
//int main() {
//    CURL* curl;               // cURL �ڵ�
//    CURLcode res;             // cURL ��û ��� �ڵ�
//    std::string response;     // ���� ������ ������ ���ڿ�
//
//    curl_global_init(CURL_GLOBAL_ALL);  // cURL ���̺귯�� �ʱ�ȭ
//    curl = curl_easy_init();            // cURL �ڵ� ����
//
//    if (curl) {  // cURL �ڵ��� ���������� �����Ǿ����� Ȯ��
//        // CA ������ ���� ���� (SSL ������ ���� ������ ���)
//        curl_easy_setopt(curl, CURLOPT_CAINFO, "./pem/cacert.pem");
//
//        // ��û�� URL ���� (���̹� Ȩ������ ��û)
//        curl_easy_setopt(curl, CURLOPT_URL, "https://www.example.com/");
//
//        // ���� �����͸� ó���� �ݹ� �Լ� ����
//        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
//        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response); // WriteCallback�� response ����
//
//        //// SSL ������ ���� ��Ȱ��ȭ (���� ���������, ������ ������ ���� �� �ذ� ����)
//        // curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
//        // curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
//
//        // HTTP ��û ����
//        res = curl_easy_perform(curl);
//
//        // ��û�� �����ߴ��� Ȯ��
//        if (res != CURLE_OK) {
//            // ��û�� ������ ��� ���� �޽��� ���
//            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
//        }
//        else {
//            // ��û�� ������ ��� ���� ���� ���
//            std::cout << "Response:\n" << response << std::endl;
//        }
//
//        curl_easy_cleanup(curl);  // cURL �ڵ� ����
//    }
//
//    curl_global_cleanup();  // cURL ���̺귯�� ����
//    return 0;               // ���α׷� ����
//}
