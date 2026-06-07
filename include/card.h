#ifndef CARD_H
#define CARD_H

typedef struct {
  char id[64];
  const char *text;
  double x;
  double y;
  double font_size;
  char color[32];
  char font_path[256];
} TextElement;

typedef struct {
  const char *template_path;
  const char *font_path;
  const char *output_path;
  TextElement *elements;
  int element_count;
} CardConfig;

int generate_card(CardConfig *config);
void init_card_engine(void);
void cleanup_card_engine(void);

#endif // CARD_H
