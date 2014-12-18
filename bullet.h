int create_bullet(int type, int x, int y,
int x_speed, int y_speed, int owner,
int damage, int timer, int mass, int angle,
int status, unsigned char seed, int colours [4], int speed_div,
int special1, int special2, int special3, int special4, int special5);

void init_bullets(void);
void run_bullets(void);

void blast(int x, int y, int radius, int damage, int force, int owner);

int closest_target(int x, int y);
int nearby_target(int range, int x, int y);

