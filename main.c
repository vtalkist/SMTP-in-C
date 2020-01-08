#define CURL_STATICLIB

#include "curl\curl.h"
#include <stdio.h>
#include <string.h>


#define FROM    "<>" //e=mail
#define TO      "<>" //e=mail
#define CC      "<>" //e=mail

static const char* payload_text[] = {
  "Date: Mon, 08 Jan 2019 21:54:29 +1100\r\n",
  "To: " TO "\r\n",
  "From: " FROM "(Example User)\r\n",
  "Cc: " CC "(Another example User)\r\n",
  "Message-ID: <dcd7cb36-11db-4аа87a-9f3fdfda-ergrtgtrffаfhb"
  "rfcpedant.example.org>\r\n",
  "Subject: SMTP  from C language\r\n",
  "\r\n", /* empty line to divide headers from body, see RFC5322 */
  "Hello, Ivanka!.\r\n",
  NULL
};

struct upload_status {
    int lines_read;
};

static size_t payload_source(void* ptr, size_t size, size_t nmemb, void* userp)
{
    struct upload_status* upload_ctx = (struct upload_status*)userp;
    const char* data;

    if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1)) {
        return 0;
    }

    data = payload_text[upload_ctx->lines_read];

    if (data) {
        size_t len = strlen(data);
        memcpy(ptr, data, len);
        upload_ctx->lines_read++;

        return len;
    }

    return 0;
}

int main(void)
{
    CURL* curl;
    CURLcode res = CURLE_OK;
    struct curl_slist* recipients = NULL;
    struct upload_status upload_ctx;

    upload_ctx.lines_read = 0;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com:465");
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
        curl_easy_setopt(curl, CURLOPT_USERNAME, ""); //e=mail
        curl_easy_setopt(curl, CURLOPT_PASSWORD, ""); //password 
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM);
        
        recipients = curl_slist_append(recipients, TO);
        recipients = curl_slist_append(recipients, CC);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        res = curl_easy_perform(curl);

        /* Check for errors */
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));

        curl_slist_free_all(recipients);

        curl_easy_cleanup(curl);
    }

    return (int)res;
}
