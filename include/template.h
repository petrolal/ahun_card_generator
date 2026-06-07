#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "card.h"

typedef struct {
    char background_path[256];
    TextElement *elements;
    int element_count;
} Template;

void list_templates_xml(const char *dir_path);
Template* load_template(const char *xml_path);
void free_template(Template *tmpl);

#endif // TEMPLATE_H
