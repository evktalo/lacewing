void init_clouds(void);

int create_cloud(int type, int x, int y, int x2, int y2,
int x_speed, int y_speed, int timer,
int tickrate, int delta_tickrate, int angle,
int status, unsigned char seed, int colour [5]);

void simple_cloud(int colour, int x, int y, int xs, int ys, int size);
void simple_cloud_trans(int colour, int x, int y, int xs, int ys, int size);
void place_explosion(int x, int y, int xs, int ys, int size, int colours [5]);
void place_rocket_trail(int x, int y, int xs, int ys, int size, int colours [5]);
void place_explosion_with_smoke(int x, int y, int xs, int ys, int size, int colours [5]);
void place_speck_burst(int x, int y, int xs, int ys, int specks, int min_timeout, int random_timeout, int scatter, int colours [5]);
void place_line_burst(int x, int y, int xs, int ys, int specks, int min_timeout, int random_timeout, int burst, int random_burst, int colours [5]);
void place_burstlet_burst(int x, int y, int xs, int ys, int burstlets, int min_timeout, int random_timeout, int burst, int random_burst, int pretravel, int colours [5]);

void run_clouds(void);


