/* BlindFold Chess Practice Tool
Players move by writing the move in an explicit way (see chess algebraic notation).

When two (or more) identical pieces can move to the same square, the moving piece is uniquely identified by stating the file or the rank (i.e. Nbd2).

If a chess move is not valid, it will not take place.

The program can understand automatically Checkmate and Stalemate positions.

You can choose a position by specifying an initial setup of the pieces with FEN notation. Default is the standard position. 

Here, in order to see what you do, the chessboard gets printed after each move. You can disable that to practice blindfold chess understanding.

Although there is still some work to be done as far as draw rules and undo option is concerned, the game follows every rule, even the complicated ones like en passant, castling, promotion and their prerequisites. 


August 2021 by Dion Adam
*/


#include <iostream>
#include <utility> 
#include <vector>
#include <algorithm>

using namespace std;

#define pci pair<char, int>
#define file position.first
#define rank position.second
#define def "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -"

enum Color{white, black};

class Piece;
class chessboard;
void move(pci initial_position, pci position, chessboard& B);

class chessboard{
public:
    chessboard();
    chessboard(chessboard &b);
    ~chessboard();
    Piece*& access(pci position);
    void setup(const string &s=def);
    friend ostream& operator << (ostream& out, chessboard& b); 
    class Player{
    public:
        Player() {}
        pci king;
        pair<pci, pci> lastmove={{' ', 0}, {' ', 0}};
        bool shortcastleright=false;
        bool longcastleright=false;
    };
    Player white_player;
    Player black_player;

    Player& returnPlayer(Color c){
        if(c==white)
            return white_player;
        else
            return black_player;
    }
    void play();
    Color to_play;

private:
    Piece* square[8][8];
};

chessboard:: chessboard(){
    for(int i=0; i<8; i++)
            for(int j=0; j<8; j++) square[i][j]=nullptr;
}

Piece*& chessboard:: access(pci position){
    if(file < 'a' || file>'h' || rank<1 || rank>8)
        throw out_of_range("invalid index");
    return square[file-'a'][rank-1]; 
}

/////////////////////////////////////////////////////////

class Piece{
public:
    Piece(pci initial_position, Color c): position(initial_position), c(c) {}
    pci position;
    Color c;
    char label;
    vector<pci> moves;
    vector<pci> checked_moves;
    virtual void moveable_to(chessboard &b)=0;
    bool is_in_danger(chessboard &b){
          for(char i='a'; i<='h'; i++)
            for(int j=1; j<=8; j++){
                bool g=false;
                Piece* x=b.access({i, j});
                if(x!=nullptr && x->c!=c){
                    x->moveable_to(b);
                    if(find(x->moves.begin(), x->moves.end(), position)!=x->moves.end())
                        g=true;
                    x->moves.clear();
                }
                if(g) return true;
            }
        return false;
    }
    void checkmoves(chessboard &b){
        for(int i=0; i<moves.size(); i++){
            chessboard b1(b);
            move(position, moves[i], b1);
            if(!b1.access(b1.returnPlayer(c).king)->is_in_danger(b1))
                checked_moves.push_back(moves[i]);
        }
    }
};

