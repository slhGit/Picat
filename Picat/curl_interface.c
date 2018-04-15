#include "curl_iterface.h"
#include "picat.h"

#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>

CURL *curl_handle;

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
	return written;
}

curl_init() {
	curl_global_init(CURL_GLOBAL_ALL);

	curl_handle = curl_easy_init();

	static const char *pagefilename = "page.out";
	FILE *pagefile;

	/* set URL to get here */
	curl_easy_setopt(curl_handle, CURLOPT_URL, "www.picat-lang.org");

	/* Switch on full protocol/debug output while testing */
	curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);

	/* disable progress meter, set to 0L to enable and disable debug output */
	curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

	/* send all data to this function  */


	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);

	/* open the file */
	pagefile = fopen(pagefilename, "wb");
	if (pagefile) {
		/* write the page body to this file handle */
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);

		/* get it! */
		curl_easy_perform(curl_handle);

		/* close the header file */
		fclose(pagefile);
	}

	/* cleanup curl stuff */
	curl_easy_cleanup(curl_handle);

	curl_global_cleanup();


	return PICAT_TRUE;
}


curl_cpreds() {
	insert_cpred("curl_init", 0, curl_init);
}