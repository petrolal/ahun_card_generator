#ifndef READ_CALENDAR_H
#define READ_CALENDAR_H

typedef struct {
  char theme[100];
  char date[12];
} Gira;

typedef struct {
  char name_month[20];
  Gira *gira_list;
  int gira_qtd;
} MonthCalendar;

char *read_file(const char *path);

void process_json(const char *path);

#endif // !READ_CALENDAR_H