class Pawn: public Piece {
public:
    Pawn(pci initial_position, Color c): Piece(initial_position, c){label='p';}
    void moveable_to(chessboard &b) override {
        if(c==white){
            if(b.access({file, rank+1})==nullptr){    
                moves.push_back({file, rank+1});
                if(rank==2 && b.access({file, rank+2})==nullptr)
                    moves.push_back({file, rank+2});    
            }
            try{
                Piece* capture1=b.access({file+1, rank+1});
                if(capture1!=nullptr && capture1->c==black)
                    moves.push_back({file+1, rank+1});
            } catch(out_of_range){}
            try{
                Piece* capture2=b.access({file-1, rank+1});
                if(capture2!=nullptr && capture2->c==black)
                    moves.push_back({file-1, rank+1});
            } catch(out_of_range){}

            if(rank==5){
                try{
                Piece* capture1=b.access({file+1, 5});
                pci pos1={file+1, 7};
                pci pos2={file+1, 5};
                if(capture1!=nullptr && capture1->label=='p' && capture1->c==black && b.black_player.lastmove==make_pair(pos1, pos2))
                    moves.push_back({file+1, 6});
                } catch(out_of_range){}
                try{
                Piece* capture2=b.access({file-1, 5});
                pci pos1={file-1, 7};
                pci pos2={file-1, 5};
                if(capture2!=nullptr && capture2->label=='p' && capture2->c==black && b.black_player.lastmove==make_pair(pos1, pos2))
                    moves.push_back({file-1, 6});
                } catch(out_of_range){}
            }
        }
        else{
            if(b.access({file, rank-1})==nullptr){
                moves.push_back({file, rank-1});
                if(rank==7 && b.access({file, rank-2})==nullptr)
                    moves.push_back({file, rank-2});  
            }
            try{
                Piece* capture1=b.access({file+1, rank-1});
                if(capture1!=nullptr && capture1->c==white)
                    moves.push_back({file+1, rank-1});
            } catch(out_of_range){}

            try{
                Piece* capture2=b.access({file-1, rank-1});
                if(capture2!=nullptr && capture2->c==white)
                    moves.push_back({file-1, rank-1});
            } catch(out_of_range){}
            if(rank==4){
                try{
                Piece* capture1=b.access({file+1, 4});
                pci pos1={file+1, 2};
                pci pos2={file+1, 4};
                if(capture1!=nullptr && capture1->label=='p' && capture1->c==white && b.white_player.lastmove==make_pair(pos1, pos2))
                    moves.push_back({file+1, 3});
                } catch(out_of_range){}
                try{
                Piece* capture2=b.access({file-1, 4});
                pci pos1={file-1, 2};
                pci pos2={file-1, 4};
                if(capture2!=nullptr && capture2->label=='p' && capture2->c==white && b.white_player.lastmove==make_pair(pos1, pos2))
                    moves.push_back({file-1, 3});
                } catch(out_of_range){}
            }
        }
    }
};

class Rook: public Piece{
public:
    Rook(pci initial_position, Color c): Piece(initial_position, c){label='R';}
    void moveable_to(chessboard &b) override {
        char col=file;
        int row=rank;
        try{
            while(1){
                row++;
                Piece* x=b.access({col, row});
                if(x==nullptr)
                    moves.push_back({col, row});
                else{
                    if(x->c!=c)
                        moves.push_back({col, row});
                    break;
                }
            } 
        }catch(out_of_range){}
        col=file;
        row=rank;
        try{
            while(1){
                col++;
                Piece* x=b.access({col, row});
                if(x==nullptr)
                    moves.push_back({col, row});
                else{
                    if(x->c!=c)
                        moves.push_back({col, row});
                    break;
                }
            } 
        }catch(out_of_range){}
        col=file;
        row=rank;
        try{
            while(1){
                row--;
                Piece* x=b.access({col, row});
                if(x==nullptr)
                    moves.push_back({col, row});
                else{
                    if(x->c!=c)
                        moves.push_back({col, row});
                    break;
                }
            } 
        }catch(out_of_range){}
        col=file;
        row=rank;
        try{
            while(1){
                col--;
                Piece* x=b.access({col, row});
                if(x==nullptr)
                    moves.push_back({col, row});
                else{
                    if(x->c!=c)
                        moves.push_back({col, row});
                    break;
                }
            } 
        }catch(out_of_range){}
    }
};

class Bishop: public Piece{
public:
    Bishop(pci initial_position, Color c): Piece(initial_position, c){label='B';}
    void moveable_to(chessboard &b) override {
        char col=file;
        int row=rank;
        try{
            while(1){
                row++;
                col++;
                Piece* x=b.access({col, row});
                if(x==nullptr)
                    moves.push_back({col, row});
                else{
                    if(x->c!=c)
                        moves.push_back({col, row});
                    break;
                }
            } 
        }catch(out_of_range){}
        col=file;
        row=rank;
        try{
            while(1){
                row++;
                col--;
                Piece* x=b.access({col, row});
                if(x==nullptr)
                    moves.push_back({col, row});
                else{
                    if(x->c!=c)
                        moves.push_back({col, row});
                    break;
                }
            } 
        }catch(out_of_range){}
        col=file;
        row=rank;
        try{
            while(1){
                row--;
                col++;
                Piece* x=b.access({col, row});
                if(x==nullptr)
                    moves.push_back({col, row});
                else{
                    if(x->c!=c)
                        moves.push_back({col, row});
                    break;
                }
            } 
        }catch(out_of_range){}
        col=file;
        row=rank;
        try{
            while(1){
                row--;
                col--;
                Piece* x=b.access({col, row});
                if(x==nullptr)
                    moves.push_back({col, row});
                else{
                    if(x->c!=c)
                        moves.push_back({col, row});
                    break;
                }
            } 
        }catch(out_of_range){}
    }
};

