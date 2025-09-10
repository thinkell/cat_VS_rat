#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <ncurses.h>



struct player {
    int x, y;
    int last_x, last_y;
    char role;
    int freeze_steps;
};


struct cheese {
    int x;
    int y;
    double distance_to_rat;
    double distance_to_cat;
    bool activ;
};


void print_map(char **map, int rows, int cols)
{
    for (int y = 0; y < rows-1; y++)
        for (int x = 0; x < cols; x++)
            mvaddch(y, x, map[y][x]);
}


void drawing_map(char **map, int rows, int cols)
{
    //границы
    for (int y = 0; y < rows-1; y++)
        for (int x = 0; x < cols; x++)
            map[y][x] = '%';
      
    
    //cтены
    for (int y = 1; y < rows-2; y++)
        for (int x = 1; x < cols-1; x++)
            map[y][x] = '#';
            
        
    //пустые места
    int  r_x = (rand() % (cols / 3)) + 1; //[1; (cols / 3)]
    int r_y = (rand() % (rows / 3)) + 1; //[1; (rows / 3)]
    
    for (int y = r_y; y < rows-r_y-1; y++)
        for (int x = r_x; x < cols-r_x; x++)
            map[y][x] = ' ';
}


void clear_display(char **map, int rows, int cols)
{
    for (int y = 0; y < rows; y++)
        for (int x = 0; x < cols; x++) {
            mvaddch(y, x, ' ');
            map[y][x] = ' ';
        }
}


void move_me(char **map, struct player *my_player, const int *my_key_input, int *my_cheese)
{
    my_player->last_x = my_player->x, my_player->last_y = my_player->y;
    if (*my_key_input == KEY_UP)
        my_player->y--;
    else if (*my_key_input == KEY_DOWN)
        my_player->y++;
    else if (*my_key_input == KEY_LEFT)
        my_player->x--;
    else if (*my_key_input == KEY_RIGHT)
        my_player->x++;
    
 
    //если шаг на стену/границу - возвращение персонажа на исходную позицию
    if (map[my_player->y][my_player->x] == '%' || (my_player->role == 'c' &&
     map[my_player->y][my_player->x] == '#')) {
        my_player->x = my_player->last_x;
        my_player->y = my_player->last_y;    
    } //только крыса может проедать стены(но не границы)
    
    //съесть сыр
    if (map[my_player->y][my_player->x] == '*')
        (*my_cheese)++;
    
    mvaddch(my_player->last_y, my_player->last_x, ' '); //clear last position
    map[my_player->last_y][my_player->last_x] = ' ';
    mvaddch(my_player->y, my_player->x, my_player->role); //print new position
    map[my_player->y][my_player->x] = my_player->role;
}


