#include <SFML/Graphics.hpp>
#include <time.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <time.h>
#include <sstream>

using sf::Sprite;
using sf::Vector2f;

const int size = 100; //  figures size 100px x 100px
const int bsize = 8;  // board size 8 x 8
const int numf = 24;  // board size 8 x 8

Sprite f[numf]; //figures 
std::string position = "";
std::pair<std::vector<std::string>, std::vector<std::string>> handleCapture;
Vector2f  startPos;
Vector2f  endPos; 

int zbite_piony = 0;
int wygrane = 0; 
int przegrane = 0;
int zbitewserii = 0;




enum class Player {
    player1,
    player2
};

Player Turn = Player::player1;
bool taken = false;
bool wastaken = false;

int board[bsize][bsize] = {
     0, 1, 0, 1, 0, 1, 0, 1,
     1, 0, 1, 0, 1, 0, 1, 0,
     0, 1, 0, 1, 0, 1, 0, 1,
     0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 
    -1, 0,-1, 0,-1, 0,-1, 0,
     0,-1, 0,-1, 0,-1, 0,-1,
    -1, 0,-1, 0,-1, 0,-1, 0
};

int restart[bsize][bsize] = {
     0, 1, 0, 1, 0, 1, 0, 1,
     1, 0, 1, 0, 1, 0, 1, 0,
     0, 1, 0, 1, 0, 1, 0, 1,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
    -1, 0,-1, 0,-1, 0,-1, 0,
     0,-1, 0,-1, 0,-1, 0,-1,
    -1, 0,-1, 0,-1, 0,-1, 0
};
void restarting() {
    for (int i = 0; i < bsize; i++)
        for (int j = 0; j < bsize; j++)
            board[i][j] = restart[i][j];
}