class Knight:public Piece{
public:
    Knight(pci initial_position, Color c): Piece(initial_position, c){label='N';}
    void moveable_to(chessboard &b) override {
        char col;
        int row;
        try{
            col=file+2;
            row=rank+1;
            Piece* x=b.access({col, row});
            if(x==nullptr || x->c!=c)
                moves.push_back({col, row});
        }catch(out_of_range){}
        try{
            col=file+1;
            row=rank+2;
            Piece* x=b.access({col, row});
            if(x==nullptr || x->c!=c)
                moves.push_back({col, row});
        }catch(out_of_range){}
        try{
            col=file-2;
            row=rank+1;
            Piece* x=b.access({col, row});
            if(x==nullptr || x->c!=c)
                moves.push_back({col, row});
        }catch(out_of_range){}
        try{
            col=file+1;
            row=rank-2;
            Piece* x=b.access({col, row});
            if(x==nullptr || x->c!=c)
                moves.push_back({col, row});
        }catch(out_of_range){}
        try{
            col=file+2;
            row=rank-1;
            Piece* x=b.access({col, row});
            if(x==nullptr || x->c!=c)
                moves.push_back({col, row});
        }catch(out_of_range){}
        try{
            col=file-1;
            row=rank+2;
            Piece* x=b.access({col, row});
            if(x==nullptr || x->c!=c)
                moves.push_back({col, row});
        }catch(out_of_range){}
        try{
            col=file-2;
            row=rank-1;
            Piece* x=b.access({col, row});
            if(x==nullptr || x->c!=c)
                moves.push_back({col, row});
        }catch(out_of_range){}
        try{
            col=file-1;
            row=rank-2;
            Piece* x=b.access({col, row});
            if(x==nullptr || x->c!=c)
                moves.push_back({col, row});
        }catch(out_of_range){}
    }
};

class Queen:public Piece{
public:
    Queen(pci initial_position, Color c): Piece(initial_position, c){label='Q';}
    void moveable_to(chessboard &b) override {
        char col=file;
        int row=rank;
        try{
            while(1){
                row++;
                Piece* x=b.access({col, row});
                if(x==nullptr)
                    moves.push_back({col, row});
                else{
                    if(x->c!=c)
                        moves.push_back({col, row});
                    break;
                }
            } 
        }catch(out_of_range){}
        col=file;
        row=rank;
        try{
            while(1){
                col++;
                Piece* x=b.access({col, row});
                if(x==nullptr)
                    moves.push_back({col, row});
                else{
                    if(x->c!=c)
                        moves.push_back({col, row});
                    break;
                }
            } 
        }catch(out_of_range){}
        col=file;
        row=rank;
        try{
            while(1){
                row--;
                Piece* x=b.access({col, row});
                if(x==nullptr)
                    moves.push_back({col, row});
                else{
                    if(x->c!=c)
                        moves.push_back({col, row});
                    break;
                }
            } 
        }catch(out_of_range){}
        col=file;
        row=rank;
        try{
            while(1){
                col--;
                Piece* x=b.access({col, row});
                if(x==nullptr)
                    moves.push_back({col, row});
                else{
                    if(x->c!=c)
                        moves.push_back({col, row});
                    break;
                }
            } 
        }catch(out_of_range){}
        col=file;
        row=rank;
        try{
            while(1){
                row++;
                col++;
                Piece* x=b.access({col, row});
                if(x==nullptr)
                    moves.push_back({col, row});
                else{
                    if(x->c!=c)
                        moves.push_back({col, row});
                    break;
                }
            } 
        }catch(out_of_range){}
        col=file;
        row=rank;
        try{
            while(1){
                row++;
                col--;
                Piece* x=b.access({col, row});
                if(x==nullptr)
                    moves.push_back({col, row});
                else{
                    if(x->c!=c)
                        moves.push_back({col, row});
                    break;
                }
            } 
        }catch(out_of_range){}
        col=file;
        row=rank;
        try{
            while(1){
                row--;
                col++;
                Piece* x=b.access({col, row});
                if(x==nullptr)
                    moves.push_back({col, row});
                else{
                    if(x->c!=c)
                        moves.push_back({col, row});
                    break;
                }
            } 
        }catch(out_of_range){}
        col=file;
        row=rank;
        try{
            while(1){
                row--;
                col--;
                Piece* x=b.access({col, row});
                if(x==nullptr)
                    moves.push_back({col, row});
                else{
                    if(x->c!=c)
                        moves.push_back({col, row});
                    break;
                }
            } 
        }catch(out_of_range){}
    }
};

