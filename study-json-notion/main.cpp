#include "Common.h"
#include "CURL.h"
#include "NotionDBController.h"
#include "JsonController.h"

int main()
{
    //1. 새 레코드 추가
    FUserInfo user;
    user.Name = "Yeb";
    user.PhoneNumber = "999-8888-7777";
    user.Age = 28;
    user.bIsMarriage = true;
    CNotionDBController::GetInstance()->CreateUserRecord(user);

    //2. 레코드 조회
    std::map<std::string, FUserInfo> users;
    CNotionDBController::GetInstance()->ReadRecords(users);

    //3. 특정 레코드 수정 (Page ID는 직접 설정해야 함)
    auto iterEnd = users.end();
    --iterEnd;
    std::string page_id = iterEnd->first;
    CNotionDBController::GetInstance()->UpdateRecord(page_id);

    //4. 특정 레코드 삭제 (아카이브)
    auto iterFirst = users.begin();
    page_id = iterFirst->first;
    CNotionDBController::GetInstance()->DeleteRecord(page_id);

    CCURL::GetInstance()->ReleaseInstance();
    CNotionDBController::GetInstance()->ReleaseInstance();
    CJsonController::GetInstance()->ReleaseInstance();
    return 0;
}
