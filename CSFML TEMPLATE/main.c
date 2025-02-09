#include <SFML/Graphics.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define PI 3.1415926535

//necesary for time
float delta;

//holds the differents caracteristics of the player
struct j {
    sfSprite* sprite;
    sfTexture* texture;
    sfVector2f pos;
    sfVector2f fwd;
    sfVector2f force;
    float angle;
    float vitesse;
    float vitesseMax;
    float angleMax;
    float angleMin;
    int life;
    int score;
    sfVector2f fwdMax;
    sfVector2f fwdMin;
};

//holds the caracteristics used for the bullets
struct obj {
    sfSprite* sprite;
    sfTexture* texture;
    sfVector2f dir;
    sfVector2f pos;
    sfVector2f force;
    int visible;
    float vitesse;
    float angle;
};

//tentacules use this struct
struct ennemis {
    sfSprite* sprite;
    sfTexture* texture;
    sfVector2f dir;
    sfVector2f pos;
    int exist;
    float vitesse;
    float angle;
    float norme;
    float toJ;
};

//the boss uses this struct
struct big_thing {
    sfSprite* sprite;
    sfTexture* texture;
    sfVector2f dir;
    sfVector2f pos;
    int ugly;
    float vitesse;
    float angle;
    float norme;
    int hp;
};

//this struct is used for the title screen
struct lettre {
    sfText* text;
    sfVector2f pos;
};


//time
int Delta(sfClock* deltaclock) {
    sfTime dtime = sfClock_getElapsedTime(deltaclock);
    delta = sfTime_asMicroseconds(dtime);
    sfClock_restart(deltaclock);
    return 0;
};

float normalize(float x,float y) {
    float rslt = sqrt(x * x + y * y);
    return rslt;

}

//return the time elapsed since the restart of the clock, used for making event between that last a certain
//period of time
int timer(sfClock* delai) {
    sfTime  dt = sfClock_getElapsedTime(delai);
    float lastMouv = sfTime_asMilliseconds(dt);
    return lastMouv;

};

//manage all the calculation concerning the player
void gestionJoueur(struct j* joueur, float delta) {
    float a_length;
    float normalized_x;
    float normalized_y;
    joueur->pos.x += joueur->force.x * delta / 10000;
    joueur->pos.y += joueur->force.y * delta / 10000;

    a_length = sqrt(joueur->force.x * joueur->force.x + joueur->force.y * joueur->force.y);
    if (a_length > 0) {
        normalized_x = joueur->force.x / a_length;
        normalized_y = joueur->force.y / a_length;
        joueur->force.x -= 0.1 * normalized_x * delta / 10000;
        joueur->force.y -= 0.1 * normalized_y * delta / 10000;
    }
    if (joueur->angle <=-360 || joueur->angle >= 360) {
        joueur->angle = 0;
    }
    if (joueur->angleMax <= -360 || joueur->angleMax >= 360) {
        joueur->angleMax = 0;
    }
    if (joueur->angleMin <= -360 || joueur->angleMin >= 360) {
        joueur->angleMin = 0;
    }

    
}

//manage the creation, the movement and calculation needed for the bullets
void gestionBullet(struct obj *bullet,struct j joueur,int option) {
    for (int i = 0; i < 10; i++) {
        if (option) {
            if (bullet[i].visible) {
                bullet[i].force.x += bullet[i].vitesse * bullet[i].dir.x * delta / 10000;
                bullet[i].force.y += bullet[i].vitesse * bullet[i].dir.y * delta / 10000;
                bullet[i].pos.x += bullet[i].force.x * delta / 10000;
                bullet[i].pos.y += bullet[i].force.y * delta / 10000;
                sfSprite_setPosition(bullet[i].sprite, (sfVector2f) { bullet[i].pos.x, bullet[i].pos.y });
                if (bullet[i].pos.x < 0 || bullet[i].pos.x>500 || bullet[i].pos.y < 0 || bullet[i].pos.y>500) {
                    bullet[i].pos.x = joueur.pos.x;
                    bullet[i].pos.y = joueur.pos.y;
                    bullet[i].force.x = 0;
                    bullet[i].force.y = 0;
                    bullet[i].visible = 0;
                }
            }
        }
        else {
            if (!bullet[i].visible) {
                bullet[i].pos.x = joueur.pos.x;
                bullet[i].pos.y = joueur.pos.y;
                bullet[i].dir.x = cosf(joueur.angle * PI / 180);
                bullet[i].dir.y = sinf(joueur.angle * PI / 180);
                bullet[i].visible = 1;
                bullet[i].angle = joueur.angle;
                sfSprite_setRotation(bullet[i].sprite, bullet[i].angle + 90);
                break;
            }
        }
    }
}