class King:public Piece{
    public:
    King(pci initial_position, Color c): Piece(initial_position, c){label='K';}
    void moveable_to(chessboard &b) override {
        char col;
        int row;
        try{
            col=file;
            row=rank+1;
            Piece* x=b.access({col, row});
            if(x==nullptr || x->c!=c)
                moves.push_back({col, row});
        }catch(out_of_range){}
        try{
            col=file+1;
            row=rank;
            Piece* x=b.access({col, row});
            if(x==nullptr || x->c!=c)
                moves.push_back({col, row});
        }catch(out_of_range){}
        try{
            col=file;
            row=rank-1;
            Piece* x=b.access({col, row});
            if(x==nullptr || x->c!=c)
                moves.push_back({col, row});
        }catch(out_of_range){}
        try{
            col=file-1;
            row=rank;
            Piece* x=b.access({col, row});
            if(x==nullptr || x->c!=c)
                moves.push_back({col, row});
        }catch(out_of_range){}
        try{
            col=file+1;
            row=rank+1;
            Piece* x=b.access({col, row});
            if(x==nullptr || x->c!=c)
                moves.push_back({col, row});
        }catch(out_of_range){}
        try{
            col=file-1;
            row=rank+1;
            Piece* x=b.access({col, row});
            if(x==nullptr || x->c!=c)
                moves.push_back({col, row});
        }catch(out_of_range){}
        try{
            col=file+1;
            row=rank-1;
            Piece* x=b.access({col, row});
            if(x==nullptr || x->c!=c)
                moves.push_back({col, row});
        }catch(out_of_range){}
        try{
            col=file-1;
            row=rank-1;
            Piece* x=b.access({col, row});
            if(x==nullptr || x->c!=c)
                moves.push_back({col, row});
        }catch(out_of_range){}
    }
};

//////////////////////////////////////////////////////////////////////////

chessboard:: ~chessboard(){   
    for(int i=0; i<8; i++)
        for(int j=0; j<8; j++)
            delete square[i][j];
}

chessboard:: chessboard(chessboard& b){
    for(int i=0; i<8; i++)
            for(int j=0; j<8; j++){
                Piece* x=b.square[i][j];
                if(x!=nullptr){
                    if(x->label=='p')
                        square[i][j]=new Pawn({'a'+i, 1+j}, x->c);
                    else if(x->label=='R')
                        square[i][j]=new Rook({'a'+i, 1+j}, x->c);
                    else if(x->label=='B')
                        square[i][j]=new Bishop({'a'+i, 1+j}, x->c);
                    else if(x->label=='N')
                        square[i][j]=new Knight({'a'+i, 1+j}, x->c);
                    else if(x->label=='Q')
                        square[i][j]=new Queen({'a'+i, 1+j}, x->c);
                    else
                        square[i][j]=new King({'a'+i, 1+j}, x->c);
                }
                else
                    square[i][j]=nullptr;
            }
    white_player=b.white_player;
    black_player=b.black_player;
    to_play=b.to_play;

}