void set_enemy_coords_for_move(char **map, struct player *enemy_person, int px, int py, int role)
{
    /*role:
    1 - hunt (for cat)
    2 - run away (for rat)
    3 - go to cheese (for rat)*/
    int k_route = (role == 2) ? -1 : 1;
    
    bool up_not_board, down_not_board, left_not_board, right_not_board;
    if (role == 1) {
        up_not_board = (map[(enemy_person->y)-1][enemy_person->x] == ' ' ||
         map[(enemy_person->y)-1][enemy_person->x] == 'r') ? true : false;
        down_not_board = (map[(enemy_person->y)+1][enemy_person->x] == ' ' ||
         map[(enemy_person->y)+1][enemy_person->x] == 'r') ? true : false;
        left_not_board = (map[enemy_person->y][(enemy_person->x)-1] == ' ' ||
         map[enemy_person->y][(enemy_person->x)-1] == 'r') ? true : false;
        right_not_board = (map[enemy_person->y][(enemy_person->x)+1] == ' ' ||
         map[enemy_person->y][(enemy_person->x)+1] == 'r') ? true : false;
    }
    else if (role == 2 || role == 3) {
        up_not_board = (map[(enemy_person->y)-k_route][enemy_person->x] != '%' &&
            map[(enemy_person->y)-k_route][enemy_person->x] != 'r') ? true : false;
        down_not_board = (map[(enemy_person->y)+k_route][enemy_person->x] != '%' &&
            map[(enemy_person->y)+k_route][enemy_person->x] != 'r') ? true : false;
        left_not_board = (map[enemy_person->y][(enemy_person->x)-k_route] != '%' &&
            map[enemy_person->y][(enemy_person->x)-k_route] != 'r') ? true : false;
        right_not_board = (map[enemy_person->y][(enemy_person->x)+k_route] != '%' &&
            map[enemy_person->y][(enemy_person->x)+k_route] != 'r') ? true : false;
    }
    
    
    
    double up_distance_to_rat = hypot(px-(enemy_person->x), py-((enemy_person->y)-1));
    double down_distance_to_rat = hypot(px-(enemy_person->x), py-((enemy_person->y)+1));
    double left_distance_to_rat = hypot(px-((enemy_person->x)-1), py-(enemy_person->y));
    double right_distance_to_rat = hypot(px-((enemy_person->x)+1), py-(enemy_person->y));

    //усл. вверх
    bool up_less_down = (down_not_board == true) ?
     (up_distance_to_rat <= down_distance_to_rat) : true;
    bool up_less_left = (left_not_board == true) ? 
     (up_distance_to_rat <= left_distance_to_rat) : true;
    bool up_less_right = (right_not_board == true) ?
     (up_distance_to_rat <= right_distance_to_rat) : true;
    //усл. вниз
    bool down_less_up = (up_not_board == true) ?
     (down_distance_to_rat <= up_distance_to_rat) : true;
    bool down_less_left = (left_not_board == true) ?
     (down_distance_to_rat <= left_distance_to_rat) : true;
    bool down_less_right = (right_not_board == true) ?
     (down_distance_to_rat <= right_distance_to_rat) : true;
    //усл. влево
    bool left_less_up = (up_not_board == true) ?
     (left_distance_to_rat <= up_distance_to_rat) : true;
    bool left_less_down = (down_not_board == true) ?
     (left_distance_to_rat <= down_distance_to_rat) : true;
    bool left_less_right = (right_not_board == true) ?
     (left_distance_to_rat <= right_distance_to_rat) : true;
    //усл.вправо
    bool right_less_up = (up_not_board == true) ?
     (right_distance_to_rat <= up_distance_to_rat) : true;
    bool right_less_down = (down_not_board == true) ?
     (right_distance_to_rat <= down_distance_to_rat) : true;
    bool right_less_left = (right_not_board == true) ?
     (right_distance_to_rat <= left_distance_to_rat) : true;



    //вверх
    if ((up_not_board == true) && (up_less_down && up_less_left && up_less_right))
        enemy_person->y -= 1*k_route;
    //вниз
    else if ((down_not_board == true) && (down_less_up && down_less_left && down_less_right))
        enemy_person->y += 1*k_route;
    //влево
    else if ((left_not_board == true) && (left_less_up && left_less_down && left_less_right))
        enemy_person->x -= 1*k_route;
    //вправо
    else if ((right_not_board == true) && (right_less_up && right_less_down && right_less_left))
        enemy_person->x += 1*k_route;
}


void move_enemy(char **map, struct player *enemy_person,
struct cheese *cheese_list, int *eated_cheeses, int px, int py)
{
    enemy_person->last_x = enemy_person->x, enemy_person->last_y = enemy_person->y;
    
    
    // для кота
    if (enemy_person->role == 'c') {
        set_enemy_coords_for_move(map, enemy_person, px, py, 1);
    }
    //для крысы
    else if (enemy_person->role == 'r') {
        //определение расстояния до сыра
        for (int i = 0; i < 5; i++) {
            int ch_x = cheese_list[i].x;
            int ch_y = cheese_list[i].y;
            cheese_list[i].distance_to_rat = hypot(ch_x-enemy_person->x, ch_y-enemy_person->y);
            cheese_list[i].distance_to_cat = hypot(ch_x-px, ch_y-py);
        }
        //сыр с минимальным расстоянием до крысы
        //сортировка массива по возрастанию
        for (int i = 0; i < 5; i++) {
            int min_id = i;
            for (int j = i+1; j < 5; j++)
                if (((cheese_list[j].distance_to_rat < cheese_list[i].distance_to_rat) &&
                cheese_list[j].activ == true) || 
                (cheese_list[j].activ == true && cheese_list[i].activ == false))
                    min_id = j;
            struct cheese temp = cheese_list[i];
            cheese_list[i] = cheese_list[min_id];
            cheese_list[min_id] = temp;       
        }
        struct cheese near_cheese = cheese_list[0];
        
        
        
       //определение куда идти: к сыру или от кота - в зависимости от расстояния
       //идти к сыру
       if (near_cheese.distance_to_rat <= near_cheese.distance_to_cat) {
            set_enemy_coords_for_move(map, enemy_person, near_cheese.x, near_cheese.y, 3);
       }
       //уходить от кота
       else {
            set_enemy_coords_for_move(map, enemy_person, px, py, 2);
       }
       
       //съесть сыр
       if (map[enemy_person->y][enemy_person->x] == '*') {
            cheese_list[0].activ = false;
            enemy_person->freeze_steps = (rand() % 3) + 1; //[1; 3]
            (*eated_cheeses)++;
       }
    }
    
