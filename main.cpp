#include <raylib.h>
#include <deque>

constexpr unsigned int window_width{1200};
constexpr unsigned int window_height{900};

constexpr unsigned int floor{850};
constexpr unsigned int ceiling{50};
constexpr unsigned int left_wall{100};
constexpr unsigned int rigth_wall{1100};

constexpr unsigned int charlimit{90};
constexpr Vector2 origin{0, 0};

class Timer{
    public:

    double start_time;

    Timer(){start_time = GetTime();}

    int GetSeconds()const{return (int)(GetTime() - start_time);}

    void Reset(){
        start_time = GetTime();
    }
};

class Char{
    public:
    Texture2D chartexture;
    Vector2 pos{static_cast<float>(window_width) / 2.0f, static_cast<float>(floor-charlimit)};
    float scale{0.04f};
    int velocity{8};

    Char(){chartexture = LoadTexture("char.png");}
    ~Char(){UnloadTexture(chartexture);}

    void Draw()const{
        Rectangle source{0,0,(float)chartexture.width,(float)chartexture.height};
        Rectangle dest{pos.x,pos.y,(float)chartexture.width * scale,(float)chartexture.height * scale};
        DrawTexturePro(chartexture, source, dest, origin, 0, WHITE);
    }

    void Update(){
        if(IsKeyDown(KEY_RIGHT)){pos.x += velocity;}
        if(IsKeyDown(KEY_LEFT)){pos.x -= velocity;}
        if(IsKeyDown(KEY_UP)){pos.y -= velocity;}
        if(IsKeyDown(KEY_DOWN)){pos.y += velocity;}

        if(pos.x <= left_wall){pos.x=left_wall;}
        else if(pos.x >= rigth_wall - charlimit){pos.x = rigth_wall - charlimit;}
        if(pos.y <= ceiling){pos.y = ceiling;}
        else if(pos.y >= floor - charlimit){pos.y = floor - charlimit;}


    }

    void Reset(){
        pos.x = static_cast<float>(window_width) / 2.0f;
        pos.y = static_cast<float>(floor-charlimit);
    }

    Rectangle GetRect()const{
        return Rectangle{pos.x,pos.y,static_cast<float>(chartexture.width) * scale,static_cast<float>(chartexture.height) * scale};
    }
};



class Obstacle{
    public:
    bool startsleft{};
    Vector2 pos{};

    virtual void Draw()const{}

    virtual void Update(){}
};


class Obstacle_Rectangle: public Obstacle{
    public:

    int width{GetRandomValue(10, 100)};
    int height{GetRandomValue(10, 100)};
    int velocity{GetRandomValue(5, 10)};

    void Draw()const override{
        DrawRectangle(pos.x, pos.y, width, height, WHITE);
        };

    void Update()override{
        if(startsleft){
            pos.x += velocity;
        }
        else{pos.x -= velocity;}



    }
};

class Obstacle_Circle: public Obstacle{
    public:

    int rad{GetRandomValue(5, 20)};
    int velocity{GetRandomValue(5, 10)};


    void Draw()const override{
        DrawCircle(pos.x, pos.y, rad, WHITE);
    }

    void Update() override{
        if(startsleft){
            pos.x += velocity;
        }
        else{pos.x-=velocity;}
    }
};


bool check_collision_rect(const Char& character, const Obstacle_Rectangle& r){
    Rectangle rectangle{r.pos.x,r.pos.y,static_cast<float>(r.width),static_cast<float>(r.height)};
    return CheckCollisionRecs(rectangle, character.GetRect());
}

bool check_collision_circ(const Char& character, const Obstacle_Circle& c){
    return CheckCollisionCircleRec(c.pos, c.rad, character.GetRect());
}

int main(){
    InitWindow(window_width, window_height, "dodge");
    SetTargetFPS(60);
    Char character{};
    Timer timer{};

    const int max_size{5};
    std::deque<Obstacle_Rectangle> deq_r{};
    std::deque<Obstacle_Circle> deq_c{};
    

    while(!WindowShouldClose()){
        
        //Delete from deques
      for (int i = deq_r.size() - 1; i >= 0; --i) {
        if (deq_r[i].pos.x > window_width || deq_r[i].pos.x < 0) {
          deq_r.erase(deq_r.begin() + i);
        }
      }

      for (int i = deq_c.size() - 1; i >= 0; --i) {
        if (deq_c[i].pos.x > window_width || deq_c[i].pos.x < 0) {
          deq_c.erase(deq_c.begin() + i);
        }
      }


      //Add to deques
        if(deq_r.size() < max_size){
            Obstacle_Rectangle r;
            int rand{GetRandomValue(1, 2)};
            if(rand == 1){
                r.startsleft = true;
                r.pos.x = left_wall - 100;
                r.pos.y = GetRandomValue(ceiling + r.height, floor - r.height);
            }
            
            else{
                r.startsleft = false;
                r.pos.x = rigth_wall + 100 - r.width;
                r.pos.y = GetRandomValue(ceiling + r.height, floor - r.height);
            }

            deq_r.push_back(r);
        }

        if(deq_c.size() < max_size){
            Obstacle_Circle c;
            int rand{GetRandomValue(1, 2)};
            if(rand == 1){
                c.startsleft = true;
                c.pos.x = left_wall - 100;
                c.pos.y = GetRandomValue(ceiling + c.rad, floor - c.rad);
            }
            
            else{
                c.startsleft = false;
                c.pos.x = rigth_wall + 100 - c.rad;
                c.pos.y = GetRandomValue(ceiling + c.rad, floor - c.rad);
            }

            deq_c.push_back(c);
        }

        BeginDrawing();

        character.Update();
        for(auto& r: deq_r){r.Update();}
        for(auto& c: deq_c){c.Update();}

        //Check for collision:
        for(auto& r: deq_r){
            if(check_collision_rect(character, r)){
                timer.Reset();
                character.Reset();
            }
        }
        for(auto& c: deq_c){
            if(check_collision_circ(character, c)){
                timer.Reset();
                character.Reset();
            }
        }

        ClearBackground(BLACK);
        DrawLine(left_wall, floor, rigth_wall, floor, WHITE);
        DrawLine(left_wall, ceiling, rigth_wall, ceiling, WHITE);
        DrawLine(left_wall, ceiling, left_wall, floor, WHITE);
        DrawLine(rigth_wall, ceiling, rigth_wall, floor, WHITE);

        character.Draw();
        for(const auto& r : deq_r){r.Draw();}
        for(const auto& c : deq_c){c.Draw();}
        DrawText(TextFormat("Time: %d", timer.GetSeconds()), window_width/2 - 70, 10, 35, RAYWHITE);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
