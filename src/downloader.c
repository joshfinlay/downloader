/*  downloader.c -- HTTP downloader code sample using cURL
    Copyright (C) 2015  Josh Finlay <josh@finlay.id.au>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "url_parser.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <libgen.h>
#include <curl/curl.h>
#include "progressbar.h"
#include <unistd.h>

progressbar *smooth;
int transferactive;
char *outfilename;
int lastpc;

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

char* readable_fs(double size, char *buf) {
    int i = 0;
    const char* units[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    while (size > 1024) {
        size /= 1024;
        i++;
    }
    sprintf(buf, "%.*f%s", i, size, units[i]);
    return buf;
}

char *get_file_from_url(struct parsed_url *purl) {
	char *ptr;
	char *search;
	const char sep[2] = "/";
	if (purl->path != NULL) {
		search = purl->path;
	}
	else {
		search = purl->query;
	}
	ptr = strtok(search, sep);

	if (ptr == NULL) { return (char *)"unknown"; }
	else {
		char *lastptr;
		while (ptr != NULL) {
			lastptr = ptr;
			ptr = strtok(NULL, sep);
		}
		return lastptr;
	}
}

double get_file_size(double sz) {
	if (sz < 1024) { return sz; }
	else if ((sz >= 1024) && (sz <= 1048576)) { return sz / 1024; }
	else if ((sz >= 1048576) && (sz <= 1073741824)) { return (sz / 1024) / 1024; }
	else { return (((sz / 1024) / 1024) / 1024); }
}

char *get_file_size_char(double sz) {
	char *outsz = "";
	char *suffix = "B";
	if (sz < 1024) { sprintf(outsz, "%0.0f%s", sz, suffix); }
	else if ((sz >= 1024) && (sz <= 1048576)) { suffix = "KB"; sprintf(outsz, "%3.2f%s", (sz / 1024), suffix); }
	else if ((sz >= 1048576) && (sz <= 1073741824)) { suffix = "MB"; sprintf(outsz, "%3.2f%s", ((sz / 1024) / 1024), suffix); }
	else { suffix = "GB"; sprintf(outsz, "%3.2f%s", (((sz / 1024) / 1024) / 1024), suffix); }
	return outsz;
}

int prog_callback(double totalsize, double position) {
	double prog_pc = ((double)position / totalsize) * 100;
	if (transferactive < 1) {
		smooth = progressbar_new_with_format(basename(outfilename), 100, "<.>");
		transferactive = 1;
	}
	int pos = (int)prog_pc;

	char title[10];
	if (pos <= 100) { snprintf(title, 10, "%3.2f%%", prog_pc); }
	else { snprintf(title, 10, "%3.2f%%", 100.00); }
	progressbar_update_label(smooth, title);

	if (pos > lastpc) {
		progressbar_inc(smooth);
		lastpc = pos;
	}
	return 0;
}


int main(int argc, const char *argv[])
{
	CURL *curl;
	FILE *fp;
	CURLcode res;
	struct parsed_url *purl;

	if (argc < 2) {
		printf("Usage: downloader <url>\n");
		return 1;
	}
	curl = curl_easy_init();
	if (curl) {
		transferactive = 0;
		lastpc = 0;
		printf("Downloading %s ...\n", argv[1]);
		curl_easy_setopt(curl, CURLOPT_URL, argv[1]);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, prog_callback);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "downloader/0.1");
		purl = parse_url(argv[1]);
		outfilename = get_file_from_url(purl);
		printf("Output: %s\n\n", outfilename);
		fp = fopen(outfilename, "wb");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		res = curl_easy_perform(curl);
		fclose(fp);
		if (res != CURLE_OK) {
			fprintf(stderr, "Download failed: %s\n", curl_easy_strerror(res));
			curl_easy_cleanup(curl);
			return 1;
		}
		else {
			progressbar_finish(smooth);
			CURLcode res;
			double val;
			res = curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD, &val);
			char buff[10];
			printf("\n========================================\n");
			printf("             DOWNLOAD SUMMARY           \n");
			if((CURLE_OK == res) && (val>0)) {
				printf("  Data downloaded: %s\n", readable_fs(val, buff));
			}
			res = curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &val);
			if((CURLE_OK == res) && (val>0)) {
				printf("  Total download time: %0.3f sec.\n", val);
			}
			res = curl_easy_getinfo(curl, CURLINFO_SPEED_DOWNLOAD, &val);
			if((CURLE_OK == res) && (val>0))
				printf("  Average download speed: %s/sec.\n", readable_fs(val, buff));
			printf("========================================\n");
			curl_easy_cleanup(curl);
			return 0;
		}
	}
	else {
		printf("Unable to initialize cURL!\n");
		return 1;
	}
	return 0;
}
