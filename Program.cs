using System;
using System.Drawing;
using SdlDotNet.Graphics;
using SdlDotNet.Core;

namespace Breakout
{

    static class Program
    {
        static int[,] brick_pos = new int[,] 
        { 
            {0,0,1}, {0,50,1}, {0,100,1}, {0,150,1}, {0,200,1}, {0,250,1},{0,300,1},
            {0,350,1}, {0,400,1}, {0,450,1}, {0,500,1}, {0,550,1}, {0,600,1}, {50,0,1}, 
            {50,50,1}, {50,100,1}, {50,150,1}, {50,200,1}, {50,250,1}, {50,300,1},
            {50,350,1}, {50,400,1}, {50,450,1}, {50,500,1}, {50,550,1}, {50,600,1},
            {100,0,1}, {100,50,1}, {100,100,1}, {100,150,1}, {100,200,1}, {100,250,1},
            {100,300,1}, {100,350,1}, {100,400,1}, {100,450,1}, {100,500,1}, {100,550,1},
            {100,600,1},
        };
        static int rx = 0;
        static int ry = 0;
        static int rw = 35;
        static int rh = 35;
        static int xpos = 0;
        static int ballx = 0;
        static int bally = 0;
        static int ball_DX = 4;
        static int ball_DY = 4;
        static bool start = false;
        static bool is_drawn = true;
        static bool remove = false;