std::string inttoString(int x) {
    std::string ns;
    std::stringstream ss;
    ss.clear();
    ss << x;
    ss >> ns;
    return ns;
}
void show_board(int board[bsize][bsize] ) {
    for (int i = 0; i < bsize; i++) {
        for (int j = 0; j < bsize; j++) {
            std::cout << board[i][j] << " ";
        }
        std::cout << std::endl;
    }
}
Player newTurn() {

    if (Turn == Player::player1) {
        Turn = Player::player2;
        return Player::player2;
    }
   
   Turn =  Player::player1;
   return Player::player1;
}
char numtochar(int n) {
    switch (n) {
    case 0: return 'a';  break;
    case 1: return 'b';  break;
    case 2: return 'c';  break;
    case 3: return 'd';  break;
    case 4: return 'e';  break;
    case 5: return 'f';  break;
    case 6: return 'g';  break;
    case 7: return 'h';  break;
    case 8: return 'i';  break;
    case 9: return 'j';  break;
    }
}
int chartonum(char n) {
    switch (n) {
    case'a':case'A': return 0; break;
    case'b':case'B': return 1; break;
    case'c':case'C': return 2; break;
    case'd':case'D': return 3; break;
    case'e':case'E': return 4; break;
    case'f':case'F': return 5; break;
    case'g':case'G': return 6; break;
    case'h':case'H': return 7; break;
    case'i':case'I': return 8; break;
    case'j':case'J': return 9; break;
    }
}
std::string toChessNote(int y, int x) {
    std::string line; 
    line = numtochar(x);
    y = abs(bsize-1 - y);
    if ((char)(y + '1') == ':') line += "10";
    else line += (char)(y + '1'); 
    return line;
}
std::string toChessNote(Vector2f pos)
{
    std::string s = "";
    std::string ss;
    s += char(pos.x / size + 97);
    ss = char(bsize-1 - pos.y / size + '1');
    if (ss == ":") s += "10";
    else s += ss;
    return s;
}
bool goodCharInNum(int n) {
    switch (n) {
    case-1: case-2: {
        if(Turn == Player::player2)
            return true;
        return false;
    } break;
    case 1: case 2: {
        if (Turn == Player::player1)
            return true;
        return false;
    } break;

    default: return false;
    }
    return false;
}
bool range(int y, int x) {
    if (x >= 0 && x <= bsize-1 && y >= 0 && y <= bsize-1)
        return true;
    return false;
}
std::pair<int, int> possition(std::string Note) {
    std::pair<int, int> pos;
    if (Note.size() == 2) pos = std::make_pair(chartonum(Note[0]), (int)Note[1] - '1');
    else pos = std::make_pair(chartonum(Note[0]), 9);
    pos.second = abs(bsize - 1 - pos.second); // revers
    return pos;
}
std::pair<std::vector<std::string>,std::vector<std::string>> findPossibleCaptures(std::string chessNote,bool depth) {
    std::pair<int, int> pos = possition(chessNote);
    std::vector<std::string> captures;
    std::vector<std::string> probably;
    //std::cout << "In memory: \n x:" << pos.first << " y:" << pos.second << std::endl;
    int max_range;
    //std::cout << "max_range" << abs(board[pos.second][pos.first]) << std::endl;
    if (abs(board[pos.second][pos.first]) == 1) max_range = 1;
    else max_range = bsize;
    
    bool rn = true, ln = true, dr = true, dl = true;
    if (!range(pos.second, pos.first)) return std::make_pair(captures, probably);

    // do zrobienia dla wszystkich 

    for (int i = 1; i <= max_range; i++) { 
        /// prawy góra 
        if (range(pos.second - (i+1),pos.first + (i+1)) && rn) {
            if (board[pos.second - i][pos.first + i] != board[pos.second][pos.first] && goodCharInNum(board[pos.second - i][pos.first + i])) {
                rn = false;
                if (board[pos.second - (i + 1)][pos.first + (i + 1)] == 0) {
                    for (int j = 0; j < max_range; j++) {
                        if (range(pos.second - (j + i + 1), pos.first + (j + i + 1))) {
                            if (board[pos.second - (j + i + 1)][pos.first + (j + i + 1)] != 0) 
                                break;
                            
                            captures.emplace_back(toChessNote(pos.second - (j + i + 1), pos.first + (j + i + 1)));
                            probably.emplace_back(toChessNote(pos.second - i, pos.first + i));
                        }
                    }
                }
            }
        }
        /// lewa góra
        if (range(pos.second - (i + 1), pos.first - (i + 1)) && ln) {
            if (board[pos.second - i][pos.first - i] != board[pos.second][pos.first] && goodCharInNum(board[pos.second - i][pos.first - i])) {
                ln = false;
                if (board[pos.second - (i + 1)][pos.first - (i + 1)] == 0) {
                    for (int j = 0; j < max_range; j++) {
                        if (range(pos.second - (j + i + 1), pos.first - (j + i + 1))) {
                            if (board[pos.second - (j + i + 1)][pos.first - (j + i + 1)] != 0)
                                break;

                            captures.emplace_back(toChessNote(pos.second - (j + i + 1), pos.first - (j + i + 1)));
                            probably.emplace_back(toChessNote(pos.second - i, pos.first - i));
                        }
                    }
                }
            }
        }
        
        /// prawy dół
        if (range(pos.second + (i + 1), pos.first + (i + 1)) && dr) {
            if (board[pos.second + i][pos.first + i] != board[pos.second][pos.first] && goodCharInNum(board[pos.second + i][pos.first + i])) {
                dr = false;
                if (board[pos.second + (i + 1)][pos.first + (i + 1)] == 0) {
                    for (int j = 0; j < max_range; j++) {
                        if (range(pos.second + (j + i + 1), pos.first + (j + i + 1))) {
                            if (board[pos.second + (j + i + 1)][pos.first + (j + i + 1)] != 0)
                                break;

                            captures.emplace_back(toChessNote(pos.second + (j + i + 1), pos.first + (j + i + 1)));
                            probably.emplace_back(toChessNote(pos.second + i, pos.first + i));
                        }
                    }
                }
            }
        }

        /// lewy dół
        if (range(pos.second + (i + 1), pos.first - (i + 1)) && dl) {
            if (board[pos.second + i][pos.first - i] != board[pos.second][pos.first] && goodCharInNum(board[pos.second + i][pos.first - i])) {
                dl = false;
                if (board[pos.second + (i + 1)][pos.first - (i + 1)] == 0) {
                    for (int j = 0; j < max_range; j++) {
                        if (range(pos.second + (j + i + 1), pos.first - (j + i + 1))) {
                            if (board[pos.second + (j + i + 1)][pos.first - (j + i + 1)] != 0)
                                break;

                            captures.emplace_back(toChessNote(pos.second + (j + i + 1), pos.first - (j + i + 1)));
                            probably.emplace_back(toChessNote(pos.second + i, pos.first - i));
                        }
                    }
                }
            }
        }
    }
    if (depth) {
        std::vector<std::string> next_captures;
        std::vector<std::string> next_probably;
        std::pair<std::vector<std::string>, std::vector<std::string>> newCaptures;
        for (int i = 0; i < captures.size(); i++) {
            newCaptures = findPossibleCaptures(captures[i],false);
            if (newCaptures.first.size()) {
                    next_captures.emplace_back(captures[i]);
                    next_probably.emplace_back(probably[i]);
                    break;
            }
        }

        if (!next_captures.empty()) {
            captures = next_captures;
            probably = next_probably;
        }
    }


    return std::make_pair(captures,probably);
}       
std::vector<std::string> probably_move(Vector2f oldPos) {
    std::string chessNote = toChessNote(oldPos);
    std::pair<int, int> pos = possition(chessNote);
    std::vector<std::string> probably;

    bool rn = true, ln = true, dr = true, dl = true;
    int max_range;
    if (abs(board[pos.second][pos.first]) == 1) max_range = 1;
    else max_range = bsize + 1;
    //std::cout << "note: " << pos.second <<" " << pos.first << std::endl;


    //std::cout << "where put \n";

    for (int i = 1; i < max_range + 1; i++) {
        if (Turn != Player::player1 || abs(board[pos.second][pos.first]) == 2 ) {
            if (range(pos.second + i, pos.first + i) && rn) {
                if (board[pos.second + i][pos.first + i] == 0) {
                    //std::cout << toChessNote(pos.second + i, pos.first + i) << std::endl;
                    probably.emplace_back(toChessNote(pos.second + i, pos.first + i));
                    if (range(pos.second + (i + 1), pos.first + (i + 1)))
                        if (board[pos.second + (i + 1)][pos.first + (i + 1)] != 0)
                            rn = false;
                }
            }
            if (range(pos.second + i, pos.first - i) && ln) {
                if (board[pos.second + i][pos.first - i] == 0) {   
                    //std::cout << toChessNote(pos.second + i, pos.first - i) << std::endl;
                    probably.emplace_back(toChessNote(pos.second + i, pos.first - i));
                    if (range(pos.second + (i + 1), pos.first - (i + 1)))
                        if (board[pos.second + (i + 1)][pos.first - (i + 1)] != 0)
                            ln = false;
                }
            }
        }
        if (Turn != Player::player2 || abs(board[pos.second][pos.first]) == 2) {
            if (range(pos.second - i, pos.first + i) && dr) {
                if (board[pos.second - i][pos.first + i] == 0) {
                    //std::cout << toChessNote(pos.second - i, pos.first + i) << std::endl;
                    probably.emplace_back(toChessNote(pos.second - i, pos.first + i));
                    if (range(pos.second + (i + 1), pos.first + (i + 1)))
                        if (board[pos.second + (i + 1)][pos.first + (i + 1)] != 0)
                            dr = false;
                }
            }

            if (range(pos.second - i, pos.first - i) && dl) {
                if (board[pos.second - i][pos.first - i] == 0) {
                    //std::cout << toChessNote(pos.second - i, pos.first - i) << std::endl;
                    probably.emplace_back(toChessNote(pos.second - i, pos.first - i));
                    if (range(pos.second - (i + 1), pos.first - (i + 1)))
                        if (board[pos.second - (i + 1)][pos.first - (i + 1)] != 0)
                            dl = false;

                }
            }
        }
    }

    return probably;
}
Vector2f toCoord(char a, char b)
{
    int x = int(a) - 'a';
    int y;
    if (b != ':')  y = bsize - 1 - int(b) + '1';
    else  y = 10;

    return Vector2f(x * size, y * size);
}
void take_piece(std::string chessNote,Vector2f newPos) {
    std::pair<int, int> pos = possition(chessNote);
    board[pos.second][pos.first] = 0;
    if (findPossibleCaptures(toChessNote(newPos),true).first.size() > 0) {
        taken = true;
    }

    for (int i = 0; i < numf; i++) {
        if (f[i].getPosition() == toCoord(chessNote[0], chessNote[1])) {
            f[i].setPosition(15*size, 15*size);
            break;
        }
    }
}
void move_piece(std::string oldNote,std::string newNote)
{
    std::pair<int, int> oldPos = possition(oldNote);
    std::pair<int, int> newPos = possition(newNote);
   
    //std::cout << "newPos: " << newPos.second << " " << newPos.first << std::endl;
    //std::cout << "oldPos: " << oldPos.second << " " << oldPos.first << std::endl;
    board[newPos.second][newPos.first] = board[oldPos.second][oldPos.first];
    board[oldPos.second][oldPos.first] = 0;
    //show_board(board);
}
bool isCorectColor(std::string chessNote) {
    std::pair<int, int> pos = possition(chessNote);
    if (Turn == Player::player1 && (board[pos.second][pos.first] == 1 || board[pos.second][pos.first] == 2))
        return true;
    if (Turn == Player::player2 && (board[pos.second][pos.first] == -1 || board[pos.second][pos.first] == -2))
        return true;

    return false;
}
bool isFree(Vector2f newPos) {
    for (int i = 0; i < numf; i++)
        if (f[i].getPosition() == newPos) return false;
    return true;
}
bool PossibleCaptures(Vector2f oldPos, Vector2f newPos,bool& marker) {   
    auto safe = findPossibleCaptures(toChessNote(oldPos),true);
    bool flag = false;
    //show_board(board);
    std::cout << "Possible Capture: " << std::endl;
    for (int i = 0; i < handleCapture.first.size(); i++) {
        std::cout << handleCapture.first[i] << std::endl;
    }

    for (int i = 0; i < safe.first.size(); i++) {
        for (int j = 0; j < handleCapture.first.size(); j++) {
            if (safe.first[i] == handleCapture.first[j]) {
                flag = true;
            }
        }
    }
    

    if (!flag) return true;
    
    for (int i = 0; i < handleCapture.first.size(); i++) {
        if (toChessNote(newPos) == handleCapture.first[i]) {
            //std::cout << "where should be taken: " << handleCapture.second[i] << std::endl;
            take_piece(handleCapture.second[i],newPos);
            //std::cout << "brawo!" << std::endl;
            marker = true;
            return false;
        }
    }
     return true;
}
std::pair<std::vector<std::string>,std::vector<std::string>> findAllCaptures(int begin,int end) {
    std::pair<std::vector<std::string>, std::vector<std::string>> base;
    std::pair<std::vector<std::string>, std::vector<std::string>> near;
    for (int j = begin; j < end; j++) {
        base = findPossibleCaptures(toChessNote(f[j].getPosition()),true);
        if (base.first.size() > 0) {
            std::cout <<"probably: " << toChessNote(f[j].getPosition()) << std::endl;
            for (int i = 0; i < base.first.size(); i++) {
                near.first.emplace_back(base.first[i]);
                near.second.emplace_back(base.second[i]);
            }
        }
    }
    return near;
}
bool isCorect(Vector2f oldPos, Vector2f newPos) {
    if (!isFree(newPos)) return false;
    if (newPos == oldPos) return false;
    if (isCorectColor(toChessNote(oldPos))) return false;

    //searching probably captures
    bool marker = false;

    if (handleCapture.first.size() > 0) { // king niby poprawiony 
        if (PossibleCaptures(oldPos,newPos,marker)) return false;
    }

    if (marker) return true;




    // where we can put piece's
    auto place = probably_move(oldPos);

    for (int i = 0; i < place.size(); i++) {
        if (toChessNote(newPos) == place[i]) return true;
    }

    // where we can't put piece's
    int j = 0;
    for (int i = 0; i < bsize; i++) {
        !(i % 2) ? j = 0 : j = 1;
        for (j; j < bsize; j += 2)
        {
            if (Vector2f(i * size, j * size) == newPos) return false;
        }
    }

    return false;
    
}
void promotion(std::string chessNote,sf::Texture& t4, sf::Texture& t5) {
    std::pair<int, int> pos = possition(chessNote);
    

    if (Turn == Player::player1) {
        if (possition(chessNote).second == 0) {
            for (int i = numf/2; i < numf; i++) {
                if (toChessNote(f[i].getPosition()) == chessNote){
                    f[i].setTexture(t4);
                    board[pos.second][pos.first] = -2;
                    //std::cout << "zamiana miejsca:" << std::endl;
                    break;
                }
            }
        }
    }
    else {
        if (possition(chessNote).second == bsize-1) {
            for (int i = 0; i < numf/2; i++) {
                if (toChessNote(f[i].getPosition()) == chessNote) {

                    f[i].setTexture(t5);
                    board[pos.second][pos.first] = 2;
                    break;
                }
            }
        }
    }
    //show_board(board);
}
bool isEND() {
    if (Turn == Player::player1) {
        for (int i = numf/2; i < numf; i++) {
            auto whe = probably_move(f[i].getPosition());
            auto cap = findPossibleCaptures(toChessNote(f[i].getPosition()),true).first;
            if (whe.size() > 0) return false;
            if (cap.size() > 0) return false;
        }
    }
    else {
        for (int i = 0; i < numf/2; i++) {
            auto whe = probably_move(f[i].getPosition());
            auto cap = findPossibleCaptures(toChessNote(f[i].getPosition()),true).first;
            if (whe.size() > 0) return false;
            if (cap.size() > 0) return false;
        }
    }

    return true;
}
void loadPosition()
{
    int k = 0;
    for (int i = 0; i < bsize; i++)
        for (int j = 0; j < bsize; j++)
        {
            int n = board[i][j];
            if (!n) continue;
            f[k].setPosition(size * j, size * i);
            k++;
        }
}
std::vector<int> score_load() {
    std::fstream file;
    std::vector<int> set;
    file.open("score.txt", std::ios::in);
    int zp = 0, sp = 0, w = 0, p = 0;
    if (!file.good()) {
        zp = 0;
        sp = 0;
        w = 0;
        p = 0;
    }
    else  if (!(file >> zp >> sp >> w >> p)) {
        zp = 0;
        sp = 0;
        w = 0;
        p = 0;
    }
    
        
    set.emplace_back(zp);
    set.emplace_back(sp);
    set.emplace_back(w);
    set.emplace_back(p);
    file.close();
    return set;
}
void score_save() {
    std::fstream file;
    file.open("score.txt", std::ios::out);
    file << zbite_piony << " " << zbitewserii << " " << wygrane << " " << przegrane << std::endl;
    file.close();
    file << "this is score" << std::endl;
}

