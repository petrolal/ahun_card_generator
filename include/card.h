#ifndef CARD_H
#define CARD_H

typedef struct {
  const char *text;
  double x;
  double y;
  double font_size;
} TextElement;

typedef struct {
  const char *template_path;
  const char *font_path;
  const char *output_path;
  TextElement *elements;
  int element_count;
} CardConfig;

int generate_card(CardConfig *config);

#endif // CARD_H
