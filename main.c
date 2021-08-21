//main.c
#include "gba.h"

void sync()
{
    //always draw in vbuffer
    while(REG_DISPLAY_VCOUNT >= 160);
    while(REG_DISPLAY_VCOUNT < 160);
}

//draw the rectangle object that makes up the objects in the game world
void drawRect(struct Rect r, uint16 color)
{
    for(int i = 0; i < r.h; i++)
    {
        for(int j = 0; j < r.w; j++)
        {
            SCREENBUFFER[SCREEN_WIDTH * (i + r.y) + (j + r.x)] = color;
        }
    }
}

//initialize the segments a-g for the both seven segment displays 
//and position them in the gameworld
void init7seg()
{
    s1.a.w = 16;
    s1.a.h = 4;
    s1.f.w = 4;
    s1.f.h = 16;
    s1.b = s1.c = s1.e = s1.f;
    s1.d = s1.g = s1.a;

    s1.a.x = s1.d.x = s1.e.x = s1.f.x = s1.g.x = SCREEN_WIDTH - 80;
    s1.b.x = s1.c.x = s1.a.x + s1.a.w;

    s1.a.y = s1.b.y = s1.f.y = 0;
    s1.c.y = s1.e.y = s1.g.y = s1.b.y + s1.b.h - s1.a.h;
    s1.d.y = s1.g.y + s1.b.h - s1.a.h;

    s2.a.w = 16;
    s2.a.h = 4;
    s2.f.w = 4;
    s2.f.h = 16;
    s2.b = s2.c = s2.e = s2.f;
    s2.d = s2.g = s2.a;

    s2.a.x = s2.d.x = s2.e.x = s2.f.x = s2.g.x = SCREEN_WIDTH - 160;
    s2.b.x = s2.c.x = s2.a.x + s2.a.w;

    s2.a.y = s2.b.y = s2.f.y = 0;
    s2.c.y = s2.e.y = s2.g.y = s2.b.y + s2.b.h - s2.a.h;
    s2.d.y = s2.g.y + s2.b.h - s2.a.h;
}

//clear the seven segment display that tracks player 2 score
void clear7seg()
{
    drawRect(s1.a, 0x0000);
    drawRect(s1.b, 0x0000);
    drawRect(s1.c, 0x0000);
    drawRect(s1.d, 0x0000);
    drawRect(s1.e, 0x0000);
    drawRect(s1.f, 0x0000);
    drawRect(s1.g, 0x0000);
}

//clear the seven segment display that tracks player 1 score
void clear7seg2()
{
    drawRect(s2.a, 0x0000);
    drawRect(s2.b, 0x0000);
    drawRect(s2.c, 0x0000);
    drawRect(s2.d, 0x0000);
    drawRect(s2.e, 0x0000);
    drawRect(s2.f, 0x0000);
    drawRect(s2.g, 0x0000);
}

//draw the seven segment display that tracks the player 2 score
void draw7seg(uint8 num)
{
    clear7seg();
    //bits of number
    uint8 w, x, y, z;

    w = num & 8;
    x = num & 4;
    y = num & 2;
    z = num & 1;

    if(y || w || (x && z) || (!x && !z))
        drawRect(s1.a, 0x001f);

    if(!x || (!y && !z) || (y && z))
        drawRect(s1.b, 0x001f);

    if(x || !y || z)
        drawRect(s1.c, 0x001f);

    if((!x && !z) || (!x && y && z) || (x && !y && z) || (x&& y && !z))
        drawRect(s1.d, 0x001f);

    if((!x && !z) || (y && !z))
        drawRect(s1.e, 0x001f);

    if(w || (!y && !z) || (x && !y) || (x && !z))
        drawRect(s1.f, 0x001f);

    if(w || (x && !y) || (!x && y) || (y && !z))
        drawRect(s1.g, 0x001f);
}

//draw the second segment display that tracks player 1 score
void draw7seg2(uint8 num)
{
    clear7seg2();
    //bits of number
    uint8 w, x, y, z;

    w = num & 8;
    x = num & 4;
    y = num & 2;
    z = num & 1;

    if(y || w || (x && z) || (!x && !z))
        drawRect(s2.a, 0x001f);
    
    if(!x || (!y && !z) || (y && z))
        drawRect(s2.b, 0x001f);

    if(x || !y || z)
        drawRect(s2.c, 0x001f);

    if((!x && !z) || (!x && y && z) || (x && !y && z) || (x&& y && !z))
        drawRect(s2.d, 0x001f);

    if((!x && !z) || (y && !z))
        drawRect(s2.e, 0x001f);

    if(w || (!y && !z) || (x && !y) || (x && !z))
        drawRect(s2.f, 0x001f);

    if(w || (x && !y) || (!x && y) || (y && !z))
        drawRect(s2.g, 0x001f);
}

