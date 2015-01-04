#define NO_SCORES 15
#define SCORE_LENGTH 15

struct score_list
{
  int score;
  int ship;
  int level;
  char name [25];
};

struct score_list hs_single [NO_SCORES];
struct score_list hs_coop [NO_SCORES];

struct score_list *hscore;
