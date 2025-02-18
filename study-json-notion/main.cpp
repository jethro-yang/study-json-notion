#include <iostream>      // 표준 입출력 헤더 (std::cout, std::cerr 사용)
#include <string>        // std::string 사용을 위한 헤더
#include <curl/curl.h>   // libcurl 라이브러리 헤더 (HTTP 요청을 위해 필요)

// 콜백 함수: 응답 데이터를 받아서 문자열에 저장
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;        // 실제 데이터 크기 계산
    output->append((char*)contents, totalSize); // 받은 데이터를 output 문자열에 추가
    return totalSize;                        // 처리한 데이터 크기 반환
}

int main() {
    CURL* curl;               // cURL 핸들
    CURLcode res;             // cURL 요청 결과 코드
    std::string response;     // 서버 응답을 저장할 문자열

    curl_global_init(CURL_GLOBAL_ALL);  // cURL 라이브러리 초기화
    curl = curl_easy_init();            // cURL 핸들 생성

    if (curl) {  // cURL 핸들이 정상적으로 생성되었는지 확인
        // CA 인증서 파일 설정 (SSL 검증을 위한 인증서 경로)
        curl_easy_setopt(curl, CURLOPT_CAINFO, "./pem/cacert.pem");

        // 요청할 URL 설정 (네이버 홈페이지 요청)
        curl_easy_setopt(curl, CURLOPT_URL, "https://www.example.com/");

        // 응답 데이터를 처리할 콜백 함수 설정
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response); // WriteCallback에 response 전달

        //// SSL 인증서 검증 비활성화 (보안 취약하지만, 인증서 문제가 있을 때 해결 가능)
        // curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        // curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        // HTTP 요청 실행
        res = curl_easy_perform(curl);

        // 요청이 성공했는지 확인
        if (res != CURLE_OK) {
            // 요청이 실패한 경우 오류 메시지 출력
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        else {
            // 요청이 성공한 경우 서버 응답 출력
            std::cout << "Response:\n" << response << std::endl;
        }

        curl_easy_cleanup(curl);  // cURL 핸들 정리
    }

    curl_global_cleanup();  // cURL 라이브러리 종료
    return 0;               // 프로그램 종료
}
