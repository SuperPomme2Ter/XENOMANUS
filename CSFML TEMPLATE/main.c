#include <SFML/Graphics.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define PI 3.1415926535

float delta;
float angleTest;

struct j {
    sfSprite* sprite;
    sfTexture* texture;
    sfVector2f pos;
    sfVector2f fwd;
    sfVector2f force;
    float angle;
    float vitesse;
    float angleMax;
    float angleMin;
    int life;
    sfVector2f fwdMax;
    sfVector2f fwdMin;
};

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
struct ennemis {
    sfSprite* sprite;
    sfTexture* texture;
    sfVector2f dir;
    sfVector2f pos;
    int exist;
    float vitesse;
    float angle;
    float norme;
};
struct lettre {
    sfText* text;
    sfVector2f pos;
};


//calcul du temps
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
int timer(sfClock* delai) {
    sfTime  dt = sfClock_getElapsedTime(delai);
    float lastMouv = sfTime_asMilliseconds(dt);
    return lastMouv;

};

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

void gestionBullet(struct obj *bullet,struct j joueur,int option) {
    for (int i = 0; i < 10; i++) {
        if (option) {
            if (bullet[i].visible) {
                bullet[i].force.x += bullet[i].vitesse * bullet[i].dir.x * delta / 10000;
                bullet[i].force.y += bullet[i].vitesse * bullet[i].dir.y * delta / 10000;
                bullet[i].pos.x += bullet[i].force.x * delta / 10000;
                bullet[i].pos.y += bullet[i].force.y * delta / 10000;
                sfSprite_setPosition(bullet[i].sprite, (sfVector2f) { bullet[i].pos.x, bullet[i].pos.y });
                if (bullet[i].pos.x < 0 || bullet[i].pos.x>1000 || bullet[i].pos.y < 0 || bullet[i].pos.y>1000) {
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

int gestionEnnemis(struct ennemis *tentacle, struct j *joueur, struct obj* bullet, int option,int *lostLife)
{
    float ecart;
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
                sfSprite_setPosition(tentacle[i].sprite, (sfVector2f) { tentacle[i].pos.x, tentacle[i].pos.y });
                ecart = normalize(tentacle[i].pos.x - joueur->pos.x, tentacle[i].pos.y - joueur->pos.y);
                if (ecart < 50) {
                    tentacle[i].exist = 0;
                    joueur->life -= 1;
                    *lostLife = 1;
                }
                for (int j = 0; j < 10; j++) {
                    if (bullet[j].visible) {
                        ecart = normalize(tentacle[i].pos.x - bullet[j].pos.x, tentacle[i].pos.y - bullet[j].pos.y);
                        if (ecart < 50) {
                            tentacle[i].exist = 0;
                            bullet[j].visible = 0;
                        }
                    }
                }
            }
        }
        else {
            
            if (!tentacle[i].exist) {
                float x = rand() % 1600 - 300;
                float y = rand() % 1600 - 300;
                float deltaX = joueur->pos.x - x;
                float deltaY = joueur->pos.y - y;
                float angle = atan2f(deltaY, deltaX);
                angle = fabs(angle * (180.0f / PI));
                if ((angle < fabs(joueur->angleMax) && angle > fabs(joueur->angleMin)) || fabs(deltaX) < 300 || fabs(deltaY) < 300) {
                    x = rand() % 1600 - 300;
                    y = rand() % 1600 - 300;
                    deltaX = joueur->pos.x - x;
                    deltaY = joueur->pos.y - y;
                    angle = atan2f(deltaY, deltaX);
                    angle = fabs(angle * (180.0f / PI));
                }
                angleTest = angle;
                tentacle[i].pos.x = x;
                tentacle[i].pos.y = y;
                tentacle[i].exist = 1;
                break;
            }
        }
    }
}

int main() {

    srand(time(NULL));

    sfVideoMode mode = { 1000, 1000, 32 };
    sfRenderWindow* window = sfRenderWindow_create(mode, "Xenomanus", sfResize | sfClose, NULL);
    sfTexture* textureL;
    sfSprite* spriteL;
    sfTexture* textureTest;
    sfSprite* test;
    sfClock* deltaclock = sfClock_create();
    sfClock* Event = sfClock_create();
    sfClock* eventLostLife = sfClock_create();
    sfTexture* textureF;
    sfSprite* spriteF;
 //   sfText* launch;
 //   sfText* quit;
    sfText* life;
    sfFont* font; 
    sfFont* oscour;

    char txtLife[3][80] = {
        "You have been hit by something.",
        "You're bleeding !",
        "As you fall, you passed away,\nSoon to be catch by the creature."

    };
    oscour = sfFont_createFromFile("wingding.ttf");
    font = sfFont_createFromFile("arial.ttf");
    life = sfText_create();

    sfText_setFont(life, font);
    sfText_setOrigin(life, (sfVector2f) { 0, 25 });
    sfText_setCharacterSize(life, 40);
    sfText_setPosition(life, (sfVector2f) { 300, 700 });
    sfText_setColor(life, (sfColor) { 255, 0, 0, 255 });

    
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
        title[i].text= sfText_create();
        title[i].pos = (sfVector2f){400+40*i,300};
        sfText_setFont(title[i].text, font);
        sfText_setCharacterSize(title[i].text, 40);
        sfText_setPosition(title[i].text, (sfVector2f) { title[i].pos.x, title[i].pos.y
        });
        sfText_setColor(title[i].text, (sfColor) { 255, 255, 255, 255 });
        sfText_setString(title[i].text, titleTxt[i]);
    }
    

    
    struct obj bullet[10];
    for (int i = 0; i < 10; i++) {
        bullet[i].sprite = sfSprite_create();
        sfSprite_setOrigin(bullet[i].sprite, (sfVector2f) { 8, 8 });
        bullet[i].texture = sfTexture_createFromFile("baballe.png", NULL);//15 sur 50 pixels
        bullet[i].dir.x = 0;
        bullet[i].dir.y = 0;
        bullet[i].pos.x = 500;
        bullet[i].pos.y = 500;
        bullet[i].vitesse = 1;
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
        sfSprite_setOrigin(tentacle[i].sprite, (sfVector2f) { 50, 50 });
        tentacle[i].texture = sfTexture_createFromFile("tentacule.png", NULL);//100 sur 100 pixels
        tentacle[i].dir.x = 0;
        tentacle[i].dir.y = 0;
        tentacle[i].pos.x = 0;
        tentacle[i].pos.y = 0;
        tentacle[i].vitesse = 0.1;
        tentacle[i].exist = 0;
        tentacle[i].angle = -90;
        tentacle[i].norme = 0;
        sfSprite_setPosition(tentacle[i].sprite, (sfVector2f) { tentacle[i].pos.x, tentacle[i].pos.y });
        sfSprite_setTexture(tentacle[i].sprite, tentacle[i].texture, sfTrue);
    }

    struct j joueur;
    joueur.sprite = sfSprite_create();
    sfSprite_setOrigin(joueur.sprite, (sfVector2f) { 8, 8 });
    joueur.texture = sfTexture_createFromFile("fruit.png", NULL);
    joueur.pos = (sfVector2f){ 500,500 };
    joueur.angle = -90;
    joueur.angleMax = 120;
    joueur.angleMin = 60;
    joueur.vitesse = 0.5;
    joueur.fwd = (sfVector2f){ 0,0 };
    joueur.force = (sfVector2f){ 0,0 };
    joueur.life = 3;
    sfSprite_setPosition(joueur.sprite, (sfVector2f) { joueur.pos.x, joueur.pos.y });
    sfSprite_setTexture(joueur.sprite, joueur.texture, sfTrue);

    test = sfSprite_create();
    textureTest = sfTexture_createFromFile("test.png", NULL);
    sfSprite_setPosition(test, (sfVector2f) { 0, 0 });
    sfSprite_setTexture(test, textureTest, sfTrue);

    spriteL = sfSprite_create();
    textureL = sfTexture_createFromFile("lumière.png", NULL);
    sfSprite_setPosition(spriteL, (sfVector2f) { joueur.pos.x, joueur.pos.y
    });
    sfSprite_setOrigin(spriteL, (sfVector2f) { 1500, 1500 });
    sfSprite_setTexture(spriteL, textureL, sfTrue);

    int press = 0;
    int delai;
    int apparition = 5000;
    int lostLife = 0;
    int delaiTxt = 6000;
    int gameState = 0;
    int transition = 0;
    int tmp;
    int rollTitle;

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
            if (tmp % 80 == 0) {
                if (!transition) {
                    rollTitle = rand() % 9;
                    sfText_setFont(title[rollTitle].text, oscour);
                    transition = 1;
                }else{
                    sfText_setFont(title[rollTitle].text, font);
                    transition = 0;
                }
            }
            for (int i = 0; i < 9; i++) {
                sfRenderWindow_drawText(window, title[i].text, NULL);
            }
            sfRenderWindow_display(window);
        }
        if (sfKeyboard_isKeyPressed(sfKeyRight)) {
            joueur.angle += 3 * delta / 10000;
            joueur.angleMax += 3 * delta / 10000;
            joueur.angleMin += 3 * delta / 10000;

        }
        if (sfKeyboard_isKeyPressed(sfKeyLeft)) {
            joueur.angle -= 2 * delta / 10000;
            joueur.angleMax -= 2 * delta / 10000;
            joueur.angleMin -= 2 * delta / 10000;
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
            if (fabs(joueur.force.x + joueur.vitesse * joueur.fwd.x * delta / 10000) < fabs(150 * joueur.fwd.x * delta / 10000)) {
                joueur.force.x += joueur.vitesse * joueur.fwd.x * delta / 10000;
            }
            if (fabs(joueur.force.y + joueur.vitesse * joueur.fwd.y * delta / 10000) < fabs(150 * joueur.fwd.y * delta / 10000)) {
                joueur.force.y += joueur.vitesse * joueur.fwd.y * delta / 10000;
            }
        }
        if (joueur.pos.x + joueur.force.x > 1000 || joueur.pos.x + joueur.force.x < 0) {
            joueur.force.x = 0;
        }
        if (joueur.pos.y + joueur.force.y > 1000 || joueur.pos.y + joueur.force.y < 0) {
            joueur.force.y = 0;
        }
        
        gestionJoueur(&joueur,delta);

        gestionBullet(&bullet, joueur, 1);


        delai=timer(Event);
        if (delai >= apparition) {
            sfClock_restart(Event);
            delai = 0;
            gestionEnnemis(tentacle, &joueur, &bullet, 1,&lostLife);
            if (apparition > 10) {
                apparition -= 10;
            }
            
        }
        printf("%d\n",joueur.life);
        gestionEnnemis(&tentacle, &joueur, &bullet,0,&lostLife);
        
        sfSprite_setRotation(spriteL, joueur.angle - 90);
        sfSprite_setRotation(joueur.sprite, joueur.angle - 90);

        Delta(deltaclock);
        
        sfRenderWindow_clear(window, sfWhite);
        sfRenderWindow_drawSprite(window, test, NULL);
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
        sfRenderWindow_drawSprite(window, spriteL, NULL);
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
                sfText_setString(life, txtLife[2]);
            }
            sfText_setColor(life, (sfColor) { 255, delaiTxt / 10, delaiTxt / 10, 255 });
            sfRenderWindow_drawText(window, life, NULL);

        }
        sfSprite_setPosition(spriteL, (sfVector2f) { joueur.pos.x, joueur.pos.y
        });
        sfSprite_setPosition(joueur.sprite, (sfVector2f) { joueur.pos.x, joueur.pos.y
        });
        sfRenderWindow_drawSprite(window, joueur.sprite, NULL);



        sfRenderWindow_display(window);
    }

    for (int i = 0; i < 10; i++) {
        sfSprite_destroy(bullet[i].sprite);
        sfTexture_destroy(bullet[i].texture);
    }
    for (int i = 0; i < 20; i++) {
        sfSprite_destroy(tentacle[i].sprite);
        sfTexture_destroy(tentacle[i].texture);
    }
    sfSprite_destroy(joueur.sprite);
    sfTexture_destroy(joueur.texture);
    sfSprite_destroy(spriteL);
    sfTexture_destroy(textureL);
    sfSprite_destroy(test);
    sfTexture_destroy(textureTest);
    for (int i = 0; i < 9; i++) {
        sfText_destroy(title[i].text);
    }
 //   sfText_destroy(launch);
 //   sfText_destroy(quit);
    sfText_destroy(life);
    sfFont_destroy(font);
    sfClock_destroy(deltaclock);
    sfClock_destroy(Event);
    sfRenderWindow_destroy(window);
}