    mvaddch(enemy_person->last_y, enemy_person->last_x, ' '); //clear last position
    map[enemy_person->last_y][enemy_person->last_x] = ' ';
    
    mvaddch(enemy_person->y, enemy_person->x, enemy_person->role); //print new position
    map[enemy_person->y][enemy_person->x] = enemy_person->role;
}


bool fight_if_collision(char **map, int px, int py, int ex, int ey,
int rows, int cols, char my_role)
{
    bool collision = (px == ex) && (py == ey);
    if (collision) {
        const char *end_text = (my_role == 'c') ? "win" : "lose";
        clear_display(map, rows, cols);
        mvprintw(rows/2, cols/2, "You %s\n", end_text);
        getch();
    }
    
    return collision;
}


void print_output_panel(int lvl, int max_lvl, int balls, int cheeses_to_finish_lvl,
int rows, int cols)
{
    mvprintw(rows-1, 1, "lvl: %d/%d                         %d/%d",
     lvl, max_lvl, balls, cheeses_to_finish_lvl);

}


void get_random_xy_in_void_place(char **map, int rows, int cols, int *x, int *y)
{
    int count = 0;
    do {
            *x = (rand() % (cols - 2)) + 1; //[1; (cols - 2)]
            *y = (rand() % (rows - 2)) + 1; //[1; (rows - 2)]
    } while (map[*y][*x] != ' ');
}


void release_map(char **map, int rows, int cols, struct player *my_player,
struct player *enemy_list, struct cheese *cheese_list,
int enemy_number, int cheeses_to_finish_lvl)
{
    char enemy_role = ((my_player->role == 'c') ? 'r' : 'c');
    for (int i = 0; i < enemy_number; i++)
        enemy_list[i].role = enemy_role;
    
    //карта
    clear_display(map, rows, cols);
    drawing_map(map, rows, cols);
    //spawn cheese
    for (int i = 0; i < 5; i++)//!!
    {
        int r_x, r_y;
        get_random_xy_in_void_place(map, rows, cols, &r_x, &r_y);
        cheese_list[i].activ = true;
        cheese_list[i].x = r_x;
        cheese_list[i].y = r_y;
        map[cheese_list[i].y][cheese_list[i].x] = '*';
    }
    
    print_map(map, rows, cols);
    
    //спавн    
    do {
    
         my_player->x = (rand() % (cols / 3)) + 1; //[1; (cols / 3)]
         my_player->y = (rand() % (rows / 3)) + 1; //[1; (rows / 3)]
         //my_player->x = rand() % cols;
         //my_player->y = rand() % rows;
    } while (map[my_player->y][my_player->x] != ' ');
    map[my_player->y][my_player->x] = my_player->role;
    
    
    for (int i = 0; i < enemy_number; i++) {
        do {
            enemy_list[i].x = (rand() % (cols / 3)) + 1; //[1; (cols / 3)]
            enemy_list[i].y = (rand() % (rows / 3)) + 1; //[1; (rows / 3)]
            //enemy_list[0].x = rand() % cols;
            //enemy_list[0].y = rand() % rows;
        } while (map[enemy_list[i].y][enemy_list[i].x] != ' ');
    }
    
    //изначально нулевая заморозка
    my_player->freeze_steps = 0;
    for (int i = 0; i < enemy_number; i++)
        enemy_list[i].freeze_steps = 0;
}