        static void draw_bricks()
        {
            for (int i = 0; i <= brick_pos.GetUpperBound(0); i++) {
                int brick_y = brick_pos[i, 0];
                int brick_x = brick_pos[i, 1];
                int brick_life = brick_pos[i, 2];
                if (brick_life == 1)
                    drawSprite(Sprite.bricks, brick_x, brick_y);
            }
        }
        static void isPointInRect(int ballx, int bally, int rx, int ry, int rw, int rh)
        {
            int RT_x = rx + rw;
            int RT_y = ry;
            int LT_x = rx;
            int LT_y = ry;
            int RB_x = rx + rw;
            int RB_y = ry + rh;
            int LB_x = rx;
            int LB_y = ry + rh;            
            if ((ballx >= LT_x) && (ballx <= RT_x) && (bally >= LT_y) && (bally <= LB_y))
                remove = true;
            else
                remove = false;
        }
        static void onTick(object sender, TickEventArgs args)
        {
            ballx += ball_DX;
            bally += ball_DY;
            isPointInRect(ballx, bally, rx,ry,rw,rh);
            if (remove == true)
            {
            }
            if (ballx < 0 || ballx > 616)
            {
                ball_DX *= -1;
            }
            if (bally < 0)
            {
                ball_DY *= -1;
            }
            if ((ballx > xpos)&&(ballx < xpos + 104)&&(bally <=442)&&(bally >= 370))
            {
                ball_DY *= -1;              
            }
            if (bally > 455)
            {
                start = false;
            }
            if ((bally == 0) && (ballx == 0))
            {
                is_drawn = false;
            }
            drawBackground();
            draw_bricks();                                    
            if (start == false)
            {
                drawSprite(Sprite.ball_small, xpos + 34, 350);               
            }
            else
            {
                drawSprite(Sprite.ball_small, ballx, bally);
            }
            drawSprite(Sprite.bat_medium, xpos, 390);
            video.Update();

        }
        static void onMouseMove(object sender, SdlDotNet.Input.MouseMotionEventArgs args)
        {
            int batw = sprite_sheet_cut[(int)Sprite.bat_small].Width;
            xpos += args.RelativeX;
            if (xpos > 640 - batw)
                xpos = 640 - batw;
            
            if (xpos < 0)
                xpos = 0;
        }
        static void onMouseButton(object sender, SdlDotNet.Input.MouseButtonEventArgs args)
        {
            if (start == false)
            {
                start = true;
                ballx = xpos + 34;
                bally = 350;
            }
        }
        static void onKeyboard(object sender, SdlDotNet.Input.KeyboardEventArgs args)
        {
        }
        static void drawBackground()
        {
            video.Blit(imgBackground);
        }
        static void drawSprite(Sprite sprite, int x, int y)
        {
            video.Blit(imgSpriteSheet, new Point(x, y), sprite_sheet_cut[(int)sprite]);

        }
        static void Main()
        {
            System.Windows.Forms.Cursor.Hide();
            video = Video.SetVideoMode(FRAME_WIDTH, FRAME_HEIGHT, 
                COLOUR_DEPTH, FRAME_RESIZABLE, USE_OPENGL, FRAME_FULLSCREEN, USE_HARDWARE);
            Video.WindowCaption = "Breakout";
            Video.WindowIcon(new Icon(@"breakout.ico"));
            setup();
            Events.Tick += new EventHandler<TickEventArgs>(onTick);
            Events.Quit += new EventHandler<QuitEventArgs>(onQuit);
            Events.KeyboardDown += 
              new EventHandler<SdlDotNet.Input.KeyboardEventArgs>(onKeyboard);
            Events.KeyboardUp += 
              new EventHandler<SdlDotNet.Input.KeyboardEventArgs>(onKeyboard);
            Events.MouseButtonDown += 
              new EventHandler<SdlDotNet.Input.MouseButtonEventArgs>(onMouseButton);
            Events.MouseButtonUp += 
              new EventHandler<SdlDotNet.Input.MouseButtonEventArgs>(onMouseButton);
            Events.MouseMotion += 
              new EventHandler<SdlDotNet.Input.MouseMotionEventArgs>(onMouseMove);
            Events.TargetFps = 60;
            Events.Run();
        }
        /* Dragon code:
         * main setup of sdl core and sprites
         */
        static void setup()
        {
            imgBackground = new Surface(@"breakout_bg.png").Convert(video, true, false);
            imgSpriteSheet = new Surface(@"breakout_sprites.png");
            {
                int s = (int)Sprite.red;
                int x = 0;
                int y = 0;
                for (int i = 0; i < 4; ++i)
                {
                    for (int j = 0; j < 10; ++j)
                    {
                        sprite_sheet_cut[s] = new Rectangle(x, y, 40, 40);
                        s = s + 1;
                        x = x + 40;
                    }
                    x = 0;
                    y = y + 40;
                }
            }
            {
                int s = (int)Sprite.coin_00;
                int x = 0;
                int y = 376;
                for (int i = 0; i < 16; ++i)
                {
                    sprite_sheet_cut[s] = new Rectangle(x, y, 24, 24);
                    s = s + 1;
                    x = x + 24;
                }
            }
            sprite_sheet_cut[(int)Sprite.bat_small] = new Rectangle(0, 200, 104, 32);
            sprite_sheet_cut[(int)Sprite.bat_medium] = new Rectangle(0, 240, 120, 32);
            sprite_sheet_cut[(int)Sprite.bat_large] = new Rectangle(0, 280, 136, 32);
            sprite_sheet_cut[(int)Sprite.ball_small] = new Rectangle(160, 200, 16, 16);
            sprite_sheet_cut[(int)Sprite.ball_small] = new Rectangle(160, 240, 24, 24);
            sprite_sheet_cut[(int)Sprite.star] = new Rectangle(280, 0, 32, 32);
            sprite_sheet_cut[(int)Sprite.cross] = new Rectangle(200, 160, 80, 80);
            sprite_sheet_cut[(int)Sprite.flare] = new Rectangle(320, 160, 80, 80); ;
            sprite_sheet_cut[(int)Sprite.swirl] = new Rectangle(200, 280, 80, 80); ;
            sprite_sheet_cut[(int)Sprite.spiral] = new Rectangle(320, 280, 80, 80); ;
        }
        static void gameover()
        {  
        }
        static void onQuit(object sender, QuitEventArgs args)
        {
            Events.QuitApplication();
        }
        const int FRAME_WIDTH = 640;
        const int FRAME_HEIGHT = 480;
        const int COLOUR_DEPTH = 32;
        const bool FRAME_RESIZABLE = false;
        const bool FRAME_FULLSCREEN = false;
        const bool USE_OPENGL = false;
        const bool USE_HARDWARE = false;
        static Surface video;
        static Surface imgBackground;
        static Surface imgSpriteSheet;  
        enum Sprite
        {
            red, purple, yellow,
            blue1, blue2, green1, green2,
            star,
            rubble, stone, bricks,
            left_rubble, right_rubble, left_stone, right_stone,
            iron_mask, jade_mask, opal_mask, ruby_mask,
            brick_rubble, brick_stone, tile_stone,
            jade, ruby, opal,
            ice, fire,
            jar,
            smooth_rock, dented_rock, smashed_rock,
            black_galaxy, blue_galaxy,
            blue_ring,
            orb_fire, orb_water,
            totem, totem_wink,
            left, right, up, down,
            bat_small, bat_medium, bat_large,
            ball_small, ball_large,
            cross, flare, swirl, spiral,
            coin_00, coin_01, coin_02, coin_03,
            coin_04, coin_05, coin_06, coin_07,
            coin_08, coin_09, coin_10, coin_11,
            coin_12, coin_13, coin_14, coin_15
        };
        static Rectangle[] sprite_sheet_cut = new Rectangle[40 + 16 + 11];
    }
    
}
