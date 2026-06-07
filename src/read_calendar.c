#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>

char *read_file(const char *path) {
  FILE *f = fopen(path, "rb");
  if (!f)
    return NULL;

  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *buffer = malloc(size + 1);
  fread(buffer, 1, size, f);
  buffer[size] = '\0';
  fclose(f);
  return buffer;
}

void process_json(const char *path) {
  char *json_raw = read_file(path);
  if (!json_raw)
    return;

  cJSON *root = cJSON_Parse(json_raw);
  if (!root)
    goto cleanup;

  cJSON *calendar = cJSON_GetObjectItem(root, "calendar");
  cJSON *month = NULL;

  cJSON_ArrayForEach(month, calendar) {
    cJSON *name = cJSON_GetObjectItem(month, "month");
    printf("Month not found: %s\n", name->valuestring);
  }

  cJSON_Delete(root);
cleanup:
  free(json_raw);
}
