#include <stdlib.h>
#include <string.h>

#include <jansson.h>
#include <curl/curl.h>

#define RESULT_SIZE (256 * 1024)  /* CURL result, 256 KB*/

#define URL_FORMAT "https://world.openfoodfacts.org/api/v0/product/%s.json"
#define URL_SIZE 256

/*
 * Struc used to build the result of the CURL request
 * Contains a string (final data) and position/size of the string
 */
struct write_result {
    char *data;
    int pos;
};

/*
 * Callback function called by CURL
 * Copies `size * nmemb` bytes from the `buffer` to the `write_result` struct (stream)
 * (`size` is always = 1)
 */
static size_t write_response(void *buffer, size_t size, size_t nmemb, void *stream) {
    // Get pointer to stream (final result)
    struct write_result *result = (struct write_result *)stream;

    // Final result should be <= RESULT_SIZE
    if(result->pos + size * nmemb >= RESULT_SIZE - 1) {
        fprintf(stderr, "error: too small buffer\n");
        return 0;
    }

    // Copy `size * nmemb` bytes from `buffer` to `result->data` at `result->pos` position of the array
    memcpy(result->data + result->pos, buffer, size * nmemb);
    // Increment ending position by the number of copied bytes
    result->pos += size * nmemb;

    // Returns number of copied bytes
    return size * nmemb;
}

/*
 * Takes an URL and returns the HTTP document
 */
static char *request(const char *url) {
    // CURL init
    CURL *curl = NULL;
    CURLcode status; // Status of the CURL request
    char *data = NULL; // HTTP document
    long code; // HTTP response code

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(!curl)
        goto error;

    // Final result has to be <= RESULT_SIZE
    data = malloc(RESULT_SIZE);
    if(!data)
        goto error;

    // Struct used to build the final result (`data`)
    struct write_result write_result = {
        .data = data,
        .pos = 0
    };

    // Set CURL parameters
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_response); // Defines callback function
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &write_result);

    // Make HTTP request
    status = curl_easy_perform(curl);
    if(status != 0) {
        fprintf(stderr, "error: unable to request data from %s:\n", url);
        fprintf(stderr, "%s\n", curl_easy_strerror(status));
        goto error;
    }

    // Check HTTP response code
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    if(code != 200) {
        fprintf(stderr, "error: server responded with code %ld\n", code);
        goto error;
    }

    // Clean memory
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    // Properly end string
    data[write_result.pos] = '\0';

    // Returns HTTP document
    return data;

error:
    // Clean memory
    if(data)
        free(data);
    if(curl)
        curl_easy_cleanup(curl);
    curl_global_cleanup();
    return NULL;
}

int main(int argc, char *argv[])
{
    char *http_document;
    char url[URL_SIZE];

    json_t *json_document;
    json_error_t error;

    if(argc != 2) {
        fprintf(stderr, "usage: %s <product id>\n\n", argv[0]);
        return 2;
    }

    // Build URL
    snprintf(url, URL_SIZE, URL_FORMAT, argv[1]);

    // Get JSON from API with CURL
    http_document = request(url);
    if(!http_document)
        return 1;

    json_document = json_loads(http_document, 0, &error);
    free(http_document);

    if(!json_document) {
        fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
        return 1;
    }

    // Open Food Facts returns a JSON object 
    if(!json_is_object(json_document)) {
        fprintf(stderr, "error: json_document is not an oject (JSON type error)\n");
        json_decref(json_document);
        return 1;
    }

    // We'll extract these value from json_document
    int product_status;
    json_t *product, *product_name, *product_image_url;

    // Check status property of the json_document
    json_unpack(json_document, "{s:i}", "status", &product_status);
    // 1 is found, 0 is not found
    if(product_status != 1) {
        fprintf(stderr, "error: product not found.\n");
        return 1;
    }

    product = json_object_get(json_document, "product");
    if(!json_is_object(product)) {
        fprintf(stderr, "eroor: product is not an object\n");
        return 1;
    }

    product_name = json_object_get(product, "product_name");
    if(!json_is_string(product_name)) {
        fprintf(stderr, "error: product.product_name is not a string\n");
        return 1;
    }

    product_image_url = json_object_get(product, "image_url");
    if(!json_is_string(product_image_url)) {
        fprintf(stderr, "error: product.product_image_url is not a string\n");
        return 1;
    }

    printf("Product name: %s\n", json_string_value(product_name));
    printf("Product image: %s\n", json_string_value(product_image_url));

    // Free json_document
    json_decref(json_document);
    return 0;
}