int main()
{
    //init
    initscr();
    //set
    keypad(stdscr, 1);
    noecho();
    curs_set(0);
    //seed
    srand(time(NULL));
    
    
    //переменные моего персонажа
    int c = 0;
    struct player my_player;
    //cтатистика
    int my_lvl = 1;
    int max_lvl = 10;
    int eated_cheeses = 0;
    int cheeses_to_finish_lvl = 5;
    //противники
    int enemy_number = 1;
    struct player enemy_list[10];//!!
    //сыр
    struct cheese cheese_list[5];
    
    
    
    //масштаб карты
    int cols, rows;
    getmaxyx(stdscr, rows, cols); //для кроссплатф.
    
    //карта(база данных) - динамический двумерный массив
    char **map = (char**)malloc(rows * sizeof(int*));
    if (map == NULL) {
        return 0;
    }
    for (int i = 0; i < rows; i++) {
        map[i] = (char*)malloc(cols * sizeof(int));
        if (map == NULL) {
            return 0;
        }
    }
    
    
    
    //выбор персонажа: кот или крыса
    mvprintw(rows/2, cols/2, "Press key\n'1' - cat\n'2' - rat\n");
    
    c = getch();
    
    my_player.role = ((c == '1') ? 'c' : 'r');///*
    
    //назначение роли противникам(которая противоположна роли игрока)
    char enemy_role = ((my_player.role == 'c') ? 'r' : 'c');
    for (int i = 0; i < enemy_number; i++)
        enemy_list[i].role = enemy_role;
    
    
    //создание карты
    release_map(map, rows, cols, &my_player,
            enemy_list, cheese_list, enemy_number, cheeses_to_finish_lvl);
    
    
    
    //точка "следующий уровень" - >
    int x_lvl_point, y_lvl_point;
    get_random_xy_in_void_place(map, rows, cols, &x_lvl_point, &y_lvl_point);
    
    
    
    
    
    //Главный цикл
    do {
        //переход на следующий уровень
        if (
         my_player.x == x_lvl_point && my_player.y == y_lvl_point
         || eated_cheeses == cheeses_to_finish_lvl) {
            my_lvl++;
            enemy_number++;
            eated_cheeses = 0;
            release_map(map, rows, cols, &my_player,
            enemy_list, cheese_list, enemy_number, cheeses_to_finish_lvl);
            get_random_xy_in_void_place(map, rows, cols, &x_lvl_point, &y_lvl_point);            
         }
         
        //прорисовка точки "следующий уровень" - >
        mvaddch(y_lvl_point, x_lvl_point, '>');
        
        
        //передвинуть меня
        bool out_cycle1 = false;
        move_me(map, &my_player, &c, &eated_cheeses);
        for (int i = 0; i < enemy_number; i++) {
            if ((out_cycle1 = fight_if_collision(map, my_player.x, my_player.y, enemy_list[i].x, enemy_list[i].y,
            rows, cols, my_player.role)) == true)
                break;
        }
        if (out_cycle1 == true)
            break;
        
         
        
        //передвинуть противников
        bool out_cycle2 = false;
        for (int i = 0; i < enemy_number; i++) {
            //если заморозка
            if (enemy_list[i].freeze_steps > 0) {
                enemy_list[i].freeze_steps--;
                continue;
            }
            move_enemy(map, &enemy_list[i], cheese_list, &eated_cheeses, my_player.x, my_player.y);
            if ((out_cycle2 = fight_if_collision(map, my_player.x, my_player.y, enemy_list[i].x, enemy_list[i].y,
            rows, cols, my_player.role)) == true)
                break;
        }
        if (out_cycle2 == true)
            break;
            
        //вывод статистики
        print_output_panel(my_lvl, max_lvl, eated_cheeses, cheeses_to_finish_lvl, rows, cols);
        
        
        
    } while ((c = getch()) && c != 'q' && c != 27); //27 - ESC
    
        
        
        
        
    for (int i = 0; i < rows; i++) 
        free(map[i]);
    free(map);
    endwin();
    return 0;
}