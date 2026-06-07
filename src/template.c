#include "template.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

Template* load_template(const char *xml_path) {
    xmlDocPtr doc;
    xmlNodePtr root, cur, layer_cur;
    Template *tmpl = calloc(1, sizeof(Template));
    
    doc = xmlReadFile(xml_path, NULL, 0);
    if (doc == NULL) {
        fprintf(stderr, "Error: Could not parse XML file %s\n", xml_path);
        free(tmpl);
        return NULL;
    }

    root = xmlDocGetRootElement(doc);
    if (root == NULL || strcmp((const char*)root->name, "template") != 0) {
        fprintf(stderr, "Error: Invalid XML root\n");
        xmlFreeDoc(doc);
        free(tmpl);
        return NULL;
    }

    // Count text layers first to allocate
    int count = 0;
    for (cur = root->children; cur; cur = cur->next) {
        if (cur->type == XML_ELEMENT_NODE && strcmp((const char*)cur->name, "text_layer") == 0) {
            count++;
        }
    }
    
    tmpl->elements = calloc(count, sizeof(TextElement));
    tmpl->element_count = count;

    int idx = 0;
    for (cur = root->children; cur; cur = cur->next) {
        if (cur->type == XML_ELEMENT_NODE) {
            if (strcmp((const char*)cur->name, "background") == 0) {
                xmlChar *bg = xmlNodeGetContent(cur);
                if (bg) {
                    memset(tmpl->background_path, 0, sizeof(tmpl->background_path));
                    strncpy(tmpl->background_path, (const char*)bg, sizeof(tmpl->background_path) - 1);
                    xmlFree(bg);
                }
            } else if (strcmp((const char*)cur->name, "text_layer") == 0) {
                for (layer_cur = cur->children; layer_cur; layer_cur = layer_cur->next) {
                    if (layer_cur->type == XML_ELEMENT_NODE) {
                        xmlChar *content = xmlNodeGetContent(layer_cur);
                        if (strcmp((const char*)layer_cur->name, "x") == 0) {
                            tmpl->elements[idx].x = atof((const char*)content);
                        } else if (strcmp((const char*)layer_cur->name, "y") == 0) {
                            tmpl->elements[idx].y = atof((const char*)content);
                        } else if (strcmp((const char*)layer_cur->name, "font_size") == 0) {
                            tmpl->elements[idx].font_size = atof((const char*)content);
                        }
                        xmlFree(content);
                    }
                }
                idx++;
            }
        }
    }

    xmlFreeDoc(doc);
    return tmpl;
}

void free_template(Template *tmpl) {
    if (tmpl) {
        if (tmpl->elements) free(tmpl->elements);
        free(tmpl);
    }
}
