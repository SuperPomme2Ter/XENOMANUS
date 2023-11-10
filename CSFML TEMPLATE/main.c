#include <SFML/Graphics.h>
#include <math.h>
#include<stdio.h>
#define PI 3.1415926535
float delta;

//calcul du temps
int Delta(sfClock* deltaclock) {
    sfTime dtime = sfClock_getElapsedTime(deltaclock);
    delta = sfTime_asMicroseconds(dtime);

    sfClock_restart(deltaclock);
    return 0;
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

int main() {
    struct obj bullet[10];
    for (int i = 0; i < 10; i++) {
        bullet[i].sprite = sfSprite_create();
        sfSprite_setOrigin(bullet[i].sprite, (sfVector2f) { 8, 8 });
        bullet[i].texture = sfTexture_createFromFile("baballe.png", NULL);
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
    sfVideoMode mode = { 1000, 1000, 32 };
    sfRenderWindow* window = sfRenderWindow_create(mode, "CSFML Test", sfResize | sfClose, NULL);
    sfTexture* textureL;
    sfSprite* spriteL;
    sfTexture* textureTest;
    sfSprite* test;
    sfClock* deltaclock = sfClock_create();
    sfTexture* textureF;
    sfSprite* spriteF;
    test = sfSprite_create();
    textureTest = sfTexture_createFromFile("test.png", NULL);
    sfSprite_setPosition(test, (sfVector2f) { 0, 0 });
    sfSprite_setTexture(test, textureTest, sfTrue);
    float camPos[2] = { 500,500 };
    float projectile[2] = { 500,500 };
    float velProjectile[2] = { 0,0 };
    sfVector2f force = { 0,0 };
    float vitesse = 0.5;
    spriteL = sfSprite_create();
    textureL = sfTexture_createFromFile("lumière.png", NULL);
    sfSprite_setPosition(spriteL, (sfVector2f) { camPos[0], camPos[1] });
    sfSprite_setOrigin(spriteL, (sfVector2f) { 1500, 1500 });
    sfSprite_setTexture(spriteL, textureL, sfTrue);
    spriteF = sfSprite_create();
    textureF = sfTexture_createFromFile("fruit.png", NULL);
    sfSprite_setPosition(spriteF, (sfVector2f) { projectile[0], projectile[1] });
    sfSprite_setOrigin(spriteF, (sfVector2f) { 8, 8 });
    sfSprite_setScale(spriteF, (sfVector2f) { 7, 7 });
    sfSprite_setTexture(spriteF, textureF, sfTrue);
    float angle = -90;
    float fwd[2] = { 0,0 };
    float a_length;
    float normalized_x;
    float normalized_y;
    int press = 0;
    while (sfRenderWindow_isOpen(window)) {
        sfEvent event;
        while (sfRenderWindow_pollEvent(window, &event)) {
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);
        }


        if (sfKeyboard_isKeyPressed(sfKeyRight)) {
            angle += 3 * delta / 10000;

        }
        if (sfKeyboard_isKeyPressed(sfKeyLeft)) {
            angle -= 3 * delta / 10000;
        }
        if (sfKeyboard_isKeyPressed(sfKeySpace)) {
            if (!press) {
                for (int i = 0; i < 10; i++) {
                    if (!bullet[i].visible) {
                        bullet[i].pos.x = camPos[0];
                        bullet[i].pos.y = camPos[1];
                        bullet[i].dir.x = cosf(angle * PI / 180);
                        bullet[i].dir.y = sinf(angle * PI / 180);
                        bullet[i].visible = 1;
                        bullet[i].angle = angle;
                        sfSprite_setRotation(bullet[i].sprite, bullet[i].angle + 90);
                        break;
                    }
                }
            }
            press = 1;
        }
        else {
            press = 0;
        }
        if (sfKeyboard_isKeyPressed(sfKeyUp)) {
            fwd[0] = cosf(angle * PI / 180);
            fwd[1] = sinf(angle * PI / 180);
            if (fabs(force.x + vitesse * fwd[0] * delta / 10000) < fabs(150 * fwd[0] * delta / 10000)) {
                force.x += vitesse * fwd[0] * delta / 10000;
            }
            if (fabs(force.y + vitesse * fwd[1] * delta / 10000) < fabs(150 * fwd[1] * delta / 10000)) {
                force.y += vitesse * fwd[1] * delta / 10000;
            }
        }
        if (camPos[0] + force.x > 1000 || camPos[0] + force.x < 0) {
            force.x = 0;
        }
        if (camPos[1] + force.y > 1000 || camPos[1] + force.y < 0) {
            force.y = 0;
        }
        camPos[0] += force.x * delta / 10000;
        camPos[1] += force.y * delta / 10000;

        a_length = sqrt(force.x * force.x + force.y * force.y);
        if (a_length > 0) {
            normalized_x = force.x / a_length;
            normalized_y = force.y / a_length;
            force.x -= 0.1 * normalized_x * delta / 10000;
            force.y -= 0.1 * normalized_y * delta / 10000;
        }
        for (int i = 0; i < 10; i++) {
            if (bullet[i].visible) {
                bullet[i].force.x += bullet[i].vitesse * bullet[i].dir.x * delta / 10000;
                bullet[i].force.y += bullet[i].vitesse * bullet[i].dir.y * delta / 10000;
                bullet[i].pos.x += bullet[i].force.x * delta / 10000;
                bullet[i].pos.y += bullet[i].force.y * delta / 10000;
                sfSprite_setPosition(bullet[i].sprite, (sfVector2f) { bullet[i].pos.x, bullet[i].pos.y });
                if (bullet[i].pos.x < 0 || bullet[i].pos.x>1000 || bullet[i].pos.y < 0 || bullet[i].pos.y>1000) {
                    bullet[i].pos.x = camPos[0];
                    bullet[i].pos.y = camPos[1];
                    bullet[i].force.x = 0;
                    bullet[i].force.y = 0;
                    bullet[i].visible = 0;
                }
            }
        }


        sfSprite_setRotation(spriteL, angle - 90);
        sfSprite_setRotation(spriteF, angle - 90);


        if (angle <= -360 || angle >= 360) {
            angle = 0;
        }
        Delta(deltaclock);
        //   printf("%f\n", angle);
        sfRenderWindow_clear(window, sfWhite);
        sfRenderWindow_drawSprite(window, test, NULL);
        for (int i = 0; i < 10; i++) {
            if (bullet[i].visible) {
                sfRenderWindow_drawSprite(window, bullet[i].sprite, NULL);
            }
        }
        sfRenderWindow_drawSprite(window, spriteL, NULL);
        sfSprite_setPosition(spriteL, (sfVector2f) { camPos[0], camPos[1] });
        sfSprite_setPosition(spriteF, (sfVector2f) { camPos[0], camPos[1] });
        sfRenderWindow_drawSprite(window, spriteF, NULL);



        sfRenderWindow_display(window);
    }
    for (int i = 0; i < 10; i++) {
        sfSprite_destroy(bullet[i].sprite);
        sfTexture_destroy(bullet[i].texture);
    }
    sfSprite_destroy(spriteF);
    sfTexture_destroy(textureF);
    sfSprite_destroy(spriteL);
    sfTexture_destroy(textureL);
    sfSprite_destroy(test);
    sfTexture_destroy(textureTest);
    sfRenderWindow_destroy(window);
}