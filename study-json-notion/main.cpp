#include <iostream>      // ǥ�� ����� ��� (std::cout, std::cerr ���)
#include <string>        // std::string ����� ���� ���
#include <curl/curl.h>   // libcurl ���̺귯�� ��� (HTTP ��û�� ���� �ʿ�)

// �ݹ� �Լ�: ���� �����͸� �޾Ƽ� ���ڿ��� ����
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;        // ���� ������ ũ�� ���
    output->append((char*)contents, totalSize); // ���� �����͸� output ���ڿ��� �߰�
    return totalSize;                        // ó���� ������ ũ�� ��ȯ
}

int main() {
    CURL* curl;               // cURL �ڵ�
    CURLcode res;             // cURL ��û ��� �ڵ�
    std::string response;     // ���� ������ ������ ���ڿ�

    curl_global_init(CURL_GLOBAL_ALL);  // cURL ���̺귯�� �ʱ�ȭ
    curl = curl_easy_init();            // cURL �ڵ� ����

    if (curl) {  // cURL �ڵ��� ���������� �����Ǿ����� Ȯ��
        // CA ������ ���� ���� (SSL ������ ���� ������ ���)
        curl_easy_setopt(curl, CURLOPT_CAINFO, "./pem/cacert.pem");

        // ��û�� URL ���� (���̹� Ȩ������ ��û)
        curl_easy_setopt(curl, CURLOPT_URL, "https://www.example.com/");

        // ���� �����͸� ó���� �ݹ� �Լ� ����
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response); // WriteCallback�� response ����

        //// SSL ������ ���� ��Ȱ��ȭ (���� ���������, ������ ������ ���� �� �ذ� ����)
        // curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        // curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        // HTTP ��û ����
        res = curl_easy_perform(curl);

        // ��û�� �����ߴ��� Ȯ��
        if (res != CURLE_OK) {
            // ��û�� ������ ��� ���� �޽��� ���
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        else {
            // ��û�� ������ ��� ���� ���� ���
            std::cout << "Response:\n" << response << std::endl;
        }

        curl_easy_cleanup(curl);  // cURL �ڵ� ����
    }

    curl_global_cleanup();  // cURL ���̺귯�� ����
    return 0;               // ���α׷� ����
}