ostream& operator << (ostream& out, chessboard& b){
    for(unsigned i=8; i>=1; i--){   
        for(char j='a'; j<='h'; j++){
            char y;
            Piece* p=b.access({j, i});
            if(p!=nullptr)
                y=p->label;
            else
                y=' ';
            out << "|";
            out << y;
        }
        out << "|" << endl;
    }
    return out;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void move(pci initial_position, pci position, chessboard& B){
    Piece* tmp=B.access(position);
    Piece* x=B.access(initial_position);
    if(x->label=='p' && initial_position.first!=file && tmp==nullptr){
        Piece* y=B.access({file, initial_position.second});
        delete y;
        B.access({file, initial_position.second})=nullptr;
    }
    delete tmp;
    B.access(position)=B.access(initial_position);
    B.access(initial_position)=nullptr;
    x->position=position;
        B.returnPlayer(x->c).lastmove={initial_position, position};
        if(x->label=='K'){
            B.returnPlayer(x->c).king=position;
            B.returnPlayer(x->c).shortcastleright=false;
            B.returnPlayer(x->c).longcastleright=false;
        }
        else if(x->label=='R'){
            pci pos1={'h', 1};
            pci pos2={'a', 1};
            if(initial_position==pos1)
                B.returnPlayer(x->c).shortcastleright=false;
            else if(initial_position==pos2)
                B.returnPlayer(x->c).longcastleright=false;
        }
}

bool find_piece(pci position, chessboard& B, char label){
    bool g=false;
    pci initial_position;
    Color c=B.to_play;
    for(char i='a'; i<='h'; i++)
        for(int j=1; j<=8; j++){
            Piece* x=B.access({i, j});
            if(x!=nullptr && x->label==label && x->c==c){
                x->moveable_to(B);
                x->checkmoves(B);
                if(find(x->checked_moves.begin(), x->checked_moves.end(), position)!=x->checked_moves.end()){
                    initial_position={i, j};
                    if(!g)
                        g=true;
                    else
                        return false;
                }
                x->moves.clear(); 
                x->checked_moves.clear();
            }  
        }
    if(g)
        move(initial_position, position, B);
    return g;    
}

bool find_specific_col_piece(char col, pci position, chessboard& B, char label){
    bool g=false, e=false;
    pci initial_position;
    Color c=B.to_play;
    for(char i='a'; i<='h'; i++)
        for(int j=1; j<=8; j++){
            Piece* x=B.access({i, j});
            if(x!=nullptr && x->label==label && x->c==c){
                x->moveable_to(B);
                x->checkmoves(B);
                if(find(x->checked_moves.begin(), x->checked_moves.end(), position)!=x->checked_moves.end()){
                    if(x->file==col){    
                        if(label=='p'){
                            x->moves.clear(); 
                            x->checked_moves.clear();
                            move({i, j}, position, B);
                            return true;
                        }
                        else if(!e){
                            initial_position={i, j};
                            e=true;
                        }
                        else{
                            x->moves.clear(); 
                            x->checked_moves.clear();
                            return false;
                        }
                    }
                    else
                        g=true;

                }            
                x->moves.clear(); 
                x->checked_moves.clear();
            }  
        }
    if(e==true && g==true){
        move(initial_position, position, B);
        return true;
    }
    else 
        return false; 
}

bool find_specific_row_piece(int row, pci position, chessboard& B, char label){
    bool g=false, e=false;
    pci initial_position;
    Color c=B.to_play;
    for(char i='a'; i<='h'; i++)
        for(int j=1; j<=8; j++){
            Piece* x=B.access({i, j});
            if(x!=nullptr && x->label==label && x->c==c){
                x->moveable_to(B);
                x->checkmoves(B);
                if(find(x->checked_moves.begin(), x->checked_moves.end(), position)!=x->checked_moves.end()){
                    if(x->rank==row){
                        if(!e){
                            initial_position={i, j};
                            e=true;
                        }
                        else
                            return false;
                    }
                    else if(x->file==i)
                        g=true;
                }            
                x->moves.clear(); 
                x->checked_moves.clear();
            }  
        }
    if(g==true && e==true){
        move(initial_position, position, B);
        return true;
    }
    else 
        return false;
}

void promote(pci position, char label, Color c, chessboard& b){
    delete b.access(position);
    if(label=='R')
        b.access(position)=new Rook(position, c);
    else if(label=='B')
         b.access(position)=new Bishop(position, c);
    else if(label=='N')
        b.access(position)=new Knight(position, c);
    else
         b.access(position)=new Queen(position, c);
}

bool understand_move(string &s, chessboard &B){
    pci position;
    char col, label;
    Color c=B.to_play;
    int row, num;
    if(c==white)
        num=1;
    else
        num=8;
    if(s[0]>='a' && s[0]<='h'){     
        if(s[1]=='x'){
            col=s[0];
            file=s[2];
            rank=s[3]-'0';
            try{Piece* x=B.access(position);}catch(out_of_range){return false;}
            if((c==white && rank<8 || c==black && rank>1) && s.size()==4){
                if(find_specific_col_piece(col,position, B, 'p'))
                    return true;
            }
            else{
                label=s[5];
                if(s[4]=='=' && (label=='R' || label=='B' || label=='Q' || label=='N') && s.size()==6){    
                    if(find_specific_col_piece(col,position, B, 'p')){
                        promote(position, label, c, B);
                        return true;
                    }
                }
            }
        }
        else{
            file=s[0];
            rank=s[1]-'0'; 
            try{Piece* x=B.access(position);}catch(out_of_range){return false;}   
            if((c==white && rank<8 || c==black && rank>1) && s.size()==2){
                if(find_piece(position, B, 'p'))
                    return true;
            }
            else{
                label=s[3];
                if(s[2]=='=' && (label=='R' || label=='B' || label=='Q' || label=='N') && s.size()==4){
                    if(find_piece(position, B, 'p')){
                        promote(position, label, c, B);
                        return true;
                    }
                }
            }
        }
    }
    else if(s=="O-O"){
        Piece* x=B.access({'e', num});
        if(B.returnPlayer(c).shortcastleright==true && x->is_in_danger(B)==false)
            if(B.access({'f', num})==nullptr && B.access({'g', num})==nullptr)
            {
                chessboard B1(B);
                move({'e', num}, {'g', num}, B1);
                move({'h', num}, {'f', num}, B1);
                if(B1.access({'f', num})->is_in_danger(B1)==false && B1.access({'g', num})->is_in_danger(B1)==false){
                    move({'e', num}, {'g', num}, B);
                    move({'h', num}, {'f', num}, B);
                    return true;
                }
            }
    }
    else if(s=="O-O-O"){
        Piece* x=B.access({'e', num});
        if(B.returnPlayer(c).longcastleright==true && x->is_in_danger(B)==false)
            if(B.access({'d', num})==nullptr && B.access({'c', num})==nullptr && B.access({'b', num})==nullptr)
            {
                chessboard B1(B);
                move({'e', num}, {'c', num}, B1);
                move({'a', num}, {'d', num}, B1);
                if(B1.access({'c', num})->is_in_danger(B1)==false && B1.access({'d', num})->is_in_danger(B1)==false){
                    move({'e', num}, {'c', num}, B);
                    move({'a', num}, {'d', num}, B);
                    return true;
                }
            }
    }
    else if(s.size()>=3){
        label=s[0];
        if(label!='p'){
            file=s[s.size()-2];
            rank=s[s.size()-1]-'0';
            try{
                Piece* x=B.access(position);
                if(s.size()==3 && x==nullptr){
                    if(find_piece(position, B, label))
                        return true;
                }
                else if(s.size()==4 && s[1]=='x' && x!=nullptr){
                    if(find_piece(position, B, label))
                        return true;
                }
                else if(s[1]>='a' && s[1]<='h' && s.size()==4 && x==nullptr){
                    col=s[1];
                    if(find_specific_col_piece(col,position, B, label))
                        return true;
                }
                else if(s[1]>='a' && s[1]<='h' && s[2]=='x' && s.size()==5 && x!=nullptr){
                    col=s[1];
                    if(find_specific_col_piece(col,position, B, label))
                        return true;
                }
                else if(s[1]>='1' && s[1]<='8' && s.size()==4 && x==nullptr){
                    row=s[1]-'0';
                    if(find_specific_row_piece(row, position, B, label))
                        return true;
                }
                else if(s[1]>='1' && s[1]<='8' && s[2]=='x' && s.size()==5 && x!=nullptr){
                    row=s[1]-'0';
                    if(find_specific_row_piece(row ,position, B, label))
                        return true;
                }
            } catch(out_of_range){return false;}
        }
    }
    return false;
}

int check_state(chessboard& B){
    bool g=true;
    Color c=B.to_play;
    for(char i='a'; i<='h'; i++)
        for(int j=1; j<=8; j++){
            Piece* x=B.access({i, j});
            if(x!=nullptr && x->c==c){
                x->moveable_to(B);
                x->checkmoves(B);
                if(!x->checked_moves.empty())
                    g=false;
                x->moves.clear(); 
                x->checked_moves.clear();
            }
        }
        if(g)
        {   if(B.access(B.returnPlayer(c).king)->is_in_danger(B))
                return 1;
            else
                return -1;
        }
        else
            return 0;
}

void chessboard:: play(){
    string s, s1, s2;
    if(to_play==white){
        s1="White";
        s2="Black";
    }
    else{
        s1="Black";
        s2="White";
    }
    cout << "> ";
    cin >> s;
    if(s=="resign"){
        cout << s1 << " resigned, "<< s2 << " wins!" << endl;
        return;
    }
    if(understand_move(s, *this)){
        cout << *this << endl;
        if(to_play==white)
            to_play=black;
        else
            to_play=white;
        int x=check_state(*this);
        if(x==1){
            cout << "Checkmate, " << s1 << " wins!" << endl;
            return;
        }
        else if(x==-1){
            cout << "Stalemate, Draw!" << endl;
            return;
        }
    }
    else
        cout << "invalid move" << endl;
    play();
}

void chessboard:: setup(const string &s){
    char i='a';
    int j=8, spaces=0;
    for(int pl=0; pl<s.size(); pl++){
        char x=s[pl];
        if(x==' '){
            spaces++;
            continue;
        }
        if(spaces==0){
            if(isalpha(x)){
                if(x=='r')
                    access({i, j})=new Rook({i, j}, black);
                else if(x=='b')
                    access({i, j})=new Bishop({i, j}, black);
                else if(x=='q')
                    access({i, j})=new Queen({i, j}, black);
                else if(x=='n')
                    access({i, j})=new Knight({i, j}, black);
                else if(x=='p')
                    access({i, j})=new Pawn({i, j}, black);
                else if(x=='k'){
                    access({i, j})=new King({i, j}, black);
                    black_player.king={i, j};
                }
                else if(x=='R')
                    access({i, j})=new Rook({i, j}, white);
                else if(x=='B')
                    access({i, j})=new Bishop({i, j}, white);
                else if(x=='Q')
                    access({i, j})=new Queen({i, j}, white);
                else if(x=='N')
                    access({i, j})=new Knight({i, j}, white);
                else if(x=='P')
                    access({i, j})=new Pawn({i, j}, white);
                else if(x=='K'){
                    access({i, j})=new King({i, j}, white);
                    white_player.king={i, j};
                }
                i++;
            }
            else if(isdigit(x)) i=i+x-'0';
            else if(x=='/'){
                j--;
                i='a';
            }
        }  
        else if(spaces==1){
            if(x=='w')
                to_play=white;
            else
                to_play=black;
        }
        else if(spaces==2){
            if(x=='K')
                white_player.shortcastleright=true;
            else if(x=='Q')
                white_player.longcastleright=true;
            else if(x=='k')
                black_player.shortcastleright=true;
            else if(x=='q')
                black_player.longcastleright=true;
            }
        else{
            if(isalpha(x)){
                if(to_play==white)
                    black_player.lastmove.first.first=black_player.lastmove.second.first=x;
                else
                    white_player.lastmove.first.first=white_player.lastmove.second.first=x;
            }
            else if(isdigit(x)){
                if(to_play==white){
                    black_player.lastmove.first.second=x+1-'0';
                    black_player.lastmove.second.second=x-1-'0';
                }
                else{
                    white_player.lastmove.first.second=x-1-'0';
                    white_player.lastmove.second.second=x+1-'0';
                }
            }
        }
    }
    cout << *this;
}

int main(){
    chessboard B;
    B.setup();
    B.play(); 
}