//manage the creation, destruction, movement and collision of the tentacles
int gestionEnnemis(struct ennemis *tentacle, struct j *joueur, struct obj* bullet, int option,int *lostLife, sfText* affScore,struct big_thing boss)
{
    float ecart;
    char score[20];
    int nbNewEnnemis=0;
    for (int i = 0; i < 20; i++) {
        if (!option) {
            if (tentacle[i].exist) {
                tentacle[i].dir.x = joueur->pos.x - tentacle[i].pos.x;
                tentacle[i].dir.y = joueur->pos.y - tentacle[i].pos.y;
                tentacle[i].norme = sqrt(tentacle[i].dir.x * tentacle[i].dir.x + tentacle[i].dir.y * tentacle[i].dir.y);
                if (tentacle[i].norme > 0) {
                    tentacle[i].dir.x /= tentacle[i].norme;
                    tentacle[i].dir.y /= tentacle[i].norme;
                    tentacle[i].pos.x += tentacle[i].dir.x * tentacle[i].vitesse;
                    tentacle[i].pos.y += tentacle[i].dir.y * tentacle[i].vitesse;
                }
                float deltaX = joueur->pos.x- tentacle[i].pos.x;
                float deltaY = joueur->pos.y- tentacle[i].pos.y;
                float angle = atan2f(deltaY, deltaX);
                angle = angle * (180.0f / PI);
                sfSprite_setPosition(tentacle[i].sprite, (sfVector2f) { tentacle[i].pos.x, tentacle[i].pos.y });
                sfSprite_setRotation(tentacle[i].sprite, angle+90);
                ecart = normalize(tentacle[i].pos.x - joueur->pos.x, tentacle[i].pos.y - joueur->pos.y);
                if (ecart < 50) {
                    tentacle[i].exist = 0;
                    joueur->life -= 1;
                    *lostLife = 1;
                }
                for (int j = 0; j < 10; j++) {
                    if (bullet[j].visible) {
                        ecart = normalize(tentacle[i].pos.x - bullet[j].pos.x, tentacle[i].pos.y - bullet[j].pos.y);
                        if (ecart < 25) {
                            tentacle[i].exist = 0;
                            bullet[j].visible = 0;
                            joueur->score += 1;
                            snprintf(score,20,"Score : %d", joueur->score);
                            sfText_setString(affScore, score);
                        }
                    }
                }
            }
        }
        if (option==1) {
            
            if (!tentacle[i].exist) {
                float x = rand() % 1600 - 300;
                float y = rand() % 1600 - 300;
                float deltaX = joueur->pos.x - x;
                float deltaY = joueur->pos.y - y;
                float angle = atan2f(deltaY, deltaX);
                angle = fabs(angle * (180.0f / PI));
                for (int j = 0; j < 200; j++) {
                    if ((angle < fabs(joueur->angleMax) && angle > fabs(joueur->angleMin)) || fabs(deltaX) < 300 || fabs(deltaY) < 300) {
                        x = rand() % 800 - 150;
                        y = rand() % 800 - 150;
                        deltaX = joueur->pos.x - x;
                        deltaY = joueur->pos.y - y;
                        angle = atan2f(deltaY, deltaX);
                        angle = fabs(angle * (180.0f / PI));
                    }
                    else {
                        break;
                    }
                }
                tentacle[i].pos.x = x;
                tentacle[i].pos.y = y;
                tentacle[i].exist = 1;
                break;
            }
        }
        //used when the boss dies, spawn 5 tentacles
        if (option == 2) {
            for (int i = 0; i < 10; i++) {
                bullet[i].visible = 0;
            }
            if (nbNewEnnemis<5){
                if (!tentacle[i].exist) {
                    float x =(rand() % 50 )+ (int)boss.pos.x - 100;
                    float y =(rand() % 50 )+ (int)boss.pos.y - 100;
                    float deltaX = joueur->pos.x - x;
                    float deltaY = joueur->pos.y - y;
                    float angle = atan2f(deltaY, deltaX);
                    angle = fabs(angle * (180.0f / PI));
                    for (int k = 0; k < 200; k++ ) {
                        if ((angle < fabs(joueur->angleMax) && angle > fabs(joueur->angleMin)) || fabs(deltaX) < 300 || fabs(deltaY) < 300) {
                            x = (rand() % 50) + (int)boss.pos.x - 100;
                            y = (rand() % 50) + (int)boss.pos.y - 100;
                            deltaX = joueur->pos.x - x;
                            deltaY = joueur->pos.y - y;
                            angle = atan2f(deltaY, deltaX);
                            angle = fabs(angle * (180.0f / PI));
                        }
                    }
                    tentacle[i].pos.x = x;
                    tentacle[i].pos.y = y;
                    tentacle[i].exist = 1;
                    nbNewEnnemis += 1;
                }
            }
            else {
                break;
            }

        }
    }
}

//manage the boss's movements, collisions and his spawn
int gestionBoss(struct big_thing *boss, struct j* joueur, struct obj *bullet, int option, int* lostLife, sfText* affScore)
{
    float ecart;
    char score[20];
    if (!option) {
        boss->dir.x = joueur->pos.x - boss->pos.x;
        boss->dir.y = joueur->pos.y - boss->pos.y;
        boss->norme = sqrt(boss->dir.x * boss->dir.x + boss->dir.y * boss->dir.y);
        if (boss->norme > 0) {
            boss->dir.x /= boss->norme;
            boss->dir.y /= boss->norme;
            boss->pos.x += boss->dir.x * boss->vitesse;
            boss->pos.y += boss->dir.y * boss->vitesse;
        }
        sfSprite_setPosition(boss->sprite, (sfVector2f) { boss->pos.x, boss->pos.y });
        ecart = normalize(boss->pos.x - joueur->pos.x, boss->pos.y - joueur->pos.y);
        if (ecart < 75) {
            boss->ugly = 0;
            joueur->life = 0;
            *lostLife = 1;
        }
        for (int j = 0; j < 10; j++) {
            if (bullet[j].visible) {
                ecart = normalize(boss->pos.x - bullet[j].pos.x, boss->pos.y - bullet[j].pos.y);
                if (ecart < 75) {
                    boss->hp -= 1;
                    bullet[j].visible = 0;
                }
            }
        }
        
        
    }
    else {
        float x = rand() % 800 -200;
        float y = rand() % 800 -200;
        float deltaX = joueur->pos.x - x;
        float deltaY = joueur->pos.y - y;
        float angle = atan2f(deltaY, deltaX);
        angle = fabs(angle * (180.0f / PI));
        for (int i = 0; i < 200; i++) {
            if ((angle < fabs(joueur->angleMax) && angle > fabs(joueur->angleMin)) || fabs(deltaX) < 400 || fabs(deltaY) < 400) {
                x = rand() % 800 - 200;
                y = rand() % 800 - 200;
                deltaX = joueur->pos.x - x;
                deltaY = joueur->pos.y - y;
                angle = atan2f(deltaY, deltaX);
                angle = fabs(angle * (180.0f / PI));
            }
            else {
                break;
            }
        }
        boss->pos.x = x;
        boss->pos.y = y;
        boss->ugly = 1;
        boss->hp = 20;
    }
}