int main()
{   
    

    /// loading texture to checkers
    sf::Texture t1, t2, t3, t4, t5;
    t1.loadFromFile("graphics/pawn_red.png");
    t2.loadFromFile("graphics/Board.png");
    t3.loadFromFile("graphics/pawn_white.png");
    t4.loadFromFile("graphics/King_white.png");
    t5.loadFromFile("graphics/King_red.png");

    sf::Sprite sBoard(t2);
    srand(time(NULL));
    

    /// loading menu 
    sf::Texture tt1,tt2,tt3,tt4,tt5;
    tt1.loadFromFile("graphics/tlo.png");
    tt2.loadFromFile("graphics/Button_play.png");
    tt3.loadFromFile("graphics/Button_play_with_friends.png");
    tt4.loadFromFile("graphics/Button_score.png");
    tt5.loadFromFile("graphics/Button_Back.png");

    sf::Text txt[5];
    sf::Font font;
    font.loadFromFile("graphics/Lato-Bold.ttf");
    for (int i = 0; i < 5; i++) {
        txt[i].setCharacterSize(24);
        txt[i].setFillColor(sf::Color::Red);
        txt[i].setCharacterSize(24);
        txt[i].setFont(font);
        txt[i].setStyle(sf::Text::Bold);
    }
    txt[0].setPosition(100, 300);
    txt[1].setPosition(100, 350);
    txt[2].setPosition(100, 400);
    txt[3].setPosition(100, 450);
    txt[4].setPosition(100, 500);

    sf::Sprite background(tt1);
    sf::Sprite Button[4] = { sf::Sprite(tt2),sf::Sprite(tt3),sf::Sprite(tt4),sf::Sprite(tt5)};

    Button[0].setPosition(200, 350);
    Button[1].setPosition(200, 500);
    Button[2].setPosition(200, 650);
    Button[3].setPosition(200, 650);
    
    int seria = 0;
    score_save();
    bool finish = false;
    while (!finish) {
        sf::RenderWindow window(sf::VideoMode(bsize * size, bsize * size), "Warcaby!");
        for (int i = 0; i < numf / 2; i++) f[i].setTexture(t1);
        for (int i = numf / 2; i < numf; i++) f[i].setTexture(t3);
        bool end = false;
        loadPosition();

        bool isMove = false;
        float dx = 0, dy = 0;
        Vector2f oldPos, newPos;
        std::string str;
        int n = 0;
        int option = 4;
        restarting();

        while (window.isOpen()) {

            sf::Event mainEvent;

            while (window.pollEvent(mainEvent)) {
                sf::Vector2i mousepos = sf::Mouse::getPosition(window);

                if (mainEvent.type == sf::Event::Closed) {
                    window.close();
                    finish = true;
                }

                /// you have choice computer or frends 
                if (mainEvent.type == sf::Event::MouseButtonPressed)
                    if (mainEvent.key.code == sf::Mouse::Left) {
                        if (Button[1].getGlobalBounds().contains(mousepos.x, mousepos.y)) {
                            option = 1;
                        }
                        if (Button[0].getGlobalBounds().contains(mousepos.x, mousepos.y)) {
                            option = 2;
                        }
                        if (Button[2].getGlobalBounds().contains(mousepos.x, mousepos.y)) {
                            option = 3;
                        }
                    }

                if (mainEvent.type == sf::Event::KeyPressed)
                    if (mainEvent.key.code == sf::Keyboard::Enter) {
                        option = 1;
                    }

                if (mainEvent.type == sf::Event::KeyPressed)
                    if (mainEvent.key.code == sf::Keyboard::Space) {
                        option = 2;
                    }
                if (mainEvent.type == sf::Event::KeyPressed)
                    if (mainEvent.key.code == sf::Keyboard::LShift || mainEvent.key.code == sf::Keyboard::RShift) {
                        option = 3;
                    }

                switch (option) {
                case 1: {
                    while (window.isOpen() && !end)
                    {// achivementy do zrobienia 

                        sf::Vector2i pos = sf::Mouse::getPosition(window);

                        sf::Event event;
                        while (window.pollEvent(event))
                        {
                            if (event.type == sf::Event::Closed) {
                                window.close();
                                finish = true;
                            }
                            
                            if (isEND() && !isMove) {
                                if (Turn == Player::player2) std::cout << "wygrywa: gracz numer 1";
                                else std::cout << "wygrywa: gracz numer 2";
                                window.close();
                                end = true;
                            }
                            /////drag and drop///////
                            if (event.type == sf::Event::MouseButtonPressed)
                                if (event.key.code == sf::Mouse::Left) {
                                    if (Turn == Player::player1) std::cout << "Turn: player1" << std::endl;
                                    else std::cout << "Turn: player2" << std::endl;

                                    if (Turn == Player::player1) {
                                        handleCapture = findAllCaptures(numf / 2, numf);
                                    }
                                    if (Turn == Player::player2) {
                                        handleCapture = findAllCaptures(0, numf / 2);
                                    }

                                    for (int i = 0; i < numf; i++)
                                        if (f[i].getGlobalBounds().contains(pos.x, pos.y))
                                        {
                                            isMove = true; n = i;
                                            dx = pos.x - f[i].getPosition().x;
                                            dy = pos.y - f[i].getPosition().y;
                                            oldPos = f[i].getPosition();
                                            //std::cout << "pos: " << toChessNote(oldPos) << std::endl;
                                            break;
                                        }
                                }

                            if (event.type == sf::Event::MouseButtonReleased)
                                if (event.key.code == sf::Mouse::Left)
                                {
                                    if (isMove) {
                                        isMove = false;
                                        Vector2f p = f[n].getPosition() + Vector2f(size / 2, size / 2);
                                        newPos = Vector2f(size * int(p.x / size), size * int(p.y / size));
                                        //std::cout << "newPos: " << toChessNote(newPos) << std::endl;

                                        if (isCorect(oldPos, newPos)) {
                                            str = toChessNote(oldPos) + '-' + toChessNote(newPos);
                                            std::cout << str << std::endl;
                                            move_piece(toChessNote(oldPos), toChessNote(newPos));
                                            f[n].setPosition(newPos);
                                            if (!taken) {
                                                promotion(toChessNote(newPos), t4, t5);
                                                wastaken = false;
                                                newTurn();
                                                //show_board(board);
                                            }
                                            if (!wastaken)
                                                startPos = oldPos;

                                            promotion(toChessNote(newPos), t4, t5);
                                            taken = false;
                                            wastaken = true;
                                        }
                                        else
                                        {
                                            isMove = false;
                                            f[n].setPosition(oldPos);
                                        }
                                    }
                                }
                        }
                        if (isMove) f[n].setPosition(pos.x - dx, pos.y - dy);

                        ////// draw  ///////
                        window.clear();
                        window.draw(sBoard);
                        for (int i = 0; i < numf; i++) window.draw(f[i]);
                        window.draw(f[n]);
                        window.display();
                    }
                }break;
                case 2: {
                    while (window.isOpen() && !end)
                    {// achivementy do zrobienia 
                        sf::Vector2i pos = sf::Mouse::getPosition(window);
                        sf::Event event;
                        while (window.pollEvent(event))
                        {
                            if (event.type == sf::Event::Closed) {
                                window.close();
                                finish = true;
                            }
                            
                            if (isEND() && !isMove) {
                                if (Turn == Player::player2) {
                                    std::cout << "Player Win";
                                    wygrane++;
                                }
                                else {
                                    std::cout << "Computer Win";
                                    przegrane++;
                                }
                                score_save();
                                window.close();
                                end = true;
                                break;
                            }

                            /////drag and drop///////
                            if (Turn == Player::player1) {
                                if (event.type == sf::Event::MouseButtonPressed)
                                    if (event.key.code == sf::Mouse::Left) {
                                        handleCapture = findAllCaptures(numf / 2, numf);

                                        for (int i = 0; i < numf; i++)
                                            if (f[i].getGlobalBounds().contains(pos.x, pos.y))
                                            {
                                                isMove = true; n = i;
                                                dx = pos.x - f[i].getPosition().x;
                                                dy = pos.y - f[i].getPosition().y;
                                                oldPos = f[i].getPosition();
                                                //std::cout << "pos: " << toChessNote(oldPos) << std::endl;
                                                break;
                                            }
                                    }

                                if (event.type == sf::Event::MouseButtonReleased)
                                    if (event.key.code == sf::Mouse::Left)
                                    {
                                        if (isMove) {
                                            isMove = false;
                                            Vector2f p = f[n].getPosition() + Vector2f(size / 2, size / 2);
                                            newPos = Vector2f(size * int(p.x / size), size * int(p.y / size));
                                            //std::cout << "newPos: " << toChessNote(newPos) << std::endl;

                                            if (isCorect(oldPos, newPos)) {
                                                str = toChessNote(oldPos) + '-' + toChessNote(newPos);
                                                std::cout << str << std::endl;
                                                move_piece(toChessNote(oldPos), toChessNote(newPos));
                                                f[n].setPosition(newPos);
                                                if (!taken) {
                                                    promotion(toChessNote(newPos), t4, t5);
                                                    newTurn();
                                                    
                                                    wastaken = false;
                                                    seria = 0;
                                                    //show_board(board);
                                                }
                                                if (!wastaken)
                                                    startPos = oldPos;
                                                if (taken) {
                                                    seria++;
                                                    if (seria > zbitewserii)
                                                        zbitewserii = seria;
                                                }

                                                promotion(toChessNote(newPos), t4, t5);
                                                taken = false;
                                                zbite_piony++;
                                            }
                                            else
                                            {
                                                isMove = false;
                                                f[n].setPosition(oldPos);
                                            }
                                        }
                                    }
                            }
                            ///////////// computer move //////////////////
                            //if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
                            if (Turn == Player::player2)
                            {
                                bool szybcik = true;
                                for (int i = 0; i < bsize; i++)
                                    for (int j = 0; j < bsize; j++)
                                        if (board[i][j] == 1 || board[i][j] == 2)
                                            szybcik = false;
                                if (szybcik)
                                    break;

                                int ran;
                                bool flag = false;
                                handleCapture = findAllCaptures(0, numf / 2);
                                if (handleCapture.first.size()) {
                                    ran = rand() % handleCapture.first.size();
                                    //std::cout << handleCapture.first[ran][0] << handleCapture.first[ran][1] << std::endl;
                                    if (handleCapture.first[ran].size() == 2) newPos = toCoord(handleCapture.first[ran][0], handleCapture.first[ran][1]);
                                    else newPos = toCoord(handleCapture.first[ran][0], ':');

                                    for (int i = 0; i < numf / 2; i++) {
                                        auto base = findPossibleCaptures(toChessNote(f[i].getPosition()), true);
                                        if (base.first.size()) {
                                            for (int j = 0; j < base.first.size(); j++) {
                                                if (base.first[j] == handleCapture.first[ran]) {
                                                    oldPos = f[i].getPosition();
                                                    f[i].setPosition(newPos);
                                                    take_piece(handleCapture.second[ran], newPos);
                                                    move_piece(toChessNote(oldPos), toChessNote(newPos));
                                                    flag = true;
                                                    break;
                                                }
                                            }
                                        }
                                        if (flag)
                                            break;
                                    }
                                }
                                else {
                                    std::vector<std::string> put;
                                    for (int i = 0; i < numf / 2; i++) {
                                        auto base = probably_move(f[i].getPosition());
                                        if (base.size())
                                            for (int j = 0; j < base.size(); j++)
                                                put.emplace_back(base[j]);
                                    }

                                    ran = rand() % put.size();
                                    if (put[ran].size() == 2) newPos = toCoord(put[ran][0], put[ran][1]);
                                    else newPos = toCoord(put[ran][0], ':');

                                    bool flag = false;
                                    for (int i = 0; i < numf / 2; i++) {
                                        auto base = probably_move(f[i].getPosition());
                                        if (base.size() > 0) {
                                            for (int j = 0; j < base.size(); j++) {
                                                if (base[j] == put[ran]) {
                                                    oldPos = f[i].getPosition();
                                                    f[i].setPosition(newPos);
                                                    flag = true;
                                                    break;
                                                }
                                            }
                                        }
                                        if (flag)
                                            break;
                                    }
                                    // std::cout << toChessNote(oldPos) << "-" << toChessNote(newPos) << std::endl;
                                    move_piece(toChessNote(oldPos), toChessNote(newPos));
                                }
                                promotion(toChessNote(newPos), t4, t5);
                                if (!taken)
                                    newTurn();

                                taken = false;
                                //show_board(board);
                            }
                        }



                        if (isMove) f[n].setPosition(pos.x - dx, pos.y - dy);

                        ////// draw  ///////
                        window.clear();
                        window.draw(sBoard);
                        for (int i = 0; i < numf; i++) window.draw(f[i]);
                        window.draw(f[n]);
                        window.display();
                    }
                }break;
                case 3: {

                    auto base = score_load();
                    //std::cout <<"\nScore Gry z Botem: "<< std::endl;
                    //std::cout <<"liczba zbitych pionkow: " << base[0] << std::endl;
                    //std::cout <<"liczba zbitych pionkow w serii: " << base[1] << std::endl;
                    //std::cout <<"liczba wygranych: " << base[2] << std::endl;
                    //std::cout <<"liczba przegranych: " << base[3] << std::endl;
                        option = 4;

                            txt[0].setString("Score poprzedniej gry z Botem: ");
                            txt[1].setString("liczba zbitych pionkow: " + inttoString(base[0]));
                            txt[2].setString("liczba zbitych pionkow w serii: " + inttoString(base[1]));
                            txt[3].setString("liczba wygranych: " + inttoString(base[2]));
                            txt[4].setString("liczba przegranych: " + inttoString(base[3]));

                        while (window.isOpen()&& !end) {
                            
                            sf::Event mainEvent;

                            while (window.pollEvent(mainEvent)) {
                                sf::Vector2i mousepos = sf::Mouse::getPosition(window);

                                if (mainEvent.type == sf::Event::Closed) {
                                    window.close();
                                    finish = true;
                                }
                                if (mainEvent.type == sf::Event::MouseButtonPressed)
                                    if (mainEvent.key.code == sf::Mouse::Left) {
                                        if (Button[3].getGlobalBounds().contains(mousepos.x, mousepos.y)) {
                                            window.close();
                                        }
                                    }
                            }

                            window.clear();
                            window.draw(background);
                            for (int i = 0; i < 5; i++)
                                window.draw(txt[i]);
                            window.draw(Button[3]);
                            window.display();
                        }
                } break;
                }
                window.clear();
                window.draw(background);
                window.draw(Button[0]);
                window.draw(Button[1]);
                window.draw(Button[2]);
                window.display();
            }
        }
    }

    return 0;
}
