#ifndef __HTTP_H__
#define __HTTP_H__

#include <stddef.h>

/**
 * HTTP client wrapper for NextUI
 * 
 * Uses curl subprocess for HTTP requests. Supports both synchronous
 * and asynchronous (threaded) requests.
 */

// Maximum response size (8MB)
#define HTTP_MAX_RESPONSE_SIZE (8 * 1024 * 1024)

// HTTP timeout in seconds
#define HTTP_TIMEOUT_SECS 30

// HTTP response structure
typedef struct HTTP_Response {
	char* data;           // Response body (caller must free)
	size_t size;          // Response body size
	int http_status;      // HTTP status code (200, 404, etc.) or -1 on error
	char* error;          // Error message if failed (caller must free), NULL on success
} HTTP_Response;

/**
 * Callback for async HTTP requests.
 * @param response The HTTP response (caller takes ownership, must call HTTP_freeResponse)
 * @param userdata User-provided data passed to the async request
 */
typedef void (*HTTP_Callback)(HTTP_Response* response, void* userdata);

/**
 * Perform a synchronous HTTP GET request.
 * @param url The URL to fetch
 * @return HTTP_Response (caller must call HTTP_freeResponse)
 */
HTTP_Response* HTTP_get(const char* url);

/**
 * Perform a synchronous HTTP POST request.
 * @param url The URL to post to
 * @param post_data The POST body data (can be NULL for empty POST)
 * @param content_type The Content-Type header (can be NULL for application/x-www-form-urlencoded)
 * @return HTTP_Response (caller must call HTTP_freeResponse)
 */
HTTP_Response* HTTP_post(const char* url, const char* post_data, const char* content_type);

/**
 * Perform an asynchronous HTTP GET request.
 * Spawns a background thread and calls callback when complete.
 * @param url The URL to fetch
 * @param callback Function to call with the response
 * @param userdata User data to pass to callback
 */
void HTTP_getAsync(const char* url, HTTP_Callback callback, void* userdata);

/**
 * Perform an asynchronous HTTP POST request.
 * Spawns a background thread and calls callback when complete.
 * @param url The URL to post to
 * @param post_data The POST body data (can be NULL for empty POST)
 * @param content_type The Content-Type header (can be NULL)
 * @param callback Function to call with the response
 * @param userdata User data to pass to callback
 */
void HTTP_postAsync(const char* url, const char* post_data, const char* content_type,
                    HTTP_Callback callback, void* userdata);

/**
 * Free an HTTP response structure.
 * @param response The response to free
 */
void HTTP_freeResponse(HTTP_Response* response);

/**
 * URL-encode a string for use in query parameters.
 * @param str The string to encode
 * @return Encoded string (caller must free), or NULL on error
 */
char* HTTP_urlEncode(const char* str);

/**
 * Build a User-Agent string for RetroAchievements per the spec
 * (see Hardcore Compliance §C). Format:
 *   NextUI/v<NEXTUI_VERSION> (<OS> <kernel>; <platform>) [<core>/<core_ver>]
 * The core suffix is appended only when HTTP_setCoreInfo() has been called
 * with a non-empty core name and version.
 * @param buffer Buffer to write User-Agent string to
 * @param buffer_size Size of buffer (256 recommended)
 */
void HTTP_getUserAgent(char* buffer, size_t buffer_size);

/**
 * Register the loaded core's name and version so HTTP_getUserAgent() can
 * include the optional core clause on subsequent requests. Pass NULL or
 * empty strings to clear (e.g., on core unload). Safe to call from the
 * main thread; reads from HTTP worker threads tolerate the race.
 * @param name Core library name (e.g. "Snes9x"); NULL/"" to clear
 * @param version Core library version (e.g. "1.62.3"); NULL/"" to clear
 */
void HTTP_setCoreInfo(const char* name, const char* version);

#endif // __HTTP_H__
