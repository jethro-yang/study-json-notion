#pragma once

// Notion API const
const std::string NOTION_API_KEY = "Bearer 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";
const std::string DATABASE_ID = "19e45759635e8028adb0d83e3cf969ff";
const std::string BASE_URL = "https://api.notion.com/v1/";
const std::string CACERT_PATH = "./pem/cacert.pem";

const std::string HEADER_AUTHORIZATION = "Authorization: ";
const std::string HEADER_NOTION_VERSION = "Notion-Version: 2022-06-28";
const std::string HEADER_CONTENT_TYPE = "Content-Type: application/json";

const std::string METHOD_POST = "POST";
const std::string METHOD_PATCH = "PATCH";
const std::string METHOD_DELETE = "DELETE";


#define RESULT "results"
#define PROPERTIES "properties"
#define ID "id"

#define ATT_TITLE "title"
#define ATT_PLAIN_TEXT "plain_text"
#define ATT_NUMBER "number"
#define ATT_CHECKBOX "checkbox"
#define ATT_PARENT "parent"
#define ATT_TYPE "type"
#define ATT_DATABASE_ID "database_id"
#define ATT_ARCHIVED "archived"

#define PROP_NAME "name"
#define PROP_PHONE_NUMBER "phone_number"
#define PROP_AGE "age"
#define PROP_IS_MARRIAGE "is_marriage"