int main() {

    srand(time(NULL));

    sfVideoMode mode = { 500, 500, 32 };
    sfRenderWindow* window = sfRenderWindow_create(mode, "Xenomanus", sfResize | sfClose, NULL);
    sfTexture* textureL;
    sfSprite* spriteL;
    sfTexture* textureTest;
    sfSprite* test;
    sfSprite* test1;
    sfSprite* test2;
    sfSprite* arbre;
    sfSprite* arbre1;
    sfSprite* arbre2;
    sfClock* deltaclock = sfClock_create();
    sfClock* Event = sfClock_create();
    sfClock* eventLostLife = sfClock_create();
    sfTexture* textureF;
    sfSprite* spriteF;
    sfTexture* textureArrow;
    sfTexture* textureArbre;
    sfSprite* arrow;
    sfText* launch;
    sfText* quit;
    sfText* life;
    sfText* gameOver;
    sfText* affScore;
    sfText* diff;
    sfFont* font;
    sfFont* oscour;

    int easterEgg = rand() % 7;
    char txtLife[3][80] = {
        "You have been hit by something.",
        "              You're bleeding !",
        "As you fall, you passed away,\nSoon to be catch by the creature."

    };
    oscour = sfFont_createFromFile("Font/Wingding Review.ttf");
    font = sfFont_createFromFile("Font/arial.ttf");
    life = sfText_create();

    sfText_setFont(life, font);
    sfText_setOrigin(life, (sfVector2f) { 10, 10 });
    sfText_setCharacterSize(life, 20);
    sfText_setPosition(life, (sfVector2f) { 100, 350 });
    
    sfText_setColor(life, (sfColor) { 255, 0, 0, 255 });

    launch = sfText_create();
    sfText_setFont(launch, font);
    sfText_setOrigin(launch, (sfVector2f) { 0, 10 });
    sfText_setCharacterSize(launch, 20);
    sfText_setPosition(launch, (sfVector2f) { 220, 350 });
    sfText_setColor(launch, (sfColor) { 255, 0, 0, 255 });
    sfText_setString(launch, "Run");

    diff = sfText_create();
    sfText_setFont(diff, font);
    sfText_setOrigin(diff, (sfVector2f) { 0, 10 });
    sfText_setCharacterSize(diff, 20);
    sfText_setPosition(diff, (sfVector2f) { 320, 350 });
    sfText_setColor(diff, (sfColor) { 255, 0, 0, 255 });
    sfText_setString(diff, "I'm too weak !\nI'm in danger.\nPanik !");

    quit = sfText_create();
    sfText_setFont(quit, font);
    sfText_setOrigin(quit, (sfVector2f) { 0, 10 });
    sfText_setCharacterSize(quit, 20);
    sfText_setPosition(quit, (sfVector2f) { 220, 400 });
    sfText_setColor(quit, (sfColor) { 255, 0, 0, 255 });
    sfText_setString(quit, "Quit");

    affScore = sfText_create();

    sfText_setFont(affScore, font);
    sfText_setOrigin(affScore, (sfVector2f) { 0, 10 });
    sfText_setCharacterSize(affScore, 20);
    sfText_setPosition(affScore, (sfVector2f) { 20, 25 });
    sfText_setColor(affScore, (sfColor) { 255, 255, 255, 255 });


    gameOver = sfText_create();
    sfText_setFont(gameOver, font);
    sfText_setOrigin(gameOver, (sfVector2f) { 15, 15 });
    sfText_setCharacterSize(gameOver, 30);
    sfText_setString(gameOver, "GAME OVER");
    sfText_setPosition(gameOver, (sfVector2f) { 165, 165 });
    sfText_setColor(gameOver, (sfColor) { 255, 0, 0, 0 });

    //title change font of one of his char so I need to declare it like this :/
    char titleTxt[9][3] = {
        "X",
        "e",
        "n",
        "o",
        "m",
        "a",
        "n",
        "u",
        "s",
    };
    struct lettre title[9];
    for (int i = 0; i < 9; i++) {
        title[i].text = sfText_create();
        title[i].pos = (sfVector2f){ 200 + 20 * i,150 };
        sfText_setFont(title[i].text, font);
        sfText_setCharacterSize(title[i].text, 20);
        sfText_setPosition(title[i].text, (sfVector2f) {
            title[i].pos.x, title[i].pos.y
        });
        sfText_setColor(title[i].text, (sfColor) { 255, 255, 255, 255 });
        sfText_setString(title[i].text, titleTxt[i]);
    }

    //
    struct obj bullet[10];
    for (int i = 0; i < 10; i++) {
        bullet[i].sprite = sfSprite_create();
        sfSprite_setOrigin(bullet[i].sprite, (sfVector2f) { 2, 2 });
        sfSprite_setScale(bullet[i].sprite, (sfVector2f) { 0.5, 0.5 });
        bullet[i].texture = sfTexture_createFromFile("Sprites/baballe.png", NULL);//15 sur 50 pixels
        bullet[i].dir.x = 0;
        bullet[i].dir.y = 0;
        bullet[i].pos.x = 250;
        bullet[i].pos.y = 250;
        bullet[i].vitesse = 0.5;
        bullet[i].visible = 0;
        bullet[i].force.x = 0;
        bullet[i].force.y = 0;
        bullet[i].angle = -90;
        sfSprite_setPosition(bullet[i].sprite, (sfVector2f) { bullet[i].pos.x, bullet[i].pos.y });
        sfSprite_setTexture(bullet[i].sprite, bullet[i].texture, sfTrue);
    }
    struct ennemis tentacle[20];
    for (int i = 0; i < 20; i++) {
        tentacle[i].sprite = sfSprite_create();
        sfSprite_setOrigin(tentacle[i].sprite, (sfVector2f) {(float)12.5, (float)12.5 });
        sfSprite_setScale(tentacle[i].sprite, (sfVector2f) { 0.5, 0.5 });
        if (easterEgg == 1) {
            tentacle[i].texture = sfTexture_createFromFile("Sprites/tentaculeBaguette.png", NULL);
        }
        else {
            tentacle[i].texture = sfTexture_createFromFile("Sprites/tentacule.png", NULL);//100 sur 100 pixels
        }
        tentacle[i].dir.x = 0;
        tentacle[i].dir.y = 0;
        tentacle[i].pos.x = 0;
        tentacle[i].pos.y = 0;
        tentacle[i].vitesse = 0.05;
        tentacle[i].exist = 0;
        tentacle[i].angle = -90;
        tentacle[i].norme = 0;
        sfSprite_setPosition(tentacle[i].sprite, (sfVector2f) { tentacle[i].pos.x, tentacle[i].pos.y });
        sfSprite_setTexture(tentacle[i].sprite, tentacle[i].texture, sfTrue);
    }
    struct big_thing boss;
    boss.sprite = sfSprite_create();
    sfSprite_setOrigin(boss.sprite, (sfVector2f) { 25, 25 });
    sfSprite_setScale(boss.sprite, (sfVector2f) { 0.5, 0.5 });
    if (easterEgg == 1) {
        boss.texture = sfTexture_createFromFile("Sprites/BossBaguette.png", NULL);
    }
    else {
        boss.texture = sfTexture_createFromFile("Sprites/boss.png", NULL);
    }
    //200 sur 200 pixels

    boss.dir.x = 0;
    boss.dir.y = 0;
    boss.pos.x = 0;
    boss.pos.y = 0;
    boss.vitesse = 0.025;
    boss.ugly = 0;
    boss.angle = -90;
    boss.norme = 0;
    boss.hp = 30;
    sfSprite_setPosition(boss.sprite, (sfVector2f) { boss.pos.x, boss.pos.y });
    sfSprite_setTexture(boss.sprite, boss.texture, sfTrue);

    struct j joueur;
    joueur.sprite = sfSprite_create();
    sfSprite_setOrigin(joueur.sprite, (sfVector2f) { 50, 50 });
    joueur.texture = sfTexture_createFromFile("Sprites/MC.png", NULL);
    joueur.pos = (sfVector2f){ 250,250 };
    joueur.angle = -90;
    //since we are in degrees, these are the corrects value for the angle
    joueur.angleMax = 120;
    joueur.angleMin = 60;
    joueur.vitesse = 0.25;
    joueur.vitesseMax = 1;
    joueur.fwd = (sfVector2f){ 0,0 };
    joueur.force = (sfVector2f){ 0,0 };
    joueur.life = 3;
    joueur.score = 0;
    sfSprite_setPosition(joueur.sprite, (sfVector2f) { joueur.pos.x, joueur.pos.y });
    sfSprite_setTexture(joueur.sprite, joueur.texture, sfTrue);
    sfSprite_setScale(joueur.sprite, (sfVector2f) { 0.8, 0.8 });

    arrow = sfSprite_create();
    textureArrow = sfTexture_createFromFile("Sprites/coche.png", NULL);
    sfSprite_setOrigin(arrow, (sfVector2f) { 10, 10 });
    sfSprite_setPosition(arrow, (sfVector2f) { 200, 350 });
    sfSprite_setTexture(arrow, textureArrow, sfTrue);
    sfSprite_setScale(arrow, (sfVector2f) { 0.2, 0.2 });

    test = sfSprite_create();
    test1 = sfSprite_create();
    test2 = sfSprite_create();
    arbre = sfSprite_create();
    arbre1 = sfSprite_create();
    arbre2 = sfSprite_create();
    textureTest = sfTexture_createFromFile("Sprites/MapGround.png", NULL);
    textureArbre = sfTexture_createFromFile("Sprites/MAP_TOP.png", NULL);
    sfSprite_setPosition(test, (sfVector2f) { 0, 0 });
    sfSprite_setTexture(test, textureTest, sfTrue);
    sfSprite_setScale(test, (sfVector2f) { 0.5, 0.5 });
    sfSprite_setPosition(test1, (sfVector2f) { 0, -500 });
    sfSprite_setTexture(test1, textureTest, sfTrue);
    sfSprite_setScale(test1, (sfVector2f) { 0.5, 0.5 });
    sfSprite_setPosition(test2, (sfVector2f) { 0, -1000 });
    sfSprite_setTexture(test2, textureTest, sfTrue);
    sfSprite_setScale(test2, (sfVector2f) { 0.5, 0.5 });
    sfSprite_setPosition(arbre, (sfVector2f) { 0, 0 });
    sfSprite_setTexture(arbre, textureArbre, sfTrue);
    sfSprite_setScale(arbre, (sfVector2f) { 0.5, 0.5 });
    sfSprite_setPosition(arbre1, (sfVector2f) { 0, -500 });
    sfSprite_setScale(arbre1, (sfVector2f) { 0.5, 0.5 });
    sfSprite_setTexture(arbre1, textureArbre, sfTrue);
    sfSprite_setPosition(arbre2, (sfVector2f) { 0, -1000 });
    sfSprite_setTexture(arbre2, textureArbre, sfTrue);
    sfSprite_setScale(arbre2, (sfVector2f) { 0.5, 0.5 });


    spriteL = sfSprite_create();
    textureL = sfTexture_createFromFile("Sprites/lumière.png", NULL);
    textureL = sfTexture_createFromFile("Sprites/lumière.png", NULL);
    sfSprite_setPosition(spriteL, (sfVector2f) {
        joueur.pos.x, joueur.pos.y
    });
    sfSprite_setOrigin(spriteL, (sfVector2f) { 1500, 1500 });
    sfSprite_setTexture(spriteL, textureL, sfTrue);
    sfSprite_setScale(spriteL, (sfVector2f) { 0.5, 0.5 });

    int press = 0;
    int delai;
    int apparition = 5000;
    int lostLife = 0;
    int delaiTxt = 6000;
    int gameState = 0;
    int transition = 0;
    int tmp;
    int rollTitle;
    int hold = 0;

    while (sfRenderWindow_isOpen(window)) {
        sfEvent event;
        while (sfRenderWindow_pollEvent(window, &event)) {
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);
        }
        sfClock_restart(Event);
        tmp = 0;
        while (!gameState) {
            while (sfRenderWindow_pollEvent(window, &event)) {
                if (event.type == sfEvtClosed)
                    sfRenderWindow_close(window);
            }
            tmp = timer(Event);
            sfRenderWindow_clear(window, sfBlack);
            if (tmp % 200 == 0) {
                if (!transition) {
                    rollTitle = rand() % 9;
                    sfText_setFont(title[rollTitle].text, oscour);
                    transition = 1;
                }
                else {
                    sfText_setFont(title[rollTitle].text, font);
                    transition = 0;
                }
            }
            if (sfKeyboard_isKeyPressed(sfKeyDown)) {
                if (!press) {
                    if (sfSprite_getPosition(arrow).y == 370) {
                        sfSprite_setPosition(arrow, (sfVector2f) { 300, 390 });
                    }
                    if (sfSprite_getPosition(arrow).y == 350 && sfSprite_getPosition(arrow).x == 200) {
                        sfSprite_setPosition(arrow, (sfVector2f) { 200, 400 });
                    }
                    if (sfSprite_getPosition(arrow).y == 350 && sfSprite_getPosition(arrow).x == 300) {
                        sfSprite_setPosition(arrow, (sfVector2f) { 300, 370 });
                    }
                }
                press = 1;
            }
            if (sfKeyboard_isKeyPressed(sfKeyUp)) {
                if (!press) {
                    /*if (sfSprite_getPosition(arrow).y == 800) {
                        sfSprite_setPosition(arrow, (sfVector2f) { 400, 700 });
                    }
                    if (sfSprite_getPosition(arrow).y == 740) {
                        sfSprite_setPosition(arrow, (sfVector2f) { 600, 700 });
                    }
                    if (sfSprite_getPosition(arrow).y == 780) {
                        sfSprite_setPosition(arrow, (sfVector2f) { 600, 740 });*/
                    if (sfSprite_getPosition(arrow).y == 400) {
                        sfSprite_setPosition(arrow, (sfVector2f) { 200, 350 });
                    }
                    if (sfSprite_getPosition(arrow).y == 370) {
                        sfSprite_setPosition(arrow, (sfVector2f) { 300, 350 });
                    }
                    if (sfSprite_getPosition(arrow).y == 390) {
                        sfSprite_setPosition(arrow, (sfVector2f) { 300, 370 });
                    }
                }
                press = 1;
            }

            if (sfKeyboard_isKeyPressed(sfKeySpace)) {
                if (!press) {
                    if (sfSprite_getPosition(arrow).x == 300 && sfSprite_getPosition(arrow).y == 350) {
                        for (int i = 0; i < 20; i++) {
                            tentacle[i].vitesse = 0.05;
                        }
                        apparition = 8000;
                        gameState = 1;
                    }
                    if (sfSprite_getPosition(arrow).y == 390) {
                        for (int i = 0; i < 20; i++) {
                            tentacle[i].vitesse = 0.11;
                        }
                        apparition = 3000;
                        gameState = 1;
                    }
                    if (sfSprite_getPosition(arrow).y == 370) {
                        gameState = 1;
                    }
                    if (sfSprite_getPosition(arrow).y == 350 && sfSprite_getPosition(arrow).x == 200) {
                        sfSprite_setPosition(arrow, (sfVector2f) { 300, 350 });
                    }
                    if (sfSprite_getPosition(arrow).y == 400) {
                        sfRenderWindow_close(window);
                        break;
                    }
                }
                press = 1;
            }
            if (!sfKeyboard_isKeyPressed(sfKeyUp) && !sfKeyboard_isKeyPressed(sfKeyDown) && !sfKeyboard_isKeyPressed(sfKeySpace)) {
                press = 0;
            }
            for (int i = 0; i < 9; i++) {
                sfRenderWindow_drawText(window, title[i].text, NULL);
            }
            sfRenderWindow_drawText(window, launch, NULL);
            sfRenderWindow_drawText(window, quit, NULL);
            sfRenderWindow_drawText(window, diff, NULL);
            sfRenderWindow_drawSprite(window, arrow, NULL);
            sfRenderWindow_display(window);
        }
        while (gameState == 1) {
            while (sfRenderWindow_pollEvent(window, &event)) {
                if (event.type == sfEvtClosed)
                    sfRenderWindow_close(window);
            }

            if (sfKeyboard_isKeyPressed(sfKeyRight)) {
                joueur.angle += 3 * delta / 10000;
                joueur.angleMax += 3 * delta / 10000;
                joueur.angleMin += 3 * delta / 10000;

            }
            if (sfKeyboard_isKeyPressed(sfKeyLeft)) {
                joueur.angle -= 3 * delta / 10000;
                joueur.angleMax -= 3 * delta / 10000;
                joueur.angleMin -= 3 * delta / 10000;
            }
            if (sfKeyboard_isKeyPressed(sfKeySpace)) {
                if (!press) {
                    gestionBullet(&bullet, joueur, 0);
                }
                press = 1;
            }
            else {
                press = 0;
            }
            if (sfKeyboard_isKeyPressed(sfKeyUp)) {
                joueur.fwd.x = cosf(joueur.angle * PI / 180);
                joueur.fwd.y = sinf(joueur.angle * PI / 180);
                /*double find_magnitude(double v1, double v2, double v3) {
                    double mag;
                    mag = sqrt(pow(v1, 2) + pow(v2, 2) + pow(v3, 2));
                    return(mag);
                }*/
                float velocityMagnitude = 0;
                velocityMagnitude = sqrtf(powf(joueur.force.x*joueur.vitesse, 2) + powf(joueur.force.y * joueur.vitesse, 2));
                if (velocityMagnitude<joueur.vitesseMax)
                /*if (fabs(joueur.force.x + joueur.vitesse * joueur.fwd.x * delta / 10000) < fabs(200 * joueur.fwd.x * delta / 10000))*/ {
                    joueur.force.x += joueur.vitesse * joueur.fwd.x * delta / 10000;
                /*if (fabs(joueur.force.y + joueur.vitesse * joueur.fwd.y * delta / 10000)< fabs(200 * joueur.fwd.y * delta / 10000))*/ 
                    joueur.force.y += joueur.vitesse * joueur.fwd.y * delta / 10000;
                }
            }
            if (joueur.pos.x + joueur.force.x > 500 || joueur.pos.x + joueur.force.x < 0) {
                joueur.force.x = 0;
            }
            if (joueur.pos.y + joueur.force.y > 500 || joueur.pos.y + joueur.force.y < 0) {
                joueur.force.y = 0;
            }

            gestionJoueur(&joueur, delta);

            gestionBullet(&bullet, joueur, 1);


            delai = timer(Event);
            if (delai >= apparition) {
                sfClock_restart(Event);
                delai = 0;
                gestionEnnemis(tentacle, &joueur, &bullet, 1, &lostLife, affScore, boss);
                if (apparition > 10) {
                    apparition -= 10;
                }

            }
            if (joueur.score != 0 && joueur.score % 10 == 0 && !boss.ugly && joueur.score != hold) {
                gestionBoss(&boss, &joueur, &bullet, 1, &lostLife, affScore);
                hold = joueur.score;
            }
            if (boss.ugly) {
                gestionBoss(&boss, &joueur, &bullet, 0, &lostLife, affScore);
                if (boss.hp <= 0) {
                    gestionEnnemis(tentacle, &joueur, &bullet, 2, &lostLife, affScore, boss);
                    boss.ugly = 0;
                }
            }
            printf("%d x : %f, y : %f\n", boss.ugly, boss.pos.x, boss.pos.y);
            gestionEnnemis(&tentacle, &joueur, &bullet, 0, &lostLife, affScore, boss);

            sfSprite_setRotation(spriteL, joueur.angle - 90);
            sfSprite_setRotation(joueur.sprite, joueur.angle - 90);
            sfSprite_move(test, (sfVector2f) { 0, delta / 1000 });
            sfSprite_move(test1, (sfVector2f) { 0, delta / 1000 });
            sfSprite_move(test2, (sfVector2f) { 0, delta / 1000 });
            sfSprite_move(arbre, (sfVector2f) { 0, delta / 1000 });
            sfSprite_move(arbre1, (sfVector2f) { 0, delta / 1000 });
            sfSprite_move(arbre2, (sfVector2f) { 0, delta / 1000 });
            if (sfSprite_getPosition(test2).y >= 0) {
                sfSprite_setPosition(test, (sfVector2f) { 0, 0 });
                sfSprite_setPosition(test1, (sfVector2f) { 0, -500 });
                sfSprite_setPosition(test2, (sfVector2f) { 0, -1000 });
                sfSprite_setPosition(arbre, (sfVector2f) { 0, 0 });
                sfSprite_setPosition(arbre1, (sfVector2f) { 0, -500 });
                sfSprite_setPosition(arbre2, (sfVector2f) { 0, -1000 });
            }
            Delta(deltaclock);

            sfRenderWindow_clear(window, sfWhite);
            sfRenderWindow_drawSprite(window, test, NULL);
            sfRenderWindow_drawSprite(window, test1, NULL);
            sfRenderWindow_drawSprite(window, test2, NULL);
            for (int i = 0; i < 10; i++) {
                if (bullet[i].visible) {
                    sfRenderWindow_drawSprite(window, bullet[i].sprite, NULL);
                }
            }
            for (int i = 0; i < 20; i++) {
                if (tentacle[i].exist) {
                    sfRenderWindow_drawSprite(window, tentacle[i].sprite, NULL);
                }
            }
            if (boss.ugly) {
                sfRenderWindow_drawSprite(window, boss.sprite, NULL);
            }
            sfRenderWindow_drawSprite(window, spriteL, NULL);
            sfRenderWindow_drawSprite(window, joueur.sprite, NULL);
            sfRenderWindow_drawSprite(window, arbre, NULL);
            sfRenderWindow_drawSprite(window, arbre1, NULL);
            sfRenderWindow_drawSprite(window, arbre2, NULL);
            sfRenderWindow_drawText(window, affScore, NULL);
            if (lostLife || delaiTxt <= 5000) {
                if (delaiTxt > 5000) {
                    lostLife = 0;
                    sfClock_restart(eventLostLife);
                    sfText_setColor(life, (sfColor) { 255, 0, 0, 255 });
                }
                delaiTxt = timer(eventLostLife);
                if (joueur.life > 0) {
                    sfText_setString(life, txtLife[2 - joueur.life]);// ?


                }
                else {
                    gameState = 2;
                    sfText_setString(life, txtLife[2]);
                }
                sfText_setColor(life, (sfColor) { 255, delaiTxt, delaiTxt, 255 });
                sfRenderWindow_drawText(window, life, NULL);

            }
            sfSprite_setPosition(spriteL, (sfVector2f) {
                joueur.pos.x, joueur.pos.y
            });
            sfSprite_setPosition(joueur.sprite, (sfVector2f) {
                joueur.pos.x, joueur.pos.y
            });
            



            sfRenderWindow_display(window);
        }

        sfRenderWindow_clear(window, sfBlack);
        sfClock_restart(eventLostLife);
        delaiTxt = timer(eventLostLife);
        sfColor gameOverClr;
        int restart = 0;
        sfText_setPosition(life, (sfVector2f) { 150, 350 });
        sfText_setColor(life, (sfColor) { 255, 0, 0, 255 });
        sfText_setColor(gameOver, (sfColor) { 255, 0, 0, 0 });
        for (int i = 0; i < 10; i++) {
            bullet[i].dir.x = 0;
            bullet[i].dir.y = 0;
            bullet[i].pos.x = 250;
            bullet[i].pos.y = 250;
            bullet[i].vitesse = 0.5;
            bullet[i].visible = 0;
            bullet[i].force.x = 0;
            bullet[i].force.y = 0;
            bullet[i].angle = -90;
            sfSprite_setPosition(bullet[i].sprite, (sfVector2f) { bullet[i].pos.x, bullet[i].pos.y });
        }
        for (int i = 0; i < 20; i++) {
            tentacle[i].dir.x = 0;
            tentacle[i].dir.y = 0;
            tentacle[i].pos.x = 0;
            tentacle[i].pos.y = 0;
            tentacle[i].vitesse = 0.025;
            tentacle[i].exist = 0;
            tentacle[i].angle = -90;
            tentacle[i].norme = 0;
            sfSprite_setPosition(tentacle[i].sprite, (sfVector2f) { tentacle[i].pos.x, tentacle[i].pos.y });
        }
        joueur.pos = (sfVector2f){ 500,500 };
        joueur.angle = -90;
        joueur.angleMax = 120;
        joueur.angleMin = 60;
        joueur.vitesse = 1;
        joueur.fwd = (sfVector2f){ 0,0 };
        joueur.force = (sfVector2f){ 0,0 };
        joueur.life = 3;
        joueur.score = 0;
        boss.dir.x = 0;
        boss.dir.y = 0;
        boss.pos.x = 0;
        boss.pos.y = 0;
        boss.vitesse = 0.0125;
        boss.ugly = 0;
        boss.angle = -90;
        boss.norme = 0;
        boss.hp = 20;
        sfSprite_setPosition(joueur.sprite, (sfVector2f) { joueur.pos.x, joueur.pos.y });
        sfSprite_setPosition(arrow, (sfVector2f) { 400, 700 });
        sfSprite_setPosition(test, (sfVector2f) { 0, 0 });
        sfSprite_setTexture(test, textureTest, sfTrue);
        sfSprite_setPosition(test1, (sfVector2f) { 0, -1000 });
        sfSprite_setTexture(test1, textureTest, sfTrue);
        sfSprite_setPosition(test2, (sfVector2f) { 0, -2000 });
        sfSprite_setTexture(test2, textureTest, sfTrue);
        sfSprite_setPosition(spriteL, (sfVector2f) { joueur.pos.x, joueur.pos.y });
        sfText_setString(affScore, joueur.score);
        press = 0;
        apparition = 5000;
        lostLife = 0;
        delaiTxt = 6000;
        transition = 0;
        sfText_setPosition(life, (sfVector2f) { 100, 250 });
        sfSprite_setPosition(arrow, (sfVector2f) { 400, 700 });
        while (gameState == 2) {
            while (sfRenderWindow_pollEvent(window, &event)) {
                if (event.type == sfEvtClosed)
                    sfRenderWindow_close(window);
            }

            sfRenderWindow_clear(window, sfBlack);
            gameOverClr = sfText_getColor(gameOver);
            if (gameOverClr.a < 255) {
                sfText_setColor(gameOver, (sfColor) { 255, 0, 0, delaiTxt / 10 });
            }
            else {
                restart = 1;
            }
            if (restart) {
                if (sfKeyboard_isKeyPressed(sfKeyDown)) {
                    if (!press) {
                        if (sfSprite_getPosition(arrow).y == 370) {
                            sfSprite_setPosition(arrow, (sfVector2f) { 300, 390 });
                        }
                        if (sfSprite_getPosition(arrow).y == 350 && sfSprite_getPosition(arrow).x == 200) {
                            sfSprite_setPosition(arrow, (sfVector2f) { 200, 400 });
                        }
                        if (sfSprite_getPosition(arrow).y == 350 && sfSprite_getPosition(arrow).x == 300) {
                            sfSprite_setPosition(arrow, (sfVector2f) { 300, 370 });
                        }
                    }
                    press = 1;
                }
                if (sfKeyboard_isKeyPressed(sfKeyUp)) {
                    if (!press) {
                        if (sfSprite_getPosition(arrow).y == 400) {
                            sfSprite_setPosition(arrow, (sfVector2f) { 200, 350 });
                        }
                        if (sfSprite_getPosition(arrow).y == 370) {
                            sfSprite_setPosition(arrow, (sfVector2f) { 300, 350 });
                        }
                        if (sfSprite_getPosition(arrow).y == 390) {
                            sfSprite_setPosition(arrow, (sfVector2f) { 300, 370 });
                        }
                    }
                    press = 1;
                }
                if (sfKeyboard_isKeyPressed(sfKeySpace)) {
                    if (!press) {
                        if (sfSprite_getPosition(arrow).x == 300 && sfSprite_getPosition(arrow).y == 350) {
                            for (int i = 0; i < 20; i++) {
                                tentacle[i].vitesse = 0.025;
                            }
                            apparition = 8000;
                            gameState = 1;
                        }
                        if (sfSprite_getPosition(arrow).y == 390) {
                            for (int i = 0; i < 20; i++) {
                                tentacle[i].vitesse = 0.05;
                            }
                            apparition = 3000;
                            gameState = 1;
                        }
                        if (sfSprite_getPosition(arrow).y == 370) {
                            gameState = 1;
                        }
                        if (sfSprite_getPosition(arrow).y == 300 && sfSprite_getPosition(arrow).x == 200) {
                            sfSprite_setPosition(arrow, (sfVector2f) { 300, 350 });
                        }
                        if (sfSprite_getPosition(arrow).y == 400) {
                            sfRenderWindow_close(window);
                            break;
                        }
                    }
                    press = 1;
                }
                if (!sfKeyboard_isKeyPressed(sfKeyUp) && !sfKeyboard_isKeyPressed(sfKeyDown) && !sfKeyboard_isKeyPressed(sfKeySpace)) {
                    press = 0;
                }
                sfRenderWindow_drawText(window, launch, NULL);
                sfRenderWindow_drawText(window, quit, NULL);
                sfRenderWindow_drawText(window, diff, NULL);
                sfRenderWindow_drawSprite(window, arrow, NULL);
            }

            sfRenderWindow_drawText(window, gameOver, NULL);
            sfText_setColor(life, (sfColor) { 255, delaiTxt, delaiTxt, 255 });
            sfRenderWindow_drawText(window, life, NULL);

            sfRenderWindow_display(window);

            delaiTxt = timer(eventLostLife);
        }


        for (int i = 0; i < 10; i++) {
            sfSprite_destroy(bullet[i].sprite);
            sfTexture_destroy(bullet[i].texture);
        }
        for (int i = 0; i < 20; i++) {
            sfSprite_destroy(tentacle[i].sprite);
            sfTexture_destroy(tentacle[i].texture);
        }
        sfSprite_destroy(boss.sprite);
        sfSprite_destroy(boss.texture);
        sfSprite_destroy(joueur.sprite);
        sfTexture_destroy(joueur.texture);
        sfSprite_destroy(spriteL);
        sfTexture_destroy(textureL);
        sfSprite_destroy(test);
        sfSprite_destroy(test1);
        sfSprite_destroy(test2);
        sfSprite_destroy(arbre);
        sfSprite_destroy(arbre1);
        sfSprite_destroy(arbre2);
        sfTexture_destroy(textureTest);
        sfTexture_destroy(textureArbre);
        for (int i = 0; i < 9; i++) {
            sfText_destroy(title[i].text);
        }
        sfText_destroy(launch);
        sfText_destroy(quit);
        sfText_destroy(life);
        sfText_destroy(diff);
        sfText_destroy(affScore);
        sfFont_destroy(font);
        sfClock_destroy(deltaclock);
        sfClock_destroy(Event);
        sfRenderWindow_destroy(window);
    }
}