//initialize position for the ball, player 1, and player 2
void initRects()
{
    player1.x = 0;
    player1.y = 0; 
    player1.w = 8;
    player1.h = 32;

    ball.w = 8;
    ball.h = 8;
    ball.x = SCREEN_WIDTH/2 - ball.w/2;
    ball.y = SCREEN_HEIGHT/2 - ball.h/2;

    player2.w = 8;
    player2.h = 32;
    player2.x = SCREEN_WIDTH - player2.w;
    player2.y = SCREEN_HEIGHT - player2.h;
}

//reset ball to the center of the game world
void ballReset()
{
    sync();
    drawRect(ball, 0x0000);
    ball.x = SCREEN_WIDTH/2 - ball.w/2;
    ball.y = SCREEN_HEIGHT/2 - ball.h/2;
}

int main()
{
    REG_DISPLAY = VIDEOMODE | BGMODE;

    init7seg(); 
    initRects(); 

    uint8 counter = 0;
    uint8 counter2 = 0;
    uint16 c = 0x001f; //might not be using this
    uint16 ballColor = 0x0ff0;
    uint16 playerColor = 0x7f00;

    uint8 down = 0;

    int speedX = 1;
    int speedY = 1;

    while(1)
    {
        sync();
        //initalize the ball, player 1, and player 2
        drawRect(ball, 0x0000);
        drawRect(player1, 0x0000);
        drawRect(player2, 0x0000);

        //setting the speed of the ball and making sure the ball bounces of the top of the screen
        ball.x += speedX;
        ball.y += speedY;
        if(ball.x + ball.w == SCREEN_WIDTH || ball.x == 0)
            speedX = -speedX;
        if(ball.y + ball.h == SCREEN_HEIGHT || ball.y == 0)
            speedY = -speedY;
        
        //movement for player 1
        if(!(REG_DISPLAY_INPUT & A))
            player1.y += 1;
        if(!(REG_DISPLAY_INPUT & B))
            player1.y -= 1;
        //keep player 1 inside the bounds of the screen
        if(player1.y < 0)
            player1.y = 0;
        if(player1.y + player1.h > SCREEN_HEIGHT)
            player1.y = SCREEN_HEIGHT - player1.h;
        //collision for ball for player 1
        if(ball.x == player1.w)
        {
            if(ball.y < player1.y + player1.h && ball.y + ball.h > player1.y)
            {
                speedX = -speedX;
            }
        }
        
        //movement for player 2
        if(!(REG_DISPLAY_INPUT & UP))
            player2.y -= 1;
        if(!(REG_DISPLAY_INPUT & DOWN))
            player2.y += 1;
        //keep player 2 inside the bounds of the screen
        if(player2.y < 0)
            player2.y = 0;
        if(player2.y + player2.h > SCREEN_HEIGHT)
            player2.y = SCREEN_HEIGHT - player2.h;
        //collision with ball for player 2
        if(ball.x + ball.w == player2.x)
        {
            if(ball.y + ball.h >= player2.y && ball.y <= player2.y + player2.h)
            {
                speedX = -speedX;
            }
        }

        //draw player1, ball, and player2
        drawRect(player1, playerColor);
        drawRect(ball, ballColor);
        drawRect(player2, playerColor);

        if(ball.x == 0) //Counter when player 1 scores a point 
        {
            down = 1;
            counter++;
            if(counter > 9)
            {
                counter = 0;
                counter2 = 0;
            }
            ballReset();
        }
        else if(ball.x + ball.w == SCREEN_WIDTH) //counter when player 2 scores a point
        {
            down = 1;
            counter2++;
            if(counter2 > 9)
            {
                counter2 = 0;
                counter = 0;
            }
            ballReset();
        }
        else
        {
            down = 0;
        }

        //draw the first and second seven segment display   
        draw7seg(counter);
        draw7seg2(counter2);
    }
    return 0;
}
